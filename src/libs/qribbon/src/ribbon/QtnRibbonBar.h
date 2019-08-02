/****************************************************************************
**
** Qtitan Library by Developer Machines (Microsoft-Ribbon implementation for Qt.C++)
** 
** Copyright (c) 2009-2013 Developer Machines (http://www.devmachines.com)
** Copyright (c) 2019 MaMinJie <canpool@163.com>
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
#ifndef QTN_RIBBONBAR_H
#define QTN_RIBBONBAR_H

#include <QMenuBar>

#include "QtitanDef.h"

class QMenu;
class QToolButton;

QTITAN_BEGIN_NAMESPACE

class RibbonPage;
class RibbonBarPrivate;
class RibbonSystemButton;
class RibbonQuickAccessBar;
/* RibbonBar */
class QTITAN_EXPORT RibbonBar : public QMenuBar
{
    Q_OBJECT
    Q_PROPERTY(int currentIndexPage READ currentIndexPage WRITE setCurrentPage NOTIFY currentPageChanged)
    Q_PROPERTY(bool frameThemeEnabled READ isFrameThemeEnabled WRITE setFrameThemeEnabled)
    Q_PROPERTY(bool minimized READ isMinimized WRITE setMinimized NOTIFY minimizationChanged)
    Q_PROPERTY(bool maximized READ isMaximized WRITE setMaximized NOTIFY minimizationChanged)
public:
    RibbonBar(QWidget* parent = Q_NULL);
    virtual ~RibbonBar();

public:
    bool isVisible() const;

public:
    RibbonPage* addPage(const QString& text);
    void addPage(RibbonPage* page);

    RibbonPage* insertPage(int index, const QString& text);
    void insertPage(int index, RibbonPage* page);

    void movePage(RibbonPage* page, int newIndex);
    void movePage(int index, int newIndex);

    void removePage(RibbonPage* page);
    void removePage(int index);

    void detachPage(RibbonPage* page);
    void detachPage(int index);

    void clearPages();

    bool isKeyTipsShowing() const;
    bool keyTipsEnabled() const;
    void setKeyTipsEnable(bool enable);
    bool isKeyTipsComplement() const;
    void setKeyTipsComplement(bool complement);
    void setKeyTip(QAction* action, const QString& keyTip);

    RibbonQuickAccessBar* getQuickAccessBar() const;
    void showQuickAccess(bool show = true);
    bool isQuickAccessVisible() const;

    void minimize();
    bool isMinimized() const;
    void setMinimized(bool flag);

    void maximize();
    bool isMaximized() const;
    void setMaximized(bool flag);

    void setMinimizationEnabled(bool enabled);
    bool isMinimizationEnabled() const;

public Q_SLOTS:
    void setCurrentPage(int index);

public:
    int currentIndexPage() const;
    RibbonPage* getPage(int index) const;
    int getPageCount() const;

public:
    QMenu* addMenu(const QString& text);
    QAction* addAction(const QIcon& icon, const QString& text, Qt::ToolButtonStyle style, QMenu* menu = Q_NULL);
    QAction* addAction(QAction* action, Qt::ToolButtonStyle style);

    QAction* addSystemButton(const QString& text);
    QAction* addSystemButton(const QIcon& icon, const QString& text);

    RibbonSystemButton* getSystemButton() const;

    bool isBackstageVisible() const;

    void setFrameThemeEnabled(bool enable = true);
    bool isFrameThemeEnabled() const;

    bool isTitleBarVisible() const;
    void setTitleBarVisible(bool show);

    int titleBarHeight() const;
    int topBorder() const;

private Q_SLOTS:
    void currentChanged(int index);

Q_SIGNALS:
    void minimizationChanged(bool minimized);
    void currentPageChanged(int index);
    void currentPageChanged(RibbonPage* page);
    void keyTipsShowed(bool showed);

protected:
    int tabBarHeight() const;
    void setMinimizedFlag(bool flag);

public:
    virtual int heightForWidth(int) const;
    virtual QSize sizeHint() const;

protected:
    virtual bool event(QEvent* event);
    virtual bool eventFilter(QObject* object, QEvent* event);
    virtual void paintEvent(QPaintEvent* p);
    virtual void changeEvent(QEvent* event);
    virtual void resizeEvent(QResizeEvent* event);

    virtual void mouseDoubleClickEvent(QMouseEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void wheelEvent(QWheelEvent* event);

#ifdef Q_OS_WIN
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    virtual bool nativeEvent(const QByteArray& eventType, void* message, long* result);
#else
    virtual bool winEvent(MSG* message, long* result);
#endif
#endif // Q_OS_WIN
private:
    friend class RibbonSystemPopupBar;
    friend class RibbonBackstageViewPrivate;
    friend class RibbonBackstageView;
    friend class RibbonStyle;
    #ifdef Q_OS_WIN
    friend class OfficeFrameHelperWin;
    #endif // Q_OS_WIN
    QTN_DECLARE_PRIVATE(RibbonBar)
    Q_DISABLE_COPY(RibbonBar)
};

QTITAN_END_NAMESPACE

#endif // QTN_RIBBONBAR_H


