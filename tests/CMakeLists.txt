if(POLICY CMP0167)
    cmake_policy(SET CMP0167 NEW)
endif()

find_package(Boost REQUIRED COMPONENTS unit_test_framework)

add_executable(ContactManagerTests
    test_main.cpp
    test_contact.cpp
    test_validator.cpp
    test_contact_list.cpp
)

target_link_libraries(ContactManagerTests
    PRIVATE
        ContactManagerLib
        Boost::unit_test_framework
)

get_target_property(_boost_utf_type Boost::unit_test_framework TYPE)
if(_boost_utf_type STREQUAL "SHARED_LIBRARY")
    get_target_property(_boost_utf_defs Boost::unit_test_framework INTERFACE_COMPILE_DEFINITIONS)
    if(NOT _boost_utf_defs OR NOT ("${_boost_utf_defs}" MATCHES "BOOST_.*DYN_LINK"))
        target_compile_definitions(ContactManagerTests PRIVATE BOOST_TEST_DYN_LINK)
    endif()
endif()

apply_project_warnings(ContactManagerTests)

add_test(NAME ContactManagerTests COMMAND ContactManagerTests)
