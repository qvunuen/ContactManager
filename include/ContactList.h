#ifndef CONTACTLIST_H
#define CONTACTLIST_H

#include "Contact.h"
#include <vector>
#include <string>

class ContactList {
private:
    std::vector<Contact> contacts;
    std::string fileName;

public:
    ContactList(std::string file = "contacts.txt");

    // Các chức năng chính (CRUD)
    void addContact(const Contact& c);
    void showAllContacts() const;
    int searchContact(std::string keyword) const; // Trả về chỉ số (index) trong vector
    bool deleteContact(int index);
    bool updateContact(int index, const Contact& newContact);
    std::vector<int> searchContactsAdvanced(std::string keyword) const;
    void showContactAtIndex(int index) const;

    // Thao tác với File (Rất quan trọng cho đồ án)
    void saveToFile();
    void loadFromFile();
};

#endif