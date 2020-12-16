cmake_minimum_required(VERSION 3.17)
project(HRJrenderer)

set(CMAKE_CXX_STANDARD 14)

add_executable(HRJrenderer main.cpp tgaimage.cpp tgaimage.h model.cpp model.h geometry.cpp geometry.h HRJmath.cpp HRJmath.h)

