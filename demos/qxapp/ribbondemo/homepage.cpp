/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "homepage.h"

#include "qxribbon/ribbonapplicationbutton.h"
#include "qxribbon/ribbonbar.h"
#include "qxribbon/ribbonbuttongroup.h"
#include "qxribbon/ribboncolorbutton.h"
#include "qxribbon/ribboncontrols.h"
#include "qxribbon/ribbongroup.h"
#include "qxribbon/ribbonmenu.h"
#include "qxribbon/ribbonutils.h"

#include "qxtheme/qxthemeicon.h"

#include <QAbstractButton>
#include <QButtonGroup>
#include <QCheckBox>
#include <QColor>
#include <QEvent>
#include <QIcon>
#include <QMessageBox>
#include <QRadioButton>

QX_THEME_USE_NAMESPACE

namespace
{

/*! Where the demo keeps the single-colour glyphs it repaints from the palette. */
QString glyphPath(const QString &name)
{
    return QStringLiteral(":/icon/res/tabler/") + name + QStringLiteral(".svg");
}

}   // namespace

HomePage::HomePage(QxThemeIcons *icons, QWidget *parent)
    : RibbonPage(parent)
    , m_icons(icons)
    , m_hideRibbonAction(Q_NULLPTR)
    , m_showMinimumButtonAction(Q_NULLPTR)
    , m_styleGroup(Q_NULLPTR)
    , m_fullScreenCheck(Q_NULLPTR)
    , m_barAttached(false)
{
    setPageName(tr("&Home"));
    setObjectName(QStringLiteral(u"pageHome"));

    // group 1: the ribbon's own switches.
    RibbonGroup *groupStyle = addGroup(tr("ribbon style"));

    QAction *actHideRibbon =
        createAction(tr("hide ribbon"), QStringLiteral("square-minus"), QStringLiteral(u"actHideRibbon"));
    actHideRibbon->setCheckable(true);
    actHideRibbon->setStatusTip(tr("Collapse the ribbon down to just the tab row"));
    groupStyle->addLargeAction(actHideRibbon);
    connect(actHideRibbon, &QAction::triggered, this, [this](bool b) {
        ribbonBar()->setMinimized(b);
    });

    QAction *actShowMinimumButton =
        createAction(tr("show minimum button"), QStringLiteral("eye"), QStringLiteral(u"show minimum button"));
    actShowMinimumButton->setCheckable(true);
    groupStyle->addSmallAction(actShowMinimumButton);
    connect(actShowMinimumButton, &QAction::triggered, this, [this](bool b) {
        ribbonBar()->showMinimumButton(b);
    });
    // These two still owe the bar a visit once it exists; see attachToBar().
    m_hideRibbonAction = actHideRibbon;
    m_showMinimumButtonAction = actShowMinimumButton;

    QAction *actRemoveAppBtn = createAction(tr("remove application"), QStringLiteral("app-window"));
    actRemoveAppBtn->setCheckable(true);
    groupStyle->addSmallAction(actRemoveAppBtn);
    connect(actRemoveAppBtn, &QAction::toggled, this, [this](bool b) {
        if (b) {
            ribbonBar()->setApplicationButton(nullptr);
        } else {
            // B4: the Backstage view owns the application-button click through
            // RibbonBar::applicationButtonClicked, so the button only needs to
            // exist with its label - no popup.
            RibbonApplicationButton *btn = new RibbonApplicationButton();
            btn->setText(tr("&File"));
            btn->setCheckable(true);
            ribbonBar()->setApplicationButton(btn);
        }
    });

    QAction *actWindowFlagNormalButton = createAction(tr("window maximizable"), QStringLiteral("maximize"));
    actWindowFlagNormalButton->setCheckable(true);
    actWindowFlagNormalButton->setChecked(true);
    groupStyle->addSmallAction(actWindowFlagNormalButton);
    connect(actWindowFlagNormalButton, &QAction::triggered, this, [this](bool b) {
        QWidget *w = window();
        if (!w) {
            return;
        }
        if (b) {
            w->setWindowFlags(w->windowFlags() | Qt::WindowMaximizeButtonHint);
        } else {
            w->setWindowFlags(w->windowFlags() & ~Qt::WindowMaximizeButtonHint);
        }
    });
    // NOTE: this was once disabled on Windows unless the window came from qxapp: the frameless
    // backend qxribbon carried itself left the window misplaced and unresizable after a
    // maximize/unmaximize toggle. Both that backend and the window class on top of it went away in
    // 4.3, so there is nothing left to guard against here.

    QButtonGroup *g = new QButtonGroup(this);

    QRadioButton *r = new QRadioButton(this);
    r->setText(tr("tab bar below"));
    r->setWindowTitle(r->text());
    r->setObjectName(QStringLiteral(u"tab bar below"));
    r->setChecked(true);
    groupStyle->addSmallWidget(r);
    g->addButton(r, RibbonBar::TabBarBelowStyle);

    r = new QRadioButton(this);
    r->setObjectName(QStringLiteral(u"tab bar inline"));
    r->setText(tr("tab bar inline"));
    r->setWindowTitle(r->text());
    r->setChecked(false);
    groupStyle->addSmallWidget(r);
    g->addButton(r, RibbonBar::TabBarInlineStyle);

    r = new QRadioButton(this);
    r->setObjectName(QStringLiteral(u"tab bar below, 2 rows"));
    r->setText(tr("tab bar below, 2 rows"));
    r->setWindowTitle(r->text());
    r->setChecked(false);
    groupStyle->addSmallWidget(r);
    g->addButton(r, RibbonBar::TabBarBelowTwoRowStyle);

    r = new QRadioButton(this);
    r->setObjectName(QStringLiteral(u"tab bar inline, 2 rows"));
    r->setText(tr("tab bar inline, 2 rows"));
    r->setWindowTitle(r->text());
    r->setChecked(false);
    groupStyle->addSmallWidget(r);
    g->addButton(r, RibbonBar::TabBarInlineTwoRowStyle);

    connect(g, static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::idClicked), this, [this](int id) {
        ribbonBar()->setRibbonStyle(static_cast<RibbonBar::RibbonStyle>(id));
    });
    // Follow the bar instead of assuming: the window may set the style itself (on
    // Linux it opens in the inline layout), and the radios have to agree with it.
    m_styleGroup = g;

    QCheckBox *cb = new QCheckBox(this);
    cb->setObjectName(QStringLiteral(u"left QAB in inline style"));
    cb->setText(tr("left QAB in inline style"));
    cb->setWindowTitle(cb->text());
    cb->setChecked(false);
    groupStyle->addSmallWidget(cb);
    connect(cb, &QCheckBox::clicked, this, [this](bool checked) {
        ribbonBar()->setQuickAccessBarPosition(checked ? RibbonBar::QABLeftPosition : RibbonBar::QABRightPosition);
    });

    cb = new QCheckBox(this);
    cb->setObjectName(QStringLiteral(u"center tab bar"));
    cb->setText(tr("center tab bar"));
    cb->setWindowTitle(cb->text());
    cb->setChecked(false);
    groupStyle->addSmallWidget(cb);
    connect(cb, &QCheckBox::clicked, this, [this](bool checked) {
        ribbonBar()->setTabBarPosition(checked ? RibbonBar::TBCenterPosition : RibbonBar::TBLeftPosition);
    });

    cb = new QCheckBox(this);
    cb->setObjectName(QStringLiteral(u"show group title"));
    cb->setText(tr("show group title"));
    cb->setWindowTitle(cb->text());
    cb->setChecked(true);
    groupStyle->addSmallWidget(cb);
    connect(cb, &QCheckBox::clicked, this, [this](bool checked) {
        RibbonGroup::setTitleVisible(checked);
        ribbonBar()->updateRibbonGeometry();
    });

    cb = new QCheckBox(this);
    cb->setObjectName(QStringLiteral(u"show full screen"));
    cb->setText(tr("show full screen"));
    cb->setWindowTitle(cb->text());
    cb->setChecked(false);
    groupStyle->addSmallWidget(cb);
    connect(cb, &QCheckBox::clicked, this, [this](bool checked) {
        QWidget *w = window();
        if (!w) {
            return;
        }
        if (checked) {
            w->showFullScreen();
        } else {
            w->showNormal();
        }
    });
    // The window owns its state, so follow it here instead of asking for a signal.
    m_fullScreenCheck = cb;
    if (QWidget *w = window()) {
        w->installEventFilter(this);
    }

    // group 2: the ribbon tool-button gallery - every popup mode and button type.
    RibbonGroup *groupToolButtonStyle = addGroup(tr("ribbon toolbutton style"));

    RibbonMenu *menu = new RibbonMenu(this);
    QAction *itemAction = nullptr;
    {
        // The entries are built by the same helper as everything else on the page: a menu
        // item is still an action, and a bare QIcon here would be the one thing on the
        // ribbon that stayed black when the theme went dark.
        for (int i = 0; i < 3; ++i) {
            const QString name = tr("item %1").arg(i + 1);
            itemAction = createAction(name, QStringLiteral("file"), name);
            menu->addAction(itemAction);
        }
    }

    QAction *act = createAction(tr("test 1"), QStringLiteral("file"));
    act->setMenu(menu);
    act->setToolTip(tr("use QToolButton::MenuButtonPopup mode"));
    RibbonButton *btn = groupToolButtonStyle->addSmallAction(act);
    btn->setPopupMode(QToolButton::MenuButtonPopup);

    act = createAction(tr("test 2"), QStringLiteral("file"));
    act->setMenu(menu);
    act->setToolTip(tr("use QToolButton::InstantPopup mode"));
    btn = groupToolButtonStyle->addSmallAction(act);
    btn->setPopupMode(QToolButton::InstantPopup);

    groupToolButtonStyle->addSeparator();

    act = createAction(tr("test 3"), QStringLiteral("file"));
    act->setMenu(menu);
    btn = groupToolButtonStyle->addSmallAction(act);
    btn->setToolButtonStyle(Qt::ToolButtonIconOnly);
    act = createAction(tr("test 4"), QStringLiteral("file"));
    groupToolButtonStyle->addSmallAction(act);
    groupToolButtonStyle->ribbonButtonForAction(act)->setToolButtonStyle(Qt::ToolButtonIconOnly);

    groupToolButtonStyle->addSeparator();

    act = createAction(tr("test 5"), QStringLiteral("file"));
    act->setMenu(menu);
    btn = groupToolButtonStyle->addSmallAction(act);
    btn->setToolButtonStyle(Qt::ToolButtonTextOnly);
    act = createAction(tr("test 6"), QStringLiteral("file"));
    groupToolButtonStyle->addSmallAction(act);
    groupToolButtonStyle->ribbonButtonForAction(act)->setToolButtonStyle(Qt::ToolButtonTextOnly);

    groupToolButtonStyle->addSeparator();

    act = createAction(tr("Delayed Popup"), QStringLiteral("folder-cog"));
    act->setMenu(menu);
    groupToolButtonStyle->addAction(act);
    btn = groupToolButtonStyle->ribbonButtonForAction(act);
    btn->setPopupMode(QToolButton::DelayedPopup);
    connect(act, &QAction::triggered, this, [this](bool b) {
        Q_EMIT messageRequested(tr("DelayedPopupCheckabletriggered : %1").arg(b));
    });

    act = createAction(tr("Menu Button Popup"), QStringLiteral("folder-star"));
    act->setMenu(menu);
    btn = groupToolButtonStyle->addLargeAction(act);
    btn->setPopupMode(QToolButton::MenuButtonPopup);
    connect(act, &QAction::triggered, this, [this](bool b) {
        Q_EMIT messageRequested(tr("MenuButtonPopupCheckabletriggered : %1").arg(b));
    });

    act = createAction(tr("Instant Popup"), QStringLiteral("folder-stats"));
    act->setMenu(menu);
    btn = groupToolButtonStyle->addLargeAction(act);
    btn->setPopupMode(QToolButton::InstantPopup);
    connect(act, &QAction::triggered, this, [this](bool b) {
        Q_EMIT messageRequested(tr("InstantPopupCheckabletriggered : %1").arg(b));
    });

    act = createAction(tr("Delayed Popup checkable"), QStringLiteral("table"));
    act->setCheckable(true);
    act->setMenu(menu);
    btn = groupToolButtonStyle->addLargeAction(act);
    btn->setPopupMode(QToolButton::DelayedPopup);
    connect(act, &QAction::triggered, this, [this](bool b) {
        Q_EMIT messageRequested(tr("DelayedPopupCheckableTest : %1").arg(b));
    });

    act = createAction(tr("Menu Button Popup checkable"), QStringLiteral("folder-check"));
    act->setCheckable(true);
    act->setMenu(menu);
    btn = groupToolButtonStyle->addLargeAction(act);
    btn->setPopupMode(QToolButton::MenuButtonPopup);
    connect(act, &QAction::triggered, this, [this](bool b) {
        Q_EMIT messageRequested(tr("MenuButtonPopupCheckableTest : %1").arg(b));
    });

    act = createAction(tr("Menu"), QStringLiteral("folder-plus"));
    act->setMenu(menu);
    btn = groupToolButtonStyle->addLargeAction(act);
    btn->setPopupMode(QToolButton::InstantPopup);

    act = createAction(tr("Action"), QStringLiteral("folder"));
    groupToolButtonStyle->addLargeAction(act);

    act = createAction(tr("Disable Action"), QStringLiteral("forbid"));
    act->setCheckable(true);
    act->setMenu(menu);
    act->setEnabled(false);
    groupToolButtonStyle->addLargeAction(act);

    act = new QAction(tr("Arrow Button"), this);
    btn = groupToolButtonStyle->addLargeAction(act);
    btn->setArrowType(Qt::DownArrow);

    act = new QAction(QxRibbon::colorIcon(Qt::red, QSize(32, 32)), tr("Color Action"), this);
    groupToolButtonStyle->addLargeAction(act);

    act = createAction(tr("Color Button"), QStringLiteral("typography"));
    RibbonColorButton *colorButton = new RibbonColorButton(this);
    colorButton->setDefaultAction(act);
    colorButton->setButtonType(RibbonButton::LargeButton);
    colorButton->setColor(Qt::blue);
    groupToolButtonStyle->addLargeWidget(colorButton);

    QAction *actionShortText = createAction(tr("Qx"), QStringLiteral("typography"), QStringLiteral(u"short-text"));
    groupToolButtonStyle->addLargeAction(actionShortText);
    actionShortText = createAction(tr("Q\nx"), QStringLiteral("typography"), QStringLiteral(u"short-wordwrap-text"));
    groupToolButtonStyle->addLargeAction(actionShortText);

    QAction *actionLongText = createAction(tr("show very long text in a button,balabalabala etc"),
                                           QStringLiteral("typography"), QStringLiteral(u"long-text"));
    groupToolButtonStyle->addLargeAction(actionLongText);

    QAction *optAct = new QAction(this);
    connect(optAct, &QAction::triggered, this, [this](bool on) {
        Q_UNUSED(on);
        QWidget *w = window();
        if (w) {
            QMessageBox::information(w, tr("Option Action Triggered"), tr("Option Action Triggered"));
        }
    });
    groupToolButtonStyle->addOptionAction(optAct);

    // group 3: the page-context playground - these tabs live in the window, so
    // the page only asks for them.
    RibbonGroup *groupPageContext = addGroup(tr("page context"));

    QAction *actShowContext = createAction(tr("show Context1"), QStringLiteral("layout-grid"));
    actShowContext->setCheckable(true);
    groupPageContext->addLargeAction(actShowContext);
    connect(actShowContext, &QAction::triggered, this, &HomePage::contextPageVisibleRequested);

    QAction *actDeleteContext = createAction(tr("delete Context1"), QStringLiteral("layout-grid-remove"));
    groupPageContext->addLargeAction(actDeleteContext);
    connect(actDeleteContext, &QAction::triggered, this, [this, actShowContext](bool on) {
        Q_UNUSED(on);
        Q_EMIT contextPageDestroyRequested();
        actShowContext->setChecked(false);
    });

    QAction *actShowContext2 = createAction(tr("show Context2"), QStringLiteral("layout-grid"));
    actShowContext2->setCheckable(true);
    connect(actShowContext2, &QAction::triggered, this, &HomePage::context2VisibleRequested);
    groupPageContext->addLargeAction(actShowContext2);
}

QAction *HomePage::createAction(const QString &text, const QString &glyph, const QString &objName)
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

/*!
 * A page is built before it is handed to a bar, so ribbonBar() is still empty
 * while the constructor runs. RibbonBar::insertPage() sets the back-pointer and
 * then re-parents the page into the bar's stack, so the first ParentChange is
 * the moment the bar becomes usable from here.
 */
bool HomePage::event(QEvent *e)
{
    if (e->type() == QEvent::ParentChange && !m_barAttached) {
        m_barAttached = true;
        attachToBar();
    }
    return RibbonPage::event(e);
}

void HomePage::attachToBar()
{
    RibbonBar *bar = ribbonBar();
    if (!bar) {
        return;
    }
    // Keep the "hide ribbon" toggle in step with the bar however it came to be
    // minimized - the action, the bar's own minimum button, a double click.
    connect(bar, &RibbonBar::minimizationChanged, m_hideRibbonAction, [this](bool minimized) {
        m_hideRibbonAction->setChecked(minimized);
    });
    // The demo opens with the minimum button visible.
    m_showMinimumButtonAction->trigger();

    connect(bar, &RibbonBar::ribbonStyleChanged, m_styleGroup, [this](RibbonBar::RibbonStyle style) {
        if (QAbstractButton *b = m_styleGroup->button(static_cast<int>(style))) {
            b->setChecked(true);
        }
    });
}

bool HomePage::eventFilter(QObject *watched, QEvent *e)
{
    // The full-screen box mirrors the window, so it has to hear about the window
    // changing state by any route (the box itself, F11, the window manager).
    if (e->type() == QEvent::WindowStateChange && m_fullScreenCheck && watched == window()) {
        m_fullScreenCheck->setChecked(static_cast<QWidget *>(watched)->isFullScreen());
    }
    return RibbonPage::eventFilter(watched, e);
}
