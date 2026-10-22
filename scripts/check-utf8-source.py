#!/usr/bin/env python3
# Copyright (C) 2026 maminjie <canpool@163.com>
# SPDX-License-Identifier: MulanPSL-2.0
#
# check-utf8-source - compile one real source the way the build does, and read
# its Chinese literal back out of the object file.
#
# The tree's sources are UTF-8 and CMakeLists.txt tells MSVC so with /utf-8.
# Whether that switch is still there, still inside the if(MSVC) branch and still
# applied before the targets are created are three ways to lose it, and all
# three are silent: the tree compiles, the literal is mojibake at run time, and
# only a test that compares that exact string notices.
#
# The reason this does not simply run the test suite is Qt 6: it passes -utf-8
# to its consumers through Qt6::Platform's interface compile options, so on any
# leg CI can run today the literal survives with the switch deleted, and a green
# suite would say nothing. Qt 5.15.2 would be the leg where the switch is the
# whole story, but its headers do not compile with the toolsets the runners now
# ship (stdext::checked_array_iterator is gone), so the observation is taken
# instead from the compiler command line itself: the build's own command for
# this file, re-run with Qt's -utf-8 taken out - which is exactly the Qt 5
# situation - has to keep the literal intact. A second re-run with every UTF-8
# switch taken out shows what "not decoded as UTF-8" looks like, so the check
# cannot pass by accident on a needle that is not actually discriminating.
#
# The subject is the demo translation unit that holds the needle below and
# includes no .moc, so it compiles on its own. Which file that is is looked up in
# compile_commands.json rather than written down here: it has moved once already.
#
# Usage:
#   python3 scripts/check-utf8-source.py <build-dir>
#
# <build-dir> must have been configured with -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
# and with the demo built (WITH_DEMOS=ON, the default).

import json
import os
import re
import subprocess
import sys

SCRIPT_DIR = os.path.dirname(os.path.realpath(__file__))
ROOT_DIR = os.path.abspath(os.path.join(SCRIPT_DIR, os.pardir))

# The needle. It is a string the demo shows - the "Chinese display test" group -
# and it is spelled here as escapes so this file holds no non-ASCII that could
# fail for the reason it is checking.
LITERAL = "\u4e2d\u6587\u663e\u793a\u6d4b\u8bd5"

# The subject is whichever translation unit in the demo tree holds that needle
# and compiles on its own. It is looked up rather than named: the needle has
# moved once already - when the demo was split into pages the group travelled
# from mainwindow.cpp to otherpage.cpp - and a named file that no longer holds
# it turns this check into a report about the wrong thing. Both runs then say
# "the object holds no UTF-16" while the switch was never missing at all.
SUBJECT_ROOTS = ("/demos/",)

# The two flags are the two spellings in play: CMakeLists.txt writes /utf-8,
# Qt writes -utf-8 on its interface. Taking out the dash one leaves the tree's
# own switch on its own.
QT_ONLY = r"(?<=\s)-utf-8(?=\s)"
ANY = r"(?<=\s)[/-]utf-8(?=\s)"

# "#include "mainwindow.moc"" - the one include that makes a file uncompilable
# until moc has run.
MOC_INCLUDE = re.compile(rb'#\s*include\s*["<][^">]*\.moc[">]')


def find_entry(build_dir):
    """The compile command for the translation unit the check is about."""
    path = os.path.join(build_dir, "compile_commands.json")
    if not os.path.isfile(path):
        sys.exit(
            "{}: no compile_commands.json - configure with "
            "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON".format(build_dir)
        )

    with open(path, encoding="utf-8") as handle:
        database = json.load(handle)

    needle = LITERAL.encode("utf-8")
    found = []
    for entry in database:
        subject = entry["file"].replace("\\", "/")
        if not any(root in subject for root in SUBJECT_ROOTS):
            continue
        if not os.path.isfile(subject):
            continue
        with open(subject, "rb") as handle:
            source = handle.read()
        # A file that includes its own .moc only compiles once moc has run, so
        # it cannot be compiled on its own from the command line alone.
        if needle not in source or MOC_INCLUDE.search(source):
            continue
        found.append((subject, entry))

    if not found:
        sys.exit(
            "{}: no source under {} holds the needle {} and compiles on its "
            "own, so there is nothing for this check to read the literal out "
            "of - configure with WITH_DEMOS=ON, or point SUBJECT_ROOTS at "
            "where the demo's Chinese strings live now".format(
                build_dir, ", ".join(SUBJECT_ROOTS), LITERAL
            )
        )

    found.sort(key=lambda pair: pair[0])
    return found[0]


def retarget(command, object_path, pdb_path):
    """Run the command for our own outputs, without the precompiled header.

    The precompiled header is dropped rather than built: /Yu names a .pch that
    only a full build produces, and the check wants the flags, not the speed.
    """
    command = re.sub(r"/Yu\S*|/Fp\S*", "", command)
    # Lambdas rather than replacement strings: the paths go through verbatim, so
    # a backslash in them is not read as an escape.
    command = re.sub(r"/Fo\S*", lambda _match: "/Fo" + object_path, command)
    command = re.sub(r"/Fd\S*", lambda _match: "/Fd" + pdb_path, command)
    return command


def compile_once(command, directory):
    """Compile, and return (succeeded, output)."""
    # shell=True so the quoting reaches the compiler the way it does from the
    # build system; the command is a string from compile_commands.json and its
    # paths are already quoted for cmd.exe.
    completed = subprocess.run(
        command,
        cwd=directory,
        shell=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
        errors="replace",
    )
    return completed.returncode == 0, completed.stdout


def holds_literal(object_path):
    """Whether the object file holds the literal as UTF-16, as it should."""
    with open(object_path, "rb") as handle:
        return LITERAL.encode("utf-16-le") in handle.read()


def main():
    build_dir = sys.argv[1] if len(sys.argv) > 1 else "build"
    subject, entry = find_entry(build_dir)

    # Absolute, because the compiler runs in the entry's directory and would
    # resolve a relative output path against that instead of against ours - on
    # CI, where the build directory is named relative to the checkout, that put
    # the object in build-encoding/build-encoding.
    scratch = os.path.abspath(os.path.join(build_dir, "encoding-check"))
    os.makedirs(scratch, exist_ok=True)

    # (label, flag to take out, what the run has to show). The last one is the
    # control: with no switch at all the literal must be gone - or the file must
    # not compile at all, which is what happens here, because the system code
    # page decodes one of these strings into a trailing byte that escapes the
    # quote. Either way it is proof that the needle above discriminates.
    runs = (
        ("the command the build uses", None, "keep"),
        ("the same command with Qt's own -utf-8 taken out", QT_ONLY, "keep"),
        ("the same command with every UTF-8 switch taken out", ANY, "lose"),
    )

    failures = []
    for index, (label, pattern, wanted) in enumerate(runs):
        command = entry["command"]
        if pattern:
            command = re.sub(pattern, "", command)
        command = retarget(
            command,
            os.path.join(scratch, "run{}.obj".format(index)),
            os.path.join(scratch, "run{}.pdb".format(index)),
        )

        object_path = os.path.join(scratch, "run{}.obj".format(index))
        succeeded, output = compile_once(command, entry["directory"])

        if wanted == "lose":
            if succeeded and holds_literal(object_path):
                failures.append(
                    "{}: the literal survived anyway, so it says nothing about "
                    "the switch and this check cannot fail for the reason it "
                    "exists".format(label)
                )
            continue

        if not succeeded:
            failures.append("{}: did not compile:\n{}".format(label, output))
        elif not holds_literal(object_path):
            failures.append(
                "{}: the object holds no UTF-16 for the literal - the source "
                "was not read as UTF-8".format(label)
            )

    if failures:
        for failure in failures:
            print(failure, file=sys.stderr)
        print(
            "{}: the UTF-8 switch the build passes to MSVC is missing or not "
            "effective; see the if(MSVC) block in CMakeLists.txt.".format(subject),
            file=sys.stderr,
        )
        return 1

    print(
        "{}: the literal survives on the build's own flags and on the tree's "
        "switch alone, and not without a switch at all.".format(subject)
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())
