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
#ifndef QTN_RIBBONSYSTEMPOPUPBAR_H
#define QTN_RIBBONSYSTEMPOPUPBAR_H

#include <QMenu>
#include <QToolButton>

#include "QtitanDef.h"

QTITAN_BEGIN_NAMESPACE

class RibbonBackstageView;
class RibbonSystemButtonPrivate;

/* RibbonSystemButton */
class QTITAN_EXPORT RibbonSystemButton : public QToolButton
{
    Q_OBJECT
    Q_PROPERTY(Qt::ToolButtonStyle toolButtonStyle READ toolButtonStyle WRITE setToolButtonStyle)
public:
    RibbonSystemButton(QWidget* parent);
    virtual ~RibbonSystemButton();

public:
    void setBackstage(RibbonBackstageView* backstage);
    RibbonBackstageView* backstage() const;

    void setBackgroundColor(const QColor& color);
    QColor backgroundColor() const;

public:
    virtual QSize sizeHint() const;

protected:
    virtual void paintEvent(QPaintEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);

private:
    Q_DISABLE_COPY(RibbonSystemButton)
    QTN_DECLARE_PRIVATE(RibbonSystemButton)
};


class RibbonPageSystemRecentFileList;
class RibbonPageSystemPopup;
class RibbonSystemPopupBarPrivate;

/* RibbonSystemPopupBar */
class QTITAN_EXPORT RibbonSystemPopupBar : public QMenu
{
    Q_OBJECT
public:
    RibbonSystemPopupBar(QWidget* parent);
    virtual ~RibbonSystemPopupBar();

public:
    QAction* addPopupBarAction(const QString& text);
    void addPopupBarAction(QAction* action, Qt::ToolButtonStyle style = Qt::ToolButtonTextOnly);

    RibbonPageSystemRecentFileList* addPageRecentFile(const QString& caption);
    RibbonPageSystemPopup* addPageSystemPopup(const QString& caption, QAction* defaultAction, bool splitAction);

public:
    virtual QSize sizeHint() const;

protected:
    virtual void showEvent(QShowEvent* event);
    virtual void paintEvent(QPaintEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void keyPressEvent(QKeyEvent* event);
    virtual void resizeEvent(QResizeEvent* event);

private:
    friend class RibbonPageSystemPopup;
    Q_DISABLE_COPY(RibbonSystemPopupBar)
    QTN_DECLARE_PRIVATE(RibbonSystemPopupBar)
};

/* RibbonSystemRecentFileList */
class QTITAN_EXPORT RibbonPageSystemPopupListCaption : public QWidget
{
    Q_OBJECT
public:
    RibbonPageSystemPopupListCaption(const QString& caption);
    virtual ~RibbonPageSystemPopupListCaption();

public:
    virtual QSize sizeHint() const;

protected:
    virtual void drawEvent(QPaintEvent* event, QPainter* p);

protected:
    virtual void paintEvent(QPaintEvent* event);

private:
    friend class RibbonSystemPopupBar;
    Q_DISABLE_COPY(RibbonPageSystemPopupListCaption)
};

class RibbonPageSystemRecentFileListPrivate;
/* RibbonSystemRecentFileList */
class QTITAN_EXPORT RibbonPageSystemRecentFileList : public RibbonPageSystemPopupListCaption
{
    Q_OBJECT
public:
    RibbonPageSystemRecentFileList(const QString& caption);
    virtual ~RibbonPageSystemRecentFileList();
    virtual QSize sizeHint() const;

public:
    void setSize(int size);
    int getSize() const;

    QAction* getCurrentAction() const;

public Q_SLOTS:
    void updateRecentFileActions(const QStringList& files);

Q_SIGNALS:
    void openRecentFile(const QString& file);

protected:
    virtual void drawEvent(QPaintEvent* event, QPainter* p);

protected:
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void enterEvent(QEvent* event);
    virtual void leaveEvent(QEvent* event);
    virtual void keyPressEvent(QKeyEvent* event);

    virtual void resizeEvent(QResizeEvent* event);

private:
    Q_DISABLE_COPY(RibbonPageSystemRecentFileList)
    QTN_DECLARE_PRIVATE(RibbonPageSystemRecentFileList)
};

class RibbonPageSystemPopupPrivate;
/* RibbonPageSystemPopup */
class QTITAN_EXPORT RibbonPageSystemPopup : public QMenu
{
    Q_OBJECT
public:
    RibbonPageSystemPopup(const QString& caption);
    virtual ~RibbonPageSystemPopup();

protected:
    virtual bool event(QEvent* event);
    virtual void paintEvent(QPaintEvent* event);

private:
    Q_DISABLE_COPY(RibbonPageSystemPopup)
    QTN_DECLARE_PRIVATE(RibbonPageSystemPopup)
};

QTITAN_END_NAMESPACE

#endif // QTN_RIBBONSYSTEMPOPUPBAR_H
