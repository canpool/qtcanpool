/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "applypage.h"

#include "qxribbon/ribbonbuttongroup.h"
#include "qxribbon/ribbongroup.h"

#include "qxtheme/qxthemeicon.h"

#include <QAction>
#include <QFontComboBox>
#include <QLabel>

QX_THEME_USE_NAMESPACE

namespace
{

/*! Where the demo keeps the single-colour glyphs it repaints from the palette. */
QString glyphPath(const QString &name)
{
    return QStringLiteral(":/icon/res/tabler/") + name + QStringLiteral(".svg");
}

}   // namespace

ApplyPage::ApplyPage(QxThemeIcons *icons, QWidget *parent)
    : RibbonPage(parent)
    , m_icons(icons)
{
    setPageName(tr("&Apply(example long page)"));
    setObjectName(QStringLiteral("pageApply"));

    RibbonGroup *group = addGroup(tr("Font"));

    RibbonButtonGroup *buttonGroup = new RibbonButtonGroup(group);
    buttonGroup->setObjectName(QStringLiteral("group1"));
    QLabel *labelFontSize = new QLabel(buttonGroup);
    labelFontSize->setText(tr("select font"));
    labelFontSize->setObjectName(QStringLiteral("labelFontSize"));
    buttonGroup->addWidget(labelFontSize);
    QFontComboBox *fontComWidget = new QFontComboBox(buttonGroup);
    fontComWidget->setObjectName(QStringLiteral("fontComboBox"));
    connect(fontComWidget, &QFontComboBox::currentFontChanged, this, &ApplyPage::fontSelected);
    buttonGroup->addWidget(fontComWidget);
    QAction *act = group->addWidget(buttonGroup, RibbonGroup::Medium);
    act->setObjectName(labelFontSize->objectName());

    RibbonButtonGroup *alignmentGroup = new RibbonButtonGroup(group);
    alignmentGroup->setObjectName(QStringLiteral("group2"));
    alignmentGroup->addAction(createAction(tr("Bold"), QStringLiteral("bold")));
    alignmentGroup->addAction(createAction(tr("Italic"), QStringLiteral("italic")));
    alignmentGroup->addSeparator();
    alignmentGroup->addAction(createAction(tr("left alignment"), QStringLiteral("align-left")));
    alignmentGroup->addAction(createAction(tr("center alignment"), QStringLiteral("align-center")));
    alignmentGroup->addAction(createAction(tr("right alignment"), QStringLiteral("align-right")));
    alignmentGroup->addAction(createAction(tr("line up on both sides"), QStringLiteral("align-justified")));
    act = group->addWidget(alignmentGroup, RibbonGroup::Medium);
    act->setObjectName(alignmentGroup->objectName());

    group->addSeparator();

    QAction *actLargerFontSize = createAction(tr("Larger"), QStringLiteral("text-increase"));
    actLargerFontSize->setObjectName(QStringLiteral("actLargerFontSize"));
    group->addLargeAction(actLargerFontSize);

    QAction *actSmallFontSize = createAction(tr("Smaller"), QStringLiteral("text-decrease"));
    actSmallFontSize->setObjectName(QStringLiteral("actSmallFontSize"));
    group->addLargeAction(actSmallFontSize);

    connect(actLargerFontSize, &QAction::triggered, this, &ApplyPage::largerFontRequested);
    connect(actSmallFontSize, &QAction::triggered, this, &ApplyPage::smallerFontRequested);
}

QAction *ApplyPage::createAction(const QString &text, const QString &glyph)
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
