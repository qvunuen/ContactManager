cmake_minimum_required(VERSION 3.10)

project(ContactManager VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

file(GLOB SOURCES "src/*.cpp")

# Create a data folder
add_custom_command(TARGET ContactManager PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_SOURCE_DIR}/data"
    COMMENT "Checking and Creating data folder..."
)

add_executable(ContactManager ${SOURCES})

set_target_properties(ContactManager PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)