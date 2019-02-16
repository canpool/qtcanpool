/****************************************************************************
**
** Qtitan Library by Developer Machines (Microsoft-Ribbon implementation for Qt.C++)
** 
** Copyright (c) 2009-2013 Developer Machines (http://www.devmachines.com)
**           ALL RIGHTS RESERVED
** 
**  The entire contents of this file is protected by copyright law and
**  international treaties. Unauthorized reproduction, reverse-engineering
**  and distribution of all or any portion of the code contained in this
**  file is strictly prohibited and may result in severe civil and 
**  criminal penalties and will be prosecuted to the maximum extent 
**  possible under the law.
**
**  RESTRICTIONS
**
**  THE SOURCE CODE CONTAINED WITHIN THIS FILE AND ALL RELATED
**  FILES OR ANY PORTION OF ITS CONTENTS SHALL AT NO TIME BE
**  COPIED, TRANSFERRED, SOLD, DISTRIBUTED, OR OTHERWISE MADE
**  AVAILABLE TO OTHER INDIVIDUALS WITHOUT WRITTEN CONSENT
**  AND PERMISSION FROM DEVELOPER MACHINES
**
**  CONSULT THE END USER LICENSE AGREEMENT FOR INFORMATION ON
**  ADDITIONAL RESTRICTIONS.
**
****************************************************************************/
#include <QApplication>
#include <QStyleOption>
#include <QPainter>
#include <QScrollBar>
#include <QMdiArea>
#include <QMenuBar>
#include <QComboBox>
#include <QRadioButton>
#include <QCheckBox>
#include <QPushButton>
#include <QDockWidget>
#include <QGroupBox>
#include <QTreeView>
#include <QLineEdit>
#include <QSpinBox>
#include <QStatusBar>
#include <QToolBar>
#include <QMenu>
#include <QProgressBar>
#include <QToolButton>
#include <QMainWindow>
#include <QBitmap>
#include <QTabBar>
#include <QTableView>
#include <QDialog>
#include <QListWidget>
#include <QStackedWidget>
#include <QHeaderView>
#include <QMdiSubWindow>
#include <QTableWidget>
#include <QTreeWidget>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <qdrawutil.h>
#endif /* Q_OS_WIN*/

#ifdef Q_OS_WIN
#include <qt_windows.h>
#endif /* Q_OS_WIN*/

#include "../src/ribbon/QtnOfficePopupMenu.h"
#include "QtnPopupHelpers.h"
#include "QtnCommonStylePrivate.h"
#include "QtnStyleHelpers.h"
#include "QtnOfficeStyle.h"
#include "QtnRibbonGallery.h"

using namespace Qtitan;

static const int windowsItemFrame        =  2; // menu item frame width
static const int windowsItemHMargin      =  3; // menu item hor text margin
static const int windowsItemVMargin      =  2; // menu item ver text margin
static const int windowsRightBorder      = 15; // right border on windows
static const int windowsArrowHMargin     =  6; // arrow horizontal margin


static inline bool isAncestor(const QObject* child)
{
    static QString className = QString("Qtitan::RibbonBar");
    while (child) 
    {
        if (child->metaObject()->className() == className.toLocal8Bit())
            return true;
        child = child->parent();
    }
    return false;
}

namespace Qtitan
{
    /* OfficeStylePrivate */
    class OfficeStylePrivate : public QObject
    {
    public:
        QTN_DECLARE_PUBLIC(OfficeStyle)
    public:
        explicit OfficeStylePrivate();

    public:
        void initialization();

        void setBrush(QWidget* widget);
        void unsetBrush(QWidget* widget);

        void updateColors();
        void refreshMetrics();

    public:
        bool m_dpiAware;
        bool m_animEnabled;
        bool m_ignoreDialogs;

        OfficeStyle::OfficePopupDecoration m_typePopupStyle;

        QStyle* m_popupProxy;
        uint m_refCountProxy;

        OfficeStyle::Theme m_themeType;
        QHash<const QWidget*, QBrush> m_customBrushWidgets; // widgets whose brush we tampered
    };
}

OfficeStylePrivate::OfficeStylePrivate()
{
    m_dpiAware = true;
    m_animEnabled = true;
    m_ignoreDialogs = false;
    m_popupProxy = Q_NULL;
    m_refCountProxy = 0;

    m_typePopupStyle = OfficeStyle::PopupSystemDecoration;

#ifdef Q_OS_LINUX
    m_themeType = OfficeStyle::Office2007Black;
#endif /* Q_OS_LINUX */

#ifdef Q_OS_MAC
    m_themeType = OfficeStyle::Windows7Scenic;
#endif /* Q_OS_MAC*/

#ifdef Q_OS_WIN
    m_themeType = OfficeStyle::Office2007Blue;
#endif /* Q_OS_WIN*/
}

void OfficeStylePrivate::initialization()
{
    updateColors();
}

void OfficeStylePrivate::setBrush(QWidget* widget)
{
    if (QMdiArea* mdiArea = qobject_cast<QMdiArea*>(widget))
    {
        m_customBrushWidgets.insert(widget, mdiArea->background());
    }
}

void OfficeStylePrivate::unsetBrush(QWidget* widget)
{
    if (m_customBrushWidgets.contains(widget)) 
    {
        if (QMdiArea* mdiArea = qobject_cast<QMdiArea*>(widget))
        {
            QBrush b = m_customBrushWidgets.value(widget);
            mdiArea->setBackground(b);
        }
        m_customBrushWidgets.remove(widget);
    }
}

void OfficeStylePrivate::updateColors()
{
    QTN_P(OfficeStyle);
    p.m_clrMenuBarGrayText = QColor(167, 167, 167);
    p.m_clrToolBarGrayText = QColor(141, 141, 141);
    p.m_clrHighlightDisabledBorder = QColor(141, 141, 141);
    p.m_clrMenubarFace = QColor(246, 246, 246);

    StyleHelper& helper = p.helper();
    helper.openIniFile(p.getStyle("OfficeTheme.ini"));

    // [Window]
    p.m_ignoreGlass                 = helper.getValue("Window", "IgnoreGlass").toInt();
    p.m_clrFrame                    = helper.getColor("DockingPane", "WordSplitter");

    p.m_clr3DFace                   = helper.getColor("Window", "ButtonFace");  
    p.m_clrFrameBorderActive0       = helper.getColor("Window", "BorderActive0");
    p.m_clrFrameBorderActive1       = helper.getColor("Window", "BorderActive1");
    p.m_clrFrameBorderActive2       = helper.getColor("Window", "BorderActive2");
    p.m_clrFrameBorderActive3       = helper.getColor("Window", "BorderActive3");

    p.m_clrFrameBorderInactive0     = helper.getColor("Window", "BorderInactive0");
    p.m_clrFrameBorderInactive1     = helper.getColor("Window", "BorderInactive1");
    p.m_clrFrameBorderInactive2     = helper.getColor("Window", "BorderInactive2");
    p.m_clrFrameBorderInactive3     = helper.getColor("Window", "BorderInactive3");

    p.m_clrHighlightBorder          = helper.getColor("Window", "HighlightSelectedBorder");
    p.m_clrHighlightPushedBorder    = helper.getColor("Window", "HighlightPressedBorder");
    p.m_clrHighlightCheckedBorder   = helper.getColor("Window", "HighlightCheckedBorder");
    p.m_clrHighlightPushed          = helper.getColor("Window", "HighlightPressed");
    p.m_clrHighlight                = helper.getColor("Window", "HighlightSelected");
    p.m_clrHighlightChecked         = helper.getColor("Window", "HighlightChecked");

    p.m_palLunaSelected.setColor(QPalette::Dark, helper.getColor("Window", "HighlightSelectedDark"));
    p.m_palLunaSelected.setColor(QPalette::Light, helper.getColor("Window", "HighlightSelectedLight"));

    p.m_palLunaPushed.setColor(QPalette::Dark, helper.getColor("Window", "HighlightPressedDark"));
    p.m_palLunaPushed.setColor(QPalette::Light, helper.getColor("Window", "HighlightPressedLight"));

    p.m_palLunaChecked.setColor(QPalette::Dark, helper.getColor("Window", "HighlightCheckedDark"));
    p.m_palLunaChecked.setColor(QPalette::Light, helper.getColor("Window", "HighlightCheckedLight"));

    p.m_clrFrameCaptionTextInActive = helper.getColor("Window", "CaptionTextInActive");
    p.m_clrFrameCaptionTextActive   = helper.getColor("Window", "CaptionTextActive");

    p.m_clrTooltipLight             = helper.getColor("Window", "TooltipLight");
    p.m_clrTooltipDark              = helper.getColor("Window", "TooltipDark");
    p.m_clrTooltipBorder            = helper.getColor("Window", "TooltipBorder");

    // [Workspace]
    p.m_clrAppWorkspace             = helper.getColor("Workspace", "AppWorkspace");
    p.m_clrWorkspaceClientTop       = helper.getColor("Workspace", "WorkspaceClientTop");
    p.m_clrWorkspaceClientMiddle    = helper.getColor("Workspace", "WorkspaceClientMiddle");
    p.m_clrWorkspaceClientBottom    =  helper.getColor("Workspace", "WorkspaceClientBottom");

    // [Ribbon]
    p.m_clrControlEditNormal        = helper.getColor("Ribbon", "ControlEditNormal");
    p.m_clrMenuPopupText            = helper.getColor("Ribbon", "MenuPopupText");
    p.m_clrTabNormalText            = helper.getColor("Ribbon", "TabNormalText");
    p.m_clrHighlightText            = helper.getColor("Ribbon", "RibbonText");
    p.m_clrSelectedText             = helper.getColor("Ribbon", "TabSelectedText");
    p.m_clrToolBarText              = helper.getColor("Ribbon", /*"RibbonText"*/"MenuPopupText");
    p.m_clrMenuPopupGripperShadow   = helper.getColor("Ribbon", "MenuPopupGripperShadow", QColor(197, 197, 197));
    p.m_clrMenuGripper              = helper.getColor("Ribbon", "MenuPopupGripper");

    // [Toolbar]
    p.m_clrEditCtrlBorder           = helper.getColor("Toolbar", "ControlEditBorder");
    p.m_clrMenuBarText              = helper.getColor("Toolbar", "MenuBarText");
    p.m_ToolbarGripper              = helper.getColor("Toolbar", "ToolbarGripper");
    p.m_clrControlGallerySelected   = helper.getColor("Toolbar", "ControlGallerySelected");
    p.m_clrControlGalleryNormal     = helper.getColor("Toolbar", "ControlGalleryNormal");
    p.m_clrControlGalleryBorder     = helper.getColor("Toolbar", "ControlGalleryBorder");
    p.m_clrControlGalleryLabel      = helper.getColor("Toolbar", "ControlLabel");
    p.m_clrDockBar                  = helper.getColor("Toolbar", "DockBarFace");
    p.m_clrMenubarBorder            = helper.getColor("Toolbar", "MenuPopupBorder");  
    p.m_clrToolbarFace              = helper.getColor("Toolbar", "ToolbarFace");
    p.m_clrToolbarSeparator         = helper.getColor("Toolbar", "ToolbarSeparator");

    // [PopupControl]
    p.m_clrBackgroundLight = helper.getColor("PopupControl", "BackgroundLight");
    p.m_clrBackgroundDark  = helper.getColor("PopupControl", "BackgroundDark");
    p.m_clrCaptionLight = helper.getColor("PopupControl", "CaptionLight");
    p.m_clrCaptionDark  = helper.getColor("PopupControl", "CaptionDark");


    // [ShortcutBar]
    p.m_clrShortcutItemShadow = helper.getColor("ShortcutBar", "FrameBorder");

    p.m_clrCommandBar.setColor(QPalette::Dark, helper.getColor("Toolbar", "ToolbarFaceDark"));
    p.m_clrCommandBar.setColor(QPalette::Light, helper.getColor("Toolbar", "ToolbarFaceLight"));
    p.m_clrCommandBar.setColor(QPalette::Shadow, helper.getColor("Toolbar", "ToolbarFaceShadow"));

    p.m_clrPopupControl.setColor(QPalette::Dark, helper.getColor("Toolbar", "ControlHighlightPopupedDark"));
    p.m_clrPopupControl.setColor(QPalette::Light, helper.getColor("Toolbar", "ControlHighlightPopupedLight"));

    // [DockingPane]
    p.m_palNormalCaption.setColor(QPalette::Dark, helper.getColor("DockingPane", "NormalCaptionDark"));
    p.m_palNormalCaption.setColor(QPalette::Light, helper.getColor("DockingPane", "NormalCaptionLight"));
    p.m_clrNormalCaptionText = helper.getColor("DockingPane", "NormalCaptionText");

    p.m_palActiveCaption.setColor(QPalette::Dark, helper.getColor("DockingPane", "ActiveCaptionDark"));
    p.m_palActiveCaption.setColor(QPalette::Light, helper.getColor("DockingPane", "ActiveCaptionLight"));
    p.m_clrActiveCaptionText = helper.getColor("DockingPane", "ActiveCaptionText");

    p.m_palSplitter.setColor(QPalette::Dark, helper.getColor("DockingPane", "SplitterDark"));
    p.m_palSplitter.setColor(QPalette::Light, helper.getColor("DockingPane", "SplitterLight"));

    // [StatusBar]
    p.m_clrStatusBarText = helper.getColor("StatusBar", "StatusBarText");
    p.m_clrStatusBarShadow = helper.getColor("StatusBar", "StatusBarShadow");
    p.m_palStatusBarTop.setColor(QPalette::Dark, helper.getColor("StatusBar", "StatusBarFaceTopDark"));
    p.m_palStatusBarTop.setColor(QPalette::Light, helper.getColor("StatusBar", "StatusBarFaceTopLight"));
    p.m_palStatusBarBottom.setColor(QPalette::Dark, helper.getColor("StatusBar", "StatusBarFaceBottomDark"));
    p.m_palStatusBarBottom.setColor(QPalette::Light, helper.getColor("StatusBar", "StatusBarFaceBottomLight"));

    // [Controls]
    p.m_crBorderShadow = helper.getColor("Controls", "GroupBoxFrame");
    p.m_tickSlider = helper.getColor("Controls", "TickSlider");  

    // [ReportControl]
    p.m_palGradientColumn.setColor(QPalette::Dark, helper.getColor("ReportControl", "NormalColumnDark"));
    p.m_palGradientColumn.setColor(QPalette::Light, helper.getColor("ReportControl", "NormalColumnLight"));
    p.m_palGradientColumn.setColor(QPalette::Shadow, helper.getColor("ReportControl", "ColumnShadow"));

    p.m_palGradientColumnPushed.setColor(QPalette::Dark, helper.getColor("ReportControl", "PressedColumnDark"));
    p.m_palGradientColumnPushed.setColor(QPalette::Light, helper.getColor("ReportControl", "PressedColumnLight"));

    p.m_clrColumnSeparator = helper.getColor("ReportControl", "ColumnSeparator");
    p.m_clrGridLineColor = helper.getColor("PropertyGrid", "GridLine");
    p.m_clrSelectionBackground = helper.getColor("ReportControl", "SelectionBackground");
}

void OfficeStylePrivate::refreshMetrics()
{
    QTN_P(OfficeStyle);
    QWidgetList all = p.getAllWidgets();

    // clean up the old style
    for (QWidgetList::ConstIterator it = all.constBegin(); it != all.constEnd(); ++it) 
    {
        register QWidget *w = *it;
        if (!(w->windowType() == Qt::Desktop) && w->testAttribute(Qt::WA_WState_Polished)) 
            p.unpolish(w);
    }
    p.unpolish(qApp);

    // initialize the application with the new style
    p.polish(qApp);

    // re-polish existing widgets if necessary
    for (QWidgetList::ConstIterator it = all.constBegin(); it != all.constEnd(); ++it) 
    {
        register QWidget *w = *it;
        if (w->windowType() != Qt::Desktop && w->testAttribute(Qt::WA_WState_Polished)) 
            p.polish(w);
    }

    for (QWidgetList::ConstIterator it = all.constBegin(); it != all.constEnd(); ++it) 
    {
        register QWidget *w = *it;
        if (w->windowType() != Qt::Desktop && !w->testAttribute(Qt::WA_SetStyle))
        {
            QEvent e(QEvent::StyleChange);
            QApplication::sendEvent(w, &e);
            w->update();
        }
    }
}

/*!
Constuctor of the OfficeStyle.
*/
OfficeStyle::OfficeStyle()
    : CommonStyle()
{
    QTN_INIT_PRIVATE(OfficeStyle);
    QTN_D(OfficeStyle);
    d.initialization();
}

/*!
Constructor of the OfficeStyle. The application is usually required to use this constructor only once. 
After this, the call QApplcation::setStyle(...) is not required. Parameter \a mainWindow is not used.
*/
OfficeStyle::OfficeStyle(QMainWindow* mainWindow)
{
    Q_UNUSED(mainWindow);
    QTN_INIT_PRIVATE(OfficeStyle);
    QTN_D(OfficeStyle);
    d.initialization();
    qApp->setStyle(this);
}

/*!
Destructor of the OfficeStyle.
*/
OfficeStyle::~OfficeStyle()
{
    unsetPopupProxy();
    QTN_FINI_PRIVATE();
}

/*! \internal */
void OfficeStyle::setOptionStyle(OptionsStyle theme)
{
    QTN_D(OfficeStyle);
    if (d.m_themeType != (Theme)theme)
    {
        d.m_themeType = (Theme)theme;
        clearCache();
        d.refreshMetrics();
    }
}

/*! \internal */
OfficeStyle::OptionsStyle OfficeStyle::getOptionStyle() const 
{
    QTN_D(const OfficeStyle);
    return (OptionsStyle)d.m_themeType; 
}

/*!
Sets the \a theme of the office style. You can choose from Office 2007 or Office 2010 theme's families. 
\sa Theme
*/
void OfficeStyle::setTheme(Theme theme)
{
    QTN_D(OfficeStyle);
    if (d.m_themeType != theme) 
    {
        d.m_themeType = theme;
        clearCache();
        d.refreshMetrics();
    }
}

/*!
Returns the theme of the style.
\sa Theme
*/
OfficeStyle::Theme OfficeStyle::getTheme() const
{
    QTN_D(const OfficeStyle);
    return d.m_themeType; 
}

/*!
\brief Sets the animation for Microsoft Office's controls when mouse over the elements to \a enable state. By default the animation is set.
 */
void OfficeStyle::setAnimationEnabled(bool enable/*= true*/)
{
    QTN_D(OfficeStyle);
    d.m_animEnabled = enable;
}

/*!
\brief Returns animation state for Microsoft Office's controls when mouse over the elements.
 */
bool OfficeStyle::isAnimationEnabled() const
{
    QTN_D(const OfficeStyle);
    return d.m_animEnabled;
}

/*!
\brief Sets \a ignore dialog flag. If this flag is true that can ignore the current Ribbon or Office Style for dialogs and standard Qt style will be used for dialog. By default this flag is false.
*/ 
void OfficeStyle::setDialogsIgnored(bool ignore)
{
    QTN_D(OfficeStyle);
    d.m_ignoreDialogs = ignore;
}

/*!
    \brief Returns the ignore dialog flag.
*/ 
bool OfficeStyle::isDialogsIgnored() const
{
    QTN_D(const OfficeStyle);
    return d.m_ignoreDialogs;
}

/*!
Activates in the style the mechanism that makes it possible to bind variety font metrics to the system DPI settings.
If parameter \a aware is true then the application is sensitive to DPI settings.
*/
void OfficeStyle::setDPIAware(bool aware)
{
    QTN_D(OfficeStyle);
    if (d.m_dpiAware == aware)
        return;

    d.m_dpiAware = aware;
    d.refreshMetrics();
}

/*!
Returns the feature of style reaction to the system DPI settings.
*/
bool OfficeStyle::isDPIAware() const
{
#ifdef Q_OS_WIN
    QTN_D(const OfficeStyle);
    return d.m_dpiAware;
#endif /* Q_OS_WIN*/
    return true;
}

/*!
Returns the value of the DPI setting in native format. If isDPIAware() is false then this function always returns 96. 
*/
int OfficeStyle::getDPI() const
{
#ifdef Q_OS_WIN
    return isDPIAware() ? DrawHelpers::getDPI() : 96;
#endif /* Q_OS_WIN*/
    return -1;
}

/*!
Returns the value of the DPI setting in percentage. If isDPIAware() is false then this function always returns 100%.
*/
int OfficeStyle::getDPIToPercent() const
{
#ifdef Q_OS_WIN
    return isDPIAware() ? DrawHelpers::getDPIToPercent() : 100;
#endif /* Q_OS_WIN*/
    return -1;
}

/*!
Returns the value of the popupDecoration flag.
*/
OfficeStyle::OfficePopupDecoration OfficeStyle::popupDecoration() const
{
    QTN_D(const OfficeStyle);
    return d.m_typePopupStyle;
}

/*!
Sets the style of the \a decoration for OfficePopupWindow. 
*/
void OfficeStyle::setPopupDecoration(OfficeStyle::OfficePopupDecoration decoration)
{
    QTN_D(OfficeStyle);
    d.m_typePopupStyle = decoration;
}

/*! \reimp */
void OfficeStyle::polish(QApplication* pApp)
{
    CommonStyle::polish(pApp);
    QTN_D(OfficeStyle);
    d.updateColors();
}

// native Dialog
static bool isParentDialog(const QWidget* widget)
{
    return widget && qobject_cast<QDialog*>(widget->topLevelWidget());
}

/*! \reimp */
void OfficeStyle::polish(QWidget* widget)
{
    CommonStyle::polish(widget);

    QTN_D(OfficeStyle);
    d.unsetBrush(widget);
    d.setBrush(widget);


    if (qobject_cast<QCheckBox*>(widget)   ||
        qobject_cast<QRadioButton*>(widget)||
        qobject_cast<QToolButton*>(widget) ||
        qobject_cast<QComboBox*>(widget)   ||
        qobject_cast<QScrollBar*>(widget)  ||
        qobject_cast<QGroupBox*>(widget)   ||
        qobject_cast<QTabBar*>(widget)     ||
        qobject_cast<QAbstractSpinBox*>(widget) ||
        qobject_cast<QSlider*>(widget)     ||
        qobject_cast<QTabWidget*>(widget)  ||
        qobject_cast<QAbstractButton*>(widget))
    {
        widget->setAttribute(Qt::WA_Hover, true);
    }

    if (widget->metaObject()->className() == QString("QComboBoxListView").toLocal8Bit())
    {
        QPalette palette = widget->palette();
        palette.setColor(QPalette::All, QPalette::HighlightedText, QColor(Qt::white));
        widget->setPalette(palette);
    }
    else if (widget->inherits("Qtitan::QuickCustomizationPopup"))
    {
        QPalette palette = widget->palette();
        palette.setColor(QPalette::All, QPalette::Background, helper().getColor("Ribbon", "RibbonFace"));
        widget->setPalette(palette);
    }
    else if (widget->inherits("QTableView") || widget->inherits("Qtitan::Grid"))
    {
        QPalette palette = widget->palette();
        palette.setColor(QPalette::All, QPalette::Background, helper().getColor("Ribbon", "RibbonFace"));
        widget->setPalette(palette);
    }
    else if (qobject_cast<QTableView*>(widget) || widget->inherits("Qtitan::Grid"))
    {
        QPalette palette = widget->palette();
        QColor clrNormalText = helper().getColor(tr("ListBox"), tr("NormalText"));
        palette.setColor(QPalette::Active, QPalette::Text, m_clrHighlight);
        palette.setColor(QPalette::All, QPalette::HighlightedText, clrNormalText);
        palette.setColor(QPalette::All, QPalette::Text, clrNormalText);

        palette.setColor(QPalette::Normal, QPalette::Highlight, m_clrSelectionBackground);
        palette.setColor(QPalette::Disabled, QPalette::Highlight, m_clrSelectionBackground);
        palette.setColor(QPalette::Inactive, QPalette::Highlight, m_clrSelectionBackground);

        widget->setPalette(palette);

        widget->setAttribute(Qt::WA_Hover, true);
    }
    else if (QTreeView* tree = qobject_cast<QTreeView*>(widget)) 
    {
        QPalette palette = widget->palette();
        QColor clrNormalText = helper().getColor(tr("ListBox"), tr("NormalText"));
        palette.setColor(QPalette::Active, QPalette::Text, m_clrHighlight);
        palette.setColor(QPalette::All, QPalette::HighlightedText, clrNormalText);
        palette.setColor(QPalette::All, QPalette::Text, clrNormalText);
        widget->setPalette(palette);

        tree->viewport()->setAttribute(Qt::WA_Hover);
    }
    else if (QListView* list = qobject_cast<QListView*>(widget)) 
    {
        QPalette palette = widget->palette();
        QColor clrNormalText = helper().getColor(tr("ListBox"), tr("NormalText"));
        palette.setColor(QPalette::Active, QPalette::Text, m_clrHighlight);
        palette.setColor(QPalette::All, QPalette::HighlightedText, clrNormalText);
        palette.setColor(QPalette::All, QPalette::Text, clrNormalText);
        widget->setPalette(palette);

        list->viewport()->setAttribute(Qt::WA_Hover);
    }
    else if (qobject_cast<QMainWindow*>(widget))
    {
        QPalette palette = widget->palette();
        palette.setColor(QPalette::Background, m_clrDockBar);
        widget->setPalette(palette);
    }
    else if (!isDialogsIgnored() && qobject_cast<QDialog*>(widget))
    {
        QPalette palette = widget->palette();
        palette.setColor(QPalette::Background, helper().getColor(tr("Dialog"), tr("Background")));
        widget->setPalette(palette);
    }
    else if (qobject_cast<QDockWidget*>(widget))
    {
        widget->setAutoFillBackground(true);
        QPalette palette = widget->palette();
        palette.setColor(QPalette::Background, m_clrDockBar);
        widget->setPalette(palette);
    }
    else if(qobject_cast<QStatusBar*>(widget))
    {
        QPalette palette = widget->palette();
        palette.setColor(QPalette::Foreground, m_clrStatusBarText);
        widget->setPalette(palette);
    }
    else if(QMdiArea* mdiArea = qobject_cast<QMdiArea*>(widget))
    {
        QPalette palette = widget->palette();
        QColor color = helper().getColor(tr("TabManager"), tr("FrameBorder"));
        QColor light = color.lighter(230);
        QColor dark = color.darker(160);
        dark.setAlpha(100);
        palette.setColor(QPalette::Dark, dark);
        palette.setColor(QPalette::Light, light);
        widget->setPalette(palette);
        mdiArea->setBackground(QBrush(m_clrAppWorkspace));
    }
    else if (qobject_cast<QProgressBar*>(widget))
    {
        QPalette palette = widget->palette();
        palette.setColor(QPalette::Active, QPalette::Highlight, m_clrAppWorkspace);
        widget->setPalette(palette);
    }
    else if (qobject_cast<QGroupBox*>(widget))
    {
        QPalette palette = widget->palette();
        palette.setBrush(QPalette::Dark, m_crBorderShadow);
        widget->setPalette(palette);
    }
    else if (qobject_cast<QMenuBar*>(widget))
    {
        QPalette palette = widget->palette();
        palette.setColor(QPalette::ButtonText, getTextColor(false, false, true, false, false, TypeMenuBar, BarNone));
        palette.setColor(QPalette::HighlightedText, getTextColor(true, false, true, false, false, TypeMenuBar, BarNone));
        widget->setPalette(palette);
    }
    else if (qobject_cast<QSlider*>(widget))
    {
        QPalette palette = widget->palette();
        palette.setBrush(QPalette::Shadow, m_tickSlider);
        palette.setBrush(QPalette::Foreground, m_tickSlider);
        widget->setPalette(palette);
    }
    else if (qobject_cast<QTabBar*>(widget) && qobject_cast<QMdiArea*>(widget->parentWidget()))
    {
        widget->setAutoFillBackground(true);
        QPalette palette = widget->palette();
        palette.setColor(QPalette::Background, helper().getColor(tr("TabManager"), tr("HeaderFaceLight")));
        widget->setPalette(palette);
        ((QTabBar*)widget)->setExpanding(false);
    }
    else if (qobject_cast<QHeaderView*>(widget))
    {
        QPalette palette = widget->palette();
        QColor clrNormalText = helper().getColor(tr("ListBox"), tr("NormalText"));
        palette.setColor(QPalette::All, QPalette::ButtonText, clrNormalText);
        widget->setPalette(palette);
    }

    if(defaultStyle())
    {   
        if (qobject_cast<QMdiSubWindow*>(widget))
            widget->setStyle(defaultStyle());
        else if(isDialogsIgnored() && isParentDialog(widget) && qApp->style() == this && 
                widget->style()->metaObject()->className() != QString("QStyleSheetStyle").toLocal8Bit())
            widget->setStyle(defaultStyle());
    }
}

/*! \reimp */
void OfficeStyle::polish(QPalette& palette)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QCommonStyle::polish(palette);
#else
    QWindowsStyle::polish(palette);
#endif
}

/*! \reimp */
void OfficeStyle::unpolish(QApplication* app)
{
    CommonStyle::unpolish(app);
}

/*! \reimp */
void OfficeStyle::unpolish(QWidget* widget)
{
    CommonStyle::unpolish(widget);

    QTN_D(OfficeStyle);
    d.unsetBrush(widget);

    if (qobject_cast<QCheckBox*>(widget)   ||
        qobject_cast<QRadioButton*>(widget)||
        qobject_cast<QPushButton*>(widget) ||
        qobject_cast<QToolButton*>(widget) ||
        qobject_cast<QAbstractSpinBox*>(widget) ||
        qobject_cast<QComboBox*>(widget)   ||
        qobject_cast<QScrollBar*>(widget)  ||
        qobject_cast<QGroupBox*>(widget)   ||
        qobject_cast<QSlider*>(widget))
        widget->setAttribute(Qt::WA_Hover, false);
}

/*! \internal */
QString OfficeStyle::getStyle(const QString& str) const
{
    QTN_D(const OfficeStyle);
    QString strStyle;

    switch (d.m_themeType) 
    {
        case Office2007Blue  :
                strStyle += ":/res/Office2007Blue/";
            break;
        case Office2007Black :
                strStyle += ":/res/Office2007Black/";
            break;
        case Office2007Silver :
                strStyle += ":/res/Office2007Silver/";
            break;
        case Office2007Aqua :
                strStyle += ":/res/Office2007Aqua/";
            break;
        case Windows7Scenic :
                strStyle += ":/res/Windows7Scenic/";
            break;
        case Office2010Silver :
                strStyle += ":/res/Office2010Silver/";
            break;
        case Office2010Blue :
                strStyle += ":/res/Office2010Blue/";
            break;
        case Office2010Black :
                strStyle += ":/res/Office2010Black/";
            break;
        case Office2013White :
                strStyle += ":/res/Office2013White/";
            break;
        case GAEA2014Black :
            strStyle += ":/res/GAEA2014Black/";
            break;
        default:
            Q_ASSERT(false);
            break;
    }

    strStyle += str;
    return strStyle;
}

/*! Returns true if style is using Office 2010 themes family. Otherwise it return false. */
bool OfficeStyle::isStyle2010() const
{
    QTN_D(const OfficeStyle);
    return d.m_themeType == Office2010Black || d.m_themeType == Office2010Blue || d.m_themeType == Office2010Silver || d.m_themeType == GAEA2014Black;
}

/*! \internal */
void OfficeStyle::createPopupProxy()
{
    QTN_D(OfficeStyle);

    if (d.m_popupProxy)
    {
        ++d.m_refCountProxy;
        return;
    }

    PopupDrawHelper* popupStyle = Q_NULL;
    switch(d.m_typePopupStyle)
    {
        case PopupCompatibleDecoration :
                popupStyle = new PopupOffice2000DrawHelper();
            break;
        case PopupOfficeFlatDecoration :
        case PopupSystemDecoration :
            {
                OfficeStyle::Theme theme = getTheme();
                PopupOffice2003DrawHelper* popup2003Style = Q_NULL;
                if (d.m_typePopupStyle == PopupSystemDecoration)
                {
                    if (theme == OfficeStyle::Office2007Blue   || 
                        theme == OfficeStyle::Office2007Black  || 
                        theme == OfficeStyle::Office2007Silver || 
                        theme == OfficeStyle::Office2007Aqua)
                        popup2003Style = new PopupOffice2007DrawHelper();
                    else
                        popup2003Style = new PopupOffice2010DrawHelper();
                }
                else
                {
                    popup2003Style = new PopupOffice2003DrawHelper();
                }
                switch (getTheme())
                {
                    case OfficeStyle::Office2007Blue :
                    case OfficeStyle::Office2010Blue :
                            popup2003Style->setDecoration(PopupOffice2003DrawHelper::OS_SYSTEMBLUE);
                        break;
                    case OfficeStyle::Office2007Black :
                    case OfficeStyle::Office2010Black :
                    case OfficeStyle::GAEA2014Black :
                            popup2003Style->setDecoration(PopupOffice2003DrawHelper::OS_SYSTEMROYALE);
                        break;
                    case OfficeStyle::Office2007Silver :
                    case OfficeStyle::Office2010Silver :
                            popup2003Style->setDecoration(PopupOffice2003DrawHelper::OS_SYSTEMSILVER);
                        break;
                    case OfficeStyle::Office2007Aqua :
                    case OfficeStyle::Windows7Scenic :
                            popup2003Style->setDecoration(PopupOffice2003DrawHelper::OS_SYSTEMOLIVE);
                        break;
                    default:
                            Q_ASSERT(false);
                        break;
                }
                popupStyle = popup2003Style;
            }
            break;
        case PopupMSNDecoration :
                popupStyle = new PopupMSNDrawHelper();
            break;
        default:
            break;
    }
    if (popupStyle)
        popupStyle->refreshPalette();

    d.m_popupProxy = popupStyle;
    ++d.m_refCountProxy;
}

/*! \internal */
bool OfficeStyle::isExistPopupProxy() const
{
    QTN_D(const OfficeStyle);
    return d.m_popupProxy;
}

/*! \internal */
void OfficeStyle::unsetPopupProxy()
{
    QTN_D(OfficeStyle);
    if (!--d.m_refCountProxy) 
    {
        delete d.m_popupProxy;
        d.m_popupProxy = Q_NULL;
    }
}

/*! \internal */
bool OfficeStyle::transitionsEnabled() const
{
    QTN_D(const OfficeStyle);
    return d.m_animEnabled;
}

/*! \reimp */
QSize OfficeStyle::sizeFromContents(ContentsType ct, const QStyleOption* opt, const QSize& contentsSize, const QWidget* w) const
{
    switch (ct) 
    {
        case CT_SizeGrip :
            {
                QPixmap soGripper = cached("StatusBarGripper.png");
                QSize sz(soGripper.size());
                sz += QSize(2,2);
                return sz;
            }
        case CT_MenuItem :
            {
                QSize s = CommonStyle::sizeFromContents(ct, opt, contentsSize, w);
                s.setWidth(s.width() + s.height());
                if (const QStyleOptionMenuItem *menuitem = qstyleoption_cast<const QStyleOptionMenuItem *>(opt))
                    if (menuitem->menuItemType != QStyleOptionMenuItem::Separator)
                        s.setHeight(qMax(s.height(), 22));
                return s;
            }
        default:
            break;
    }
    return CommonStyle::sizeFromContents(ct, opt, contentsSize, w);
}

/*! \reimp */
QRect OfficeStyle::subControlRect(ComplexControl cc, const QStyleOptionComplex* opt, SubControl sc, const QWidget* widget) const
{
    QTN_D(const OfficeStyle);
    switch (cc)
    {
        case CC_TitleBar:
            if (widget && (widget->metaObject()->className() == QString("Qtitan::OfficePopupWindow").toLocal8Bit()) && d.m_popupProxy)
                return d.m_popupProxy->subControlRect(cc, opt, sc, widget);
            break;
        case CC_ComboBox:
            if (const QStyleOptionComboBox* cmb = qstyleoption_cast<const QStyleOptionComboBox*>(opt)) 
            {
                int x = cmb->rect.x(), y = cmb->rect.y(), wi = cmb->rect.width(), he = cmb->rect.height();
                int xpos = x;
                xpos += wi - 16;

                switch (sc) 
                {
                    case SC_ComboBoxArrow:
                        {
                            QRect rect = QRect(xpos, y, 16, he);
                            return visualRect(opt->direction, opt->rect, rect);
                        }
                    default:
                        break;
                }
            }
            break;
        default:
            break;
    };

    return CommonStyle::subControlRect(cc, opt, sc, widget);
}

/*! \reimp */
int OfficeStyle::pixelMetric(PixelMetric pm, const QStyleOption* opt, const QWidget* w) const
{
    QTN_D(const OfficeStyle);
    int ret = 0;
    switch(pm)
    {
        case PM_TitleBarHeight:
            if (w && (w->metaObject()->className() == QString("Qtitan::OfficePopupWindow").toLocal8Bit()) && d.m_popupProxy)
                ret = d.m_popupProxy->pixelMetric(pm, opt, w);
            else
                ret = CommonStyle::pixelMetric(pm, opt, w);
            break;
        case PM_DefaultFrameWidth:
            if (w && (w->metaObject()->className() == QString("Qtitan::OfficePopupWindow").toLocal8Bit()) && d.m_popupProxy)
                d.m_popupProxy->pixelMetric(pm, opt, w);
            else
                ret = CommonStyle::pixelMetric(pm, opt, w);
            break;
        case PM_MenuPanelWidth :
                ret = 2;
            break;
        case PM_MenuHMargin:
                ret = 0;
            break;
        case PM_MenuBarItemSpacing :
                ret = 1;
            break;
        case PM_MenuBarVMargin:
                ret = 2;
            break;
        case PM_MenuBarHMargin:
                ret = 0;
            break;
        case PM_DockWidgetSeparatorExtent :
                ret = 4;
            break;
        case PM_DockWidgetTitleMargin :
            {
//                ret = CommonStyle::pixelMetric(pm, opt, w) + 3;
                ret = CommonStyle::pixelMetric(pm, opt, w);
            }
            break;
        default:
                ret = CommonStyle::pixelMetric(pm, opt, w);
            break;
    }
    return ret;
}

/*! \reimp */
int OfficeStyle::styleHint(StyleHint hint, const QStyleOption* opt, const QWidget* widget, QStyleHintReturn* returnData) const
{
    int ret = 0;
    switch (hint)
    {
        case SH_Menu_MouseTracking:
        case SH_ComboBox_ListMouseTracking :
                ret = 1;
            break;
        case SH_ToolTip_Mask :
        case SH_Menu_Mask :
            if (QStyleHintReturnMask* mask = qstyleoption_cast<QStyleHintReturnMask*>(returnData))
            {
                mask->region = opt->rect;
                //left rounded corner
                mask->region -= QRect(opt->rect.left(), opt->rect.top(), 1, 1);
                //right rounded corner
                mask->region -= QRect(opt->rect.right(), opt->rect.top(), 1, 1);
                //right-bottom rounded corner
                mask->region -= QRect(opt->rect.right(), opt->rect.bottom(), 1, 1);
                //left-bottom rounded corner
                mask->region -= QRect(opt->rect.left(), opt->rect.bottom(), 1, 1);
                ret = 1;
            }
            break;
        case SH_Table_GridLineColor:
                ret = opt ? m_clrGridLineColor.rgb() : CommonStyle::styleHint(hint, opt, widget, returnData);;
            break;
        default:
                ret = CommonStyle::styleHint(hint, opt, widget, returnData);
            break;
    }
    return ret;
}

/*! \reimp */
QStyle::SubControl OfficeStyle::hitTestComplexControl(ComplexControl control, const QStyleOptionComplex* opt, const QPoint& pt, const QWidget* widget) const
{
    if (control == CC_TitleBar)
    {
        QTN_D(const OfficeStyle);
        if (widget && (widget->metaObject()->className() == QString("Qtitan::OfficePopupWindow").toLocal8Bit()) && d.m_popupProxy)
            return d.m_popupProxy->hitTestComplexControl(control, opt, pt, widget);
    }
    return CommonStyle::hitTestComplexControl(control, opt, pt, widget);
}

/*! \reimp */
void OfficeStyle::drawComplexControl(ComplexControl cc, const QStyleOptionComplex* opt, QPainter* p, const QWidget* widget) const
{
    if (cc == CC_TitleBar)
    {
        QTN_D(const OfficeStyle);
        if (widget && (widget->metaObject()->className() == QString("Qtitan::OfficePopupWindow").toLocal8Bit()) && d.m_popupProxy)
        {
            d.m_popupProxy->drawComplexControl(cc, opt, p, widget);
            return;
        }
    }
    CommonStyle::drawComplexControl(cc, opt, p, widget);
}

/*! \reimp */
void OfficeStyle::drawPrimitive(PrimitiveElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget) const
{
    if (element == PE_FrameWindow)
    {
        QTN_D(const OfficeStyle);
        if (widget && (widget->metaObject()->className() == QString("Qtitan::OfficePopupWindow").toLocal8Bit()) && d.m_popupProxy)
        {
            d.m_popupProxy->drawPrimitive(element, option, painter, widget);
            return;
        }
    }
    CommonStyle::drawPrimitive(element, option, painter, widget);
}

/*! \internal */
QPixmap OfficeStyle::cached(const QString& img) const
{
    return CommonStyle::cached(getStyle(img));
}

/*! \internal */
bool OfficeStyle::drawWorkspace(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    Q_UNUSED(widget);
    if (opt == Q_NULL)
       return false;

    QPixmap soWorkspace = cached("WorkspaceTopLeft.png");
    if (!soWorkspace.isNull())
    {
        QRect rc = opt->rect;
        QRect rcSrc = soWorkspace.rect();
        QRect rcTopLeft = opt->rect;
        rcTopLeft.setBottom(rcTopLeft.top() + rcSrc.height());
        rcTopLeft.setRight(rcTopLeft.left() + qMax(rcTopLeft.width(), rcSrc.width()));
        drawImage(soWorkspace, *p, rcTopLeft, rcSrc, QRect(QPoint(rcSrc.width() - 1, 0), QPoint(0, 0)));

        QRect rcFill(QPoint(rc.left(), rc.top() + rcSrc.height()), QPoint(rc.right(), rc.bottom()));
        QRect rcFillTop(QPoint(rcFill.left(), rcFill.top()), QPoint(rcFill.right() + 1, rcFill.top() + rcFill.height() * 2 / 3));
        QRect rcFillBottom(QPoint(rcFill.left(), rcFillTop.bottom()), QPoint(rcFill.right() + 1, rcFill.bottom()));
        DrawHelpers::drawGradientFill(*p, rcFillTop, m_clrWorkspaceClientTop, m_clrWorkspaceClientMiddle, true);
        DrawHelpers::drawGradientFill(*p, rcFillBottom, m_clrWorkspaceClientMiddle, m_clrWorkspaceClientBottom, true);
        return true;
    }
    return false;
}

/*! \internal */
bool OfficeStyle::drawWidget(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    if (qobject_cast<const QMainWindow*>(w))
    {
        QRect rc(opt->rect);
        p->fillRect(rc, m_clrFrame);
        return true;
    }
    return false;
}

// for QForm
/*! \internal */
bool OfficeStyle::drawFrameTabWidget(const QStyleOption* opt, QPainter* p, const QWidget*) const
{
    QPixmap soTabPaneEdge = cached("TabPaneEdge.png");
    if (soTabPaneEdge.isNull())
        return false;
    drawImage(soTabPaneEdge, *p, opt->rect, soTabPaneEdge.rect(), QRect(QPoint(5, 18), QPoint(5, 5)));
    return true;
}

/*! \internal */
bool OfficeStyle::drawShapedFrame(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    bool ret = false;
    if (const QMdiArea* mdiArea = qobject_cast<const QMdiArea*>(w))
    {
        if (const QStyleOptionFrameV3* f = qstyleoption_cast<const QStyleOptionFrameV3*>(opt)) 
        {
            int frameShape  = f->frameShape;
            int frameShadow = QFrame::Plain;
            if (f->state & QStyle::State_Sunken)
                frameShadow = QFrame::Sunken;
            else if (f->state & QStyle::State_Raised)
                frameShadow = QFrame::Raised;

            switch (frameShape) 
            {
                case QFrame::Panel:
                {
                    if (frameShadow == QFrame::Sunken)
                    {
                        QRect rect = opt->rect;
                        if (QTabBar* tabBar = mdiArea->findChild<QTabBar*>())
                        {
                            int lw = f->lineWidth;
                            int mlw = f->midLineWidth;

                            p->fillRect(opt->rect, opt->palette.light().color());

                            switch (mdiArea->tabPosition()) 
                            {
                                case QTabWidget::North:
                                        rect.adjust(0,tabBar->sizeHint().height(),0,0);
                                    break;
                                case QTabWidget::South:
                                        rect.adjust(0, 0, 0, -tabBar->sizeHint().height()-1);
                                    break;
                                case QTabWidget::West:
                                    rect.adjust(tabBar->sizeHint().width()-1, 0, 0, 0);
                                    break;
                                case QTabWidget::East:
                                        rect.adjust(0, 0, -tabBar->sizeHint().width()-1, 0);
                                    break;
                                default:
                                    break;
                                }
                            rect.adjust(0, 0, -1, -1);
                            QPen savePen = p->pen();
                            p->setPen(opt->palette.dark().color());
                            p->drawRect(rect);
                            p->setPen(savePen);

                            rect.adjust(1, 1, -1, -1);
                            qDrawShadeRect(p, rect, opt->palette, true, lw, mlw);
                        }
                        ret = true;
                    }
                    break;
                }
                default:
                    break;
             }
        }
    }
    return ret;
}

// for statusBar
/*! \internal */
bool OfficeStyle::drawPanelStatusBar(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);
    QRect rc = opt->rect;
    p->fillRect(QRect(QPoint(rc.left(), rc.top()), QSize(rc.width(), 1)), m_clrStatusBarShadow);
    // [1]
    DrawHelpers::drawGradientFill(*p, QRect(QPoint(rc.left(), rc.top() + 1), QPoint(rc.right(), rc.top() + 9)),
        m_palStatusBarTop.color(QPalette::Light), m_palStatusBarTop.color(QPalette::Dark), true);

    // [2]
    DrawHelpers::drawGradientFill(*p, QRect(QPoint(rc.left(), rc.top() + 9), QPoint(rc.right(), rc.bottom())),
        m_palStatusBarBottom.color(QPalette::Light), m_palStatusBarBottom.color(QPalette::Dark), true);
    return true;
}

// for menuBar
/*! \internal */
bool OfficeStyle::drawMenuBarEmptyArea(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(opt);
    Q_UNUSED(p);
    Q_UNUSED(w);
//    QPixmap soImage = cached("ToolbarFaceHorizontal.png");
//    drawImage(soImage, *p, opt->rect, soImage.rect(), QRect(QPoint(5, 5), QPoint(5, 5)));
    return true;
}

/*! \internal */
bool OfficeStyle::drawPanelMenu(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);

    QPixmap soPopupBarFrame = cached("PopupBarFrame.png");
    if (!soPopupBarFrame.isNull())
        drawImage(soPopupBarFrame, *p, opt->rect, soPopupBarFrame.rect(), QRect(4, 4, 4, 4));

    bool showGripper = true;
    // draw a Gripper or not
    QVariant var = w->property("showGripper");
    if (!var.isNull())
        showGripper = var.toBool();

    if(showGripper)
    {
        const int nMenuPanelWidth = pixelMetric(PM_MenuPanelWidth, opt, w);
        const int nIconSize = pixelMetric(PM_ToolBarIconSize, opt, w)+2;
        QRect rcBorders(QPoint(nMenuPanelWidth, nMenuPanelWidth), QPoint(nMenuPanelWidth, nMenuPanelWidth));

        int x = rcBorders.left()-1;
        int y = rcBorders.top();
        int cx = nIconSize;
        int cy = opt->rect.bottom() - rcBorders.top() - rcBorders.bottom();
        // [1]
        DrawHelpers::drawGradientFill(*p, QRect(QPoint(x + 1, y), QSize(cx - 1, cy)), m_clrMenuGripper, m_clrMenuGripper, true);

        p->fillRect(x + cx - 1, y, 1, cy + 1, m_clrMenuPopupGripperShadow);
        p->fillRect(x + cx, y, 1, cy + 1, QColor(245, 245, 245));
    }
    return true;
}

/*! \internal */
bool OfficeStyle::drawMenuBarItem(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    if (const QStyleOptionMenuItem* mbi = qstyleoption_cast<const QStyleOptionMenuItem*>(opt))
    {
        QRect rect(opt->rect);

        if (mbi->menuItemType == QStyleOptionMenuItem::DefaultItem)
            return true;

        QPixmap pix = mbi->icon.pixmap(proxy()->pixelMetric(PM_SmallIconSize, opt, w), QIcon::Normal);

        uint alignment = Qt::AlignCenter | Qt::TextShowMnemonic | Qt::TextDontClip | Qt::TextSingleLine;
        if (!proxy()->styleHint(SH_UnderlineShortcut, mbi, w))
            alignment |= Qt::TextHideMnemonic;

        bool enabled  = opt->state & State_Enabled;
        bool checked  = opt->state & State_On;
        bool selected = opt->state & State_Selected;
        bool pressed  = opt->state & State_Sunken;
//        bool popuped  = false;//(mbi->activeSubControls & QStyle::SC_ToolButtonMenu) && (opt->state & State_Sunken);
        bool popuped  = (opt->state & QStyle::State_Selected) && (opt->state & State_Sunken);

        drawRectangle(p, opt->rect, selected, pressed, enabled, checked, popuped, TypeMenuBar, BarTop);

        QPalette::ColorRole textRole = !enabled ? QPalette::Text:
            selected ? QPalette::HighlightedText : QPalette::ButtonText;

        if (!pix.isNull())
            drawItemPixmap(p, mbi->rect, alignment, pix);
        else
            drawItemText(p, mbi->rect, alignment, mbi->palette, mbi->state & State_Enabled, mbi->text, textRole);
        return true;
    }
    return false;
}

/*! \internal */
bool OfficeStyle::drawIndicatorMenuCheckMark(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);
    bool dis = !(opt->state & State_Enabled);
    QPixmap soMenuCheckedItem = cached("MenuCheckedItem.png");
    QRect rcSrc = getSource(soMenuCheckedItem.rect(), !dis ? 0 : 1, 2);

    const int iconSize = proxy()->pixelMetric(PM_SmallIconSize, opt, w) + 4;
    QRect rc(QPoint(opt->rect.left() + opt->rect.width()/2 - iconSize/2, 
        opt->rect.top() + opt->rect.height()/2 - iconSize/2), QSize(iconSize, iconSize));
    drawImage(soMenuCheckedItem, *p, rc, rcSrc, QRect(QPoint(2, 2), QPoint(2, 2)));

    QPixmap soMenuCheckedItemMark = cached("MenuCheckedItemMark.png");
    rcSrc = getSource(soMenuCheckedItemMark.rect(), !dis ? 0 : 1, 4);
    QRect rcDest(QPoint((rc.left() + rc.right()-rcSrc.width())/2+1, (rc.top() + rc.bottom() - rcSrc.height())/2+1), rcSrc.size() );
    drawImage(soMenuCheckedItemMark, *p, rcDest, rcSrc, QRect(QPoint(0, 0), QPoint(0, 0)), QColor(0xFF, 0, 0xFF));
    return true;
}

/*! \internal */
bool OfficeStyle::drawMenuItem(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    if (const QStyleOptionMenuItem* menuitem = qstyleoption_cast<const QStyleOptionMenuItem*>(opt)) 
    {
        int x, y, w, h;
        menuitem->rect.getRect(&x, &y, &w, &h);
        int tab = menuitem->tabWidth;
        bool dis = !(menuitem->state & State_Enabled);
        bool checked = menuitem->checkType != QStyleOptionMenuItem::NotCheckable ? menuitem->checked : false;
        bool act = menuitem->state & State_Selected;

        // windows always has a check column, regardless whether we have an icon or not
        const int nIconSize = pixelMetric(PM_ToolBarIconSize, opt, widget);
        int checkcol = nIconSize;

        if (act)
            drawRectangle(p, menuitem->rect, true/*selected*/, false/*pressed*/, !dis/*enabled*/, 
                false/*checked*/, false/*popuped*/, TypePopup, BarPopup);

        if (menuitem->text.count(QString(qtn_SplitActionPopup)) != 0)
            drawSplitButtonPopup(p, menuitem->rect, act, !dis, menuitem->checkType == QStyleOptionMenuItem::Exclusive);


        if (menuitem->menuItemType == QStyleOptionMenuItem::Separator)
        {
            int yoff = (y-1 + h / 2);
            int xIcon = nIconSize;    
            if(qobject_cast<const OfficePopupMenu*>(widget))
                xIcon = 0;
            p->setPen(menuitem->palette.dark().color());
            p->drawLine(x + 2+/*menuitem->maxIconWidth*/xIcon, yoff, x + w - 4, yoff);
            p->setPen(menuitem->palette.light().color());
            p->drawLine(x + 2+/*menuitem->maxIconWidth*/xIcon, yoff + 1, x + w - 4, yoff + 1);
            return true;
        }

        QRect vCheckRect = visualRect(opt->direction, menuitem->rect, QRect(menuitem->rect.x(), menuitem->rect.y(), checkcol, menuitem->rect.height()));

        // On Windows Style, if we have a checkable item and an icon we
        // draw the icon recessed to indicate an item is checked. If we
        // have no icon, we draw a checkmark instead.
        if (!menuitem->icon.isNull()) 
        {
            QIcon::Mode mode = dis ? QIcon::Disabled : QIcon::Normal;

            int iconSize = pixelMetric(PM_LargeIconSize, opt, widget);
            if (iconSize > qMin(opt->rect.height(), opt->rect.width()))
                iconSize = pixelMetric(PM_SmallIconSize, opt, widget);

            if ( act && !dis )
                mode = QIcon::Active;
            QPixmap pixmap;
            if ( checked )
                pixmap = menuitem->icon.pixmap(iconSize, mode, QIcon::On);
            else
                pixmap = menuitem->icon.pixmap(iconSize, mode);

            int pixw = pixmap.width();
            int pixh = pixmap.height();

            QRect pmr(0, 0, pixw, pixh);
            pmr.moveCenter(vCheckRect.center());
            p->setPen(menuitem->palette.text().color());

            if (checked)
            {
                QRect vIconRect = visualRect(opt->direction, opt->rect, pmr);
                QRect rcChecked = menuitem->rect;
                rcChecked.setLeft(vIconRect.left());
                rcChecked.setWidth(vIconRect.width());
                drawRectangle(p, rcChecked.adjusted(-2, 1, 2, -1), false/*selected*/, true/*pressed*/, !dis/*enabled*/, 
                    true/*checked*/, false/*popuped*/, TypePopup, BarPopup);
            }

            p->drawPixmap(pmr.topLeft(), pixmap);
        } 
        else if (checked) 
        {
            QStyleOptionMenuItem newMi = *menuitem;
            newMi.state = State_None;
            if (!dis)
                newMi.state |= State_Enabled;
            if (act)
                newMi.state |= State_On;
            newMi.rect = visualRect(opt->direction, menuitem->rect, QRect(menuitem->rect.x() + windowsItemFrame, menuitem->rect.y() + windowsItemFrame,
                                    checkcol - 2 * windowsItemFrame, menuitem->rect.height() - 2*windowsItemFrame));
            drawPrimitive(PE_IndicatorMenuCheckMark, &newMi, p, widget);
        }
        //    p->setPen(act ? menuitem->palette.highlightedText().color() : menuitem->palette.buttonText().color());

        QColor discol;
        if (dis) 
        {
            discol = menuitem->palette.text().color();
            p->setPen(discol);
        }

        int xm = windowsItemFrame + checkcol + windowsItemHMargin;
        int xpos = menuitem->rect.x() + xm;
        QRect textRect(xpos, y + windowsItemVMargin, w - xm - windowsRightBorder - tab + 1, h - 2 * windowsItemVMargin);
        QRect vTextRect = visualRect(opt->direction, menuitem->rect, textRect);
        QString s = menuitem->text;
        s = s.remove(QString(qtn_SplitActionPopup));
        // draw text    
        if (!s.isEmpty()) 
        { 
            p->save();
            int t = s.indexOf(QLatin1Char('\t'));
            int text_flags = Qt::AlignVCenter | Qt::TextShowMnemonic | Qt::TextDontClip | Qt::TextSingleLine;

            if (!styleHint(SH_UnderlineShortcut, menuitem, widget))
                text_flags |= Qt::TextHideMnemonic;
            text_flags |= Qt::AlignLeft;
            // draw hotkeys
            if (t >= 0) 
            {
                QRect vShortcutRect = visualRect( opt->direction, menuitem->rect, QRect(textRect.topRight(), 
                    QPoint(menuitem->rect.right(), textRect.bottom())));
                /*
                if (dis && !act && styleHint(SH_EtchDisabledText, opt, widget)) 
                {
                    p->setPen(menuitem->palette.light().color());
                    p->drawText(vShortcutRect.adjusted(1,1,1,1), text_flags, s.mid(t + 1));
                    p->setPen(discol);
                }
                */

                p->setPen(opt->state & State_Enabled ? m_clrMenuPopupText : m_clrMenuBarGrayText);
                p->drawText(vShortcutRect, text_flags, s.mid(t + 1));
                p->setPen(discol);
                s = s.left(t);
            }

            QFont font = menuitem->font;
            if (menuitem->menuItemType == QStyleOptionMenuItem::DefaultItem)
                font.setBold(true);
            p->setFont(font);
            /*
            if (dis && !act && styleHint(SH_EtchDisabledText, opt, widget)) 
            {
                p->setPen(menuitem->palette.light().color());
                p->drawText(vTextRect.adjusted(1,1,1,1), text_flags, s.left(t));
                p->setPen(discol);
            }
            */
            p->setPen(opt->state & State_Enabled ? m_clrMenuPopupText : m_clrMenuBarGrayText);
            p->drawText(vTextRect, text_flags, s.left(t));
            p->setPen(discol);
            p->restore();
        }
        // draw sub menu arrow
        if (menuitem->menuItemType == QStyleOptionMenuItem::SubMenu) 
        {
/*
            int dim = (h - 2 * windowsItemFrame) / 2;
            PrimitiveElement arrow;
            arrow = (opt->direction == Qt::RightToLeft) ? PE_IndicatorArrowLeft : PE_IndicatorArrowRight;
            xpos = x + w - windowsArrowHMargin - windowsItemFrame - dim;
            QRect  vSubMenuRect = visualRect(opt->direction, menuitem->rect, QRect(xpos, y + h / 2 - dim / 2, dim, dim));
*/
            xpos = x + w - windowsArrowHMargin - windowsItemFrame * 2;
            QRect  vSubMenuRect = visualRect(opt->direction, menuitem->rect, QRect(xpos, y + (h - windowsArrowHMargin)/2, w, h));
//            QRect vSubMenuRect = QRect(xpos, y + (h - windowsArrowHMargin)/2, w, h);

            PrimitiveElement arrow;
            arrow = (opt->direction == Qt::RightToLeft) ? PE_IndicatorArrowLeft : PE_IndicatorArrowRight;

            QStyleOptionMenuItem newMI = *menuitem;
            newMI.rect = vSubMenuRect;
            newMI.state = dis ? State_None : State_Enabled;
            drawPrimitive(arrow, &newMI, p, widget);
        }
        return true;
    }
    return false;
}

/*! \internal */
bool OfficeStyle::drawFrameMenu(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    if (qobject_cast<const QToolBar*>(w))
        return true;
    return CommonStyle::drawFrameMenu(opt, p, w);
}

// for toolBar
/*! \internal */
bool OfficeStyle::drawToolBar(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);
    if (const QStyleOptionToolBar* toolbar = qstyleoption_cast<const QStyleOptionToolBar*>(opt)) 
    {
        if (toolbar->state & QStyle::State_Horizontal/*toolbar->toolBarArea == Qt::BottomToolBarArea || toolbar->toolBarArea == Qt::TopToolBarArea*/)
        {
            if (w->parentWidget())
                p->fillRect(toolbar->rect, m_clrDockBar);

            QRect rcFill = toolbar->rect;
            rcFill.setTop(rcFill.top()+1);
            // [1]
            DrawHelpers::drawGradientFill(*p, rcFill, m_clrCommandBar.color(QPalette::Light), m_clrCommandBar.color(QPalette::Dark), true);

            p->fillRect(QRect(QPoint(rcFill.left(), rcFill.bottom()), QSize(rcFill.width(), rcFill.height() - rcFill.height() - 2)), m_clrCommandBar.color(QPalette::Dark));

            p->fillRect(QRect(QPoint(toolbar->rect.left() + 5, toolbar->rect.bottom() - 1), QSize(toolbar->rect.width()/* - 7*/, 1)), m_clrCommandBar.color(QPalette::Shadow));

            p->fillRect(QRect(QPoint(toolbar->rect.right(), toolbar->rect.top() - 1), QSize(1, toolbar->rect.height())), m_clrCommandBar.color(QPalette::Shadow));
        }
        else //if (toolbar->toolBarArea == Qt::RightToolBarArea || toolbar->toolBarArea == Qt::LeftToolBarArea)
        {
            if (w->parentWidget())
            {
                if (toolbar->toolBarArea & Qt::RightToolBarArea)
                    p->fillRect(toolbar->rect, m_clrCommandBar.color(QPalette::Dark));
                else
                    p->fillRect(toolbar->rect, m_clrDockBar);
            }
            QRect rcFill = toolbar->rect;
            rcFill.setLeft(rcFill.left() + 1);
            // [1]
            DrawHelpers::drawGradientFill(*p, rcFill, m_clrCommandBar.color(QPalette::Light), m_clrCommandBar.color(QPalette::Dark), false);

            p->fillRect(QRect(QPoint(toolbar->rect.right() - 1, toolbar->rect.top() + 1), QSize(1, toolbar->rect.height()/* - 7*/)), m_clrCommandBar.color(QPalette::Shadow));

            p->fillRect(QRect(QPoint(toolbar->rect.left(), toolbar->rect.bottom()), QSize(toolbar->rect.width(), 1)), m_clrCommandBar.color(QPalette::Shadow));
        }
        return true;
    }
    return false;
}

/*! \internal */
static void drawArrow(const QStyle* style, const QStyleOptionToolButton* toolbutton,
                      const QRect& rect, QPainter* painter, const QWidget* widget = 0)
{
    QStyle::PrimitiveElement pe;

    switch (toolbutton->arrowType) 
    {
        case Qt::LeftArrow:
                pe = QStyle::PE_IndicatorArrowLeft;
            break;
        case Qt::RightArrow:
                pe = QStyle::PE_IndicatorArrowRight;
            break;
        case Qt::UpArrow:
                pe = QStyle::PE_IndicatorArrowUp;
            break;
        case Qt::DownArrow:
                pe = QStyle::PE_IndicatorArrowDown;
            break;
        default:
            return;
    }

    QStyleOptionToolButton arrowOpt;
    arrowOpt.rect = rect;
    arrowOpt.palette = toolbutton->palette;
    arrowOpt.state = toolbutton->state;
    style->drawPrimitive(pe, &arrowOpt, painter, widget);
}

/*! \internal */
bool OfficeStyle::drawToolButtonLabel(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    if (const QStyleOptionToolButton* toolbutton = qstyleoption_cast<const QStyleOptionToolButton*>(opt))
    {
        QRect rect = toolbutton->rect;
        // Arrow type always overrules and is always shown
        bool hasArrow = toolbutton->features & QStyleOptionToolButton::Arrow;
        if (((!hasArrow && toolbutton->icon.isNull()) && !toolbutton->text.isEmpty())|| toolbutton->toolButtonStyle == Qt::ToolButtonTextOnly) 
        {
            int alignment = Qt::AlignCenter | Qt::TextShowMnemonic;
            if (!styleHint(SH_UnderlineShortcut, opt, w))
                alignment |= Qt::TextHideMnemonic;
            proxy()->drawItemText(p, rect, alignment, toolbutton->palette, opt->state & State_Enabled, toolbutton->text, QPalette::ButtonText);
        }
        else 
        {
            QPixmap pm;
            QSize pmSize = toolbutton->iconSize;
            if (pmSize.width() > qMin(rect.width(), rect.height()))
            {
                const int iconExtent = proxy()->pixelMetric(PM_SmallIconSize);
                pmSize = QSize(iconExtent, iconExtent);
            }

            if (!toolbutton->icon.isNull())
            {
                QIcon::State state = toolbutton->state & State_On ? QIcon::On : QIcon::Off;
                QIcon::Mode mode;
                if (!(toolbutton->state & State_Enabled))
                    mode = QIcon::Disabled;
                else if ((opt->state & State_MouseOver) && (opt->state & State_AutoRaise))
                    mode = QIcon::Active;
                else
                    mode = QIcon::Normal;
                pm = toolbutton->icon.pixmap(toolbutton->rect.size().boundedTo(pmSize), mode, state);
                pmSize = pm.size();
            }

            if (toolbutton->toolButtonStyle != Qt::ToolButtonIconOnly)
            {
                p->setFont(toolbutton->font);
                QRect pr = rect, tr = rect;

                int alignment = Qt::TextShowMnemonic;
                if (!proxy()->styleHint(SH_UnderlineShortcut, opt, w))
                    alignment |= Qt::TextHideMnemonic;

                if (toolbutton->toolButtonStyle == Qt::ToolButtonTextUnderIcon)
                {
                    pr.setHeight(pmSize.height() + 6);
                    tr.adjust(0, pr.height() - 1, 0, -3);
                    if ( !hasArrow )
                        drawItemPixmap(p, pr, Qt::AlignCenter, pm);
                    else 
                        drawArrow(this, toolbutton, pr, p, w);

                    alignment |= Qt::AlignCenter;
                }
                else
                {
                    pr.setWidth(pmSize.width() + 8);
                    tr.adjust(pr.width(), 0, 0, 0);
                    if (!hasArrow) 
                        drawItemPixmap(p, QStyle::visualRect(opt->direction, rect, pr), Qt::AlignCenter, pm);
                    else 
                        drawArrow(this, toolbutton, pr, p, w);
                    alignment |= Qt::AlignLeft | Qt::AlignVCenter;
                }

                if (toolbutton->toolButtonStyle == Qt::ToolButtonTextUnderIcon)
                {
                    QString strFirstRow, strSecondRow;
                    splitString(toolbutton->text, strFirstRow, strSecondRow);

                    QRect rcText(QStyle::visualRect(opt->direction, rect, tr));
                    int height = toolbutton->fontMetrics.height();
                    QRect rcFirstRowText(QPoint(rcText.left(), rcText.bottom() - height*2 + 1), QPoint(rcText.right(), rcText.bottom() - height + 1));

                    if (!strFirstRow.isEmpty())
                    {
                        proxy()->drawItemText(p, rcFirstRowText, alignment, toolbutton->palette,
                                    toolbutton->state & State_Enabled, strFirstRow, QPalette::ButtonText);
                    }

                    if (!strSecondRow.isEmpty()) 
                    {
                        int left = rcText.left();
                        if (toolbutton->subControls & SC_ToolButtonMenu || toolbutton->features & QStyleOptionToolButton::HasMenu)
                            left = opt->rect.left()-5;
                        
                        QRect rcSecondRowText(QPoint(left, rcText.bottom() - height), QPoint(rcText.right(), rcText.bottom()+2));
                        proxy()->drawItemText(p, rcSecondRowText, alignment, toolbutton->palette,
                                    toolbutton->state & State_Enabled, strSecondRow, QPalette::ButtonText);
                    }
                }
                else
                {
                    proxy()->drawItemText(p, QStyle::visualRect(opt->direction, rect, tr), alignment, toolbutton->palette,
                                toolbutton->state & State_Enabled, toolbutton->text, QPalette::ButtonText);
                }
            }
            else
            {
                if (hasArrow) 
                    drawArrow(this, toolbutton, rect, p, w);
                else 
                {
                    QRect pr = rect;
                    if ((toolbutton->subControls & SC_ToolButtonMenu) || (toolbutton->features & QStyleOptionToolButton::HasMenu))
                    {
                        int mbi = proxy()->pixelMetric(PM_MenuButtonIndicator, opt, w);
                        pr.setWidth(pr.width()-mbi);
                    }
                    drawItemPixmap(p, pr, Qt::AlignCenter, pm);
                }
            }
        }
        return true;
    }
    return false;
}

/*! \internal */
bool OfficeStyle::drawPanelButtonTool(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);
    if (const QStyleOptionToolButton* toolbutton = qstyleoption_cast<const QStyleOptionToolButton*>(opt)) 
    {
        bool smallSize = opt->rect.height() < 33;

        bool enabled  = opt->state & State_Enabled;
        bool checked  = opt->state & State_On;
        bool selected = opt->state & State_MouseOver;
        bool mouseInSplit = opt->state & State_MouseOver && toolbutton->activeSubControls & SC_ToolButton;
        bool mouseInSplitDropDown = opt->state & State_MouseOver && toolbutton->activeSubControls & SC_ToolButtonMenu;
        bool pressed  = opt->state & State_Sunken;
        bool popuped  = (toolbutton->activeSubControls & QStyle::SC_ToolButtonMenu) && (opt->state & State_Sunken);

        if (!(toolbutton->features & QStyleOptionToolButton::MenuButtonPopup))
        {
            drawRectangle(p, opt->rect, selected, pressed, enabled, checked, popuped, TypeNormal, BarTop);
            return true;
        }

        int specialOffset = 0;
        QPixmap soImageSplit = smallSize ? cached("ToolbarButtonsSplit22.png") 
                : cached("ToolbarButtonsSplit50.png");
        QPixmap soImageSplitDropDown = smallSize ? cached("ToolbarButtonsSplitDropDown22.png") 
                : cached("ToolbarButtonsSplitDropDown50.png");

        if(soImageSplit.isNull() || soImageSplitDropDown.isNull())
            return false;

        QRect rcButton = opt->rect;
        QRect popupr = subControlRect(QStyle::CC_ToolButton, toolbutton, QStyle::SC_ToolButtonMenu, w);

        QRect rcSplit = smallSize ? QRect(QPoint(rcButton.left(), rcButton.top()), QPoint(rcButton.right() - popupr.width()-2, rcButton.bottom())) 
            : QRect(QPoint(rcButton.left(), rcButton.top()), QPoint(rcButton.right(), rcButton.bottom() - popupr.height()-2));

        int stateSplit = -1;

        if (/*enabledSplit && (selected || pressed || popuped)*/false)
        {
            stateSplit = 4;
        }
        else if (!enabled)
        {
            //            if (isKeyboardSelected(selected)) nStateSplit = 5;
        }
        else if (checked)
        {
            if (popuped) stateSplit = 5;
            else if (!selected && !pressed) stateSplit = 2;
            else if (selected && !pressed) stateSplit = 3;
            else if (/*isKeyboardSelected(bPressed) ||*/ (selected && pressed)) stateSplit = 1;
            else if (pressed) stateSplit = 2;
            else if (selected || pressed) stateSplit = !mouseInSplit ? 5 : 0;
        }
        else
        {
            if (popuped) stateSplit = 5;
            else if (/*isKeyboardSelected(bPressed) ||*/ (selected && pressed)) stateSplit = 1;
            else if (selected || pressed) stateSplit = !mouseInSplit ? 5 : 0;
        }
        stateSplit += specialOffset;

        if (stateSplit != -1)
            drawImage(soImageSplit, *p, rcSplit, getSource(soImageSplit.rect(), stateSplit, 6+specialOffset),
                      QRect(QPoint(2, 2), QPoint(2, 2)),  QColor(0xFF, 0, 0xFF));

        QRect rcSplitDropDown = smallSize ? QRect(QPoint(rcButton.right() - popupr.width()-1, rcButton.top()), QPoint(rcButton.right(), rcButton.bottom())) :
                                             QRect(QPoint(rcButton.left(), rcButton.bottom() - popupr.height()-1), QPoint(rcButton.right(), rcButton.bottom()));

        int stateSplitDropDown = -1;

        if (/*enabledDropDown && (selected || pressed || popuped)*/false)
        {
            stateSplitDropDown = 3;
        }
        else if (!enabled)
        {
            //            if (isKeyboardSelected(selected)) stateSplitDropDown = 4;
        }
        else if (checked)
        {
            if (popuped) stateSplitDropDown = 2;
            else if (!selected && !pressed) stateSplitDropDown = 2;
            else if (/*isKeyboardSelected(pressed) ||*/ (selected && pressed)) stateSplitDropDown = 0;
            else if (selected || pressed) stateSplitDropDown = !mouseInSplitDropDown ? 4 : 0;
            else stateSplitDropDown = 4;
        }
        else
        {
            if (popuped) stateSplitDropDown = 2;
            else if (/*isKeyboardSelected(pressed) ||*/ (selected && pressed)) stateSplitDropDown = 0;
            else if (selected || pressed) stateSplitDropDown = !mouseInSplitDropDown ? 4 : 0;
        }

        stateSplitDropDown += specialOffset;
        if (stateSplitDropDown != -1)
            drawImage(soImageSplitDropDown, *p, rcSplitDropDown, getSource(soImageSplitDropDown.rect(), stateSplitDropDown, 5+specialOffset), 
                QRect(QPoint(2, 2), QPoint(2, 2)), QColor(0xFF, 0, 0xFF));
        return true;
    }
    return false;
}

/*! \internal */
bool OfficeStyle::drawToolButton(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    if (const QStyleOptionToolButton* toolbutton = qstyleoption_cast<const QStyleOptionToolButton*>(opt))
    {
        if (widget)
        {
            QPixmap soImage;
            if(widget->property(qtn_PopupButtonGallery).toBool())
            {
                soImage = cached("ControlGalleryPopup.png");
            }
            else if(widget->property(qtn_ScrollUpButtonGallery).toBool())
            {
                soImage = cached("ControlGalleryUp.png");
            }
            else if(widget->property(qtn_ScrollDownButtonGallery).toBool())
            {
                soImage = cached("ControlGalleryDown.png");
            }

            if (!soImage.isNull())
            {
                bool enabled  = opt->state & State_Enabled;
                bool selected = opt->state & State_MouseOver;
                bool pressed  = opt->state & State_Sunken;
//                bool popuped  = (toolbutton->activeSubControls & QStyle::SC_ToolButtonMenu) && (opt->state & State_Sunken);
                #define CALCSTATE \
                    (!enabled ? 4 : pressed ? 3 : selected ? 2 : selected || pressed ? 1 : 0)

                drawImage(soImage, *p, toolbutton->rect, getSource(soImage.rect(), CALCSTATE, 5), QRect(QPoint(3, 3), QPoint(3, 3)), QColor(0xFF, 0, 0xFF));
                return true;
            }
        }
        QRect button, menuarea;
        button = proxy()->subControlRect(CC_ToolButton, toolbutton, SC_ToolButton, widget);
        menuarea = proxy()->subControlRect(CC_ToolButton, toolbutton, SC_ToolButtonMenu, widget);

        State flags = toolbutton->state & ~State_Sunken;

        if (flags & State_AutoRaise) 
        {
            if (!(flags & State_MouseOver) || !(flags & State_Enabled)) 
                flags &= ~State_Raised;
        }
        State mflags = flags;
        if (toolbutton->state & State_Sunken) 
        {
            if (toolbutton->activeSubControls & SC_ToolButton)
                flags |= State_Sunken;
            mflags |= State_Sunken;
        }

        proxy()->drawPrimitive(PE_PanelButtonTool, toolbutton, p, widget);

        QStyleOptionToolButton label = *toolbutton;
        label.state = flags;
        proxy()->drawControl(CE_ToolButtonLabel, &label, p, widget);

        if (toolbutton->subControls & SC_ToolButtonMenu) 
        {
            QStyleOption tool(0);
            tool.palette = toolbutton->palette;

            QRect ir = menuarea, rcArrow;

            if (toolbutton->toolButtonStyle == Qt::ToolButtonTextUnderIcon)
            {
                QString strFirstRow, strSecondRow;
                splitString(toolbutton->text, strFirstRow, strSecondRow);
                rcArrow = QRect(QPoint(strSecondRow.isEmpty() ? opt->rect.width()/2 - 2 : opt->rect.right()-7, opt->rect.bottom()-8), QSize(5, 4));
            }
            else
                rcArrow = QRect(QPoint((ir.left() + ir.right()-6) / 2, (ir.top() + ir.bottom()-2) / 2), QSize(5, 4));

            tool.rect = rcArrow;
            tool.state = mflags;

            proxy()->drawPrimitive(PE_IndicatorArrowDown, &tool, p, widget);
        } 
        else if (toolbutton->features & QStyleOptionToolButton::HasMenu) 
        {
            QStyleOptionToolButton newBtn = *toolbutton;
            if (toolbutton->toolButtonStyle == Qt::ToolButtonTextUnderIcon)
            {
                QString strFirstRow, strSecondRow;
                splitString(toolbutton->text, strFirstRow, strSecondRow);
                newBtn.rect = QRect(QPoint(strSecondRow.isEmpty() ? opt->rect.width()/2 - 2 : opt->rect.right()-8, opt->rect.bottom()-8), QSize(5, 5));
            }
            else
            {
                QRect ir = menuarea;
                int mbi = proxy()->pixelMetric(PM_MenuButtonIndicator, toolbutton, widget);

                if (!isDPIAware())
                    mbi = (double)mbi/((double)(DrawHelpers::getDPIToPercent()/100.0));

                newBtn.rect = QRect(QPoint(ir.right() + 4 - mbi, ir.y() + ir.height() - mbi), QSize(mbi - 5, mbi - 5));
            }
            proxy()->drawPrimitive(PE_IndicatorArrowDown, &newBtn, p, widget);
        }
        return true;
    }
    return false;
}

/*! \internal */
bool OfficeStyle::drawGroupControlEntry(const QStyleOption *, QPainter *, const QWidget *) const
{ 
    return false;
}

/*! \internal */
bool OfficeStyle::drawPanelButtonCommand(const QStyleOption* opt, QPainter* p, const QWidget* w) const 
{
    Q_UNUSED(w);
    if (const QStyleOptionButton* btn = qstyleoption_cast<const QStyleOptionButton*>(opt)) 
    {
        QPixmap soImage = cached("PushButton.png");

        if (soImage.isNull())
        {
            Q_ASSERT(false);
            return false;
        }

        bool enabled  = opt->state & State_Enabled;
        bool checked  = opt->state & State_On;
        bool selected = opt->state & State_MouseOver;
        bool pressed  = opt->state & State_Sunken;
        bool defaultBtn = btn->features & QStyleOptionButton::DefaultButton;

        int state = defaultBtn ? 4 : 0;

        if (!enabled)
            state = 3;
        else if (checked && !selected && !pressed) state = 2;
        else if (checked && selected && !pressed) state = 1;
        else if ((selected && pressed) /*|| qobject_cast<const QPushButton*>(w)*/) state = 2;
        else if (selected || pressed) state = 1;

        drawImage(soImage, *p, opt->rect, getSource(soImage.rect(), state, 5), 
            QRect(QPoint(4, 4), QPoint(4, 4)), QColor(0xFF, 0, 0xFF));
        return true;
    }
    return false;
}

/*! \internal */
bool OfficeStyle::drawIndicatorToolBarSeparator(const QStyleOption* opt, QPainter* p, const QWidget* w) const 
{
    QStyleOption option = *opt;
    option.palette.setColor(QPalette::Dark, m_clrToolbarSeparator);
    option.palette.setColor(QPalette::Light, QColor(255, 255, 255));

    /* QWindowsStyle*/ QCommonStyle::drawPrimitive(QStyle::PE_IndicatorToolBarSeparator, &option, p, w);
    return true; 
}

/*! \internal */
bool OfficeStyle::drawIndicatorCheckRadioButton(PrimitiveElement element, const QStyleOption* option, 
                                                    QPainter* painter, const QWidget* widget) const
{
    Q_UNUSED(widget);
    if (const QStyleOptionButton* but = qstyleoption_cast<const QStyleOptionButton*>(option))
    {
        QStyleOptionButton but1(*but);
        but1.rect.setWidth(13); but1.rect.setHeight(13);
        if (paintAnimation(tp_PrimitiveElement, (int)element, &but1, painter, widget))
            return true;
    }

    bool isRadio = (element == PE_IndicatorRadioButton);
    QPixmap soImage = cached(isRadio ? QString("ToolbarButtonRadioButton.png") : QString("ToolbarButtonCheckBox.png"));
    int state = 0;
    bool enabled  = option->state & State_Enabled;
    bool checked  = option->state & State_On;
    bool noChange = option->state & State_NoChange;
    bool selected = option->state & State_MouseOver;
    bool pressed  = option->state & State_Sunken;

    int stateChecked = checked ? 1 : 0;
    if (!isRadio && noChange)
        stateChecked = 2;

    if (!enabled)
        state = 3;
    else if (selected && pressed)
        state = 2;
    else if (selected)
        state = 1;

    if (stateChecked == 1)
        state += 4;

    if (stateChecked == 2)
        state += 8;

    QRect rc = option->rect;
    rc.setWidth(13); rc.setHeight(13);
    drawImage(soImage, *painter, rc, getSource(soImage.rect(), state, isRadio ? 8 : 12));
    return true;
}

/*! \internal */
bool OfficeStyle::drawFrameDefaultButton(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    Q_UNUSED(opt);
    Q_UNUSED(p);
    Q_UNUSED(widget);
    return true;
}

/*! \internal */
bool OfficeStyle::drawIndicatorToolBarHandle(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);
    p->translate(opt->rect.x(), opt->rect.y());
    QRect rect(opt->rect);
    if (opt->state & State_Horizontal) 
    {
        for (int y = 4; y < rect.height() - 4; y += 4)
        {
            p->fillRect(QRect(QPoint(3, y+1), QPoint(4, y + 2)), QColor(234, 251, 251));
            p->fillRect(QRect(QPoint(2, y), QPoint(3, y + 1)), m_ToolbarGripper);
        }
    } 
    else 
    {
        for (int x = 4; x < opt->rect.width() - 4; x += 4)
        {
            p->fillRect(QRect(QPoint(x + 1, 3), QPoint(x + 2, 4)), QColor(234, 251, 251));
            p->fillRect(QRect(QPoint(x, 2), QPoint(x + 1, 3)), m_ToolbarGripper);
        }
    }
    return true;
}

// for SizeGrip
/*! \internal */
bool OfficeStyle::drawSizeGrip(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);
    QPoint pt(opt->rect.right() - 3, opt->rect.bottom() - 3);
    for (int y = 0; y < 3; y++)
    {
        for (int x = 0; x < 3 - y; x++)
        {
            p->fillRect(QRect(QPoint(pt.x() + 1 - x * 4, pt.y() + 1 - y * 4), QSize(2, 2)), QColor(255, 255, 255));
            p->fillRect(QRect(QPoint(pt.x() + 0 - x * 4, pt.y() + 0 - y * 4), QSize(2, 2)), m_clrToolbarSeparator);
        }
    }
    return true;
}

/*! \internal */
static QRect getSourceH(QRect rcSrc, int nState, int nCount)
{
    QRect rcImage(0, 0, rcSrc.width() / nCount, rcSrc.height());
    rcImage.translate(nState * rcImage.width(), 0);
    return rcImage;
}

// for Slider
/*! \internal */
bool OfficeStyle::drawSlider(const QStyleOptionComplex* opt, QPainter* p, const QWidget* w) const
{
    if (const QStyleOptionSlider* slider = qstyleoption_cast<const QStyleOptionSlider*>(opt))
    {
        QRect groove = proxy()->subControlRect(CC_Slider, opt, SC_SliderGroove, w);
        QRect handle = proxy()->subControlRect(CC_Slider, opt, SC_SliderHandle, w);

        int thickness  = proxy()->pixelMetric(PM_SliderControlThickness, slider, w);
        int len        = proxy()->pixelMetric(PM_SliderLength, slider, w);
        int ticks = slider->tickPosition;

        if ((opt->subControls & SC_SliderHandle)) 
        {
            int stateId = 0;
            if (!(slider->state & State_Enabled))
                stateId = 4;
            else if (slider->activeSubControls & SC_SliderHandle && (slider->state & State_Sunken))
                stateId = 2;
            else if (slider->activeSubControls & SC_SliderHandle && (slider->state & State_MouseOver))
                stateId = 1;
            else if (slider->state & State_HasFocus)
                stateId = 3;
            else
                stateId = 0;

            QPixmap hndl;
            QRect rcSrc;
            QRect rcSizingMargins = QRect(QPoint(0, 0), QPoint(0, 0));
            if (slider->orientation == Qt::Horizontal)
            {
                if (slider->tickPosition == QSlider::TicksAbove)
                {
                    hndl = cached("TrackBarUp13.png");
                    rcSizingMargins = QRect(QPoint(3, 6), QPoint(6, 10));
                    rcSrc = getSource(hndl.rect(), stateId, 5);
                }
                else if (slider->tickPosition == QSlider::TicksBelow)
                {
                    rcSizingMargins = QRect(QPoint(3, 6), QPoint(6, 10));
                    hndl = cached("TrackBarDown13.png");
                    rcSrc = getSource(hndl.rect(), stateId, 5);
                }
                else
                {
                    rcSizingMargins = QRect(QPoint(5, 5), QPoint(5, 5));
                    hndl = cached("TrackbarVertical.png");
                    rcSrc = getTheme() == Office2010Silver || getTheme() == Office2007Aqua ? getSource(hndl.rect(), stateId, 5) : getSourceH(hndl.rect(), stateId, 5);
                }
            }
            else if (slider->orientation == Qt::Vertical)
            {
                if (slider->tickPosition == QSlider::TicksLeft)
                {
                    rcSizingMargins = QRect(QPoint(10, 3), QPoint(6, 6));
                    hndl = cached("TrackBarLeft13.png");
                    rcSrc = getTheme() == Office2010Silver || getTheme() == Office2007Aqua ? getSource(hndl.rect(), stateId, 5) : getSourceH(hndl.rect(), stateId, 5);
                }
                else if (slider->tickPosition == QSlider::TicksRight)
                {
                    rcSizingMargins = QRect(QPoint(6, 3), QPoint(10, 6));
                    hndl = cached("TrackbarRight13.png");
                    rcSrc = getTheme() == Office2010Silver || getTheme() == Office2007Aqua ? getSource(hndl.rect(), stateId, 5) : getSourceH(hndl.rect(), stateId, 5);
                }
                else
                {
                    rcSizingMargins = QRect(QPoint(5, 5), QPoint(5, 5));
                    hndl = cached("TrackbarHorizontal.png");
                    rcSrc = getSource(hndl.rect(), stateId, 5);
                }
            }

            if (hndl.isNull())
                return false;

            if ((slider->subControls & SC_SliderGroove) && groove.isValid()) 
            {
                int mid = thickness / 2;

                if (ticks & QSlider::TicksAbove)
                    mid += len / 8;
                if (ticks & QSlider::TicksBelow)
                    mid -= len / 8;

                p->setPen(slider->palette.shadow().color());
                if (slider->orientation == Qt::Horizontal) 
                {
                    qDrawWinPanel(p, groove.x(), groove.y() + mid - 2, groove.width(), 4, slider->palette, true);
                    p->drawLine(groove.x() + 1, groove.y() + mid - 1, groove.x() + groove.width() - 3, groove.y() + mid - 1);
                } 
                else 
                {
                    qDrawWinPanel(p, groove.x() + mid - 2, groove.y(), 4, groove.height(), slider->palette, true);
                    p->drawLine(groove.x() + mid - 1, groove.y() + 1, groove.x() + mid - 1, groove.y() + groove.height() - 3);
                }
            }

            if (slider->subControls & SC_SliderTickmarks) 
            {
                QStyleOptionSlider tmpSlider = *slider;
                tmpSlider.subControls = SC_SliderTickmarks;
                QCommonStyle::drawComplexControl(CC_Slider, &tmpSlider, p, w);
            }

            drawPixmap(hndl, *p, handle, rcSrc, false, rcSizingMargins);
            return true;
        }
    }
    return false;
}

// for ScrollBar
/*! \internal */
bool OfficeStyle::drawScrollBar(const QStyleOptionComplex* opt, QPainter* p, const QWidget* w) const
{
    if (const QStyleOptionSlider* scrollbar = qstyleoption_cast<const QStyleOptionSlider*>(opt))
    {
        // Make a copy here and reset it for each primitive.
        QStyleOptionSlider newScrollbar(*scrollbar);
        if (scrollbar->minimum == scrollbar->maximum)
            newScrollbar.state &= ~State_Enabled; //do not draw the slider.

        State saveFlags = scrollbar->state;

        newScrollbar.state = saveFlags;
        newScrollbar.rect = opt->rect;

        bool light = w && (qobject_cast<const RibbonGallery*>(w->parentWidget()) ||
                           qobject_cast<const QDialog*>(w->topLevelWidget()) || ::isAncestor(w));

        QPixmap soImage;
        if (opt->state & State_Horizontal) 
        {
            soImage = light ? cached("ControlGalleryScrollHorizontalLight.png")
                             : cached("ControlGalleryScrollHorizontalDark.png");
            drawImage(soImage, *p, newScrollbar.rect, getSource(soImage.rect(), 0, 2),
                QRect(QPoint(0, 1), QPoint(0, 1)));
        }
        else
        {
            soImage = light ? cached("ControlGalleryScrollVerticalLight.png")
                             : cached("ControlGalleryScrollVerticalDark.png");
            drawImage(soImage, *p, newScrollbar.rect, getSource(soImage.rect(), 0, 2),
                QRect(QPoint(1, 0), QPoint(1, 0)));
        }

        if (scrollbar->subControls & SC_ScrollBarSubLine) 
        {
            newScrollbar.rect = scrollbar->rect;
            newScrollbar.state = saveFlags;
            newScrollbar.rect = proxy()->subControlRect(CC_ScrollBar, &newScrollbar, SC_ScrollBarSubLine, w);

            if (opt->state & State_Horizontal) 
                newScrollbar.rect.adjust(0, 1, 0, -1);
            else
                newScrollbar.rect.adjust(1, 0, -1, 0);

            if (newScrollbar.rect.isValid()) 
                proxy()->drawControl(CE_ScrollBarSubLine, &newScrollbar, p, w);
        }

        if (scrollbar->subControls & SC_ScrollBarAddLine) 
        {
            newScrollbar.rect = scrollbar->rect;
            newScrollbar.state = saveFlags;
            newScrollbar.rect = proxy()->subControlRect(CC_ScrollBar, &newScrollbar, SC_ScrollBarAddLine, w);

            if (opt->state & State_Horizontal) 
                newScrollbar.rect.adjust(0, 1, 0, -1);
            else
                newScrollbar.rect.adjust(1, 0, -1, 0);

            if (newScrollbar.rect.isValid()) 
                proxy()->drawControl(CE_ScrollBarAddLine, &newScrollbar, p, w);
        }

        if (scrollbar->subControls & SC_ScrollBarSubPage) 
        {
            newScrollbar.rect = scrollbar->rect;
            newScrollbar.state = saveFlags;
            newScrollbar.rect = proxy()->subControlRect(CC_ScrollBar, &newScrollbar, SC_ScrollBarSubPage, w);

            if (newScrollbar.rect.isValid()) 
            {
                if (!(scrollbar->activeSubControls & SC_ScrollBarSubPage))
                    newScrollbar.state &= ~(State_Sunken | State_MouseOver);
                proxy()->drawControl(CE_ScrollBarSubPage, &newScrollbar, p, w);
            }
        }

        if (scrollbar->subControls & SC_ScrollBarAddPage) 
        {
            newScrollbar.rect = scrollbar->rect;
            newScrollbar.state = saveFlags;
            newScrollbar.rect = proxy()->subControlRect(CC_ScrollBar, &newScrollbar, SC_ScrollBarAddPage, w);

            if (newScrollbar.rect.isValid()) 
            {
                if (!(scrollbar->activeSubControls & SC_ScrollBarAddPage))
                    newScrollbar.state &= ~(State_Sunken | State_MouseOver);
                proxy()->drawControl(CE_ScrollBarAddPage, &newScrollbar, p, w);
            }
        }

        if (scrollbar->subControls & SC_ScrollBarSlider) 
        {
            newScrollbar.rect = scrollbar->rect;
            newScrollbar.state = saveFlags;
            newScrollbar.rect = proxy()->subControlRect(CC_ScrollBar, &newScrollbar, SC_ScrollBarSlider, w);

            if (opt->state & State_Horizontal) 
                newScrollbar.rect.adjust(0, 1, 0, -1);
            else
                newScrollbar.rect.adjust(1, 0, -1, 0);

            if (newScrollbar.rect.isValid()) 
                proxy()->drawControl(CE_ScrollBarSlider, &newScrollbar, p, w);
        }
        return true;
    }
    return false;
}

/*! \internal */
static QRect offsetSourceRect(QRect rc, int state)
{
    rc.translate(0, (state - 1) * rc.height());
    return rc;
}

/*! \internal */
bool OfficeStyle::drawScrollBarLine(ControlElement element, const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    if (const QStyleOptionSlider* scrollbar = qstyleoption_cast<const QStyleOptionSlider*>(opt))
    {
        bool light = w && (qobject_cast<const RibbonGallery*>(w->parentWidget()) ||
                           qobject_cast<const QDialog*>(w->topLevelWidget()) || ::isAncestor(w));

        QPixmap soImage;
        if (opt->state & State_Horizontal)
            soImage = light ? cached("ControlGalleryScrollArrowsHorizontalLight.png")
                            : cached("ControlGalleryScrollArrowsHorizontalDark.png"); 
        else
            soImage = light ? cached("ControlGalleryScrollArrowsVerticalLight.png")
                            : cached("ControlGalleryScrollArrowsVerticalDark.png"); 

        bool enabled  = opt->state & State_Enabled;
        bool selected = opt->state & State_MouseOver;
        bool pressed  = opt->state & State_Sunken;

        int state = -1;
        if (!enabled)
            state = 0;
        else if (selected && pressed)
        {
            if(((scrollbar->activeSubControls & SC_ScrollBarAddLine) && (element == CE_ScrollBarAddLine)) ||
               ((scrollbar->activeSubControls & SC_ScrollBarSubLine) && (element == CE_ScrollBarSubLine)) )
                state = 3;
            else
                state = 1;
        }
        else if(selected)
        {
            if(((scrollbar->activeSubControls & SC_ScrollBarAddLine) && (element == CE_ScrollBarAddLine)) ||
               ((scrollbar->activeSubControls & SC_ScrollBarSubLine) && (element == CE_ScrollBarSubLine)) )
                state = 2;
            else
                state = 1;
        }

        if (state != -1) 
            drawImage(soImage, *p, opt->rect, getSource(soImage.rect(), state, 4), QRect(QPoint(3, 3), QPoint(3, 3)), QColor(0xFF, 0, 0xFF));

        QPixmap soImageArrowGlyphs;
        if (!light)
            soImageArrowGlyphs = cached("ControlGalleryScrollArrowGlyphsDark.png");

        if (soImageArrowGlyphs.isNull())
            soImageArrowGlyphs = cached("ControlGalleryScrollArrowGlyphs.png");

        int number = -1;
        if (opt->state & State_Horizontal) 
        {
            QRect rcSrc;
            if (element == CE_ScrollBarAddLine)
                number = !enabled ? 16 : state != 0 ? 14 : 13;
            else
                number = !enabled ? 12 : state != 0 ? 10 : 9;
        }
        else
        {
            if (element == CE_ScrollBarAddLine)
                number = !enabled ? 8 : state != 0 ? 6 : 5;
            else
                number = !enabled ? 4 : state != 0 ? 2 : 1;
        }
        QRect rcArrowGripperSrc(0, 0, 9, 9);
        QRect rcSrc = offsetSourceRect(rcArrowGripperSrc, number);
        QRect rcArrowGripper(QPoint((opt->rect.left() + opt->rect.right() - 8) / 2, (opt->rect.top() + opt->rect.bottom() - 8) / 2), QSize(9,9));

        QSize sz = proxy()->sizeFromContents(QStyle::CT_ScrollBar, opt, scrollbar->rect.size(), w);   
        if (opt->state & State_Horizontal) 
        {
            if (sz.height() % 2 == 0) //Horizontal
                rcArrowGripper.setHeight(rcArrowGripper.height()+1);
        }
        else 
        {
            if (sz.width() % 2 == 0) //for vertical
                rcArrowGripper.setWidth(rcArrowGripper.width()+1);
        }

        drawImage(soImageArrowGlyphs, *p, rcArrowGripper, rcSrc, QRect(QPoint(0, 0), QPoint(0, 0)), QColor(255, 0, 255));
        return true;
    }
    return false;
}

/*! \internal */
bool OfficeStyle::drawScrollBarSlider(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    if (const QStyleOptionSlider* scrollbar = qstyleoption_cast<const QStyleOptionSlider*>(opt)) 
    {
        if (!opt->rect.isEmpty())
        {
            bool light = w && (qobject_cast<const RibbonGallery*>(w->parentWidget()) ||
                               qobject_cast<const QDialog*>(w->topLevelWidget()) || ::isAncestor(w));

            bool enabled  = opt->state & State_Enabled;
            bool selected = opt->state & State_MouseOver;
            bool pressed  = opt->state & State_Sunken;

            int state = -1;
            if (!enabled)
                state = 0;
            else if(pressed && scrollbar->activeSubControls&SC_ScrollBarSlider)
                state = 2;
            else if(selected && scrollbar->activeSubControls&SC_ScrollBarSlider)
                state = 1;
            else if((selected || pressed) && scrollbar->activeSubControls&SC_ScrollBarSlider)
                state = 2;
            else
                state = 0;


            QRect rc(opt->rect);
            QPixmap scrollThumb;
            if (opt->state & State_Horizontal)
            {
                if (!light)
                    scrollThumb = cached("ControlGalleryScrollThumbHorizontalDark.png");

                if (scrollThumb.isNull())
                    scrollThumb = cached("ControlGalleryScrollThumbHorizontal.png");
            }
            else
            {
                if (!light)
                    scrollThumb = cached("ControlGalleryScrollThumbVerticalDark.png");

                if (scrollThumb.isNull())
                    scrollThumb = cached("ControlGalleryScrollThumbVertical.png");
            }

            Q_ASSERT(!scrollThumb.isNull());

            bool show = opt->state & State_Horizontal ? rc.width() > 7 : rc.height() > 7;
            if( !rc.isEmpty() && show )
                drawImage(scrollThumb, *p, rc, getSource(scrollThumb.rect(), state, 3), QRect(QPoint(5, 5), QPoint(5, 5)));

            QRect rcGripper(QPoint(opt->rect.center().x() - 3, opt->rect.center().y() - 3), QSize(8, 8));
            if (opt->state & State_Horizontal) 
            {
                if (opt->rect.width() > 10)
                {
                    QPixmap soImageScrollThumbGripperHorizontal = cached("ControlGalleryScrollThumbGripperHorizontal.png");
//                    Q_ASSERT(soImageScrollThumbGripperHorizontal);
                    Q_ASSERT(!soImageScrollThumbGripperHorizontal.isNull()); // modified by canpool 2018-4-2 19:53:56
                    drawImage(soImageScrollThumbGripperHorizontal, *p, rcGripper, getSource(soImageScrollThumbGripperHorizontal.rect(), state, 3));
                }
            }
            else
            {
                if (opt->rect.height() > 10)
                {
                    QPixmap soImageScrollThumbGripperVertical = cached("ControlGalleryScrollThumbGripperVertical.png");
                    Q_ASSERT(!soImageScrollThumbGripperVertical.isNull());
                    drawImage(soImageScrollThumbGripperVertical, *p, rcGripper, getSource(soImageScrollThumbGripperVertical.rect(), state, 3));
                }
            }

        }
        return true;
    }
    return false;
}

/*! \internal */
bool OfficeStyle::drawScrollBarPage(ControlElement element, const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(element);
    Q_UNUSED(w);

    if (!(opt->state & State_Sunken))
        return true;

    bool light = w && (qobject_cast<const RibbonGallery*>(w->parentWidget()) ||
                       qobject_cast<const QDialog*>(w->topLevelWidget()) || ::isAncestor(w));

    if (opt->state & State_Horizontal) 
    {
        QPixmap soImage = light ? cached("ControlGalleryScrollHorizontalLight.png") : cached("ControlGalleryScrollHorizontalDark.png"); 
        if (!soImage.isNull())
            drawImage(soImage, *p, opt->rect, getSource(soImage.rect(), 1, 2), QRect(QPoint(0, 1), QPoint(0, 1)));
    }
    else
    {
        QPixmap soImage = light ? cached("ControlGalleryScrollVerticalLight.png") : cached("ControlGalleryScrollVerticalDark.png"); 
        if (!soImage.isNull())
            drawImage(soImage, *p, opt->rect, getSource(soImage.rect(), 1, 2), QRect(QPoint(1, 0), QPoint(1, 0)));
    }
    return true;
}

/*! \internal */
bool OfficeStyle::drawControlEdit(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w)
    if (const QStyleOptionFrame* panel = qstyleoption_cast<const QStyleOptionFrame*>(opt)) 
    {
        if(!qobject_cast<const QComboBox*>(w->parentWidget()))
        {
            bool enabled  = opt->state & State_Enabled;
            bool selected = opt->state & State_MouseOver;
            bool hasFocus = opt->state & State_HasFocus;
            QRect rect(panel->rect);
//            rect.adjust(1, 1, -1, -1);
            rect.adjust(0, 0, -1, -1);
            p->fillRect(rect, selected || hasFocus ? panel->palette.brush(QPalette::Current, QPalette::Base) : 
                enabled ? m_clrControlEditNormal : panel->palette.brush(QPalette::Disabled, QPalette::Base));
            if ( panel->lineWidth > 0 )
                proxy()->drawPrimitive(PE_FrameLineEdit, panel, p, w);
        }
        return true;
    }
    return false;
}

/*! \internal */
bool OfficeStyle::drawFrameLineEdit(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);
    if (const QStyleOptionFrame* panel = qstyleoption_cast<const QStyleOptionFrame*>(opt))
    {
        QRect rect(panel->rect);
        p->save();
        QPen savePen = p->pen();
        p->setPen(m_clrEditCtrlBorder);
        rect.adjust(0, 0, -1, -1);
        p->drawRect(rect);
        p->setPen(savePen);
        p->restore();
        return true;
    }
    return false;
}

/*! \internal */
bool OfficeStyle::drawComboBox(const QStyleOptionComplex* opt, QPainter* p, const QWidget* w) const
{
    if (const QStyleOptionComboBox* cmb = qstyleoption_cast<const QStyleOptionComboBox*>(opt))
    {
        State flags = opt->state;
        SubControls sub = opt->subControls;
        QRect r = opt->rect;

//        if (w && w->testAttribute(Qt::WA_UnderMouse) && w->isActiveWindow())
//            flags |= State_MouseOver;

        bool enabled  = flags & State_Enabled;
        bool selected = flags & State_MouseOver;
        bool dropped  = cmb->activeSubControls == SC_ComboBoxArrow && ((cmb->state & State_On) || (cmb->state & State_Sunken));
        bool dropButtonHot = cmb->activeSubControls == SC_ComboBoxArrow && (cmb->state & State_MouseOver);
        bool hasFocus = opt->state & State_HasFocus;
        
        if (cmb->frame) 
        {
            r.adjust(0, 0, -1, -1);
            p->fillRect(r, dropped || selected || hasFocus ? cmb->palette.brush(QPalette::Base) : 
                enabled ? m_clrControlEditNormal : cmb->palette.brush(QPalette::Disabled, QPalette::Base));

            p->save();
            QPen savePen = p->pen();
            p->setPen(m_clrEditCtrlBorder);
            p->drawRect(r);
            p->setPen(savePen);
            p->restore();
        }

        if (sub & SC_ComboBoxArrow)
        {
            QPixmap soImage = cached("ToolbarButtonsComboDropDown.png"); 
            QRect rcBtn = proxy()->subControlRect(CC_ComboBox, opt, SC_ComboBoxArrow, w);

            if (!enabled)
                drawImage(soImage, *p, rcBtn, getSource(soImage.rect(), 4, 5), QRect(QPoint(2, 2), QPoint(2, 2)));
            else if (dropped)
                drawImage(soImage, *p, rcBtn, getSource(soImage.rect(), 3, 5), QRect(QPoint(2, 2), QPoint(2, 2)));
            else if (selected)
            {
                int state = !cmb->editable || dropButtonHot || hasFocus ? 2 : selected ? 1 : 2;
                drawImage(soImage, *p, rcBtn, getSource(soImage.rect(), state, 5), QRect(QPoint(2, 2), QPoint(2, 2)));
            }
            else // NORMAL
                drawImage(soImage, *p, rcBtn, getSource(soImage.rect(), hasFocus ? 2 : 0, 5), QRect(QPoint(2, 2), QPoint(2, 2)));

            QPoint pt = rcBtn.center();     
            QRect rc(QPoint(pt.x() - 2, pt.y() - 2), QPoint(pt.x() + 3, pt.y() + 2));
            drawDropDownGlyph(p, pt, selected, dropped, enabled, false/*bVert*/);
        }
        return true;
    }
    return false;
}

/*! \internal */
bool OfficeStyle::drawDockWidgetTitle(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    if (const QStyleOptionDockWidget* dwOpt = qstyleoption_cast<const QStyleOptionDockWidget*>(opt)) 
    {
        bool verticalTitleBar = true;//v2 == 0 ? false : v2->verticalTitleBar;
        bool active = false; //opt->state & State_HasFocus;

//        QRect rect = dwOpt->rect;
//        QRect r = rect;

        // [1]
        DrawHelpers::drawGradientFill(*p, opt->rect, active ? m_palActiveCaption.color(QPalette::Light) : m_palNormalCaption.color(QPalette::Light), 
            active ? m_palActiveCaption.color(QPalette::Dark) : m_palNormalCaption.color(QPalette::Dark), verticalTitleBar);

        bool floating = false;
        if (dwOpt->movable) 
        {
            if (w && w->isWindow()) 
                floating = true;
        }

        if (!dwOpt->title.isEmpty()) 
        {
            QFont oldFont = p->font();

            if (floating) 
            {
                QFont font = oldFont;
                font.setBold(true);
                p->setFont(font);
            }

            QPalette palette = dwOpt->palette;
            palette.setColor(QPalette::Window, m_clrNormalCaptionText);
            palette.setColor(QPalette::WindowText, m_clrActiveCaptionText);
            palette.setColor(QPalette::BrightText, m_clrActiveCaptionText);

            QRect titleRect = subElementRect(SE_DockWidgetTitleBarText, opt, w);
            titleRect.adjust(4, 0, 0, 0);
/*
            if (verticalTitleBar) 
            {
                titleRect = QRect(r.left() + rect.bottom()- titleRect.bottom(),
                        r.top() + titleRect.left() - rect.left(),
                        titleRect.height(), titleRect.width());
            }
*/
            proxy()->drawItemText(p, titleRect, Qt::AlignLeft | Qt::AlignVCenter | Qt::TextShowMnemonic, palette,
                    dwOpt->state & State_Enabled, dwOpt->title,
                    floating ? (active ? QPalette::BrightText : QPalette::Window) : active ? QPalette::WindowText : QPalette::Window);
            p->setFont(oldFont);
        }
        return true;
    }
    return false;
}

// docksplitter
/*! \internal */
bool OfficeStyle::drawIndicatorDockWidgetResizeHandle(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    Q_UNUSED(widget);
    QRect rcFrame = widget->rect();

    float fHeight = (float)rcFrame.height();

    QRgb clr1 = DrawHelpers::blendColors(m_palSplitter.color(QPalette::Light).rgb(), 
        m_palSplitter.color(QPalette::Dark).rgb(), float(opt->rect.bottom() - rcFrame.top()) / fHeight);

    QRgb clr2 = DrawHelpers::blendColors(m_palSplitter.color(QPalette::Light).rgb(), 
        m_palSplitter.color(QPalette::Dark).rgb(), float(opt->rect.top() - rcFrame.top()) / fHeight);

    // [1]
    DrawHelpers::drawGradientFill(*p, opt->rect, clr1, clr2, true);
    return true;
}

/*! \internal */
static QPixmap invertPixmap(const QPixmap& pm, const QColor& invertColor )
{
    if (qRgb(0,0,0) == invertColor.rgb())
        return pm;

    QImage image = pm.toImage();
    int w = pm.width();
    int h = pm.height();

    for (int y = 0 ; y < h ; y++)
    {
        for (int x = 0; x < w ; x++)
        {
            QRgb px = image.pixel(x, y);
            if (px == qRgb(0,0,0))
                image.setPixel(x, y, invertColor.rgb());
        }
    }
    return QPixmap::fromImage(image);
}

// MDI
/*! \internal */
bool OfficeStyle::drawMdiControls(const QStyleOptionComplex* opt, QPainter* p, const QWidget* w) const
{
    QStyleOptionToolButton btnOpt;
    btnOpt.QStyleOption::operator=(*opt);
    if (opt->subControls & QStyle::SC_MdiCloseButton)
    {
        if (opt->activeSubControls & QStyle::SC_MdiCloseButton && (opt->state & State_Sunken))
        {
            btnOpt.state |= State_Sunken;
            btnOpt.state &= ~State_Raised;
        } 
        else
        {
            btnOpt.state |= State_Raised;
            btnOpt.state &= ~State_Sunken;
        }
        btnOpt.rect = proxy()->subControlRect(CC_MdiControls, opt, SC_MdiCloseButton, w);
        if (opt->activeSubControls & QStyle::SC_MdiCloseButton)
            drawRectangle(p, btnOpt.rect, opt->state & State_MouseOver, opt->state & State_Sunken,
                opt->state & State_Enabled, false, false, TypeMenuBar, BarTop);

        QPixmap pm = standardIcon(SP_TitleBarCloseButton).pixmap(16, 16);
        pm = invertPixmap(pm, getTextColor((opt->state & State_MouseOver)&&(opt->activeSubControls & QStyle::SC_MdiCloseButton),
            /*opt->state & State_Sunken*/false,  opt->state & State_Enabled, false, false, TypeMenuBar, BarNone));
        proxy()->drawItemPixmap(p, btnOpt.rect, Qt::AlignCenter, pm);
    }

    if (opt->subControls & QStyle::SC_MdiNormalButton)
    {
        if (opt->activeSubControls & QStyle::SC_MdiNormalButton && (opt->state & State_Sunken))
        {
            btnOpt.state |= State_Sunken;
            btnOpt.state &= ~State_Raised;
        } 
        else 
        {
            btnOpt.state |= State_Raised;
            btnOpt.state &= ~State_Sunken;
        }
        btnOpt.rect = proxy()->subControlRect(CC_MdiControls, opt, SC_MdiNormalButton, w);
        if (opt->activeSubControls & QStyle::SC_MdiNormalButton)
            drawRectangle(p, btnOpt.rect, opt->state & State_MouseOver, opt->state & State_Sunken,
            opt->state & State_Enabled, false, false, TypeMenuBar, BarTop);

        QPixmap pm = standardIcon(SP_TitleBarNormalButton).pixmap(16, 16);
        pm = invertPixmap(pm, getTextColor((opt->state & State_MouseOver)&&(opt->activeSubControls & QStyle::SC_MdiNormalButton),
            /*opt->state & State_Sunken*/false, opt->state & State_Enabled, false, false, TypeMenuBar, BarNone));

        proxy()->drawItemPixmap(p, btnOpt.rect, Qt::AlignCenter, pm);
    }
    if (opt->subControls & QStyle::SC_MdiMinButton) 
    {
        if (opt->activeSubControls & QStyle::SC_MdiMinButton && (opt->state & State_Sunken))
        {
            btnOpt.state |= State_Sunken;
            btnOpt.state &= ~State_Raised;
        }
        else
        {
            btnOpt.state |= State_Raised;
            btnOpt.state &= ~State_Sunken;
        }
        btnOpt.rect = proxy()->subControlRect(CC_MdiControls, opt, SC_MdiMinButton, w);

        if (opt->activeSubControls & QStyle::SC_MdiMinButton)
            drawRectangle(p, btnOpt.rect, opt->state & State_MouseOver, opt->state & State_Sunken,
            opt->state & State_Enabled, false, false, TypeMenuBar, BarTop);

        QPixmap pm = standardIcon(SP_TitleBarMinButton).pixmap(16, 16);
        pm = invertPixmap(pm, getTextColor((opt->state & State_MouseOver)&&(opt->activeSubControls & QStyle::SC_MdiMinButton),
            /*opt->state & State_Sunken*/false, opt->state & State_Enabled, false, false, TypeMenuBar, BarNone));
        proxy()->drawItemPixmap(p, btnOpt.rect, Qt::AlignCenter, pm);
    }
    return true;
}

// for TabBar
/*! \internal */
bool OfficeStyle::drawTabBarTab(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    Q_UNUSED(widget);
    if (const QStyleOptionTab* tab = qstyleoption_cast<const QStyleOptionTab*>(opt))
    {
        proxy()->drawControl(CE_TabBarTabShape, tab, p, widget);
        proxy()->drawControl(CE_TabBarTabLabel, tab, p, widget);
        return true; 
    }
    return false; 
}

/*! \internal */
bool OfficeStyle::drawTabBarTabShape(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    Q_UNUSED(widget);
    if (const QStyleOptionTab* tab = qstyleoption_cast<const QStyleOptionTab*>(opt))
    {
        QPixmap soImage = tab->position == QStyleOptionTab::Beginning || tab->position == QStyleOptionTab::OnlyOneTab ? 
            cached("TabItemTopLeft.png") : cached("TabItemTop.png"); 

        if (soImage.isNull())
        {
            Q_ASSERT(false);
            return false;
        }

        bool selected = tab->state & State_Selected;
        bool highlight = tab->state & State_MouseOver;
        bool focused = tab->state & State_HasFocus;
        bool pressed = tab->state & State_Sunken;

        QRect rect = opt->rect;
/*
        if (!selected) 
        {
            switch (tab->shape) 
            {
            case QTabBar::RoundedNorth:
                rect.adjust(0, 0, 0, -1);
                break;
            case QTabBar::RoundedSouth:
                rect.adjust(0, 1, 0, 0);
                break;
            case QTabBar::RoundedEast:
                rect.adjust(1, 0, 0, 0);
                break;
            case QTabBar::RoundedWest:
                rect.adjust(0, 0, -1, 0);
                break;
            default:
                break;
            }
        }
*/
        int state = 0;
        if (selected && focused)
            state = 4;
        if (selected && highlight)
            state = 4;
        else if (selected)
            state = 2;
        else if (pressed)
            state = 4;
        else if (highlight)
            state = 1;

        if (state == 0)
            return true;

        bool correct = widget != 0 ? qobject_cast<QMdiArea*>(widget->parentWidget()) != 0 : true;
        int borderThickness = proxy()->pixelMetric(PM_DefaultFrameWidth, opt, widget)/2;

        QSize sz;
        QTransform matrix;
        switch (tab->shape)
        {
            case QTabBar::RoundedNorth:
            case QTabBar::TriangularNorth:
                {
                    if (!selected)
                        rect.adjust(0, 0, 0, correct ? 0 : -borderThickness);

                    sz = QSize(rect.width(), rect.height());
                    matrix.rotate(0.);
                    break;
                }
            case QTabBar::RoundedSouth:
            case QTabBar::TriangularSouth:
                {
                    if (!selected)
                        rect.adjust(0 , correct ? -borderThickness : borderThickness, 0, 0);

                    sz = QSize(rect.width(), rect.height());
                    matrix.rotate(-180., Qt::XAxis);
                    break;
                }
            case QTabBar::RoundedWest:
            case QTabBar::TriangularWest:
                {
                    if (!selected)
                        rect.adjust(0, 0, correct ? 0 : -borderThickness, 0);

                    sz = QSize(rect.height(), rect.width());
                    matrix.rotate(-90.);
                    matrix.rotate(180., Qt::YAxis);
                    break;
                }
            case QTabBar::RoundedEast:
            case QTabBar::TriangularEast:
                {
                    if (!selected)
                        rect.adjust(correct ? 0 : borderThickness, 0, 0, 0);

                    sz = QSize(rect.height(), rect.width());
                    matrix.rotate(90.);
                    break;
                }
            default:
                break;
        }

        QPixmap soImageRotate(sz);
        soImageRotate.fill(Qt::transparent);

        QPainter painter(&soImageRotate);
        drawPixmap(soImage, painter, QRect(QPoint(0, 0), sz), getSource(soImage.rect(), state, 5), false, QRect(QPoint(6, 6), QPoint(6, 6)));

        QPixmap resultImage = soImageRotate.transformed(matrix);
        p->drawPixmap(rect, resultImage);

        return true;
    }
    return false;
}

/*! \internal */
bool OfficeStyle::drawTabBarTabLabel(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    if (const QStyleOptionTabV3* tab = qstyleoption_cast<const QStyleOptionTabV3*>(opt))
    {
        QPalette palette = opt->palette;  
        QColor color =  palette.color(QPalette::WindowText);

        QColor col = helper().getColor(tr("TabManager"), tr("TextColor"));
        if (col.isValid())
            color = col;

        if (!(tab->state & State_Selected) && (widget && qobject_cast<const QMainWindow*>(widget->parentWidget())))
        {
            QColor col = helper().getColor(tr("TabManager"), tr("NormalTextColor"));
            if (col.isValid())
                color = col;
        }

        if (color.isValid())
            palette.setColor(QPalette::WindowText, color);

        QStyleOptionTabV3 optTab = *tab;
        optTab.palette = palette;

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        if (defaultStyle())
            defaultStyle()->drawControl(CE_TabBarTabLabel, &optTab, p, widget);
        else
            QCommonStyle::drawControl(CE_TabBarTabLabel, &optTab, p, widget);
#else
            QWindowsStyle::drawControl(CE_TabBarTabLabel, &optTab, p, widget);
#endif

        return true;
    }
    return false;
}

// for SpinBox
/*! \internal */
bool OfficeStyle::drawSpinBox(const QStyleOptionComplex* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);
    if (const QStyleOptionSpinBox* sb = qstyleoption_cast<const QStyleOptionSpinBox *>(opt))
    {
        bool selected = opt->state & State_MouseOver;
        bool enabled  = opt->state & State_Enabled;
        bool hasFocus = opt->state & State_HasFocus;

        if (sb->frame && (sb->subControls & SC_SpinBoxFrame)) 
        {
            p->save();
            QRect r = proxy()->subControlRect(CC_SpinBox, sb, SC_SpinBoxFrame, w);

            p->fillRect(r, selected || hasFocus ? sb->palette.brush(QPalette::Base) : 
                enabled ? m_clrControlEditNormal : sb->palette.brush(QPalette::Disabled, QPalette::Base));

            QPen savePen = p->pen();
            p->setPen(m_clrEditCtrlBorder);
            r.adjust(0, 0, -1, -1);
            p->drawRect(r);
            p->setPen(savePen);
            p->restore();
        }

        QStyleOptionSpinBox copy = *sb;
        copy.state = State_None;
        copy.subControls |= SC_SpinBoxUp;
        QRect rcTop = proxy()->subControlRect(CC_SpinBox, &copy, SC_SpinBoxUp, w);

        QPixmap soBackground = cached("ToolbarButtonSpinArrowsVertical.png"); 

        QPixmap pImage = rcTop.width() > 12 && rcTop.height() > 12 ? cached("ControlGalleryScrollArrowGlyphs.png") :
            cached("ToolbarButtonSpinArrowGlyphs.png");

        if (soBackground.isNull() || pImage.isNull())
        {
            Q_ASSERT(false);
            return false;
        }

        copy = *sb;

        if (sb->subControls & SC_SpinBoxUp)
        {
            bool pressedButton = false;
            bool hotButton = false;
            bool enabledIndicator = true;

            rcTop = proxy()->subControlRect(CC_SpinBox, sb, SC_SpinBoxUp, w);

            if (!(sb->stepEnabled & QAbstractSpinBox::StepUpEnabled) || !(sb->state & State_Enabled))
                enabledIndicator = false;
            else if (sb->activeSubControls == SC_SpinBoxUp && (sb->state & State_Sunken))
                pressedButton = true;
            else if (sb->activeSubControls == SC_SpinBoxUp && (sb->state & State_MouseOver))
                hotButton = true;

            int state = !enabled ? 4 : (hasFocus || selected) && pressedButton ? 3 : hotButton ? 2 : (hasFocus || selected) ? 1 : 0;
            if (sb->state & State_MouseOver)
               drawImage(soBackground, *p, rcTop, getSource(soBackground.rect(), state, 10), QRect(QPoint(2, 2), QPoint(2, 2)), QColor(0xFF, 0, 0xFF));

            state = !enabledIndicator || !enabled ? 3 : selected && pressedButton ? 2 : selected ? 2 : selected ? 1 :  0;
            QRect rcSrc = getSource(pImage.rect(), state, 16);
            QSize sz(rcSrc.size());
            QRect rcDraw(QPoint(rcTop.left() + (rcTop.width()/2-sz.width()/2), rcTop.top()+(rcTop.height()/2-sz.height()/2)), rcSrc.size());
            drawImage(pImage, *p, rcDraw, rcSrc, QRect(QPoint(0, 0), QPoint(0, 0)), QColor(0xFF, 0, 0xFF));
        }

        if (sb->subControls & SC_SpinBoxDown) 
        {
            bool pressedButton = false;
            bool hotButton = false;
            bool enabledIndicator = true;

            QRect rcBottom = proxy()->subControlRect(CC_SpinBox, sb, SC_SpinBoxDown, w);

            if (!(sb->stepEnabled & QAbstractSpinBox::StepDownEnabled) || !(sb->state & State_Enabled))
                enabledIndicator = false;
            else if (sb->activeSubControls == SC_SpinBoxDown && (sb->state & State_Sunken))
                pressedButton = true;
            else if (sb->activeSubControls == SC_SpinBoxDown && (sb->state & State_MouseOver))
                hotButton = true;

            int state = !enabled ? 9 : (hasFocus || selected) && pressedButton ? 8 : hotButton ? 7 : (hasFocus || selected) ? 6 : 5;
            if (sb->state & State_MouseOver)
                drawImage(soBackground, *p, rcBottom, getSource(soBackground.rect(), state, 10), QRect(QPoint(2, 2), QPoint(2, 2)), QColor(0xFF, 0, 0xFF));

            state = 4 + (!enabledIndicator || !enabled ? 3 : selected && pressedButton ? 2 : selected ? 2 : selected ? 1 :  0);
            QRect rcSrc = getSource(pImage.rect(), state, 16);
            QSize sz(rcSrc.size());
            QRect rcDraw(QPoint(rcBottom.left() + (rcBottom.width()/2-sz.width()/2), rcBottom.top()+(rcBottom.height()/2-sz.height()/2)), rcSrc.size());
            drawImage(pImage, *p, rcDraw, rcSrc, QRect(QPoint(0, 0), QPoint(0, 0)), QColor(0xFF, 0, 0xFF));
        }
        return true;
    }
    return false;
}

// for ProgressBar
/*! \internal */
bool OfficeStyle::drawProgressBarGroove(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);
    p->save();
    QRect r = opt->rect;
    p->fillRect(r, m_clrControlEditNormal);
    QPen savePen = p->pen();
    p->setPen(m_clrEditCtrlBorder);
    r.adjust(0, 0, -1, -1);
    p->drawRect(r);
    p->setPen(savePen);
    p->restore();
    return true;
}

/*! \internal */
bool OfficeStyle::drawToolBoxTab(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{ 
    if (const QStyleOptionToolBox* tb = qstyleoption_cast<const QStyleOptionToolBox*>(opt))
    {
        proxy()->drawControl(CE_ToolBoxTabShape, tb, p, widget);
        proxy()->drawControl(CE_ToolBoxTabLabel, tb, p, widget);
        return true;
    }
    return false;
}

/*! \internal */
bool OfficeStyle::drawToolBoxTabShape(const QStyleOption* opt, QPainter* p, const QWidget*) const
{
    QPixmap soImage = cached("ShortcutBarItems.png"); 
    if (soImage.isNull())
    {
        Q_ASSERT(false);
        return false;
    }
    int state = 0;
    if (opt->state & State_Sunken)
        state = 3;
    else if (opt->state & State_MouseOver)
        state = 2;
    drawImage(soImage, *p, opt->rect, getSource(soImage.rect(), state, 4), QRect(QPoint(2, 2), QPoint(2, 2)));

    p->fillRect(opt->rect.left(), opt->rect.bottom() - 1, opt->rect.width(), 1, m_clrShortcutItemShadow);

    return true; 
}

/*! \internal */
bool OfficeStyle::drawToolBoxTabLabel(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    if (const QStyleOptionToolBoxV2* tb = qstyleoption_cast<const QStyleOptionToolBoxV2*>(opt)) 
    {
        QStyleOptionToolBoxV2 optTB = *tb;

        optTB.palette.setColor(QPalette::ButtonText, helper().getColor(tr("ShortcutBar"), tr("NormalText")));

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        if (defaultStyle())
            defaultStyle()->drawControl(CE_ToolBoxTabLabel, &optTB, p, widget);
        else
            QCommonStyle::drawControl(CE_ToolBoxTabLabel, &optTB, p, widget);
#else
            QWindowsStyle::drawControl(CE_ToolBoxTabLabel, &optTB, p, widget);
#endif
        return true;
    }
    return false;
}

// for ViewItem
/*! \internal */
bool OfficeStyle::drawItemViewItem(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    if (defaultStyle())
        defaultStyle()->drawControl(CE_ItemViewItem, opt, p, widget);
    else
        QCommonStyle::drawControl(CE_ItemViewItem, opt, p, widget);
#else
        QWindowsStyle::drawControl(CE_ItemViewItem, opt, p, widget);
#endif
    return true;
}

/*! \internal */
bool OfficeStyle::drawPanelItemViewItem(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    if (const QStyleOptionViewItemV4* vopt = qstyleoption_cast<const QStyleOptionViewItemV4*>(opt))
    {
        if (qobject_cast<const QTableView*>(widget))
        {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
            if (defaultStyle())
                defaultStyle()->drawPrimitive(PE_PanelItemViewItem, vopt, p, widget);
            else
                QCommonStyle::drawPrimitive(PE_PanelItemViewItem, vopt, p, widget);
#else
            QWindowsStyle::drawPrimitive(PE_PanelItemViewItem, vopt, p, widget);
#endif
            return true;
        }

        const QAbstractItemView* view = qobject_cast<const QAbstractItemView*>(widget);
        if (!view)
            return false;

        Q_UNUSED(widget);
        QPixmap soImage = cached("ListBox.png");
        if (soImage.isNull())
        {
            Q_ASSERT(false);
            return false;
        }

        bool enabled  = vopt->state & QStyle::State_Enabled;
        bool selected = vopt->state & QStyle::State_Selected;
        bool hasFocus = vopt->state & QStyle::State_HasFocus;
        //        bool hasFocus = vopt->state & QStyle::State_Active;
        bool hover = vopt->state & QStyle::State_MouseOver;

        QRect itemRect = subElementRect(QStyle::SE_ItemViewItemFocusRect, opt, widget).adjusted(-1, 0, 1, 0);
        itemRect.setTop(vopt->rect.top());
        itemRect.setBottom(vopt->rect.bottom());

        QSize sectionSize = itemRect.size();
        if (vopt->showDecorationSelected)
            sectionSize = vopt->rect.size();

        if (view->selectionBehavior() == QAbstractItemView::SelectRows)
            sectionSize.setWidth(vopt->rect.width());

        if (view->selectionMode() == QAbstractItemView::NoSelection)
            hover = false;

        int state = -1;
        if ((selected || hover) && enabled)
            state = selected && hover? 2 : hasFocus && selected ? 1 : !hasFocus && selected ? 3 : 0;

        if (vopt->backgroundBrush.style() != Qt::NoBrush) 
        {
//            QPointF oldBO = p->brushOrigin();
            p->setBrushOrigin(vopt->rect.topLeft());
            p->fillRect(vopt->rect, vopt->backgroundBrush);
        }
        
       if (state != -1)
       {
            if (vopt->showDecorationSelected) 
            {
//                QPixmap pixmap = soImage;
//              const int frame = 2; //Assumes a 2 pixel pixmap border
//                QRect srcRect = QRect(0, 0, sectionSize.width(), sectionSize.height());
//                QRect pixmapRect = vopt->rect;
                bool reverse = vopt->direction == Qt::RightToLeft;
                bool leftSection = vopt->viewItemPosition == QStyleOptionViewItemV4::Beginning;
                bool rightSection = vopt->viewItemPosition == QStyleOptionViewItemV4::End;

                if (vopt->viewItemPosition == QStyleOptionViewItemV4::OnlyOne || vopt->viewItemPosition == QStyleOptionViewItemV4::Invalid)
                {
                    QRect rect(QPoint(vopt->rect.left(), vopt->rect.top()), sectionSize);
                    drawImage(soImage, *p, rect, getSource(soImage.rect(), state, 4), QRect(QPoint(4, 4), QPoint(4, 4)));
                }
                else if (reverse ? rightSection : leftSection)
                {
                    QRect rect(QPoint(vopt->rect.left(), vopt->rect.top()), sectionSize);
                    QRect rectImage = soImage.rect();
                    rectImage.adjust(0, 0, -4, 0);
                    drawImage(soImage, *p, rect, getSource(rectImage, state, 4), QRect(QPoint(4, 4), QPoint(4, 4)));
                } 
                else if (reverse ? leftSection : rightSection) 
                {
                    QRect rectImage = soImage.rect();
                    QPixmap copyPix = soImage.copy(4, 0, rectImage.width(), rectImage.height());
                    QRect rect(QPoint(vopt->rect.left(), vopt->rect.top()), sectionSize);
                    drawImage(copyPix, *p, rect, getSource(copyPix.rect(), state, 4), QRect(QPoint(4, 4), QPoint(4, 4)));
                }
                else if (vopt->viewItemPosition == QStyleOptionViewItemV4::Middle)
                {
                    QRect rectImage = soImage.rect();
                    QPixmap copyPix = soImage.copy(4, 0, rectImage.width()-8, rectImage.height());
                    QRect rect(QPoint(vopt->rect.left(), vopt->rect.top()), sectionSize);
                    drawImage(copyPix, *p, rect, getSource(copyPix.rect(), state, 4), QRect(QPoint(4, 4), QPoint(4, 4)));
                }
            }
            else
            {
                QRect rect(QPoint(vopt->rect.left(), vopt->rect.top()), sectionSize);
                drawImage(soImage, *p, rect, getSource(soImage.rect(), state, 4), QRect(QPoint(4, 4), QPoint(4, 4)));
            }
        }
        return true;
    }
    return false;
}

/*! \internal */
bool OfficeStyle::drawHeader(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    if (const QStyleOptionHeader* header = qstyleoption_cast<const QStyleOptionHeader*>(opt))
    {
        QRegion clipRegion = p->clipRegion();
        p->setClipRect(opt->rect);

        proxy()->drawControl(CE_HeaderSection, header, p, widget);

        QStyleOptionHeader subopt = *header;
        subopt.rect = subElementRect(SE_HeaderLabel, header, widget);

        if (subopt.rect.isValid())
        {
//            QPalette palette = header->palette;
//            QColor clrNormalText = helper().getColor(tr("ListBox"), tr("NormalText"));
//            palette.setColor(QPalette::All, QPalette::ButtonText, clrNormalText);

            QStyleOptionHeader headerLabelOpt = subopt;
//            headerLabelOpt.palette = palette;

            proxy()->drawControl(CE_HeaderLabel, &headerLabelOpt, p, widget);
        }

        if (header->sortIndicator != QStyleOptionHeader::None) 
        {
            subopt.rect = subElementRect(SE_HeaderArrow, opt, widget);
            proxy()->drawPrimitive(PE_IndicatorHeaderArrow, &subopt, p, widget);
        }
        p->setClipRegion(clipRegion);
        return true;
    }
    return false;
}

/*! \internal */
bool OfficeStyle::drawHeaderSection(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    Q_UNUSED(widget);
    bool columnPressed  = opt->state & State_Sunken;
    // [1]
    DrawHelpers::drawGradientFill(*p, opt->rect, columnPressed ? m_palGradientColumnPushed.color(QPalette::Light) : m_palGradientColumn.color(QPalette::Light), 
        columnPressed ? m_palGradientColumnPushed.color(QPalette::Dark) : m_palGradientColumn.color(QPalette::Dark), true);

    p->fillRect(QRect(QPoint(opt->rect.left(), opt->rect.bottom()), QSize(opt->rect.width(), 1)), 
        m_palGradientColumn.color(QPalette::Shadow));
    
//    p->fillRect(QRect(QPoint(opt->rect.left(), opt->rect.top()+1), QSize(1, opt->rect.height() - 4)), m_clrColumnSeparator);
    p->fillRect(QRect(QPoint(opt->rect.right(), opt->rect.top()), QSize(1, opt->rect.height())), m_clrColumnSeparator);

//    p->fillRect(QRect(QPoint(opt->rect.right(), opt->rect.top()), QSize(1, opt->rect.height())), 
//        m_palGradientColumn.color(QPalette::Shadow));

    return true;
}

/*! \internal */
bool OfficeStyle::drawHeaderEmptyArea(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    Q_UNUSED(widget);
    // [1]
    DrawHelpers::drawGradientFill(*p, opt->rect, m_palGradientColumn.color(QPalette::Light), 
        m_palGradientColumn.color(QPalette::Dark), true);

    p->fillRect(QRect(QPoint(opt->rect.left(), opt->rect.bottom()), QSize(opt->rect.width(), 1)), 
        m_palGradientColumn.color(QPalette::Shadow));

    p->fillRect(QRect(QPoint(opt->rect.right(), opt->rect.top()), QSize(1, opt->rect.height())), 
        m_clrColumnSeparator);

    return true;
}

/*! \internal */
bool OfficeStyle::drawIndicatorHeaderArrow(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    Q_UNUSED(widget);

    if (const QStyleOptionHeader* header = qstyleoption_cast<const QStyleOptionHeader*>(opt)) 
    {
        QPoint pt(opt->rect.center());
        if (header->sortIndicator == QStyleOptionHeader::SortUp) 
        {
            DrawHelpers::drawTriangle(*p, QPoint(pt.x() - 4, pt.y() - 2),
                QPoint(pt.x(), pt.y() + 2), QPoint(pt.x() + 4, pt.y()  - 2), m_clrColumnSeparator);
        } 
        else if (header->sortIndicator == QStyleOptionHeader::SortDown) 
        {
            DrawHelpers::drawTriangle(*p, QPoint(pt.x() - 4, pt.y() + 2),
                QPoint(pt.x(), pt.y() - 2), QPoint(pt.x() + 4, pt.y()  + 2), m_clrColumnSeparator);
        }
        return true;
    }
    return false;
}

/*! \internal */
void OfficeStyle::drawDropDownGlyph(QPainter* p, QPoint pt, bool selected, bool popuped, bool enabled, bool vert) const
{
    Q_UNUSED(vert);
    Q_UNUSED(popuped);
    QPixmap soGlyphImage = cached("ToolbarButtonDropDownGlyph.png"); 
    if (soGlyphImage.isNull())
    {
        Q_ASSERT(false);
        return;
    }
    QRect rcSrc = getSource(soGlyphImage.rect(), !enabled ? 3 : selected ? 1 : 0, 4);
    QRect rc(QPoint(pt.x() - 2, pt.y() - 2), QSize(rcSrc.size()));

    drawImage(soGlyphImage, *p, rc, rcSrc, QRect(QPoint(0, 0), QPoint(0, 0)), QColor(0xFF, 0, 0xFF));
}

/*! \internal */
bool OfficeStyle::drawIndicatorArrow(PrimitiveElement pe, const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
//    Q_UNUSED(w);
    switch(pe)
    {
        case PE_IndicatorArrowUp:
        case PE_IndicatorArrowDown:
        case PE_IndicatorArrowRight:
        case PE_IndicatorArrowLeft:
            {
                if (pe == PE_IndicatorArrowRight && qstyleoption_cast<const QStyleOptionMenuItem*>(opt))
                {
                    QPixmap soIndicator = cached("MenuPopupGlyph.png");
                    QRect rcSrc = getSource(soIndicator.rect(), opt->state & State_Enabled ? 0 : 1, 2);
                    QRect rcEntry(QPoint(opt->rect.topLeft()), rcSrc.size());
                    drawImage(soIndicator, *p, rcEntry, rcSrc, QRect(QPoint(0, 0), QPoint(0, 0)), QColor(0xFF, 0, 0xFF));
                    return true;
                }
                else if (pe == PE_IndicatorArrowDown)
                {
                    if (qobject_cast<const QToolButton*>(w))
                    {
                        // revision: 764
                        if (qobject_cast<QToolBar*>(w->parentWidget()) &&  !qobject_cast<const QTabBar*>(w->parentWidget()))
                        {
                            QPixmap soIndicator = cached("ToolbarButtonDropDownGlyph.png");
                            bool enabled = opt->state & State_Enabled;    
                            bool selected = opt->state & State_Selected;
                            QRect rcSrc = getSource(soIndicator.rect(), !enabled ? 3 : selected ? 1 : 0, 4);
                            QRect rcEntry(QPoint(opt->rect.topLeft()), rcSrc.size());
                            drawImage(soIndicator, *p, rcEntry, rcSrc, QRect(QPoint(0, 0), QPoint(0, 0)), QColor(0xFF, 0, 0xFF));
                            return true;
                        }
                    }
                }

                bool enabled = opt->state & State_Enabled;    
                bool selected = opt->state & State_Selected;
                QColor color = getTextColor(selected, false, enabled, false, false/*popuped*/, TypeNormal, BarTop);

                QPalette palette = opt->palette;
                palette.setColor(QPalette::ButtonText, color);

                if (opt->rect.width() <= 1 || opt->rect.height() <= 1)
                    break;

                QRect r = opt->rect;
                int size = qMin(r.height(), r.width());

                int border = (size/2) - 3;

                int sqsize = 2*(size/2);
                QImage image(sqsize, sqsize, QImage::Format_ARGB32_Premultiplied);
                image.fill(0);
                QPainter imagePainter(&image);

                QPolygon a;
                switch (pe) 
                {
                    case PE_IndicatorArrowUp:
                        a.setPoints(3, border, sqsize/2,  sqsize/2, border,  sqsize - border, sqsize/2);
                        break;
                    case PE_IndicatorArrowDown:
                        a.setPoints(3, border, sqsize/2,  sqsize/2, sqsize - border,  sqsize - border, sqsize/2);
                        break;
                    case PE_IndicatorArrowRight:
                        a.setPoints(3, sqsize - border, sqsize/2,  sqsize/2, border,  sqsize/2, sqsize - border);
                        break;
                    case PE_IndicatorArrowLeft:
                        a.setPoints(3, border, sqsize/2,  sqsize/2, border,  sqsize/2, sqsize - border);
                        break;
                    default:
                        break;
                }

                QRect bounds = a.boundingRect();
                int sx = sqsize / 2 - bounds.center().x() - 1;
                int sy = sqsize / 2 - bounds.center().y() - 1;
                imagePainter.translate(sx, sy);
                imagePainter.setPen(palette.buttonText().color());
                imagePainter.setBrush(palette.buttonText());


                if (!enabled) 
                {
                    imagePainter.translate(1, 1);
                    imagePainter.setBrush(palette.light().color());
                    imagePainter.setPen(palette.light().color());
                    imagePainter.drawPolygon(a);
                    imagePainter.translate(-1, -1);
                    imagePainter.setBrush(palette.mid().color());
                    imagePainter.setPen(palette.mid().color());
                }

                imagePainter.drawPolygon(a);
                imagePainter.end();

                QPixmap pixmap = QPixmap::fromImage(image);

                int xOffset = r.x() + (r.width() - size)/2;
                int yOffset = r.y() + (r.height() - size)/2;

//                QRect rc(QPoint(xOffset, yOffset), pixmap.size());
//                drawImage(pixmap, *p, rc, pixmap.rect(), QRect(QPoint(0, 0), QPoint(0, 0)));
                p->drawPixmap(xOffset, yOffset, pixmap);
                return true;
            }
            break;
        default:
            return false;
    }
    return false;
}

/*! \internal */
bool OfficeStyle::drawPanelTipLabel(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);

    QColor clrLight = m_clrTooltipLight;
    QColor clrDark = m_clrTooltipDark;
    if (!clrLight.isValid()) 
        clrLight = QColor(255, 255, 255);
    if (!clrDark.isValid()) 
        clrDark = QColor(201, 217, 239);
    // [1]
    DrawHelpers::drawGradientFill(*p, opt->rect, clrLight, clrDark, true);

    QPixmap soImage = cached("TooltipFrame.png");
    if (!soImage.isNull())
    {
        drawImage(soImage, *p, opt->rect, getSource(soImage.rect(), 0, 1), QRect(QPoint(3, 3), QPoint(3, 3)), QColor(0xFF, 0, 0xFF));
    }
    else
    {
        const QPen oldPen = p->pen();
        p->setPen(m_clrTooltipBorder);
        p->drawRect(opt->rect.adjusted(0, 0, -1, -1));
        p->setPen(oldPen);
    }

    return true; 
}

/*! \internal */
void OfficeStyle::drawRectangle(QPainter* p, const QRect& rect, bool selected, bool pressed, bool enabled, bool checked, bool popuped,
                                    BarType barType, BarPosition barPos) const
{
    if (popuped)
    {
        // [1]
        DrawHelpers::drawGradientFill(*p, rect, m_clrPopupControl.color(QPalette::Light), m_clrPopupControl.color(QPalette::Dark),  true);
        DrawHelpers::draw3DRect(*p, m_clrMenubarBorder, m_clrMenubarBorder, rect.x(), rect.y(), rect.width()-1, rect.height()-1, true);
    }
    else if (barType != TypePopup)
    {
        if (!enabled)
        {
            if (/*isKeyboardSelected(selected)*/false)
            {
                // [1]
                DrawHelpers::drawGradientFill(*p, rect, checked ? m_palLunaPushed.color(QPalette::Light) : m_palLunaSelected.color(QPalette::Light), 
                    checked ? m_palLunaPushed.color(QPalette::Dark) : m_palLunaSelected.color(QPalette::Dark), true);
                DrawHelpers::draw3DRect(*p, m_clrHighlightPushedBorder, m_clrHighlightPushedBorder, rect.x(), rect.y(), rect.width()-1, rect.height()-1, true);
            }
            else if (checked)
            {
                // [1]
                DrawHelpers::drawGradientFill(*p, rect, m_palLunaChecked.color(QPalette::Light), m_palLunaChecked.color(QPalette::Dark), true);
                DrawHelpers::draw3DRect(*p, m_clrHighlightDisabledBorder, m_clrHighlightDisabledBorder, rect.x(), rect.y(), rect.width()-1, rect.height()-1, true);
            }
            return;
        }
//        else if (checked && !selected && !pressed)
//        {
//            DrawHelpers::drawRectangle(*p, rect.adjusted(0, 0, -1, -1), m_clrHighlightPushedBorder, m_clr3DFace);
//        }
        else if (checked && !selected && !pressed) 
        {
            // [1]
            DrawHelpers::drawGradientFill(*p, rect, m_palLunaChecked.color(QPalette::Light), m_palLunaChecked.color(QPalette::Dark), true);
            DrawHelpers::draw3DRect(*p, m_clrHighlightCheckedBorder, m_clrHighlightCheckedBorder, rect.x(), rect.y(), rect.width()-1, rect.height()-1, true);
        }
        else if (checked && selected && !pressed) 
        {
            // [1]
            DrawHelpers::drawGradientFill(*p, rect, m_palLunaPushed.color(QPalette::Light), m_palLunaPushed.color(QPalette::Dark),  true);
            DrawHelpers::draw3DRect(*p, m_clrHighlightPushedBorder, m_clrHighlightPushedBorder, rect.x(), rect.y(), rect.width()-1, rect.height()-1, true);
        }
        else if (/*isKeyboardSelected(pressed) ||*/ (selected && pressed)) 
        {
            // [1]
            DrawHelpers::drawGradientFill(*p, rect, m_palLunaPushed.color(QPalette::Light), m_palLunaPushed.color(QPalette::Dark), true);
            DrawHelpers::draw3DRect(*p, m_clrHighlightPushedBorder, m_clrHighlightPushedBorder, rect.x(), rect.y(), rect.width()-1, rect.height()-1, true);
        }
        else if (selected || pressed) 
        {
            // [1]
            DrawHelpers::drawGradientFill(*p, rect, m_palLunaSelected.color(QPalette::Light), m_palLunaSelected.color(QPalette::Dark), true);
            DrawHelpers::draw3DRect(*p, m_clrHighlightBorder, m_clrHighlightBorder, rect.x(), rect.y(), rect.width()-1, rect.height()-1, true);
        }
    }
    else //if (barPos == BarPopup && selected && barType == TypePopup)
    {
        QRect rc = rect;
        rc.adjust(0, 0, -1, -1);
        if (!enabled)
        {
            if (/*isKeyboardSelected(bSelected)*/false)
                DrawHelpers::drawRectangle(*p, rc, m_clrHighlightBorder, barPos != BarPopup ? (checked ? m_clrHighlightPushed : m_clrHighlight) : m_clrMenubarFace);
            else if (checked)
                DrawHelpers::drawRectangle(*p, rc, m_clrHighlightDisabledBorder, barPos != BarPopup ? m_clrHighlightChecked : QColor());
            return;
        }
        if (popuped) 
            DrawHelpers::drawRectangle(*p, rc, m_clrMenubarBorder, m_clrToolbarFace);
//        else if (checked && !selected && !pressed) 
//            DrawHelpers::drawRectangle(*p, rc, m_clrHighlightCheckedBorder, m_clr3DFace);
        else if (checked && !selected && !pressed) 
            DrawHelpers::drawRectangle(*p, rc, m_clrHighlightCheckedBorder, m_clrHighlightChecked);
        else if (checked && selected && !pressed) 
            DrawHelpers::drawRectangle(*p, rc, m_clrHighlightPushedBorder, m_clrHighlightPushed);
        else if (/*isKeyboardSelected(bPressed) ||*/ (selected && pressed)) 
            DrawHelpers::drawRectangle(*p, rc, m_clrHighlightPushedBorder, m_clrHighlightPushed);
        else if (selected || pressed) 
            DrawHelpers::drawRectangle(*p, rc, m_clrHighlightBorder, m_clrHighlight);
    }
}

/*! \internal */
void OfficeStyle::drawSplitButtonPopup(QPainter* p, const QRect& rect, bool selected, bool enabled, bool popuped) const
{
    Q_UNUSED(p);
    Q_UNUSED(rect);
    Q_UNUSED(selected);
    Q_UNUSED(enabled);
    Q_UNUSED(popuped);
}

/*! \internal */
void OfficeStyle::drawLabelGallery(QPainter* p, RibbonGalleryItem* item, const QRect& rect)
{
    QRect rectItem = rect;
    p->fillRect(rect, m_clrControlGalleryLabel);
    p->fillRect(rectItem.left(), rectItem.bottom() - 1, rectItem.width(), 1, QColor(197, 197, 197));

    int alignment = 0;
    alignment |= Qt::TextSingleLine | Qt::AlignVCenter;

    QPalette palette;
    palette.setColor(QPalette::WindowText, m_clrMenuPopupText);
    rectItem.adjust(10, 0, 0, 0);
    const QFont& saveFont = p->font();
    QFont font(saveFont);
    font.setBold(true);
    p->setFont(font);
    proxy()->drawItemText(p, rectItem, alignment, palette, true, item->caption(), QPalette::WindowText);
    p->setFont(saveFont);
}

/*! \internal */
QColor OfficeStyle::getTextColor(bool selected, bool pressed, bool enabled, bool checked, bool popuped, BarType barType, BarPosition barPosition) const
{
    Q_UNUSED(barPosition);
    if (barType == TypeMenuBar && !selected && enabled && !pressed && !checked && !popuped)
        return m_clrMenuBarText;

    if (barType == TypePopup)
        return enabled ? m_clrMenuPopupText : m_clrMenuBarGrayText;
//        return enabled ? m_clrMenuBarText : m_clrMenuBarGrayText;

    return enabled ? m_clrToolBarText : m_clrToolBarGrayText;
}

/*! \reimp */
bool OfficeStyle::eventFilter(QObject* watched, QEvent* event)
{
    return CommonStyle::eventFilter(watched, event);
}

/*! \internal */
QWidgetList OfficeStyle::getAllWidgets() const
{
    return qApp->allWidgets();
}

/*! \internal */
bool OfficeStyle::isNativeDialog(const QWidget* wid) const
{
    return ::isParentDialog(wid) && isDialogsIgnored();
}

/*!
    \class Qtitan::OfficeStylePlugin
    \internal
*/
QStringList OfficeStylePlugin::keys() const
{
    return QStringList() << "OfficeStyle";
}

QStyle* OfficeStylePlugin::create( const QString& key )
{
    if ( key.toLower() == QLatin1String("officestyle"))
        return new OfficeStyle();
    return Q_NULL;
}

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#else
QObject* qt_plugin_instance_officestyle()
{
    static QObject* instance = Q_NULL;
    if ( !instance )
        instance = new OfficeStylePlugin();
    return instance;
}

Q_IMPORT_PLUGIN(officestyle)
#endif
