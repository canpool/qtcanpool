/****************************************************************************
**
** Qtitan Library by Developer Machines (Microsoft-Ribbon implementation for Qt.C++)
** 
** Copyright (c) 2009-2013 Developer Machines (http://www.devmachines.com)
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
#ifndef QTN_RIBBONPRIVATE_H
#define QTN_RIBBONPRIVATE_H

#include <QEvent>
#include <QMenu>
#include <QToolButton>
#include <QStyleOption>
#include "QtitanDef.h"

class QEventLoop;

QTITAN_BEGIN_NAMESPACE

class RibbonGroup;
class ExWidgetWrapperPrivate;

/* ExWidgetWrapper */
class ExWidgetWrapper: public QWidget
{
    Q_OBJECT
protected:
    explicit ExWidgetWrapper(QWidget* parent, QWidget* widget = Q_NULL);
    virtual ~ExWidgetWrapper();

public:
    QString labelText() const;
    void setLabelText(const QString& text);

    void setIcon(const QIcon& icon);
    QIcon icon() const;

    void setAlignWidget(bool align);
    bool alignWidget() const;

    void setLengthLabel(int length);

    QVariant inputMethodQuery(Qt::InputMethodQuery property) const;

public:
    virtual QSize sizeHint() const;

protected:
    virtual void focusInEvent(QFocusEvent* event);
    virtual void focusOutEvent(QFocusEvent* event);
    virtual void paintEvent(QPaintEvent* event);
    virtual void resizeEvent(QResizeEvent* event);
    virtual void inputMethodEvent(QInputMethodEvent* event);
    virtual bool event(QEvent* event);

protected:
    void updateGeometries();
    void initStyleOption(QStyleOptionFrameV2* option) const;

protected:
    virtual bool isReadOnly() const;

private:
    friend class RibbonGroup;
    QTN_DECLARE_PRIVATE(ExWidgetWrapper)
    Q_DISABLE_COPY(ExWidgetWrapper)
};


/* RibbonGroupPopupMenu */
class ReducedGroupPopupMenu : public QWidget
{
    Q_OBJECT
public:
    explicit ReducedGroupPopupMenu(QWidget* parent);
    virtual ~ReducedGroupPopupMenu();

public:
    void setGroup(RibbonGroup*);
    RibbonGroup* getGroup() const;
    void setNoReplayFor(QWidget* noReplayFor);

Q_SIGNALS:
    void aboutToHide();

public slots:
    void actionTriggered(QAction* action);

public:
    virtual QSize sizeHint() const;

protected:
    virtual void showEvent(QShowEvent* event);
    virtual void hideEvent(QHideEvent* event);
    virtual void paintEvent(QPaintEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);

protected:
    RibbonGroup* m_group;
    QWidget* m_parent;
    QWidget* m_noReplayFor;
private:
    Q_DISABLE_COPY(ReducedGroupPopupMenu)
};


/* RibbonGroupWrapper */
class RibbonGroupWrapper : public QToolButton
{
    Q_OBJECT
public:
    explicit RibbonGroupWrapper(QWidget* p);
    virtual ~RibbonGroupWrapper();

public:
    void setGroup(RibbonGroup* rg);
    RibbonGroup* getGroup() const;

    bool isReducedGroup() const;
    void setReducedGroup(bool);

    void resetReducedGroup();

public Q_SLOTS:
    void resetPopopGroup();

public:
    QSize realSize() const;
    QSize reducedSize() const;
    void startLayout();
    void finalLayout();
    bool extendSize(int width);
    bool reduceSize(int level, int width);
    void updateLayout(int* width, bool reset);

    virtual QSize sizeHint() const;
    virtual bool eventFilter(QObject* object, QEvent* event);

protected:
    virtual bool event(QEvent* event);
    virtual void paintEvent(QPaintEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void resizeEvent(QResizeEvent* event);

private:
//        bool m_reduced;
    bool m_hasPopup;
    QSize m_realSize;
    QEventLoop* m_eventLoop;
    RibbonGroup* m_ribbonGroup;
    ReducedGroupPopupMenu* m_popupMenu;

private:
    Q_DISABLE_COPY(RibbonGroupWrapper)
};

/* RibbonGroupOption */
class RibbonGroupOption: public QToolButton
{
    Q_OBJECT
public:
    RibbonGroupOption(QWidget* parent);
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
    RibbonGroupScroll(QWidget* parent, bool scrollLeft);
    virtual ~RibbonGroupScroll();

protected:
    virtual void paintEvent(QPaintEvent* event);
protected:
    bool m_scrollLeft;
};


class RibbonKeyTip;

/* KeyTipEvent */
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

/* ShowKeyTipEvent */
class ShowKeyTipEvent : public QEvent
{
public:
    //! Constructor for the event.
    ShowKeyTipEvent(QWidget* w);

public:
    QWidget* getWidget() const;

public:
    // Returns the event type/number for ShowKeyTipEvent.
    // The type is registered on first access. Use this to detect incoming
    static QEvent::Type eventNumber();
protected:
    QWidget* widget;
};

/* HideKeyTipEvent */
class HideKeyTipEvent : public QEvent
{
public:
    //! Constructor for the event.
    HideKeyTipEvent();
public:
    // Returns the event type/number for ShowKeyTipEvent.
    // The type is registered on first access. Use this to detect incoming
    static QEvent::Type eventNumber();
};

/* MinimizedEvent */
class MinimizedEvent : public QEvent
{
public:
    //! Constructor for the event.
    MinimizedEvent();
public:
    // Returns the event type/number for ShowKeyTipEvent.
    // The type is registered on first access. Use this to detect incoming
    static QEvent::Type eventNumber();
};

QTITAN_END_NAMESPACE

#endif // QTN_RIBBONPRIVATE_H


