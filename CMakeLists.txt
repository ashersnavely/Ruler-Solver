cmake_minimum_required(VERSION 3.14)
project(GolombRuler)

set(SOURCE_FILES main.cpp Ruler.h)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS -pthread)
set(CMAKE_EXE_LINKER_FLAGS " -static")

#set(CMAKE_CXX_COMPILER i686-w64-mingw32-g++)
#set(CMAKE_SYSTEM_NAME Windows)

add_executable(Ruler main.cpp Ruler.h PolynomialRegression.h Ruler.cpp)