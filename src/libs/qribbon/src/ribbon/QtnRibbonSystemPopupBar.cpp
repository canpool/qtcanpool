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
#include <QPaintEvent>
#include <QPainter>
#include <QStyleOption>
#include <QFileInfo>
#include <QColor>
#include <QKeySequence>
#include <qevent.h>

#include "QtnRibbonSystemPopupBar.h"
#include "QtnRibbonBackstageView.h"
#include "QtnRibbonBar.h"
#include "QtnRibbonButton.h"
#include "QtnStyleHelpers.h"
#include "QtnRibbonStyle.h"

QTITAN_USE_NAMESPACE

static const int splitActionPopupWidth  = 20; // Split button drop down width in popups

QTITAN_BEGIN_NAMESPACE

/* QtnEventParent */
class QtnEventParent : public QEvent
{
public:
    QtnEventParent(QWidget* parent) : QEvent(QEvent::ParentChange) { m_parent = parent; }
    QWidget* m_parent;
};


/* RibbonSystemButtonPrivate */
class RibbonSystemButtonPrivate: public QObject
{
public:
    QTN_DECLARE_PUBLIC(RibbonSystemButton)

public:
    explicit RibbonSystemButtonPrivate() { m_widget = Q_NULL; m_colorBackground = QColor(QColor::Invalid); }

public:
    QWidget* m_widget;
    QColor m_colorBackground;
};

/* SysPopupRibbonButton */
class SysPopupRibbonButton : public QToolButton
{
public:
    SysPopupRibbonButton(QWidget* parent);
    virtual ~SysPopupRibbonButton();

protected:
    virtual bool event(QEvent* event);
    virtual void paintEvent(QPaintEvent* event);

private:
    Q_DISABLE_COPY(SysPopupRibbonButton);
};

QTITAN_END_NAMESPACE


/* RibbonSystemButton */
RibbonSystemButton::RibbonSystemButton(QWidget* parent)
    : QToolButton(parent)
{
    QTN_INIT_PRIVATE(RibbonSystemButton);
}

RibbonSystemButton::~RibbonSystemButton()
{
    QTN_FINI_PRIVATE();
}

void RibbonSystemButton::setBackstage(RibbonBackstageView* backstage)
{
    Q_ASSERT(backstage != Q_NULL);
    if (!backstage)
        return; 

    QTN_D(RibbonSystemButton);
    if (QAction* action = defaultAction())
    {
        d.m_widget = Q_NULL;
        if (QMenu* menu = action->menu())
        {
            action->setMenu(Q_NULL);
            delete menu;
        }

        d.m_widget = backstage;
        connect(action, SIGNAL(triggered()), backstage, SLOT(open()));
    }
}

RibbonBackstageView* RibbonSystemButton::backstage() const
{
    QTN_D(const RibbonSystemButton);
    return qobject_cast<RibbonBackstageView*>(d.m_widget);
}

void RibbonSystemButton::setBackgroundColor(const QColor& color)
{
    QTN_D(RibbonSystemButton);
    d.m_colorBackground = color;
    repaint();
}

QColor RibbonSystemButton::backgroundColor() const
{
    QTN_D(const RibbonSystemButton);
    return d.m_colorBackground;
}

/*! \reimp */
QSize RibbonSystemButton::sizeHint() const
{
    QSize sz = QToolButton::sizeHint();

    const int heightTabs = style()->pixelMetric((QStyle::PixelMetric)RibbonStyle::PM_RibbonTabsHeight, 0, 0) - 2;
    return QSize(sz.width() + 24, heightTabs).expandedTo(QApplication::globalStrut());
}

/*! \reimp */
void RibbonSystemButton::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QTN_D(RibbonSystemButton);
    QPainter p(this);

#ifdef Q_OS_WIN
    Qt::ToolButtonStyle styleButton = toolButtonStyle();
#endif // Q_OS_WIN

    StyleSystemToolButton opt;
    initStyleOption(&opt);
    opt.colorBackground = d.m_colorBackground;

#ifdef Q_OS_WIN
    opt.toolButtonStyle = styleButton;
#endif // Q_OS_WIN

    QRect rc = opt.rect;
    if (opt.toolButtonStyle == Qt::ToolButtonFollowStyle)
        rc.adjust(2, 2, -2, -2);

    int actualArea = rc.height()*rc.width();

    int index = -1;
    int curArea = 0;
    QList<QSize> lSz = opt.icon.availableSizes();
    for (int i = 0, count = lSz.count(); count > i; i++)
    {
        QSize curSz = lSz[i];
        int area = curSz.height()*curSz.width();
        if (actualArea > area)
        {
            if (area > curArea)
                index = i;
            curArea = area;
        }
    }

    opt.iconSize = index != -1 ?  opt.icon.actualSize(lSz[index]) : opt.icon.actualSize(opt.rect.adjusted(0, 2, 0, -2).size());
    // draw pixmap ribbonFileButton
    style()->drawPrimitive((QStyle::PrimitiveElement)RibbonStyle::PE_RibbonFileButton, &opt, &p, this);

    // draw text
    if (opt.toolButtonStyle != Qt::ToolButtonFollowStyle && opt.toolButtonStyle != Qt::ToolButtonIconOnly)
        style()->drawControl((QStyle::ControlElement)RibbonStyle::CE_RibbonFileButtonLabel, &opt, &p, this);
}

/*! \reimp */
void RibbonSystemButton::mousePressEvent(QMouseEvent* event)
{
    QTN_D(RibbonSystemButton);
    if (RibbonBackstageView* backstage = qobject_cast<RibbonBackstageView*>(d.m_widget))
    {
        if (backstage->isVisible())
        {
            backstage->close();
            return; 
        }
    }
    QToolButton::mousePressEvent(event);
}


/* SysPopupRibbonButton */
SysPopupRibbonButton::SysPopupRibbonButton(QWidget* parent)
    : QToolButton(parent)
{
}

SysPopupRibbonButton::~SysPopupRibbonButton()
{
}

/*! \reimp */
bool SysPopupRibbonButton::event(QEvent* event)
{
    if (event->type() == QEvent::ToolTip)
        event->setAccepted(false);
    return QToolButton::event(event);
}

/*! \reimp */
void SysPopupRibbonButton::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    {
        QPainter p(this);
        QStyleOptionToolButton opt;
        initStyleOption(&opt);
        opt.iconSize = opt.icon.actualSize(QSize(QWIDGETSIZE_MAX,QWIDGETSIZE_MAX));
        style()->drawPrimitive((QStyle::PrimitiveElement)RibbonStyle::PE_RibbonPopupBarButton, &opt, &p, this);
    } //QToolButton::paintEvent() will create the second painter, so we need to destroy the first painter.
    QToolButton::paintEvent(event);
}

QTITAN_BEGIN_NAMESPACE
/* RibbonSystemPopupBarPrivate */
class RibbonSystemPopupBarPrivate : public QObject
{
public:
    QTN_DECLARE_PUBLIC(RibbonSystemPopupBar)

public:
    explicit RibbonSystemPopupBarPrivate();
public:
    QRect m_rcBorders;
    QList<SysPopupRibbonButton*>  m_systemButtonList;
    QList<QWidget*>  m_pageList;
};
QTITAN_END_NAMESPACE

RibbonSystemPopupBarPrivate::RibbonSystemPopupBarPrivate()
{
    m_rcBorders = QRect(QPoint(6, 18), QPoint(6, 29));
}


/* RibbonSystemPopupBar */
RibbonSystemPopupBar::RibbonSystemPopupBar(QWidget* parent)
    : QMenu(parent)
{
    QTN_INIT_PRIVATE(RibbonSystemPopupBar);
}

RibbonSystemPopupBar::~RibbonSystemPopupBar()
{
    QTN_FINI_PRIVATE();
}

QAction* RibbonSystemPopupBar::addPopupBarAction(const QString& text)
{
    QTN_D(RibbonSystemPopupBar);
    SysPopupRibbonButton* systemPopupBarButton = new SysPopupRibbonButton(this);
    systemPopupBarButton->setAutoRaise(true);
    systemPopupBarButton->setToolButtonStyle(Qt::ToolButtonTextOnly);

    d.m_systemButtonList.append(systemPopupBarButton);
    systemPopupBarButton->setText(text);

    QAction* pAction = new QAction(text, systemPopupBarButton);
    systemPopupBarButton->setDefaultAction(pAction);
    return pAction;
}

void RibbonSystemPopupBar::addPopupBarAction(QAction* pAction, Qt::ToolButtonStyle style)
{
    QTN_D(RibbonSystemPopupBar);
    if (style == Qt::ToolButtonTextUnderIcon)
    {
        Q_ASSERT_X(false, Q_FUNC_INFO, "Invalid style button");
        return;
    }

    SysPopupRibbonButton* systemPopupBarButton = new SysPopupRibbonButton(this);
    systemPopupBarButton->setAutoRaise(true);
    systemPopupBarButton->setToolButtonStyle(style);

    d.m_systemButtonList.append(systemPopupBarButton);
    systemPopupBarButton->setText(pAction->text());

    systemPopupBarButton->setDefaultAction(pAction);
}

RibbonPageSystemRecentFileList* RibbonSystemPopupBar::addPageRecentFile(const QString& caption)
{
    RibbonPageSystemRecentFileList* recentFileList = new RibbonPageSystemRecentFileList(caption);
    recentFileList->setParent(this);
    QTN_D(RibbonSystemPopupBar);
    d.m_pageList.append(recentFileList);
    adjustSize();
    return recentFileList;
}

RibbonPageSystemPopup* RibbonSystemPopupBar::addPageSystemPopup(const QString& caption, QAction* defaultAction, bool splitAction)
{
    RibbonPageSystemPopup* pageSystemPopupBar = new RibbonPageSystemPopup(caption);
    pageSystemPopupBar->setDefaultAction(defaultAction);
    if (splitAction)
        defaultAction->setObjectName(QString(qtn_SplitActionPopup));
    defaultAction->setMenu(pageSystemPopupBar);

    QtnEventParent e(this);
    QApplication::sendEvent(pageSystemPopupBar, &e);

    return pageSystemPopupBar;
}

/*! \reimp */
QSize RibbonSystemPopupBar::sizeHint() const
{
    QTN_D(const RibbonSystemPopupBar);

    QSize sz = QMenu::sizeHint();
    sz.setHeight(sz.height() + 12);

    if (d.m_pageList.size() > 0)
    {
        int nMruAreaHeight = d.m_pageList[0]->sizeHint().height() + 45;
        if (nMruAreaHeight > sz.height())
            sz.setHeight(nMruAreaHeight);
        sz.setWidth(sz.width() + d.m_pageList[0]->sizeHint().width() + d.m_rcBorders.right()/2 - 1);
    }
    return sz;
}

/*! \reimp */
void RibbonSystemPopupBar::showEvent(QShowEvent* event)
{
    QMenu::showEvent(event);

    if (RibbonBar* parent = qobject_cast<RibbonBar*>(parentWidget()))
    {
        if (QToolButton* button = parent->getSystemButton())
        {
            if (button->toolButtonStyle() == Qt::ToolButtonFollowStyle)
            {
                QRect rectButton =  button->rect();
                if (rectButton.isValid())
                {
                    QPoint posButton = button->mapToGlobal(rectButton.topLeft());
                    QRect rectMenu = geometry();
                    if (rectMenu.isValid())
                    {
                        QPoint posMenu = rectMenu.topLeft();
                        if (posButton.y() < posMenu.y())
                        {
                            // set new menu position
                            QPoint posParent = parent->mapToGlobal(parent->pos());
                            move(QPoint(posMenu.x(), posParent.y() + parent->titleBarHeight() + parent->topBorder()));
                        }
                    }
                }
            }
        }
    }
}

/*! \reimp */
void RibbonSystemPopupBar::mousePressEvent(QMouseEvent* event)
{
    if (RibbonBar* parent = qobject_cast<RibbonBar*>(parentWidget()))
    {
        if (QToolButton* button = parent->getSystemButton())
        {
            if (button->toolButtonStyle() == Qt::ToolButtonFollowStyle)
            {
                QRect rectButton = button->rect();
                if (rectButton.isValid())
                {
                    QPoint posButton = button->mapToGlobal(rectButton.topLeft());
                    if (QRect(posButton, button->size()).contains(event->globalPos()))
                    {
                        QMouseEvent ev(QEvent::MouseButtonPress, QPoint(-1, -1), event->button(), event->buttons(), event->modifiers());
                        QMenu::mousePressEvent(&ev);
                        return;
                    }
                }
            }
        }
    }
    QMenu::mousePressEvent(event);
}

/*! \reimp */
void RibbonSystemPopupBar::mouseReleaseEvent(QMouseEvent* event)
{
    QAction* action = actionAt(event->pos());
    if (action && (action->objectName() == QString(qtn_SplitActionPopup)))
    {
        QRect rect = actionGeometry(action);
        rect.adjust(0, 0, -splitActionPopupWidth, 0);
        if (rect.contains(event->pos()))
        {
            QMenu::mousePressEvent(event);
            action->trigger();
        }
    }
    else
        QMenu::mouseReleaseEvent(event);
}

/*! \reimp */
void RibbonSystemPopupBar::paintEvent(QPaintEvent* event)
{
    QTN_D(RibbonSystemPopupBar);

    QPainter p(this);

    QRegion emptyArea = QRegion(rect());
    QStyleOptionMenuItem menuOpt;
    menuOpt.initFrom(this);
    menuOpt.state = QStyle::State_None;
    menuOpt.checkType = QStyleOptionMenuItem::NotCheckable;
    menuOpt.maxIconWidth = 0;
    menuOpt.tabWidth = 0;
    style()->drawPrimitive( QStyle::PE_FrameMenu, &menuOpt, &p, this);
    
    QList<QAction*> actionList = actions();
    for (int i = 0; i < actionList.count(); ++i)
    {
        QAction* action = actionList.at( i );
        QRect adjustedActionRect = actionGeometry(action);

        if (!event->rect().intersects(adjustedActionRect))
            continue;

        QStyleOptionMenuItem opt;
        initStyleOption(&opt, action);
        opt.rect = adjustedActionRect;

        if (action->objectName() == QString(qtn_SplitActionPopup))
        {
            opt.text = QString(qtn_SplitActionPopup) + opt.text;

            QPoint pnt = mapFromGlobal(QCursor::pos()) ;
            if ( opt.rect.adjusted(0, 0, -splitActionPopupWidth, 0).contains(pnt) )
                opt.checkType = QStyleOptionMenuItem::Exclusive;
        }
        style()->drawControl(QStyle::CE_MenuItem, &opt, &p, this);
    }

    int hMargin = style()->pixelMetric(QStyle::PM_MenuHMargin, Q_NULL, this)*2;
    int count = d.m_pageList.size();
    if (count > 0)
    {
        QSize sz = QMenu::sizeHint();
        StyleHintReturnThemeColor hintReturn(tr("Ribbon"), tr("MenuPopupSeparator"), QColor(197, 197, 197));
        const QColor fillColor = static_cast<QRgb>(style()->styleHint(QStyle::SH_CustomBase, Q_NULL, Q_NULL, &hintReturn));

        QSize szConsiderMRU = this->sizeHint();
        if (szConsiderMRU.height() + 30 > sz.height())
        {
            sz.setHeight(szConsiderMRU.height());
            p.fillRect(QRect(QPoint(sz.width() + d.m_rcBorders.left() - hMargin, d.m_rcBorders.top()),
                     QSize(1, sz.height() - d.m_rcBorders.bottom() - d.m_rcBorders.top()/2+2 - 11)), fillColor);
        }
        else
        {
            p.fillRect(QRect(QPoint(sz.width() + d.m_rcBorders.left() - hMargin, d.m_rcBorders.top()),
                             QSize(1, sz.height() - d.m_rcBorders.top() - d.m_rcBorders.bottom()/2-2)), fillColor);
        }
    }

    for (int i = 0; i < count; i++)
    {
        if (RibbonPageSystemPopupListCaption* page = qobject_cast<RibbonPageSystemPopupListCaption*>(d.m_pageList.at(i)))
        {
            StyleHintReturnThemeColor hintReturn(tr("Ribbon"), tr("RecentFileListBackground"));
            const QColor fillColor = static_cast<QRgb>(style()->styleHint(QStyle::SH_CustomBase, Q_NULL, Q_NULL, &hintReturn));
            p.fillRect(page->geometry(), fillColor);
            break;
        }
    }

    if (RibbonBar* parent = qobject_cast<RibbonBar*>(parentWidget()))
    {
        if (QToolButton* button = parent->getSystemButton())
        {
            if (button->toolButtonStyle() == Qt::ToolButtonFollowStyle)
            {
                QStyleOptionToolButton opt;
                opt.state |= QStyle::State_Enabled | QStyle::State_MouseOver | QStyle::State_Sunken;
                opt.activeSubControls |= QStyle::SC_ToolButtonMenu;
                opt.toolButtonStyle = Qt::ToolButtonFollowStyle;
                QRect rc = button->rect();
                QPoint posButton = button->mapToGlobal(rc.topLeft());
                posButton = mapFromGlobal(posButton);
                rc.translate(posButton);
                opt.rect = rc;
                opt.icon = button->icon();
                opt.iconSize = opt.icon.actualSize(QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX));
                style()->drawPrimitive((QStyle::PrimitiveElement)RibbonStyle::PE_RibbonFileButton, &opt, &p, this);
            }
        }
    }
}

/*! \reimp */
void RibbonSystemPopupBar::keyPressEvent(QKeyEvent* event)
{
    QMenu::keyPressEvent(event);

    if (!event->isAccepted())
    {
        QTN_D(RibbonSystemPopupBar);
        for (int i = 0; i < d.m_pageList.count(); i++)
        {
            if (RibbonPageSystemPopupListCaption* page = qobject_cast<RibbonPageSystemPopupListCaption*>(d.m_pageList.at(i)))
            {
                page->keyPressEvent(event);
                break;
            }
        }
    }
}

/*! \reimp */
void RibbonSystemPopupBar::resizeEvent(QResizeEvent* event)
{
    QMenu::resizeEvent(event);

    QTN_D(RibbonSystemPopupBar);

    QSize sz(rect().size());

    int nRight = sz.width() - d.m_rcBorders.right() + 1;

    for (int i = 0; i < d.m_systemButtonList.count(); ++i) 
    {
        SysPopupRibbonButton* barButton = d.m_systemButtonList.at(i);

        QSize szControl = barButton->sizeHint();
        QRect rc( QPoint(nRight - szControl.width(), sz.height() - d.m_rcBorders.bottom() + 4), QPoint(nRight, sz.height() - 3));
        barButton->move( rc.left(), rc.top() );
        barButton->resize( rc.width(), rc.height() );

        nRight -= szControl.width() + 6;
    }

    for (int i = 0; i < d.m_pageList.size(); i++)
    {
        if (RibbonPageSystemPopupListCaption* page = qobject_cast<RibbonPageSystemPopupListCaption*>(d.m_pageList.at(i)))
        {
            QSize sz = QMenu::sizeHint();
            QSize szConsiderMRU = this->sizeHint();
            int hMargin = style()->pixelMetric(QStyle::PM_MenuHMargin, Q_NULL, this)*2;
            if (szConsiderMRU.height() + 30 > sz.height())
            {
                sz.setHeight(szConsiderMRU.height());
                QRect rc(QPoint(sz.width() + d.m_rcBorders.left() - (hMargin-1), d.m_rcBorders.top()),
                         QSize(page->sizeHint().width(), sz.height() - d.m_rcBorders.bottom() - d.m_rcBorders.top()/2+2 - 11));
                page->setGeometry(rc);
            }
            else
            {
                QRect rc(QPoint(sz.width() + d.m_rcBorders.left() - (hMargin-1), d.m_rcBorders.top()),
                         QSize(page->sizeHint().width(), sz.height() - d.m_rcBorders.bottom() - d.m_rcBorders.top()/2+2));
                page->setGeometry(rc);
            }

            break;
        }
    }
}



/* RibbonPageSystemPopupListCaption */
RibbonPageSystemPopupListCaption::RibbonPageSystemPopupListCaption(const QString& caption)
{
    setWindowTitle(caption);
}

RibbonPageSystemPopupListCaption::~RibbonPageSystemPopupListCaption()
{
}

/*! \reimp */
QSize RibbonPageSystemPopupListCaption::sizeHint() const
{
    return QSize(300, 27);
}

void RibbonPageSystemPopupListCaption::drawEvent(QPaintEvent* event, QPainter* p)
{
    Q_UNUSED(event);
    Q_UNUSED(p);
}

/*! \reimp */
void RibbonPageSystemPopupListCaption::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter p(this);
    QStyleOption opt;
    opt.init(this);

    QRect rc = opt.rect;
    QRect rcText(rc.left() + 7, rc.top() + 4, rc.right(), rc.bottom() - 5);

    QFont fontSave = p.font();

    QFont font = fontSave;
    font.setBold(true);
    p.setFont(font);

    StyleHintReturnThemeColor hintClientText(tr("Ribbon"), tr("GroupClientText"));
    const QColor colorClientText = static_cast<QRgb>(style()->styleHint(QStyle::SH_CustomBase, Q_NULL, Q_NULL, &hintClientText));
    opt.palette.setColor(QPalette::WindowText, colorClientText);
    uint alignment = Qt::AlignTop | Qt::TextSingleLine;
    style()->drawItemText(&p, rcText, alignment, opt.palette, true, windowTitle(), QPalette::WindowText);

    p.setFont(fontSave);

    StyleHintReturnThemeColor hintEdgeShadow(tr("Ribbon"), tr("RecentFileListEdgeShadow"));
    const QColor fillEdgeShadow = static_cast<QRgb>(style()->styleHint(QStyle::SH_CustomBase, Q_NULL, Q_NULL, &hintEdgeShadow));

    p.fillRect(QRect(QPoint(rc.left(), rc.top() + sizeHint().height() - 6), QSize(rc.width(), 1)), fillEdgeShadow);

    StyleHintReturnThemeColor hintEdgeHighLight(tr("Ribbon"), tr("RecentFileListEdgeHighLight"));
    const QColor edgeHighLightColor = static_cast<QRgb>(style()->styleHint(QStyle::SH_CustomBase, Q_NULL, Q_NULL, &hintEdgeHighLight));
    p.fillRect(QRect(QPoint(rc.left(), rc.top() + sizeHint().height() - 5 ), QSize(rc.width(), 1)), edgeHighLightColor);

    drawEvent(event, &p);
}


QTITAN_BEGIN_NAMESPACE
/* RibbonPageSystemRecentFileListPrivate */
class RibbonPageSystemRecentFileListPrivate : public QObject
{
public:
    QTN_DECLARE_PUBLIC(RibbonPageSystemRecentFileList)
public:
    explicit RibbonPageSystemRecentFileListPrivate();

public:
    void updateActionRects() const;
    void initStyleOption(QStyleOptionMenuItem *option, const QAction *action) const;
    QAction* actionAt(const QPoint& p) const;
    QRect actionRect(QAction* act) const;
    void setCurrentAction(QAction* currentAction);
    void activateAction(QAction* action, QAction::ActionEvent action_e, bool self = true);

public:
    bool m_itemsDirty;
    bool m_mouseDown;
    QAction* m_currentAction;
    QList<QAction*> m_recentFileActs; // contents of the MRU list

    mutable bool m_hasCheckableItems;
    mutable QVector<QRect> m_actionRects;
};
QTITAN_END_NAMESPACE

RibbonPageSystemRecentFileListPrivate::RibbonPageSystemRecentFileListPrivate()
{
    m_itemsDirty = true;
    m_mouseDown = false;
    m_hasCheckableItems = false;
    m_currentAction = Q_NULL;
}

QAction* RibbonPageSystemRecentFileListPrivate::actionAt(const QPoint& pt) const
{
    QTN_P(const RibbonPageSystemRecentFileList);

    if (!p.rect().contains(pt)) //sanity check
        return 0;

    for(int i = 0; i < m_actionRects.count(); i++)
    {
        if (m_actionRects.at(i).contains(pt))
            return m_recentFileActs.at(i);
    }
    return 0;
}

QRect RibbonPageSystemRecentFileListPrivate::actionRect(QAction* act) const
{
    int index = m_recentFileActs.indexOf(act);
    if (index == -1)
        return QRect();

    updateActionRects();

    //we found the action
    return m_actionRects.at(index);
}

void RibbonPageSystemRecentFileListPrivate::setCurrentAction(QAction* currentAction)
{
    QTN_P(RibbonPageSystemRecentFileList);

    if (m_currentAction && m_currentAction != currentAction)
        p.update(actionRect(m_currentAction));

    if (m_currentAction == currentAction)
        return;
    m_currentAction = currentAction;

    p.update(actionRect(m_currentAction));
}

void RibbonPageSystemRecentFileListPrivate::activateAction(QAction* action, QAction::ActionEvent action_e, bool self)
{
    Q_UNUSED(self);
    Q_UNUSED(action_e);

    QTN_P(RibbonPageSystemRecentFileList);
    p.parentWidget()->hide();

//    action->activate(action_e);
    emit p.openRecentFile(action->data().toString());
}

void RibbonPageSystemRecentFileListPrivate::updateActionRects() const
{
    QTN_P(const RibbonPageSystemRecentFileList);
    if (!m_itemsDirty)
        return;

    p.ensurePolished();

    m_actionRects.resize(m_recentFileActs.count());
    m_actionRects.fill(QRect());

    //let's try to get the last visible action
    int lastVisibleAction = m_recentFileActs.count() - 1;
    for(;lastVisibleAction >= 0; --lastVisibleAction) 
    {
        const QAction *action = m_recentFileActs.at(lastVisibleAction);
        if (action->isVisible())
            break;
    }

    QStyle* style = p.style();
    QStyleOption opt;
    opt.init(&p);

    const int hmargin = style->pixelMetric(QStyle::PM_MenuHMargin, &opt, &p),
        vmargin = style->pixelMetric(QStyle::PM_MenuVMargin, &opt, &p);

    const int fw = style->pixelMetric(QStyle::PM_MenuPanelWidth, &opt, &p);
    int max_column_width = p.sizeHint().width() - vmargin - fw; 
    int y = p.sizeHint().height();

    //calculate size
    QFontMetrics qfm = p.fontMetrics();
    for(int i = 0; i <= lastVisibleAction; i++) 
    {
        QAction *action = m_recentFileActs.at(i);

        if (!action->isVisible())
            continue; // we continue, this action will get an empty QRect

        //let the style modify the above size..
        QStyleOptionMenuItem opt;
        initStyleOption(&opt, action);
        const QFontMetrics& fm = opt.fontMetrics;

        QSize sz;
        QString s = action->text();
        sz.setWidth(fm.boundingRect(QRect(), Qt::TextSingleLine | Qt::TextShowMnemonic, s).width());
        sz.setHeight(qMax(fm.height(), qfm.height()));
        sz = style->sizeFromContents(QStyle::CT_MenuItem, &opt, sz, &p);
        //update the item
        if (!sz.isEmpty()) 
            m_actionRects[i] = QRect(0, 0, sz.width(), sz.height());
    }

    int x = hmargin + fw;
    for(int i = 0; i < m_recentFileActs.count(); i++) 
    {
        QRect& rect = m_actionRects[i];
        if (rect.isNull())
            continue;

        rect.translate(x, y); //move
        rect.setWidth(max_column_width); //uniform width

        y += rect.height();
    }
    
    ((RibbonPageSystemRecentFileListPrivate*)this)->m_itemsDirty = false;
}

void RibbonPageSystemRecentFileListPrivate::initStyleOption(QStyleOptionMenuItem* option, const QAction* action) const
{
    if (!option || !action)
        return;

    QTN_P(const RibbonPageSystemRecentFileList);

    option->initFrom(&p);
    option->palette = p.palette();
    option->state = QStyle::State_None;


    if (p.isEnabled() && action->isEnabled())
        option->state |= QStyle::State_Enabled;
    else
        option->palette.setCurrentColorGroup(QPalette::Disabled);

    option->font = action->font().resolve(p.font());
    option->fontMetrics = QFontMetrics(option->font);

    if (m_currentAction && m_currentAction == action) 
        option->state |= QStyle::State_Selected | (m_mouseDown ? QStyle::State_Sunken : QStyle::State_None);

    option->menuHasCheckableItems = m_hasCheckableItems;
    if (!action->isCheckable()) 
    {
        option->checkType = QStyleOptionMenuItem::NotCheckable;
    } 
    else 
    {
        option->checkType = (action->actionGroup() && action->actionGroup()->isExclusive())
            ? QStyleOptionMenuItem::Exclusive : QStyleOptionMenuItem::NonExclusive;
        option->checked = action->isChecked();
    }

    option->menuItemType = QStyleOptionMenuItem::Normal;
    if (action->isIconVisibleInMenu())
        option->icon = action->icon();

    QString textAndAccel = action->text();

    if (textAndAccel.indexOf(QLatin1Char('\t')) == -1) 
    {
        QKeySequence seq = action->shortcut();
        if (!seq.isEmpty())
            textAndAccel += QLatin1Char('\t') + seq.toString();
    }

    option->text = textAndAccel;
    option->tabWidth = 0;
    option->maxIconWidth = 0;
    option->menuRect = p.rect();
}

/* RibbonPageSystemRecentFileList */
RibbonPageSystemRecentFileList::RibbonPageSystemRecentFileList(const QString& caption)
    : RibbonPageSystemPopupListCaption(caption)
{
    QTN_INIT_PRIVATE(RibbonPageSystemRecentFileList);

    if (caption.isEmpty())
        setWindowTitle(tr("Recent Documents"));

    setAttribute(Qt::WA_MouseTracking, true);
}

RibbonPageSystemRecentFileList::~RibbonPageSystemRecentFileList()
{
    QTN_FINI_PRIVATE();
}

QSize RibbonPageSystemRecentFileList::sizeHint() const
{
    QSize temp = RibbonPageSystemPopupListCaption::sizeHint();
    QTN_D(const RibbonPageSystemRecentFileList);
    const int actions = d.m_actionRects.size();
    int nHeight = 0;
    for (int i = 0; i < actions; ++i)
    {
        nHeight += d.m_actionRects[i].height();
    }
    if (nHeight > 20)
        temp.setHeight(temp.height() + nHeight);
    return temp;
}

void RibbonPageSystemRecentFileList::setSize(int size)
{
    QTN_D(RibbonPageSystemRecentFileList);

    if (size < 0 || size > 9)
    {
        Q_ASSERT_X(false, Q_FUNC_INFO, "Number of files could not be more than 9.");
        return; 
    }
    // clean actions
    for (int i = 0, count = d.m_recentFileActs.count(); count > i; i++)
    {
        QAction* act = d.m_recentFileActs[i];
        removeAction(act);
        delete act;
    }
    d.m_recentFileActs.clear();

    for (int i = 0; i < size; ++i) 
    {
        QAction* recentFileAct = new QAction(this);
        recentFileAct->setVisible(false);
        addAction(recentFileAct);
        d.m_recentFileActs.append(recentFileAct);
//        connect(recentFileAct, SIGNAL(triggered()), this, SLOT(openRecentFile()));
    }
}

int RibbonPageSystemRecentFileList::getSize() const
{
    QTN_D(const RibbonPageSystemRecentFileList);
    return d.m_recentFileActs.size();
}

QAction* RibbonPageSystemRecentFileList::getCurrentAction() const
{
    QTN_D(const RibbonPageSystemRecentFileList);
    return d.m_currentAction;
}

static QString strippedName(const QString& fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void RibbonPageSystemRecentFileList::updateRecentFileActions(const QStringList& files)
{
    QTN_D(RibbonPageSystemRecentFileList);

    if (d.m_recentFileActs.size() == 0)
    {
        Q_ASSERT_X(false, Q_FUNC_INFO, "Number of files not defined.");
        return;
    }

    int numRecentFiles = qMin(files.size(), d.m_recentFileActs.size());

    for (int i = 0; i < numRecentFiles; ++i) 
    {
        QAction* recentFileAct = d.m_recentFileActs.at(i);
        QString text = tr("&%1 %2").arg(i + 1).arg(strippedName(files[i]));
        recentFileAct->setText(text);
        recentFileAct->setData(files[i]);
        recentFileAct->setVisible(true);
    }

    for (int j = numRecentFiles; j < d.m_recentFileActs.size(); ++j)
        d.m_recentFileActs[j]->setVisible(false);

    d.m_itemsDirty = true;
    d.updateActionRects();
}


void RibbonPageSystemRecentFileList::drawEvent(QPaintEvent* event, QPainter* p)
{
    QTN_D(RibbonPageSystemRecentFileList);

    QRegion emptyArea = QRegion(rect());

    //draw the items that need updating..
    for (int i = 0; i < d.m_recentFileActs.count(); ++i) 
    {
        QAction* action = d.m_recentFileActs.at(i);
        QRect adjustedActionRect = d.m_actionRects.at(i);
        if (!event->rect().intersects(adjustedActionRect)/* || d->widgetItems.value(action)*/)
            continue;
        //set the clip region to be extra safe (and adjust for the scrollers)
        QRegion adjustedActionReg(adjustedActionRect);
        emptyArea -= adjustedActionReg;
        p->setClipRegion(adjustedActionReg);

        QStyleOptionMenuItem opt;
        d.initStyleOption(&opt, action);
        opt.rect = adjustedActionRect;
        QString text = opt.text;
        opt.text = QString();
        style()->drawControl(QStyle::CE_MenuItem, &opt, p, this);
        
        uint alignment = Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine | Qt::TextShowMnemonic;
        opt.rect.translate(7, 0); //move

        StyleHintReturnThemeColor hintClientTextReturn(tr("Ribbon"), tr("GroupClientText"));
        const QColor clientTextColor = static_cast<QRgb>(style()->styleHint(QStyle::SH_CustomBase, Q_NULL, Q_NULL, &hintClientTextReturn));
        opt.palette.setColor(QPalette::WindowText, clientTextColor);

        opt.text = p->fontMetrics().elidedText(text, Qt::ElideRight, opt.rect.adjusted(2, 0, -2, 0).width());
        style()->drawItemText(p, opt.rect, alignment, opt.palette, true, opt.text, QPalette::WindowText);
    }
}

/*! \reimp */
void RibbonPageSystemRecentFileList::mousePressEvent(QMouseEvent* event)
{
//    setAttribute(Qt::WA_TransparentForMouseEvents, true);
    RibbonPageSystemPopupListCaption::mousePressEvent(event);
}

/*! \reimp */
void RibbonPageSystemRecentFileList::mouseMoveEvent(QMouseEvent* event)
{
    QTN_D(RibbonPageSystemRecentFileList);

    if (!isVisible())
        return;
    d.setCurrentAction(d.actionAt(event->pos()));
}

/*! \reimp */
void RibbonPageSystemRecentFileList::mouseReleaseEvent(QMouseEvent* event)
{
    QTN_D(RibbonPageSystemRecentFileList);

    QAction* action = d.actionAt(event->pos());
    if (action && action == d.m_currentAction) 
    {
#if defined(Q_WS_WIN)
        //On Windows only context menus can be activated with the right button
        if (event->button() == Qt::LeftButton)
#endif
            d.activateAction(action, QAction::Trigger);
    }
    else
        RibbonPageSystemPopupListCaption::mouseReleaseEvent(event);
}

/*! \reimp */
void RibbonPageSystemRecentFileList::enterEvent(QEvent* event)
{
    RibbonPageSystemPopupListCaption::enterEvent(event);

    QEvent leaveEvent(QEvent::Leave);
    QApplication::sendEvent(parentWidget(), &leaveEvent);

    setFocus();
}

/*! \reimp */
void RibbonPageSystemRecentFileList::leaveEvent(QEvent* event)
{
    Q_UNUSED(event);

    QTN_D(RibbonPageSystemRecentFileList);
    d.setCurrentAction(Q_NULL);
    parentWidget()->setFocus();
}

/*! \reimp */
void RibbonPageSystemRecentFileList::keyPressEvent(QKeyEvent* event)
{
    QTN_D(RibbonPageSystemRecentFileList);

    int key = event->key();
    bool key_consumed = false;
    switch(key) 
    {
        case Qt::Key_Up:
        case Qt::Key_Down: 
            {
                key_consumed = true;
                QAction* nextAction = Q_NULL;
                if (!d.m_currentAction) 
                {
                    if(key == Qt::Key_Down) 
                    {
                        for(int i = 0; i < d.m_recentFileActs.count(); ++i) 
                        {
                            QAction* act = d.m_recentFileActs.at(i);
                            if (d.m_actionRects.at(i).isNull())
                                continue;
                            if ( (style()->styleHint(QStyle::SH_Menu_AllowActiveAndDisabled, 0, this) || act->isEnabled())) 
                            {
                                nextAction = act;
                                break;
                            }
                        }
                    } 
                    else 
                    {
                        for (int i = d.m_recentFileActs.count()-1; i >= 0; --i) 
                        {
                            QAction* act = d.m_recentFileActs.at(i);
                            if (d.m_actionRects.at(i).isNull())
                                continue;
                            if ( (style()->styleHint(QStyle::SH_Menu_AllowActiveAndDisabled, 0, this) || act->isEnabled())) 
                            {
                                nextAction = act;
                                break;
                            }
                        }
                    }
                }
                else
                {
                    for(int i = 0, y = 0; !nextAction && i < d.m_recentFileActs.count(); i++) 
                    {
                        QAction *act = d.m_recentFileActs.at(i);
                        if (act == d.m_currentAction) 
                        {
                            if (key == Qt::Key_Up) 
                            {
                                for(int next_i = i-1; true; next_i--) 
                                {
                                    if (next_i == -1) 
                                    {
                                        if(!style()->styleHint(QStyle::SH_Menu_SelectionWrap, 0, this))
                                            break;
                                        next_i = d.m_actionRects.count()-1;
                                    }
                                    QAction *next = d.m_recentFileActs.at(next_i);
                                    if (next == d.m_currentAction)
                                        break;
                                    if (d.m_actionRects.at(next_i).isNull())
                                        continue;
                                    if (next->isSeparator() ||
                                        (!next->isEnabled() &&
                                        !style()->styleHint(QStyle::SH_Menu_AllowActiveAndDisabled, 0, this)))
                                        continue;
                                    nextAction = next;
                                    break;
                                }
                            } 
                            else 
                            {
                                y += d.m_actionRects.at(i).height();
                                for(int next_i = i+1; true; next_i++) 
                                {
                                    if (next_i == d.m_recentFileActs.count()) 
                                    {
                                        if(!style()->styleHint(QStyle::SH_Menu_SelectionWrap, 0, this))
                                            break;
                                        next_i = 0;
                                    }
                                    QAction* next = d.m_recentFileActs.at(next_i);
                                    if (next == d.m_currentAction)
                                        break;
                                    if (d.m_actionRects.at(next_i).isNull())
                                        continue;
                                    if (!next->isEnabled() && !style()->styleHint(QStyle::SH_Menu_AllowActiveAndDisabled, 0, this))
                                        continue;
                                    nextAction = next;
                                    break;
                                }
                            }
                            break;
                        }
                        y += d.m_actionRects.at(i).height();
                    }
                }

                if (nextAction) 
                    d.setCurrentAction(nextAction);
            }
            break;
        case Qt::Key_Return:
        case Qt::Key_Enter: 
            {
                if (!d.m_currentAction) 
                {
                    key_consumed = true;
                    break;
                }
                d.activateAction(d.m_currentAction, QAction::Trigger);
                key_consumed = true;
            }
            break;
        default:
            key_consumed = false;
            break;
    }

    if (!key_consumed)
    {
        if ((!event->modifiers() || event->modifiers() == Qt::AltModifier || event->modifiers() == Qt::ShiftModifier) && event->text().length()==1 ) 
        {
            bool activateAction = false;
            QAction *nextAction = 0;

            int clashCount = 0;
            QAction* first = 0, *currentSelected = 0, *firstAfterCurrent = 0;
            QChar c = event->text().at(0).toUpper();
            for(int i = 0; i < d.m_recentFileActs.size(); ++i) 
            {
                if (d.m_actionRects.at(i).isNull())
                    continue;
                QAction *act = d.m_recentFileActs.at(i);
                QKeySequence sequence = QKeySequence::mnemonic(act->text());
                int key = sequence[0] & 0xffff;
                if (key == c.unicode()) 
                {
                    clashCount++;
                    if (!first)
                        first = act;
                    if (act == d.m_currentAction)
                        currentSelected = act;
                    else if (!firstAfterCurrent && currentSelected)
                        firstAfterCurrent = act;
                }
            }
            if (clashCount == 1)
                activateAction = true;

            if (clashCount >= 1) 
            {
                if (clashCount == 1 || !currentSelected || !firstAfterCurrent)
                    nextAction = first;
                else
                    nextAction = firstAfterCurrent;
            }

            if (nextAction) 
            {
                key_consumed = true;
                if (!nextAction->menu() && activateAction)
                    d.activateAction(nextAction, QAction::Trigger);
            }
        }

#ifdef Q_OS_WIN32
        if (key_consumed && (event->key() == Qt::Key_Control || event->key() == Qt::Key_Shift || event->key() == Qt::Key_Meta))
            QApplication::beep();
#endif // Q_OS_WIN32
    }

    if (key_consumed)
        event->accept();
    else
        event->ignore();
}

/*! \reimp */
void RibbonPageSystemRecentFileList::resizeEvent(QResizeEvent* event)
{
    RibbonPageSystemPopupListCaption::resizeEvent(event);

    QTN_D(RibbonPageSystemRecentFileList);
    d.updateActionRects();
}

namespace Qtitan
{
    /* RibbonPageSystemPopupPrivate */
    class RibbonPageSystemPopupPrivate : public QObject 
    {
        QTN_DECLARE_PUBLIC(RibbonPageSystemPopup)
    public:
        explicit RibbonPageSystemPopupPrivate() { m_parentMenu = Q_NULL; }
    public:
        QWidget* m_parentMenu;
        QString m_caption;
    };
}

/* RibbonPageSystemPopup */
RibbonPageSystemPopup::RibbonPageSystemPopup(const QString& caption)
    : QMenu()
{
    QTN_INIT_PRIVATE(RibbonPageSystemPopup);
    QTN_D(RibbonPageSystemPopup);
    d.m_caption = caption;

    QString text = QString(qtn_PopupLable) + d.m_caption;
    QAction* actLabel = new QAction(text, this);
    addAction(actLabel);
}

RibbonPageSystemPopup::~RibbonPageSystemPopup()
{
    QTN_FINI_PRIVATE();
}

/*! \reimp */
bool RibbonPageSystemPopup::event(QEvent* event)
{
    QTN_D(RibbonPageSystemPopup);

    if (event->type() == QEvent::Move)
    {
        QTN_D(RibbonPageSystemPopup);
        if (RibbonSystemPopupBar* parent = qobject_cast<RibbonSystemPopupBar*>(d.m_parentMenu))
        {
            QSize sz = parent->QMenu::sizeHint();
            QRect rcBorders = parent->qtn_d().m_rcBorders;
            QPoint pnt(sz.width() - rcBorders.left() + 1, rcBorders.top() + 1);
            pnt = parent->mapToGlobal(pnt);

            QRect rc(pnt, QSize(sizeHint().width(), sz.height() - rcBorders.bottom() - rcBorders.top()/2));
            setGeometry(rc);
        }

    }
    else if (event->type() == QEvent::ParentChange)
    {
        if (QtnEventParent* e = dynamic_cast<QtnEventParent*>(event))
        {
            if (!d.m_parentMenu)
            {
                d.m_parentMenu = e->m_parent;
                setFont(d.m_parentMenu->font());
            }
        }
    }
    bool result = QMenu::event(event);

    return result;
}

/*! \reimp */
void RibbonPageSystemPopup::paintEvent(QPaintEvent* event)
{
    QTN_D(RibbonPageSystemPopup);

    QPainter p(this);

    QRegion emptyArea = QRegion(rect());
    QStyleOptionMenuItem menuOpt;
    menuOpt.initFrom(this);
    menuOpt.state = QStyle::State_None;
    menuOpt.checkType = QStyleOptionMenuItem::NotCheckable;
    menuOpt.maxIconWidth = 0;
    menuOpt.tabWidth = 0;
    menuOpt.text = d.m_caption;
    style()->drawPrimitive(QStyle::PE_FrameMenu, &menuOpt, &p, this );

    QList<QAction*> actionList = actions();
    for (int i = 0; i < actionList.count(); ++i) 
    {
        QAction* action = actionList.at(i);
        QRect adjustedActionRect = actionGeometry(action);

        if (!event->rect().intersects(adjustedActionRect))
            continue;

        QStyleOptionMenuItem opt;
        initStyleOption(&opt, action);
        opt.rect = adjustedActionRect;

        if (opt.text.count(QString(qtn_PopupLable)) == 0)
            opt.text = opt.text + QString("\n") + action->toolTip();

        style()->drawControl( QStyle::CE_MenuItem, &opt, &p, this );
    }
}

