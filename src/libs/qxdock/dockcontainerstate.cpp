/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "dockcontainer_p.h"
#include "dockwindow.h"
#include "docksplitter.h"
#include "dockpanel.h"
#include "dockutils.h"
#include "dockstatereader.h"

#include <QGridLayout>
#include <QTimer>
#include <QXmlStreamWriter>
#include <QApplication>
#include <QPointer>
#include <QTextStream>

QX_DOCK_BEGIN_NAMESPACE

void DockContainerPrivate::saveChildNodesState(QXmlStreamWriter &s, QWidget *w) const
{
    QSplitter *splitter = qobject_cast<QSplitter *>(w);
    if (splitter) {
        s.writeStartElement("Splitter");
        s.writeAttribute("Orientation", (splitter->orientation() == Qt::Horizontal) ? "|" : "-");
        s.writeAttribute("Count", QString::number(splitter->count()));
        for (int i = 0; i < splitter->count(); ++i) {
            saveChildNodesState(s, splitter->widget(i));
        }

        s.writeStartElement("Sizes");
        for (auto Size : splitter->sizes()) {
            s.writeCharacters(QString::number(Size) + " ");
        }
        s.writeEndElement();   // Sizes

        s.writeEndElement();   // Splitter
    } else {
        DockPanel *panel = qobject_cast<DockPanel *>(w);
        if (panel) {
            panel->saveState(s);
        }
    }
}

void DockContainerPrivate::saveAutoHideWidgetsState(QXmlStreamWriter &s) const
{
    for (const auto sideTabBar : m_sideTabBarWidgets.values()) {
        if (!sideTabBar->count()) {
            continue;
        }

        sideTabBar->saveState(s);
    }
}

bool DockContainerPrivate::restoreChildNodes(DockStateReader &s, QWidget *&createdWidget, bool testing)
{
    bool result = true;
    while (s.readNextStartElement()) {
        if (s.name() == QLatin1String("Splitter")) {
            result = restoreSplitter(s, createdWidget, testing);
        } else if (s.name() == QLatin1String("Area")) {
            result = restoreDockArea(s, createdWidget, testing);
        } else if (s.name() == QLatin1String("SideBar")) {
            result = restoreSideBar(s, createdWidget, testing);
        } else {
            s.skipCurrentElement();
        }
    }

    return result;
}

bool DockContainerPrivate::restoreSplitter(DockStateReader &s, QWidget *&createdWidget, bool testing)
{
    bool ok;
    QString orientationStr = s.attributes().value("Orientation").toString();

    // Check if the orientation string is right
    if (!orientationStr.startsWith("|") && !orientationStr.startsWith("-")) {
        return false;
    }

    // The "|" shall indicate a vertical splitter handle which in turn means
    // a Horizontal orientation of the splitter layout.
    bool horizontalSplitter = orientationStr.startsWith("|");
    // In version 0 we had a small bug. The "|" indicated a vertical orientation,
    // but this is wrong, because only the splitter handle is vertical, the
    // layout of the splitter is a horizontal layout. We fix this here
    if (s.fileVersion() == 0) {
        horizontalSplitter = !horizontalSplitter;
    }

    int orientation = horizontalSplitter ? Qt::Horizontal : Qt::Vertical;
    int widgetCount = s.attributes().value("Count").toInt(&ok);
    if (!ok) {
        return false;
    }

    QSplitter *splitter = nullptr;
    if (!testing) {
        splitter = newSplitter(static_cast<Qt::Orientation>(orientation));
    }
    bool visible = false;
    QList<int> sizes;
    while (s.readNextStartElement()) {
        QWidget *childNode = nullptr;
        bool result = true;
        if (s.name() == QLatin1String("Splitter")) {
            result = restoreSplitter(s, childNode, testing);
        } else if (s.name() == QLatin1String("Area")) {
            result = restoreDockArea(s, childNode, testing);
        } else if (s.name() == QLatin1String("Sizes")) {
            QString sSizes = s.readElementText().trimmed();

            QTextStream TextStream(&sSizes);
            while (!TextStream.atEnd()) {
                int value;
                TextStream >> value;
                sizes.append(value);
            }
        } else {
            s.skipCurrentElement();
        }

        if (!result) {
            return false;
        }

        if (testing || !childNode) {
            continue;
        }

        splitter->addWidget(childNode);
        visible |= childNode->isVisibleTo(splitter);
    }
    if (!testing) {
        updateSplitterHandles(splitter);
    }

    if (sizes.count() != widgetCount) {
        return false;
    }

    if (!testing) {
        if (!splitter->count()) {
            delete splitter;
            splitter = nullptr;
        } else {
            splitter->setSizes(sizes);
            splitter->setVisible(visible);
        }
        createdWidget = splitter;
    } else {
        createdWidget = nullptr;
    }

    return true;
}

bool DockContainerPrivate::restoreDockArea(DockStateReader &s, QWidget *&createdWidget, bool testing)
{
    Q_Q(DockContainer);
    DockPanel *panel = nullptr;
    auto result = DockPanel::restoreState(s, panel, testing, q);
    if (result && panel) {
        appendPanels({panel});
    }
    createdWidget = panel;
    return result;
}

bool DockContainerPrivate::restoreSideBar(DockStateReader &s, QWidget *&createdWidget, bool testing)
{
    Q_UNUSED(createdWidget)
    Q_Q(DockContainer);
    // Simply ignore side bar auto hide widgets from saved state if
    // auto hide support is disabled
    if (!DockManager::testAutoHideConfigFlag(DockManager::AutoHideFeatureEnabled)) {
        return true;
    }

    bool ok;
    auto area = (Qx::DockSideBarArea)s.attributes().value("Area").toInt(&ok);
    if (!ok) {
        return false;
    }

    while (s.readNextStartElement()) {
        if (s.name() != QLatin1String("Widget")) {
            continue;
        }

        auto name = s.attributes().value("Name");
        if (name.isEmpty()) {
            return false;
        }

        bool ok;
        bool closed = s.attributes().value("Closed").toInt(&ok);
        if (!ok) {
            return false;
        }

        int Size = s.attributes().value("Size").toInt(&ok);
        if (!ok) {
            return false;
        }

        s.skipCurrentElement();
        DockWidget *dockWidget = m_window->findDockWidget(name.toString());
        if (!dockWidget || testing) {
            continue;
        }

        auto sideBar = q->autoHideSideBar(area);
        DockAutoHideContainer *autoHideContainer;
        if (dockWidget->isAutoHide()) {
            autoHideContainer = dockWidget->autoHideContainer();
            if (autoHideContainer->autoHideSideBar() != sideBar) {
                sideBar->addAutoHideWidget(autoHideContainer);
            }
        } else {
            autoHideContainer = sideBar->insertDockWidget(-1, dockWidget);
        }
        autoHideContainer->setSize(Size);
        dockWidget->setProperty(internal::ClosedProperty, closed);
        dockWidget->setProperty(internal::DirtyProperty, false);
    }

    return true;
}

/* DockContainer */

void DockContainer::saveState(QXmlStreamWriter &s) const
{
    Q_D(const DockContainer);
    s.writeStartElement("Container");
    s.writeAttribute("Floating", QString::number(isFloating() ? 1 : 0));
    if (isFloating()) {
        DockFloatingContainer *floatingWidget = this->floatingWidget();
        QByteArray geometry = floatingWidget->saveGeometry();
        s.writeTextElement("Geometry", geometry.toHex(' '));
    }
    d->saveChildNodesState(s, d->m_rootSplitter);
    d->saveAutoHideWidgetsState(s);
    s.writeEndElement();
}

bool DockContainer::restoreState(DockStateReader &s, bool testing)
{
    Q_D(DockContainer);
    bool isFloating = s.attributes().value("Floating").toInt();

    QWidget *newRootSplitter{};
    if (!testing) {
        d->m_panels.clear();
        std::fill(std::begin(d->m_lastAddedPanelCache), std::end(d->m_lastAddedPanelCache), nullptr);
    }

    if (isFloating) {
        if (!s.readNextStartElement() || s.name() != QLatin1String("Geometry")) {
            return false;
        }

        QByteArray geometryString = s.readElementText(DockStateReader::ErrorOnUnexpectedElement).toLocal8Bit();
        QByteArray geometry = QByteArray::fromHex(geometryString);
        if (geometry.isEmpty()) {
            return false;
        }

        if (!testing) {
            DockFloatingContainer *floatingWidget = this->floatingWidget();
            if (floatingWidget) {
                floatingWidget->restoreGeometry(geometry);
            }
        }
    }

    if (!d->restoreChildNodes(s, newRootSplitter, testing)) {
        return false;
    }

    if (testing) {
        return true;
    }

    // If the root splitter is empty, rostoreChildNodes returns a 0 pointer
    // and we need to create a new empty root splitter
    if (!newRootSplitter) {
        newRootSplitter = d->newSplitter(Qt::Horizontal);
    }

    QLayoutItem *li = d->m_layout->replaceWidget(d->m_rootSplitter, newRootSplitter);
    auto oldRoot = d->m_rootSplitter;
    d->m_rootSplitter = qobject_cast<DockSplitter *>(newRootSplitter);
    oldRoot->deleteLater();
    delete li;

    return true;
}

QX_DOCK_END_NAMESPACE
