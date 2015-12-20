cmake_minimum_required(VERSION 3.3)

#set(MACOSX_BUNDLE_BUNDLE_NAME SymPoly)

include_directories(./include)

set(SOURCES include/sympoly/utils.hpp include/sympoly/internal/monomial.hpp include/sympoly/polynomial.hpp include/sympoly/lazy_operators.hpp include/sympoly/functional_support.h include/sympoly/operators.hpp demo.cpp)

add_definitions(-std=c++14)

project (sympoly)

add_executable(sympoly ${SOURCES})

#set_target_properties(sympoly PROPERTIES MACOSX_BUNDLE TRUE)
#target_link_libraries(sympoly lib1 lib2 lib3)
