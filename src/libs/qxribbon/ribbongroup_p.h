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
protected:
    void changeEvent(QEvent *e) Q_DECL_OVERRIDE;
private:
    void updateIcon();
};

class RibbonGroupPrivate
{
    QX_DECLARE_PUBLIC(RibbonGroup)
public:
    RibbonGroupPrivate();

    void init();
    void resetLargeToolButtonStyle();

    // B11: bring the button presentation in line with m_reduced. A reduced group
    // places a button that would own a whole column in a fraction of the height
    // instead, so that button has to be drawn as a small one.
    void applyButtonTypes();
    // B11: mark the cached widths as stale. They depend on the group's contents and
    // on its height, and the height in turn changes the width a large button asks
    // for, so anything that can move either has to come through here.
    void invalidateWidths();
    // B11: measure the group in both presentations and cache the widths. The
    // presentation the group came in with is restored before returning.
    void ensureWidths();

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
    bool m_isCanCustomize;     ///< Records whether customization is allowed
    bool m_reduced;            ///< B11: lay the group out in its reduced presentation
    bool m_widthsValid;        ///< B11: whether the cached widths match the current height, title and contents
    int m_normalWidth;         ///< B11: width of the natural presentation
    int m_reducedWidth;        ///< B11: width of the reduced presentation
    int m_widthsHeight;        ///< B11: height the cached widths were measured at
    int m_widthsTitleHeight;   ///< B11: title height the cached widths were measured with
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
