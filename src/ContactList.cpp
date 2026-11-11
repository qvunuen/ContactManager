#include "ContactList.h"
#include "Validator.h"
#include <cctype>
#include <cstddef>
#include <filesystem>
#include <sqlite3.h>
#include <stdexcept>
#include <system_error>
#include <utility>

namespace {

struct SqliteDbGuard {
    sqlite3* db = nullptr;
    ~SqliteDbGuard() {
        if (db != nullptr) {
            sqlite3_close(db);
        }
    }
};

struct SqliteStmtGuard {
    sqlite3_stmt* stmt = nullptr;
    ~SqliteStmtGuard() {
        if (stmt != nullptr) {
            sqlite3_finalize(stmt);
        }
    }
};

bool execSql(sqlite3* db, const char* sql) {
    return sqlite3_exec(db, sql, nullptr, nullptr, nullptr) == SQLITE_OK;
}

std::string readColumnText(sqlite3_stmt* stmt, int col) {
    const unsigned char* text = sqlite3_column_text(stmt, col);
    if (text == nullptr) {
        return "";
    }
    const int bytes = sqlite3_column_bytes(stmt, col);
    if (bytes <= 0) {
        return "";
    }
    return std::string(reinterpret_cast<const char*>(text), static_cast<std::size_t>(bytes));
}

std::string toLower(const std::string& str) {
    std::string result = str;
    for (char& c : result) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    return result;
}

} // namespace

ContactList::ContactList(std::string file) : fileName(std::move(file)) {
    loadFromFile();
}

std::size_t ContactList::size() const {
    return contacts.size();
}

bool ContactList::empty() const {
    return contacts.empty();
}

const Contact& ContactList::getContact(std::size_t index) const {
    if (index >= contacts.size()) {
        throw std::out_of_range("Contact index out of range");
    }
    return contacts[index];
}

const std::vector<Contact>& ContactList::getContacts() const {
    return contacts;
}

const std::string& ContactList::getFileName() const {
    return fileName;
}

bool ContactList::containsDuplicate(
    const Contact& contact,
    std::optional<std::size_t> excludeIndex
) const {
    for (std::size_t i = 0; i < contacts.size(); ++i) {
        if (excludeIndex.has_value() && i == *excludeIndex) {
            continue;
        }
        if (contacts[i] == contact) {
            return true;
        }
    }
    return false;
}

bool ContactList::hasContact(
    const Contact& contact,
    std::optional<std::size_t> excludeIndex
) const {
    return containsDuplicate(contact, excludeIndex);
}

bool ContactList::addContact(const Contact& contact) {
    if (containsDuplicate(contact)) {
        return false;
    }
    contacts.push_back(contact);
    return true;
}

bool ContactList::updateContact(std::size_t index, const Contact& newContact) {
    if (index >= contacts.size()) {
        return false;
    }
    if (containsDuplicate(newContact, index)) {
        return false;
    }
    contacts[index] = newContact;
    return true;
}

bool ContactList::deleteContact(std::size_t index) {
    if (index >= contacts.size()) {
        return false;
    }
    contacts.erase(contacts.begin() + static_cast<std::ptrdiff_t>(index));
    return true;
}

void ContactList::clear() {
    contacts.clear();
}

std::optional<std::size_t> ContactList::searchContact(const std::string& keyword) const {
    for (std::size_t i = 0; i < contacts.size(); ++i) {
        if (contacts[i].getName() == keyword || contacts[i].getPhone() == keyword) {
            return i;
        }
    }
    return std::nullopt;
}

std::vector<std::size_t> ContactList::searchContactsAdvanced(const std::string& keyword) const {
    std::vector<std::size_t> matchedIndices;
    if (keyword.empty()) {
        return matchedIndices;
    }

    const std::string lowerKeyword = toLower(keyword);
    for (std::size_t i = 0; i < contacts.size(); ++i) {
        const bool matchName = toLower(contacts[i].getName()).find(lowerKeyword) != std::string::npos;
        const bool matchPhone = toLower(contacts[i].getPhone()).find(lowerKeyword) != std::string::npos;
        if (matchName || matchPhone) {
            matchedIndices.push_back(i);
        }
    }
    return matchedIndices;
}

bool ContactList::saveToFile() const {
    SqliteDbGuard dbGuard;
    if (sqlite3_open_v2(
            fileName.c_str(),
            &dbGuard.db,
            SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
            nullptr
        ) != SQLITE_OK) {
        return false;
    }

    sqlite3_busy_timeout(dbGuard.db, 2000);

    const char* createTableSql =
        "CREATE TABLE IF NOT EXISTS contacts ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL,"
        "phone TEXT NOT NULL,"
        "email TEXT NOT NULL,"
        "UNIQUE(name, phone, email)"
        ");";

    if (!execSql(dbGuard.db, createTableSql)) {
        return false;
    }

    if (!execSql(dbGuard.db, "BEGIN IMMEDIATE;")) {
        return false;
    }

    if (!execSql(dbGuard.db, "DELETE FROM contacts;")) {
        execSql(dbGuard.db, "ROLLBACK;");
        return false;
    }

    {
        SqliteStmtGuard stmtGuard;
        const char* insertSql = "INSERT INTO contacts (name, phone, email) VALUES (?, ?, ?);";
        if (sqlite3_prepare_v2(dbGuard.db, insertSql, -1, &stmtGuard.stmt, nullptr) != SQLITE_OK) {
            execSql(dbGuard.db, "ROLLBACK;");
            return false;
        }

        for (const auto& c : contacts) {
            sqlite3_reset(stmtGuard.stmt);
            sqlite3_clear_bindings(stmtGuard.stmt);

            const std::string& n = c.getName();
            const std::string& p = c.getPhone();
            const std::string& e = c.getEmail();

            if (sqlite3_bind_text(stmtGuard.stmt, 1, n.c_str(), static_cast<int>(n.size()), SQLITE_TRANSIENT) != SQLITE_OK ||
                sqlite3_bind_text(stmtGuard.stmt, 2, p.c_str(), static_cast<int>(p.size()), SQLITE_TRANSIENT) != SQLITE_OK ||
                sqlite3_bind_text(stmtGuard.stmt, 3, e.c_str(), static_cast<int>(e.size()), SQLITE_TRANSIENT) != SQLITE_OK) {
                execSql(dbGuard.db, "ROLLBACK;");
                return false;
            }

            if (sqlite3_step(stmtGuard.stmt) != SQLITE_DONE) {
                execSql(dbGuard.db, "ROLLBACK;");
                return false;
            }
        }
    }

    if (!execSql(dbGuard.db, "COMMIT;")) {
        execSql(dbGuard.db, "ROLLBACK;");
        return false;
    }

    return true;
}

bool ContactList::loadFromFile() {
    std::error_code ec;
    const std::filesystem::path dbPath = std::filesystem::u8path(fileName);
    if (!std::filesystem::exists(dbPath, ec) || std::filesystem::is_directory(dbPath, ec)) {
        return false;
    }

    SqliteDbGuard dbGuard;
    if (sqlite3_open_v2(fileName.c_str(), &dbGuard.db, SQLITE_OPEN_READONLY, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_busy_timeout(dbGuard.db, 2000);

    SqliteStmtGuard stmtGuard;
    const char* selectSql = "SELECT name, phone, email FROM contacts ORDER BY id ASC;";
    if (sqlite3_prepare_v2(dbGuard.db, selectSql, -1, &stmtGuard.stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    std::vector<Contact> loaded;
    int rc = SQLITE_OK;
    while ((rc = sqlite3_step(stmtGuard.stmt)) == SQLITE_ROW) {
        const std::string n = readColumnText(stmtGuard.stmt, 0);
        const std::string p = readColumnText(stmtGuard.stmt, 1);
        const std::string e = readColumnText(stmtGuard.stmt, 2);
        const Contact entry(n, p, e);

        bool duplicate = false;
        for (const auto& existing : loaded) {
            if (existing == entry) {
                duplicate = true;
                break;
            }
        }
        if (!duplicate) {
            loaded.push_back(entry);
        }
    }

    if (rc != SQLITE_DONE) {
        return false;
    }

    contacts = std::move(loaded);
    return true;
}
