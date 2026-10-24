#!/usr/bin/env python3
# Copyright (C) 2026 maminjie <canpool@163.com>
# SPDX-License-Identifier: MulanPSL-2.0
#
# check-changelog - fail when a library's changelog breaks its shape.
#
# Every library here keeps the same small document: the version in force at the
# top - "Unreleased" while the next one is being made - a rule under it, and one
# section per kind of change, each entry a bullet. The shape is the point: a
# reader scans for the section, and a section that runs into the entry above it,
# or a heading spelled its own way, is a document nobody can scan. A compiler
# never sees any of this - the file is text, and it renders either way.
#
# What is checked, and only in the block being written - the top block, up to
# the first version number:
#   - the block opens with "Unreleased" and a rule under it
#   - every heading is one of the four kinds, spelled as the project spells them
#   - the four come in this order, once each: a second "Bugfix" further down is a
#     section the writer started instead of adding to the one above it
#   - a heading stands alone: one empty line before it, and at least one entry
#     under it
#   - an entry is a bullet, and a wrapped line is indented under a bullet
#   - the file is UTF-8 with no byte order mark, LF line endings, no trailing
#     space, and ends with a newline
#
# The published blocks below it are left alone: they are what shipped, and a
# gate has no business rewriting history. What it guards is the block that is
# still being added to, which is the one that can be got wrong today - and a
# file whose top block is a version number has no such block, so only its
# encoding is read.
#
# Usage:
#   python3 scripts/check-changelog.py
#   python3 scripts/check-changelog.py --root <dir>

import argparse
import os
import re
import sys

SCRIPT_DIR = os.path.dirname(os.path.realpath(__file__))
ROOT_DIR = os.path.abspath(os.path.join(SCRIPT_DIR, os.pardir))

HEADINGS = ("Feature", "Interface", "Optimize", "Bugfix")
VERSION = re.compile(r"^\d+\.\d+\.\d+$")
HEADING = re.compile(r"^[A-Z][A-Za-z]+$")
BULLET = re.compile(r"^- ")


def library_changelogs(root):
    """Every library's changelog, in a stable order."""
    found = []
    libs = os.path.join(root, "src", "libs")
    for name in sorted(os.listdir(libs)):
        path = os.path.join(libs, name, "CHANGELOG")
        if os.path.isfile(path):
            found.append(path)
    return found


def living_block(lines):
    """The index where the block being written ends: the first version line."""
    for index, line in enumerate(lines):
        if index > 0 and VERSION.match(line):
            return index
    return len(lines)


def findings_in(path):
    """The (line, message) of everything wrong with the block being written."""
    with open(path, "rb") as handle:
        raw = handle.read()
    found = []
    if raw.startswith(b"\xef\xbb\xbf"):
        found.append((1, "byte order mark at the top of the file"))
    if b"\r\n" in raw:
        found.append((1, "CRLF line endings"))
    if raw and not raw.endswith(b"\n"):
        found.append((len(raw.splitlines()), "no newline at the end of the file"))

    lines = raw.decode("utf-8", errors="replace").splitlines()
    for number, line in enumerate(lines, start=1):
        if line != line.rstrip():
            found.append((number, "trailing space"))

    if not lines:
        found.append((1, "the file is empty"))
        return found
    if not (lines[0] == "Unreleased" or VERSION.match(lines[0])):
        found.append((1, 'the block must open with "Unreleased" or a version'))
        return found
    if len(lines) < 2 or lines[1] != "---":
        found.append((2, 'a "---" rule belongs under the version'))
        return found
    if VERSION.match(lines[0]):
        # Nothing is being written in this file: what it holds is what shipped,
        # and the shape it shipped in is not this gate's to judge.
        return found

    end = living_block(lines)
    seen = {}
    rank = -1
    headings = []
    for index in range(2, end):
        line = lines[index]
        if not HEADING.match(line):
            continue
        number = index + 1
        if line not in HEADINGS:
            found.append((number, 'unknown section "%s" (the four are Feature, Interface, Optimize, Bugfix)' % line))
            continue
        if line in seen:
            found.append((number, '"%s" is already a section of this block (line %d): add to that one' % (line, seen[line])))
            continue
        if HEADINGS.index(line) < rank:
            found.append((number, '"%s" stands after a kind that follows it (the four are in the order Feature, '
                                  'Interface, Optimize, Bugfix)' % line))
        rank = max(rank, HEADINGS.index(line))
        seen[line] = number
        headings.append(number)
        if index > 2 and lines[index - 1].strip():
            found.append((number, 'no empty line before "%s"' % line))
        # An entry under the heading, before the next heading or the end.
        under = []
        for later in range(index + 1, end):
            text = lines[later]
            if HEADING.match(text) and text in HEADINGS:
                break
            if BULLET.match(text):
                under.append(later)
        if not under:
            found.append((number, '"%s" has no entry under it' % line))

    if not headings:
        found.append((3, "the block has no section"))

    # Bullets and their wrapped lines, outside the headings.
    for index in range(2, end):
        line = lines[index]
        number = index + 1
        if not line.strip() or HEADING.match(line) or line == "---":
            continue
        if BULLET.match(line):
            continue
        if line.startswith("  "):
            # A wrapped line belongs under a bullet somewhere above it: a nested
            # list inside an entry is a bullet of its own, further in.
            if not any(BULLET.match(earlier) for earlier in lines[2:index]):
                found.append((number, "an indented line with no entry above it"))
            continue
        found.append((number, "a line that is neither a heading, an entry nor a wrap: %r" % line[:40]))

    return found


def main():
    parser = argparse.ArgumentParser(description="Check the shape of the libraries' changelogs.")
    parser.add_argument("--root", default=ROOT_DIR, help="the source tree to read")
    options = parser.parse_args()

    paths = library_changelogs(options.root)
    if not paths:
        print("no library changelog found under %s" % options.root)
        return 1

    bad = 0
    for path in paths:
        for number, message in findings_in(path):
            bad += 1
            print("%s:%d: %s" % (os.path.relpath(path, options.root), number, message))

    if bad:
        print("%d problem(s) in the changelog block being written." % bad)
        return 1

    print("%d library changelog(s) keep their shape" % len(paths))
    return 0


if __name__ == "__main__":
    sys.exit(main())
