#!/usr/bin/env python3
# Copyright (C) 2026 maminjie <canpool@163.com>
# SPDX-License-Identifier: MulanPSL-2.0
#
# check-encoding - fail when a tracked text file is not plain UTF-8.
#
# Every source file in this tree is UTF-8 and carries no byte order mark, which
# is only safe because the top-level CMakeLists.txt passes /utf-8 to MSVC: that
# switch is what tells the one compiler here that cannot work it out what the
# bytes mean. GCC and Clang assume UTF-8 and never needed telling, Qt 6 passes
# -utf-8 to its own consumers, and Qt 5 + MSVC gets it from that block - so the
# convention holds on every leg only as long as the files keep it.
#
# What makes a violation worth a gate is that the compiler does not report it.
# A file saved in the system code page builds, links and runs, and the u"..."
# literals in it are mojibake at run time; a BOM would build too, and only some
# of the compilers would read it as a hint, which is a second source of truth
# nobody would think to look for. Neither shows up in a diff and neither fails
# a test unless the test happens to compare that exact string, so the scan of
# the bytes is the only place that reports the file itself.
#
# Binary files are recognised by content - a NUL byte in the first block - and
# not by extension, so a new kind of file is covered without anyone having to
# update a list, and an image that happens not to be valid UTF-8 is not a
# finding. The file set comes from `git ls-files`, so the build trees and
# anything else untracked are out of scope by construction.
#
# Usage:
#   python3 scripts/check-encoding.py    # check; non-zero if anything is off

import os
import subprocess
import sys

SCRIPT_DIR = os.path.dirname(os.path.realpath(__file__))
ROOT_DIR = os.path.abspath(os.path.join(SCRIPT_DIR, os.pardir))

BOM = b"\xef\xbb\xbf"

# Enough for a NUL to appear in any binary format worth skipping, and short
# enough that the read stays cheap over a whole tree.
SNIFF_BYTES = 8192


def check_file(path):
    """Return the problems found in one file, as printable strings.

    Empty means the file is either UTF-8 without a BOM or binary, which is the
    only thing that can be said about a file from its bytes alone.
    """
    with open(path, "rb") as handle:
        data = handle.read()

    if b"\0" in data[:SNIFF_BYTES]:
        return []

    problems = []
    if data.startswith(BOM):
        problems.append("{}: starts with a byte order mark".format(path))

    try:
        data.decode("utf-8")
    except UnicodeDecodeError as error:
        # The line number is what a person needs to find the offending text; the
        # bytes are there for when the whole file is one line. Four of them are
        # shown rather than the single byte the decoder objected to, because a
        # file in the system code page fails one byte at a time and the sequence
        # is what says which code page it was saved in.
        line = data.count(b"\n", 0, error.start) + 1
        problems.append(
            "{}: not UTF-8 on line {}, at byte {}: the bytes there are {}".format(
                path,
                line,
                error.start,
                data[error.start : error.start + 4].hex(" "),
            )
        )

    return problems


def tracked_files():
    """Every file git has, relative to the repository root."""
    result = subprocess.run(
        ["git", "ls-files", "-z"], cwd=ROOT_DIR, stdout=subprocess.PIPE, check=True
    )
    return [name.decode("utf-8") for name in result.stdout.split(b"\0") if name]


def main():
    problems = []
    checked = 0
    for name in tracked_files():
        path = os.path.join(ROOT_DIR, name)
        if not os.path.isfile(path):
            continue
        checked += 1
        problems.extend(check_file(path))

    if problems:
        for problem in problems:
            print(problem, file=sys.stderr)
        print(
            "{} problem(s) in {} tracked file(s): every text file must be UTF-8 "
            "without a byte order mark - see doc/pages/build.md.".format(
                len(problems), checked
            ),
            file=sys.stderr,
        )
        return 1

    print("{} tracked file(s) checked: all UTF-8, no byte order mark.".format(checked))
    return 0


if __name__ == "__main__":
    sys.exit(main())
