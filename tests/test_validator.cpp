#include <boost/test/unit_test.hpp>
#include "Validator.h"

BOOST_AUTO_TEST_SUITE(ValidatorTests)

BOOST_AUTO_TEST_CASE(TrimRemovesLeadingAndTrailingWhitespaceOnly) {
    BOOST_CHECK_EQUAL(Validator::trim("  Quang Vu Nguyen   "), "Quang Vu Nguyen");
    BOOST_CHECK_EQUAL(Validator::trim("\t 0123 456 \r\n"), "0123 456");
    BOOST_CHECK_EQUAL(Validator::trim("   "), "");
    BOOST_CHECK_EQUAL(Validator::trim(""), "");
}

BOOST_AUTO_TEST_CASE(ValidNamesAccepted) {
    BOOST_CHECK(Validator::isValidName("Alice"));
    BOOST_CHECK(Validator::isValidName("Quang Vu Nguyen"));
    BOOST_CHECK(Validator::isValidName("Mary-Jane"));
    BOOST_CHECK(Validator::isValidName("O'Connor"));
}

BOOST_AUTO_TEST_CASE(InvalidNamesRejected) {
    BOOST_CHECK(!Validator::isValidName(""));
    BOOST_CHECK(!Validator::isValidName("   "));
    BOOST_CHECK(!Validator::isValidName(" Alice"));
    BOOST_CHECK(!Validator::isValidName("Alice "));
    BOOST_CHECK(!Validator::isValidName(" Alice Bob "));
    BOOST_CHECK(!Validator::isValidName("---"));
    BOOST_CHECK(!Validator::isValidName("'''"));
    BOOST_CHECK(!Validator::isValidName("Alice123"));
    BOOST_CHECK(!Validator::isValidName("Alice@Home"));
    BOOST_CHECK(!Validator::isValidName("Alice,Bob"));
    BOOST_CHECK(!Validator::isValidName("Nguyễn"));
}

BOOST_AUTO_TEST_CASE(ValidPhonesAccepted) {
    BOOST_CHECK(Validator::isValidPhone("113"));
    BOOST_CHECK(Validator::isValidPhone("015566131440"));
    BOOST_CHECK(Validator::isValidPhone("+84 912-345-678"));
    BOOST_CHECK(Validator::isValidPhone("(024) 3825 1234"));
}

BOOST_AUTO_TEST_CASE(InvalidPhonesRejected) {
    BOOST_CHECK(!Validator::isValidPhone(""));
    BOOST_CHECK(!Validator::isValidPhone("12"));
    BOOST_CHECK(!Validator::isValidPhone("   "));
    BOOST_CHECK(!Validator::isValidPhone(" 012345"));
    BOOST_CHECK(!Validator::isValidPhone("012345 "));
    BOOST_CHECK(!Validator::isValidPhone("---"));
    BOOST_CHECK(!Validator::isValidPhone("abc123"));
    BOOST_CHECK(!Validator::isValidPhone("0123+456"));
    BOOST_CHECK(!Validator::isValidPhone("0123,456"));
}

BOOST_AUTO_TEST_CASE(ValidEmailsAccepted) {
    BOOST_CHECK(Validator::isValidEmail("user@example.com"));
    BOOST_CHECK(Validator::isValidEmail("quangvunguyen012@gmail.com"));
    BOOST_CHECK(Validator::isValidEmail("first.last@sub.domain.co.uk"));
    BOOST_CHECK(Validator::isValidEmail("user+tag@example.org"));
}

BOOST_AUTO_TEST_CASE(InvalidEmailsRejected) {
    BOOST_CHECK(!Validator::isValidEmail(""));
    BOOST_CHECK(!Validator::isValidEmail("plainaddress"));
    BOOST_CHECK(!Validator::isValidEmail("@domain.com"));
    BOOST_CHECK(!Validator::isValidEmail("user@"));
    BOOST_CHECK(!Validator::isValidEmail("user@domain"));
    BOOST_CHECK(!Validator::isValidEmail("user@.com"));
    BOOST_CHECK(!Validator::isValidEmail("user@domain."));
    BOOST_CHECK(!Validator::isValidEmail("user@domain..com"));
    BOOST_CHECK(!Validator::isValidEmail(".user@domain.com"));
    BOOST_CHECK(!Validator::isValidEmail("user.@domain.com"));
    BOOST_CHECK(!Validator::isValidEmail("user..name@domain.com"));
    BOOST_CHECK(!Validator::isValidEmail("a@@b.com"));
    BOOST_CHECK(!Validator::isValidEmail("user name@domain.com"));
    BOOST_CHECK(!Validator::isValidEmail("a,b@domain.com"));
}

BOOST_AUTO_TEST_SUITE_END()
