#include "ContactList.h"
#include <fstream>
#include <iostream>

ContactList::ContactList(std::string file) : fileName(file) {
    loadFromFile(); // Tự động load dữ liệu khi tạo đối tượng
}

void ContactList::addContact(const Contact& c) {
    contacts.push_back(c);
}

void ContactList::showAllContacts() const {
    if (contacts.empty()) {
        std::cout << "PhoneBook is empty!\n";
        return;
    }
    std::cout << "------------------------------------------------------------\n";
    for (const auto& c : contacts) {
        c.displayContact();
    }
    std::cout << "------------------------------------------------------------\n";
}

int ContactList::searchContact(std::string keyword) const {
    for (int i = 0; i < contacts.size(); i++) {
        if (contacts[i].getName() == keyword || contacts[i].getPhone() == keyword) {
            return i; // Trả về vị trí nếu tìm thấy
        }
    }
    return -1; // Không tìm thấy
}

bool ContactList::deleteContact(int index) {
    if (index >= 0 && index < contacts.size()) {
        contacts.erase(contacts.begin() + index);
        return true;
    }
    return false;
}

void ContactList::saveToFile() {
    std::ofstream outFile(fileName);
    if (!outFile.is_open()) {
        std::cerr << "Error: Cannot open file: " << fileName << std::endl;
        return;
    } else {
        for (const auto& c : contacts) {
            // Lưu mỗi thuộc tính cách nhau bằng dấu phẩy để dễ đọc lại
            outFile << c.getName() << "," << c.getPhone() << "," << c.getEmail() << "\n";
        }
        std::cout << "Data saved. Good Bye!";
    }
    outFile.close();
}

void ContactList::loadFromFile() {
    std::ifstream inFile(fileName);
    if (!inFile) return;

    std::string n, p, e;
    // Đọc từng dòng, tách dữ liệu bằng dấu phẩy (Cơ chế đơn giản)
    while (std::getline(inFile, n, ',') && 
           std::getline(inFile, p, ',') && 
           std::getline(inFile, e)) {
        contacts.push_back(Contact(n, p, e));
    }
    inFile.close();
}

bool ContactList::updateContact(int index, const Contact& newContact) {
    // Kiểm tra xem chỉ số index có hợp lệ không (tránh crash chương trình)
    if (index >= 0 && index < contacts.size()) {
        contacts[index] = newContact; // Ghi đè đối tượng cũ bằng đối tượng mới
        return true;
    }
    return false; // Trả về false nếu index nằm ngoài phạm vi
}

std::vector<int> ContactList::searchContactsAdvanced(std::string keyword) const {
    std::vector<int> matchedIndices;
    
    for (int i = 0; i < contacts.size(); i++) {
        // Hàm find() dùng để tìm chuỗi con. Nếu tìm thấy, nó trả về một giá trị khác std::string::npos
        bool matchName = contacts[i].getName().find(keyword) != std::string::npos;
        bool matchPhone = contacts[i].getPhone().find(keyword) != std::string::npos;
        
        if (matchName || matchPhone) {
            matchedIndices.push_back(i); // Lưu lại vị trí (index) nếu khớp
        }
    }
    return matchedIndices;
}

void ContactList::showContactAtIndex(int index) const {
    if (index >= 0 && index < contacts.size()) {
        contacts[index].displayContact();
    }
}