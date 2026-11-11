#ifndef CONTACTLIST_H
#define CONTACTLIST_H

#include "Contact.h"
#include <cstddef>
#include <optional>
#include <string>
#include <vector>

class ContactList {
private:
    std::vector<Contact> contacts;
    std::string fileName;

    bool containsDuplicate(
        const Contact& contact,
        std::optional<std::size_t> excludeIndex = std::nullopt
    ) const;

public:
    explicit ContactList(std::string file = "contacts.db");

    // Accessors
    std::size_t size() const;
    bool empty() const;
    const Contact& getContact(std::size_t index) const;
    const std::vector<Contact>& getContacts() const;
    const std::string& getFileName() const;
    bool hasContact(
        const Contact& contact,
        std::optional<std::size_t> excludeIndex = std::nullopt
    ) const;

    // Core features (CRUD & Search)
    /** Adds contact if no identical contact exists; returns false on duplicate. */
    bool addContact(const Contact& contact);
    /** Updates contact at index unless index is invalid or newContact matches another contact. */
    bool updateContact(std::size_t index, const Contact& newContact);
    bool deleteContact(std::size_t index);
    void clear();

    /** Exact match search on name or phone; returns first matching index or std::nullopt. */
    std::optional<std::size_t> searchContact(const std::string& keyword) const;

    /** Case-insensitive substring search on name or phone; returns all matching indices. */
    std::vector<std::size_t> searchContactsAdvanced(const std::string& keyword) const;

    // SQLite persistence
    bool saveToFile() const;
    bool loadFromFile();
};

#endif
