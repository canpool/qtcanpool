/****************************************************************************
**
** Qtitan Library by Developer Machines (Microsoft-Ribbon implementation for Qt.C++)
** 
** Copyright (c) 2009-2015 Developer Machines (http://www.devmachines.com)
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
#ifndef QTN_RIBBONPRIVATE_H
#define QTN_RIBBONPRIVATE_H

#include <QEvent>
#include <QMenu>
#include <QToolButton>
#include <QStyleOption>
#include <QHBoxLayout>
#include <QLabel>
#include <QResizeEvent>

#include "QtitanDef.h"

class QEventLoop;

QTITAN_BEGIN_NAMESPACE

class RibbonGroup;
class RibbonGroupPopup;
/* RibbonGroupPopupButton */
class RibbonGroupPopupButton : public QToolButton
{
    Q_OBJECT
public:
    explicit RibbonGroupPopupButton(QWidget* parent, RibbonGroup* group);
    virtual ~RibbonGroupPopupButton();

public:
    bool isShowPopup() const;
    virtual void setVisible(bool visible);
    RibbonGroupPopup* popupWidget();

protected:
    void resetReducedGroup();

public Q_SLOTS:
    void resetPopopGroup();

public:
    virtual QSize sizeHint() const;

protected:
    virtual void paintEvent(QPaintEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);

private:
    bool m_hasPopup;
    QEventLoop* m_eventLoop;
    RibbonGroup* m_ribbonGroup;
    RibbonGroupPopup* m_popupWidget;
private:
    Q_DISABLE_COPY(RibbonGroupPopupButton)
};

/* RibbonGroupPopup */
class RibbonGroupPopup : public QMenu
{
public:
    explicit RibbonGroupPopup(RibbonGroup* group);
    virtual ~RibbonGroupPopup();

public:
    virtual QSize sizeHint() const;

protected:
    virtual void paintEvent(QPaintEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
private:
    RibbonGroup* m_group;
private:
    Q_DISABLE_COPY(RibbonGroupPopup)
};

/* RibbonGroupOption */
class RibbonGroupOption : public QToolButton
{
    Q_OBJECT
public:
    explicit RibbonGroupOption(QWidget* parent);
    virtual ~RibbonGroupOption();

public:
    QString text() const;

public:
    virtual QSize sizeHint() const;

protected:
    virtual void paintEvent(QPaintEvent* event);
    virtual void actionEvent(QActionEvent* event);
private:
    Q_DISABLE_COPY(RibbonGroupOption)
};

/* RibbonGroupScroll */
class RibbonGroupScroll : public QToolButton
{
    Q_OBJECT
public:
    explicit RibbonGroupScroll(QWidget* parent, bool scrollLeft);
    virtual ~RibbonGroupScroll();

Q_SIGNALS:
    void stopScrollTimer();

protected:
    virtual void paintEvent(QPaintEvent* event);
    virtual void leaveEvent(QEvent* event);

protected:
    bool m_scrollLeft;
private:
    Q_DISABLE_COPY(RibbonGroupScroll)
};


class RibbonKeyTip;
class KeyTipEvent : public QEvent
{
public:
    //! Constructor for the event.
    explicit KeyTipEvent(RibbonKeyTip* kTip);

public:
    RibbonKeyTip* getKeyTip() const;

public:
    // Returns the event type/number for KeyTipEvent.
    // The type is registered on first access. Use this to detect incoming
    static QEvent::Type eventNumber();
protected:
    RibbonKeyTip* keyTip;
};

class ShowKeyTipEvent : public QEvent
{
public:
    ShowKeyTipEvent(QWidget* w);
    QWidget* buddy() const;
    // Returns the event type/number for ShowKeyTipEvent.
    // The type is registered on first access. Use this to detect incoming
    static QEvent::Type eventNumber();
protected:
    QWidget* m_buddy;
};

class HideKeyTipEvent : public QEvent
{
public:
    HideKeyTipEvent();
    // Returns the event type/number for ShowKeyTipEvent.
    // The type is registered on first access. Use this to detect incoming
    static QEvent::Type eventNumber();
};

/* MinimizedEvent */
class MinimizedEvent : public QEvent
{
public:
    MinimizedEvent();
    // Returns the event type/number for ShowKeyTipEvent.
    // The type is registered on first access. Use this to detect incoming
    static QEvent::Type eventNumber();
};

QTITAN_END_NAMESPACE

#endif // QTN_RIBBONPRIVATE_H


