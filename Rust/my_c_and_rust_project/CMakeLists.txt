# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following three lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(main)

# Method 1.

find_package(Corrosion REQUIRED)
corrosion_import_crate(MANIFEST_PATH "/home/travis/ECE406/my_rust_package_folder/Cargo.toml")
# add_executable(${PACKAGE_NAME} "/home/travis/ECE406/sample_project/main/main.c")
target_link_libraries(${PACKAGE_NAME} my_rust_package)

# Method 2.

# set(MY_RUST_PATH /home/travis/ECE406/my_rust_package_folder/target/debug/libmy_rust_package.a)
# add_library(my_rust_package STATIC IMPORTED)
# set_target_properties(my_rust_package PROPERTIES IMPORTED_LOCATION MY_RUST_PATH)
# target_link_libraries(${PACKAGE_NAME} my_rust_package)
