/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "custompage.h"

#include "qxribbon/ribbonbar.h"
#include "qxribbon/ribbongroup.h"
#include "qxribbon/ribbonquickaccessbar.h"

#include "qxtheme/qxthemeicon.h"

#include <QAction>

QX_THEME_USE_NAMESPACE

namespace
{

/*! Where the demo keeps the single-colour glyphs it repaints from the palette. */
QString glyphPath(const QString &name)
{
    return QStringLiteral(":/icon/res/tabler/") + name + QStringLiteral(".svg");
}

}   // namespace

CustomPage::CustomPage(QxThemeIcons *icons, QWidget *parent)
    : RibbonPage(parent)
    , m_icons(icons)
{
    setObjectName(QStringLiteral("pageCustom"));

    RibbonGroup *group = new RibbonGroup(tr("quick access bar"));

    QAction *addAct = createAction(tr("add action"), QStringLiteral("circle-plus"));
    QAction *removeAct = createAction(tr("remove action"), QStringLiteral("circle-minus"));
    group->addLargeAction(addAct);
    group->addLargeAction(removeAct);
    removeAct->setDisabled(true);

    QAction *testAct = createAction(tr("custom action"), QStringLiteral("star"));
    // The action has no ribbon of its own to sit on before it is added, so this
    // page keeps owning it - including its icon.
    connect(addAct, &QAction::triggered, this, [this, addAct, removeAct, testAct]() {
        RibbonBar *bar = ribbonBar();
        if (!bar || !bar->quickAccessBar()) {
            return;
        }
        bar->quickAccessBar()->addAction(testAct);
        bar->resizeRibbon();
        addAct->setDisabled(true);
        removeAct->setDisabled(false);
    });
    connect(removeAct, &QAction::triggered, this, [this, addAct, removeAct, testAct]() {
        RibbonBar *bar = ribbonBar();
        if (!bar || !bar->quickAccessBar()) {
            return;
        }
        bar->quickAccessBar()->removeAction(testAct);
        bar->resizeRibbon();
        addAct->setDisabled(false);
        removeAct->setDisabled(true);
    });
    addGroup(group);

    RibbonGroup *groupStyle = addGroup(tr("config file"));

    QAction *actionUseQss = createAction(tr("use qss"), QStringLiteral("paint"));
    connect(actionUseQss, &QAction::triggered, this, &CustomPage::useQssRequested);
    groupStyle->addSmallAction(actionUseQss);

    QAction *actionLoadCustomizeXmlFile =
        createAction(tr("load customize from xml file"), QStringLiteral("file-import"));
    connect(actionLoadCustomizeXmlFile, &QAction::triggered, this, &CustomPage::loadCustomizeRequested);
    groupStyle->addSmallAction(actionLoadCustomizeXmlFile);
}

QAction *CustomPage::createAction(const QString &text, const QString &glyph)
{
    QAction *action = new QAction(text, this);
    // The object name is the text unless the caller named the action itself,
    // which is what the customization file addresses it by.
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
