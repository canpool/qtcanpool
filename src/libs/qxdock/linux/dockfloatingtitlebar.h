/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qxdock_global.h"
#include <QFrame>
#include <QIcon>

QX_DOCK_BEGIN_NAMESPACE

class DockFloatingContainer;

class DockFloatingTitleBarPrivate;

class QX_DOCK_EXPORT DockFloatingTitleBar : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(QIcon maximizeIcon READ maximizeIcon WRITE setMaximizeIcon)
    Q_PROPERTY(QIcon normalIcon READ normalIcon WRITE setNormalIcon)
public:
    using Super = QFrame;
public:
    explicit DockFloatingTitleBar(DockFloatingContainer *parent = nullptr);
    virtual ~DockFloatingTitleBar();

    void enableCloseButton(bool enable);
    void setTitle(const QString &text);
    void updateStyle();
    void setMaximizedIcon(bool maximized);

Q_SIGNALS:
    void closeRequested();
    void maximizeRequested();

protected:
    QIcon maximizeIcon() const;
    void setMaximizeIcon(const QIcon &icon);
    QIcon normalIcon() const;
    void setNormalIcon(const QIcon &icon);

protected:
    virtual void mousePressEvent(QMouseEvent *e) override;
    virtual void mouseReleaseEvent(QMouseEvent *e) override;
    virtual void mouseMoveEvent(QMouseEvent *e) override;
    virtual void mouseDoubleClickEvent(QMouseEvent *e) override;

private:
    QX_DECLARE_PRIVATE(DockFloatingTitleBar)
};

QX_DOCK_END_NAMESPACE
