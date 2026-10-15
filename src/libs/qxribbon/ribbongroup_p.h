/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#pragma once

#include "qxribbon_global.h"
#include "ribbongroup.h"

#include <QToolButton>

QX_RIBBON_BEGIN_NAMESPACE

class RibbonButton;
class RibbonGroupItem;
class RibbonGroupLayout;

/**
 * @brief The option button at the bottom-right corner of a Group
 *
 * This button is associated with an action; use @ref RibbonGroup::addOptionAction to create it
 *
 */
class RibbonGroupOptionButton : public QToolButton
{
    Q_OBJECT
public:
    explicit RibbonGroupOptionButton(QWidget *parent = Q_NULLPTR);
    // Unlike setDefaultAction, this function only connects the action's response without setting text, icon, etc.
    void connectAction(QAction *action);
};

class RibbonGroupPrivate
{
    QX_DECLARE_PUBLIC(RibbonGroup)
public:
    RibbonGroupPrivate();

    void init();
    void resetLargeToolButtonStyle();

    // Return the button for the last added action, provided the last one is a toolbutton; otherwise return Q_NULLPTR
    RibbonButton *lastAddedButton();
    // RibbonGroup's friend class RibbonCustommizeWidget uses this interface to get all items
    const QList<RibbonGroupItem *> &ribbonGroupItems() const;
public:
    RibbonGroupLayout *m_layout;
    RibbonGroupOptionButton *m_optionActionButton;
    RibbonGroup::GroupLayoutMode m_groupLayoutMode;   ///< Group layout mode; defaults to the 3-row ThreeRowMode
    static int s_groupTitleHeight;                    ///< Global group title bar height; defaults to 21
    static bool s_titleVisible;
    bool m_isCanCustomize;   ///< Records whether customization is allowed
};

/* RibbonSeparator */
class RibbonSeparator : public QWidget
{
    Q_OBJECT
public:
    RibbonSeparator(int height, QWidget *parent = Q_NULLPTR);
    RibbonSeparator(QWidget *parent = Q_NULLPTR);

    virtual QSize sizeHint() const Q_DECL_OVERRIDE;
    void setTopBottomMargins(int top, int bottom);
protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
private:
    int m_topMargin;
    int m_bottomMargin;
};

QX_RIBBON_END_NAMESPACE
