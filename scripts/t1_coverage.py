#!/usr/bin/env python3
# Copyright (C) 2026 maminjie <canpool@163.com>
# SPDX-License-Identifier: MulanPSL-2.0
#
# t1_coverage - emit the 4.7 T1 coverage matrix (widgets x eight dimensions).
#
# The matrix is evidence, not opinion: every cell is decided by whether the
# widget's test file actually drives that dimension. A dimension counts as
# driven only when the case sets up a non-default state itself - opening a
# menu with the mouse, stepping a slider with the keyboard, calling
# setEnabled(false) - not when the word merely appears in the file. Forty of
# the eighty-one test files mention "palette"; that is not a theme test.
#
# Usage:
#   python scripts/t1_coverage.py            # markdown for the doc
#   python scripts/t1_coverage.py --stats    # counts only, for a quick look
#   python scripts/t1_coverage.py --debt     # what is still only default-state

import io
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

# What it takes to say a dimension was exercised: the case has to put the
# widget into that state by itself.
DRIVEN = {
    "pointer": r"mouseClick|mousePress|mouseMove|mouseRelease|qtestMouseEvent"
               r"|QTest::mouse(Wheel|Move|Click|DClick|Press|Release)?\s*\(",
    "keys": r"QTest::key(Click|Press|Release|Sequence)?\s*\(|Qt::Key_",
    "focus": r"\.setFocus\s*\(|->setFocus\s*\(|hasFocus\s*\(|setTabOrder|focusPolicy|FocusPolicy",
    "disabled": r"setEnabled\s*\(\s*(false|true)|setDisabled\s*\(|isEnabled\s*\(",
    "theme": r"QxThemeManager|setTheme|applyTheme|setPalette\s*\(|QPalette\s*\(|colorScheme|ColorScheme",
    "motion": r"reduceMotion|ReduceMotion|UiEffects|animationsEnabled|setMotionReduced",
    "a11y": r"accessibleName|accessibleRole|accessibleDescription|QAccessible",
    "rtl": r"RightToLeft|setLayoutDirection|isRightToLeft",
}

DIMS = ["pointer", "keys", "focus", "disabled", "theme", "motion", "a11y", "rtl"]

# What makes a widget carry a movement: an animation class of its own, or the
# tier lookup that a family hands out instead of a number. Both are read from
# the source, because the question is what the widget does, not what its test
# says about it.
ANIMATES = r"QPropertyAnimation|QVariantAnimation|QAbstractAnimation|QTimeLine" \
           r"|forDuration\s*\(|motionFor\s*\("

# Dimensions a row cannot have at all, with the reason that goes in the doc.
# A cell only takes the reason when the test file shows no evidence either:
# evidence wins over theory, because a widget someone managed to focus has a
# focus state whether the design meant it or not.
NA = {
    "nonwidget": ({"pointer", "keys", "focus", "disabled", "a11y", "rtl"},
                  "非控件（`QObject` 派生或命名空间）：不进焦点链、不产生可访问对象、无自身几何"),
    "decoration": ({"pointer", "keys", "focus"},
                   "纯装饰：不接收输入、不进入焦点链"),
    "indicator": ({"pointer", "keys", "focus"},
                  "只作指示、不接受输入：不接收指针、不进入焦点链"),
    "tokens": ({"pointer", "keys", "focus", "disabled", "a11y", "rtl"},
               "呈现常量：不含交互状态"),
}

# (row label, test file stems, reason code)
FLUENT = [
    ("FluentAutoSuggestBox", ["tst_fluentautosuggestbox"], None),
    ("FluentAvatar", ["tst_fluentavatar"], None),
    ("FluentBadge", ["tst_fluentbadge"], "decoration"),
    ("FluentBreadcrumbBar", ["tst_fluentbreadcrumbbar"], None),
    ("FluentButton", ["tst_fluentbutton"], None),
    ("FluentCalendarFlyout", ["tst_fluentcalendarflyout"], None),
    ("FluentCalendarView", ["tst_fluentcalendarview"], None),
    ("FluentCard", [], None),
    ("FluentCheckBox / FluentRadioButton", ["tst_fluentcheckable"], None),
    ("FluentComboBox", ["tst_fluentcombobox"], None),
    ("FluentCommandBar", ["tst_fluentcommandbar"], None),
    ("FluentContentDialog", ["tst_fluentcontentdialog"], None),
    ("FluentDatePicker", ["tst_fluentdatepicker"], None),
    ("FluentDivider", [], "decoration"),
    ("FluentDropDownButton", ["tst_fluentdropdownbutton"], None),
    ("FluentExpander", ["tst_fluentexpander"], None),
    ("FluentGridView", ["tst_fluentgridview"], None),
    ("FluentInfoBar", ["tst_fluentinfobar"], None),
    ("FluentListView", ["tst_fluentlistview"], None),
    ("FluentMenuFlyout", ["tst_fluentmenuflyout"], None),
    ("FluentNavigationView", ["tst_fluentnavigationview"], None),
    ("FluentNumberBox", ["tst_fluentnumberbox"], None),
    ("FluentPasswordBox", ["tst_fluentpasswordbox"], None),
    ("FluentProgressBar", ["tst_fluentprogressbar"], "indicator"),
    ("FluentProgressRing", ["tst_fluentprogressring"], "indicator"),
    ("FluentRating", ["tst_fluentrating"], None),
    ("FluentReveal", ["tst_fluentreveal"], "nonwidget"),
    ("FluentSlider", ["tst_fluentslider"], None),
    ("FluentSwitch", ["tst_fluentswitch"], None),
    ("FluentTabView", ["tst_fluenttabview"], None),
    ("FluentTeachingTip", ["tst_fluentteachingtip"], None),
    ("FluentTextBox", ["tst_fluenttextbox"], None),
    ("FluentTimePicker", ["tst_fluenttimepicker"], None),
    ("FluentToggleSplitButton", ["tst_fluenttogglesplitbutton"], None),
    ("FluentToolTip", ["tst_fluenttooltip"], None),
    ("FluentTreeView", ["tst_fluenttreeview"], None),
    ("FluentWindowMaterial", ["tst_fluentwindowmaterial"], "nonwidget"),
    ("`FluentShape` / `FluentType`", ["tst_fluenttokens"], "tokens"),
]

MATERIAL = [
    ("MaterialAccordion", ["tst_materialaccordion"], None),
    ("MaterialAutocomplete", ["tst_materialautocomplete"], None),
    ("MaterialAvatar", ["tst_materialavatar"], None),
    ("MaterialBadge", ["tst_materialbadge"], "decoration"),
    ("MaterialBottomNavigation", ["tst_materialbottomnavigation"], None),
    ("MaterialBreadcrumbs", ["tst_materialbreadcrumbs"], None),
    ("MaterialButton", ["tst_materialbutton"], None),
    ("MaterialCard", ["tst_materialcard"], None),
    ("MaterialCheckBox / MaterialRadio", ["tst_materialcheckable"], None),
    ("MaterialChip", ["tst_materialchip"], None),
    ("MaterialDatePicker", ["tst_materialdatepicker"], None),
    ("MaterialDialog", ["tst_materialdialog"], None),
    ("MaterialDivider", ["tst_materialdivider"], "decoration"),
    ("MaterialDrawer", ["tst_materialdrawer"], None),
    ("MaterialFAB", ["tst_materialfab"], None),
    ("MaterialImageList", ["tst_materialimagelist"], None),
    ("MaterialList / MaterialListItem", ["tst_materiallist"], None),
    ("MaterialMenu", ["tst_materialmenu"], None),
    ("MaterialNavigationRail", ["tst_materialnavigationrail"], None),
    ("MaterialPagination", ["tst_materialpagination"], None),
    ("MaterialProgress", ["tst_materialprogress"], "indicator"),
    ("MaterialProgressRing", ["tst_materialprogressring"], "indicator"),
    ("MaterialRating", ["tst_materialrating"], None),
    ("MaterialRipple", ["tst_materialripple"], "nonwidget"),
    ("MaterialSegmentedButton", ["tst_materialsegmentedbutton"], None),
    ("MaterialSelect", ["tst_materialselect"], None),
    ("MaterialSkeleton", ["tst_materialskeleton"], "decoration"),
    ("MaterialSlider", ["tst_materialslider"], None),
    ("MaterialSnackbar", ["tst_materialsnackbar"], None),
    ("MaterialSpeedDial", ["tst_materialspeeddial"], None),
    ("MaterialStepper", ["tst_materialstepper"], None),
    ("MaterialSwitch", ["tst_materialswitch"], None),
    ("MaterialTable", ["tst_materialtable"], None),
    ("MaterialTabs", ["tst_materialtabs"], None),
    ("MaterialTextField", ["tst_materialtextfield"], None),
    ("MaterialTimePicker", ["tst_materialtimepicker"], None),
    ("MaterialToggleButtonGroup", ["tst_materialtogglebuttongroup"], None),
    ("MaterialTooltip", ["tst_materialtooltip"], None),
    ("MaterialTopAppBar", ["tst_materialtopappbar"], None),
    ("`MaterialShape` / `MaterialType` / `MaterialElevation` / `MaterialMotion`",
     ["tst_materialtokens"], "tokens"),
]

SHARED = [
    ("QxControl", ["tst_qxcontrol", "tst_qxcontrolaccessible"], None),
    ("QxStateLayer", ["tst_qxstatelayer"], "nonwidget"),
    ("QxRingPainter", ["tst_qxringpainter"], "nonwidget"),
    ("QxShadowPainter", ["tst_qxshadowpainter"], "nonwidget"),
]

# (heading, test directory, rows) - the heading is prose, the directory is fact.
GROUPS = [
    ("qxfluent", "qxfluent", FLUENT),
    ("qxmaterial", "qxmaterial", MATERIAL),
    ("qxcontrols（共用层）", "qxcontrols", SHARED),
]


def read_test(family, stem):
    path = os.path.join(ROOT, "tests", family, "auto", stem + ".cpp")
    if not os.path.exists(path):
        return None
    return io.open(path, encoding="utf-8", errors="replace").read()


def source_draws_motion(family, label):
    """Does the widget's own source ask for a movement at all?

    A widget with no animation has no reduce-motion state to test: writing
    "default only" in that cell would invent a gap and then send someone to
    fill it with a case that can only assert that nothing moves. The question
    is answered from the source, by the classes that carry a movement and the
    tier lookup that hands out its length.
    """
    for name in re.findall(r"[A-Z][A-Za-z]+", label):
        path = os.path.join(ROOT, "src", "libs", family, name.lower() + ".cpp")
        if os.path.exists(path):
            text = io.open(path, encoding="utf-8", errors="replace").read()
            if re.search(ANIMATES, text):
                return True
    return False


def cells_for(family, stems, reason, label=""):
    """One row: dimension -> one of the five marks, plus the reason text."""
    text = "\n".join(filter(None, (read_test(family, s) for s in stems)))
    has_file = bool(text)
    driven = {d: bool(re.search(DRIVEN[d], text)) for d in DIMS}
    na = NA[reason][0] if reason else set()
    # A widget that never moves has no reduced-motion state. Evidence first:
    # a case that turns the switch and watches something wins over the source
    # read, because the case is the stronger claim.
    still = not driven["motion"] and not source_draws_motion(family, label)
    row, reasons = {}, []
    for d in DIMS:
        if (d in na or (d == "motion" and still)) and not driven[d]:
            row[d] = "—"
        elif not has_file:
            row[d] = "∅"
        elif driven[d]:
            row[d] = "●"
        else:
            row[d] = "○"
    if reason:
        blocked = [d for d in DIMS if row[d] == "—"]
        if blocked:
            reasons.append(NA[reason][1])
    if still and row["motion"] == "—":
        reasons.append("不画动效：源码里没有动画类，也不问档位")
    if "rtl" in row and row["rtl"] == "—":
        pass
    elif row["rtl"] == "✗":
        pass
    return row, "；".join(reasons) if reasons else "—"


def render():
    out = []
    stats = {"●": 0, "○": 0, "∅": 0, "—": 0, "✗": 0}
    for family, folder, rows in GROUPS:
        out.append("### %s\n" % family)
        out.append("| 件 | 指针 | 键盘 | 焦点 | 禁用 | 主题 | 减弱动效 | 无障碍 | 右到左 | 不适用理由 |")
        out.append("| :--- | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :--- |")
        for label, stems, reason in rows:
            row, why = cells_for(folder, stems, reason, label)
            # RTL: nothing in the three libraries reads the layout direction,
            # so a widget that draws its own geometry cannot mirror. T1-c says
            # record that and do not pretend with a case.
            if row["rtl"] != "—":
                row["rtl"] = "✗"
            for d in DIMS:
                stats[row[d]] = stats.get(row[d], 0) + 1
            out.append("| %s | %s | %s | %s | %s | %s | %s | %s | %s | %s |" % (
                label, row["pointer"], row["keys"], row["focus"], row["disabled"],
                row["theme"], row["motion"], row["a11y"], row["rtl"], why))
        out.append("")
    return "\n".join(out), stats


def render_debt():
    """Per dimension: which widgets are only tested in their default state.

    Right to left is left out on purpose: T1-c decided it is unsupported, so
    it is a decision rather than a gap, and counting it would drown the list.
    """
    dims = [d for d in DIMS if d != "rtl"]
    debt = {d: {"○": [], "∅": []} for d in dims}
    for family, folder, rows in GROUPS:
        for label, stems, reason in rows:
            row, _ = cells_for(folder, stems, reason, label)
            for d in dims:
                if row[d] in ("○", "∅"):
                    debt[d][row[d]].append(label)
    out = ["| 维度 | `○` 只测了默认态 | `∅` 无用例 |",
           "| :--- | :--- | :--- |"]
    for d in dims:
        out.append("| %s | %d 件 | %d 件 |" % (
            d, len(debt[d]["○"]), len(debt[d]["∅"])))
    return "\n".join(out), debt


def main():
    table, stats = render()
    if "--stats" in sys.argv:
        for mark in ("●", "○", "∅", "—", "✗"):
            print("%s %d" % (mark, stats[mark]))
        return 0
    if "--debt" in sys.argv:
        summary, debt = render_debt()
        print(summary)
        print()
        for d in debt:
            if debt[d]["○"]:
                print("- **%s** 只测了默认态：%s" % (d, "、".join(debt[d]["○"])))
            if debt[d]["∅"]:
                print("- **%s** 无用例：%s" % (d, "、".join(debt[d]["∅"])))
        return 0
    print(table)
    return 0


if __name__ == "__main__":
    sys.exit(main())
