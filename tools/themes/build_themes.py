#!/usr/bin/env python3
"""Generate res/themes/<family>_<scheme>.css from one skeleton and one palette.

A theme is three things, and they are kept apart on purpose:

  skeleton_<scheme>.css   which rules a scheme has to carry. A light theme only
                          describes the ribbon; a dark theme also repaints the
                          standard widgets, because the platform style ignores a
                          dark palette and would draw light combos and trees.
  chrome_<family>.css     what makes a family recognisable beyond its colours -
                          the tab treatment, the corner radius, whether the strip
                          is separated from the page at all.
  PALETTES below          the colour roles. One entry per family and scheme.

Changing a colour is a one-line edit here; changing a family's look is an edit to
its chrome file; nothing has to be repeated eight times.

Run:  python3 tools/themes/build_themes.py
"""

import re
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parent.parent
OUT = ROOT / "res" / "themes"

FAMILIES = ("fluent", "office", "wps", "vscode")

FAMILY_NOTES = {
    "fluent": "Microsoft Fluent UI (formerly Office UI Fabric): flat white or "
              "nearly black surfaces, hairline strokes, a muted accent.",
    "office": "Office 2013/2016: a grey strip holding the tabs over white pages, "
              "square corners, an #0072c6 accent.",
    "wps": "WPS Office: one flat surface, no separators, the selection carried by "
           "an accent rule under the tab.",
    "vscode": "VS Code: a single editor-grey surface, no chrome separation, the "
              "active tab marked by an accent rule along its top.",
}

# role -> colour. Every palette fills every role the two skeletons and the four
# chrome files can name, so a file never emits an unfilled placeholder.
PALETTES = {
    ("fluent", "light"): {
        "text": "#323130", "text-muted": "#605e5c", "text-on-fill": "#ffffff",
        "surface": "#ffffff", "surface-alt": "#faf9f8", "surface-sub": "#f3f2f1",
        "state-hover": "#f3f2f1", "state-pressed": "#e1dfdd",
        "state-pressed-deep": "#c8c6c4",
        "border": "#e1dfdd", "border-strong": "#c8c6c4",
        "accent": "#0078d4", "accent-hover": "#106ebe",
        "accent-fill": "#106ebe", "accent-fill-strong": "#005a9e",
        "accent-faint": "#eff6fc", "accent-soft": "#c7e0f4",
        "highlight": "#fff4ce",
        "close-hover": "#e81123", "close-pressed": "#f1707a",
    },
    ("fluent", "dark"): {
        "text": "#f3f2f1", "text-muted": "#c8c8c8", "text-on-fill": "#ffffff",
        "surface": "#1f1f1f", "surface-alt": "#282828", "surface-sub": "#313131",
        "state-hover": "#313131", "state-pressed": "#484848",
        "state-pressed-deep": "#4f4f4f",
        "border": "#3f3f3f", "border-strong": "#6d6d6d",
        "accent": "#3a96dd", "accent-hover": "#4ba0e1",
        "accent-fill": "#3385c3", "accent-fill-strong": "#235a85",
        "accent-faint": "#112d43", "accent-soft": "#235a85",
        "highlight": "#fff4ce",
        "close-hover": "#e81123", "close-pressed": "#f1707a",
    },
    ("office", "light"): {
        "text": "#333333", "text-muted": "#666666", "text-on-fill": "#ffffff",
        "surface": "#ffffff", "surface-alt": "#f4f4f4", "surface-sub": "#eaeaea",
        "state-hover": "#eaeaea", "state-pressed": "#d6d6d6",
        "state-pressed-deep": "#c0c0c0",
        "border": "#d6d6d6", "border-strong": "#b8b8b8",
        "accent": "#0072c6", "accent-hover": "#106ebe",
        "accent-fill": "#0072c6", "accent-fill-strong": "#004e8c",
        "accent-faint": "#e8f1fb", "accent-soft": "#cfe4f7",
        "highlight": "#fff1b6",
        "close-hover": "#e81123", "close-pressed": "#f1707a",
    },
    ("office", "dark"): {
        "text": "#f2f2f2", "text-muted": "#b0b0b0", "text-on-fill": "#ffffff",
        "surface": "#1b1b1b", "surface-alt": "#242424", "surface-sub": "#2e2e2e",
        "state-hover": "#333333", "state-pressed": "#454545",
        "state-pressed-deep": "#525252",
        "border": "#3a3a3a", "border-strong": "#666666",
        "accent": "#0078d7", "accent-hover": "#2490e8",
        "accent-fill": "#106ebe", "accent-fill-strong": "#004e8c",
        "accent-faint": "#12314f", "accent-soft": "#1d4a75",
        "highlight": "#fff1b6",
        "close-hover": "#e81123", "close-pressed": "#f1707a",
    },
    ("wps", "light"): {
        "text": "#333333", "text-muted": "#757575", "text-on-fill": "#ffffff",
        "surface": "#ffffff", "surface-alt": "#f7f7f7", "surface-sub": "#efefef",
        "state-hover": "#efefef", "state-pressed": "#dedede",
        "state-pressed-deep": "#cccccc",
        "border": "#e6e6e6", "border-strong": "#c4c4c4",
        "accent": "#2b7de9", "accent-hover": "#1a6ad4",
        "accent-fill": "#2b7de9", "accent-fill-strong": "#1259a8",
        "accent-faint": "#eaf3ff", "accent-soft": "#d6e8ff",
        "highlight": "#fff1b6",
        "close-hover": "#e81123", "close-pressed": "#f1707a",
    },
    ("wps", "dark"): {
        "text": "#f0f0f0", "text-muted": "#a0a0a0", "text-on-fill": "#ffffff",
        "surface": "#202324", "surface-alt": "#2a2e30", "surface-sub": "#35393b",
        "state-hover": "#3a3f42", "state-pressed": "#474d50",
        "state-pressed-deep": "#555b5e",
        "border": "#33383a", "border-strong": "#5c6265",
        "accent": "#00a0e6", "accent-hover": "#22b3f0",
        "accent-fill": "#0f7fbf", "accent-fill-strong": "#0a5f90",
        "accent-faint": "#12333f", "accent-soft": "#1a4a5c",
        "highlight": "#ffe08a",
        "close-hover": "#e81123", "close-pressed": "#f1707a",
    },
    ("vscode", "light"): {
        "text": "#333333", "text-muted": "#616161", "text-on-fill": "#ffffff",
        "surface": "#ffffff", "surface-alt": "#f3f3f3", "surface-sub": "#e5e5e5",
        "state-hover": "#e5e5e5", "state-pressed": "#d6d6d6",
        "state-pressed-deep": "#c8c8c8",
        "border": "#e5e5e5", "border-strong": "#c8c8c8",
        "accent": "#007acc", "accent-hover": "#0062a3",
        "accent-fill": "#007acc", "accent-fill-strong": "#005a9e",
        "accent-faint": "#e8f2fb", "accent-soft": "#cfe6f7",
        "highlight": "#fff1b6",
        "close-hover": "#e81123", "close-pressed": "#f1707a",
    },
    ("vscode", "dark"): {
        "text": "#cccccc", "text-muted": "#9d9d9d", "text-on-fill": "#ffffff",
        "surface": "#1e1e1e", "surface-alt": "#252526", "surface-sub": "#2d2d30",
        "state-hover": "#2a2d2e", "state-pressed": "#37373d",
        "state-pressed-deep": "#3e3e42",
        "border": "#3c3c3c", "border-strong": "#6b6b6b",
        "accent": "#007acc", "accent-hover": "#1f9cf0",
        "accent-fill": "#0e639c", "accent-fill-strong": "#094771",
        "accent-faint": "#04395e", "accent-soft": "#0e639c",
        "highlight": "#cca700",
        "close-hover": "#e81123", "close-pressed": "#f1707a",
    },
}

HEADER = """/**
name: {family} {scheme}
**/

/*
 * {note}
 *
 * Generated by tools/themes/build_themes.py from skeleton_{scheme}.css,
 * chrome_{family}.css and the palette in that script - edit those, not this
 * file, and run the script again.
 */
"""

PLACEHOLDER = re.compile(r"\{\{([a-z-]+)\}\}")


def read(path):
    return path.read_text(encoding="utf-8")


def fill(text, palette, where):
    missing = sorted({m.group(1) for m in PLACEHOLDER.finditer(text)} - set(palette))
    if missing:
        raise SystemExit(f"{where}: the palette has no colour for {', '.join(missing)}")
    return PLACEHOLDER.sub(lambda m: palette[m.group(1)], text)


def main():
    written = []
    for family in FAMILIES:
        for scheme in ("light", "dark"):
            palette = PALETTES[(family, scheme)]
            skeleton = read(HERE / f"skeleton_{scheme}.css")
            chrome = read(HERE / f"chrome_{family}.css")
            body = skeleton.rstrip() + "\n\n" + chrome.lstrip()
            text = HEADER.format(family=family, scheme=scheme, note=FAMILY_NOTES[family]) + "\n" + body
            text = fill(text, palette, f"{family}_{scheme}")
            target = OUT / f"{family}_{scheme}.css"
            target.write_bytes(text.encode("utf-8"))
            written.append(target.name)
    print("wrote:", ", ".join(written))


if __name__ == "__main__":
    main()
