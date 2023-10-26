/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 **/
#pragma once

#include "ribbonbackstageview.h"
#include <QHash>
#include <QToolButton>

class QStyleOptionMenuItem;
class QWidgetAction;

QRIBBON_BEGIN_NAMESPACE

/* RibbonBackstageCloseButton */
class RibbonBackstageCloseButton : public QToolButton
{
public:
    RibbonBackstageCloseButton(QWidget *parent);
public:
    virtual QSize sizeHint() const;
protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
private:
    Q_DISABLE_COPY(RibbonBackstageCloseButton)
};

class StyleOptionRibbonBackstage;
/* RibbonBackstageViewMenu */
class RibbonBackstageViewMenu : public QWidget
{
public:
    RibbonBackstageViewMenu(RibbonBackstageView *backstageView, QWidget *parent = Q_NULL);
public:
    void createBackstageCloseButton();
    QAction *actionAt(const QPoint &p) const;
    QRect actionRect(QAction *act) const;
    void resetItemsDirty();
public:
    virtual QSize sizeHint() const;
protected:
    void initStyleOption(StyleOptionRibbonBackstage *opt) const;
    void initStyleOption(QStyleOptionMenuItem *option, const QAction *action) const;
    void updateActionRects() const;
protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void keyPressEvent(QKeyEvent *);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void leaveEvent(QEvent *event);
private:
    RibbonBackstageView *m_backstageView;
    RibbonBackstageCloseButton *m_backStageCloseButton;
    bool m_mouseDown;

    mutable bool m_itemsDirty;
    mutable int m_menuWidth;
    mutable int m_maxIconWidth;
    mutable QVector<QRect> m_actionRects;
private:
    Q_DISABLE_COPY(RibbonBackstageViewMenu)
};

class RibbonBar;
class HackFilter;
/* RibbonBackstageViewPrivate */
class RibbonBackstageViewPrivate : public QObject
{
public:
    Q_OBJECT
    QRN_DECLARE_PUBLIC(RibbonBackstageView)
public:
    explicit RibbonBackstageViewPrivate();
    virtual ~RibbonBackstageViewPrivate();
public:
    void init(RibbonBar *ribbonBar);
    void layoutBackstage();
    void updateLayout();
    void updateGeometryPage(QWidget *widget);

    void setCurrentAction(QAction *action);
    QAction *currentAction() const;

    QWidgetAction *getAction(QWidget *w) const;
public:
    RibbonBar *m_ribbon;
    RibbonBackstageViewMenu *m_backstageMenu;
    RibbonBar::TabBarPosition m_tabBarPosition;
    QWidget *m_activePage;
    QAction *m_currentAction;

    bool m_closePrevented;
    mutable QVector<QRect> m_actionRects;
    mutable QHash<QAction *, QWidget *> m_widgetItems;
private:
    Q_DISABLE_COPY(RibbonBackstageViewPrivate)
};

QRIBBON_END_NAMESPACE
