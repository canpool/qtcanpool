/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 **/
#pragma once

#include "ribbon_global.h"
#include <QEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QResizeEvent>
#include <QStyleOption>
#include <QToolButton>

class QEventLoop;

QRIBBON_BEGIN_NAMESPACE

class RibbonGroup;
class RibbonGroupPopup;
/* RibbonGroupPopupButton */
class RibbonGroupPopupButton : public QToolButton
{
    Q_OBJECT
public:
    explicit RibbonGroupPopupButton(QWidget *parent, RibbonGroup *group);
    virtual ~RibbonGroupPopupButton();
public:
    bool isShowPopup() const;
    virtual void setVisible(bool visible);
    RibbonGroupPopup *popupWidget();
protected:
    void resetReducedGroup();

public Q_SLOTS:
    void resetPopopGroup();
public:
    virtual QSize sizeHint() const;
protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
private:
    bool m_hasPopup;
    QEventLoop *m_eventLoop;
    RibbonGroup *m_ribbonGroup;
    RibbonGroupPopup *m_popupWidget;
private:
    Q_DISABLE_COPY(RibbonGroupPopupButton)
};

/* RibbonGroupPopup */
class RibbonGroupPopup : public QMenu
{
public:
    explicit RibbonGroupPopup(RibbonGroup *group);
    virtual ~RibbonGroupPopup();
public:
    virtual QSize sizeHint() const;
protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
private:
    RibbonGroup *m_group;
private:
    Q_DISABLE_COPY(RibbonGroupPopup)
};

/* RibbonGroupOption */
class RibbonGroupOption : public QToolButton
{
    Q_OBJECT
public:
    explicit RibbonGroupOption(QWidget *parent);
    virtual ~RibbonGroupOption();
public:
    QString text() const;
public:
    virtual QSize sizeHint() const;
protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void actionEvent(QActionEvent *event);
private:
    Q_DISABLE_COPY(RibbonGroupOption)
};

/* RibbonGroupScroll */
class RibbonGroupScroll : public QToolButton
{
    Q_OBJECT
public:
    explicit RibbonGroupScroll(QWidget *parent, bool scrollLeft);
    virtual ~RibbonGroupScroll();

Q_SIGNALS:
    void stopScrollTimer();
protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void leaveEvent(QEvent *event);
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
    explicit KeyTipEvent(RibbonKeyTip *kTip);
public:
    RibbonKeyTip *getKeyTip() const;
public:
    // Returns the event type/number for KeyTipEvent.
    // The type is registered on first access. Use this to detect incoming
    static QEvent::Type eventNumber();
protected:
    RibbonKeyTip *keyTip;
};

class ShowKeyTipEvent : public QEvent
{
public:
    ShowKeyTipEvent(QWidget *w);
    QWidget *buddy() const;
    // Returns the event type/number for ShowKeyTipEvent.
    // The type is registered on first access. Use this to detect incoming
    static QEvent::Type eventNumber();
protected:
    QWidget *m_buddy;
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

QRIBBON_END_NAMESPACE
