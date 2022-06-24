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
#ifndef QTN_RIBBONPAGE_H
#define QTN_RIBBONPAGE_H

#include <QWidget>
#include "QtnRibbonBar.h"
#include "QtitanDef.h"

QTITAN_BEGIN_NAMESPACE

class RibbonGroup;
class RibbonPagePrivate;

class QTITAN_EXPORT RibbonPage : public QWidget
{
    Q_OBJECT
    Q_ENUMS(ContextColor)
    Q_PROPERTY(const QString& title READ title WRITE setTitle)
    Q_PROPERTY(const QString& contextTitle READ contextTitle WRITE setContextTitle)
    Q_PROPERTY(int groupCount READ groupCount)
    Q_PROPERTY(ContextColor contextColor READ contextColor WRITE setContextColor)
    Q_PROPERTY(int tabWidth READ tabWidth WRITE setTabWidth)
public:
    enum ContextColor
    { 
        ContextColorNone,
        ContextColorGreen,
        ContextColorBlue,
        ContextColorRed,
        ContextColorYellow,
        ContextColorCyan,
        ContextColorPurple,
        ContextColorOrange
    };
public:
    RibbonPage(QWidget* parent = Q_NULL);
    RibbonPage(RibbonBar* ribbonBar, const QString& title);
    virtual ~RibbonPage();

public:
    bool isVisible() const;

public:
    void addGroup(RibbonGroup* group);
    RibbonGroup* addGroup(const QString& title);
    RibbonGroup* addGroup(const QIcon& icon, const QString& title);

    void insertGroup(int index, RibbonGroup* group);
    RibbonGroup* insertGroup(int index, const QString& title);
    RibbonGroup* insertGroup(int index, const QIcon& icon, const QString& title);

    void removeGroup(RibbonGroup* group);
    void removeGroup(int index);

    void detachGroup(RibbonGroup* group);
    void detachGroup(int index);

    void clearGroups();

    QAction* defaultAction() const;

    int groupCount() const;
    RibbonGroup* getGroup(int index) const;
    int groupIndex(RibbonGroup* group) const;
    QList<RibbonGroup*> groups() const;

    void setContextColor(ContextColor color);
    ContextColor contextColor() const;

    const QString& title() const;
    const QString& contextTitle() const;
    void setContextGroupName(const QString& groupName);

    void setTabWidth(int width);
    int tabWidth() const;

    void updateLayout();

public Q_SLOTS:
    virtual void setVisible(bool visible);
    void setTitle(const QString& title);
    void setContextTitle(const QString& title);

Q_SIGNALS:
    void titleChanged(const QString& title);
    void activated();
    void activating(bool& allow);

protected Q_SLOTS:
    void released();
    void actionTriggered(QAction* action);

private:
    void setAssociativeTab(QWidget* widget);
    QWidget* associativeTab() const;
    void setPageVisible(bool visible);
    void popup();
    void setRibbonMinimized(bool minimized);

protected:
    void activatingPage(bool& allow);
    void setGroupsHeight(int height);

public:
    virtual QSize sizeHint() const;

protected:
    virtual bool event(QEvent* event);
    virtual void paintEvent(QPaintEvent* event);
    virtual void changeEvent(QEvent* event);
    virtual void resizeEvent(QResizeEvent* event);

#ifdef Q_OS_WIN
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    virtual bool nativeEvent(const QByteArray& eventType, void* message, long* result);
#else
    virtual bool winEvent(MSG* message, long* result);
#endif
#endif // Q_OS_WIN

private:
    friend class RibbonBar;
    friend class RibbonBarPrivate;
    friend class RibbonTabBar;
    friend class RibbonGroup;
    friend class RibbonGroupPrivate;
    friend class RibbonTab;
    friend class RibbonBarCustomizeEngine;

    QTN_DECLARE_PRIVATE(RibbonPage)
    Q_DISABLE_COPY(RibbonPage)
};

QTITAN_END_NAMESPACE

#endif // QTN_RIBBONPAGE_H
