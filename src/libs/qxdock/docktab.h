/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include "qxdock_global.h"
#include <QWidget>

QX_DOCK_BEGIN_NAMESPACE

class DockWidget;
class DockPanel;

class DockTabPrivate;

class QX_DOCK_EXPORT DockTab : public QWidget
{
    Q_OBJECT
public:
    using Super = QWidget;
public:
    explicit DockTab(DockWidget *w, QWidget *parent = nullptr);
    virtual ~DockTab();

    bool isActive() const;
    void setActive(bool active);

    void setDockPanel(DockPanel *panel);

    DockWidget *dockWidget() const;

    const QIcon &icon() const;
    void setIcon(const QIcon &icon);

    QSize iconSize() const;
    void setIconSize(const QSize &size);

    QString text() const;

    bool isTitleElided() const;
    bool isClosable() const;

    void updateStyle();

Q_SIGNALS:
    void activeTabChanged();
    void clicked();
    void closeRequested();
    void closeOtherTabsRequested();
    void moved(const QPoint &globalPos);
    void elidedChanged(bool elided);

private Q_SLOTS:
    void detachDockWidget();

protected:
    virtual void mousePressEvent(QMouseEvent *e) override;
    virtual void mouseReleaseEvent(QMouseEvent *e) override;
    virtual void mouseMoveEvent(QMouseEvent *e) override;
    virtual void mouseDoubleClickEvent(QMouseEvent *e) override;
    virtual void contextMenuEvent(QContextMenuEvent *e) override;

private:
    QX_DECLARE_PRIVATE(DockTab)
};

QX_DOCK_END_NAMESPACE
