/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "otherpage.h"

#include "qxribbon/ribbonbar.h"
#include "qxribbon/ribbonbuttongroup.h"
#include "qxribbon/ribboncontainers.h"
#include "qxribbon/ribboncontrols.h"
#include "qxribbon/ribbongallery.h"
#include "qxribbon/ribbongallerygroup.h"
#include "qxribbon/ribbongroup.h"

#include "qxtheme/qxthemeicon.h"

#include <QAction>
#include <QCalendarWidget>
#include <QComboBox>
#include <QDebug>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QRadioButton>
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

OtherPage::OtherPage(QxThemeIcons *icons, QWidget *parent)
    : RibbonPage(parent)
    , m_icons(icons)
{
    setPageName(tr("&Other"));

    // group 1: a button group and a gallery.
    RibbonGroup *group1 = addGroup(tr("button group and gallery"));

    RibbonButtonGroup *btnGroup1 = new RibbonButtonGroup(group1);
    btnGroup1->addAction(createAction(tr("Copy"), QStringLiteral("copy")));
    btnGroup1->addAction(createAction(tr("Cut"), QStringLiteral("scissors")));
    btnGroup1->addAction(createAction(tr("Paste"), QStringLiteral("clipboard")));
    QAction *printerAction = createAction(tr("Printer"), QStringLiteral("printer"));
    QMenu *printerMenu = new QMenu(this);
    printerMenu->addAction(tr("test1"));
    printerMenu->addAction(tr("test2"));
    printerAction->setMenu(printerMenu);
    btnGroup1->addAction(printerAction);
    group1->addWidget(btnGroup1, RibbonGroup::Medium);

    RibbonButtonGroup *btnGroup2 = new RibbonButtonGroup(group1);
    QAction *titleAlgnment = createAction(tr("Title Align Left"), QStringLiteral("align-left"));
    titleAlgnment->setProperty("align", (int)Qt::AlignLeft | Qt::AlignVCenter);
    btnGroup2->addAction(titleAlgnment);
    titleAlgnment = createAction(tr("Title Align Center"), QStringLiteral("align-center"));
    titleAlgnment->setProperty("align", (int)Qt::AlignCenter);
    btnGroup2->addAction(titleAlgnment);
    titleAlgnment = createAction(tr("Title Align Right"), QStringLiteral("align-right"));
    titleAlgnment->setProperty("align", (int)Qt::AlignRight | Qt::AlignVCenter);
    btnGroup2->addAction(titleAlgnment);
    group1->addWidget(btnGroup2, RibbonGroup::Medium);
    connect(btnGroup2, &RibbonButtonGroup::actionTriggered, this, [this](QAction *act) {
        const QVariant v = act->property("align");
        if (!v.isValid()) {
            return;
        }
        RibbonBar *bar = ribbonBar();
        if (!bar) {
            return;
        }
        bar->setWindowTitleAligment(static_cast<Qt::Alignment>(v.toInt()));
        bar->repaint();
    });

    // Gallery: the shapes a drawing program offers, two groups of them.
    RibbonGallery *gallery = group1->addGallery();
    auto makeGalleryAction = [this](const QString &text, const QString &glyph) -> QAction * {
        QAction *act = createAction(text, glyph);
        connect(act, &QAction::triggered, this, [this, text]() {
            Q_EMIT messageRequested(text);
        });
        return act;
    };

    QList<QAction *> galleryActions;
    galleryActions.append(makeGalleryAction(tr("Square"), QStringLiteral("square")));
    galleryActions.append(makeGalleryAction(tr("Circle"), QStringLiteral("circle")));
    galleryActions.append(makeGalleryAction(tr("Triangle"), QStringLiteral("triangle")));
    galleryActions.append(makeGalleryAction(tr("Hexagon"), QStringLiteral("hexagon")));
    galleryActions.append(makeGalleryAction(tr("Pentagon"), QStringLiteral("pentagon")));
    galleryActions.append(makeGalleryAction(tr("Octagon"), QStringLiteral("octagon")));
    galleryActions.append(makeGalleryAction(tr("Star"), QStringLiteral("star")));
    galleryActions.append(makeGalleryAction(tr("Heart"), QStringLiteral("heart")));
    galleryActions.append(makeGalleryAction(tr("Diamond"), QStringLiteral("diamond")));
    galleryActions.append(makeGalleryAction(tr("Plus"), QStringLiteral("circle-plus")));
    galleryActions.append(makeGalleryAction(tr("Minus"), QStringLiteral("circle-minus")));
    RibbonGalleryGroup *galleryGroup1 = gallery->addGalleryGroup(tr("Shapes"), galleryActions);
    galleryGroup1->setGalleryGroupStyle(RibbonGalleryGroup::IconWithWordWrapText);
    galleryGroup1->setGridMinimumWidth(80);

    galleryActions.clear();
    galleryActions.append(makeGalleryAction(tr("Arrow Up"), QStringLiteral("arrow-up")));
    galleryActions.append(makeGalleryAction(tr("Arrow Right"), QStringLiteral("arrow-right")));
    galleryActions.append(makeGalleryAction(tr("Arrow Down"), QStringLiteral("arrow-down")));
    galleryActions.append(makeGalleryAction(tr("Arrow Left"), QStringLiteral("arrow-left")));
    galleryActions.append(makeGalleryAction(tr("Line"), QStringLiteral("line")));
    galleryActions.append(makeGalleryAction(tr("Maximize"), QStringLiteral("arrows-maximize")));
    RibbonGalleryGroup *galleryGroup2 = gallery->addGalleryGroup(tr("Arrows"), galleryActions);
    galleryGroup2->setGridMinimumWidth(80);
    gallery->setCurrentViewGroup(galleryGroup1);

    QAction *optAct = new QAction(this);
    optAct->setObjectName(QStringLiteral(u"debug"));
    group1->addOptionAction(optAct);

    // group 2: the widget menagerie.
    RibbonGroup *group2 = addGroup(tr("widget test"));

    RibbonComboBox *com = new RibbonComboBox(this);
    com->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    com->setWindowTitle(tr("RibbonComboBox Editable"));
    for (int i = 0; i < 15; ++i) {
        com->addItem(QStringLiteral("RibbonComboBox test%1").arg(i + 1));
    }
    com->setEditable(true);
    group2->addSmallWidget(com);

    com = new RibbonComboBox(this);
    com->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    com->setWindowTitle(tr("RibbonComboBox"));
    for (int i = 0; i < 15; ++i) {
        com->addItem(QStringLiteral("item %1").arg(i + 1));
    }
    group2->addSmallWidget(com);

    RibbonLineEdit *lineEdit = new RibbonLineEdit(this);
    lineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    lineEdit->setText(tr("RibbonLineEdit"));
    lineEdit->setWindowTitle(lineEdit->text());
    group2->addSmallWidget(lineEdit);

    RibbonCheckBox *checkBox = new RibbonCheckBox(this);
    checkBox->setText(tr("RibbonCheckBox"));
    checkBox->setWindowTitle(checkBox->text());
    group2->addSmallWidget(checkBox);

    QRadioButton *radioButton = new QRadioButton(this);
    radioButton->setText(tr("QRadioButton"));
    radioButton->setWindowTitle(radioButton->text());
    group2->addSmallWidget(radioButton);

    QSpinBox *spinBox = new QSpinBox(this);
    spinBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    group2->addSmallWidget(spinBox);

    group2->addSeparator();

    QCalendarWidget *calendarWidget = new QCalendarWidget(this);
    calendarWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    calendarWidget->setObjectName(QStringLiteral(u"calendarWidget"));
    group2->addLargeWidget(calendarWidget);

    optAct = new QAction(this);
    connect(optAct, &QAction::triggered, this, [this](bool on) {
        Q_UNUSED(on);
        QMessageBox::information(this, tr("Option Action Triggered"), tr("Option Action Triggered"));
    });
    group2->addOptionAction(optAct);

    // group: the title flips between two languages.
    RibbonGroup *groupUtf8 = addGroup(QStringLiteral(u"中文显示测试"));
    QAction *actionChangeText = createAction(QStringLiteral(u"改变显示为英文"), QStringLiteral("language"),
                                             QStringLiteral(u"actionChangeText"));
    actionChangeText->setCheckable(true);
    actionChangeText->setChecked(false);
    groupUtf8->addLargeAction(actionChangeText);
    connect(actionChangeText, &QAction::triggered, this, [groupUtf8, actionChangeText](bool on) {
        if (on) {
            groupUtf8->setGroupName(QStringLiteral(u"show chinese char"));
            actionChangeText->setText(QStringLiteral(u"show in chinese"));
        } else {
            groupUtf8->setGroupName(QStringLiteral(u"中文显示测试"));
            actionChangeText->setText(QStringLiteral(u"改变显示为英文"));
        }
    });
}

QAction *OtherPage::createAction(const QString &text, const QString &glyph, const QString &objName)
{
    QAction *action = new QAction(text, this);
    action->setObjectName(objName.isEmpty() ? text : objName);

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
