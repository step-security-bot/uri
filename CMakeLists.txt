cmake_minimum_required (VERSION 3.13)
project (uri CXX)
add_executable (uri
  uri.hpp
  main.cpp
  rule.hpp
  rule.cpp
)
set_target_properties (
  uri
  PROPERTIES CXX_STANDARD 20
             CXX_STANDARD_REQUIRED Yes
             CXX_EXTENSIONS No
)
set (opts -fsanitize=undefined) #  -fsanitize=address)
target_compile_options (uri PUBLIC ${opts})
target_link_options (uri PUBLIC ${opts})
