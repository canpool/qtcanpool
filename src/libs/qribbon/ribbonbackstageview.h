/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 **/
#pragma once

#include "ribbon_global.h"
#include "ribbonbar.h"
#include <QFrame>
#include <QToolButton>

QRIBBON_BEGIN_NAMESPACE

class QRIBBON_EXPORT RibbonBackstageSeparator : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation)
public:
    RibbonBackstageSeparator(QWidget *parent);
    virtual ~RibbonBackstageSeparator();
public:
    void setOrientation(Qt::Orientation orient);
    Qt::Orientation orientation() const;
private:
    Q_DISABLE_COPY(RibbonBackstageSeparator)
};

class RibbonBackstageButtonPrivate;
/* RibbonBackstagePage */
class QRIBBON_EXPORT RibbonBackstageButton : public QToolButton
{
    Q_OBJECT
    Q_PROPERTY(bool tabStyle READ tabStyle WRITE setTabStyle)
    Q_PROPERTY(bool flatStyle READ flatStyle WRITE setFlatStyle)
public:
    RibbonBackstageButton(QWidget *parent);
    virtual ~RibbonBackstageButton();
public:
    bool tabStyle() const;
    void setTabStyle(bool style);

    bool flatStyle();
    void setFlatStyle(bool flat);
protected:
    virtual void paintEvent(QPaintEvent *event);
private:
    QRN_DECLARE_PRIVATE(RibbonBackstageButton)
    Q_DISABLE_COPY(RibbonBackstageButton)
};

/* RibbonBackstagePage */
class QRIBBON_EXPORT RibbonBackstagePage : public QWidget
{
    Q_OBJECT
public:
    RibbonBackstagePage(QWidget *parent);
    virtual ~RibbonBackstagePage();
protected:
    virtual void paintEvent(QPaintEvent *event);
private:
    Q_DISABLE_COPY(RibbonBackstagePage)
};

class RibbonBackstageViewPrivate;
/* RibbonBackstageView */
class QRIBBON_EXPORT RibbonBackstageView : public QWidget
{
    Q_OBJECT
public:
    explicit RibbonBackstageView(RibbonBar *ribbonBar);
    virtual ~RibbonBackstageView();
public:
    bool isClosePrevented() const;
    void setClosePrevented(bool prevent);

    QAction *addAction(const QIcon &icon, const QString &text);
    QAction *addSeparator();
    QAction *addPage(QWidget *widget);

    void setActivePage(QWidget *widget);
    QWidget *getActivePage() const;

    QRect actionGeometry(QAction *) const;
    QRect menuGeometry() const;

    using QWidget::addAction;

public Q_SLOTS:
    void open();
Q_SIGNALS:
    void aboutToShow();
    void aboutToHide();
protected:
    virtual bool event(QEvent *event);
    virtual void paintEvent(QPaintEvent *event);
    virtual bool eventFilter(QObject *object, QEvent *event);
    virtual void actionEvent(QActionEvent *event);
    virtual void showEvent(QShowEvent *event);
    virtual void hideEvent(QHideEvent *event);
private:
    friend class RibbonBackstageViewMenu;
    friend class RibbonBar;
    QRN_DECLARE_PRIVATE(RibbonBackstageView)
    Q_DISABLE_COPY(RibbonBackstageView)
};

QRIBBON_END_NAMESPACE
