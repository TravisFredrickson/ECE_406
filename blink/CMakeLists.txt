# The following three lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(blink)

# My Rust stuff.

# include(FetchContent)
# FetchContent_Declare(
#     Corrosion
#     GIT_REPOSITORY https://github.com/corrosion-rs/corrosion.git
#     GIT_TAG v0.5
# )
# Set any global configuration variables such as `Rust_TOOLCHAIN` before this line!
# FetchContent_MakeAvailable(Corrosion)
find_package(Corrosion REQUIRED)

corrosion_import_crate(MANIFEST_PATH "/home/travis/ECE406/my_rust_package_folder/Cargo.toml")
add_executable(${PACKAGE_NAME} "/home/travis/ECE406/blink/main/blink_example_main.c")
target_link_libraries(${PACKAGE_NAME} my_rust_package)
