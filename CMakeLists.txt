cmake_minimum_required(VERSION 3.14)
project(socli VERSION 0.1.0 LANGUAGES C)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(curl ${CURL_VERSION} QUIET)
if(NOT curl_FOUND)
    message(STATUS "curl not found, downloading...")
    include(FetchContent)
    FetchContent_Declare(
        curl
        GIT_REPOSITORY  https://github.com/curl/curl
        GIT_TAG curl-8_11_1
    )
    FetchContent_GetProperties(CURL)
    if (NOT curl_POPULATED)
        set(FETCHCONTENT_QUIET NO)
        FetchContent_Populate(curl)
        add_subdirectory(${curl_SOURCE_DIR} ${curl_BINARY_DIR})
    endif()
endif()

file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} libcurl)