#ifndef VALIDATOR_H
#define VALIDATOR_H

#include <string>

namespace Validator {

/** Removes leading and trailing whitespace characters from a string. */
std::string trim(const std::string& str);

/** Checks that a contact name has no leading/trailing spaces, contains only letters, internal spaces, hyphens, or apostrophes, and has at least one letter. */
bool isValidName(const std::string& name);

/** Checks that a phone number has no leading/trailing spaces, contains valid characters (optional leading '+', digits, internal spaces, '-', '(', ')') and at least 3 digits. */
bool isValidPhone(const std::string& phone);

/** Checks that an email address follows basic local@domain.tld structure without spaces or CSV commas. */
bool isValidEmail(const std::string& email);

} // namespace Validator

#endif
