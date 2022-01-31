cmake_minimum_required(VERSION 3.21)

project(cardlock)

find_package(Threads REQUIRED)

if(WIN32 OR MSYS)
    add_library(PkgConfig::PCSCLITE INTERFACE IMPORTED)
    target_link_libraries(PkgConfig::PCSCLITE INTERFACE winscard)
elseif(APPLE)
    find_library(PCSC_LIBRARIES PCSC)
    add_library(PkgConfig::PCSCLITE INTERFACE IMPORTED)
    set_target_properties(PkgConfig::PCSCLITE PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${PCSC_LIBRARIES}/Headers/")
    set_target_properties(PkgConfig::PCSCLITE PROPERTIES INTERFACE_LINK_LIBRARIES "${PCSC_LIBRARIES}")
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(PCSCLITE libpcsclite REQUIRED IMPORTED_TARGET)
endif()

if(MSYS)
    add_compile_options(-mwin32)
endif()

add_executable(cardlock cardlock.c)
target_link_libraries(cardlock PRIVATE PkgConfig::PCSCLITE)

add_executable(cardlock_cancel cardlock_cancel.c)
target_link_libraries(cardlock_cancel PRIVATE PkgConfig::PCSCLITE Threads::Threads)
