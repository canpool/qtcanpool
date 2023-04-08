/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 **/
#include "ribbon_def.h"

const char *QRibbon::RibbonCustomizeQuickAccessToolBarDotString =
    QT_TRANSLATE_NOOP("QRibbon::RibbonBar", "Customize Quick Access Toolbar...");
const char *QRibbon::RibbonCustomizeQuickAccessToolBarString =
    QT_TRANSLATE_NOOP("QRibbon::RibbonBar", "Customize Quick Access Toolbar");

const char *QRibbon::RibbonShowQuickAccessToolBarBelowString =
    QT_TRANSLATE_NOOP("QRibbon::RibbonBar", "Show Quick Access Toolbar Below the Ribbon");
const char *QRibbon::RibbonShowQuickAccessToolBarAboveString =
    QT_TRANSLATE_NOOP("QRibbon::RibbonBar", "Show Quick Access Toolbar Above the Ribbon");
const char *QRibbon::RibbonCustomizeDialogOptionsString = QT_TRANSLATE_NOOP("QRibbon::RibbonBar", "Options");
const char *QRibbon::RibbonCustomizeActionString = QT_TRANSLATE_NOOP("QRibbon::RibbonBar", "Customize the Ribbon...");
const char *QRibbon::RibbonMinimizeActionString = QT_TRANSLATE_NOOP("QRibbon::RibbonBar", "Minimize the Ribbon");

const char *QRibbon::RibbonRecentDocumentsString = QT_TRANSLATE_NOOP("QRibbon::RibbonBar", "Recent Documents");
const char *QRibbon::RibbonUntitledString = QT_TRANSLATE_NOOP("QRibbon::RibbonBar", "Untitled");
const char *QRibbon::RibbonSeparatorString = QT_TRANSLATE_NOOP("QRibbon::RibbonBar", "<Separator>");
const char *QRibbon::RibbonNewPageString = QT_TRANSLATE_NOOP("QRibbon::RibbonBar", "New Page");
const char *QRibbon::RibbonNewGroupString = QT_TRANSLATE_NOOP("QRibbon::RibbonBar", "New Group");
const char *QRibbon::RibbonAddCommandWarningString = QT_TRANSLATE_NOOP(
    "QRibbon::RibbonBar",
    "One should add commands to custom groups. To create a group, choose page in list and press button 'Create group'.");

const char *QRIBBON_PREPEND_NAMESPACE(getRibbonVersion)()
{
    return QRIBBON_VERSION_STR;
}
