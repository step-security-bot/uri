#!/usr/bin/env python3
# ===- systemtests/test.py ------------------------------------------------===//
# *  _            _    *
# * | |_ ___  ___| |_  *
# * | __/ _ \/ __| __| *
# * | ||  __/\__ \ |_  *
# *  \__\___||___/\__| *
# *                    *
# ===----------------------------------------------------------------------===//
#  Distributed under the MIT License.
#  See https://github.com/paulhuggett/uri/blob/main/LICENSE for information.
#  SPDX-License-Identifier: MIT
# ===----------------------------------------------------------------------===//
import argparse
import sys
import uritools

def asstr (x):
    return '"' + x + '"' if x is not None else None


def parse (s):
    print('URI:', s)
    parts = uritools.urisplit(s)
    print(f" scheme: {asstr(parts.scheme)}")
    print(f" userinfo: {asstr(parts.userinfo)}")
    print(f" host: {asstr(parts.host)}")
    print(f" port: {asstr(parts.port)}")
    print(f" path: {asstr(parts.path)}")
    print(f" query: {asstr(parts.query)}")
    print(f" fragment: {asstr(parts.fragment)}")


def parse_file (path):
    with open(path) as f:
        for line in f.readlines():
            parse(line)


def main (args):
    parser = argparse.ArgumentParser (description="URI parser wrapper")
    parser.add_argument("binary", help="The path of the binary to be tested")
    parser.add_argument("tests", help="The path of the directory containing test inputs")

    options = parser.parse_args(args)
    parse_file (options.infile)


if __name__ == '__main__':
    sys.exit(main (sys.argv[1:]))
