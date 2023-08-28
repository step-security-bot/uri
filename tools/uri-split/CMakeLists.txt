#===- tools/uri-split/CMakeLists.txt --------------------------------------===//
#*   ____ __  __       _        _     _     _        *
#*  / ___|  \/  | __ _| | _____| |   (_)___| |_ ___  *
#* | |   | |\/| |/ _` | |/ / _ \ |   | / __| __/ __| *
#* | |___| |  | | (_| |   <  __/ |___| \__ \ |_\__ \ *
#*  \____|_|  |_|\__,_|_|\_\___|_____|_|___/\__|___/ *
#*                                                   *
#===----------------------------------------------------------------------===//
#
# Distributed under the MIT License.
# See https://github.com/paulhuggett/uri/blob/main/LICENSE.TXT
# for license information.
# SPDX-License-Identifier: MIT
#
#===----------------------------------------------------------------------===//
add_executable (uri-split uri-split.cpp)
setup_target (uri-split)
target_link_libraries (uri-split PUBLIC urilib)

if (TARGET gtest)
  set (prelink_target uri-split)
  set (test_target unittest)
  set (out_xml "${CMAKE_BINARY_DIR}/${test_target}.xml")
  add_custom_command (
    TARGET ${prelink_target}
    PRE_LINK
    COMMAND "$<TARGET_FILE:${test_target}>" "--gtest_output=xml:${out_xml}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    COMMENT "Running unit tests" DEPENDS ${test_target}
    BYPRODUCTS ${out_xml}
    VERBATIM
  )
endif ()
