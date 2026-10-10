#!/usr/bin/env python3
"""Emit a clang-tidy -line-filter for the lines a change touched.

Why this exists: the CI job that runs clang-tidy wants to report the findings a
change *introduces*, and the obvious way to approximate that - analyse the
changed files - also reports everything those files were already guilty of.
On this tree that is not a rounding error: the whole of `src/libs` carries
hundreds of `bugprone-*` / `performance-*` findings, so a per-file scope would
bury the handful that matter. Filtering by *line* is what "introduced by this
change" actually means, and clang-tidy supports it directly.

Usage:
    scripts/tidy_line_filter.py <base-rev> [<head-rev>] [--path <pathspec>]

Writes the JSON array to stdout, ready for `-line-filter=<value>`. When the
change touches no source line at all the array is empty, which clang-tidy
reads as "filter everything out" - the caller is expected to treat that as
"nothing to report" rather than running the analysis.

Notes on the format (clang-tidy --help):
    [{"name":"file1.cpp","lines":[[1,3],[5,7]]},{"name":"file2.h"}]
`name` is matched against the *basename*, which is how the documented example
spells it and what the tool actually compares. Two files sharing a basename
would therefore share a filter; this tree has no such pair under `src/libs`,
and ranges are merged per basename so a future collision widens the filter
instead of raising an error.
"""

import collections
import json
import os
import re
import subprocess
import sys

HUNK = re.compile(r"^@@ -\d+(?:,\d+)? \+(\d+)(?:,(\d+))? @@")


def changed_lines(base, head, pathspec):
    """{path: [[start, end], ...]} of the lines *added* between base and head.

    Deletions count as a change even though they add no line: the hunk's
    position is where clang-tidy would report a finding about what is left
    there, so a zero-length range is widened to that single line.
    """
    diff = subprocess.run(
        ["git", "diff", "-U0", "--no-color", base, head, "--", pathspec],
        check=True,
        capture_output=True,
        text=True,
    ).stdout

    per_file = collections.defaultdict(list)
    path = None
    for line in diff.splitlines():
        if line.startswith("+++ "):
            target = line[4:].strip()
            # "/dev/null" appears when a file is created or deleted; the b/
            # side is the one that exists at head, so only it is of interest.
            path = None if target == "/dev/null" else target[2:]
            continue
        match = HUNK.match(line)
        if match and path:
            start = int(match.group(1))
            count = int(match.group(2)) if match.group(2) is not None else 1
            per_file[path].append([start, start + max(count, 1) - 1])
    return per_file


def main():
    argv = sys.argv[1:]
    pathspec = "src/libs"
    if "--path" in argv:
        i = argv.index("--path")
        pathspec = argv[i + 1]
        argv = argv[:i] + argv[i + 2 :]
    base = argv[0]
    head = argv[1] if len(argv) > 1 else "HEAD"

    per_file = changed_lines(base, head, pathspec)

    # clang-tidy matches on the basename, so merge by basename rather than by
    # path - two entries with the same basename would otherwise silently
    # override each other.
    per_name = collections.defaultdict(set)
    for path, ranges in per_file.items():
        for start, end in ranges:
            per_name[os.path.basename(path)].add((start, end))

    filtered = []
    for name in sorted(per_name):
        merged = []
        for start, end in sorted(per_name[name]):
            if merged and start <= merged[-1][1] + 1:
                merged[-1][1] = max(merged[-1][1], end)
            else:
                merged.append([start, end])
        filtered.append({"name": name, "lines": merged})

    json.dump(filtered, sys.stdout, separators=(",", ":"))
    sys.stdout.write("\n")


main()
