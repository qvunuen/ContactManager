#ifndef CONTACT_H
#define CONTACT_H

#include <iosfwd>
#include <string>

class Contact {
private:
    std::string name;
    std::string phone;
    std::string email;

public:
    explicit Contact(std::string name = "", std::string phone = "", std::string email = "");

    const std::string& getName() const;
    const std::string& getPhone() const;
    const std::string& getEmail() const;

    void setName(const std::string& name);
    void setPhone(const std::string& phone);
    void setEmail(const std::string& email);

    bool operator==(const Contact& other) const;
    bool operator!=(const Contact& other) const;
};

std::ostream& operator<<(std::ostream& os, const Contact& contact);

#endif
