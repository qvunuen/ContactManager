#include "Contact.h"
#include <iomanip>

// Constructor: Khởi tạo giá trị ban đầu
Contact::Contact(std::string n, std::string p, std::string e) 
    : name(n), phone(p), email(e) {}

// Getters
std::string Contact::getName() const { return name; }
std::string Contact::getPhone() const { return phone; }
std::string Contact::getEmail() const { return email; }

// Setters
void Contact::setName(std::string n) { name = n; }
void Contact::setPhone(std::string p) { phone = p; }
void Contact::setEmail(std::string e) { email = e; }

// Hiển thị một dòng thông tin
void Contact::displayContact() const {
    std::cout << std::left << std::setw(20) << name 
              << std::setw(15) << phone 
              << std::setw(25) << email << std::endl;
}