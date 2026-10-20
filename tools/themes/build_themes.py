#!/usr/bin/env python3
"""Generate res/themes/<family>_<scheme>.css from one skeleton and one palette.

A theme is three things, and they are kept apart on purpose:

  menu.css                the menu geometry every scheme carries, so both come
                          out at the height the sheet states rather than at
                          whatever the platform style hands out.
  skeleton_<scheme>.css   which rules a scheme has to carry. A light theme only
                          describes the ribbon; a dark theme also repaints the
                          standard widgets, because the platform style ignores a
                          dark palette and would draw light combos and trees.
  chrome_<family>.css     what makes a family recognisable beyond its colours -
                          the tab treatment, the corner radius, whether the strip
                          is separated from the page at all.
  PALETTES below          the colour roles. One entry per family and scheme.

Changing a colour is a one-line edit here; changing a family's look is an edit to
its chrome file; nothing has to be repeated ten times.

The gates in this file run before a sheet is written, so a bad value in a
palette fails the build instead of being repeated ten times.

Run:  python3 tools/themes/build_themes.py
"""

import json
import re
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parent.parent
OUT = ROOT / "res" / "themes"

FAMILIES = ("classic", "soft", "flat", "slate", "accent")

FAMILY_NOTES = {
    "classic": "A grey shelf holds the tabs and the selected one is a taller white "
               "card lifted out of it, with a rule closing the shelf off. Square "
               "corners, a box around the large buttons and a rule between groups.",
    "soft": "An inverse step: the body carries the tone and the selected tab opens "
            "into it, rounded and with no border at all, so nothing is drawn "
            "between tab and page. Hover is a wash of the accent.",
    "flat": "One flat surface from shelf to page, divided by nothing. The selection "
            "is an accent rule under the tab and an accent label, and the accent is "
            "used nowhere else - everything else hovers in plain grey.",
    "slate": "A deeper grey shelf over a white body. The active tab does not leave "
             "the shelf: only an accent rule along its top edge marks it. The accent "
             "appears there and on focus, nowhere else; groups are divided by "
             "hairlines.",
    "accent": "The active tab is the only colour on the bar - a solid block of the "
              "accent with a white label, since no other family fills the tab. The "
              "rest stay transparent in grey, a hover washed in the palest accent.",
}

# role -> colour. Every palette fills every role the two skeletons and the five
# chrome files can name, so a file never emits an unfilled placeholder.
PALETTES = {
    ("soft", "light"): {
        "body": "#edebe9",
        "text": "#323130", "text-muted": "#605e5c", "text-on-fill": "#ffffff",
        "surface": "#ffffff", "surface-alt": "#faf9f8", "surface-sub": "#f3f2f1",
        "state-rest": "#faf9f8",
        "state-hover": "#f3f2f1", "state-pressed": "#e1dfdd",
        "state-pressed-deep": "#c8c6c4",
        "border": "#e1dfdd", "border-strong": "#c8c6c4",
        "accent": "#0078d4", "accent-hover": "#106ebe",
        "accent-fill": "#106ebe", "accent-fill-strong": "#005a9e",
        "accent-faint": "#eff6fc", "accent-soft": "#c7e0f4",
        "accent-text": "#005a9e",
        "highlight": "#fff4ce",
        "close-hover": "#e81123", "close-pressed": "#e8636e",
    },
    ("soft", "dark"): {
        "body": "#3a3a3a",
        "text": "#f3f2f1", "text-muted": "#c8c8c8", "text-on-fill": "#ffffff",
        "surface": "#1f1f1f", "surface-alt": "#282828", "surface-sub": "#313131",
        "state-rest": "#282828",
        "state-hover": "#313131", "state-pressed": "#484848",
        "state-pressed-deep": "#4f4f4f",
        "border": "#3f3f3f", "border-strong": "#6d6d6d",
        "accent": "#3a96dd", "accent-hover": "#4ba0e1",
        "accent-fill": "#2b74a8", "accent-fill-strong": "#235a85",
        "accent-faint": "#112d43", "accent-soft": "#235a85",
        "accent-text": "#4ba0e1",
        "highlight": "#fff4ce",
        "close-hover": "#e81123", "close-pressed": "#e8636e",
    },
    ("classic", "light"): {
        "body": "#ffffff",
        "text": "#333333", "text-muted": "#666666", "text-on-fill": "#ffffff",
        "surface": "#ffffff", "surface-alt": "#f4f4f4", "surface-sub": "#eaeaea",
        "state-rest": "#f4f4f4",
        "state-hover": "#eaeaea", "state-pressed": "#d6d6d6",
        "state-pressed-deep": "#c0c0c0",
        "border": "#d6d6d6", "border-strong": "#b8b8b8",
        "accent": "#0072c6", "accent-hover": "#106ebe",
        "accent-fill": "#0072c6", "accent-fill-strong": "#004e8c",
        "accent-faint": "#e8f1fb", "accent-soft": "#cfe4f7",
        "accent-text": "#004e8c",
        "highlight": "#fff1b6",
        "close-hover": "#e81123", "close-pressed": "#e8636e",
    },
    ("classic", "dark"): {
        "body": "#1b1b1b",
        "text": "#f2f2f2", "text-muted": "#b0b0b0", "text-on-fill": "#ffffff",
        "surface": "#1b1b1b", "surface-alt": "#242424", "surface-sub": "#2e2e2e",
        "state-rest": "#242424",
        "state-hover": "#333333", "state-pressed": "#454545",
        "state-pressed-deep": "#525252",
        "border": "#3a3a3a", "border-strong": "#666666",
        "accent": "#0078d7", "accent-hover": "#2490e8",
        "accent-fill": "#106ebe", "accent-fill-strong": "#004e8c",
        "accent-faint": "#12314f", "accent-soft": "#1d4a75",
        "accent-text": "#2490e8",
        "highlight": "#fff1b6",
        "close-hover": "#e81123", "close-pressed": "#e8636e",
    },
    ("flat", "light"): {
        "body": "#ffffff",
        "text": "#333333", "text-muted": "#757575", "text-on-fill": "#ffffff",
        "surface": "#ffffff", "surface-alt": "#f7f7f7", "surface-sub": "#efefef",
        "state-rest": "#f7f7f7",
        "state-hover": "#efefef", "state-pressed": "#dedede",
        "state-pressed-deep": "#cccccc",
        "border": "#e6e6e6", "border-strong": "#c4c4c4",
        "accent": "#2b7de9", "accent-hover": "#1a6ad4",
        "accent-fill": "#1c6fd0", "accent-fill-strong": "#1259a8",
        "accent-faint": "#eaf3ff", "accent-soft": "#bfd8f8",
        "accent-text": "#1a6ad4",
        "highlight": "#fff1b6",
        "close-hover": "#e81123", "close-pressed": "#e8636e",
    },
    ("flat", "dark"): {
        "body": "#202324",
        "text": "#f0f0f0", "text-muted": "#a0a0a0", "text-on-fill": "#ffffff",
        "surface": "#202324", "surface-alt": "#2a2e30", "surface-sub": "#35393b",
        "state-rest": "#2a2e30",
        "state-hover": "#3a3f42", "state-pressed": "#474d50",
        "state-pressed-deep": "#555b5e",
        "border": "#33383a", "border-strong": "#5c6265",
        "accent": "#00a0e6", "accent-hover": "#22b3f0",
        "accent-fill": "#0f7fbf", "accent-fill-strong": "#0a5f90",
        "accent-faint": "#12333f", "accent-soft": "#1a4a5c",
        "accent-text": "#00a0e6",
        "highlight": "#ffe08a",
        "close-hover": "#e81123", "close-pressed": "#e8636e",
    },
    ("slate", "light"): {
        "body": "#ffffff",
        "text": "#333333", "text-muted": "#616161", "text-on-fill": "#ffffff",
        "surface": "#ffffff", "surface-alt": "#f3f3f3", "surface-sub": "#e5e5e5",
        "state-rest": "#f3f3f3",
        "state-hover": "#e5e5e5", "state-pressed": "#d6d6d6",
        "state-pressed-deep": "#c8c8c8",
        "border": "#e5e5e5", "border-strong": "#c8c8c8",
        "accent": "#007acc", "accent-hover": "#0062a3",
        "accent-fill": "#007acc", "accent-fill-strong": "#005a9e",
        "accent-faint": "#e8f2fb", "accent-soft": "#cfe6f7",
        "accent-text": "#005293",
        "highlight": "#fff1b6",
        "close-hover": "#e81123", "close-pressed": "#e8636e",
    },
    ("slate", "dark"): {
        "body": "#1e1e1e",
        "text": "#cccccc", "text-muted": "#9d9d9d", "text-on-fill": "#ffffff",
        "surface": "#1e1e1e", "surface-alt": "#252526", "surface-sub": "#2d2d30",
        "state-rest": "#252526",
        "state-hover": "#2a2d2e", "state-pressed": "#37373d",
        "state-pressed-deep": "#3e3e42",
        "border": "#3c3c3c", "border-strong": "#6b6b6b",
        "accent": "#007acc", "accent-hover": "#1f9cf0",
        "accent-fill": "#0e639c", "accent-fill-strong": "#094771",
        "accent-faint": "#04395e", "accent-soft": "#0d5580",
        "accent-text": "#4db8f5",
        "highlight": "#cca700",
        "close-hover": "#e81123", "close-pressed": "#e8636e",
    },
    ("accent", "light"): {
        "body": "#ffffff",
        "text": "#323130", "text-muted": "#605e5c", "text-on-fill": "#ffffff",
        "surface": "#ffffff", "surface-alt": "#faf9f8", "surface-sub": "#f3f2f1",
        "state-rest": "#faf9f8",
        "state-hover": "#f3f2f1", "state-pressed": "#e1dfdd",
        "state-pressed-deep": "#c8c6c4",
        "border": "#e1dfdd", "border-strong": "#c8c6c4",
        "accent": "#0078d4", "accent-hover": "#106ebe",
        "accent-fill": "#106ebe", "accent-fill-strong": "#005a9e",
        "accent-faint": "#eff6fc", "accent-soft": "#c7e0f4",
        "accent-text": "#005a9e",
        "highlight": "#fff4ce",
        "close-hover": "#e81123", "close-pressed": "#e8636e",
    },
    ("accent", "dark"): {
        "body": "#3a3a3a",
        "text": "#f3f2f1", "text-muted": "#c8c8c8", "text-on-fill": "#ffffff",
        "surface": "#1f1f1f", "surface-alt": "#282828", "surface-sub": "#313131",
        "state-rest": "#282828",
        "state-hover": "#313131", "state-pressed": "#484848",
        "state-pressed-deep": "#4f4f4f",
        "border": "#3f3f3f", "border-strong": "#6d6d6d",
        "accent": "#3a96dd", "accent-hover": "#4ba0e1",
        "accent-fill": "#2b74a8", "accent-fill-strong": "#235a85",
        "accent-faint": "#112d43", "accent-soft": "#235a85",
        "accent-text": "#4ba0e1",
        "highlight": "#fff4ce",
        "close-hover": "#e81123", "close-pressed": "#e8636e",
    },
}

HEADER = """/**
name: {family} {scheme}
**/

/*
 * {note}
 *
 * Generated by tools/themes/build_themes.py from menu.css,
 * skeleton_{scheme}.css, chrome_{family}.css, res/dockstyle/base.css and the
 * palette in that script - edit those, not this file, and run the script again.
 */
"""

# The three the palette does not carry, because they are washes rather than
# colours: fractions of white - of black, in a light scheme - laid over a
# surface the sheet cannot name. Two of them are the states of a button sitting
# on the accent, and what they wash is the text colour that reads there, which
# is white in both schemes; the third is the mark a side tab carries before it
# has anything to mark, and it has to go the way the surface is not. Deriving
# them keeps the palette to the colours a theme means, and keeps the sheet's
# answer the one DockStyleColors::fromPalette() computes at run time - the same
# file is the fall-back an application without a theme gets.
WASHES = {
    "on-accent-hover": "rgba(255, 255, 255, 60)",
    "on-accent-pressed": "rgba(255, 255, 255, 92)",
}
SUBTLE = {"light": "rgba(0, 0, 0, 48)", "dark": "rgba(255, 255, 255, 48)"}
# The corner tiers, the one non-colour vocabulary the sheets name: a radius is
# a family's manner as much as its colours are, so a skeleton or a chrome file
# picks a tier per element instead of spelling a pixel count. The values are
# the radii the sheets already emitted, kept so a theme regenerates to the same
# bytes it replaced; lg and pill have no user in these sheets yet and are there
# for the ones coming. The dock's run-time renderer keeps its own copy of the
# tiers for base.css - qxdock's dockstylesheet.cpp - and the two stay in step.
SHAPE = {
    "corner-none": "0px",
    "corner-sm": "2px",
    "corner-md": "3px",
    "corner-lg": "6px",
    "corner-pill": "9999px",
}
# A radius the gate can see spelled out is one a hand wrote past the tiers, and
# it is checked on the raw sheet - after fill() a tier *is* a pixel count, so
# the generated sheet is the wrong place to look.
RADIUS = re.compile(r"border-radius\s*:\s*[0-9]")
# The type tiers, the second non-colour vocabulary. The values are the sizes the
# dark skeleton already emitted - caption is the password character, subtitle is
# the calendar grid, body is the size the combo popup reads at - kept so a theme
# regenerates to the same bytes it replaced; title has no user in these sheets
# yet and is there for the ones coming. The light skeleton carries no type rules
# (a light theme leaves the standard widgets to the platform style), so the
# scale is picked once here, from the dark side's current values, and both
# skeletons read the same tiers when they name one - that is the flattening,
# not a second scale invented for light.
TYPE = {
    "type-caption": "10px",
    "type-subtitle": "13px",
    "type-body": "15px",
    "type-title": "18px",
}
# A font size the gate can see spelled out is one a hand wrote past the tiers,
# checked on the raw sheet for the same reason RADIUS is.
FONT_SIZE = re.compile(r"font-size\s*:\s*[0-9]")
# The spacing tiers, the third non-colour vocabulary: every value sits on the
# 4px grid, so a sheet cannot place an element between the steps. The chrome
# files speak them first - a family picks its tier per element, the way it
# picks a corner - and the skeletons still spell their paddings out; the gate
# below holds the chrome files to the tiers until the rest follows. The sheets
# speak the comfortable tier; compact is the control libraries' runtime switch,
# which follows the same tier names rather than a second scale. classic and
# accent carried a 10px tab padding that the grid has no step for - classic
# takes sm, the tighter boxy manner it already had; accent takes md, since a
# solid block of accent reads better with the room.
SPACING = {
    "space-xs": "4px",
    "space-sm": "8px",
    "space-md": "12px",
    "space-lg": "16px",
}
# A padding in a chrome file spelled out is one a hand wrote past the tiers.
# The gate reads the chrome file on its own - the skeletons and menu.css still
# spell paddings legitimately, and the combined sheet would fail for the wrong
# file's sake.
PAD = re.compile(r"padding[a-z-]*\s*:\s*[0-9]")
# The dock's half of a theme: one file, not one per scheme, because the two
# schemes ask it for the same structure and differ only in what the tokens are
# filled with. It stays in res/ rather than here because the library renders it
# at run time too - that is the sheet an application with no theme gets.
DOCK_PART = HERE.parent.parent / "res" / "dockstyle" / "base.css"

PLACEHOLDER = re.compile(r"\{\{([a-z-]+)\}\}")


def read(path):
    return path.read_text(encoding="utf-8")


def fill(text, palette, where):
    missing = sorted({m.group(1) for m in PLACEHOLDER.finditer(text)} - set(palette))
    if missing:
        raise SystemExit(f"{where}: the palette has no colour for {', '.join(missing)}")
    return PLACEHOLDER.sub(lambda m: palette[m.group(1)], text)


# ---------------------------------------------------------------------------
# Gates
# ---------------------------------------------------------------------------
# A sheet is generated, so one wrong value in a palette is repeated ten times
# before anyone looks at the result. Three mistakes are easy to make and hard
# to see, so the generator refuses to write any sheet that contains one:
#
#   contrast  text painted on a fill it cannot be read on. WCAG AA at 4.5:1 -
#             not the 3:1 large-text allowance, because these sheets paint
#             12-13px labels. A rule that names no colour takes the one its
#             resting rule last declared and, failing that, the palette's
#             text role, since that is what the platform paints when the
#             sheet is silent. Disabled text is exempt: it is meant to
#             recede. A gradient is checked stop by stop.
#   state     a hovered, selected or checked rule that rests in a state
#             colour and stays there, so the pointer has nothing to show.
#             Resting in a surface colour, a state may show itself on the
#             edge instead - an entry field whose hover is its border - so
#             only state colours are held to the fill. A state that paints
#             no fill at all is edge- or label-defined and left alone;
#             pressed is transient and focus is a ring.
#   selected  a selected item painted in the fill of the view behind it,
#             which is the first mistake seen from the outside: the row
#             cannot be found.
#
# What the gate cannot see it does not guess at: a rule that paints no fill
# is checked against nothing, because what is behind it is the platform's
# business. The caption glyphs are the one place where the sheet chooses a
# fill for a colour the C++ paints - windowbuttongroup.cpp tints the close
# button's hover and pressed glyph from the palette's highlighted-text role
# and leaves the others in window text - so those fills are checked against
# the role that paints them, at WCAG's 3:1 non-text floor.

WCAG_MIN = 4.5
WCAG_GLYPH_MIN = 3.0
FALLBACK_FG_ROLE = "text"
GATED_STATES = ("hover", "selected", "checked")
STATE_ROLES = ("state-rest", "state-hover", "state-pressed", "state-pressed-deep")
GLYPH_ON_FILL = "QxRibbon--WindowButton"

COMMENT = re.compile(r"/\*.*?\*/", re.S)
# a rule body may hold {{role}} placeholders, so the braces of a placeholder do
# not end it - the gate runs on the sheet before fill() spells the roles out
RULE = re.compile(r"([^{}]+)\{((?:[^{}]|\{\{[a-z-]+\}\})*)\}", re.S)
TOKEN = re.compile(r"\{\{([a-z-]+)\}\}")
HEX = re.compile(r"#([0-9a-fA-F]{3,8})\b")
# a declaration's value runs to the semicolon, the closing brace of the rule,
# or the end of the sheet - and a {{role}} placeholder is two braces that are
# not the rule's, so they are matched as a pair
DECL_BG = re.compile(r"(?<![-\w])background(?:-color)?\s*:\s*((?:[^;}]|\}\})*)")
DECL_FG = re.compile(r"(?<![-\w])color\s*:\s*((?:[^;}]|\}\})*)")
STATE = re.compile(r":(!)?(hover|selected|checked|pressed|focus|disabled|default)\b")
NOT_A_FILL = ("", "transparent", "none")
STATE_FILLS = {"{{%s}}" % role for role in STATE_ROLES}


def resolve(value, palette):
    """{{role}} spelled out; anything else passes through untouched."""
    if value is None:
        return None
    return TOKEN.sub(lambda m: palette[m.group(1)], value)


def norm(value):
    """One comparable form for a colour: #rgb spelled out, case folded."""
    if value is None:
        return None
    value = value.strip().lower()
    m = HEX.match(value)
    if m:
        h = m.group(1)
        if len(h) in (3, 4):
            h = "".join(c * 2 for c in h[:3])
        return "#" + h[:6]
    return value


def is_fill(value):
    n = norm(value)
    return n is not None and n not in NOT_A_FILL


def fill_hexes(value):
    """The plain colours a fill paints with - one, or every stop of a gradient."""
    out = []
    for m in HEX.finditer(value or ""):
        h = m.group(1)
        if len(h) in (3, 4):
            h = "".join(c * 2 for c in h[:3])
        out.append("#" + h[:6].lower())
    return out


def contrast(a, b):
    def lum(h):
        r, g, b = (int(h[i:i + 2], 16) for i in (1, 3, 5))
        return 0.2126 * _lin(r) + 0.7152 * _lin(g) + 0.0722 * _lin(b)

    la, lb = lum(norm(a)), lum(norm(b))
    hi, lo = max(la, lb), min(la, lb)
    return (hi + 0.05) / (lo + 0.05)


def _lin(c):
    c = c / 255.0
    return c / 12.92 if c <= 0.04045 else ((c + 0.055) / 1.055) ** 2.4


def base_of(selector):
    """The selector with its interaction states taken off - what it rests as."""
    return " ".join(STATE.sub("", selector).split())


def parse_rules(text):
    text = COMMENT.sub(" ", text)
    rules = []
    for sel, body in RULE.findall(text):
        sel = " ".join(sel.split())
        body = body.strip()
        if not sel or not body:
            continue
        fg = DECL_FG.search(body)
        bg = DECL_BG.search(body)
        rules.append({
            "selector": sel,
            "parts": [p.strip() for p in sel.split(",")],
            "fg": fg.group(1).strip() if fg else None,
            "bg": bg.group(1).strip() if bg else None,
            # a negated state is not a state: :hover:!selected is a hover
            "states": tuple(sorted({m.group(2) for m in STATE.finditer(sel)
                                    if not m.group(1)})),
        })
    return rules


def effective(rules):
    """What each selector-and-state pair ends up declaring. Two rules with the
    same selector and the same states are one rule as far as the cascade is
    concerned, so the later one's declarations win outright - which is how a
    chrome file overrides the skeleton it is appended to."""
    eff = {}
    for r in rules:
        for part in r["parts"]:
            slot = eff.setdefault((base_of(part), r["states"]), {"fg": None, "bg": None})
            if r["fg"]:
                slot["fg"] = r["fg"]
            if r["bg"]:
                slot["bg"] = r["bg"]
    return eff


def gate(raw, palette, where):
    """Every way the sheet can paint something unreadable or unseeable."""
    eff = effective(parse_rules(raw))
    bad = []

    # What each selector rests as: its stateless rule, and nothing else - a
    # state rule never restyles the selector it hangs off.
    rest = {base: slot for (base, states), slot in eff.items() if not states}

    for (base, states), slot in sorted(eff.items()):
        if "disabled" in states:
            continue
        r = rest.get(base, {"fg": None, "bg": None})
        fg = slot["fg"] or r["fg"] or "{{%s}}" % FALLBACK_FG_ROLE
        bg = slot["bg"] or r["bg"]
        if not is_fill(resolve(bg, palette)):
            continue
        if base.startswith(GLYPH_ON_FILL):
            # The close button's hover and pressed glyph is white: it sits on the
            # button's own fill, not on the application's emphasis, so it does
            # not take the pair of it. The others keep the window-text one.
            fg = "#ffffff" if "CloseWindowButton" in base else "{{text}}"
            floor = WCAG_GLYPH_MIN
        else:
            floor = WCAG_MIN
        fg = resolve(fg, palette)
        stops = fill_hexes(resolve(bg, palette))
        if not stops or not norm(fg).startswith("#"):
            continue
        worst = min(contrast(fg, s) for s in stops)
        if worst < floor:
            bad.append(f"{where}: {base} [{'/'.join(states) or 'resting'}] paints "
                       f"{norm(fg)} on {norm(resolve(bg, palette))}, {worst:.1f}:1 "
                       f"where {floor}:1 is the floor")

    for (base, states), slot in sorted(eff.items()):
        gated = [s for s in states if s in GATED_STATES]
        if not gated:
            continue
        rest_bg = rest.get(base, {"bg": None})["bg"]
        state_bg = slot["bg"] or rest_bg
        if not is_fill(resolve(rest_bg, palette)) and not is_fill(resolve(state_bg, palette)):
            continue
        if (rest_bg and rest_bg.strip() in STATE_FILLS
                and norm(resolve(state_bg, palette)) == norm(resolve(rest_bg, palette))):
            bad.append(f"{where}: {base} [{'/'.join(gated)}] keeps the resting fill "
                       f"{resolve(rest_bg, palette)} - nothing moves")

    for (base, states), slot in sorted(eff.items()):
        if "selected" not in states or "::item" not in base:
            continue
        container = base_of(base.split("::")[0])
        behind = rest.get(container, {}).get("bg")
        if (is_fill(resolve(behind, palette)) and is_fill(resolve(slot["bg"], palette))
                and norm(resolve(behind, palette)) == norm(resolve(slot["bg"], palette))):
            bad.append(f"{where}: {base} [selected] selects into "
                       f"{resolve(behind, palette)}, the fill of {container} behind it")

    return bad


def build(family, scheme):
    palette = dict(PALETTES[(family, scheme)])
    palette.update(WASHES)
    palette.update(SHAPE)
    palette.update(TYPE)
    palette.update(SPACING)
    palette["subtle"] = SUBTLE[scheme]
    menu = read(HERE / "menu.css")
    skeleton = read(HERE / f"skeleton_{scheme}.css")
    chrome = read(HERE / f"chrome_{family}.css")
    dock = read(DOCK_PART)
    # The menu geometry first: one set of rules for both schemes, so a skeleton
    # or a chrome file that has to restate one of them can. The dock's rules
    # last: where they restate one of the skeleton's, they have to come after
    # it, and nothing in them is meant to be restated in turn.
    body = (menu.rstrip() + "\n\n" + skeleton.rstrip() + "\n\n" + chrome.lstrip() + "\n\n"
            + dock.strip() + "\n")
    raw = HEADER.format(family=family, scheme=scheme, note=FAMILY_NOTES[family]) + "\n" + body
    return raw, fill(raw, palette, f"{family}_{scheme}"), palette


def main():
    sheets = [(family, scheme) + build(family, scheme)
              for family in FAMILIES for scheme in ("light", "dark")]
    problems = []
    for family, scheme, raw, filled, palette in sheets:
        problems += gate(raw, palette, f"{family}_{scheme}")
        if RADIUS.search(raw):
            problems.append(f"{family}_{scheme}: a border-radius spells a pixel "
                            "count where a corner tier belongs")
        if FONT_SIZE.search(raw):
            problems.append(f"{family}_{scheme}: a font-size spells a pixel "
                            "count where a type tier belongs")
    for family in FAMILIES:
        chrome_raw = read(HERE / f"chrome_{family}.css")
        if PAD.search(chrome_raw):
            problems.append(f"chrome_{family}: a padding spells a pixel count "
                            "where a spacing tier belongs")
    if problems:
        for line in problems:
            print(line, file=sys.stderr)
        raise SystemExit(f"{len(problems)} gate violation(s); no sheet was written - "
                         "fix the skeleton, the chrome file or the palette and run again")
    accents = {}
    for family, scheme, raw, filled, palette in sheets:
        (OUT / f"{family}_{scheme}.css").write_bytes(filled.encode("utf-8"))
        # The accent and the colour that reads on it, published as the pair the
        # sheet was drawn with: anything else and the palette computes the
        # second from the first, which on a saturated blue picks black where the
        # sheet picked white.
        accents[f"{family}-{scheme}"] = {"accent": palette["accent"],
                                         "on-accent": palette["text-on-fill"]}
    # The two colours a sheet cannot publish: the sheet has them substituted in,
    # where an application can only read them back by parsing CSS, and what
    # needs them is the palette - QPalette::Highlight, whose pair
    # HighlightedText is what a glyph on an emphasized surface is painted with.
    # One table, keyed by theme id, rather than one file per theme: a lone
    # key/value pair does not carry the weight of ten files, and a table lets
    # the ten be read at once - an accent out of step with its family is
    # visible next to the others. QX_APP's registerBundledThemes() reads this.
    (OUT / "accents.json").write_bytes(
        (json.dumps(accents, indent=2, sort_keys=True) + "\n").encode("utf-8"))
    print("wrote:", ", ".join(f"{f}_{s}.css" for f, s, _, _, _ in sheets))
    print("accents: accents.json")
    print(f"gates: {len(sheets)} sheets pass contrast and state checks")


if __name__ == "__main__":
    main()
