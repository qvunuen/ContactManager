cmake_minimum_required(VERSION 3.16)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE AND DEFINED ENV{VCPKG_ROOT} AND EXISTS "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
endif()

project(ContactManager VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

function(apply_project_warnings target_name)
    if(MSVC)
        target_compile_options(${target_name} PRIVATE /W4 /utf-8)
    else()
        target_compile_options(${target_name} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endfunction()

option(USE_SYSTEM_SQLITE3 "Use pre-installed system SQLite3 if available" ON)
if(USE_SYSTEM_SQLITE3)
    find_package(SQLite3 QUIET)
endif()

if(NOT SQLite3_FOUND)
    message(STATUS "System SQLite3 not found — fetching SQLite 3.47.2 amalgamation via FetchContent")
    if(POLICY CMP0135)
        cmake_policy(SET CMP0135 NEW)
    endif()
    include(FetchContent)
    FetchContent_Declare(
        sqlite3_amalgamation
        URL https://www.sqlite.org/2024/sqlite-amalgamation-3470200.zip
        URL_HASH SHA256=aa73d8748095808471deaa8e6f34aa700e37f2f787f4425744f53fdd15a89c40
    )
    FetchContent_MakeAvailable(sqlite3_amalgamation)

    add_library(sqlite3_static STATIC
        ${sqlite3_amalgamation_SOURCE_DIR}/sqlite3.c
    )
    target_include_directories(sqlite3_static
        PUBLIC
            ${sqlite3_amalgamation_SOURCE_DIR}
    )
    if(MSVC)
        target_compile_options(sqlite3_static PRIVATE /W0)
    else()
        target_compile_options(sqlite3_static PRIVATE -w)
    endif()

    find_package(Threads REQUIRED)
    target_link_libraries(sqlite3_static
        PUBLIC
            Threads::Threads
            ${CMAKE_DL_LIBS}
    )
    add_library(SQLite::SQLite3 ALIAS sqlite3_static)
endif()

option(BUILD_GUI "Build Qt6 GUI application" ON)

add_library(ContactManagerLib STATIC
    src/Contact.cpp
    src/ContactList.cpp
    src/Validator.cpp
)

target_include_directories(ContactManagerLib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(ContactManagerLib
    PRIVATE
        SQLite::SQLite3
)

apply_project_warnings(ContactManagerLib)

if(BUILD_GUI)
    find_package(Qt6 REQUIRED COMPONENTS Widgets)

    add_executable(ContactManager
        include/ContactDialog.h
        include/MainWindow.h
        src/ContactDialog.cpp
        src/MainWindow.cpp
        src/main.cpp
    )

    target_link_libraries(ContactManager
        PRIVATE
            ContactManagerLib
            Qt6::Widgets
    )

    apply_project_warnings(ContactManager)

    set_target_properties(ContactManager PROPERTIES
        AUTOMOC ON
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
    )

    if(WIN32)
        set_target_properties(ContactManager PROPERTIES
            WIN32_EXECUTABLE TRUE
        )
        option(DEPLOY_QT_RUNTIME "Deploy Qt runtime dependencies after build" ON)

        if(DEPLOY_QT_RUNTIME)
            find_program(WINDEPLOYQT_EXECUTABLE windeployqt
                HINTS
                    "${Qt6_DIR}/../../../bin"
                    "${Qt6_DIR}/../../../tools/Qt6/bin"
                    "${Qt6_DIR}/../../../tools/qtbase/bin"
            )
            if(WINDEPLOYQT_EXECUTABLE)
                add_custom_command(TARGET ContactManager POST_BUILD
                    COMMAND "${WINDEPLOYQT_EXECUTABLE}" --no-translations "$<TARGET_FILE:ContactManager>"
                    COMMENT "Deploying Qt6 runtime dependencies for Windows"
                )
            endif()
        endif()
    endif()
endif()

include(CTest)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
