/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "contextpage1.h"

#include "qxribbon/ribbonbar.h"
#include "qxribbon/ribboncontainers.h"
#include "qxribbon/ribbongroup.h"

#include "qxtheme/qxthemeicon.h"

#include <QAction>
#include <QLineEdit>
#include <QSpinBox>

QX_THEME_USE_NAMESPACE

namespace
{

/*! Where the demo keeps the single-colour glyphs it repaints from the palette. */
QString glyphPath(const QString &name)
{
    return QStringLiteral(":/icon/res/tabler/") + name + QStringLiteral(".svg");
}

}   // namespace

ContextPage1::ContextPage1(QxThemeIcons *icons, QWidget *parent)
    : RibbonPage(parent)
    , m_icons(icons)
{
    RibbonGroup *group = addGroup(tr("show and hide test"));

    QAction *actionHideGroup = createAction(tr("hide group"), QStringLiteral("eye-off"));
    actionHideGroup->setCheckable(true);
    group->addLargeAction(actionHideGroup);

    QAction *actionDisable = createAction(tr("Disable"), QStringLiteral("ban"));
    actionDisable->setDisabled(true);
    group->addLargeAction(actionDisable);
    connect(actionDisable, &QAction::triggered, this, [actionDisable](bool b) {
        Q_UNUSED(b);
        actionDisable->setDisabled(true);
    });

    QAction *actionUnlock = createAction(tr("unlock"), QStringLiteral("lock-open"));
    actionUnlock->setShortcut(QKeySequence(QLatin1String("Ctrl+E")));
    group->addLargeAction(actionUnlock);
    connect(actionUnlock, &QAction::triggered, this, [actionDisable](bool b) {
        Q_UNUSED(b);
        actionDisable->setEnabled(true);
        actionDisable->setText(tr("Enabled"));
    });

    QAction *actionSetTextTest = createAction(tr("set text"), QStringLiteral("pencil"));
    actionSetTextTest->setCheckable(true);
    actionSetTextTest->setShortcut(QKeySequence(QLatin1String("Ctrl+D")));
    group->addLargeAction(actionSetTextTest);
    connect(actionSetTextTest, &QAction::toggled, this, [actionSetTextTest](bool b) {
        if (b) {
            actionSetTextTest->setText(QStringLiteral(u"setText测试"));
        } else {
            actionSetTextTest->setText(QStringLiteral(u"set text"));
        }
    });

    // hide the group
    QAction *actionShowTest = createAction(tr("show beside group"), QStringLiteral("eye"));
    actionShowTest->setCheckable(true);
    group->addLargeAction(actionShowTest);

    RibbonGroup *group2 = addGroup(tr("show/hide"));
    group2->addLargeAction(actionSetTextTest);
    connect(actionShowTest, &QAction::toggled, this, [actionShowTest, group2, this](bool b) {
        group2->setVisible(!b);
        if (b) {
            actionShowTest->setText(tr("hide beside group"));
        } else {
            actionShowTest->setText(tr("show beside group"));
        }
        RibbonBar *bar = ribbonBar();
        if (bar) {
            bar->repaint();
        }
    });

    RibbonGroup *group3 = addGroup(tr("show/hide action test"));

    QAction *actionHideAction2 = createAction(tr("hide action 2"), QStringLiteral("square"));
    QAction *actionHideAction4 = createAction(tr("hide action 4"), QStringLiteral("square"));
    actionHideAction2->setCheckable(true);
    actionHideAction2->setChecked(true);
    actionHideAction4->setCheckable(true);
    actionHideAction4->setChecked(true);
    QAction *act2 = createAction(tr("action2"), QStringLiteral("circle"));
    QAction *act3 = createAction(tr("action3"), QStringLiteral("triangle"));
    QAction *act4 = createAction(tr("action4"), QStringLiteral("square"));
    group3->addLargeAction(actionHideAction2);
    group3->addLargeAction(actionHideAction4);
    group3->addSmallAction(act2);
    group3->addSmallAction(act3);
    group3->addSmallAction(act4);
    connect(actionHideAction2, &QAction::triggered, this, [actionHideAction2, act2](bool b) {
        if (b) {
            act2->setVisible(true);
            actionHideAction2->setText(tr("hide action2"));
        } else {
            act2->setVisible(false);
            actionHideAction2->setText(tr("show action2"));
        }
    });
    connect(actionHideAction4, &QAction::triggered, this, [actionHideAction4, act4](bool b) {
        if (b) {
            act4->setVisible(true);
            actionHideAction4->setText(tr("hide action4"));
        } else {
            act4->setVisible(false);
            actionHideAction4->setText(tr("show action4"));
        }
    });

    RibbonGroup *group4 = addGroup(tr("widget"));
    QSpinBox *spinbox = new QSpinBox(this);
    RibbonCtrlContainer *ctrlContainer1 = new RibbonCtrlContainer(group4);
    ctrlContainer1->setWidget(spinbox);
    ctrlContainer1->setText(tr("spinbox:"));
    ctrlContainer1->setIconVisible(false);
    group4->addMediumWidget(ctrlContainer1);

    QLineEdit *linedit = new QLineEdit(this);
    RibbonCtrlContainer *ctrlContainer2 = new RibbonCtrlContainer(group4);
    ctrlContainer2->setWidget(linedit);
    ctrlContainer2->setText(tr("linedit:"));
    ctrlContainer2->setIconVisible(false);
    group4->addMediumWidget(ctrlContainer2);
}

QAction *ContextPage1::createAction(const QString &text, const QString &glyph)
{
    QAction *action = new QAction(text, this);
    action->setObjectName(text);

    const QString path = glyphPath(glyph);
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
