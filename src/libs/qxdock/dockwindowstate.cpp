/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "dockwindow.h"
#include "dockwindow_p.h"
#include "dockpanel.h"
#include "dockwidget.h"
#include "dockfloatingcontainer.h"
#include "dockoverlay.h"
#include "dockfocuscontroller.h"
#include "dockmanager.h"
#include "dockautohidecontainer.h"
#include "dockutils.h"
#include "dockstatereader.h"

#include <QXmlStreamWriter>
#include <QFile>
#include <QPointer>
#include <QAction>
#include <QDebug>

QX_DOCK_BEGIN_NAMESPACE

enum StateFileVersion {
    InitialVersion = 0,         //!< InitialVersion
    Version1 = 1,               //!< Version1
    CurrentVersion = Version1   //!< CurrentVersion
};

DockWindowPrivate::DockWindowPrivate()
{
}

void DockWindowPrivate::init()
{
    Q_Q(DockWindow);

    m_containerOverlay = new DockOverlay(q, DockOverlay::PanelOverlayMode);
    m_panelOverlay = new DockOverlay(q, DockOverlay::ContainerOverlayMode);

    if (DockManager::testConfigFlag(DockManager::FocusHighlighting)) {
        m_focusController = new DockFocusController(q);
    }
}

void DockWindowPrivate::mapDockWidget(DockWidget *w)
{
    const QString &objname = w->objectName();
    DockWidget *oldWidget = m_dockWidgetsMap.value(objname, nullptr);
    if (oldWidget) {
        qWarning() << QString("Object name (%1) of DockWidget already exists.").arg(objname);
    }
    m_dockWidgetsMap.insert(objname, w);
}

bool DockWindowPrivate::checkFormat(const QByteArray &state, int version)
{
    return restoreStateFromXml(state, version, internal::RestoreTesting);
}

bool DockWindowPrivate::restoreStateFromXml(const QByteArray &state, int version, bool testing)
{
    Q_Q(DockWindow);

    if (state.isEmpty()) {
        return false;
    }
    DockStateReader s(state);
    s.readNextStartElement();
    if (s.name() != QLatin1String("QxDock")) {
        return false;
    }
    bool ok;
    int v = s.attributes().value("Version").toInt(&ok);
    if (!ok || v > CurrentVersion) {
        return false;
    }
    s.setFileVersion(v);

    // Older files do not support UserVersion but we still want to load them so
    // we first test if the attribute exists
    if (!s.attributes().value("UserVersion").isEmpty()) {
        v = s.attributes().value("UserVersion").toInt(&ok);
        if (!ok || v != version) {
            return false;
        }
    }

    bool result = true;

    if (m_centralWidget) {
        const auto centralWidgetAttribute = s.attributes().value("CentralWidget");
        // If we have a central widget but a state without central widget, then
        // something is wrong.
        if (centralWidgetAttribute.isEmpty()) {
            qWarning() << "Dock window has central widget but saved state does not have central widget.";
            return false;
        }

        // If the object name of the central widget does not match the name of the
        // saved central widget, the something is wrong
        if (m_centralWidget->objectName() != centralWidgetAttribute.toString()) {
            qWarning() << "Object name of central widget does not match name of central widget in saved state.";
            return false;
        }
    }

    int dockContainerCount = 0;
    while (s.readNextStartElement()) {
        if (s.name() == QLatin1String("Container")) {
            result = restoreContainer(dockContainerCount, s, testing);
            if (!result) {
                break;
            }
            dockContainerCount++;
        }
    }

    if (!testing) {
        // Delete remaining empty floating widgets
        int floatingWidgetIndex = dockContainerCount - 1;
        for (int i = floatingWidgetIndex; i < m_floatingContainers.count(); ++i) {
            DockFloatingContainer *floatingWidget = m_floatingContainers[i];
            if (!floatingWidget)
                continue;
            q->removeDockContainer(floatingWidget->dockContainer());
            floatingWidget->deleteLater();
        }
    }

    return result;
}

bool DockWindowPrivate::restoreContainer(int index, DockStateReader &stream, bool testing)
{
    Q_Q(DockWindow);
    if (testing) {
        index = 0;
    }

    bool result = false;
    if (index >= m_containers.count()) {
        DockFloatingContainer *floatingWidget = new DockFloatingContainer(q);
        result = floatingWidget->restoreState(stream, testing);
    } else {
        auto container = m_containers[index];
        if (container->isFloating()) {
            result = container->floatingWidget()->restoreState(stream, testing);
        } else {
            result = container->restoreState(stream, testing);
        }
    }

    return result;
}

bool DockWindowPrivate::restoreState(const QByteArray &state, int version)
{
    QByteArray s = state.startsWith("<?xml") ? state : qUncompress(state);
    if (!checkFormat(s, version)) {
        return false;
    }

    // Hide updates of floating widgets from use
    hideFloatingWidgets();
    markDockWidgetsDirty();

    if (!restoreStateFromXml(s, version)) {
        return false;
    }

    restoreDockWidgetsOpenState();
    restoreDockAreasIndices();
    emitTopLevelEvents();

    return true;
}

void DockWindowPrivate::restoreDockWidgetsOpenState()
{
    // All dock widgets, that have not been processed in the restore state
    // function are invisible to the user now and have no assigned dock area
    // They do not belong to any dock container, until the user toggles the
    // toggle view action the next time
    for (auto dockWidget : m_dockWidgetsMap) {
        if (dockWidget->property(internal::DirtyProperty).toBool()) {
            // If the DockWidget is an auto hide widget that is not assigned yet,
            // then we need to delete the auto hide container now
            if (dockWidget->isAutoHide()) {
                dockWidget->autoHideContainer()->cleanupAndDelete();
            }
            dockWidget->flagAsUnassigned();
            Q_EMIT dockWidget->viewToggled(false);
        } else {
            dockWidget->toggleViewInternal(!dockWidget->property(internal::ClosedProperty).toBool());
        }
    }
}

void DockWindowPrivate::restoreDockAreasIndices()
{
    Q_Q(DockWindow);
    // Now all dock panels are properly restored and we setup the index of
    // The dock panels because the previous toggleView() action has changed
    // the dock panel index
    for (auto dockContainer : m_containers) {
        for (int i = 0; i < dockContainer->dockPanelCount(); ++i) {
            DockPanel *panel = dockContainer->dockPanel(i);
            QString dockWidgetName = panel->property("currentDockWidget").toString();
            DockWidget *dockWidget = nullptr;
            if (!dockWidgetName.isEmpty()) {
                dockWidget = q->findDockWidget(dockWidgetName);
            }

            if (!dockWidget || dockWidget->isClosed()) {
                int index = panel->indexOfFirstOpenDockWidget();
                if (index < 0) {
                    continue;
                }
                panel->setCurrentIndex(index);
            } else {
                panel->internalSetCurrentDockWidget(dockWidget);
            }
        }
    }
}

void DockWindowPrivate::emitTopLevelEvents()
{
    // Finally we need to send the topLevelChanged() signals for all dock
    // widgets if top level changed
    for (auto dockContainer : m_containers) {
        DockWidget *topLevelWidget = dockContainer->topLevelDockWidget();
        if (topLevelWidget) {
            topLevelWidget->emitTopLevelChanged(true);
        } else {
            for (int i = 0; i < dockContainer->dockPanelCount(); ++i) {
                auto panel = dockContainer->dockPanel(i);
                for (auto dockWidget : panel->dockWidgets()) {
                    dockWidget->emitTopLevelChanged(false);
                }
            }
        }
    }
}

void DockWindowPrivate::hideFloatingWidgets()
{
    // Hide updates of floating widgets from user
    for (auto floatingWidget : m_floatingContainers) {
        if (floatingWidget) {
            floatingWidget->hide();
        }
    }
}

void DockWindowPrivate::markDockWidgetsDirty()
{
    for (auto dockWidget : m_dockWidgetsMap) {
        dockWidget->setProperty(internal::DirtyProperty, true);
    }
}

/**
 * Saves the current state of the dockwindow and all its dock widgets
 * into the returned QByteArray.
 * The XmlMode enables / disables the auto formatting for the XmlStreamWriter.
 * If auto formatting is enabled, the output is intended and line wrapped.
 * The XmlMode XmlAutoFormattingDisabled is better if you would like to have
 * a more compact XML output - i.e. for storage in ini files.
 * The version number is stored as part of the data.
 * To restore the saved state, pass the return value and version number
 * to restoreState().
 * \see restoreState()
 */
QByteArray DockWindow::saveState(int version) const
{
    return saveStateData(DockManager::configFlags().testFlag(DockManager::XmlCompressionEnabled), version);
}

/**
 * Serializes the current state of the dock window and all its dock widgets
 * into the returned QByteArray. When \a compress is true the XML document is
 * compressed with qCompress(), otherwise the plain XML text is returned. The
 * auto formatting of the XML stream is still controlled by the global
 * XmlAutoFormattingEnabled configuration flag.
 */
QByteArray DockWindow::saveStateData(bool compress, int version) const
{
    Q_D(const DockWindow);
    QByteArray xmldata;
    QXmlStreamWriter s(&xmldata);
    auto configFlags = DockManager::configFlags();
    s.setAutoFormatting(configFlags.testFlag(DockManager::XmlAutoFormattingEnabled));

    s.writeStartDocument();
    s.writeStartElement("QxDock");

    s.writeAttribute("Version", QString::number(CurrentVersion));
    s.writeAttribute("UserVersion", QString::number(version));
    s.writeAttribute("Containers", QString::number(d->m_containers.count()));
    if (d->m_centralWidget) {
        s.writeAttribute("CentralWidget", d->m_centralWidget->objectName());
    }
    for (auto container : d->m_containers) {
        container->saveState(s);
    }

    s.writeEndElement();
    s.writeEndDocument();

    return compress ? qCompress(xmldata, 9) : xmldata;
}

/**
 * Saves the current state of the dock window to \a fileName as a human readable
 * XML document. Unlike saveState(), the XML is never compressed - a file on disk
 * is expected to be inspectable and diffable. The version number is stored as
 * part of the document.
 * Returns true on success, and false if the file could not be written.
 * \see restoreStateFromFile()
 */
bool DockWindow::saveStateToFile(const QString &fileName, int version) const
{
    if (fileName.isEmpty()) {
        return false;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qWarning() << "Failed to open dock state file for writing:" << fileName << file.errorString();
        return false;
    }

    const QByteArray data = saveStateData(false, version);
    const qint64 written = file.write(data);
    file.close();

    return written == data.size();
}

/**
 * Restores the state of this dock window from the XML document stored in
 * \a fileName. The file is expected to have been written by saveStateToFile().
 * The version number is compared with the one stored in the document; if they
 * do not match the state is left unchanged and this function returns false.
 * Returns true if the file could be read and the state was restored.
 * \see saveStateToFile()
 */
bool DockWindow::restoreStateFromFile(const QString &fileName, int version)
{
    if (fileName.isEmpty()) {
        return false;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open dock state file for reading:" << fileName << file.errorString();
        return false;
    }

    const QByteArray data = file.readAll();
    file.close();

    return restoreState(data, version);
}

/**
 * Restores the state of this dockwindow dockwidgets.
 * The version number is compared with that stored in state. If they do
 * not match, the dockmanager's state is left unchanged, and this function
 * returns false; otherwise, the state is restored, and this function
 * returns true.
 * \see saveState()
 */
bool DockWindow::restoreState(const QByteArray &state, int version)
{
    Q_D(DockWindow);
    // Prevent multiple calls as long as state is not restore. This may
    // happen, if QApplication::processEvents() is called somewhere
    if (d->m_restoringState) {
        return false;
    }

    // We hide the complete dock window here. Restoring the state means
    // that DockWidgets are removed from the DockArea internal stack layout
    // which in turn means, that each time a widget is removed the stack
    // will show and raise the next available widget which in turn
    // triggers show events for the dock widgets. To avoid this we hide the
    // dock window. Because there will be no processing of application
    // events until this function is finished, the user will not see this
    // hiding
    bool isHidden = this->isHidden();
    if (!isHidden) {
        hide();
    }
    d->m_restoringState = true;
    Q_EMIT restoringState();
    bool result = d->restoreState(state, version);
    d->m_restoringState = false;
    if (!isHidden) {
        show();
    }
    Q_EMIT stateRestored();
    return result;
}

/**
 * This function returns true between the restoringState() and
 * stateRestored() signals.
 */
bool DockWindow::isRestoringState() const
{
    Q_D(const DockWindow);
    return d->m_restoringState;
}

void DockWindow::openPerspective(const QString &perspectiveName)
{
    Q_D(DockWindow);
    const auto iterator = d->m_perspectives.find(perspectiveName);
    if (d->m_perspectives.end() == iterator) {
        return;
    }

    Q_EMIT openingPerspective(perspectiveName);
    restoreState(iterator.value());
    Q_EMIT perspectiveOpened(perspectiveName);
}

/**
 * hide DockWindow and all floating widgets. Calling regular QWidget::hide()
 * hides the DockWindow but not the floating widgets;
 */
void DockWindow::hideWindowAndFloatingWidgets()
{
    Q_D(DockWindow);
    hide();

    d->m_hiddenFloatingContainers.clear();
    // Hide updates of floating widgets from user
    for (auto floatingWidget : d->m_floatingContainers) {
        if (floatingWidget->isVisible()) {
            QList<DockWidget *> visibleWidgets;
            for (auto dockWidget : floatingWidget->dockWidgets()) {
                if (dockWidget->toggleViewAction()->isChecked())
                    visibleWidgets.push_back(dockWidget);
            }

            // save as floating widget to be shown when DockWindow will be shown back
            d->m_hiddenFloatingContainers.push_back(floatingWidget);
            floatingWidget->hide();

            // hiding floating widget automatically marked contained DockWidgets as hidden
            // but they must remain marked as visible as we want them to be restored visible
            // when DockWindow will be shown back
            for (auto dockWidget : visibleWidgets) {
                dockWidget->toggleViewAction()->setChecked(true);
            }
        }
    }
}

void DockWindow::restoreHiddenFloatingWidgets()
{
    Q_D(DockWindow);
    if (d->m_hiddenFloatingContainers.isEmpty()) {
        return;
    }

    // Restore floating widgets that were hidden upon hideWindowAndFloatingWidgets
    for (auto floatingWidget : d->m_hiddenFloatingContainers) {
        bool hasDockWidgetVisible = false;

        // Needed to prevent DockFloatingContainer being shown empty
        // Could make sense to move this to DockFloatingContainer::showEvent(QShowEvent *event)
        // if experiencing DockFloatingContainer being shown empty in other situations, but let's keep
        // it here for now to make sure changes to fix Issue #380 does not impact existing behaviours
        for (auto dockWidget : floatingWidget->dockWidgets()) {
            if (dockWidget->toggleViewAction()->isChecked()) {
                dockWidget->toggleView(true);
                hasDockWidgetVisible = true;
            }
        }

        if (hasDockWidgetVisible) {
            floatingWidget->show();
        }
    }

    d->m_hiddenFloatingContainers.clear();
}

QX_DOCK_END_NAMESPACE
