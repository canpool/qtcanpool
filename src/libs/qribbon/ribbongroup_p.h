/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 **/
#pragma once

#include "ribbon_global.h"
#include "ribboncontrols.h"
#include "ribbongroup.h"
#include <QIcon>

class QAction;
class QToolButton;
class QStyleOptionGroupBox;

QRIBBON_BEGIN_NAMESPACE

class RibbonGroupOption;
class RibbonGroupScroll;
class RibbonControl;
class RibbonGroupPopup;

/* RibbonGroupPrivate */
class RibbonGroupPrivate : public QObject
{
    Q_OBJECT
public:
    QTC_DECLARE_PUBLIC(RibbonGroup)
public:
    explicit RibbonGroupPrivate();
    virtual ~RibbonGroupPrivate();
public:
    void init();
    void clear(bool deleteControls);
    void enableGroupScroll(bool scrollLeft, bool scrollRight);
    void showGroupScroll();

    void initStyleOption(QStyleOptionGroupBox &opt) const;
    int calcMinimumWidth(RibbonControlSizeDefinition::GroupSize size) const;
    int arrangeRowContent(QList<RibbonControl *> &row, int leftOffset, int rowItemHeight, int rowItemCount,
                          QWidget *parent) const;
    void reposition(const QRect &rect);
    void updateOptionButtonLayout();
    QSize updateControlsLayout(QWidget *parent, int leftOffset = 0) const;
    void adjustCurrentSize(bool expand);
    void updateLayoutParent(bool first);
    QSize sizeHint() const;

    void setReductionSize(RibbonControlSizeDefinition::GroupSize size);
    void reduce();
    void expand();
    bool canReduce();
    bool canExpand();

public Q_SLOTS:
    void pressLeftScrollButton();
    void pressRightScrollButton();
    void hidePopupMode(QAction *action);
public:
    static bool visualIndexLessThan(RibbonControl *first, RibbonControl *second);
    RibbonGroupOption *m_optionButton;
    RibbonGroupScroll *m_buttonScrollGroupLeft;    // Button to draw left scroll
    RibbonGroupScroll *m_buttonScrollGroupRight;   // Button to draw right scroll
    RibbonGroupPopup *m_groupPopup;
    RibbonToolBarControl *m_toolBar;

    QList<RibbonControl *> m_controls;
    RibbonControlSizeDefinition::GroupSize m_currentSize;
    Qt::Alignment m_contentAlignment;
    Qt::Alignment m_controlsAlignment;

    bool m_butOptionVisible;
    bool m_dirty;

    QAction *m_actOption;
    QString m_title;
    QIcon m_icon;
    QFont *m_font;
    QColor m_color;
    int m_pageScrollPos;
    int m_groupScrollPos;   // Scroll position
    Qt::TextElideMode m_titleElideMode;
private:
    Q_DISABLE_COPY(RibbonGroupPrivate)
};

QRIBBON_END_NAMESPACE
