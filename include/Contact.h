#ifndef CONTACT_H
#define CONTACT_H

#include <string>
#include <iostream>

class Contact {
private:
    std::string name;
    std::string phone;
    std::string email;

public:
    // Constructor (Hàm khởi tạo)
    Contact(std::string n = "", std::string p = "", std::string e = "");

    // Getters (Lấy dữ liệu)
    std::string getName() const;
    std::string getPhone() const;
    std::string getEmail() const;

    // Setters (Cập nhật dữ liệu - Dùng cho chức năng Edit)
    void setName(std::string n);
    void setPhone(std::string p);
    void setEmail(std::string e);

    // Phương thức bổ trợ
    void displayContact() const; 
};

#endif