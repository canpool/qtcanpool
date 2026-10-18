/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qxdock_global.h"
#include "dockcontainer.h"
#include "dockwidget.h"

class QSettings;
class QMimeData;
class QDragEnterEvent;
class QDragMoveEvent;
class QDragLeaveEvent;
class QDropEvent;

QX_DOCK_BEGIN_NAMESPACE

class DockPanel;
class DockWidget;
class DockFloatingContainer;
class DockOverlay;
class DockContainer;
class DockFocusController;
class DockAutoHideContainer;

class DockWindowPrivate;

/**
 * The central dock window that maintains the complete docking system.
 * The dock window uses an internal stylesheet to style its components
 * like splitters, tabs and buttons. If you want to disable this stylesheet
 * because your application uses its own, just call the function for
 * settings the stylesheet with an empty string.
 * \code
 * DockWindow->setStyleSheet("");
 * \endcode
 **/
class QX_DOCK_EXPORT DockWindow : public DockContainer
{
    Q_OBJECT
public:
    using Super = DockContainer;
public:
    explicit DockWindow(QWidget *parent = nullptr);
    virtual ~DockWindow();

    DockPanel *addDockWidget(Qx::DockWidgetArea area, DockWidget *w, DockPanel *p = nullptr, int index = -1);
    DockPanel *addDockWidgetTab(Qx::DockWidgetArea area, DockWidget *w);
    DockPanel *addDockWidgetTab(DockWidget *w, DockPanel *p, int index = -1);
    DockPanel *addDockWidgetToContainer(Qx::DockWidgetArea area, DockWidget *w, DockContainer *container);
    void removeDockWidget(DockWidget *w);
    DockWidget *findDockWidget(const QString &objectName) const;

    DockAutoHideContainer *addAutoHideDockWidget(Qx::DockSideBarArea area, DockWidget *w);
    DockAutoHideContainer *addAutoHideDockWidgetToContainer(Qx::DockSideBarArea area, DockWidget *w,
                                                            DockContainer *container);

    DockFloatingContainer *addDockWidgetFloating(DockWidget *w);

    const QList<DockContainer *> dockContainers() const;
    QMap<QString, DockWidget *> dockWidgetsMap() const;

    DockWidget *centralWidget() const;
    DockPanel *setCentralWidget(DockWidget *widget);

    bool isLeavingMinimizedState() const;

    Qt::ToolButtonStyle dockWidgetToolBarStyle(DockWidget::State state) const;
    void setDockWidgetToolBarStyle(Qt::ToolButtonStyle style, DockWidget::State state);
    QSize dockWidgetToolBarIconSize(DockWidget::State state) const;
    void setDockWidgetToolBarIconSize(const QSize &iconSize, DockWidget::State state);

    DockWidget::DockWidgetFeatures globallyLockedDockWidgetFeatures() const;
    void lockDockWidgetFeaturesGlobally(DockWidget::DockWidgetFeatures features = DockWidget::GloballyLockableFeatures);

    QList<int> splitterSizes(DockPanel *panel) const;
    void setSplitterSizes(DockPanel *panel, const QList<int> &sizes);

    QByteArray saveState(int version = 0) const;
    bool restoreState(const QByteArray &state, int version = 0);
    bool saveStateToFile(const QString &fileName, int version = 0) const;
    bool restoreStateFromFile(const QString &fileName, int version = 0);
    bool isRestoringState() const;

    void addPerspective(const QString &uniquePrespectiveName);
    void removePerspective(const QString &name);
    void removePerspectives(const QStringList &names);
    QStringList perspectiveNames() const;
    void savePerspectives(QSettings &settings) const;
    void loadPerspectives(QSettings &settings);

Q_SIGNALS:
    void dockWidgetAdded(DockWidget *w);
    void dockWidgetAboutToBeRemoved(DockWidget *w);
    void dockWidgetRemoved(DockWidget *w);
    void floatingWidgetCreated(DockFloatingContainer *floatingWidget);
    void focusedDockWidgetChanged(DockWidget *old, DockWidget *now);
    void restoringState();
    void stateRestored();
    void perspectiveListChanged();
    void perspectiveListLoaded();
    void perspectivesRemoved();
    void openingPerspective(const QString &perspectiveName);
    void perspectiveOpened(const QString &perspectiveName);

    /**
     * Emitted when a native drag carrying a dock widget (see DockDrag) is
     * dropped onto this window. The dock widget named by the payload may be one
     * this window owns already, may not exist at all because the drag came from
     * another process, or may belong to another window of the application:
     * \a dockWidgetName is what identifies it, and \a globalPos is where it was
     * dropped. Only the application can build a dock widget that is not here
     * yet, which is why the drop is reported instead of being carried out.
     *
     * A dock widget of this window that was closed is brought back before the
     * signal is emitted; nothing else is repositioned.
     */
    void dockWidgetDropped(const QString &dockWidgetName, const QPoint &globalPos);

public Q_SLOTS:
    void setDockWidgetFocused(DockWidget *w);
    void endLeavingMinimizedState();
    void openPerspective(const QString &perspectiveName);
    void hideWindowAndFloatingWidgets();
protected:
    void registerDockContainer(DockContainer *container);
    void removeDockContainer(DockContainer *container);
    void registerFloatingWidget(DockFloatingContainer *floatingWidget);
    void removeFloatingWidget(DockFloatingContainer *floatingWidget);

    DockOverlay *containerOverlay() const;
    DockOverlay *panelOverlay() const;

    void notifyDockAreaRelocation(QWidget *droppedWidget);
    void notifyFloatingWidgetDrop(DockFloatingContainer *floatingWidget);

    /*!
     * The window's focus controller, or null when DockManager::FocusHighlighting
     * is off - the controller is the only thing that sets the "focused" property
     * the sheet's focused rules ask for, so it is not constructed at all when
     * nothing is going to read it.
     */
    DockFocusController *dockFocusController() const;

    void restoreHiddenFloatingWidgets();
protected:
    bool eventFilter(QObject *obj, QEvent *e) override;
    virtual void showEvent(QShowEvent *e) override;

    // Dock widgets arrive here only when a native drag - one started through
    // DockDrag - is dropped on the window. The in-process mouse handling never
    // reaches these, so refusing a payload that is not ours keeps the two paths
    // from meeting.
    void dragEnterEvent(QDragEnterEvent *e) override;
    void dragMoveEvent(QDragMoveEvent *e) override;
    void dragLeaveEvent(QDragLeaveEvent *e) override;
    void dropEvent(QDropEvent *e) override;

    /**
     * Carries out a drop of a dock widget payload (see DockDrag) at \a dropPos,
     * which is in this window's coordinates, and returns whether the payload was
     * one: false means nothing happened at all.
     *
     * dropEvent() is a shell around this - the position it takes differs between
     * Qt 5 and 6, and nothing else differs - so an application that wants to
     * decide itself which dock widgets may arrive reimplements this rather than
     * the event handler. The base implementation lets any of them in.
     */
    virtual bool handleDockWidgetDrop(const QMimeData *data, const QPoint &dropPos);
private:
    QByteArray saveStateData(bool compress, int version) const;

    QX_DECLARE_PRIVATE(DockWindow)
    friend class DockContainer;
    friend class DockContainerPrivate;
    friend class DockFloatingContainer;
    friend class DockFloatingContainerPrivate;
    friend class DockTitleBar;
    friend class DockFloatingPreview;
    friend class DockFloatingPreviewPrivate;
    friend class DockTabPrivate;
    friend class DockSideBar;
    friend class DockSideTabPrivate;
};

QX_DOCK_END_NAMESPACE
