/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#pragma once

#include "qxribbon_global.h"
#include "ribbonpage.h"
#include "ribbonpagecontext.h"

#include <QMenuBar>
#include <QScopedPointer>
#include <QVariant>

class QAbstractButton;
class QContextMenuEvent;
class QMenu;

QX_RIBBON_BEGIN_NAMESPACE

class RibbonFactory;
class RibbonBarPrivate;
class RibbonButtonGroup;
class RibbonQuickAccessBar;

/**
 * @brief RibbonBar inherits QMenuBar and directly replaces the original QMenuBar in RibbonMainWindow
 *
 * Set the ribbon style via setRibbonStyle:
 *
 * @code
 * void setRibbonStyle(RibbonStyle v);
 * @endcode
 *
 * RibbonBar follows office and wps and provides four ribbon style modes, @ref RibbonBar::RibbonStyle
 *
 * If the ribbon needs to be compact, WpsLiteStyleTwoRow saves 35% of the height used by OfficeStyle
 *
 * How to build a ribbon? First look at how a traditional MenuBar/ToolBar is built:
 *
 * @code
 * void MainWindow::MainWindow()
 * {
 *    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
 *    QToolBar *fileToolBar = addToolBar(tr("File"));
 *    // create the action
 *    QAction *newAct = new QAction(newIcon, tr("&New"), this);
 *    fileMenu->addAction(newAct);
 *    fileToolBar->addAction(newAct);
 *
 *    QAction *openAct = new QAction(openIcon, tr("&Open..."), this);
 *    fileMenu->addAction(openAct);
 *    fileToolBar->addAction(openAct);
 * }
 * @endcode
 *
 * A traditional MenuBar/ToolBar adds menus via QMenuBar::addMenu, creates a QToolBar via QMainWindow::addToolBar,
 * and then puts QActions into the QMenu and QToolBar
 *
 * RibbonBar is similar, but whereas QMenuBar and QToolBar are peers in a traditional MenuBar/ToolBar,
 * a Ribbon has a clear hierarchy: below RibbonBar is @ref RibbonPage,
 * below RibbonPage is @ref RibbonGroup, below RibbonGroup is @ref RibbonButton,
 * and RibbonButton manages the QAction
 *
 * Therefore only the following functions are needed to build a ribbon:
 * @code
 * RibbonPage *RibbonBar::addPage(const QString &title);
 * RibbonGroup *RibbonPage::addGroup(const QString &title);
 * RibbonButton *RibbonGroup::addLargeAction(QAction *action);
 * RibbonButton *RibbonGroup::addSmallAction(QAction *action);
 * @endcode
 *
 * The steps are therefore:
 *
 * @code
 * // member variables
 * RibbonPage *pageMain;
 * RibbonGroup *fileGroup;
 *
 * // build the ui
 * void setupRibbonUi()
 * {
 *     ......
 *     // ribbonwindow is a RibbonMainWindow
 *     RibbonBar *ribbon = ribbonwindow->ribbonBar();
 *     ribbon->setRibbonStyle(RibbonBar::WpsLiteStyle);
 *     // add a Main tab
 *     pageMain = ribbon->addPage(QStringLiteral("Main"));
 *     // add a File Group under the Main tab
 *     fileGroup = pageMain->addGroup(QStringLiteral("FileGroup"));
 *     // start adding actions to the File Group
 *     fileGroup->addLargeAction(actionNew);
 *     fileGroup->addLargeAction(actionOpen);
 *     fileGroup->addLargeAction(actionSave);
 *     fileGroup->addSmallAction(actionImportMesh);
 *     fileGroup->addSmallAction(actionImportGeometry);
 * }
 * @endcode
 */
class QX_RIBBON_EXPORT RibbonBar : public QMenuBar
{
    Q_OBJECT
    Q_PROPERTY(RibbonStyle ribbonStyle READ currentRibbonStyle WRITE setRibbonStyle)
    Q_PROPERTY(
        QuickAccessBarPosition quickAccessBarPosition READ quickAccessBarPosition WRITE setQuickAccessBarPosition)
    Q_PROPERTY(bool minimized READ isMinimized WRITE setMinimized)
    Q_PROPERTY(bool minimumButton READ haveShowMinimumButton WRITE showMinimumButton)
    Q_PROPERTY(Qt::Alignment windowTitleAligment READ windowTitleAligment WRITE setWindowTitleAligment)
public:
    enum RibbonStyle {
        OfficeStyle = 0x0000,
        WpsLiteStyle = 0x0001,
        OfficeStyleTwoRow = 0x0100 | OfficeStyle,
        WpsLiteStyleTwoRow = 0x0100 | WpsLiteStyle,
    };
    Q_ENUM(RibbonStyle)

    enum QuickAccessBarPosition {
        // Only for WpsLiteStyle
        QABLeftPosition = 1,
        QABRightPosition
    };
    Q_ENUM(QuickAccessBarPosition)

    enum TabBarPosition {
        TBLeftPosition,
        TBCenterPosition
    };
    Q_ENUM(TabBarPosition)
public:
    RibbonBar(QWidget *parent = Q_NULLPTR);
    ~RibbonBar() Q_DECL_OVERRIDE;

    QAbstractButton *applicationButton();
    void setApplicationButton(QAbstractButton *btn);

    RibbonPage *addPage(const QString &title);
    void addPage(RibbonPage *page);
    RibbonPage *insertPage(int index, const QString &title);
    void insertPage(int index, RibbonPage *page);

    RibbonPage *page(int index) const;
    RibbonPage *pageByName(const QString &title) const;
    RibbonPage *pageByObjectName(const QString &objname) const;
    QList<RibbonPage *> pages(bool getAll = true) const;

    bool isPageVisible(RibbonPage *page) const;
    void showPage(RibbonPage *page);
    void hidePage(RibbonPage *page);
    void raisePage(RibbonPage *page);

    void removePage(RibbonPage *page);
    void clearPages();
    void movePage(int from, int to);

    int pageIndex(RibbonPage *page) const;

    RibbonPageContext *addPageContext(const QString &title, const QColor &color = QColor());
    void addPageContext(RibbonPageContext *context);

    void showPageContext(RibbonPageContext *context);
    void hidePageContext(RibbonPageContext *context);

    bool isPageContextVisible(RibbonPageContext *context);
    void setPageContextVisible(RibbonPageContext *context, bool visible);

    bool isPageContextCoverTab();
    void setPageContextCoverTab(bool cover);

    QList<RibbonPageContext *> pageContextList() const;

    void destroyPageContext(RibbonPageContext *context);

    void minimize();
    bool isMinimized() const;
    void setMinimized(bool flag);

    bool haveShowMinimumButton() const;
    void showMinimumButton(bool isShow = true);

    int tabBarHeight() const;
    int titleBarHeight() const;

    RibbonButtonGroup *cornerButtonGroup(Qt::Corner corner = Qt::BottomRightCorner);

    RibbonQuickAccessBar *quickAccessBar() const;
    void setQuickAccessBarPosition(QuickAccessBarPosition position);
    QuickAccessBarPosition quickAccessBarPosition() const;

    void setTabBarPosition(TabBarPosition position);
    TabBarPosition tabBarPosition() const;

    RibbonStyle currentRibbonStyle() const;
    void setRibbonStyle(RibbonStyle v);

    int currentIndex();
    void setCurrentIndex(int index);

    bool isOfficeStyle() const;
    bool isTwoRowStyle() const;

    void setWindowButtonGroup(QWidget *widget);

    void updateRibbonGeometry();
    void updateRibbonTheme();
    void resizeRibbon();
    /*!
     * The same layout work resizeRibbon() asks for, done before this returns
     * instead of on the next event loop pass.
     *
     * resizeRibbon() defers on purpose and that deferral costs one frame of stale
     * geometry - see doc/pages/limitations.md, "RibbonBar resize is deferred".
     * A caller that has just changed something the user is already looking at (a
     * style, the window button group) and needs the new geometry now asks here.
     *
     * The reason the work is deferred is the reason to be careful with this: it
     * reads widget state that a style switch updates halfway through, so a style
     * switch is the caller that has to keep using resizeRibbon().
     */
    void resizeRibbonNow();

    Qt::Alignment windowTitleAligment() const;
    void setWindowTitleAligment(Qt::Alignment al);

    void setWindowTitleVisible(bool visible);

    QSize sizeHint() const Q_DECL_OVERRIDE;
    QSize minimumSizeHint() const Q_DECL_OVERRIDE;

Q_SIGNALS:
    void minimizationChanged(bool minimized);
    void applicationButtonClicked();
    void currentRibbonTabChanged(int index);
    void ribbonStyleChanged(RibbonStyle nowStyle);
protected:
    bool event(QEvent *event) Q_DECL_OVERRIDE;
    bool eventFilter(QObject *obj, QEvent *e) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *e) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *e) Q_DECL_OVERRIDE;
    void contextMenuEvent(QContextMenuEvent *event) Q_DECL_OVERRIDE;
    /// Fills @a menu with the ribbon's standard context entries: minimizing the ribbon
    /// and customizing the quick access bar. Override to add entries; call the base
    /// implementation first (B10).
    virtual void addContextMenuActions(QMenu *menu);
private:
    QX_DECLARE_PRIVATE(RibbonBar)
};

QX_RIBBON_END_NAMESPACE
