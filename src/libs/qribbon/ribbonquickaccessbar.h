/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 **/
#pragma once

#include "ribbon_global.h"
#include <QMenu>
#include <QToolBar>

QRIBBON_BEGIN_NAMESPACE

class RibbonQuickAccessBarPrivate;
class QRIBBON_EXPORT RibbonQuickAccessBar : public QToolBar
{
    Q_OBJECT
public:
    RibbonQuickAccessBar(QWidget *parent = Q_NULL);
    virtual ~RibbonQuickAccessBar();
public:
    QAction *actionCustomizeButton() const;
    void setActionVisible(QAction *action, bool visible);
    bool isActionVisible(QAction *action) const;
    int visibleCount() const;
public:
    virtual QSize sizeHint() const;

Q_SIGNALS:
    void showCustomizeMenu(QMenu *menu);

private Q_SLOTS:
    void customizeAction(QAction *);
    void aboutToShowCustomizeMenu();
    void aboutToHideCustomizeMenu();
protected:
    virtual bool event(QEvent *event);
    virtual void actionEvent(QActionEvent *);
    virtual void paintEvent(QPaintEvent *);
    virtual void resizeEvent(QResizeEvent *);
private:
    QRN_DECLARE_PRIVATE(RibbonQuickAccessBar)
    Q_DISABLE_COPY(RibbonQuickAccessBar)
};

QRIBBON_END_NAMESPACE
