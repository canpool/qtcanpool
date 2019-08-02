/****************************************************************************
**
** Qtitan Library by Developer Machines (Microsoft-Ribbon implementation for Qt.C++)
** 
** Copyright (c) 2009-2011 Developer Machines (http://www.devmachines.com)
** Copyright (c) 2019 MaMinJie <canpool@163.com>
**           ALL RIGHTS RESERVED
** 
**  The entire contents of this file is protected by copyright law and
**  international treaties. Unauthorized reproduction, reverse-engineering
**  and distribution of all or any portion of the code contained in this
**  file is strictly prohibited and may result in severe civil and 
**  criminal penalties and will be prosecuted to the maximum extent 
**  possible under the law.
**
**  RESTRICTIONS
**
**  THE SOURCE CODE CONTAINED WITHIN THIS FILE AND ALL RELATED
**  FILES OR ANY PORTION OF ITS CONTENTS SHALL AT NO TIME BE
**  COPIED, TRANSFERRED, SOLD, DISTRIBUTED, OR OTHERWISE MADE
**  AVAILABLE TO OTHER INDIVIDUALS WITHOUT WRITTEN CONSENT
**  AND PERMISSION FROM DEVELOPER MACHINES
**
**  CONSULT THE END USER LICENSE AGREEMENT FOR INFORMATION ON
**  ADDITIONAL RESTRICTIONS.
**
****************************************************************************/
#ifndef QTN_RIBBONBACKSTAGEVIEW_H
#define QTN_RIBBONBACKSTAGEVIEW_H

#include <QFrame>
#include <QToolButton>

#include "QtitanDef.h"

QTITAN_BEGIN_NAMESPACE

/* RibbonBackstageSeparator */
class QTITAN_EXPORT RibbonBackstageSeparator : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation)
public:
    RibbonBackstageSeparator(QWidget* parent);
    virtual ~RibbonBackstageSeparator();

public:
    void setOrientation(Qt::Orientation orient);
    Qt::Orientation orientation() const;

private:
    Q_DISABLE_COPY(RibbonBackstageSeparator)
};

class RibbonBackstageButtonPrivate;
/* RibbonBackstagePage */
class QTITAN_EXPORT RibbonBackstageButton : public QToolButton
{
    Q_OBJECT
    Q_PROPERTY(bool tabStyle READ tabStyle WRITE setTabStyle)
    Q_PROPERTY(bool flatStyle READ flatStyle WRITE setFlatStyle)
public:
    RibbonBackstageButton(QWidget* parent);
    virtual ~RibbonBackstageButton();

public:
    bool tabStyle() const;
    void setTabStyle(bool style);

    bool flatStyle();
    void setFlatStyle(bool flat);

protected:
    virtual void paintEvent(QPaintEvent* event);

private:
    QTN_DECLARE_PRIVATE(RibbonBackstageButton)
    Q_DISABLE_COPY(RibbonBackstageButton)
};


/* RibbonBackstagePage */
class QTITAN_EXPORT RibbonBackstagePage : public QWidget
{
    Q_OBJECT
public:
    RibbonBackstagePage(QWidget* parent);
    virtual ~RibbonBackstagePage();

protected:
    virtual void paintEvent(QPaintEvent* event);

private:
    Q_DISABLE_COPY(RibbonBackstagePage)
};


class RibbonBackstageViewPrivate;
/* RibbonBackstageView */
class QTITAN_EXPORT RibbonBackstageView : public QWidget
{
    Q_OBJECT
public:
    RibbonBackstageView(QWidget* parent = Q_NULL);
    virtual ~RibbonBackstageView();

public:
    bool isClosePrevented() const;
    void setClosePrevented(bool prevent);

    QAction* addAction(const QIcon& icon, const QString& text);
    QAction* addPage(QWidget* widget);

    void setActivePage(QWidget* widget);
    QWidget* getActivePage() const;

    QRect actionGeometry(QAction* ) const;

    using QWidget::addAction;

public Q_SLOTS:
    void open();

Q_SIGNALS:
    void aboutToShow();
    void aboutToHide();

protected:
    virtual bool event(QEvent* event);
    virtual bool eventFilter(QObject* object, QEvent* event);
    virtual void paintEvent(QPaintEvent* event);
    virtual void actionEvent(QActionEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);

private:
    QTN_DECLARE_PRIVATE(RibbonBackstageView)
    Q_DISABLE_COPY(RibbonBackstageView)
};

QTITAN_END_NAMESPACE

#endif // QTN_RIBBONBACKSTAGEVIEW_H
