/**
 * Copyright (C) 2020-2022 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#ifndef RIBBONBAR_H
#define RIBBONBAR_H

#include "qcanpool.h"
#include <QWidget>

class QToolBar;

QCANPOOL_BEGIN_NAMESPACE

class QuickAccessBar;
class FancyTitleBar;
class RibbonPage;
class RibbonBarPrivate;

class QCANPOOL_SHARED_EXPORT RibbonBar : public QWidget
{
    Q_OBJECT
public:
    enum RibbonStyle {
        ClassicStyle,   // default, office-like
        MergedStyle,
    };

    enum ToolBarPosition {
        ToolBarTopPosition,
        ToolBarBottomPosition,
    };

public:
    explicit RibbonBar(QWidget *parent = nullptr);
    ~RibbonBar();

public:
    RibbonPage *addPage(const QString &text);
    RibbonPage *addPage(const QIcon &icon, const QString &text);
    void addPage(RibbonPage *page);
    RibbonPage *insertPage(int index, const QString &text);
    RibbonPage *insertPage(int index, const QIcon &icon, const QString &text);
    void insertPage(int index, RibbonPage *page);

    void removePage(RibbonPage *page);
    void removePage(int index);

    void clearPages();

    RibbonPage *page(int index) const;
    int pageCount() const;
    int currentPageIndex() const;
    QList<RibbonPage *> pages() const;

    int addAction(QAction *action);

    QuickAccessBar *quickAccessBar() const;

    RibbonStyle ribbonStyle() const;
    void setRibbonStyle(RibbonStyle style);

    QToolBar *toolBar(ToolBarPosition position) const;
    FancyTitleBar *titleBar() const;

    void updateParentFlags();

protected:
    virtual bool eventFilter(QObject *object, QEvent *event);

private:
    RibbonBarPrivate *d;
};

QCANPOOL_END_NAMESPACE

#endif // RIBBONBAR_H
