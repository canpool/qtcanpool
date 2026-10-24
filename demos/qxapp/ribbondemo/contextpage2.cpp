/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "contextpage2.h"

#include "icons.h"
#include "qxribbon/ribbonbar.h"
#include "qxribbon/ribbongroup.h"

#include "qxtheme/qxthemeicon.h"

#include <QAction>
#include <QHash>
#include <QToolButton>

QX_THEME_USE_NAMESPACE

namespace
{

/*!
 * One button of the layout group: the row proportion it takes, and whether the
 * group's next separator comes after it.
 */
struct LayoutItem {
    RibbonGroup::RowProportion rp;
    bool separatorAfter;
};

/*!
 * What this group shows is the sequence of row proportions, so that sequence -
 * not twenty calls - is what the source spells out.
 */
const LayoutItem layoutItems[] = {
    {RibbonGroup::Large, false},  {RibbonGroup::Small, false},  {RibbonGroup::Small, true},
    {RibbonGroup::Small, false},  {RibbonGroup::Small, false},  {RibbonGroup::Small, false},
    {RibbonGroup::Small, true},   {RibbonGroup::Large, false},  {RibbonGroup::Medium, false},
    {RibbonGroup::Medium, false}, {RibbonGroup::Small, false},  {RibbonGroup::Medium, false},
    {RibbonGroup::Large, false},  {RibbonGroup::Medium, false}, {RibbonGroup::Medium, false},
    {RibbonGroup::Large, true},   {RibbonGroup::Medium, false}, {RibbonGroup::Large, false},
    {RibbonGroup::Medium, false}, {RibbonGroup::Small, false},
};

/*! How each row proportion names itself, in the action's object name. */
QString proportionName(RibbonGroup::RowProportion rp)
{
    switch (rp) {
    case RibbonGroup::Large:
        return QStringLiteral("Large");
    case RibbonGroup::Medium:
        return QStringLiteral("Medium");
    case RibbonGroup::Small:
        return QStringLiteral("Small");
    default:
        return QStringLiteral("Auto");
    }
}

}   // namespace

ContextPage2::ContextPage2(QxThemeIcons *icons, QWidget *parent)
    : RibbonPage(parent)
    , m_icons(icons)
{
    RibbonGroup *group = addGroup(QStringLiteral(u"删除PagePage测试"));

    QAction *actionDeleteThisPage = createAction(tr("delete this page"), QStringLiteral("trash"));
    group->addLargeAction(actionDeleteThisPage);
    connect(actionDeleteThisPage, &QAction::triggered, this, [this]() {
        RibbonBar *bar = ribbonBar();
        if (bar) {
            bar->removePage(this);
        }
        deleteLater();
    });

    RibbonGroup *groupLayout = addGroup(QStringLiteral(u"特殊布局"));

    // The buttons say nothing but their size, and the customization file
    // addresses them by name, so the name carries both.
    QHash<QString, int> seen;
    for (const LayoutItem &item : layoutItems) {
        const QString name = proportionName(item.rp);
        const QString objName = QLatin1Char('@') + name + QString::number(++seen[name]);
        QAction *action = createAction(name, QStringLiteral("file"), objName);
        groupLayout->addAction(action, QToolButton::InstantPopup, item.rp);
        if (item.separatorAfter) {
            groupLayout->addSeparator();
        }
    }
}

QAction *ContextPage2::createAction(const QString &text, const QString &glyph, const QString &objName)
{
    QAction *action = new QAction(text, this);
    // The object name is the text unless the caller named the action itself,
    // which is what the customization file addresses it by.
    action->setObjectName(objName.isEmpty() ? text : objName);

    const QString path = Icons::file(glyph);
    if (m_icons) {
        // Handing the action over is what keeps the glyph following the theme:
        // the icon is painted from the palette now, and painted again whenever
        // the palette is replaced.
        m_icons->add(action, path, QPalette::ButtonText, QSize(24, 24));
    } else {
        action->setIcon(QIcon(path));
    }
    return action;
}
