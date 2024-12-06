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

QX_RIBBON_BEGIN_NAMESPACE

class RibbonFactory;
class RibbonBarPrivate;
class RibbonButtonGroup;
class RibbonQuickAccessBar;

/**
 * @brief RibbonBar 继承 QMenuBar, 在 RibbonWindow 中直接替换了原来的 QMenuBar
 *
 * 通过 setRibbonStyle 函数设置 ribbon 的风格:
 *
 * @code
 * void setRibbonStyle(RibbonStyle v);
 * @endcode
 *
 * RibbonBar 参考 office 和 wps，提供了四种风格的 Ribbon 模式, @ref RibbonBar::RibbonStyle
 *
 * 如果想 ribbon 占用的空间足够小，WpsLiteStyleTwoRow 模式能比 OfficeStyle 节省 35% 的高度空间
 *
 * 如何生成 ribbon? 先看看一个传统的 MenuBar/ToolBar 是如何生成的：
 *
 * @code
 * void MainWindow::MainWindow()
 * {
 *    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
 *    QToolBar *fileToolBar = addToolBar(tr("File"));
 *    // 生成action
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
 * 传统的 MenuBar/ToolBar 主要通过 QMenuBar 的 addMenu 添加菜单, 通过 QMainWindow::addToolBar 生成 QToolBar,
 * 再把 QAction 设置进 QMenu 和 QToolBar 中
 *
 * RibbonBar 和传统方法相似，不过相对于传统的 MenuBar/ToolBar, QMenuBar 和 QToolBar 是平级的，
 * Ribbon 是有明显的层级关系，RibbonBar 下面是 @ref RibbonPage，
 * RibbonPage 下面是 @ref RibbonGroup ，RibbonGroup下 面是 @ref RibbonButton ，
 * RibbonButton 管理着 QAction
 *
 * 因此，生成一个 ribbon 只需以下几个函数：
 * @code
 * RibbonPage *RibbonBar::addPage(const QString &title);
 * RibbonGroup *RibbonPage::addGroup(const QString &title);
 * RibbonButton *RibbonGroup::addLargeAction(QAction *action);
 * RibbonButton *RibbonGroup::addSmallAction(QAction *action);
 * @endcode
 *
 * 因此生成步骤如下：
 *
 * @code
 * // 成员变量
 * RibbonPage *pageMain;
 * RibbonGroup *fileGroup;
 *
 * // 建立ui
 * void setupRibbonUi()
 * {
 *     ......
 *     // ribbonwindow 为 RibbonWindow
 *     RibbonBar *ribbon = ribbonwindow->ribbonBar();
 *     ribbon->setRibbonStyle(RibbonBar::WpsLiteStyle);
 *     // 添加一个 Main 标签页
 *     pageMain = ribbon->addPage(QStringLiteral("Main"));
 *     // Main 标签页下添加一个 File Group
 *     fileGroup = pageMain->addGroup(QStringLiteral("FileGroup"));
 *     // 开始为 File Group 添加 action
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
    Q_PROPERTY(QuickAccessBarPosition quickAccessBarPosition READ quickAccessBarPosition WRITE setQuickAccessBarPosition)
    Q_PROPERTY(bool minimized READ isMinimized WRITE setMinimized)
    Q_PROPERTY(bool minimumButton READ haveShowMinimumButton WRITE showMinimumButton)
    Q_PROPERTY(QColor tabBarBaseLineColor READ tabBarBaseLineColor WRITE setTabBarBaseLineColor)
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
#if QX_RIBBON_DEPRECATED_SINCE(0, 9)
    QT_DEPRECATED_X("Use cornerButtonGroup")
    RibbonButtonGroup *rightButtonGroup();
    void activeRightButtonGroup();
#endif

    RibbonQuickAccessBar *quickAccessBar() const;
    void setQuickAccessBarPosition(QuickAccessBarPosition position);
    QuickAccessBarPosition quickAccessBarPosition() const;

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

    QColor tabBarBaseLineColor() const;
    void setTabBarBaseLineColor(const QColor &clr);

    Qt::Alignment windowTitleAligment() const;
    void setWindowTitleAligment(Qt::Alignment al);

    void setWindowTitleVisible(bool visible);

signals:
    void minimizationChanged(bool minimized);
    void applicationButtonClicked();
    void currentRibbonTabChanged(int index);
    void ribbonStyleChanged(RibbonStyle nowStyle);
protected:
    bool event(QEvent *event) Q_DECL_OVERRIDE;
    bool eventFilter(QObject *obj, QEvent *e) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *e) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *e) Q_DECL_OVERRIDE;
private:
    QX_DECLARE_PRIVATE(RibbonBar)
};

QX_RIBBON_END_NAMESPACE
