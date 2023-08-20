cmake_minimum_required (VERSION 3.13)
project (uri CXX)
set (STANDARD 17)

include (CheckCXXCompilerFlag)

function (setup_gtest)
  if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/googletest/CMakeLists.txt")
    # Tell gtest to link against the "Multi-threaded Debug DLL runtime library"
    # on Windows.
    set (
      gtest_force_shared_crt
      On
      CACHE BOOL "Always use msvcrt.dll"
    )
    # We don't want to install either gtest or gmock.
    set (
      INSTALL_GTEST
      Off
      CACHE BOOL "Disable gtest install"
    )
    set (
      INSTALL_GMOCK
      Off
      CACHE BOOL "Disable gmock install"
    )
    add_subdirectory (googletest)
    foreach (target gtest gmock gmock_main gtest_main)
      set_target_properties (
        ${target}
        PROPERTIES CXX_STANDARD ${STANDARD}
                   CXX_STANDARD_REQUIRED Yes
                   CXX_EXTENSIONS No
      )

      if (CMAKE_CXX_COMPILER_ID MATCHES "Clang$")
        check_cxx_compiler_flag (
          -Wno-implicit-int-float-conversion
          CLANG_W_NO_IMPLICIT_INT_FLOAT_CONVERSION
        )
        if (${CLANG_W_NO_IMPLICIT_INT_FLOAT_CONVERSION})
          list (APPEND gclang_options -Wno-implicit-int-float-conversion)
        endif ()
      endif ()

      if (LIBCXX)
        list (APPEND gclang_options -stdlib=libc++)
      endif ()

      target_compile_definitions (
        ${target} PUBLIC GTEST_REMOVE_LEGACY_TEST_CASEAPI_=1
      )
      target_compile_options (
        ${target}
        PRIVATE
          $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:${gclang_options}>
      )
      target_link_options (
        ${target}
        PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:${gclang_options}>
      )
    endforeach ()
  endif ()

endfunction (setup_gtest)

function (setup_target target)

  set_target_properties (
    ${target}
    PROPERTIES CXX_STANDARD ${STANDARD}
               CXX_STANDARD_REQUIRED Yes
               CXX_EXTENSIONS No
  )
  set (${target} -fsanitize=undefined) # -fsanitize=address)
  target_compile_options (${target} PUBLIC ${opts})
  target_link_options (${target} PUBLIC ${opts})

endfunction (setup_target)

add_executable (uri uri.hpp main.cpp rule.hpp rule.cpp)
setup_target (uri)

setup_gtest ()

add_executable (unittest test_rule.cpp)
target_link_libraries (unittest gmock_main)
setup_target (unittest)
