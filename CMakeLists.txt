cmake_minimum_required(VERSION 3.27)
project(trading_engine_task)

# C++ standard
set(CMAKE_CXX_STANDARD 20)

# Optionally enable testing for CTest
enable_testing()

# Add subdirectories
add_subdirectory(src)
add_subdirectory(tests)
