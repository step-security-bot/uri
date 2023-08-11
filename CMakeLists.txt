cmake_minimum_required (VERSION 3.13)
project (uri CXX)
add_executable (uri main.cpp)
set_target_properties (
  uri
  PROPERTIES CXX_STANDARD 20
             CXX_STANDARD_REQUIRED Yes
             CXX_EXTENSIONS No
)
