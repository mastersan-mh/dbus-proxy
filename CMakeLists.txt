# CMakeLists.txt
cmake_minimum_required(VERSION 3.15)
project(dbus-proxy
    VERSION 1.0.0
    DESCRIPTION "D-Bus TCP proxy with constant framing"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Предупреждение о платформе
if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(WARNING "This project relies on Linux-specific POSIX APIs. Build may fail on non-Linux systems.")
endif()

# Собирает все .cpp из src/ (автоматически подхватит новые файлы)
file(GLOB SOURCES "src/*.cpp" "src/*/*.cpp")
add_executable(dbus-proxy ${SOURCES})

target_include_directories(dbus-proxy PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_compile_options(dbus-proxy PRIVATE
    -Wall
    -Weffc++
    -Werror=sign-compare
    -Wmissing-declarations
    -Wpointer-arith
    -Wcast-align
    -Wformat
    -Wformat-nonliteral
    -Wformat-security
    -Wformat-signedness
    -Wundef
    -Wunused
    -Wunused-parameter
    -Wunused-but-set-parameter
    -Wunused-but-set-variable
    -Wshadow
    -march=native
    $<$<CONFIG:debug>:-g -O0 -DDEBUG>
    $<$<CONFIG:release>:-O2 -DNDEBUG>
)

target_compile_definitions(dbus-proxy PRIVATE _GNU_SOURCE)

find_package(Threads REQUIRED)
target_link_libraries(dbus-proxy PRIVATE
    Threads::Threads
)

# Установка в системные пути
include(GNUInstallDirs)
install(TARGETS dbus-proxy DESTINATION ${CMAKE_INSTALL_BINDIR})
