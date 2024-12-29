/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qxdock_global.h"
#include <QScrollArea>

class QXmlStreamWriter;

QX_DOCK_BEGIN_NAMESPACE

class DockContainer;
class DockAutoHideContainer;
class DockWidget;
class DockSideTab;

class DockSideBarPrivate;

/**
 * Side tab bar widget that is shown at the edges of a dock container.
 * The tab bar is only visible, if it contains visible content, that means if
 * it contains visible tabs. If it is empty or all tabs are hidden, then the
 * side bar is also hidden. As soon as one single tab becomes visible, this
 * tab bar will be shown.
 * The DockSideBar uses a QScrollArea here, to enable proper resizing.
 * If the side bar contains many tabs, then the tabs are simply clipped - this
 * is the same like in visual studio
 */
class QX_DOCK_EXPORT DockSideBar : public QScrollArea
{
    Q_OBJECT
    Q_PROPERTY(int sideBarArea READ sideBarArea)
    Q_PROPERTY(Qt::Orientation orientation READ orientation)
    Q_PROPERTY(int spacing READ spacing WRITE setSpacing)
public:
    explicit DockSideBar(DockContainer *parent, Qx::DockSideBarArea area);
    virtual ~DockSideBar();

    void removeTab(DockSideTab *sideTab);

    DockAutoHideContainer *insertDockWidget(int index, DockWidget *w);

    void addAutoHideWidget(DockAutoHideContainer *autoHideWidget, int index = Qx::DockTabDefaultInsertIndex);
    void removeAutoHideWidget(DockAutoHideContainer *autoHideWidget);

    Qt::Orientation orientation() const;

    DockSideTab *tab(int index) const;
    int tabAt(const QPoint &pos) const;
    int tabInsertIndexAt(const QPoint &pos) const;
    int indexOfTab(const DockSideTab &tab) const;
    int count() const;

    int visibleTabCount() const;
    bool hasVisibleTabs() const;

    Qx::DockSideBarArea sideBarArea() const;

    virtual QSize minimumSizeHint() const override;
    virtual QSize sizeHint() const override;

    int spacing() const;
    void setSpacing(int spacing);

protected:
    void insertTab(int index, DockSideTab *sideTab);
    void saveState(QXmlStreamWriter &s) const;

protected:
    virtual bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QX_DECLARE_PRIVATE(DockSideBar)
    friend class DockContainerPrivate;
};

QX_DOCK_END_NAMESPACE
