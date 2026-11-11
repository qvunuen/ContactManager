#include <boost/test/unit_test.hpp>
#include <sstream>
#include "Contact.h"

BOOST_AUTO_TEST_SUITE(ContactTests)

BOOST_AUTO_TEST_CASE(DefaultConstructorInitializesEmptyFields) {
    Contact c;
    BOOST_CHECK_EQUAL(c.getName(), "");
    BOOST_CHECK_EQUAL(c.getPhone(), "");
    BOOST_CHECK_EQUAL(c.getEmail(), "");
}

BOOST_AUTO_TEST_CASE(ParameterizedConstructorSetsFields) {
    Contact c("Alice Nguyen", "0123456789", "alice@example.com");
    BOOST_CHECK_EQUAL(c.getName(), "Alice Nguyen");
    BOOST_CHECK_EQUAL(c.getPhone(), "0123456789");
    BOOST_CHECK_EQUAL(c.getEmail(), "alice@example.com");
}

BOOST_AUTO_TEST_CASE(SettersUpdateFields) {
    Contact c;
    c.setName("Bob Tran");
    c.setPhone("+84 912-345-678");
    c.setEmail("bob@domain.org");

    BOOST_CHECK_EQUAL(c.getName(), "Bob Tran");
    BOOST_CHECK_EQUAL(c.getPhone(), "+84 912-345-678");
    BOOST_CHECK_EQUAL(c.getEmail(), "bob@domain.org");
}

BOOST_AUTO_TEST_CASE(ConstructorAndSettersTrimLeadingAndTrailingSpaces) {
    Contact c("  Quang Vu Nguyen   ", " +84 912 345   ", "  vu@example.com  ");
    BOOST_CHECK_EQUAL(c.getName(), "Quang Vu Nguyen");
    BOOST_CHECK_EQUAL(c.getPhone(), "+84 912 345");
    BOOST_CHECK_EQUAL(c.getEmail(), "vu@example.com");

    c.setName("Alice Bob   ");
    c.setPhone("  0123 456  ");
    c.setEmail(" alice@test.com   ");
    BOOST_CHECK_EQUAL(c.getName(), "Alice Bob");
    BOOST_CHECK_EQUAL(c.getPhone(), "0123 456");
    BOOST_CHECK_EQUAL(c.getEmail(), "alice@test.com");
}

BOOST_AUTO_TEST_CASE(EqualityOperatorsCompareAllFields) {
    Contact a("Alice", "0123", "a@b.com");
    Contact b("Alice", "0123", "a@b.com");
    Contact diffName("Bob", "0123", "a@b.com");
    Contact diffPhone("Alice", "9999", "a@b.com");
    Contact diffEmail("Alice", "0123", "x@y.com");

    BOOST_CHECK(a == b);
    BOOST_CHECK(!(a != b));
    BOOST_CHECK(a != diffName);
    BOOST_CHECK(a != diffPhone);
    BOOST_CHECK(a != diffEmail);
}

BOOST_AUTO_TEST_CASE(StreamOperatorFormatsContactDetails) {
    Contact c("Alice", "012345", "alice@test.com");
    std::ostringstream oss;
    oss << c;

    const std::string output = oss.str();
    BOOST_CHECK(output.find("Alice") != std::string::npos);
    BOOST_CHECK(output.find("012345") != std::string::npos);
    BOOST_CHECK(output.find("alice@test.com") != std::string::npos);
}

BOOST_AUTO_TEST_SUITE_END()
