# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following three lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(my_project)

# Method 1: Corrosion.

set(Rust_CARGO_TARGET riscv32imac-esp-espidf) # Added because of a warning.
find_package(Corrosion REQUIRED)
corrosion_import_crate(MANIFEST_PATH "/home/travis/ECE_406/Rust/my_rust_folder/Cargo.toml") # Creates target "my_rust_package".
add_executable(my_target "main/main.c")
target_link_libraries(my_target PUBLIC my_rust_package)

# Method 2: Manual.

# add_library(my_rust_package STATIC IMPORTED)
# set_target_properties(my_rust_package PROPERTIES IMPORTED_LOCATION /home/travis/ECE_406/Rust/my_rust_folder/target/debug/libmy_rust_package.a)
# add_executable(my_target "main/main.c")
# target_link_libraries(my_target PUBLIC my_rust_package)
