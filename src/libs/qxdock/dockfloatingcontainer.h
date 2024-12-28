/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include "qxdock_global.h"
#include "dockfloatingwidget.h"

#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
#include <QDockWidget>
#define DockFloatingContainerBase QDockWidget
#else
#include <QWidget>
#define DockFloatingContainerBase QWidget
#endif

QX_DOCK_BEGIN_NAMESPACE

class DockWindow;
class DockPanel;
class DockWidget;
class DockContainer;
class DockStateReader;

class DockFloatingContainerPrivate;

/**
 * This implements a floating widget that is a dock container that accepts
 * docking of dock widgets like the main window and that can be docked into
 * another dock container.
 * Every floating window of the docking system is a DockFloatingContainer.
 */
class QX_DOCK_EXPORT DockFloatingContainer : public DockFloatingContainerBase
                                           , public DockFloatingWidget
{
    Q_OBJECT
public:
    using Super = DockFloatingContainerBase;
public:
    explicit DockFloatingContainer(DockWindow *window);
    explicit DockFloatingContainer(DockPanel *panel);
    explicit DockFloatingContainer(DockWidget *widget);
    virtual ~DockFloatingContainer();

    bool isClosable() const;

    DockContainer *dockContainer() const;

    DockWidget *topLevelDockWidget() const;
    QList<DockWidget *> dockWidgets() const;
    bool hasTopLevelDockWidget() const;

    void finishDropOperation();

#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
    void onMaximizeRequest();
    void showNormal(bool fixGeometry=false);
    void showMaximized();
    bool isMaximized() const;
    void show();
    bool hasNativeTitleBar();
#endif

private Q_SLOTS:
    void onDockAreasAddedOrRemoved();
    void onDockAreaCurrentChanged(int index);

protected:
    virtual void startFloating(const QPoint &dragStartMousePos, const QSize &size,
        Qx::DockDragState dragState, QWidget *mouseEventHandler) override;
    virtual void finishDragging() override;
    void moveFloating() override;

    void deleteContent();
    void updateWindowTitle();

    bool restoreState(DockStateReader &stream, bool testing);

protected:
    virtual void closeEvent(QCloseEvent *event) override;
    virtual void hideEvent(QHideEvent *event) override;
    virtual void showEvent(QShowEvent *event) override;
    virtual void changeEvent(QEvent *event) override;

#ifdef Q_OS_MACOS
    virtual bool event(QEvent *e) override;
    virtual void moveEvent(QMoveEvent *e) override;
#elif defined(Q_OS_UNIX)
    virtual void moveEvent(QMoveEvent *e) override;
    virtual void resizeEvent(QResizeEvent *e) override;
    virtual bool event(QEvent *e) override;
#endif

#ifdef Q_OS_WIN
    /**
     * Native event filter for handling WM_MOVING messages on Windows
     */
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;
#else
    virtual bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;
#endif
#endif

private:
    QX_DECLARE_PRIVATE(DockFloatingContainer)
    friend class DockWindow;
    friend class DockWidget;
    friend class DockPanel;
    friend class DockWindowPrivate;
    friend class DockFloatingTitleBar;
};

QX_DOCK_END_NAMESPACE
