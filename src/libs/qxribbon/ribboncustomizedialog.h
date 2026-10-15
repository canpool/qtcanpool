/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#pragma once

#include "qxribbon_global.h"
#include <QDialog>

class QXmlStreamWriter;
class QXmlStreamReader;

QX_RIBBON_BEGIN_NAMESPACE

class RibbonActionsManager;
class RibbonCustomizeDialogUi;
class RibbonBar;

/**
 * @brief Dialog wrapper for RibbonCustomizeWidget
 *
 * This feature depends on @ref RibbonActionsManager; RibbonActionsManager is recommended to be a member
 * variable of the main window and can quickly bind all QAction instances; see the RibbonActionsManager
 * documentation for details
 *
 * @note Ribbon customization is step-based; if a function such as @ref QxRibbonCustomizeApplyFromXmlFile
 * was called before the window is created, then @ref RibbonCustomizeDialog::fromXml must be called before
 * the dialog runs to keep them in sync, so that the configuration file stays consistent after further edits
 */
class QX_RIBBON_EXPORT RibbonCustomizeDialog : public QDialog
{
    Q_OBJECT
public:
    RibbonCustomizeDialog(RibbonBar *ribbonBar, QWidget *p = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());

    // Sets up the action manager
    void setupActionsManager(RibbonActionsManager *mgr);

    // Applies all settings
    bool apply();

    // Clears all actions
    void clear();

    // Converts to xml
    bool toXml(QXmlStreamWriter *xml) const;
    bool toXml(const QString &xmlpath) const;

    // Loads QList<RibbonCustomizeData> from xml; for file-based settings, call this before showing the dialog to ensure
    // correct recording of stacked settings
    void fromXml(QXmlStreamReader *xml);
    void fromXml(const QString &xmlpath);
protected:
    void showEvent(QShowEvent *e) override;
private:
    RibbonCustomizeDialogUi *ui;
};

/**
 * @brief Directly loads an xml custom ribbon configuration file for custom ribbon display
 * @param filePath xml configuration file
 * @param bar
 * @param mgr Action manager
 * @return Returns true on success
 * @note Loading the same configuration file twice causes an exception; to avoid this, typically use a variable to
 * ensure it is loaded only once, e.g.:
 * @code
 * static bool has_call = false;
 * if (!has_call) {
 *     has_call = QxRibbonCustomizeApplyFromXmlFile("customize.xml", this->ribbonBar(), m_actMgr);
 * }
 * @endcode
 */
bool QX_RIBBON_EXPORT QxRibbonCustomizeApplyFromXmlFile(const QString &filePath, RibbonBar *bar,
                                                        RibbonActionsManager *mgr);

QX_RIBBON_END_NAMESPACE
