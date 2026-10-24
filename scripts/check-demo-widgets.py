#!/usr/bin/env python3
# Copyright (C) 2026 maminjie <canpool@163.com>
# SPDX-License-Identifier: MulanPSL-2.0
#
# check-demo-widgets - fail when a demo puts a bare Qt control on a page.
#
# The demos are what the two libraries are shown by: a screen built out of
# qxfluent or qxmaterial is supposed to be built out of those two libraries, and
# a QPushButton on it is a control the library does not have. What makes this
# worth a gate rather than a habit is that a fallback never looks wrong - the
# theme sheets dress QPushButton, QComboBox and QLineEdit like everything else,
# so a page that dropped back to Qt renders plausibly and is simply not the
# family it claims to be. Nobody notices, and it accumulates: that is how the
# two application demos came to have fourteen of them.
#
# Three classes are counted, because three are what the demos had - the command,
# the choice, and the one line of text. The rest of Qt is not counted here: a
# demo needs a QLabel to say a word, a QWidget to be a host and a QScrollArea to
# scroll a page, and none of those is a control either library answers for.
#
# A construction is counted, not a mention. A demo that connects to the button
# one of the libraries made (FluentContentDialog's own, say) is talking about a
# widget it did not choose, and counting the mention would push the demos into
# avoiding the library to keep a script quiet.
#
# Usage:
#   python3 scripts/check-demo-widgets.py                 # non-zero exit on overrun
#   python3 scripts/check-demo-widgets.py --root <dir>
#   python3 scripts/check-demo-widgets.py --list          # print every finding

import argparse
import os
import re
import sys

SCRIPT_DIR = os.path.dirname(os.path.realpath(__file__))
ROOT_DIR = os.path.abspath(os.path.join(SCRIPT_DIR, os.pardir))

# The demos that answer to the two families this check is about. The ribbon and
# dock demos are not here: they show libraries of their own, and a QLineEdit
# inside a ribbon's panel is that library's vocabulary rather than a fallback.
DEFAULT_DEMOS = (
    "demos/qxapp/fluentdemo",
    "demos/qxapp/materialdemo",
    "demos/qxapp/controlgallery",
)

# What each class is allowed, and it stays zero: the number was there to come
# down, and it has. Adding one is the finding.
BUDGET = {
    "QPushButton": 0,
    "QComboBox": 0,
    "QLineEdit": 0,
}

SOURCES = (".cpp", ".h")

# A construction is the class name followed by its argument list. A mention -
# "connect(box, &QComboBox::activated, ...)" - has "::" there instead, and a
# sentence about the class in a comment has nothing.
CONSTRUCTION = re.compile(r"\b(QPushButton|QComboBox|QLineEdit)\s*\(")


def findings_in(path):
    """The (line, class, text) of every bare control *path* puts on a page."""
    found = []
    with open(path, "r", encoding="utf-8", errors="replace") as handle:
        for number, line in enumerate(handle, start=1):
            stripped = line.strip()
            if stripped.startswith("//") or stripped.startswith("*"):
                continue
            match = CONSTRUCTION.search(line)
            if match:
                found.append((number, match.group(1), stripped))
    return found


def main():
    parser = argparse.ArgumentParser(description="Count bare Qt controls in the demos.")
    parser.add_argument("--root", default=ROOT_DIR, help="the source tree to read")
    parser.add_argument("--list", action="store_true", help="print every finding, not only the overrun")
    options = parser.parse_args()

    counts = dict.fromkeys(BUDGET, 0)
    for demo in DEFAULT_DEMOS:
        for folder, _, files in os.walk(os.path.join(options.root, demo)):
            for name in sorted(files):
                if not name.endswith(SOURCES):
                    continue
                path = os.path.join(folder, name)
                for number, kind, text in findings_in(path):
                    counts[kind] += 1
                    if options.list:
                        print("%s:%d: %s" % (os.path.relpath(path, options.root), number, text))

    over = 0
    for kind in sorted(BUDGET):
        if counts[kind] > BUDGET[kind]:
            over += counts[kind] - BUDGET[kind]
            print("%s: %d on a page, budget %d" % (kind, counts[kind], BUDGET[kind]))

    if over:
        print("%d bare Qt control(s) over budget. The demos are what the libraries are" % over)
        print("shown by: use the family's own control, or add the one it is missing.")
        return 1

    print("no bare Qt control on a demo page (QPushButton, QComboBox, QLineEdit: all at budget 0)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
