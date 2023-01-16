cmake_minimum_required(VERSION 3.0.0)
project(demo VERSION 0.1.0)
include_directories("D:/MSVC/eigen-3.3.8")

add_executable(demo Matrix.hpp Matrix.cpp OptionPricing.cpp OptionPricing.hpp main.cpp) 
