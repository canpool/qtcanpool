#!/usr/bin/env python3
# Copyright (C) 2026 maminjie <canpool@163.com>
# SPDX-License-Identifier: MulanPSL-2.0
#
# check-icons - fail when an icon resource does not match the icon policy.
#
# The libraries do not share icon code: qxribbon and qxdock both stay free of
# qxtheme, and only qxapp can see all three, so nothing forces the two tinting
# mechanisms to agree. They do not agree on their own either - qxribbon
# rasterises an icon and repaints its alpha, while qxdock replaces the literals
# #ffffff / #fff / #a0a0a0 / #000000 in the svg text. An icon that looks right
# under one of them can therefore be invisible, or stubbornly unthemed, under
# the other, and no compiler and no unit test says a word about it.
#
# What keeps them honest instead is a policy: every managed icon declares which
# of the two consumes it (or that none does), and this script checks that the
# file really matches what it declared. See doc/pages/icons.md.
#
# Four classes of finding are reported:
#
#   1. a tinted icon whose colours the consumer cannot handle - several colours
#      where one is required, or colours outside the consumer's canonical set;
#   2. an icon under a managed directory that was never classified;
#   3. a dock-consumed icon that paints no colour qxdock replaces, which is the
#      whole class of breakage an icon-set swap tends to produce;
#   4. a policy entry that rotted - it points at a file that is gone, or it
#      claims multicolour for an icon that is not.
#
# Usage:
#   python3 scripts/check-icons.py                  # non-zero exit on finding
#   python3 scripts/check-icons.py --policy <file>
#   python3 scripts/check-icons.py --root <dir>

import argparse
import os
import re
import sys

SCRIPT_DIR = os.path.dirname(os.path.realpath(__file__))
ROOT_DIR = os.path.abspath(os.path.join(SCRIPT_DIR, os.pardir))
DEFAULT_POLICY = os.path.join(SCRIPT_DIR, "icons-policy.txt")

KINDS = ("tinted", "multicolor", "plain")
CONSUMERS = ("alpha", "dock")

# The literals qxdock's applyDockIcons() knows how to replace, expanded to their
# six digit form so that #fff and #ffffff compare equal.
DOCK_CANONICAL = ("#000000", "#ffffff", "#a0a0a0")

# The colour a single-colour icon is written in. Which colour it is makes no
# difference to qxribbon (it keeps the alpha only); one fixed value exists so
# that "this icon is tinted" is visible in the file.
ALPHA_CANONICAL = ("#000000",)

# Anything that is not a colour of its own.
IGNORED_COLOURS = ("none", "transparent", "inherit", "currentcolor")

NAMED_COLOURS = {"white": "#ffffff", "black": "#000000"}

# fill / stroke as an attribute (fill="#000000") or inside style (fill:#000000),
# plus the two gradient stops, which paint as well.
COLOUR_RE = re.compile(
    r"(?:fill|stroke|stop-color|flood-color)\s*[:=]\s*[\"']?\s*([^\"';,>/]+)",
    re.IGNORECASE,
)

HEX3_RE = re.compile(r"#[0-9a-fA-F]{3}\Z")


class PolicyEntry(object):
    """One line of the policy file."""

    def __init__(self, kind, path, consumer, lineno):
        self.kind = kind
        self.path = path
        self.consumer = consumer
        self.lineno = lineno


class Finding(object):
    def __init__(self, cls, path, message):
        self.cls = cls
        self.path = path
        self.message = message


def normalise(value):
    """Lower case, #rgb expanded to #rrggbb, named colours resolved."""
    text = value.strip().strip("\"'").strip().lower()
    if text in NAMED_COLOURS:
        return NAMED_COLOURS[text]
    if HEX3_RE.match(text):
        return "#" + "".join(c * 2 for c in text[1:])
    return text


def painted_colours(path):
    """Every colour the icon paints, in the order it first appears."""
    with open(path, "r", encoding="utf-8", errors="replace") as handle:
        text = handle.read()
    colours = []
    for match in COLOUR_RE.finditer(text):
        raw = match.group(1).strip()
        if raw.lower().startswith("url("):
            continue
        colour = normalise(raw)
        if colour in IGNORED_COLOURS:
            continue
        if colour not in colours:
            colours.append(colour)
    return colours


def load_policy(path):
    entries = []
    errors = []
    with open(path, "r", encoding="utf-8") as handle:
        lines = handle.read().splitlines()
    for lineno, raw in enumerate(lines, start=1):
        line = raw.split("#", 1)[0].strip()
        if not line:
            continue
        parts = line.split()
        if len(parts) < 2:
            errors.append("%s:%d: expected '<kind> <path> [<consumer>]'" % (path, lineno))
            continue
        kind, target = parts[0], parts[1]
        consumer = parts[2] if len(parts) > 2 else "alpha"
        if kind not in KINDS:
            errors.append("%s:%d: unknown kind '%s' (want one of %s)"
                          % (path, lineno, kind, ", ".join(KINDS)))
            continue
        if consumer not in CONSUMERS:
            errors.append("%s:%d: unknown consumer '%s' (want one of %s)"
                          % (path, lineno, consumer, ", ".join(CONSUMERS)))
            continue
        if len(parts) > 3:
            errors.append("%s:%d: trailing fields ignored" % (path, lineno))
        entries.append(PolicyEntry(kind, target.replace("\\", "/").strip("/"), consumer, lineno))
    return entries, errors


def as_posix(path):
    return path.replace("\\", "/")


def managed_icons(root, entries):
    """Every svg under the directories the policy manages, as relative paths."""
    found = []
    for entry in entries:
        directory = os.path.join(root, entry.path)
        if not os.path.isdir(directory):
            continue
        for parent, dirs, names in os.walk(directory):
            dirs.sort()
            for name in sorted(names):
                if not name.endswith(".svg"):
                    continue
                absolute = os.path.join(parent, name)
                relative = as_posix(os.path.relpath(absolute, root))
                if relative not in found:
                    found.append(relative)
    return sorted(found)


def declaration_for(relative, entries):
    """The entry that decides this icon: a file entry, else the longest prefix."""
    exact = [e for e in entries if e.path == relative]
    if exact:
        return exact[0]
    prefixes = [e for e in entries if relative.startswith(e.path + "/")]
    if prefixes:
        return max(prefixes, key=lambda e: len(e.path))
    return None


def check_tinted(relative, colours, consumer):
    findings = []
    if consumer == "dock":
        replaceable = [c for c in colours if c in DOCK_CANONICAL]
        if not replaceable:
            findings.append(Finding(
                3, relative,
                "paints nothing qxdock can replace (dock icons must use %s)"
                % " / ".join(DOCK_CANONICAL)))
        outside = [c for c in colours if c not in DOCK_CANONICAL]
        if outside:
            findings.append(Finding(
                1, relative,
                "colour(s) qxdock cannot replace: %s" % ", ".join(outside)))
        return findings
    if len(colours) > 1:
        findings.append(Finding(
            1, relative,
            "declared tinted but paints %d colours: %s (a tinted icon is single colour)"
            % (len(colours), ", ".join(colours))))
    outside = [c for c in colours if c not in ALPHA_CANONICAL]
    if outside:
        findings.append(Finding(
            1, relative,
            "not the canonical single-colour value %s: %s"
            % (ALPHA_CANONICAL[0], ", ".join(outside))))
    return findings


def check(root, entries):
    findings = []
    stale = []
    for entry in entries:
        if not os.path.exists(os.path.join(root, entry.path)):
            stale.append(Finding(
                4, entry.path,
                "policy entry (line %d) points at a path that does not exist" % entry.lineno))
    icons = managed_icons(root, entries)
    for relative in icons:
        declaration = declaration_for(relative, entries)
        colours = painted_colours(os.path.join(root, relative))
        if declaration is None:
            findings.append(Finding(
                2, relative,
                "not classified: add it to the policy as %s" % " / ".join(KINDS)))
            continue
        if declaration.kind == "tinted":
            findings.extend(check_tinted(relative, colours, declaration.consumer))
        elif declaration.kind == "multicolor" and len(colours) < 2:
            findings.append(Finding(
                4, relative,
                "declared multicolor but paints %d colour(s)" % len(colours)))
    return findings + stale, len(icons)


def main():
    parser = argparse.ArgumentParser(
        description="fail when an icon resource does not match the icon policy")
    parser.add_argument("--policy", default=DEFAULT_POLICY,
                        help="the policy file to read (default: scripts/icons-policy.txt)")
    parser.add_argument("--root", default=ROOT_DIR,
                        help="the repository root the policy paths are relative to")
    args = parser.parse_args()

    if not os.path.isfile(args.policy):
        sys.stderr.write("check-icons: no such policy file: %s\n" % args.policy)
        return 2

    entries, errors = load_policy(args.policy)
    for error in errors:
        sys.stderr.write("check-icons: %s\n" % error)
    if errors:
        return 2

    findings, count = check(args.root, entries)
    findings.sort(key=lambda f: (f.cls, f.path))
    for finding in findings:
        print("[%d] %s: %s" % (finding.cls, finding.path, finding.message))

    managed = len([e for e in entries if os.path.isdir(os.path.join(args.root, e.path))])
    print("check-icons: %d icon(s) under %d managed director(y/ies), %d finding(s)"
          % (count, managed, len(findings)))
    return 1 if findings else 0


if __name__ == "__main__":
    sys.exit(main())
