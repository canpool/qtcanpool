/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#pragma once

#include "qxribbon_global.h"
#include "ribbongroup.h"
#include <QPushButton>
#include <QScopedPointer>
#include <QWheelEvent>
#include <QWidget>

class QHBoxLayout;
class QWheelEvent;

QX_RIBBON_BEGIN_NAMESPACE

class RibbonPageProxyPrivate;
class RibbonPagePrivate;
class RibbonBar;
class RibbonBarPrivate;

/**
 * @brief A ribbon tab page
 * @note The windowTitle of RibbonPage affects how its tab label is shown in the RibbonBar;
 * to change the tab name, call the setWindowTitle function of RibbonPage directly
 */
class QX_RIBBON_EXPORT RibbonPage : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool isCanCustomize READ isCanCustomize WRITE setCanCustomize)
    Q_PROPERTY(QString pageName READ pageName WRITE setPageName)
public:
    explicit RibbonPage(QWidget *parent = Q_NULLPTR);
    explicit RibbonPage(const QString &name, QWidget *parent = Q_NULLPTR);
    ~RibbonPage() Q_DECL_OVERRIDE;

    RibbonBar *ribbonBar() const;

    // equal to windowTitle and setWindowTitle
    QString pageName() const;
    void setPageName(const QString &title);
    QString title() const;

    RibbonGroup::GroupLayoutMode groupLayoutMode() const;

    RibbonGroup *addGroup(const QString &title);
    void addGroup(RibbonGroup *group);
    RibbonGroup *insertGroup(int index, const QString &title);

    RibbonGroup *group(int index) const;
    RibbonGroup *groupByName(const QString &title) const;
    RibbonGroup *groupByObjectName(const QString &objname) const;

    int groupIndex(RibbonGroup *group) const;

    void moveGroup(int from, int to);

    bool takeGroup(RibbonGroup *group);

    bool removeGroup(RibbonGroup *group);
    bool removeGroup(int index);
    void clearGroups();

    int groupCount() const;
    QList<RibbonGroup *> groupList() const;
    QList<RibbonGroup *> groups() const;

    void setBackgroundBrush(const QBrush &brush);

    QSize sizeHint() const Q_DECL_OVERRIDE;

    bool isPageContext() const;

    bool isCanCustomize() const;
    void setCanCustomize(bool b);

    void updateItemGeometry();
public Q_SLOTS:
    void setTitle(const QString &title);
protected:
    void setGroupLayoutMode(RibbonGroup::GroupLayoutMode m);
    void markIsPageContext(bool isPageContext = true);
protected:
    bool event(QEvent *e) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *e) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
private:
    void setRibbonBar(RibbonBar *bar);
private:
    QX_DECLARE_PRIVATE(RibbonPage)
    friend class RibbonBar;
    friend class RibbonBarPrivate;
    friend class RibbonPageContext;
};

QX_RIBBON_END_NAMESPACE
