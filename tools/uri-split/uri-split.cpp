//===- tools/uri-split/uri-split.cpp --------------------------------------===//
//*             _                 _ _ _    *
//*  _   _ _ __(_)      ___ _ __ | (_) |_  *
//* | | | | '__| |_____/ __| '_ \| | | __| *
//* | |_| | |  | |_____\__ \ |_) | | | |_  *
//*  \__,_|_|  |_|     |___/ .__/|_|_|\__| *
//*                        |_|             *
//===----------------------------------------------------------------------===//
// Distributed under the MIT License.
// See https://github.com/paulhuggett/uri/blob/main/LICENSE for information.
// SPDX-License-Identifier: MIT
//===----------------------------------------------------------------------===//
#include <filesystem>
#include <fstream>
#include <iostream>

#include "uri/uri.hpp"

using namespace std::literals::string_literals;

namespace {

bool read_stream (std::istream& is) {
  std::string line;
  while (getline (is, line)) {
    std::cout << "URI: " << line << '\n';

    auto r = uri::split (line);
    if (!r) {
      return false;
    }
    auto value_or_none = [] (std::optional<std::string_view> const& s) {
      return s ? "\""s + std::string{s.value ()} + "\""s : "None"s;
    };
    std::cout << " scheme: " << value_or_none (r->scheme);
    if (r->authority.has_value ()) {
      std::cout << "\n userinfo: " << value_or_none (r->authority->userinfo)
                << "\n host: " << value_or_none (r->authority->host)
                << "\n port: " << value_or_none (r->authority->port);
    } else {
      std::cout << "\n userinfo: None"
                << "\n host: None"
                << "\n port: None";
    }
    std::cout << "\n path: " << std::quoted (static_cast<std::string> (r->path))
              << "\n query: " << value_or_none (r->query)
              << "\n fragment: " << value_or_none (r->fragment) << '\n';
  }
  return true;
}

}  // end anonymous namespace

int main (int argc, char const* argv[]) {
  int exit_code = EXIT_SUCCESS;
  try {
    if (argc == 1) {
      return read_stream (std::cin) ? EXIT_SUCCESS : EXIT_FAILURE;
    }

    for (int arg = 1; arg < argc; ++arg) {
      std::filesystem::path const p = argv[arg];
      std::ifstream infile{p};
      if (!infile.is_open ()) {
        std::cerr << "Error: couldn't open " << p << '\n';
        return EXIT_FAILURE;
      }
      if (!read_stream (infile)) {
        return EXIT_FAILURE;
      }
    }
  } catch (std::exception const& ex) {
    std::cerr << "Error: " << ex.what () << '\n';
    exit_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "An unknown error occurred\n";
    exit_code = EXIT_FAILURE;
  }
  return exit_code;
}
