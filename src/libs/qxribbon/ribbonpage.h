/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
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
 * @brief 一项 ribbon tab 页
 * @note RibbonPage 的 windowTitle 影响了其在 RibbonBar 的标签显示，
 * 如果要改标签名字，直接调用 RibbonPage 的 setWindowTitle 函数
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

    int groupCount() const;
    QList<RibbonGroup *> groupList() const;

    void setBackgroundBrush(const QBrush &brush);

    QSize sizeHint() const Q_DECL_OVERRIDE;

    bool isPageContext() const;

    bool isCanCustomize() const;
    void setCanCustomize(bool b);

    void updateItemGeometry();
protected:
    void setGroupLayoutMode(RibbonGroup::GroupLayoutMode m);
    void markIsPageContext(bool isPageContext = true);
protected:
    bool event(QEvent *e) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *e) Q_DECL_OVERRIDE;
    bool eventFilter(QObject *watched, QEvent *event) Q_DECL_OVERRIDE;
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
