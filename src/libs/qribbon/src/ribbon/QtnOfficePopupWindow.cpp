/****************************************************************************
**
** Qtitan Library by Developer Machines (Microsoft-Ribbon implementation for Qt.C++)
** 
** Copyright (c) 2009-2011 Developer Machines (http://www.devmachines.com)
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
#include <QStyle>
#include <QStyleOption>
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <QDesktopWidget>
#include <QLabel>
#include <QAction>
#include <QLayout>
#include <QToolButton>

#ifdef Q_OS_WIN
#include <qt_windows.h>
#endif // Q_OS_WIN

#include "QtnOfficePopupWindow.h"
#include "../styles/QtnPopupHelpers.h"

QTITAN_USE_NAMESPACE

QTITAN_BEGIN_NAMESPACE
/* TitleBar */
class TitleBar : public QWidget
{
public:
    TitleBar(QWidget* parent = Q_NULL);
    virtual ~TitleBar();

public:
    QHBoxLayout* horizontalMainLayout;
    QHBoxLayout* horizontalLayout;
    QLabel* labelIcon;
    QLabel* labelTitle;
};
QTITAN_END_NAMESPACE

TitleBar::TitleBar(QWidget* parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    horizontalMainLayout = new QHBoxLayout(this);
    horizontalMainLayout->setMargin(0);
    horizontalMainLayout->setSpacing(0);

    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setSpacing(6);
    labelIcon = new QLabel(this);
    labelIcon->setVisible(false);
    horizontalLayout->addWidget(labelIcon);

    labelTitle = new QLabel(this);
    labelTitle->setVisible(false);
    horizontalLayout->addWidget(labelTitle);

    horizontalLayout->setStretch(1, 1);
    horizontalMainLayout->addLayout(horizontalLayout);
}

TitleBar::~TitleBar()
{
}


QTITAN_BEGIN_NAMESPACE
/* ManagerPopup */
class ManagerPopup
{
public:
    static ManagerPopup& getMngPopup()
    {
        static ManagerPopup mngPopup;
        return mngPopup;
    }

protected:
    ManagerPopup() { m_ptPopup = QPoint(-1, -1); }
    virtual ~ManagerPopup() {}

public:
    void appendPopup(OfficePopupWindow* popup);
    void removePopup(OfficePopupWindow* popup);
    QPoint findBestPosition(OfficePopupWindow* popup);

protected:
    QList<QPointer<OfficePopupWindow> > m_listPopup;
public:
    QPoint m_ptPopup;
};
QTITAN_END_NAMESPACE


void ManagerPopup::appendPopup(OfficePopupWindow* popup)
{
    m_listPopup.append(popup);
}

void ManagerPopup::removePopup(OfficePopupWindow* popup)
{
    m_listPopup.removeOne(popup);
}

QPoint ManagerPopup::findBestPosition(OfficePopupWindow* popup)
{
    if (m_ptPopup != QPoint(-1, -1)) 
        popup->setPosition(m_ptPopup);

    QPoint ptPopup = popup->getPosition();
    QSize szPopup = popup->sizeHint();
    QRect rcActivePopup(QPoint(ptPopup.x() - szPopup.width(), ptPopup.y() - szPopup.height()), szPopup);
    
    bool intersect = false;
    do 
    {
        intersect = false;
        int index = 0;
        while(index < m_listPopup.count())
        {
            OfficePopupWindow* curPopup = m_listPopup.at(index);
            if (curPopup)
            {
                QSize szCurPopup = curPopup->sizeHint();
                QRect rcPopup(QPoint(curPopup->getPosition().x() - szCurPopup.width(), 
                    curPopup->getPosition().y() - szCurPopup.height()), szCurPopup);

                if (rcPopup.intersects(rcActivePopup))
                {
                    ptPopup.setY(rcPopup.top());
                    rcActivePopup = QRect(QPoint(ptPopup.x() - szPopup.width(), ptPopup.y() - szPopup.height()), szPopup);
                    intersect = true;
                }
            }
            index++;
        }
    } while (intersect);
    return ptPopup;
}


QTITAN_BEGIN_NAMESPACE
enum PopupState
{
    PopupStateClosed,
    PopupStateExpanding,
    PopupStateShow,
    PopupStateCollapsing
};

/* OfficePopupWindowPrivate */
class OfficePopupWindowPrivate : public QObject
{
public:
    struct InfoState
    {
        QRect rcPopup;
        int transparency;
    };
public:
    QTN_DECLARE_PUBLIC(OfficePopupWindow)
public:
    explicit OfficePopupWindowPrivate();
    virtual ~OfficePopupWindowPrivate();
    void init();

public:
    void onCollapsing();
    void onExpanding(bool updateCurrent);

    void updateState(bool repaint);
    bool closePopup();
    void showPopup();
    void animate(int step);

    void handleMousePressEvent(QMouseEvent* event);
    void handleMouseReleaseEvent(QMouseEvent* event);
    void handleMouseMoveEvent(QMouseEvent* event);

    void initTitleBarStyleOption(StyleOptionPopupTitleBar* option) const;
    void initFormStyleOption(QStyleOptionFrame* option) const;

    void setPopupState(PopupState popupState);
    PopupState getPopupState() const;
    void createStyle();
    void calclayout();

public:
    int m_transparency;
    uint m_showDelay;
    uint m_animationSpeed;
    uint m_animationInterval;
    int m_step;

    bool m_capture;
    bool m_allowMove;
    bool m_dragging;

    QPoint m_positionPopup;
    QPoint m_dragPressPosition;

    PopupLocation m_popupLocation;
    PopupAnimation m_popupAnimation;
    PopupState m_popupState;

    InfoState m_stateTarget;
    InfoState m_stateCurrent;

    QTimer m_showDelayTimer;
    QTimer m_collapsingTimer;
    QTimer m_expandingTimer;

    QWidget* m_form;
    TitleBar* m_title;
    QFont m_titleFont;
    QIcon m_titleIcon;
    QString m_titleTxt;
    QString m_titleTxtRef;
    QString m_bodyTxtRef;
    QPixmap m_closePixmap;
    QPalette m_titlePalette;
    QStyle::SubControl m_buttonDown;
    bool m_pressed;
    bool m_closeButton;
    bool m_notCloseTimer;
};
QTITAN_END_NAMESPACE

OfficePopupWindowPrivate::OfficePopupWindowPrivate()
{
    m_form = Q_NULL;
    m_closeButton = false;
    m_notCloseTimer = false;
}

OfficePopupWindowPrivate::~OfficePopupWindowPrivate()
{
}

void OfficePopupWindowPrivate::init()
{
    QTN_P(OfficePopupWindow);
    p.setAttribute(Qt::WA_MouseTracking);

    m_titleFont = QApplication::font("QDockWidgetTitle");
    p.setFont(m_titleFont);

    m_buttonDown =  QStyle::SC_None;
    m_transparency = 255;
    m_showDelay = 5000;
    m_animationSpeed = 256;
    m_animationInterval = 16;
    m_step = 0;

    m_capture = false;
    m_allowMove = false;
    m_dragging = false;
    m_pressed = false;

    m_positionPopup = QPoint(-1, -1);
    m_dragPressPosition = QPoint(-1, -1);
#ifdef Q_OS_WIN
    m_popupLocation = PopupLocationNearTaskBar;
#else // Q_OS_WIN
    m_popupLocation = PopupLocationBottomRight;
#endif // Q_OS_WIN
    m_popupAnimation = PopupAnimationNone;
    m_popupState = PopupStateClosed;

    connect(&m_showDelayTimer,  SIGNAL(timeout()), &p, SLOT(showDelayTimer()));
    connect(&m_collapsingTimer, SIGNAL(timeout()), &p, SLOT(collapsingTimer()));
    connect(&m_expandingTimer,  SIGNAL(timeout()), &p, SLOT(expandingTimer()));

    m_title = new TitleBar(&p);

    QLabel* label = new QLabel();
    p.setCentralWidget(label);
}

void OfficePopupWindowPrivate::onCollapsing()
{
    QTN_P(OfficePopupWindow);
    setPopupState(PopupStateCollapsing);

    if (m_popupAnimation == PopupAnimationNone || m_animationSpeed <= 0)
    {
        closePopup();
        return;
    }
    else if (m_popupAnimation == PopupAnimationFade)
    {
        m_stateTarget.transparency = 0;
    }
    else if (m_popupAnimation == PopupAnimationSlide)
    {
        QStyleOption opt;
        opt.init(&p);
        QPoint positonPopup = p.getPosition();
        m_stateTarget.rcPopup = QRect(QPoint(positonPopup.x() - opt.rect.width(), positonPopup.y()), positonPopup);
    }
    else if (m_popupAnimation == PopupAnimationUnfold)
    {
        m_stateTarget.rcPopup = QRect(p.getPosition(), QSize(0, 0));
    }

    m_step = qMax((uint)1, m_animationSpeed/m_animationInterval);

    m_collapsingTimer.start(m_animationInterval);

    updateState(true);
}

void OfficePopupWindowPrivate::onExpanding(bool updateCurrent)
{
    QTN_P(OfficePopupWindow);
    setPopupState(PopupStateExpanding);

    QPoint positionPopup = p.getPosition();

//    QStyleOption opt;
//    opt.init(&p);
    QSize sz = p.sizeHint();//opt.rect.size(); 

    m_stateTarget.rcPopup = QRect(QPoint(positionPopup.x() - sz.width(), positionPopup.y() - sz.height()), sz);
    m_stateTarget.transparency = m_transparency;

    if (updateCurrent)
    {
        m_stateCurrent = m_stateTarget;
        if (m_popupAnimation == PopupAnimationNone || m_animationSpeed <= 0)
        {
            updateState(true);
            showPopup();
            return;
        }
        else if (m_popupAnimation == PopupAnimationFade)
        {
            m_stateCurrent.transparency = 0;
        }
        if (m_popupAnimation == PopupAnimationSlide)
            m_stateCurrent.rcPopup = QRect(QPoint(positionPopup.x() - sz.width(), positionPopup.y()), positionPopup);
        else if (m_popupAnimation == PopupAnimationUnfold)
            m_stateCurrent.rcPopup = QRect(positionPopup, QSize(0,0));

        m_step = qMax((uint)1, m_animationSpeed/m_animationInterval);
    }
    else
        m_step = qMax((uint)1, m_animationSpeed/m_animationInterval - m_step);

    m_expandingTimer.start(m_animationInterval);
    updateState(true);
}

void OfficePopupWindowPrivate::updateState(bool repaint)
{
    QTN_P(OfficePopupWindow);
    QRect rc = m_stateCurrent.rcPopup;

    p.setGeometry(rc);

    if (repaint)
        p.repaint();

    if (!m_capture)
        p.setWindowOpacity((qreal)m_stateCurrent.transparency/255.0);
    p.update();
}

bool OfficePopupWindowPrivate::closePopup()
{
    QTN_P(OfficePopupWindow);
    bool result = false;
    m_capture = false;

    if (p.testAttribute(Qt::WA_DeleteOnClose))
    {
        setPopupState(PopupStateClosed);
        result = p.close();
        p.deleteLater();
        return result;
    }

    result = p.close();
    setPopupState(PopupStateClosed);
    return result;
}

void OfficePopupWindowPrivate::showPopup()
{
    QTN_P(OfficePopupWindow);

    setPopupState(PopupStateShow);

    QPoint positionPopup = p.getPosition();
    QSize sizePopup = p.size();
    m_stateCurrent.rcPopup = QRect(QPoint(positionPopup.x() - sizePopup.width(), positionPopup.y() - sizePopup.height()), sizePopup);
    m_stateCurrent.transparency = m_transparency;

    if (m_showDelay != (uint)-1)
        m_showDelayTimer.start(m_showDelay);
}

static void moveTo(int& x1, int& x2, int step)
{
    if (x1 != x2)
        x1 += qMax(1, ::abs(x1 - x2)/step) * (x1 > x2 ? -1 : 1);
}

void OfficePopupWindowPrivate::animate(int step)
{
    if (step < 1)
    {
        m_stateCurrent = m_stateTarget;
    }
    else
    {
        int x11, y11, x12, y12;
        m_stateCurrent.rcPopup.getCoords(&x11, &y11, &x12, &y12);
        int x21, y21, x22, y22;
        m_stateTarget.rcPopup.getCoords(&x21, &y21, &x22, &y22);

        moveTo(y11, y21, step);
        moveTo(x11, x21, step);
        moveTo(x12, x22, step);
        moveTo(y12, y22, step);
        m_stateCurrent.rcPopup.setCoords(x11, y11, x12, y12);
        moveTo(m_stateCurrent.transparency, m_stateTarget.transparency, step);
    }
    updateState(true);
}

void OfficePopupWindowPrivate::handleMousePressEvent(QMouseEvent* event)
{
    QTN_P(OfficePopupWindow);

    StyleOptionPopupTitleBar optTitle;
    initTitleBarStyleOption(&optTitle);

    if (event->button() == Qt::LeftButton) 
    {
        QStyle::SubControl ctrl = p.style()->hitTestComplexControl(QStyle::CC_TitleBar, &optTitle, event->pos(), &p);
        m_buttonDown = ctrl;
        m_pressed = true;
        if (m_buttonDown == QStyle::SC_TitleBarCloseButton)
        {
            p.repaint();
            return;
        }
    }

    if (getPopupState() == PopupStateExpanding)
    {
        m_step = 0;
        animate(0);
        m_expandingTimer.stop();
        showPopup();
    }

    if (m_allowMove && event->button() == Qt::LeftButton)
    {
        const int titleBarHeight = p.style()->pixelMetric(QStyle::PM_TitleBarHeight, 0, &p);
        QRect rect = optTitle.rect;
        rect.setHeight(titleBarHeight);
        if (rect.contains(event->pos()))
        {
            m_dragging = true;
            m_dragPressPosition = event->pos();
            return;
        }
    }
}

void OfficePopupWindowPrivate::handleMouseReleaseEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    QTN_P(OfficePopupWindow);

    if (event->button() == Qt::LeftButton) 
    {
        m_pressed = false;

        StyleOptionPopupTitleBar optTitle;
        initTitleBarStyleOption(&optTitle);
        QStyle::SubControl ctrl = p.style()->hitTestComplexControl(QStyle::CC_TitleBar, &optTitle, event->pos(), &p);
        m_buttonDown = ctrl;
        if (m_buttonDown == QStyle::SC_TitleBarCloseButton)
        {
            closePopup();
            return;
        }
    }

    if (m_dragging)
    {
        m_stateTarget.rcPopup = m_stateCurrent.rcPopup = p.geometry();
        m_positionPopup = m_stateCurrent.rcPopup.bottomRight();

        m_dragging = false;
        m_dragPressPosition = QPoint(-1, -1);
    }
}

void OfficePopupWindowPrivate::handleMouseMoveEvent(QMouseEvent* event)
{
    QTN_P(OfficePopupWindow);
    if (m_dragging && m_form)
    {
        const QPoint delta = event->pos() - m_dragPressPosition;

        QRect rc = p.rect();
        QRect rcScreenGeometry = QApplication::desktop()->screenGeometry();

        rc.translate(p.pos() + delta);

        if (rc.left() < rcScreenGeometry.left()) 
            rc.translate(rcScreenGeometry.left() - rc.left(), 0);
        if (rc.top() < rcScreenGeometry.top()) 
            rc.translate(0, rcScreenGeometry.top() - rc.top());
        if (rc.right() > rcScreenGeometry.right()) 
            rc.translate(rcScreenGeometry.right() - rc.right(), 0);
        if (rc.bottom() > rcScreenGeometry.bottom()) 
            rc.translate(0, rcScreenGeometry.bottom() - rc.bottom());

        p.setGeometry(rc);
        //move(this->pos() + delta);

        m_stateTarget.rcPopup = m_stateCurrent.rcPopup = p.geometry();
        m_positionPopup = m_stateCurrent.rcPopup.bottomRight();

        ManagerPopup::getMngPopup().m_ptPopup = p.getPosition();
        return;
    }

    QRect rc = p.rect();
    bool contains = rc.contains(event->pos());

    if (contains && !m_capture)
    {
        m_capture = true;
        p.setWindowOpacity(255.0);
    }
    if (!contains && m_capture)
    {
        m_capture = false;
        p.setWindowOpacity((qreal)m_transparency/255.0);
    }
    if (m_popupState == PopupStateCollapsing)
    {
        m_collapsingTimer.stop();

        if (m_popupAnimation == PopupAnimationFade)
            showPopup();
        else
            onExpanding(false);
    }

    StyleOptionPopupTitleBar optTitle;
    initTitleBarStyleOption(&optTitle);
    QStyle::SubControl last_ctrl = m_buttonDown;
    m_buttonDown = p.style()->hitTestComplexControl(QStyle::CC_TitleBar, &optTitle, event->pos(), &p);
    if (last_ctrl != m_buttonDown)
        p.repaint();
}

void OfficePopupWindowPrivate::initTitleBarStyleOption(StyleOptionPopupTitleBar* option) const
{
    QTN_P(const OfficePopupWindow);
    option->init(&p);
    option->palette = m_titlePalette;
    option->text = m_titleTxt;
    QFontMetrics fm(m_titleFont);
    option->fontMetrics = fm;
    option->subControls = 0;

    if (!m_titleTxt.isEmpty())
      option->subControls = QStyle::SC_TitleBarLabel;

    if (m_closeButton)
      option->subControls |= QStyle::SC_TitleBarCloseButton;

    if (!m_titleIcon.isNull())
    {
        option->subControls |= QStyle::SC_TitleBarSysMenu;
        option->icon = m_titleIcon;
    }

    if (!m_closePixmap.isNull())
        option->pixmapCloseButton = m_closePixmap;

    option->activeSubControls = m_buttonDown ? QStyle::SC_TitleBarCloseButton : QStyle::SC_None;

    if (m_pressed)
        option->state |= QStyle::State_Sunken;

    const int titleBarHeight = p.style()->pixelMetric(QStyle::PM_TitleBarHeight, 0, &p);
    const int lineWidth = p.style()->pixelMetric(QStyle::PM_DefaultFrameWidth, 0, &p) + 2;

    QSize szTitle = m_title->sizeHint();

    QSize size = m_form->sizeHint();
    if (!size.isValid())
        size = m_form->size();

    option->rect.setHeight(titleBarHeight);
    option->rect.setWidth(qMax((size.width() + lineWidth*2), szTitle.width()));
}

void OfficePopupWindowPrivate::initFormStyleOption(QStyleOptionFrame* option) const
{
    QTN_P(const OfficePopupWindow);
    option->init(&p);
    option->palette = m_titlePalette;

    const int lineWidth = p.style()->pixelMetric(QStyle::PM_DefaultFrameWidth, 0, &p) + 2;
    option->lineWidth = lineWidth;
    option->midLineWidth = 1;

    QSize size = m_form->sizeHint();
    if (!size.isValid())
        size = m_form->size();

    StyleOptionPopupTitleBar optTitle;
    initTitleBarStyleOption(&optTitle);
    QRect titleBar = optTitle.rect;

    const int titleBarHeight = p.style()->pixelMetric(QStyle::PM_TitleBarHeight, 0, &p);
    QSize szTitle = m_title->sizeHint();
    
    if (m_closeButton && szTitle.isNull())
    {
        QRect buttonRect = p.style()->subControlRect(QStyle::CC_TitleBar, &optTitle, QStyle::SC_TitleBarCloseButton, &p);
        szTitle.setHeight(buttonRect.height());
    }

    QRect labelRect = QRect(QPoint(0,0), QSize(szTitle.width(), titleBarHeight > 7 ? titleBarHeight : titleBarHeight + szTitle.height()));

    titleBar = titleBar.united(labelRect);

    int additive = lineWidth*2;
    option->rect.setHeight(size.height() + titleBar.height() + additive);
    option->rect.setWidth(size.width() + additive);
}

void OfficePopupWindowPrivate::setPopupState(PopupState popupState)
{
    if (m_popupState == popupState)
        return;
    m_popupState = popupState;
}

PopupState OfficePopupWindowPrivate::getPopupState() const
{
    return m_popupState;
}

void OfficePopupWindowPrivate::createStyle()
{
    QTN_P(OfficePopupWindow);
    if (OfficeStyle* officeStyle = qobject_cast<OfficeStyle*>(p.style()))
        officeStyle->setPopupDecoration(OfficeStyle::PopupSystemDecoration);
    else
        p.setStyle(new PopupDrawHelper());
}

void OfficePopupWindowPrivate::calclayout()
{
    QTN_P(OfficePopupWindow);
    if (OfficeStyle* officeStyle = qobject_cast<OfficeStyle*>(p.style()))
    {
        if (!officeStyle->isExistPopupProxy())
            createStyle();
    }
    else
        createStyle();

    StyleOptionPopupTitleBar optTitle;
    initTitleBarStyleOption(&optTitle);

    QSize szTitle = m_title->sizeHint();
    QRect buttonRect = p.style()->subControlRect(QStyle::CC_TitleBar, &optTitle, QStyle::SC_TitleBarCloseButton, &p);

    if (m_closeButton && szTitle.isNull())
        szTitle.setHeight(buttonRect.height());

    const int titleBarHeight = p.style()->pixelMetric(QStyle::PM_TitleBarHeight, 0, &p);
    const int lineWidth = p.style()->pixelMetric(QStyle::PM_DefaultFrameWidth, 0, &p) + 2;

    int y = titleBarHeight > 7 ? /*lineWidth*/0 : titleBarHeight + lineWidth/2;
    szTitle.setWidth(optTitle.rect.width() - buttonRect.width() - lineWidth * 4);
    szTitle.setHeight(titleBarHeight > 7 ? titleBarHeight : szTitle.height());
    QRect rectTitle(QPoint(lineWidth, y), szTitle);
    m_title->setGeometry(rectTitle);

    if (!optTitle.icon.isNull())
    {
        QSize sz = optTitle.icon.actualSize(rectTitle.size());
        QImage px(sz, QImage::Format_ARGB32_Premultiplied);
        QPainter p(&px);
        px.fill(0);
        optTitle.icon.paint(&p, 0, 0, sz.width(), sz.height());
        m_title->labelIcon->setPixmap(QPixmap::fromImage(px));
        m_title->labelIcon->setVisible(true);
    }

//    QRect rect = m_form->geometry();

    m_form->move(lineWidth, szTitle.height() > 0 ? rectTitle.bottom() + lineWidth : titleBarHeight + lineWidth);
    setPopupState(PopupStateShow);

    QPoint positionPopup = p.getPosition();

//    QStyleOption opt;
//    opt.init(&p);
    QSize sz = p.sizeHint();//opt.rect.size(); 

    m_stateCurrent.rcPopup = QRect(QPoint(positionPopup.x() - sz.width(), positionPopup.y() - sz.height()), sz);
    m_stateCurrent.transparency = m_transparency;

    if (m_showDelay != (uint)-1)
        m_showDelayTimer.start(m_showDelay);
}


/* OfficePopupWindow */
OfficePopupWindow::OfficePopupWindow(QWidget* parent)
    : QWidget(parent, Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint)
{
    QTN_INIT_PRIVATE(OfficePopupWindow);
    QTN_D(OfficePopupWindow);
    d.init();
}

OfficePopupWindow::~OfficePopupWindow()
{
    if (OfficeStyle* officeStyle = qobject_cast<OfficeStyle*>(style()))
        officeStyle->unsetPopupProxy();
    QTN_FINI_PRIVATE();
}

/*!
    Static method to quick show the office popup box with given \a parent, \a titleIcon - title icon, \a titleText - title text and bodyText - body text. 
    You may use HTML to fill \a titleText and \a bodyText.
    Optional parameters \a closeButtonVisible and \a decoration are used for the customization purposes.
    After it will be closed the instance of the window will be deleted automaticaly.

    \image ribbonui_officepopupwindow5.png
    To show such message on the desktop you need invoke code bellow:

    \code
    QString titleHtml = "<span style=\"font-family:'MS Shell Dlg 2'; font-size:8pt; font-weight:600; font-style:normal;\">John Smith</span><br>Re: Your request";
    QString bodyHtml = "<a href='#'><img src=':/res/cross.ico'></a> \
                    <a href='#'><img src=':/res/flag.ico'></a> \
                    <span style=\"color:#0000ff; font-family:'MS Shell Dlg 2'; font-size:8pt; font-weight:400; font-style:normal;\"> \
                    I am sending you the requested inform&nbsp;&nbsp;&nbsp;</span>";
    OfficePopupWindow::showPopup(0, QIcon(":/res/letter.ico"), titleHtml, bodyHtml);
    \endcode
*/
void OfficePopupWindow::showPopup(QWidget* parent, const QIcon& titleIcon, const QString& titleText,
    const QString& bodyText, bool closeButtonVisible, OfficeStyle::OfficePopupDecoration decoration)
{
    OfficePopupWindow* popup = new OfficePopupWindow(parent);
    popup->setTitleIcon(titleIcon);
    popup->setTitleText(titleText);
    popup->setTitleCloseButtonVisible(closeButtonVisible);
    popup->setBodyText(bodyText);

    if (QLabel* label = qobject_cast<QLabel*>(popup->centralWidget()))
        label->setOpenExternalLinks(true);

    if (OfficeStyle* officeStyle = qobject_cast<OfficeStyle*>(popup->style()))
        officeStyle->setPopupDecoration(decoration);

    popup->setDragDropEnabled(true);
    popup->setTransparency(200.0/255.0);
    popup->setDisplayTime(7500);
    popup->setAnimationSpeed(250);
    popup->setAnimation(PopupAnimationFade);

    popup->setPosition(popup->getPosition());
    popup->setAttribute(Qt::WA_DeleteOnClose);
    popup->showPopup();
}

/*!
Sets the central \a widget for the office popup window. 
The method is used when you have to show the non-standard contents inside the popup box. In this case the settings of the 
titleText(), titleIcon() and bodyText() will be ignored and given \a widget will be showing instead.
*/ 
void OfficePopupWindow::setCentralWidget(QWidget* widget)
{
    QTN_D(OfficePopupWindow);
    if (!widget)
        return;

    if (d.m_form)
    {
        delete d.m_form;
        d.m_form = Q_NULL;
    }

    d.m_form = widget;
    d.m_form->setAttribute(Qt::WA_MouseTracking);
    d.m_form->setParent(this);
}

/*!
Returns the central widget for the office popup window. 
*/ 
QWidget* OfficePopupWindow::centralWidget() const
{
    QTN_D(const OfficePopupWindow);
    return d.m_form;
}

/*!
Sets the body \a text for the popup window. The body text can be pure text or HTML.
*/ 
void OfficePopupWindow::setBodyText(const QString& text)
{
    QTN_D(OfficePopupWindow);
    d.m_bodyTxtRef = text;
    if (QLabel* label = qobject_cast<QLabel*>(d.m_form))
        label->setText(text);
}

/*!
Returns the text of the popup window body. Returned text is the same you have passed to setBodyText() function.
*/ 
const QString& OfficePopupWindow::bodyText() const
{
    QTN_D(const OfficePopupWindow);
    return d.m_bodyTxtRef;
}

/*!
Sets the \a icon for the office popup window title.
*/ 
void OfficePopupWindow::setTitleIcon(const QIcon& icon)
{
    QTN_D(OfficePopupWindow);
    if (!icon.isNull())
        d.m_title->labelIcon->setVisible(true);
    else
        d.m_title->labelIcon->setVisible(false);
    d.m_titleIcon = icon;
}

/*!
Returns the icon of the office popup window title.
*/ 
const QIcon& OfficePopupWindow::titleIcon() const
{
    QTN_D(const OfficePopupWindow);
    return d.m_titleIcon;
}

/*!
Sets the \a text for the office popup window title. Text can be pure text or HTML.
*/ 
void OfficePopupWindow::setTitleText(const QString& text)
{
    QTN_D(OfficePopupWindow);
    d.m_titleTxtRef = text;
    d.m_title->labelTitle->setText(text);
    d.m_title->labelTitle->setVisible(!text.isEmpty());
}

/*!
Returns the text of the popup window title. Returned text is the same you have passed to setTitleText() function.
*/ 
const QString& OfficePopupWindow::titleText() const
{
    QTN_D(const OfficePopupWindow);
    return d.m_titleTxtRef;
}

/*!
Sets the close button \a pixmap for painting on the office popup window title.
*/ 
void OfficePopupWindow::setCloseButtonPixmap(const QPixmap& pixmap)
{
    QTN_D(OfficePopupWindow);
    d.m_closePixmap = pixmap;
}

/*!
Returns the close button pixmap for painting on the office popup window title.
*/ 
const QPixmap& OfficePopupWindow::closeButtonPixmap() const
{
    QTN_D(const OfficePopupWindow);
    return d.m_closePixmap;
}

/*!
Sets the visibility of the title's close button. Parameter \a visible is a visibility of the close button.
*/ 
void OfficePopupWindow::setTitleCloseButtonVisible(bool visible)
{
    QTN_D(OfficePopupWindow);
    d.m_closeButton = visible;
}

/*!
Returns the visibility of the title's close buttone.
*/ 
bool OfficePopupWindow::isTitleCloseButtonVisible() const
{
    QTN_D(const OfficePopupWindow);
    return d.m_closeButton;
}

/*!
Shows popup box on the desktop.
*/ 
bool OfficePopupWindow::showPopup()
{
    QPoint pt = ManagerPopup::getMngPopup().findBestPosition(this);
    return showPopup(pt);
}

/*!
Shows popup box on the desktop at the given \a pos. This method is equal to: setPosition(pos); showPopup(); line of code.
*/ 
bool OfficePopupWindow::showPopup(const QPoint& pos)
{
    QTN_D(OfficePopupWindow);

    if (OfficeStyle* officeStyle = qobject_cast<OfficeStyle*>(style()))
        officeStyle->createPopupProxy();

    if (d.m_popupState != PopupStateClosed)
        return false;

    setPosition(pos);

    emit aboutToShow();

    d.calclayout();
    d.onExpanding(true);
    setVisible(true);
    return true;
}

/*!
Returns the current popup window position on the desktop.
*/ 
QPoint OfficePopupWindow::getPosition() const
{
    QTN_D(const OfficePopupWindow);
    if (d.m_positionPopup != QPoint(-1, -1))
        return d.m_positionPopup;

    QSize sz = size();

#ifdef Q_OS_WIN
    if (d.m_popupLocation == PopupLocationNearTaskBar)
    {
        APPBARDATA abd;
        ZeroMemory(&abd, sizeof(APPBARDATA));
        abd.cbSize = sizeof(APPBARDATA);
        if (SHAppBarMessage(ABM_GETTASKBARPOS, &abd))
        {
            RECT rcWork;
            if (!::SystemParametersInfo(SPI_GETWORKAREA, 0, static_cast<void*>(&rcWork), 0))
                return QPoint(0,0);

            QRect rc(QPoint(rcWork.left, rcWork.top), QPoint(rcWork.right, rcWork.bottom));

            QRect taskRc = QRect(QPoint(abd.rc.left, abd.rc.top), QPoint(abd.rc.right, abd.rc.bottom));

            if (rc.center().y() < taskRc.top())
                return QPoint(rc.right(), rc.bottom());

            if (rc.center().x() > taskRc.right())
                return QPoint(rc.left() + sz.width(), rc.bottom());

            if (rc.center().y() > taskRc.bottom())
                return QPoint(rc.right(), rc.top() + sz.height());

            if (rc.center().x() < taskRc.left())
                return QPoint(rc.right(), rc.bottom());
        }
    }
#endif // Q_OS_WIN

    QRect screen = QApplication::desktop()->screenGeometry();

    if (d.m_popupLocation == PopupLocationCenter)
        return QPoint(screen.center().x() + sz.width() / 2, screen.center().y() + sz.height() / 2);
    return screen.bottomRight();
}

/*!
\brief Sets the popup window position on the desktop with given \a pos parameter.
*/ 
void OfficePopupWindow::setPosition(const QPoint& pos)
{
    QTN_D(OfficePopupWindow);
    d.m_positionPopup = pos;
}

/*!
\brief Sets the popup window animation type with given \a popupAnimation parameter.
*/ 
void OfficePopupWindow::setAnimation(PopupAnimation popupAnimation)
{
    QTN_D(OfficePopupWindow);
    d.m_popupAnimation = popupAnimation;
}

/*!
Returns the popup window animation type.
*/ 
PopupAnimation OfficePopupWindow::animation() const
{
    QTN_D(const OfficePopupWindow);
   return d.m_popupAnimation;
}

/*!
\brief Sets the start \a transparency of the office popup window.
*/ 
void OfficePopupWindow::setTransparency(qreal transparency)
{
    QTN_D(OfficePopupWindow);
    d.m_transparency = (int)(transparency*255.0);
}

/*!
\brief Returns the start transparency of the office popup window.
*/ 
qreal OfficePopupWindow::transparency() const
{
    QTN_D(const OfficePopupWindow);
    return (qreal)d.m_transparency/255.0;
}

/*!
\brief Sets the display time for showing office popup window. Parameter \a time is the value in milliseconds.
*/ 
void OfficePopupWindow::setDisplayTime(int time)
{
    QTN_D(OfficePopupWindow);
    d.m_notCloseTimer = time < 0;
    d.m_showDelay = time;
}

/*!
    \brief Returns the display time for showing office popup window. The returned value is in milliseconds.
*/ 
int OfficePopupWindow::displayTime() const
{
    QTN_D(const OfficePopupWindow);
    return d.m_showDelay;
}

/*!
    \brief Sets the speed of the animation. \a time - the value of the speed  in milliseconds.
*/ 
void OfficePopupWindow::setAnimationSpeed(int time)
{
    QTN_D(OfficePopupWindow);
    d.m_animationSpeed = time;
}

/*!
    \brief Returns the speed of the animation in milliseconds. 
*/ 
int OfficePopupWindow::animationSpeed() const
{
    QTN_D(const OfficePopupWindow);
    return d.m_animationSpeed;
}

/*!
\brief Sets the drag-and-drop \a enabled flag for the popup box. If \a enabled is true then the user may use grip to drag-and-drop window over desktop.
*/ 
void OfficePopupWindow::setDragDropEnabled(bool enabled)
{
    QTN_D(OfficePopupWindow);
    d.m_allowMove = enabled;
}

/*!
\brief Returns the drag-and-drop flag for the popup box. If returned value is true then the user may use grip to drag-and-drop window over desktop.
*/ 
bool OfficePopupWindow::dragDropEnabled() const
{
    QTN_D(const OfficePopupWindow);
    return d.m_allowMove;
}

/*!
    \brief Sets the start \a location of the office popup window. 
*/ 
void OfficePopupWindow::setLocation(PopupLocation location)
{
    QTN_D(OfficePopupWindow);
    d.m_popupLocation = location;
}

/*!
    \brief Returns the start location of the office popup window. 
*/ 
PopupLocation OfficePopupWindow::location() const
{
    QTN_D(const OfficePopupWindow);
    return d.m_popupLocation;
}

/*! \internal */
void OfficePopupWindow::showDelayTimer()
{
    QTN_D(OfficePopupWindow);
    if (!d.m_capture)
    {
        d.m_showDelayTimer.stop();
        d.onCollapsing();
    }
}

/*! \internal */
void OfficePopupWindow::collapsingTimer()
{
    QTN_D(OfficePopupWindow);
    d.animate(d.m_step);
    d.m_step--;

    if (d.m_step <= 0)
    {
        d.m_collapsingTimer.stop();
        if (!d.m_notCloseTimer)
            d.closePopup();
    }
}

/*! \internal */
void OfficePopupWindow::expandingTimer()
{
    QTN_D(OfficePopupWindow);
    d.animate(d.m_step);
    d.m_step--;

    if (d.m_step <= 0)
    {
        d.m_expandingTimer.stop();
        d.showPopup();
    }
}

/*! \reimp */
QSize OfficePopupWindow::sizeHint() const
{
    QTN_D(const OfficePopupWindow);
    QStyleOptionFrame option;
    d.initFormStyleOption(&option);
    return option.rect.size().expandedTo(QApplication::globalStrut());
}

/*!
Hides the office popup window.
*/ 
void OfficePopupWindow::closePopup()
{
    QTN_D(OfficePopupWindow);
    d.closePopup();
}

/*! \internal */
static void setChildStyle(QWidget* widget, QStyle* style)
{
    widget->setStyle(style);
    QList<QWidget*> widgets = widget->findChildren<QWidget*>();
    foreach (QWidget* w, widgets)
        ::setChildStyle(w, style);
}

/*! \reimp */
bool OfficePopupWindow::event(QEvent* event)
{
    QTN_D(OfficePopupWindow);
    switch (event->type())
    {
        case QEvent::MouseButtonPress :
            {
                d.handleMousePressEvent((QMouseEvent*)event);
                break;
            }
        case QEvent::MouseButtonRelease :
            {
                d.handleMouseReleaseEvent((QMouseEvent*)event);
                break;
            }
        case QEvent::MouseMove :
            {
                d.handleMouseMoveEvent((QMouseEvent*)event);
                break;
            }
        case QEvent::Show :
                ManagerPopup::getMngPopup().appendPopup(this);
            break;
        case QEvent::Hide :
            {
                emit aboutToHide();

                if (d.m_popupState == PopupStateShow)
                {
                    d.m_showDelayTimer.stop();
                    d.onCollapsing();
                }
                ManagerPopup::getMngPopup().removePopup(this);
            }
            break;
        case QEvent::StyleChange :
            {
  //              OfficeStyle* officeStyle = qobject_cast<OfficeStyle*>(style());
//                if (!officeStyle->isExistPopupProxy())
//                    officeStyle->createPopupProxy();
//                    officeStyle->setPopupDecoration(OfficeStyle::PopupSystemDecoration);

                PopupDrawHelper* popupStyle = qobject_cast<PopupDrawHelper*>(style());
                if (popupStyle)
                    popupStyle->refreshPalette();

                if (qobject_cast<PopupOffice2003DrawHelper*>(style()) || qobject_cast<PopupOffice2007DrawHelper*>(style()))
                {
                    d.m_titlePalette = palette();
                }

                if (d.m_form)
                    ::setChildStyle(d.m_form, style());
            }
            break;
        default:
            break;
    }
    return QWidget::event(event);
}

/*! \reimp */
void OfficePopupWindow::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QTN_D(OfficePopupWindow);
    QPainter p(this);

    QStyleOptionFrame opt;
    d.initFormStyleOption(&opt);
    style()->drawPrimitive(QStyle::PE_FrameWindow, &opt, &p, this);

    StyleOptionPopupTitleBar optTitle;
    d.initTitleBarStyleOption(&optTitle);
    style()->drawComplexControl(QStyle::CC_TitleBar, &optTitle, &p, this);
}

/*! \reimp */
void OfficePopupWindow::enterEvent(QEvent* event)
{
    Q_UNUSED(event);
    QTN_D(OfficePopupWindow);
    QMouseEvent ev(QEvent::MouseMove, QPoint(1, 1), Qt::NoButton, Qt::NoButton, Qt::NoModifier);
    d.handleMouseMoveEvent(&ev);
}

/*! \reimp */
void OfficePopupWindow::leaveEvent(QEvent* event)
{
    Q_UNUSED(event);
    QTN_D(OfficePopupWindow);
    QMouseEvent ev(QEvent::MouseMove, QPoint(-1, -1), Qt::NoButton, Qt::NoButton, Qt::NoModifier);
    d.handleMouseMoveEvent(&ev);
}

