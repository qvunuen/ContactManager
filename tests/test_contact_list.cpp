#include <atomic>
#include <boost/test/unit_test.hpp>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <system_error>
#include "ContactList.h"

namespace {

struct TempFileFixture {
    std::filesystem::path tempDir;
    std::filesystem::path filePath;
    std::filesystem::path missingDirFilePath;

    TempFileFixture() {
        static std::atomic<unsigned long long> counter{0};
        const auto stamp = std::chrono::steady_clock::now().time_since_epoch().count();
        const auto id = counter.fetch_add(1);
        tempDir = std::filesystem::temp_directory_path() /
                  ("contact_manager_test_" + std::to_string(stamp) + "_" + std::to_string(id));
        std::filesystem::create_directories(tempDir);
        filePath = tempDir / "contacts.db";
        missingDirFilePath = tempDir / "does_not_exist" / "contacts.db";
    }

    std::string dbPathUtf8() const { return filePath.u8string(); }
    std::string missingDbPathUtf8() const { return missingDirFilePath.u8string(); }

    ~TempFileFixture() {
        std::error_code ec;
        std::filesystem::remove_all(tempDir, ec);
    }
};

} // namespace

BOOST_FIXTURE_TEST_SUITE(ContactListTests, TempFileFixture)

BOOST_AUTO_TEST_CASE(ConstructsEmptyWhenFileDoesNotExist) {
    ContactList list(dbPathUtf8());
    BOOST_CHECK(list.empty());
    BOOST_CHECK_EQUAL(list.size(), 0u);
    BOOST_CHECK_EQUAL(list.getFileName(), dbPathUtf8());
}

BOOST_AUTO_TEST_CASE(AddAndGetContacts) {
    ContactList list(dbPathUtf8());
    const Contact alice("Alice", "012345", "alice@test.com");
    const Contact bob("Bob", "678910", "bob@test.com");

    BOOST_CHECK(list.addContact(alice));
    BOOST_CHECK(list.addContact(bob));

    BOOST_CHECK(!list.empty());
    BOOST_CHECK_EQUAL(list.size(), 2u);
    BOOST_CHECK_EQUAL(list.getContact(0), alice);
    BOOST_CHECK_EQUAL(list.getContact(1), bob);
    BOOST_CHECK_THROW(list.getContact(2), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(AddContactRejectsExactDuplicateOnly) {
    ContactList list(dbPathUtf8());
    const Contact alice("Alice", "012345", "alice@test.com");

    BOOST_CHECK(list.addContact(alice));
    BOOST_CHECK(list.hasContact(alice));

    // Exact match across all three fields must be rejected (even with trailing spaces)
    BOOST_CHECK(!list.addContact(Contact("Alice", "012345", "alice@test.com")));
    BOOST_CHECK(!list.addContact(Contact("Alice   ", "012345  ", "alice@test.com   ")));
    BOOST_CHECK_EQUAL(list.size(), 1u);

    // Differing in at least one field is allowed
    BOOST_CHECK(list.addContact(Contact("Alice", "012345", "alice2@test.com")));
    BOOST_CHECK(list.addContact(Contact("Alice", "999999", "alice@test.com")));
    BOOST_CHECK(list.addContact(Contact("Alice B", "012345", "alice@test.com")));
    BOOST_CHECK_EQUAL(list.size(), 4u);

    BOOST_CHECK(!list.hasContact(Contact("Alice", "012345", "other@test.com")));
}

BOOST_AUTO_TEST_CASE(UpdateContactValidAndOutOfRange) {
    ContactList list(dbPathUtf8());
    list.addContact(Contact("Alice", "012345", "alice@test.com"));

    const Contact updated("Alice Updated", "999999", "new@test.com");
    BOOST_CHECK(list.updateContact(0, updated));
    BOOST_CHECK_EQUAL(list.getContact(0), updated);

    BOOST_CHECK(!list.updateContact(1, Contact("Ghost", "000", "g@t.com")));
    BOOST_CHECK_EQUAL(list.size(), 1u);
}

BOOST_AUTO_TEST_CASE(UpdateContactRejectsDuplicateOfOtherEntryButAllowsSelfUpdate) {
    ContactList list(dbPathUtf8());
    const Contact alice("Alice", "012345", "alice@test.com");
    const Contact bob("Bob", "678910", "bob@test.com");
    BOOST_REQUIRE(list.addContact(alice));
    BOOST_REQUIRE(list.addContact(bob));

    // Updating Bob to match Alice must fail and keep Bob intact
    BOOST_CHECK(list.hasContact(alice, 1));
    BOOST_CHECK(!list.hasContact(alice, 0));
    BOOST_CHECK(!list.updateContact(1, alice));
    BOOST_CHECK_EQUAL(list.getContact(1), bob);

    // Updating Alice with her own unchanged data must succeed
    BOOST_CHECK(list.updateContact(0, alice));
    BOOST_CHECK_EQUAL(list.getContact(0), alice);

    // Updating Bob to differ in only one field from Alice must succeed
    const Contact partialMatch("Bob", "012345", "alice@test.com");
    BOOST_CHECK(list.updateContact(1, partialMatch));
    BOOST_CHECK_EQUAL(list.getContact(1), partialMatch);
}

BOOST_AUTO_TEST_CASE(DeleteContactShiftsIndicesAndValidatesBounds) {
    ContactList list(dbPathUtf8());
    const Contact first("First", "111", "f@t.com");
    const Contact second("Second", "222", "s@t.com");
    const Contact third("Third", "333", "t@t.com");
    list.addContact(first);
    list.addContact(second);
    list.addContact(third);

    BOOST_CHECK(!list.deleteContact(3));
    BOOST_CHECK(list.deleteContact(1));
    BOOST_CHECK_EQUAL(list.size(), 2u);
    BOOST_CHECK_EQUAL(list.getContact(0), first);
    BOOST_CHECK_EQUAL(list.getContact(1), third);
}

BOOST_AUTO_TEST_CASE(SearchContactMatchesExactNameOrPhone) {
    ContactList list(dbPathUtf8());
    list.addContact(Contact("Quang Vu Nguyen", "015566131440", "vu@test.com"));
    list.addContact(Contact("Alice Tran", "0987654321", "alice@test.com"));

    const auto byName = list.searchContact("Alice Tran");
    BOOST_REQUIRE(byName.has_value());
    BOOST_CHECK_EQUAL(*byName, 1u);

    const auto byPhone = list.searchContact("015566131440");
    BOOST_REQUIRE(byPhone.has_value());
    BOOST_CHECK_EQUAL(*byPhone, 0u);

    BOOST_CHECK(!list.searchContact("Alice").has_value());
    BOOST_CHECK(!list.searchContact("Unknown").has_value());
}

BOOST_AUTO_TEST_CASE(SearchContactsAdvancedIsCaseInsensitiveSubstringMatch) {
    ContactList list(dbPathUtf8());
    list.addContact(Contact("Quang Vu Nguyen", "015566131440", "vu@test.com"));
    list.addContact(Contact("Minh Nguyen", "0900112233", "minh@test.com"));
    list.addContact(Contact("Bob Smith", "0911661399", "bob@test.com"));

    const auto matchNguyen = list.searchContactsAdvanced("nguyen");
    BOOST_REQUIRE_EQUAL(matchNguyen.size(), 2u);
    BOOST_CHECK_EQUAL(matchNguyen[0], 0u);
    BOOST_CHECK_EQUAL(matchNguyen[1], 1u);

    const auto matchPhone = list.searchContactsAdvanced("6613");
    BOOST_REQUIRE_EQUAL(matchPhone.size(), 2u);
    BOOST_CHECK_EQUAL(matchPhone[0], 0u);
    BOOST_CHECK_EQUAL(matchPhone[1], 2u);

    const auto matchUpper = list.searchContactsAdvanced("MINH");
    BOOST_REQUIRE_EQUAL(matchUpper.size(), 1u);
    BOOST_CHECK_EQUAL(matchUpper[0], 1u);

    BOOST_CHECK(list.searchContactsAdvanced("").empty());
    BOOST_CHECK(list.searchContactsAdvanced("nonexistent").empty());
}

BOOST_AUTO_TEST_CASE(SaveAndLoadRoundtripPersistsContactsInSqliteWithoutDuplication) {
    const Contact c1("Alice", "012345", "alice@test.com");
    const Contact c2("Bob", "678910", "bob@test.com");

    {
        ContactList writer(dbPathUtf8());
        writer.addContact(c1);
        writer.addContact(c2);
        BOOST_CHECK(writer.saveToFile());
    }

    // Verify the persisted file is a real SQLite 3 database
    {
        std::ifstream rawDb(filePath, std::ios::binary);
        std::string header(16, '\0');
        rawDb.read(&header[0], 16);
        BOOST_CHECK_EQUAL(header.substr(0, 15), "SQLite format 3");
    }

    ContactList reader(dbPathUtf8());
    BOOST_REQUIRE_EQUAL(reader.size(), 2u);
    BOOST_CHECK_EQUAL(reader.getContact(0), c1);
    BOOST_CHECK_EQUAL(reader.getContact(1), c2);

    // Calling loadFromFile again should reload cleanly rather than duplicating entries
    BOOST_CHECK(reader.loadFromFile());
    BOOST_REQUIRE_EQUAL(reader.size(), 2u);
    BOOST_CHECK_EQUAL(reader.getContact(0), c1);
    BOOST_CHECK_EQUAL(reader.getContact(1), c2);

    // Deleting and saving must update SQLite rows accurately
    BOOST_REQUIRE(reader.deleteContact(0));
    BOOST_REQUIRE(reader.saveToFile());

    ContactList afterDelete(dbPathUtf8());
    BOOST_REQUIRE_EQUAL(afterDelete.size(), 1u);
    BOOST_CHECK_EQUAL(afterDelete.getContact(0), c2);
}

BOOST_AUTO_TEST_CASE(SaveToFileWritesTrimmedFieldsWithoutTrailingSpaces) {
    {
        ContactList writer(dbPathUtf8());
        BOOST_REQUIRE(writer.addContact(Contact("  Quang Vu Nguyen   ", " 0155 6613 1440  ", "  vu@test.com   ")));
        BOOST_REQUIRE(writer.saveToFile());
    }

    ContactList reader(dbPathUtf8());
    BOOST_REQUIRE_EQUAL(reader.size(), 1u);
    BOOST_CHECK_EQUAL(reader.getContact(0).getName(), "Quang Vu Nguyen");
    BOOST_CHECK_EQUAL(reader.getContact(0).getPhone(), "0155 6613 1440");
    BOOST_CHECK_EQUAL(reader.getContact(0).getEmail(), "vu@test.com");
}

BOOST_AUTO_TEST_CASE(LoadFromFileReturnsFalseForInvalidSqliteFile) {
    {
        std::ofstream garbage(filePath, std::ios::binary);
        garbage << "not a sqlite database";
    }

    ContactList list(dbPathUtf8());
    BOOST_CHECK(list.empty());
    BOOST_CHECK(!list.loadFromFile());
}

BOOST_AUTO_TEST_CASE(SaveAndLoadReturnFalseOnInvalidPaths) {
    ContactList list(missingDbPathUtf8());
    BOOST_CHECK(!list.loadFromFile());

    list.addContact(Contact("Alice", "012345", "alice@test.com"));
    BOOST_CHECK(!list.saveToFile());
}

BOOST_AUTO_TEST_CASE(SupportsUtf8DatabaseDirectoryAndFileName) {
    const std::filesystem::path utf8Dir = tempDir / std::filesystem::u8path("danh_bạ_việt");
    std::filesystem::create_directories(utf8Dir);
    const std::string utf8Db = (utf8Dir / std::filesystem::u8path("liên_hệ.db")).u8string();

    const Contact c("Quang Vu Nguyen", "015566131440", "vu@test.com");
    {
        ContactList writer(utf8Db);
        BOOST_REQUIRE(writer.addContact(c));
        BOOST_REQUIRE(writer.saveToFile());
    }

    ContactList reader(utf8Db);
    BOOST_REQUIRE_EQUAL(reader.size(), 1u);
    BOOST_CHECK_EQUAL(reader.getContact(0), c);
}

BOOST_AUTO_TEST_SUITE_END()
