#include "Validator.h"
#include <cctype>
#include <cstddef>

namespace Validator {

std::string trim(const std::string& str) {
    const std::size_t first = str.find_first_not_of(" \t\r\n\v\f");
    if (first == std::string::npos) {
        return "";
    }
    const std::size_t last = str.find_last_not_of(" \t\r\n\v\f");
    return str.substr(first, last - first + 1);
}

bool isValidName(const std::string& name) {
    if (name.empty() || name.front() == ' ' || name.back() == ' ') {
        return false;
    }

    bool hasLetter = false;
    for (char c : name) {
        const auto uc = static_cast<unsigned char>(c);
        if (std::isalpha(uc)) {
            hasLetter = true;
        } else if (c != ' ' && c != '-' && c != '\'') {
            return false;
        }
    }
    return hasLetter;
}

bool isValidPhone(const std::string& phone) {
    if (phone.empty() || phone.front() == ' ' || phone.back() == ' ') {
        return false;
    }

    int digitCount = 0;
    for (std::size_t i = 0; i < phone.size(); ++i) {
        const char c = phone[i];
        const auto uc = static_cast<unsigned char>(c);
        if (std::isdigit(uc)) {
            ++digitCount;
        } else if (c == '+') {
            if (i != 0) {
                return false;
            }
        } else if (c != '-' && c != ' ' && c != '(' && c != ')') {
            return false;
        }
    }
    return digitCount >= 3;
}

bool isValidEmail(const std::string& email) {
    if (email.empty()) {
        return false;
    }

    for (char c : email) {
        if (std::isspace(static_cast<unsigned char>(c)) || c == ',') {
            return false;
        }
    }

    const std::size_t atPos = email.find('@');
    if (atPos == std::string::npos || atPos == 0) {
        return false;
    }
    if (email.find('@', atPos + 1) != std::string::npos) {
        return false;
    }

    const std::string local = email.substr(0, atPos);
    const std::string domain = email.substr(atPos + 1);

    if (local.front() == '.' || local.back() == '.' || local.find("..") != std::string::npos) {
        return false;
    }
    if (domain.empty() || domain.find('.') == std::string::npos ||
        domain.front() == '.' || domain.back() == '.' ||
        domain.find("..") != std::string::npos) {
        return false;
    }

    return true;
}

} // namespace Validator
