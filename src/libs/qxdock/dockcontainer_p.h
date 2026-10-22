#pragma once

#include "dockcontainer.h"
#include "dockwindow.h"
#include "docksplitter.h"
#include "dockpanel.h"
#include "dockutils.h"
#include "dockmanager.h"
#include "dockfloatingcontainer.h"
#include "dockoverlay.h"
#include "dockautohidecontainer.h"
#include "docksidebar.h"
#include "docksidetab.h"
#include "dockstatereader.h"

#include <QGridLayout>
#include <QMap>
#include <QPointer>
#include <QList>
#include <QSplitter>
#include <QTimer>
#include <QWidget>
#include <QXmlStreamWriter>

QX_DOCK_BEGIN_NAMESPACE

class DockContainerPrivate
{
public:
    QX_DECLARE_PUBLIC(DockContainer)
public:
    DockContainerPrivate();

    void init();
    void createRootSplitter();

    void onVisibleDockAreaCountChanged();
    void emitDockAreasRemoved();
    void emitDockAreasAdded();

    DockSplitter *newSplitter(Qt::Orientation orientation, QWidget *parent = nullptr);
    void updateSplitterHandles(QSplitter *s);
    void adjustSplitterSizesOnInsertion(QSplitter *s, qreal lastRatio = 1.0);

    bool widgetResizesWithContainer(QWidget *w);
    void onDockAreaViewToggled(bool visible);

    DockPanel *addDockWidgetToContainer(Qx::DockWidgetArea area, DockWidget *w);
    DockPanel *addDockWidgetToPanel(Qx::DockWidgetArea area, DockWidget *w, DockPanel *targetPanel, int index = -1);

    void addDockPanel(DockPanel *np, Qx::DockWidgetArea area = Qx::CenterDockWidgetArea);
    void addDockPanelsToList(const QList<DockPanel *> ps);
    void appendPanels(const QList<DockPanel *> ps);

    void dropIntoContainer(DockFloatingContainer *floatingWidget, Qx::DockWidgetArea area);
    void dropIntoCenterOfPanel(DockFloatingContainer *floatingWidget, DockPanel *targetPanel, int tabIndex = 0);
    void dropIntoPanel(DockFloatingContainer *floatingWidget, DockPanel *targetPanel, Qx::DockWidgetArea area,
                       int tabIndex = 0);

    void moveToContainer(QWidget *widget, Qx::DockWidgetArea area);
    void moveIntoCenterOfSection(QWidget *widget, DockPanel *targetPanel, int tabIndex = 0);
    void moveToNewSection(QWidget *widget, DockPanel *targetPanel, Qx::DockWidgetArea area, int tabIndex = 0);

    void dropIntoAutoHideSideBar(DockFloatingContainer *floatingWidget, Qx::DockWidgetArea area);
    void moveToAutoHideSideBar(QWidget *widget, Qx::DockWidgetArea area, int tabIndex = Qx::DockTabDefaultInsertIndex);

    void saveChildNodesState(QXmlStreamWriter &s, QWidget *w) const;
    void saveAutoHideWidgetsState(QXmlStreamWriter &s) const;

    bool restoreChildNodes(DockStateReader &s, QWidget *&createdWidget, bool testing);
    bool restoreSplitter(DockStateReader &s, QWidget *&createdWidget, bool testing);
    bool restoreDockArea(DockStateReader &s, QWidget *&createdWidget, bool testing);
    bool restoreSideBar(DockStateReader &s, QWidget *&createdWidget, bool testing);
public:
    DockWindow *m_window = nullptr;
    QGridLayout *m_layout = nullptr;
    DockSplitter *m_rootSplitter = nullptr;
    QList<QPointer<DockPanel>> m_panels;
    DockPanel *m_lastAddedPanelCache[5];
    DockPanel *m_topLevelPanel = nullptr;
    bool m_isFloating = false;
    unsigned int m_zOrderIndex = 0;
    QMap<Qx::DockSideBarArea, DockSideBar *> m_sideTabBarWidgets;
    QList<DockAutoHideContainer *> m_autoHideWidgets;
    DockSideTab *m_delayedAutoHideTab;
    QTimer m_delayedAutoHideTimer;
};

inline void insertWidgetIntoSplitter(QSplitter *s, QWidget *w, bool append)
{
    if (append) {
        s->addWidget(w);
    } else {
        s->insertWidget(0, w);
    }
}

inline int areaIdToIndex(Qx::DockWidgetArea area)
{
    switch (area) {
    case Qx::LeftDockWidgetArea:
        return 0;
    case Qx::RightDockWidgetArea:
        return 1;
    case Qx::TopDockWidgetArea:
        return 2;
    case Qx::BottomDockWidgetArea:
        return 3;
    case Qx::CenterDockWidgetArea:
        return 4;
    default:
        return 4;
    }
}

QX_DOCK_END_NAMESPACE
