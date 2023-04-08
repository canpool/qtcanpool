/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 **/
#pragma once

#include "ribbon_global.h"
#include <QStatusBar>

QRIBBON_BEGIN_NAMESPACE

class RibbonStatusBarSwitchGroupPrivate;
class QRIBBON_EXPORT RibbonStatusBarSwitchGroup : public QWidget
{
    Q_OBJECT
public:
    explicit RibbonStatusBarSwitchGroup();
    virtual ~RibbonStatusBarSwitchGroup();
public:
    void clear();
protected:
    virtual void actionEvent(QActionEvent *event);
private:
    QTC_DECLARE_PRIVATE(RibbonStatusBarSwitchGroup)
    Q_DISABLE_COPY(RibbonStatusBarSwitchGroup)
};

class RibbonStatusBarPrivate;
/* RibbonStatusBar */
class QRIBBON_EXPORT RibbonStatusBar : public QStatusBar
{
    Q_OBJECT
public:
    explicit RibbonStatusBar(QWidget *parent = Q_NULL);
    virtual ~RibbonStatusBar();
public:
    QAction *addAction(const QString &text);
    QAction *addSeparator();

    void addPermanentAction(QAction *action);

    QRect childrenPermanentRect() const;
public:
    using QWidget::addAction;
protected:
    virtual bool event(QEvent *event);
    virtual void paintEvent(QPaintEvent *event);
    virtual void actionEvent(QActionEvent *event);
    virtual void resizeEvent(QResizeEvent *);
private:
    QTC_DECLARE_PRIVATE(RibbonStatusBar)
    Q_DISABLE_COPY(RibbonStatusBar)
};

QRIBBON_END_NAMESPACE
