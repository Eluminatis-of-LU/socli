cmake_minimum_required(VERSION 3.14)
project(socli VERSION 0.1.0 LANGUAGES C)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/template/version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/include/version.h @ONLY)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_STANDARD_REQUIRED ON)

set(BUILD_CURL_EXE OFF)
set(BUILD_SHARED_LIBS OFF)
set(BUILD_STATIC_LIBS ON)

if (WIN32)
    set(CURL_USE_SCHANNEL ON)
endif()

find_package(curl ${CURL_VERSION} QUIET)
if(NOT curl_FOUND)
    message(STATUS "curl not found, downloading...")
    include(FetchContent)
    FetchContent_Declare(
        curl
        GIT_REPOSITORY  https://github.com/curl/curl
        GIT_TAG curl-8_11_1
    )
    FetchContent_GetProperties(curl)
    if (NOT curl_POPULATED)
        set(FETCHCONTENT_QUIET NO)
        FetchContent_Populate(curl)
        add_subdirectory(${curl_SOURCE_DIR} ${curl_BINARY_DIR})
    endif()
endif()

find_package(c-logger QUIET)
if (NOT c-logger_FOUND)
    message(STATUS "c-logger not found, download...")
    include(FetchContent)
    FetchContent_Declare(
        c-logger
        GIT_REPOSITORY https://github.com/yksz/c-logger
        GIT_TAG de61ab5bccd65c08f20d4da02e36f2d971644952
    )
    FetchContent_GetProperties(c-logger)
    if (NOT c-logger_POPULATE)
        set(FETCHCONTENT_QUIET NO)
        FetchContent_Populate(c-logger)
        add_subdirectory(${c-logger_SOURCE_DIR} ${c-logger_BINARY_DIR})
    endif()
endif()

include_directories(${CMAKE_SOURCE_DIR}/include)
file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/* lib/* include/*) 
message("SOURCES: ${SOURCES}")
add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} CURL::libcurl_static logger_static)

if (LINUX)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(deps REQUIRED IMPORTED_TARGET glib-2.0 libsecret-1)
    target_link_libraries(${PROJECT_NAME} PkgConfig::deps)
endif()