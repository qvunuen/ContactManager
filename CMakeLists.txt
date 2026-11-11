cmake_minimum_required(VERSION 3.16)

project(ContactManager VERSION 1.0 LANGUAGES CXX)

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

find_package(SQLite3 QUIET)
if(NOT SQLite3_FOUND)
    message(FATAL_ERROR
        "SQLite3 development library not found. "
        "Install it via: "
        "Linux (apt): sudo apt install libsqlite3-dev | "
        "Linux (dnf): sudo dnf install sqlite-devel | "
        "Windows (vcpkg): vcpkg install sqlite3 | "
        "MSYS2: pacman -S mingw-w64-x86_64-sqlite3"
    )
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
    else()
        option(DEPLOY_QT_RUNTIME "Deploy Qt runtime dependencies after build" OFF)
    endif()

    if(APPLE)
        set_target_properties(ContactManager PROPERTIES
            MACOSX_BUNDLE TRUE
            MACOSX_BUNDLE_BUNDLE_NAME "PhoneBook Manager"
            MACOSX_BUNDLE_GUI_IDENTIFIER "com.contactmanager.app"
            MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
            MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
        )
    endif()

    if(DEPLOY_QT_RUNTIME)
        if(WIN32)
            find_program(WINDEPLOYQT_EXECUTABLE windeployqt
                HINTS "${Qt6_DIR}/../../../bin"
            )
            if(WINDEPLOYQT_EXECUTABLE)
                add_custom_command(TARGET ContactManager POST_BUILD
                    COMMAND "${WINDEPLOYQT_EXECUTABLE}" --no-translations "$<TARGET_FILE:ContactManager>"
                    COMMENT "Deploying Qt6 runtime dependencies for Windows"
                )
            endif()
        elseif(APPLE)
            find_program(MACDEPLOYQT_EXECUTABLE macdeployqt
                HINTS "${Qt6_DIR}/../../../bin"
            )
            if(MACDEPLOYQT_EXECUTABLE)
                set(_macdeployqt_args -always-overwrite)
                get_filename_component(_qt_lib_dir "${Qt6_DIR}/../../../lib" ABSOLUTE)
                if(EXISTS "${_qt_lib_dir}")
                    list(APPEND _macdeployqt_args "-libpath=${_qt_lib_dir}")
                endif()
                foreach(_brew_lib IN ITEMS "/opt/homebrew/lib" "/usr/local/lib")
                    if(EXISTS "${_brew_lib}")
                        list(APPEND _macdeployqt_args "-libpath=${_brew_lib}")
                    endif()
                endforeach()

                add_custom_command(TARGET ContactManager POST_BUILD
                    COMMAND "${MACDEPLOYQT_EXECUTABLE}" "$<TARGET_BUNDLE_DIR:ContactManager>" ${_macdeployqt_args}
                    COMMENT "Deploying Qt6 frameworks into macOS .app bundle"
                )
            endif()
        endif()
    endif()
endif()

include(CTest)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
