/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include "qxdock_global.h"
#include <QFrame>

QX_DOCK_BEGIN_NAMESPACE

class DockWidget;
class DockPanel;

class DockTabPrivate;

class QX_DOCK_EXPORT DockTab : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(bool activeTab READ isActive WRITE setActive NOTIFY activeTabChanged)
    Q_PROPERTY(QSize iconSize READ iconSize WRITE setIconSize)
public:
    using Super = QFrame;
public:
    explicit DockTab(DockWidget *w, QWidget *parent = nullptr);
    virtual ~DockTab();

    bool isActive() const;
    void setActive(bool active);

    void setDockPanel(DockPanel *panel);
    DockPanel *dockPanel() const;
    DockWidget *dockWidget() const;

    const QIcon &icon() const;
    void setIcon(const QIcon &icon);

    QSize iconSize() const;
    void setIconSize(const QSize &size);

    QString text() const;
    void setText(const QString &title);

    bool isTitleElided() const;
    bool isClosable() const;

    void updateStyle();

    void setElideMode(Qt::TextElideMode mode);
    Qx::DockDragState dragState() const;

Q_SIGNALS:
    void activeTabChanged();
    void clicked();
    void closeRequested();
    void closeOtherTabsRequested();
    void moved(const QPoint &globalPos);
    void elidedChanged(bool elided);

public Q_SLOTS:
    virtual void setVisible(bool visible) override;

private Q_SLOTS:
    void detachDockWidget();
    void autoHideDockWidget();
    void onAutoHideToActionClicked();
    void onDockWidgetFeaturesChanged();

protected:
    virtual bool event(QEvent *e) override;
    virtual void mousePressEvent(QMouseEvent *e) override;
    virtual void mouseReleaseEvent(QMouseEvent *e) override;
    virtual void mouseMoveEvent(QMouseEvent *e) override;
    virtual void mouseDoubleClickEvent(QMouseEvent *e) override;
    virtual void contextMenuEvent(QContextMenuEvent *e) override;

private:
    QX_DECLARE_PRIVATE(DockTab)
    friend class DockWidget;
};

QX_DOCK_END_NAMESPACE
