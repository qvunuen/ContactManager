#include "Contact.h"
#include "Validator.h"
#include <ostream>

Contact::Contact(std::string name, std::string phone, std::string email)
    : name(Validator::trim(name)),
      phone(Validator::trim(phone)),
      email(Validator::trim(email)) {}

const std::string& Contact::getName() const {
    return name;
}

const std::string& Contact::getPhone() const {
    return phone;
}

const std::string& Contact::getEmail() const {
    return email;
}

void Contact::setName(const std::string& newName) {
    name = Validator::trim(newName);
}

void Contact::setPhone(const std::string& newPhone) {
    phone = Validator::trim(newPhone);
}

void Contact::setEmail(const std::string& newEmail) {
    email = Validator::trim(newEmail);
}

bool Contact::operator==(const Contact& other) const {
    return name == other.name && phone == other.phone && email == other.email;
}

bool Contact::operator!=(const Contact& other) const {
    return !(*this == other);
}

std::ostream& operator<<(std::ostream& os, const Contact& contact) {
    os << "Contact{name=\"" << contact.getName()
       << "\", phone=\"" << contact.getPhone()
       << "\", email=\"" << contact.getEmail() << "\"}";
    return os;
}
