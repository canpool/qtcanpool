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
#include <QApplication>
#include <QDesktopWidget>
#include <QMainWindow>
#include <QStyleOption>
#include <QPainter>
#include <QCheckBox>
#include <QRadioButton>
#include <QComboBox>
#include <QLineEdit>
#include <QWidgetAction>
#include <qevent.h>

#include "QtnRibbonBar.h"
#include "QtnRibbonPrivate.h"
#include "QtnStyleHelpers.h"
#include "QtnRibbonButton.h"
#include "QtnRibbonStyle.h"
#include "QtnRibbonBarPrivate.h"
#include "QtnOfficeFrameHelper.h"
#include "QtnRibbonQuickAccessBar.h"
#include "QtnRibbonSystemPopupBar.h"
#include "QtnRibbonBackstageView.h"
#include "QtnRibbonToolTip.h"
#include "QtnRibbonGroup.h"
#include "QtnRibbonGallery.h"


#ifdef Q_OS_WIN
#include "QtnOfficeFrameHelper_win.h"
#endif // Q_OS_WIN

QTITAN_USE_NAMESPACE

/*!
\property Qtitan::RibbonBar::currentIndexPage
*/
/*!
\property Qtitan::RibbonBar::minimized
*/
/*!
\property Qtitan::RibbonBar::maximized
*/

/*!
\class Qtitan::RibbonBarPrivate
\internal
*/
RibbonBarPrivate::RibbonBarPrivate()
{
    m_initFlag = false;
    m_tabBar = Q_NULL;
    m_quickAccessBar = Q_NULL;
    m_controlSystemButton = Q_NULL;
    m_currentIndexPage = -1;
    m_frameHelper = Q_NULL;
    m_showTitleAlways = true;
    m_quickAccessOnTop = true;
    m_ribbonBarVisible = true;
    m_minimizationEnabled = true;
    m_keyTipsComplement = true;
    m_keyTipsEnabled = true;
    m_keyTipsShowing = false;
    m_minimized = false;
    m_hoveredSubControl = QStyle::SC_None;
    m_activeSubControl = QStyle::SC_None;
    m_countKey = 0;
    m_wheelEvent = false;
}

RibbonBarPrivate::~RibbonBarPrivate()
{
    removeAllContextHeaders();
}

void RibbonBarPrivate::init()
{
    QTN_P(RibbonBar);

    p.setNativeMenuBar(false);

    p.setAttribute(Qt::WA_Hover, true);
    p.setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum); 
    p.ensurePolished();

    m_tabBar = new RibbonTabBar(&p);
    m_tabBar->show();
    QObject::connect(m_tabBar, SIGNAL(currentChanged(int)), &p, SLOT(currentChanged(int)), Qt::DirectConnection);

    m_quickAccessBar = new RibbonQuickAccessBar(&p);
    m_quickAccessBar->setVisible(false);
}

void RibbonBarPrivate::layoutRibbon()
{
    QTN_P(RibbonBar);

    /* In MacOSX the font has been selected in RibbonStyle::polish(), so we have non valid reference to the tabbar. */
    if (m_tabBar == Q_NULL || m_quickAccessBar == Q_NULL)
        return;

    QWidget* parent = p.parentWidget();
    bool updates = parent ? parent->updatesEnabled() : true;
    if (parent && updates)
        parent->setUpdatesEnabled(false);

    QStyleOption opt;
    opt.init(&p);

    bool saveRibbonBarVisible = m_ribbonBarVisible;
    m_ribbonBarVisible = true;

    const int vmargin = p.style()->pixelMetric(QStyle::PM_MenuBarVMargin, &opt, &p);
//    const int heightGroup = p.style()->pixelMetric((QStyle::PixelMetric)RibbonStyle::PM_RibbonHeightGroup, &opt, 0);
    int heightGroup = getMaxHeightPages();
//    const int heightCaptionGroup = p.style()->pixelMetric((QStyle::PixelMetric)RibbonStyle::PM_RibbonHeightCaptionGroup, &opt, 0);
    const int tabsHeight = m_tabBar->sizeHint().height();
    const int pageAreaHeight = heightGroup/* + heightCaptionGroup*/;

    int quickAccessHeight = m_quickAccessBar->sizeHint().height();

    bool frameThemeEnabled = p.isFrameThemeEnabled();
    bool showTitle = frameThemeEnabled || m_showTitleAlways;

    int frameBorder = 4;

    QRect rcTabBar(QPoint(2, 0), QSize(opt.rect.width()-2, tabsHeight));
    QRect rcPageArea(QPoint(0, tabsHeight), QSize(opt.rect.width(), pageAreaHeight + vmargin * 2 + 2 + 1/* * 4*/ /*+ vmargin + 1*/));

    if (showTitle)
    {
        frameBorder = m_frameHelper ? m_frameHelper->getFrameBorder() : 0;
        int ribbonTopBorder = p.topBorder();
        int titleOffset = ribbonTopBorder - frameBorder;

        m_rcTitle = QRect(QPoint(-frameBorder, titleOffset), 
            QPoint(opt.rect.width() + frameBorder, p.titleBarHeight() + ribbonTopBorder));

        rcTabBar.translate(0, m_rcTitle.height() + titleOffset);
        rcPageArea.translate(0, m_rcTitle.height() + titleOffset);

        m_rcHeader = QRect(0, 0, opt.rect.width(), m_rcTitle.bottom());
        m_rcTitleText  = QRect(0, 0, opt.rect.width(), m_rcTitle.bottom());

        if (!m_frameHelper)
            m_rcTitleText = QRect();
    }
    else
    {
//        if (!showTitle && m_controlSystemButton)
//        {
//            if (m_controlSystemButton->toolButtonStyle() != Qt::ToolButtonTextOnly)
//                m_controlSystemButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
//        }

        m_rcTitle = QRect();
        m_rcHeader = QRect();
        m_rcTitleText = QRect();
    }

    if (m_controlSystemButton && m_controlSystemButton->toolButtonStyle() == Qt::ToolButtonTextBesideIcon)
        m_controlSystemButton->setToolButtonStyle(Qt::ToolButtonTextOnly);

    int corner = quickAccessHeight + tabsHeight - 3;

    if (!m_controlSystemButton || m_controlSystemButton->toolButtonStyle() != Qt::ToolButtonFollowStyle)
    {
        StyleOptionTitleBar titleBarOpt;
        titleBarOptions(titleBarOpt);

        QSize szIcon = m_frameHelper ? m_frameHelper->getSizeSystemIcon() : QSize(16, 16);
//        QSize szIcon = titleBarOpt.icon.actualSize(QSize(64, 64));
        corner = 10 + (!titleBarOpt.icon.isNull() ? /*titleBarOpt.icon.actualSize(QSize(16, 16)).width()*/szIcon.width() : 0);
    }

    QSize szBar = m_quickAccessBar->sizeHint();
    if (m_quickAccessBar->isVisible() && !m_quickAccessOnTop)
        m_rcQuickAccess = QRect(QPoint(0, rcPageArea.bottom()), QPoint(szBar.width(), rcPageArea.bottom() + quickAccessHeight));
    else
    {
        m_rcQuickAccess = QRect(QPoint(5 + corner, m_rcTitle.top() + frameBorder), szBar);
//        m_rcQuickAccess = QRect(QPoint(3 + corner, m_rcTitle.top() + frameBorder - 2), 
//            QPoint(6 + corner + szBar.width(), m_rcTitle.top() + /*frameBorder - 2 +*/ quickAccessHeight - 1));
    }

    QPoint posAccessBar = m_rcQuickAccess.topLeft();
    m_quickAccessBar->move(posAccessBar);
    m_quickAccessBar->resize(m_rcQuickAccess.size());

    int nLeft = 2;
    int nRight = opt.rect.width() - 2;
    if (m_controlSystemButton)
    {
        QSize szControl(corner + 1, corner + 1 /*- 1*/);
        int nTop = m_rcTitle.top() + frameBorder - 1;
        nLeft = 0;
        int nGap = 0;

        if (m_controlSystemButton->toolButtonStyle() != Qt::ToolButtonFollowStyle)
        {
            szControl = m_controlSystemButton->sizeHint();
            szControl.setHeight(tabsHeight);

            nTop = rcTabBar.top() + (tabsHeight - szControl.height()) * 0.5;
            nGap = 3;
        }
        else if (!showTitle)
        {
            szControl = m_controlSystemButton->sizeHint();
            nTop = rcTabBar.top() + (tabsHeight - szControl.height()) * 0.5;
        }

        QRect rcControl(QPoint(nLeft, nTop), QPoint(nLeft + szControl.width(), nTop + szControl.height()));
        nLeft += szControl.width() + nGap;
        m_controlSystemButton->setGeometry(rcControl);
    }

    bool quickAccessNearTabs = m_quickAccessBar->isVisible() && m_quickAccessOnTop && !showTitle;

    if (quickAccessNearTabs)
    {
//      nLeft += 5;
        m_rcQuickAccess.setLeft(nLeft);
        m_rcQuickAccess.setTop(m_rcTabBar.top());
        m_rcQuickAccess.setBottom(m_rcTabBar.bottom());

        m_quickAccessBar->move(m_rcQuickAccess.topLeft());

        nLeft += szBar.width();
    }
    else
    {
//      nLeft = m_rcQuickAccess.left() + (!m_quickAccessOnTop ? 2 : 0);
    }

    if (nLeft + 6 > nRight || !m_tabBar->isVisible())
        m_rcTabBar = QRect(QPoint(0, rcTabBar.top()), QPoint(0, rcTabBar.bottom() + 1));
    else
        m_rcTabBar = QRect(QPoint(nLeft, rcTabBar.top()), QPoint(nRight, rcTabBar.bottom() + 1));

    m_tabBar->setGeometry(m_rcTabBar);
    m_tabBar->layoutWidgets();

    m_rcPageArea = rcPageArea;

    if (m_minimized)
        m_rcPageArea.setHeight(0);

    int index = m_tabBar->currentIndex();
    if (validIndex(index))
    {
        RibbonPage* page = m_listPages.at(index);
        page->setGroupsHeight(heightGroup);
        page->setGeometry(m_rcPageArea);
    }

    layoutTitleButton();

    if (m_frameHelper)
        m_frameHelper->setHeaderRect(m_rcHeader);

    m_ribbonBarVisible = saveRibbonBarVisible;

    bool ribbonBarVisible = p.isTitleBarVisible() ? m_rcHeader.width() > 3 : opt.rect.width() >= 250;
    if (m_ribbonBarVisible != ribbonBarVisible)
    {
        m_ribbonBarVisible = ribbonBarVisible;
        if (m_frameHelper)
            m_frameHelper->postRecalcFrameLayout();
    }
        
//    if (m_frameHelper)
//        m_frameHelper->setHeaderRect(m_rcHeader);

    if (showTitle && m_ribbonBarVisible)
        layoutContextHeaders();
    else
        removeAllContextHeaders();

    if (parent && updates)
        parent->setUpdatesEnabled(updates);
}

void RibbonBarPrivate::layoutTitleButton()
{
    QTN_P(RibbonBar);
    if (!p.isFrameThemeEnabled())
        return;

    StyleOptionTitleBar titleBarOpt;
    titleBarOptions(titleBarOpt);

    QRect rcQuickAccess = m_rcQuickAccess;
    if (m_ribbonBarVisible)
    {
        if (!m_quickAccessOnTop || !m_quickAccessBar->isVisible())
        {
            if (!m_controlSystemButton)
                m_rcHeader.setLeft(m_rcHeader.left() + /*30*/titleBarOpt.icon.actualSize(QSize(16, 16)).width() + 2);
            else
            {
                if (m_controlSystemButton->toolButtonStyle() == Qt::ToolButtonFollowStyle)
                    m_rcHeader.setLeft(m_rcHeader.left() + /*70*/m_controlSystemButton->sizeHint().width() - 5);
                else
                    m_rcHeader.setLeft(m_rcHeader.left() + /*30*/titleBarOpt.icon.actualSize(QSize(16, 16)).width() + 2);
            }

            if (m_controlSystemButton)
                rcQuickAccess = m_controlSystemButton->rect();
            else
                rcQuickAccess = QRect();
        }
        else
        {
            rcQuickAccess.setRight(rcQuickAccess.left() + (m_quickAccessBar->isVisible() ? m_quickAccessBar->width() + 2 : 12));
            m_rcHeader.setLeft(rcQuickAccess.right());
        }
    }
    else
    {
        int corner = 2 + (!titleBarOpt.icon.isNull() ? titleBarOpt.icon.actualSize(QSize(16, 16)).width() : 0);
        m_rcHeader.setLeft(corner);
    }

    bool dwmEnabled = m_frameHelper->isDwmEnabled();

    int frameBorder = m_frameHelper && dwmEnabled ? m_frameHelper->getFrameBorder() : 0;
    m_rcHeader.adjust(0, frameBorder, 0, -3);

    QRect rcHeader = m_rcHeader;

    bool buttonClose = titleBarOpt.titleBarFlags & Qt::WindowSystemMenuHint;

    bool buttonMax =  titleBarOpt.titleBarFlags & Qt::WindowMaximizeButtonHint
        && !(titleBarOpt.titleBarState & Qt::WindowMaximized); 
    if (!m_initFlag)
    {
        m_initFlag = true;
        buttonMax = false;
    }

    bool buttonMin =  titleBarOpt.titleBarFlags & Qt::WindowMinimizeButtonHint
        && !(titleBarOpt.titleBarState & Qt::WindowMinimized); 

    bool buttonNormal = (((titleBarOpt.titleBarFlags & Qt::WindowMinimizeButtonHint)
        && (titleBarOpt.titleBarState & Qt::WindowMinimized))
        || ((titleBarOpt.titleBarFlags & Qt::WindowMaximizeButtonHint)
        && (titleBarOpt.titleBarState & Qt::WindowMaximized)));

    m_actCloseButton = addTitleButton(QStyle::SC_TitleBarCloseButton, !dwmEnabled && buttonClose, m_rcHeader);
    m_actNormalButton = addTitleButton(QStyle::SC_TitleBarNormalButton, !dwmEnabled && buttonNormal, m_rcHeader);
    m_actMaxButton = addTitleButton(QStyle::SC_TitleBarMaxButton, !dwmEnabled && buttonMax, m_rcHeader);
    m_actMinButton = addTitleButton(QStyle::SC_TitleBarMinButton, !dwmEnabled && buttonMin, m_rcHeader);

    if (dwmEnabled)
        m_rcHeader.setRight(m_rcHeader.right() - 110);
//  else
//      m_rcHeader.setRight(m_rcHeader.right() - (m_rcHeader.width() - rcHeader.width()));

    m_rcTitleText = QRect(m_rcTitleText.left() + (rcHeader.right() - m_rcHeader.right()), m_rcHeader.top(), 
         m_rcHeader.right() - (rcHeader.right() - m_rcHeader.right()), m_rcHeader.bottom() - m_rcHeader.top() );

    QString strTitle = p.parentWidget() ? p.parentWidget()->windowTitle() : QString();

    const QFont font = QApplication::font("QMdiSubWindowTitleBar");
    const QFontMetrics fm(font);

    QRect br(fm.boundingRect(strTitle));

    int captionLength = br.width();

    if (dwmEnabled)
        captionLength += 2 * 10;// glowSize = 10 for Windows7 (Vista)

    captionLength = qMin(m_rcTitleText.width(), captionLength);

    QRect rcTitleText(m_rcTitleText);
    rcTitleText.setLeft(rcTitleText.left() + (rcTitleText.width() - captionLength) * 0.5);
    if (rcTitleText.left() < m_rcHeader.left())
        rcTitleText.setLeft(m_rcHeader.left());

    rcTitleText.setRight(rcTitleText.left() + captionLength);

    if (rcQuickAccess.intersects(rcTitleText))
    {
        captionLength = qMin(m_rcHeader.width(), captionLength);
//        rcTitleText.setLeft(m_rcHeader.left() + (m_rcHeader.width() - captionLength) * 0.5);
        rcTitleText.setRight(rcTitleText.left() + captionLength);
    }
    else if (!rcQuickAccess.isValid() || !m_quickAccessBar->isVisible())
    {
        captionLength = qMin(m_rcHeader.width(), captionLength);
        if (dwmEnabled)
            rcTitleText.setLeft(m_rcHeader.left() + 10);
        else
            rcTitleText.setLeft(m_rcHeader.left() + (m_rcHeader.width() - captionLength) * 0.5);

        rcTitleText.setRight(m_rcHeader.right());

    }

    if (rcTitleText.right() > m_rcHeader.right()) 
        rcTitleText.setRight(m_rcHeader.right());

    m_rcTitleText = rcTitleText;
}

void RibbonBarPrivate::layoutContextHeaders()
{
    QTN_P(RibbonBar);
    removeAllContextHeaders();

    if (!(p.isFrameThemeEnabled() || m_showTitleAlways))
        return;

    int count = m_tabBar->getTabCount();
    if (count == 0)
        return;

    if (m_quickAccessOnTop && m_quickAccessBar->isVisible() && !m_frameHelper)
        m_rcHeader.setLeft(m_rcQuickAccess.right() + (m_quickAccessBar ? m_quickAccessBar->sizeHint().width() + 2 : 12));

    for (int i = 0; i < count; i++)
    {
        RibbonTab* tab = m_tabBar->getTab(i);

        if (!tab->isVisible())
            continue;

        if (tab->getContextColor() == ContextColorNone || tab->contextTextTab().isEmpty())
        {
//            prevContextHeader = Q_NULL;
            continue;
        }

/*      // Fixed: pages with the same context information (color and title), the context title is not drawn upon the tabs of the pages.
        if (prevContextHeader && prevContextHeader->color == tab->getContextColor() &&
            prevContextHeader->strTitle == tab->contextTextTab())
        {
            prevContextHeader->rcRect.setRight(tab->rect().right());
            prevContextHeader->lastTab = tab;
            tab->setContextHeader(prevContextHeader);
        }
        else
*/
        {
            ContextHeader* header = new ContextHeader(tab);
            QRect rect = tab->rect();
            QPoint topLeft = tab->mapToParent(rect.topLeft());
            QPoint bottomRight = tab->mapToParent(rect.bottomRight());

            topLeft = m_tabBar->mapToParent(topLeft);
            bottomRight = m_tabBar->mapToParent(bottomRight);

            header->rcRect = QRect(QPoint(topLeft.x(), m_rcTitle.top() + 4), QPoint(bottomRight.x(), m_rcTitle.bottom()));

            m_listContextHeaders.append(header);
//            prevContextHeader = header;
        }
    }

    count = m_listContextHeaders.count();
    if (count == 0)
        return;

    if (!reduceContextHeaders())
    {
        removeAllContextHeaders();
        return;
    }

    QRect rcHeaders(QPoint(m_listContextHeaders.at(0)->rcRect.left(), m_rcTitle.top() + 6),
        QPoint(m_listContextHeaders.at(count - 1)->rcRect.right(), m_rcTitle.bottom()));

    int captionLength = m_rcTitleText.width();
    QRect rcCaptionText(m_rcTitleText);

    if (rcCaptionText.intersects(rcHeaders))
    {
        if (rcCaptionText.center().x() > rcHeaders.center().x())
        {
            if (m_rcHeader.right() - rcHeaders.right() < captionLength)
            {
                m_rcTitleText.setLeft(rcHeaders.right());
                m_rcTitleText.setRight(m_rcHeader.right());
            }
            else
            {
                m_rcTitleText.setLeft(rcHeaders.right());
                m_rcTitleText.setRight(rcHeaders.right() + captionLength);
            }
        }
        else
        {
            if (rcHeaders.left() - m_rcHeader.left() < captionLength)
            {
                m_rcTitleText.setLeft(m_rcHeader.left());
                m_rcTitleText.setRight(rcHeaders.left());
            }
            else
            {
                m_rcTitleText.setLeft(rcHeaders.left() - captionLength);
                m_rcTitleText.setRight(rcHeaders.left());
            }
        }
    }
}

void RibbonBarPrivate::layoutCorner()
{
    QTN_P(RibbonBar);
    int q_width = p.width()-(p.style()->pixelMetric(QStyle::PM_MenuBarPanelWidth, 0, &p)*2);

    QWidget* rightWidget = p.cornerWidget(Qt::TopRightCorner);
    if(rightWidget) 
    {
        int vmargin = p.style()->pixelMetric(QStyle::PM_MenuBarVMargin, 0, &p) + p.style()->pixelMetric(QStyle::PM_MenuBarPanelWidth, 0, &p);
        int hmargin = p.style()->pixelMetric(QStyle::PM_MenuBarHMargin, 0, &p) + p.style()->pixelMetric(QStyle::PM_MenuBarPanelWidth, 0, &p);

        if (rightWidget && rightWidget->isVisible()) 
        {
            QSize sz = rightWidget->sizeHint();
            q_width -= sz.width();
            QPoint pos(p.width() - sz.width() - hmargin, vmargin);
            QRect vRect = QStyle::visualRect(p.layoutDirection(), p.rect(), QRect(pos, sz));
            rightWidget->setGeometry(vRect);
        }
    }
}

bool RibbonBarPrivate::reduceContextHeaders()
{
    int left = m_rcHeader.left();
    int right = m_rcHeader.right();

    int count = m_listContextHeaders.count();

    for ( int i = 0; i < count; i++)
    {
        ContextHeader* header =  m_listContextHeaders.at(i);
        if (header->rcRect.left() < left)
        {
            header->rcRect.setLeft(left);
            if (header->rcRect.width()  < 40)
                header->rcRect.setRight(left + 40);

            left = header->rcRect.right();
        }
        else break;
    }
    for (int i = count - 1; i >= 0; i--)
    {
        ContextHeader* header =  m_listContextHeaders.at(i);
        if (header->rcRect.right() > right)
        {
            header->rcRect.setRight(right);
            if (header->rcRect.width() < 40)
                header->rcRect.setLeft(right - 40);

            right = header->rcRect.left();

            if (right < left)
                return false;
        }
        else break;
    }
    return true;
}

void RibbonBarPrivate::removeAllContextHeaders()
{
    for (int i = 0; i < m_listContextHeaders.count(); i++)
        delete m_listContextHeaders.at(i);
    m_listContextHeaders.clear();
}

void RibbonBarPrivate::updateMinimizedModePage(RibbonPage* page)
{
    QTN_P(RibbonBar);
    if (m_minimized)
    {
        page->setPageVisible(false);
        page->setWindowFlags(Qt::Popup);
        page->setFocusPolicy(Qt::NoFocus);
        page->setFocusProxy(&p);
        page->setRibbonMinimized(true);

        QEvent ev(QEvent::LayoutRequest);
        QApplication::sendEvent(page, &ev);
    }
    else
    {
        Qt::WindowFlags flags = page->windowFlags();
        if(flags & Qt::Popup) 
        {
            flags &= ~Qt::Popup;
            page->setWindowFlags(flags);
            page->setRibbonMinimized(false);

            QEvent ev(QEvent::LayoutRequest);
            QApplication::sendEvent(page, &ev);
        }
    }
}

void RibbonBarPrivate::swapVisiblePages(int index)
{
    if (validIndex(index))
    {
        for (int i = 0, count = m_listPages.size(); count > i; i++)
            m_listPages.at(i)->setPageVisible(index == i);
    }
}

QAction* RibbonBarPrivate::createSystemButton(const QIcon& icon, const QString& text)
{
    QTN_P(RibbonBar);
    QAction* actionSystemPopupBar = Q_NULL;
    if (icon.isNull())
    {
        QIcon iconQtnLogo;
        QIcon qtitan(":/res/qtitan.png");
        iconQtnLogo.addPixmap(qtitan.pixmap(QSize(64,56), QIcon::Disabled));
        QIcon icon32(":/res/qtitanlogo32x32.png");
        iconQtnLogo.addPixmap(icon32.pixmap(QSize(32,32), QIcon::Disabled));
        actionSystemPopupBar = new QAction(iconQtnLogo, text, &p);
    }
    else
        actionSystemPopupBar = new QAction(icon, text, &p);

    m_controlSystemButton = new RibbonSystemButton(&p);
    m_controlSystemButton->show();
    m_controlSystemButton->setAutoRaise(true);
    m_controlSystemButton->setFocusPolicy(Qt::NoFocus);
    m_controlSystemButton->setDefaultAction(actionSystemPopupBar);

    m_controlSystemButton->setPopupMode(QToolButton::InstantPopup);
    m_controlSystemButton->setToolButtonStyle(Qt::ToolButtonFollowStyle);

    actionSystemPopupBar->setIconText(actionSystemPopupBar->text());

    return actionSystemPopupBar;
}

RibbonPage* RibbonBarPrivate::insertPage(int indTab, int index)
{
    QTN_P(RibbonBar);
    RibbonPage* page = new RibbonPage(&p);
    page->hide();
    insertPage(indTab, index, page);
    
    return page;
}

void RibbonBarPrivate::insertPage(int indTab, int index, RibbonPage* page)
{
    Q_ASSERT(page != Q_NULL);

    bool layoutPage = index == 0 && m_listPages.size() == 0;
    if (!validIndex(index)) 
    {
        index = m_listPages.count();
        m_listPages.append(page);
    }
    else
        m_listPages.insert(index, page);

    if (RibbonTab* tab = m_tabBar->getTab(indTab))
        page->setAssociativeTab(tab);

    if (!m_minimized)
    {
        int currentIndex = m_tabBar->currentIndex();
        if (currentIndex != -1 && m_currentIndexPage != currentIndex &&
            currentIndex < m_listPages.count())
        {
            swapVisiblePages(currentIndex);
            m_currentIndexPage = currentIndex;
        }
        if (layoutPage)
            swapVisiblePages(index);
    }
}

void RibbonBarPrivate::removePage(int index, bool deletePage)
{
    if (validIndex(index))
    {
        RibbonPage* page = m_listPages[index];
        m_listPages.removeAt(index);
        if (deletePage)
            delete page;
        else
        {
            updateMinimizedModePage(page);
            page->setAssociativeTab(Q_NULL);
            page->setParent(Q_NULL);
        }
    }
    m_currentIndexPage = -1;
}

int RibbonBarPrivate::getIndexPage(RibbonPage* page) const
{
    return m_listPages.indexOf(page);
}

void RibbonBarPrivate::currentChanged(int index)
{
    QTN_P(RibbonBar);

    if (validIndex(index))
    {
        if (!m_minimized) 
        {
            bool updates = p.updatesEnabled();
            if (updates)
                p.setUpdatesEnabled(false);

            emit p.currentPageChanged(index);
            emit p.currentPageChanged(m_listPages[index]);
            swapVisiblePages(index);

            if (m_dirtyPages.indexOf(index) == -1)
            {
                QEvent ev(QEvent::LayoutRequest);
                QApplication::sendEvent(p.getPage(index), &ev);
                layoutRibbon();
                m_dirtyPages.append(index);
            }
            if (updates)
                p.setUpdatesEnabled(updates);
        }
        else
        {
            if (RibbonPage* page = p.getPage(index))
            {
                emit p.currentPageChanged(index);
                emit p.currentPageChanged(m_listPages[index]);

                page->setGeometry(QRect(QPoint(0,0), QSize(1, 1)));

                bool updates = page->updatesEnabled();
                if (updates)
                    page->setUpdatesEnabled(false);

                QWidget* activePopup = qobject_cast<RibbonPage*>(QApplication::activePopupWidget());

                QStyleOption opt;
                opt.init(page);

                const int vmargin = p.style()->pixelMetric(QStyle::PM_MenuBarVMargin, &opt, &p);
                int heightGroup = getMaxHeightPages();
                int height = heightGroup + vmargin * 2 + vmargin + 1;
                page->setGroupsHeight(heightGroup);

                QPoint pnt(0, 0);
                QRect rect = p.rect();
                rect.setTop(m_rcTabBar.top());
                rect.setBottom(m_rcTabBar.bottom());

                QRect screen = QApplication::desktop()->availableGeometry(&p);
                int h = height;
                if (p.mapToGlobal(QPoint(0, rect.bottom())).y() + h <= screen.height()) 
                    pnt = p.mapToGlobal(rect.bottomLeft());
                else 
                    pnt = p.mapToGlobal(rect.topLeft() - QPoint(0, h));

                page->popup();
                page->setGeometry(QRect(pnt, QSize(p.sizeHint().width(), height)));

                if (updates)
                    page->setUpdatesEnabled(updates);

                page->repaint();

                if (activePopup)
                    activePopup->close();
            }
        }
    }
}

void RibbonBarPrivate::titleBarOptions(StyleOptionTitleBar& titleBarOptions) const
{
    QTN_P(const RibbonBar);
    QWidget* widget = p.parentWidget();
    if (!widget)
        return;

    titleBarOptions.initFrom(widget);

    titleBarOptions.frameHelper = m_frameHelper;

    if (m_activeSubControl != QStyle::SC_None) 
    {
        if (m_hoveredSubControl == m_activeSubControl) 
        {
            titleBarOptions.state |= QStyle::State_Sunken;
            titleBarOptions.activeSubControls = m_activeSubControl;
        }
    }
    else if (p.style()->styleHint(QStyle::SH_TitleBar_AutoRaise, 0, widget) && 
             m_hoveredSubControl != QStyle::SC_None &&
             m_hoveredSubControl != QStyle::SC_TitleBarLabel)
    {
        titleBarOptions.state |= QStyle::State_MouseOver;
        titleBarOptions.activeSubControls = m_hoveredSubControl;
    }
    else
    {
        titleBarOptions.state &= ~QStyle::State_MouseOver;
        titleBarOptions.activeSubControls = QStyle::SC_None;
    }

    titleBarOptions.subControls = QStyle::SC_All;
    titleBarOptions.titleBarFlags = widget->windowFlags();
    titleBarOptions.titleBarState = widget->windowState();

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    if (titleBarOptions.frameHelper && titleBarOptions.frameHelper->isMaximize() && !(titleBarOptions.titleBarState & Qt::WindowMaximized))
        titleBarOptions.titleBarState |= Qt::WindowMaximized;
#endif

    if (titleBarOptions.titleBarFlags & Qt::WindowTitleHint)
    {
        QIcon icon = p.parentWidget() ? p.parentWidget()->windowIcon() : QIcon();
        QSize s = icon.actualSize(QSize(64, 64));
        titleBarOptions.icon = icon.pixmap(s);
    }

    if (m_frameHelper && titleBarOptions.icon.isNull())
        titleBarOptions.icon = m_frameHelper->getFrameSmallIcon();

    if (m_controlSystemButton && m_controlSystemButton->isVisible())
        titleBarOptions.drawIcon = !m_controlSystemButton  || m_controlSystemButton->toolButtonStyle() != Qt::ToolButtonFollowStyle;

    int border = p.style()->styleHint(QStyle::SH_TitleBar_NoBorder, &titleBarOptions, widget) ? 4 : 0;

    int paintHeight = p.titleBarHeight();
    paintHeight -= p.isMinimized() ? 2 * border : border;
    titleBarOptions.rect = m_rcTitle;
    
    RibbonBarPrivate* that = const_cast<RibbonBarPrivate*>(this);
    that->m_windowTitle = p.parentWidget() ? p.parentWidget()->windowTitle() : QString();

    if (!m_windowTitle.isEmpty()) 
    {
        // Set the text here before asking for the width of the title bar label
        // in case people uses the actual text to calculate the width.
        titleBarOptions.text = m_windowTitle;
        const QFont font = QApplication::font("QMdiSubWindowTitleBar");
        titleBarOptions.fontMetrics = QFontMetrics(font);
        int width = m_rcHeader.width();
        // Set elided text if we don't have enough space for the entire title.
        titleBarOptions.text = titleBarOptions.fontMetrics.elidedText(m_windowTitle, Qt::ElideRight, width);
    }

    titleBarOptions.rcTitleText = m_rcTitleText;
    titleBarOptions.quickAccessVisibleCount = m_quickAccessBar ? m_quickAccessBar->getVisibleCount() : 0;
    titleBarOptions.quickAccessOnTop = m_quickAccessOnTop;
    QSize szAccessBar =  m_quickAccessBar->size();
    titleBarOptions.quickAccessVisible = m_quickAccessBar && m_quickAccessBar->isVisible() && !szAccessBar.isNull();
    titleBarOptions.rcQuickAccess = m_rcQuickAccess;
    titleBarOptions.existSysButton = m_controlSystemButton;
    titleBarOptions.normalSysButton = m_controlSystemButton ? (m_controlSystemButton->toolButtonStyle() != Qt::ToolButtonFollowStyle) : true;
}

QStyle::SubControl RibbonBarPrivate::getSubControl(const QPoint& pos) const
{
    QTN_P(const RibbonBar);
    StyleOptionTitleBar opt;
    titleBarOptions(opt);
    return p.style()->hitTestComplexControl(QStyle::CC_TitleBar, &opt, pos, p.parentWidget());
}

RibbonTitleButton* RibbonBarPrivate::findTitleButton(QStyle::SubControl subControl) const
{
    QTN_P(const RibbonBar);
    QList<RibbonTitleButton*> buttons_list = p.findChildren<RibbonTitleButton*>();
    foreach (RibbonTitleButton* but, buttons_list) 
    {
        if (but->getSubControl() == subControl)
            return but;
    }
    return Q_NULL;
}

QAction* RibbonBarPrivate::addTitleButton(QStyle::SubControl subControl, bool add, QRect& rcControl)
{
    QTN_P(RibbonBar);

    RibbonTitleButton* but = findTitleButton(subControl);

    QAction* act = but ? but->defaultAction() : Q_NULL;

    if (!but && add)
    {
        act = new QAction(&p);
        but = new RibbonTitleButton(&p, subControl);
        but->show();
        but->setDefaultAction(act);
        connect(act, SIGNAL(triggered()), this, SLOT(processClickedSubControl()));
        act->setToolTip(m_frameHelper->getSystemMenuString(subControl));
    }
    else if (but&& !add)
    {
        act = but->defaultAction();
        disconnect(act, SIGNAL(triggered()), this, SLOT(processClickedSubControl()));

//        delete but;
        but->deleteLater();
        but = Q_NULL;

        delete act;
        act = Q_NULL;
    }

    if (add)
    {
        QRect rcButton(rcControl.right() - rcControl.height(), rcControl.top(), rcControl.right()-(rcControl.right() - rcControl.height()), rcControl.bottom());
        rcButton.adjust(0,0,-1,0);
        but->setGeometry(rcButton);
        rcControl.setRight(rcControl.right() - rcButton.width());
    }
    return act;
}

ContextHeader* RibbonBarPrivate::hitTestContextHeaders(const QPoint& point) const
{
    for ( int i = 0, count = m_listContextHeaders.count(); i < count; i++)
    {
        ContextHeader* header =  m_listContextHeaders.at(i);
        if (header->rcRect.contains(point))
             return header;
    }
    return Q_NULL;
}

static QString findAccel(const QString &text)
{
    if (text.isEmpty())
        return text;

    int fa = 0;
    QChar ac;
    while ((fa = text.indexOf(QLatin1Char('&'), fa)) != -1) 
    {
        ++fa;
        if (fa < text.length()) 
        {
            // ignore "&&"
            if (text.at(fa) == QLatin1Char('&')) 
            {
                ++fa;
                continue;
            } 
            else 
            {
                ac = text.at(fa);
                break;
            }
        }
    }
    if (ac.isNull())
        return QString();
    return (QString)ac.toUpper();
}

void RibbonBarPrivate::showKeyTips(QWidget* w)
{
    hideKeyTips();
    m_countKey = 0;

    if (qobject_cast<RibbonBar*>(w))
        createKeyTips();
    else if (qobject_cast<RibbonPage*>(w))
        createGroupKeyTips();
    else if (QMenu* menu = qobject_cast<QMenu*>(w))
        createPopupKeyTips(menu);

    int count = m_keyTips.count();
    if (count == 0)
        return;

    for (int i = count - 1; i >= 0; i--)
    {
        RibbonKeyTip* keyTip = m_keyTips.at(i);
        keyTip->setExplicit(!keyTip->getStringTip().isEmpty());

        if (keyTip->isExplicit())
            continue;

        QString strCaption = keyTip->getCaption();
        if (strCaption.isEmpty())
        {
            keyTip->close();
            keyTip->deleteLater();
            m_keyTips.removeAt(i);
            continue;
        }

        strCaption = strCaption.toUpper();
        keyTip->setCaption(strCaption);

        if (keyTip->getPrefix().isEmpty())
        {
            QString key = ::findAccel(strCaption);
            if (key.isEmpty())
            {
                if (!strCaption.isEmpty())
                    keyTip->setStringTip(QChar(strCaption[0]));
            }
            else
                keyTip->setStringTip(key);
        }
        else
        {
            keyTip->setStringTip(keyTip->getPrefix());
        }
    }

    count = m_keyTips.count();
    for (int i = 0; i < count; i++)
    {
        RibbonKeyTip* keyTip = m_keyTips.at(i);

        QString strTip = keyTip->getStringTip();

        QList<RibbonKeyTip*> list;
        list.append(keyTip);

        int j;
        QString strUsed = QObject::tr("& ");

        for ( j = i + 1; j < count; j++)
        {
            RibbonKeyTip* keyTipWidget = m_keyTips.at(j);;
            if (keyTipWidget->getStringTip()[0] == strTip[0])
            {
                list.append(keyTipWidget);
                if (keyTipWidget->getStringTip().length() == 2)
                    strUsed += keyTipWidget->getStringTip()[1];
            }
        }

        if (list.count() < 2)
            continue;

        int k = 1;
        for (int j = 0; j < list.count(); j++)
        {
            RibbonKeyTip* keyTipWidget = list.at(j);
            if (keyTipWidget->getStringTip().length() > 1)
                continue;

            QChar chAccel = 0;
            QString strCaption = keyTipWidget->getCaption();
            for (int n = 0; n < strCaption.length() - 1; n++)
            {
                if ((strCaption[n] == ' ') && (strUsed.indexOf(strCaption[n + 1]) == -1))
                {
                    chAccel = strCaption[n + 1];
                    strUsed += chAccel;
                    break;
                }
            }

            if (chAccel.isNull())
            {
                int start = strCaption[0] == QLatin1Char('&') ? 1 : 0;
                if (strCaption[start] == keyTipWidget->getStringTip()[0])
                    start++;

                for (int n = start; n < strCaption.length(); n++)
                {
                    if (strUsed.indexOf(strCaption[n]) == -1)
                    {
                        chAccel = keyTipWidget->getCaption()[n];
                        strUsed += chAccel;
                        break;
                    }
                }
            }
            if (chAccel == 0)
            {
                QString str = QObject::tr("%1").arg(k);
                chAccel = str[0];
                k++;
            }
            keyTipWidget->setStringTip(keyTipWidget->getStringTip() + chAccel);
        }
    }

    calcPositionKeyTips();

    if (m_keyTips.count() > 0)
        qApp->installEventFilter(this);

    QTN_P(RibbonBar);
    p.setFocus();
}

bool RibbonBarPrivate::hideKeyTips()
{
    bool res = false;
    if (m_keyTips.count() > 0)
    {
        qApp->removeEventFilter(this);
        res = true;
    }
    destroyKeyTips();
    m_countKey = 0;
    return res;
}

void RibbonBarPrivate::createKeyTips()
{
    QTN_P(RibbonBar);
    m_levels.push(&p);

    QList<QToolButton*> list = m_quickAccessBar->findChildren<QToolButton*>();
    for (int i = 0, count = list.count(); i < count; i++)
    {
        QToolButton* button = list.at(i);
        if (!button->isVisible() || button->objectName() == QLatin1String("QtnRibbonQuickAccessButton"))
            continue;

        int k = 1;
        QString strCaption;
        if (QAction* act = button->defaultAction())
        {
            strCaption = act->property(qtn_Prop_Key_Tip).toString();
            if (strCaption.isEmpty())
            {
                QList<QAction*> listActs = m_quickAccessBar->actions();
                int index = listActs.indexOf(act);
                if (index != -1)
                    k = index;
            }
        }

        if (strCaption.isEmpty())
        {
            if (k < 10)
                strCaption = QObject::tr("%1").arg(k);
            else if (k < 19)
                strCaption = QObject::tr("%1").arg(k - 9);
            else 
                break;
        }

        m_keyTips.append(new RibbonKeyTip(&p, button, strCaption, button->mapToGlobal(QPoint(button->rect().center().x(), button->rect().bottom() - 11)), 
            RibbonKeyTip::AlignTipCenter | RibbonKeyTip::AlignTipTop, button->isEnabled()));
    }

    if (m_tabBar && m_tabBar->isVisible())
    {
        for (int i = 0, count = m_tabBar->getTabCount(); i < count; i++)
        {
            RibbonTab* tab = m_tabBar->getTab(i);

            bool propKeyTip = true;
            QString strCaption = tab->defaultAction()->property(qtn_Prop_Key_Tip).toString();

            if (strCaption.isEmpty())
            {
                strCaption = tab->textTab();
                propKeyTip = false;
            }

            if (!m_keyTipsComplement && !propKeyTip && ::findAccel(strCaption).isEmpty())
                strCaption = QString();

            if (!strCaption.isEmpty() && tab->isEnabled() && tab->isVisible())
            {
                QPoint pt(tab->rect().center().x(), tab->rect().bottom() - 9);
                RibbonKeyTip* keyTip = new RibbonKeyTip(&p, tab, strCaption, tab->mapToGlobal(pt), 
                    RibbonKeyTip::AlignTipCenter | RibbonKeyTip::AlignTipTop, true);
                m_keyTips.append(keyTip);
            }
        }
    }

    if (m_controlSystemButton)
    {
        QPoint pt(m_controlSystemButton->rect().center().x(), m_controlSystemButton->rect().center().y());

        QString strCaption = m_controlSystemButton->defaultAction()->property(qtn_Prop_Key_Tip).toString();
        if (strCaption.isEmpty())
            strCaption = m_controlSystemButton->text();

        m_keyTips.append(new RibbonKeyTip(&p, m_controlSystemButton, strCaption, 
            m_controlSystemButton->mapToGlobal(pt), RibbonKeyTip::AlignTipCenter | RibbonKeyTip::AlignTipVCenter, m_controlSystemButton->isEnabled()));
    }
}

void RibbonBarPrivate::createGroupKeyTips()
{
    int index = m_tabBar->currentIndex();
    if (!validIndex(index))
        return;

    RibbonPage* page = m_listPages.at(index);
    m_levels.push(page);
    for (int i = 0, count = page->groupCount(); i < count; i++)
    {
        RibbonGroup* group = page->getGroup(i);
        bool visible = group->isVisible();

        QList<QWidget*> list = group->findChildren<QWidget*>();
        for (int j = 0; j < list.count(); j++)
        {
            QWidget* widget = list.at(j);
            createWidgetKeyTips(group, widget, QString(), page->rect().translated(page->mapToGlobal(page->rect().topLeft())), visible);
        }
    }
}

static QString getTextAction(const RibbonGroup& group, QWidget& widget, bool& propKeyTip)
{
    QString strCaption;
    foreach (QAction* action, group.actions())
    {
        if (QWidgetAction* widgetAction = qobject_cast<QWidgetAction*>(action))
        {
            if (&widget == widgetAction->defaultWidget())
            {
                strCaption = widgetAction->property(qtn_Prop_Key_Tip).toString();
                if (strCaption.isEmpty())
                {
                    strCaption = widgetAction->text();
                    propKeyTip = false;
                }
                break;
            }
        }
    }
    return strCaption;
}

void RibbonBarPrivate::createWidgetKeyTips(RibbonGroup* group, QWidget* widget, const QString& prefix, const QRect& rcGroups, bool visible)
{
    QTN_P(RibbonBar);

    if (!widget /*|| Qt::NoFocus == widget->focusPolicy()*/)
        return;

    bool propKeyTip = true;
    QString strCaption;
    if (QAbstractButton* button = qobject_cast<QAbstractButton*>(widget))
    {
        strCaption = ::getTextAction(*group, *button, propKeyTip);
        if(strCaption.isEmpty())
        {
            if (QToolButton* toolButton = qobject_cast<QToolButton*>(button))
            {
                if (QAction* action = toolButton->defaultAction())
                {
                    strCaption = action->property(qtn_Prop_Key_Tip).toString();
                    if (strCaption.isEmpty())
                    {
                        strCaption = action->text();
                        propKeyTip = false;
                    }
                }

                if (strCaption.isEmpty())
                {
                    if (qobject_cast<RibbonGallery*>(toolButton->parentWidget()))
                    {
                        if (toolButton->property(qtn_PopupButtonGallery).toBool())
                            strCaption = ::getTextAction(*group, *toolButton->parentWidget(), propKeyTip);
                    }
                    else if (strCaption.isEmpty())
                        strCaption = group->title();
                }
            }
            else
            {
                strCaption = ::getTextAction(*group, *widget, propKeyTip);
                if (strCaption.isEmpty())
                    strCaption = button->text();
            }
        }
    }
    else if (ExWidgetWrapper* wrapper = qobject_cast<ExWidgetWrapper*>(widget))
        strCaption = ::getTextAction(*group, *wrapper, propKeyTip);
//        strCaption = wrapper->labelText();

    bool textWidget = true;
    if (RibbonGallery* gallery = qobject_cast<RibbonGallery*>(widget))
        textWidget = !gallery->popupMenu();

    if (strCaption.isEmpty() && textWidget)
        strCaption = ::getTextAction(*group, *widget, propKeyTip);

    if (!m_keyTipsComplement && !propKeyTip && ::findAccel(strCaption).isEmpty())
        strCaption = QString();

    if (strCaption.isEmpty())
        return;

    QRect rc = widget->rect();
    rc.translate(widget->mapToGlobal(rc.topLeft()));

    if (rc.isEmpty())
        return;

    QPoint pt(rc.left() + 21, rc.bottom() - 4);
    uint align = RibbonKeyTip::AlignTipTop;

    QStyleOption opt;
    opt.init(&p);

    QRect rcGroup = group->rect();
    rcGroup.translate(group->mapToGlobal(rcGroup.topLeft()));
    const int heightCaptionGroup = p.style()->pixelMetric((QStyle::PixelMetric)RibbonStyle::PM_RibbonHeightCaptionGroup, &opt, 0);
    QRect rcCaption(QPoint(rcGroup.left(), rcGroup.bottom() - heightCaptionGroup + 2), QPoint(rcGroup.right(), rcGroup.bottom()));
    rcGroup.setBottom(rcCaption.top());

    if (rc.intersects(rcCaption))
    {
        pt = QPoint(rc.center().x(), rcGroups.bottom() - 6);
        align = RibbonKeyTip::AlignTipTop | RibbonKeyTip::AlignTipCenter;
    }
    else if ((Qt::ToolButtonStyle)widget->property("toolButtonStyle").toInt() == Qt::ToolButtonTextUnderIcon && rc.height() > rcGroup.height() * 0.5)
    {
        pt = QPoint(rc.center().x(), rcGroup.bottom() - 2);
        align = RibbonKeyTip::AlignTipCenter | RibbonKeyTip::AlignTipVCenter;
    }
    else if (qobject_cast<RibbonGallery*>(widget))
    {
        pt = QPoint(rc.right(), rcGroup.bottom() - 2);
        align = RibbonKeyTip::AlignTipCenter | RibbonKeyTip::AlignTipVCenter;
    }
    else if (rc.center().y() < rcGroup.top() + rcGroup.height() * 1 / 3)
    {
        ///
        pt = QPoint(rc.left() + 11, rcGroup.top() + 2);
        align = RibbonKeyTip::AlignTipLeft | RibbonKeyTip::AlignTipVCenter;
    }
    else if (rc.center().y() > rcGroup.top() + rcGroup.height() * 2 / 3)
    {
        pt = QPoint(rc.left() + 11, rcGroup.bottom() - 2);
        align = RibbonKeyTip::AlignTipLeft | RibbonKeyTip::AlignTipVCenter;
    }
    else
    {
        pt = QPoint(rc.left() + 11, rcGroup.center().y() - 1);
        align = RibbonKeyTip::AlignTipLeft | RibbonKeyTip::AlignTipVCenter;
    }
    if (qobject_cast<QCheckBox*>(widget) || qobject_cast<QRadioButton*>(widget))
    {
        pt.setX(rc.left() + 6);
    }
/*
    if (qobject_cast<QComboBox*>(widget) || qobject_cast<QLineEdit*>(widget))
    {
        pt.setX(rc.center().x());
        align |= RibbonKeyTip::AlignTipCenter;
    }
*/
    RibbonKeyTip* keyTip = new RibbonKeyTip(&p, widget, strCaption, pt, align, widget->isEnabled());

    if (!prefix.isEmpty())
        keyTip->setPrefix(prefix);

    QRect rcIntersect; 
    rcIntersect = rcGroups.intersected(rc);
    keyTip->setVisibleTip(visible && (rcIntersect == rc) && (widget->isVisible()));
    m_keyTips.append(keyTip);
}

bool RibbonBarPrivate::createPopupKeyTips(QMenu* levalMenu)
{
    QTN_P(RibbonBar);
    QMenu* menu = levalMenu ? levalMenu : qobject_cast<QMenu*>(QApplication::activePopupWidget());
    if (menu)
    {
        m_levels.push(menu);

        QList<QAction*> list = menu->actions();
        for (int i = 0; i < list.count(); i++)
        {
            QAction* action = list.at(i);

            if (action->text().count(QString(qtn_PopupLable)) != 0)
                continue;

            bool propKeyTip = true;
            QString strCaption = action->property(qtn_Prop_Key_Tip).toString();

            if (strCaption.isEmpty())
            {
                strCaption = action->text();
                propKeyTip = false;
            }

            if (!m_keyTipsComplement && !propKeyTip && ::findAccel(strCaption).isEmpty())
                strCaption = QString();

            if (!action->isVisible() || strCaption.isEmpty())
                continue;

            QRect rect = menu->actionGeometry(action);
            QPoint pt(rect.left() + 12, rect.bottom() - 11);
            RibbonKeyTip* keyTip = new RibbonKeyTip(&p, menu, strCaption, menu->mapToGlobal(pt), RibbonKeyTip::AlignTipTop, action->isEnabled(), action);

            m_keyTips.append(keyTip);
        }

        QList<QWidget*> listWidget = menu->findChildren<QWidget*>();
        for (int i = 0; i < listWidget.count(); i++)
        {
            QWidget* widget = listWidget.at(i);

            bool propKeyTip = true;
            QString strCaption;

            if (QToolButton* toolButton = qobject_cast<QToolButton*>(widget))
            {
                if (QAction* action = toolButton->defaultAction())
                {
                    strCaption = action->property(qtn_Prop_Key_Tip).toString();
                    if (strCaption.isEmpty())
                    {
                        strCaption = action->text();
                        propKeyTip = false;
                    }
                }
            }

            if (!m_keyTipsComplement && !propKeyTip && ::findAccel(strCaption).isEmpty())
                strCaption = QString();

            if (!widget->isVisible() || strCaption.isEmpty())
                continue;

            QPoint pt(widget->rect().left() + 12, widget->rect().bottom() - 11);
            RibbonKeyTip* keyTip = new RibbonKeyTip(&p, widget, strCaption, widget->mapToGlobal(pt), 
                RibbonKeyTip::AlignTipTop, widget->isEnabled());
            m_keyTips.append(keyTip);
        }
        return true;
    }
    return false;
}

void RibbonBarPrivate::destroyKeyTips()
{
    for (int i = 0; i < m_keyTips.count(); i++)
    {
        RibbonKeyTip* keyTip = m_keyTips.at(i);
        keyTip->close();
        keyTip->deleteLater();
    }
    m_keyTips.clear();
}

void RibbonBarPrivate::calcPositionKeyTips()
{
    for (int i = 0, count = m_keyTips.count(); i < count; i++)
    {
        RibbonKeyTip* keyTip = m_keyTips.at(i);
        if (!keyTip->isVisibleTip())
            continue;

        QSize sz(0, 0);
        QFontMetrics fontMetrics(keyTip->font());
        QRect rect = fontMetrics.boundingRect(keyTip->getStringTip());
        sz = QSize(qMax(rect.width() + 6, 16), qMax(rect.height() + 2, 15));

        QPoint pt = keyTip->posTip();
        uint flagAlign = keyTip->getAlign();

        QRect rc(QPoint(pt.x() - ((flagAlign & RibbonKeyTip::AlignTipCenter) ?  sz.width() * 0.5 : (flagAlign & RibbonKeyTip::AlignTipRight) ? sz.width() : 0),
            pt.y() - (flagAlign & RibbonKeyTip::AlignTipVCenter ?  sz.height() * 0.5 : flagAlign & RibbonKeyTip::AlignTipBottom ? sz.height() : 0)), sz);

        keyTip->setGeometry(rc);
        keyTip->show();
        keyTip->repaint();
    }
}

int RibbonBarPrivate::getMaxHeightPages() const
{
    QTN_P(const RibbonBar);
    QStyleOption opt;
    int maxHeightPages =  p.style()->pixelMetric((QStyle::PixelMetric)RibbonStyle::PM_RibbonHeightGroup, &opt, 0);
    for (int i = 0, count = m_listPages.count(); count > i; i++)
    {
        RibbonPage* page = m_listPages[i];
        for (int g = 0, countG = page->groupCount(); countG > g; g++)
        {
            if (RibbonGroup* group = page->getGroup(g))
                maxHeightPages = qMax(maxHeightPages, group->sizeHint().height());
        }
    }
    return maxHeightPages;
}

bool RibbonBarPrivate::pressTipCharEvent(const QKeyEvent* key)
{
    QTN_P(RibbonBar);
    QString str = key->text().toUpper();
    if (str.isEmpty())
        return false;
    for (int i = 0; i < m_keyTips.count(); i++)
    {
        RibbonKeyTip* keyTip = m_keyTips.at(i);
        int length = keyTip->getStringTip().length();
        if (length > m_countKey)
        {
            if (keyTip->getStringTip()[m_countKey] == str[0])
            {
                if (length - 1 > m_countKey)
                {
                    m_countKey++;

                    QString str = keyTip->getStringTip().left(m_countKey);
                    for (int j = m_keyTips.count() - 1; j >= 0; j--)
                    {
                        keyTip = m_keyTips.at(j);
                        if (keyTip->getStringTip().left(m_countKey) != str)
                        {
                            m_keyTips.removeAt(j);
                            keyTip->hide();
                            keyTip->deleteLater();
                        }
                    }
                }
                else
                {
                    if (QAbstractButton* absButton = qobject_cast<QAbstractButton*>(keyTip->getOwner()))
                    {
                        if (keyTip->isEnabledTip() && keyTip->isVisibleTip())
                        {
                            hideKeyTips();

                            m_keyTipsShowing = false;
                            emit p.keyTipsShowed(false);

                            bool clearLevel = true;
                            bool showMenu = false;
                            if (QToolButton* button = qobject_cast<QToolButton*>(absButton))
                            {
                                if (button->popupMode() == QToolButton::InstantPopup)
                                    clearLevel = false;
                                else if (button->popupMode() == QToolButton::MenuButtonPopup)
                                {
                                    button->showMenu();
                                    clearLevel = false;
                                    showMenu = true;
                                }
                            }
                            if (clearLevel)
                                m_levels.clear();

                            if (!showMenu)
                                absButton->animateClick(0);
                        }
                        return false;
                    }
                    else if (qobject_cast<QMenu*>(keyTip->getOwner()) && keyTip->isEnabledTip() && keyTip->isVisibleTip())
                    {
                        hideKeyTips();

                        m_keyTipsShowing = false;
                        emit p.keyTipsShowed(false);

                        if (QAction* action = keyTip->getAction())
                        {
                            if (qobject_cast<RibbonPageSystemPopup*>(action->menu()))
                            {
                                if (QMenu* activPopup = qobject_cast<RibbonSystemPopupBar*>(QApplication::activePopupWidget()))
                                    activPopup->setActiveAction(action);
                                return false;
                            }
                            else
                            {
                                m_levels.clear();
                                action->trigger();
                                QWidget* owner = keyTip->getOwner();
                                owner->close();

                                if (qobject_cast<RibbonPageSystemPopup*>(owner))
                                {
                                    if (QWidget* activPopup = qobject_cast<RibbonSystemPopupBar*>(QApplication::activePopupWidget()))
                                        activPopup->close();
                                }
                                return false;
                            }
                        }
                    } 
                    else if (qobject_cast<RibbonTab*>(keyTip->getOwner()))
                    {
                        KeyTipEvent kte(keyTip);
                        QApplication::sendEvent(&p, &kte);
                        return false;
                    }
                    else if (keyTip->getOwner() && keyTip->isEnabledTip() && keyTip->isVisibleTip())
                    {
                        hideKeyTips();

                        m_keyTipsShowing = false;
                        emit p.keyTipsShowed(false);

                        m_levels.clear();
                        keyTip->getOwner()->setFocus();
                    }
                }
                return false;
            }
        }
    }
    return false;
}

bool RibbonBarPrivate::eventFilter(QObject* obj, QEvent* event)
{
    QTN_P(RibbonBar)
    switch (event->type()) 
    {
        case QEvent::WindowActivate:
        case QEvent::WindowDeactivate:
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseButtonDblClick:
        case QEvent::NonClientAreaMouseButtonPress :
        case QEvent::NonClientAreaMouseButtonRelease :
        case QEvent::NonClientAreaMouseButtonDblClick :
            {
                m_keyTipsShowing = false;
                emit p.keyTipsShowed(false);

                hideKeyTips();
                m_levels.clear();
            }
            break;
        case QEvent::Show :
            if (qobject_cast<RibbonPageSystemPopup*>(obj))
            {
                hideKeyTips();
                m_levels.clear();
            }
            break;
        case QEvent::KeyPress :
            if (QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event))
            {
                if (keyEvent->key() != Qt::Key_Escape)
                {
                    if ((qobject_cast<QMainWindow*>(obj) || (m_minimized && qobject_cast<RibbonPage*>(obj))))
                        pressTipCharEvent(keyEvent);
                    else if (qobject_cast<QMenu*>(obj))
                        return !pressTipCharEvent(keyEvent);
                }
                else
                {
                    if (m_levels.size() > 1 && m_levels.pop())
                        showKeyTips(m_levels.pop());
                    else if (m_levels.size() > 0 && m_levels.pop())
                    {
                        hideKeyTips();

                        m_keyTipsShowing = false;
                        emit p.keyTipsShowed(false);
                    }
                }
            }
            break;
        case QEvent::Close :
            if (m_minimized)
            {
                if (m_levels.size() > 1 && m_levels.pop())
                    showKeyTips(m_levels.pop());
            }
            break;

        default:
            break;
    }
    return false;
}

void RibbonBarPrivate::processClickedSubControl()
{
    QTN_P(RibbonBar);
    if (QAction* action = qobject_cast<QAction *>(sender()))
    {
        QWidget* widget = p.parentWidget();
        if (widget == Q_NULL )
            return;

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        if (m_frameHelper)
        {
            if (m_actCloseButton == action)
                m_frameHelper->processClickedSubControl(QStyle::SC_TitleBarCloseButton);
            else if (m_actNormalButton == action)
                m_frameHelper->processClickedSubControl(QStyle::SC_TitleBarNormalButton);
            else if (m_actMaxButton == action)
                m_frameHelper->processClickedSubControl(QStyle::SC_TitleBarMaxButton);
            else if (m_actMinButton == action)
                m_frameHelper->processClickedSubControl(QStyle::SC_TitleBarMinButton);
        }
#else
        if (m_actCloseButton == action)
            widget->close();
        else if (m_actNormalButton == action)
            widget->showNormal();
        else if (m_actMaxButton == action)
            widget->showMaximized();
        else if (m_actMinButton == action)
            widget->showMinimized();
#endif
    }
}



/* RibbonBar */
RibbonBar::RibbonBar(QWidget* parent)
    : QMenuBar(parent)
{
    QTN_INIT_PRIVATE(RibbonBar);
    QTN_D(RibbonBar);
    d.init();
}

RibbonBar::~RibbonBar()
{
    QTN_FINI_PRIVATE();
}

/*!
\brief Returns the visible flag of the Ribbon Bar.
*/ 
bool RibbonBar::isVisible() const
{
    QTN_D(const RibbonBar);
    return QMenuBar::isVisible() && d.m_ribbonBarVisible;
}

RibbonPage* RibbonBar::addPage(const QString& text)
{
    return insertPage(-1, text);
}

void RibbonBar::addPage(RibbonPage* page)
{
    insertPage(-1, page);
}

RibbonPage* RibbonBar::insertPage(int index, const QString& text)
{
    QTN_D(RibbonBar);
    d.m_dirtyPages.clear();
    int indTab = d.m_tabBar->insertTab(index, text);

    RibbonPage* page = d.insertPage(indTab, index);
    Q_ASSERT(page != Q_NULL);
    d.updateMinimizedModePage(page);
    return page;
}

void RibbonBar::insertPage(int index, RibbonPage* page)
{
    Q_ASSERT(page != Q_NULL);

    QTN_D(RibbonBar);
    d.m_dirtyPages.clear();
    page->setParent(this);
    int indTab = d.m_tabBar->insertTab(index, page->title());
    page->hide();
    d.insertPage(indTab, index, page);
    d.updateMinimizedModePage(page);
}

void RibbonBar::movePage(RibbonPage* page, int newIndex)
{
    QTN_D(RibbonBar);
    movePage(d.getIndexPage(page), newIndex);
}

void RibbonBar::movePage(int index, int newIndex)
{
    QTN_D(RibbonBar);
    if (d.validIndex(index) && d.validIndex(newIndex))
    {
        d.m_dirtyPages.clear();
        d.m_tabBar->moveTab(index, newIndex);
        d.m_listPages.move(index, newIndex);
        d.layoutRibbon();
    }
}

void RibbonBar::removePage(RibbonPage* page)
{
    QTN_D(RibbonBar);
    int index = d.getIndexPage(page);
    if (index != -1)
        removePage(index);
}

void RibbonBar::removePage(int index)
{
    QTN_D(RibbonBar);
    d.m_dirtyPages.clear();
    d.removePage(index);
    d.m_tabBar->removeTab(index);
    d.layoutRibbon();
}

void RibbonBar::detachPage(RibbonPage* page)
{
    QTN_D(RibbonBar);
    int index = d.getIndexPage(page);
    if (index != -1)
        detachPage(index);
}

void RibbonBar::detachPage(int index)
{
    QTN_D(RibbonBar);
    d.m_dirtyPages.clear();
    d.removePage(index, false);
    d.m_tabBar->removeTab(index);
    d.layoutRibbon();
}

void RibbonBar::clearPages()
{
    QTN_D(RibbonBar);
    d.m_dirtyPages.clear();
    for (int i = getPageCount() - 1; i >= 0; i--)
        removePage(i);
}

/*!
\brief Returns the current state of showing Key-Tips. If true then Key-Tips visible on screen at the moment.
*/ 
bool RibbonBar::isKeyTipsShowing() const
{
    QTN_D(const RibbonBar);
    return d.m_keyTipsShowing;
}

/*!
\brief Returns the flag is mechanizm of Key-Tips enabled or not.
*/ 
bool RibbonBar::keyTipsEnabled() const
{
    QTN_D(const RibbonBar);
    return d.m_keyTipsEnabled;
}

/*!
\brief Enabled or disabled mechanizm of Key-Tips.
*/ 
void RibbonBar::setKeyTipsEnable(bool enable)
{
    QTN_D(RibbonBar);
    d.m_keyTipsEnabled = enable;
}


/*!
\brief Returns the policy of assignment for KeyTips of the buttons and other controls. If return value
is true then the keytips assigned automatically, based on keyboard shortcuts <&File, &Open...>. 
If return value is false keytips will be assigned only if user set it by RibbonBar::setKeyTip() function.
\sa setKeyTip()
*/ 
bool RibbonBar::isKeyTipsComplement() const
{
    QTN_D(const RibbonBar);
    return d.m_keyTipsComplement;
}

/*!
\brief The function sets the policy of assignment for KeyTips of the buttons and other controls. If the parameter \a complement
is true then the keytips assigned automatically, based on keyboard shortcuts <&File, &Open...>. 
If parameter \a complement is false keytips will be assigned only if user set it by RibbonBar::setKeyTip() function.
\sa setKeyTip()
*/ 
void RibbonBar::setKeyTipsComplement(bool complement)
{
    QTN_D(RibbonBar);
    d.m_keyTipsComplement = complement;
}

/*!
\brief Sets the \a keyTip for the control associated with given \a action. Function is taken effect if RibbonBar::isKeyTipsComplement() return true.
\sa setKeyTipsComplement()
*/
void RibbonBar::setKeyTip(QAction* action, const QString& keyTip)
{
    if (!action)
    {
        Q_ASSERT(false);
        return;
    }
    action->setProperty(qtn_Prop_Key_Tip, keyTip);
}

RibbonQuickAccessBar* RibbonBar::getQuickAccessBar() const
{
    QTN_D(const RibbonBar);
    return d.m_quickAccessBar;
}

void RibbonBar::showQuickAccess(bool show)
{
    QTN_D(RibbonBar);
    d.m_quickAccessBar->setVisible(show);
}

bool RibbonBar::isQuickAccessVisible() const
{
    QTN_D(const RibbonBar);
    return d.m_quickAccessBar && d.m_quickAccessBar->isVisible();
}

/*! \internal */
void RibbonBar::setMinimizedFlag(bool flag)
{
    QTN_D(RibbonBar);
    if (d.m_minimized != flag)
    {
        d.m_minimized = flag;

        if (!d.m_minimized && (d.m_tabBar && d.m_tabBar->getTabCount() > 0 && d.m_tabBar->currentIndex() == -1))
            setCurrentPage(0);

        if (d.m_minimized)
        {
            for (int i = 0, count = d.m_listPages.size(); count > i; i++)
                d.updateMinimizedModePage(d.m_listPages.at(i));
        }
        else
        {
            for (int i = 0, count = d.m_listPages.size(); count > i; i++)
                d.updateMinimizedModePage(d.m_listPages.at(i));
            currentChanged(currentIndexPage());
        }

        d.m_tabBar->refresh();

        d.layoutRibbon();
        adjustSize();

        emit minimizationChanged(d.m_minimized);
    }
}

void RibbonBar::minimize()
{
    setMinimizedFlag(true);
}

bool RibbonBar::isMinimized() const
{
    QTN_D(const RibbonBar);
    return d.m_minimized;
}

void RibbonBar::setMinimized(bool flag)
{
    setMinimizedFlag(flag);
}

void RibbonBar::maximize()
{
    setMinimizedFlag(false);
}

bool RibbonBar::isMaximized() const
{
    return !isMinimized();
}

void RibbonBar::setMaximized(bool flag)
{
    setMinimizedFlag(!flag);
}

/*!
\brief Sets the minimization feature of the Ribbon Bar to the \a enabled.
*/ 
void RibbonBar::setMinimizationEnabled(bool enabled)
{
    QTN_D(RibbonBar);
    d.m_minimizationEnabled = enabled;
}

/*!
\brief Returns the enable flag of the minimization feature of the Ribbon Bar.
*/ 
bool RibbonBar::isMinimizationEnabled() const
{
    QTN_D(const RibbonBar);
    return d.m_minimizationEnabled;
}

/*!
\brief Make a page in the position specified by \a index, the current one.
*/ 
void RibbonBar::setCurrentPage(int index)
{
    QTN_D(RibbonBar);
    d.m_tabBar->setCurrentIndex(index);
}

/*!
\brief Returns the index of the current page.
*/ 
int RibbonBar::currentIndexPage() const
{
    QTN_D(const RibbonBar);
    return d.m_tabBar->currentIndex();
}

RibbonPage* RibbonBar::getPage(int index) const
{
    QTN_D(const RibbonBar);
    if (d.validIndex(index))
        return d.m_listPages[index];
    return Q_NULL;
}

int RibbonBar::getPageCount() const
{
    QTN_D(const RibbonBar);
    return d.m_listPages.count();
}

QMenu* RibbonBar::addMenu(const QString& text)
{
    QTN_D(RibbonBar);
    return d.m_tabBar->addMenu(text);
}

QAction* RibbonBar::addAction(const QIcon& icon, const QString& text, Qt::ToolButtonStyle style, QMenu* menu)
{
    QTN_D(RibbonBar);
    QAction* action = d.m_tabBar->addAction(icon, text, style, menu);
    QMenuBar::addAction(action);
    return action;
}

QAction* RibbonBar::addAction(QAction* action, Qt::ToolButtonStyle style)
{
    QTN_D(RibbonBar);
    QMenuBar::addAction(action);
    return d.m_tabBar->addAction(action, style);
}

QAction* RibbonBar::addSystemButton(const QString& text)
{
    return addSystemButton(QIcon(), text);
}

QAction* RibbonBar::addSystemButton(const QIcon& icon, const QString& text)
{
    QTN_D(RibbonBar);
    QAction* actionSystemPopupBar = d.m_controlSystemButton ? 
        d.m_controlSystemButton->defaultAction() : d.createSystemButton(icon, text);

    if (QMenu* menu = actionSystemPopupBar->menu())
        delete menu;

    actionSystemPopupBar->setMenu(new RibbonSystemPopupBar(this));

    return actionSystemPopupBar;
}

RibbonSystemButton* RibbonBar::getSystemButton() const
{
    QTN_D(const RibbonBar);
    return d.m_controlSystemButton;
}

/*!
\brief Returns the visibility of the BackStage view at the moment.
*/ 
bool RibbonBar::isBackstageVisible() const
{
    QTN_D(const RibbonBar);
    if (d.m_controlSystemButton)
    {
        if (RibbonBackstageView* backstage = qobject_cast<RibbonBackstageView*>(d.m_controlSystemButton->backstage()))
            return backstage->isVisible();
    }
    return false;
}

void RibbonBar::setFrameThemeEnabled(bool enable)
{
#ifdef Q_OS_WIN
    QTN_D(RibbonBar);
    if (!enable)
    {
        if (d.m_frameHelper)
        {
            delete d.m_frameHelper;
            d.m_frameHelper = Q_NULL;
        }

        d.addTitleButton(QStyle::SC_TitleBarCloseButton, false, d.m_rcHeader);
        d.addTitleButton(QStyle::SC_TitleBarNormalButton, false, d.m_rcHeader);
        d.addTitleButton(QStyle::SC_TitleBarMaxButton, false, d.m_rcHeader);
        d.addTitleButton(QStyle::SC_TitleBarMinButton, false, d.m_rcHeader);
        adjustSize();
    }
    else if (!d.m_frameHelper)
    {
        d.m_frameHelper = new OfficeFrameHelperWin(this);
        d.m_frameHelper->enableOfficeFrame(this->parentWidget());
        d.m_frameHelper->setContextHeader(&d.m_listContextHeaders);
    }
#else // Q_OS_WIN
    Q_UNUSED(enable)
#endif // Q_OS_WIN
}

bool RibbonBar::isFrameThemeEnabled() const
{
    QTN_D(const RibbonBar);
    return d.m_frameHelper;
}

int RibbonBar::tabBarHeight() const
{
    QTN_D(const RibbonBar);
    return (d.m_tabBar && d.m_tabBar->isVisible()) ? d.m_rcTabBar.height() : 2;
}

bool RibbonBar::isTitleBarVisible() const
{
    QTN_D(const RibbonBar);
    return isFrameThemeEnabled() || d.m_showTitleAlways;
}

void RibbonBar::setTitleBarVisible(bool show)
{
    QTN_D(RibbonBar);
    d.m_showTitleAlways = show;
    d.layoutRibbon();
}

int RibbonBar::titleBarHeight() const
{
    QTN_D(const RibbonBar);
    if (!isFrameThemeEnabled() && !d.m_showTitleAlways)
        return 0;

    int height = style()->pixelMetric(QStyle::PM_TitleBarHeight, 0);

    if (!isQuickAccessVisible() || !d.m_quickAccessOnTop)
        return height + (parentWidget()->windowState() & Qt::WindowMaximized ? topBorder() * 0.5 : topBorder() + 1);

    int quickAccessHeight = getQuickAccessBar()->height();
    quickAccessHeight = qMax(quickAccessHeight, d.m_tabBar->height() - 2);

    if (height >= quickAccessHeight)
        return height;

    return quickAccessHeight;
}

int RibbonBar::topBorder() const
{
    QTN_D(const RibbonBar);
    return d.m_frameHelper && d.m_frameHelper->isDwmEnabled() ? d.m_frameHelper->getFrameBorder() : 0;
}

void RibbonBar::currentChanged(int index)
{
    QTN_D(RibbonBar);
    d.currentChanged(index);
}

/*! \reimp */
bool RibbonBar::event(QEvent* event)
{
    QTN_D(RibbonBar);

    bool result = QMenuBar::event(event);

    switch(event->type())
    {
        case QEvent::LayoutRequest :
            {
                if (!d.m_wheelEvent)
                    d.layoutRibbon();
                else
                    d.m_wheelEvent = false;
            }
            break;
        case QEvent::WindowTitleChange :
            {
                d.m_windowTitle = parentWidget() ? parentWidget()->windowTitle() : QString();
                QApplication::postEvent(this, new QEvent(QEvent::LayoutRequest));
            }
            break;
        case QEvent::WindowIconChange:
            if (d.m_frameHelper)
                d.m_frameHelper->resetWindowIcon();
            break;
        case QEvent::ActionRemoved :
        case QEvent::ActionChanged :
                QApplication::sendEvent(d.m_tabBar, event);
            break;
        case QEvent::Show :
        case QEvent::Hide :
            {
                bool show = event->type() == QEvent::Show;
                if (isFrameThemeEnabled() && show != d.m_ribbonBarVisible)
                {
                    d.m_ribbonBarVisible = show;
                    d.m_frameHelper->recalcFrameLayout();
                }
            }
            break;
        default:
            break;
    }

    if (event->type() == KeyTipEvent::eventNumber())
    {
        KeyTipEvent* keyTipEvent = static_cast<KeyTipEvent*>(event);
        if (RibbonTab* tab = qobject_cast<RibbonTab*>(keyTipEvent->getKeyTip()->getOwner()))
        {
            int index = d.m_tabBar->getIndex(tab);
            if (index != -1)
            {
                setCurrentPage(index);
                d.showKeyTips(getPage(index));
            }
        }
    }
    else if (event->type() == ShowKeyTipEvent::eventNumber())
    {
        ShowKeyTipEvent* showKeyTipEvent = static_cast<ShowKeyTipEvent*>(event);
        if (QWidget* widget = showKeyTipEvent->getWidget())
        {
            if (d.m_levels.indexOf(widget) == -1)
            {
                d.m_keyTipsShowing = true;
                emit keyTipsShowed(true);

                d.showKeyTips(widget);
            }
        }
    }
    else if (event->type() == HideKeyTipEvent::eventNumber())
    {
        d.hideKeyTips();
        d.m_levels.clear();
    }
    else if (event->type() == MinimizedEvent::eventNumber())
    {
        setMinimizedFlag(!isMinimized());
    }

    return result;
}

/*! \reimp */
bool RibbonBar::eventFilter(QObject* object, QEvent* event)
{
    QTN_D(RibbonBar);
    const QEvent::Type type = event->type();

    if (type == QEvent::Resize)
    {
        if (object == parentWidget() && !isVisible())
        {
            d.layoutRibbon();
            return true;
        }
    } 

    return QMenuBar::eventFilter(object, event);
}

/*! \reimp */
void RibbonBar::paintEvent(QPaintEvent* event)
{
    QTN_D(RibbonBar);

    QWidget* leftCorner = cornerWidget(Qt::TopLeftCorner);
    if(leftCorner && leftCorner->isVisible())
        leftCorner->setVisible(false);

    // draw ribbon
    QPainter p(this);
    QRect rectEvent = event->rect();
    p.setClipRect(rectEvent);

    StyleOptionRibbon opt;
    opt.init(this);
    opt.frameHelper = d.m_frameHelper;
    opt.rectTabBar = d.m_rcTabBar;
    opt.minimized = d.m_minimized;
    opt.titleBarHeight = titleBarHeight();
    opt.airRegion = event->region();

    QStyle* ribbonStyle = style();
    if (rectEvent.intersects(opt.rect))
        ribbonStyle->drawControl((QStyle::ControlElement)RibbonStyle::CE_RibbonBar, &opt, &p, this);

    if ((isFrameThemeEnabled() || d.m_showTitleAlways) &&  d.m_rcTitle.isValid())
    {
        StyleOptionTitleBar titleBarOptions;
        d.titleBarOptions(titleBarOptions);
        titleBarOptions.frameHelper = d.m_frameHelper;
        titleBarOptions.rectTabBar = d.m_rcTabBar;
        titleBarOptions.rcRibbonClient = rect();
        titleBarOptions.listContextHeaders = &d.m_listContextHeaders;
        titleBarOptions.airRegion = event->region();

        if (rectEvent.intersects(titleBarOptions.rect))
        {
            QFont saveFont = p.font();
            const QFont font = QApplication::font("QMdiSubWindowTitleBar");
            p.setFont(font);
            ribbonStyle->drawComplexControl(QStyle::CC_TitleBar, &titleBarOptions, &p, this);
            p.setFont(saveFont);
        }
    }

    opt.rect = d.m_rcPageArea;
    if (rectEvent.intersects(opt.rect))
        ribbonStyle->drawControl((QStyle::ControlElement)RibbonStyle::CE_RibbonGroups, &opt, &p, this);
}

/*! \reimp */
void RibbonBar::changeEvent(QEvent* event)
{
    QTN_D(RibbonBar);
    switch (event->type()) 
    {
        case QEvent::FontChange: 
            {
                for (int i = 0, count = d.m_listPages.count(); count > i; i++)
                {
                    RibbonPage* page = d.m_listPages[i];
                    page->setFont(font());
                    QEvent ev(QEvent::LayoutRequest);
                    QApplication::sendEvent(page, &ev);
                }
                QEvent ev(QEvent::LayoutRequest);
                QApplication::sendEvent(this, &ev);
                updateGeometry();
            }
            break;
        default:
            break;
    };
    return QMenuBar::changeEvent(event);
}

/*! \reimp */
void RibbonBar::resizeEvent(QResizeEvent* event)
{
    QTN_D(RibbonBar);

    QSize delta = event->size() - event->oldSize();
    if (delta.isNull())
        return;

    d.layoutCorner();

    d.m_dirtyPages.clear();
    d.layoutRibbon();
    updateGeometry();
}

/*! \reimp */
void RibbonBar::mouseDoubleClickEvent(QMouseEvent* event)
{
    QMenuBar::mouseDoubleClickEvent(event);

    if (event->button() != Qt::LeftButton)
        return;

    QTN_D(RibbonBar);
    if (isFrameThemeEnabled() && isTitleBarVisible())
    {
        if (d.hitTestContextHeaders(event->pos()))
        {
            QWidget* widget = parentWidget();
            if (widget == Q_NULL )
                return;

            StyleOptionTitleBar titleBarOpt;
            d.titleBarOptions(titleBarOpt);

            bool buttonMax =  titleBarOpt.titleBarFlags & Qt::WindowMaximizeButtonHint
                && !(titleBarOpt.titleBarState & Qt::WindowMaximized); 

            bool buttonNormal = (((titleBarOpt.titleBarFlags & Qt::WindowMinimizeButtonHint)
                && (titleBarOpt.titleBarState & Qt::WindowMinimized))
                || ((titleBarOpt.titleBarFlags & Qt::WindowMaximizeButtonHint)
                && (titleBarOpt.titleBarState & Qt::WindowMaximized)));

//            Qt::WindowStates titleBarState = widget->windowState();

            if (buttonMax)
                widget->showMaximized();
            else if (buttonNormal)
                widget->showNormal();

            QSize s = widget->size();
            if (s.isValid())
                widget->resize(s);

            return;
        }
    }
    if (d.m_minimizationEnabled && d.m_tabBar->getTab(d.m_tabBar->currentIndex()) == childAt(event->pos()))
        QApplication::postEvent(this, new MinimizedEvent());
/*
    if (d.m_minimizationEnabled && d.m_tabBar->getTab(d.m_tabBar->currentIndex()) == childAt(event->pos()))
    {
        if (isMinimized())
            setMinimizedFlag(false);
        else
            setMinimizedFlag(true);
    }
*/
}

/*! \reimp */
void RibbonBar::mousePressEvent(QMouseEvent* event)
{
    // block the function call QMenuBar
    QWidget::mousePressEvent(event);

    QTN_D(RibbonBar);
    if (event->button() == Qt::LeftButton)
    {
        if (ContextHeader* header = d.hitTestContextHeaders(event->pos()))
        {
            for (int i = 0, count = d.m_tabBar->getTabCount(); count > i; i++)
            {
                if (RibbonTab* tab = d.m_tabBar->getTab(i))
                {
                    if (tab == header->firstTab)
                    {
                        setCurrentPage(i);
                        break;
                    }
                }
            }
        }
    }
}

/*! \reimp */
void RibbonBar::wheelEvent(QWheelEvent* event)
{
    QTN_D(RibbonBar);
    d.m_wheelEvent = true;
    if (!isMaximized())
        return;

    QPoint pos = event->pos();

    if (!geometry().contains(pos))
        return;

    d.m_tabBar->currentNextTab(event->delta() < 0);
}

int RibbonBar::heightForWidth(int) const
{
    ensurePolished();
    QTN_D(const RibbonBar);

    const int tabsHeight = d.m_rcTabBar.height();
    const int pageAreaHeight = d.m_rcPageArea.height();
    const int ribbonTopBorder = d.m_frameHelper && d.m_frameHelper->isDwmEnabled() ? d.m_frameHelper->getFrameBorder() : 0;
    const int heightTitle = d.m_frameHelper ? titleBarHeight() : d.m_rcTitle.height();

    int height = tabsHeight + pageAreaHeight + heightTitle + ribbonTopBorder;
    if (d.m_minimized)
        height += 3;

    if (d.m_ribbonBarVisible)
        return height;

    return 0;
}

QSize RibbonBar::sizeHint() const
{
    return QSize(rect().width(), heightForWidth(0)).expandedTo(QApplication::globalStrut());
}

#ifdef Q_OS_WIN
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
bool RibbonBar::nativeEvent(const QByteArray& eventType, void* message, long* result)
#else
bool RibbonBar::winEvent(MSG* message, long* result)
#endif
{
    QTN_D(RibbonBar);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    if (d.m_frameHelper)
    {
        MSG* msg = static_cast<MSG *>(message);
        if (d.m_frameHelper->winEvent(msg, result))
            return true;
    }

    return QMenuBar::nativeEvent(eventType, message, result);
#else
    if (d.m_frameHelper && d.m_frameHelper->winEvent(message, result))
        return true;

    return QMenuBar::winEvent(message, result);
#endif
}
#endif // Q_OS_WIN
