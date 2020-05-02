cmake_minimum_required(VERSION 3.10.2)
project(infMOSTRO CXX)

set(PROJECT_AUTHOR "Laboratorio de Gestión de Sistemas en Tiempo Real")
set(PROJECT_COPYRIGHT "2020, Facultad de Minas, Universidad Nacional de Colombia")
set(PROJECT_DESCRIPTION "C++ Recomedador de Planes Semaforicos para Secretaría de Movilidad Medellín")
set(PROJECT_HOMEPAGE_URL "https://sites.google.com/unal.edu.co/lab-gstr/proyectos/moycot")
set(PROJECT_VCS_URL "https://github.com/jpnorenam/infMOSTRO")


set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_THREAD_PREFER_PTHREAD ON)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(TOPLEVEL_PROJECT ON)
else()
    set(TOPLEVEL_PROJECT OFF)
endif()


include(FindPkgConfig)
include(CheckIncludeFile)
include(FeatureSummary)
include(GNUInstallDirs)
include(GetVersion)
include(CMakeDependentOption)

add_definitions(-D_POSIX_C_SOURCE=200809L -D_GNU_SOURCE)
add_compile_options(-Wall -Werror -fdiagnostics-color=auto)
    
find_package(jsoncpp REQUIRED)
find_package(Boost REQUIRED)
find_package(spdlog 1.3.1 REQUIRED)

set(ENV{PKG_CONFIG_PATH} $ENV{PKG_CONFIG_PATH}:/usr/local/lib/pkgconfig:/usr/local/lib64/pkgconfig:/usr/local/share/pkgconfig:/usr/lib64/pkgconfig)

cmake_dependent_option(WITH_TESTS           "Run tests"                                             ON "TOPLEVEL_PROJECT" OFF)

GetVersion(${PROJECT_SOURCE_DIR} "CMAKE_PROJECT")

add_subdirectory(src)

if(WITH_TEST)
    add_subdirectory(test)
endif()

add_feature_info(TESTS                  WITH_TESTS                  "Run tests")

if(TOPLEVEL_PROJECT)
    feature_summary(WHAT ALL VAR FEATURES)
    message(STATUS "${FEATURES}")
    message(STATUS "Building ${CMAKE_PROJECT_DESCRIPTION}:")
    message(STATUS "  VERSION:         ${CMAKE_PROJECT_VERSION}")
    message(STATUS "  RELEASE:         ${CMAKE_PROJECT_RELEASE}")
    message(STATUS "  GIT_REV:         ${CMAKE_PROJECT_GIT_REV}")
    message(STATUS "  GIT_BRANCH:      ${CMAKE_PROJECT_GIT_BRANCH}")
    message(STATUS "  VARIANT:         ${CMAKE_PROJECT_VARIANT}")
    message(STATUS "  BUILD_ID:        ${CMAKE_PROJECT_BUILD_ID}")
    message(STATUS "  BUILD_DATE:      ${CMAKE_PROJECT_BUILD_DATE}")
    message(STATUS "  ARCH:            ${CMAKE_SYSTEM_PROCESSOR}")
    message(STATUS "  OS:              ${CMAKE_SYSTEM_NAME}")
endif()
