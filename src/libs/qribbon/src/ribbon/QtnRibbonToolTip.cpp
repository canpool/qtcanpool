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
#include <qapplication.h>
#include <qdesktopwidget.h>
#include <qhash.h>
#include <qstyle.h>
#include <QIcon>
#include <qstyleoption.h>
#include <qstylepainter.h>
#include <qtextdocument.h>
#include <QToolTip>
#include <QBitmap>

#include "QtnRibbonToolTip.h"
#include "QtnRibbonStyle.h"
#include "QtnRibbonPrivate.h"


QTITAN_USE_NAMESPACE

struct QEffects
{
    enum Direction {
        LeftScroll  = 0x0001,
        RightScroll = 0x0002,
        UpScroll    = 0x0004,
        DownScroll  = 0x0008
    };

    typedef uint DirFlags;
};

extern void Q_GUI_EXPORT qScrollEffect(QWidget*, QEffects::DirFlags dir = QEffects::DownScroll, int time = -1);
extern void Q_GUI_EXPORT qFadeEffect(QWidget*, int time = -1);

QTITAN_BEGIN_NAMESPACE
/* RibbonToolTipPrivate */
class RibbonToolTipPrivate : public QObject
{
public:
    QTN_DECLARE_PUBLIC(RibbonToolTip)
public:
    explicit RibbonToolTipPrivate();

public:
    void updateTool();

public:
    int m_margin;
    int m_indent;
    int m_nMaxTipWidth;
    QBasicTimer m_hideTimer, m_expireTimer;
    bool m_fadingOut;
    QWidget* m_widget;
    QRect m_rect;
    QString m_text;
    QString m_textTitle;
    QIcon m_icon;
    static RibbonToolTip* m_instance;
    static RibbonToolTip::WrapMode m_wrapMode;
    static bool m_showIcon;
    QLabel* m_label;
};
QTITAN_END_NAMESPACE

RibbonToolTip* RibbonToolTipPrivate::m_instance = Q_NULL;
RibbonToolTip::WrapMode RibbonToolTipPrivate::m_wrapMode = RibbonToolTip::NoWrap;
bool RibbonToolTipPrivate::m_showIcon = true;
RibbonToolTipPrivate::RibbonToolTipPrivate()
{
    m_margin = 0;
    m_fadingOut = false;
    m_widget = Q_NULL;
    m_indent = -1;
    m_nMaxTipWidth = 160;
    m_label = Q_NULL;
}

void RibbonToolTipPrivate::updateTool()
{
    QTN_P(RibbonToolTip);
    if (m_label)
    {
        m_label->setText(m_text);
        m_label->setWordWrap(Qt::mightBeRichText(m_text));
        m_label->adjustSize();
    }

    p.updateGeometry();
    p.update(p.contentsRect());
}


/* RibbonToolTip */
/*!
\brief Constructs RibbonToolTip object with the given \a parent. Parameter \a title - is a title of the tooltip, \a text - is a text of the tooltip.
*/ 
RibbonToolTip::RibbonToolTip(const QString& title, const QString& text, const QIcon& icon, QWidget* parent)
    : QFrame(parent, Qt::ToolTip | Qt::BypassGraphicsProxyWidget)
{
    delete RibbonToolTipPrivate::m_instance;
    RibbonToolTipPrivate::m_instance = this;

    QTN_INIT_PRIVATE(RibbonToolTip);
    QTN_D(RibbonToolTip);

    if (wordWrap() == NativeWrap)
        d.m_label = new QLabel(this);

    if (d.m_label)
    {
        d.m_label->setForegroundRole(QPalette::ToolTipText);
        d.m_label->setBackgroundRole(QPalette::ToolTipBase);
        d.m_label->setPalette(QToolTip::palette());
        d.m_label->setFont(QToolTip::font());
        d.m_label->ensurePolished();
        d.m_label->setMargin(1 + style()->pixelMetric(QStyle::PM_ToolTipLabelFrameWidth, 0, this));
        d.m_label->setFrameStyle(QFrame::NoFrame);
    }

    QWidgetData* pDate = qt_qwidget_data(this);
    d.m_nMaxTipWidth = pDate->crect.width()/4;

    setForegroundRole(QPalette::ToolTipText);
    setBackgroundRole(QPalette::ToolTipBase);
    setPalette(QToolTip::palette());
    setFont(QToolTip::font());
    ensurePolished();
    setMargin(1 + style()->pixelMetric(QStyle::PM_ToolTipLabelFrameWidth, 0, this));
    setFrameStyle(QFrame::NoFrame);
    qApp->installEventFilter(this);
    setWindowOpacity(style()->styleHint(QStyle::SH_ToolTipLabel_Opacity, 0, this) / 255.0);
    setMouseTracking(true);
    d.m_fadingOut = false;
    setIcon(isIconVisible() ? icon : QIcon());
    reuseTip(title, text);
}

/*!
\brief Destructor of the RibbonToolTip object.
*/ 
RibbonToolTip::~RibbonToolTip()
{
    RibbonToolTipPrivate::m_instance = Q_NULL;
    QTN_FINI_PRIVATE();
}

RibbonToolTip* RibbonToolTip::instance()
{
    return RibbonToolTipPrivate::m_instance;
}

/*!
\brief Shows tooltip in \a pos position. Parameter \a title - is a title of the tooltip, \a text - is a text of the tooltip, \a icon - the icon of the tooltip. Parameter \a widget - reference to the widget for calculating the position of tooltip.
*/ 
void RibbonToolTip::showToolTip(const QPoint &pos, const QString& title, const QString& text, const QIcon& icon, QWidget* w)
{
    QRect rect;
    // a tip does already exist
    if (RibbonToolTipPrivate::m_instance && RibbonToolTipPrivate::m_instance->isVisible())
    {
        // empty text means hide current tip
        if (text.isEmpty())
        {
            RibbonToolTipPrivate::m_instance->hideTip();
            return;
        }
        else if (!RibbonToolTipPrivate::m_instance->fadingOut())
        {
            // If the tip has changed, reuse the one
            // that is showing (removes flickering)
            QPoint localPos = pos;
            if (w)
                localPos = w->mapFromGlobal(pos);
            if (RibbonToolTipPrivate::m_instance->tipChanged(localPos, text, title, w))
            {
                RibbonToolTipPrivate::m_instance->setIcon(isIconVisible() ? icon : QIcon());
                RibbonToolTipPrivate::m_instance->reuseTip(title, text);
                RibbonToolTipPrivate::m_instance->setTipRect(w, rect);
                RibbonToolTipPrivate::m_instance->placeTip(pos, w);
            }
            return;
        }
    }
    // no tip can be reused, create new tip:
    if (!text.isEmpty())
    {
#ifndef Q_WS_WIN
        new RibbonToolTip(title, text, icon, w); // sets TipLabel::m_pInstanceto itself
#else
        // On windows, we can't use the widget as parent otherwise the window will be
        // raised when the tooltip will be shown
        new RibbonToolTip(title, text, icon, QApplication::desktop()->screen(RibbonToolTip::getTipScreen(pos, w)));
#endif
        RibbonToolTipPrivate::m_instance->setTipRect(w, rect);
        RibbonToolTipPrivate::m_instance->placeTip(pos, w);
        RibbonToolTipPrivate::m_instance->setObjectName(QLatin1String("qtntooltip_label"));


#if !defined(QT_NO_EFFECTS) && !defined(Q_WS_MAC)
        if (QApplication::isEffectEnabled(Qt::UI_FadeTooltip))
            qFadeEffect(RibbonToolTipPrivate::m_instance);
        else if (QApplication::isEffectEnabled(Qt::UI_AnimateTooltip))
            qScrollEffect(RibbonToolTipPrivate::m_instance);
        else
            RibbonToolTipPrivate::m_instance->show();
#else
        RibbonToolTipPrivate::m_instance->show();
#endif
    }
}

/*!
\brief Hides the tooltip window.
*/ 
void RibbonToolTip::hideToolTip() 
{ 
    showToolTip(QPoint(), QString(), QString(), QIcon()); 
}

/*!
\brief Returns visibility of the tooltip at the moment.
*/ 
bool RibbonToolTip::isToolTipVisible()
{
    return (RibbonToolTipPrivate::m_instance != 0 && RibbonToolTipPrivate::m_instance->isVisible());
}
/*
QString RibbonToolTip::textToolTip()
{
    if (RibbonToolTipPrivate::m_instance)
        return RibbonToolTipPrivate::m_instance->text();
    return QString();
}
*/
/*!
Sets the wrap \a mode for the tolltip's text.
*/ 
void RibbonToolTip::setWrapMode(WrapMode mode)
{
    RibbonToolTipPrivate::m_wrapMode = mode;
}

/*!
Returns the wrap mode for the tooltip's text.
*/
RibbonToolTip::WrapMode RibbonToolTip::wordWrap()
{
    return RibbonToolTipPrivate::m_wrapMode;
}

/*!
Sets the visibility of the tooltip's icon to \a visible. Method is static.
*/ 
void RibbonToolTip::setIconVisible(bool visible)
{
    RibbonToolTipPrivate::m_showIcon = visible;
}

/*!
Returns the visibility of the tooltip's icon. Method is static.
*/ 
bool RibbonToolTip::isIconVisible()
{
    return RibbonToolTipPrivate::m_showIcon;
}

/*!
Returns the margin \a mode for the tolltip's text in pixels.
*/ 
int RibbonToolTip::margin() const
{
    QTN_D(const RibbonToolTip);
    return d.m_margin;
}

/*!
Sets the margin \a mode for the tolltip's text in in pixels.
*/ 
void RibbonToolTip::setMargin(int margin)
{
    QTN_D(RibbonToolTip);
    if (d.m_margin == margin)
        return;
    d.m_margin = margin;
    d.updateTool();
}

void RibbonToolTip::setTitle(const QString& text)
{
    QTN_D(RibbonToolTip);
    if (d.m_textTitle == text)
        return;
    d.m_textTitle = text;
    d.updateTool();
}

const QString& RibbonToolTip::title() const
{
    QTN_D(const RibbonToolTip);
    return d.m_textTitle;
}

void RibbonToolTip::setText(const QString& text)
{
    QTN_D(RibbonToolTip);
    if (d.m_text == text)
        return;
    d.m_text = text;
    d.updateTool();
}

const QString& RibbonToolTip::text() const
{
    QTN_D(const RibbonToolTip);
    return d.m_text;
}

void RibbonToolTip::setIcon(const QIcon& icon)
{
    QTN_D(RibbonToolTip);
    d.m_icon = icon;
    d.updateTool();
}

const QIcon& RibbonToolTip::icon() const
{
    QTN_D(const RibbonToolTip);
    return d.m_icon;
}

void RibbonToolTip::restartExpireTimer()
{
    QTN_D(RibbonToolTip);
    int time = 10000 + 40 * qMax(0, text().length()-100);
    d.m_expireTimer.start(time, this);
    d.m_hideTimer.stop();
}

void RibbonToolTip::reuseTip(const QString& textTitle, const QString& text)
{
    setText(text);
    setTitle(textTitle);
    QFontMetrics fm(font());
    QSize extra(1, 0);
    // Make it look good with the default RibbonToolTip font on Mac, which has a small descent.
    if (fm.descent() == 2 && fm.ascent() >= 11)
        ++extra.rheight();

    resize(sizeHint() + extra);
    restartExpireTimer();
}

/*! \reimp */
QSize RibbonToolTip::sizeHint() const
{
    QTN_D(const RibbonToolTip);

    QFontMetrics fm = fontMetrics();

    QRect rcMargin(margin(), margin(), margin(), margin());
    QSize szMargin(3 + rcMargin.left() + rcMargin.right() + 3, 3 + rcMargin.top() + rcMargin.bottom() + 3);

    int flags = Qt::TextExpandTabs | Qt::TextHideMnemonic;

    bool drawTitle = !d.m_textTitle.isEmpty();
    bool drawImage = !d.m_icon.isNull();
    bool drawImageTop = true;
    QSize szImage(0, 0);
    QSize szTitle(0, 0);

    if (drawTitle)
    {
        int w = fm.averageCharWidth() * 80;
        QRect rcTitle = fm.boundingRect(0, 0, w, 2000, flags | Qt::TextSingleLine, d.m_textTitle);
        szTitle = QSize(rcTitle.width() + fm.width(QLatin1Char('x'))*2 + 4, rcTitle.height() + 20);
    }

    if (drawImage)
    {
        QSize szIcon(0, 0);
        if (!d.m_icon.availableSizes().isEmpty())
            szIcon = d.m_icon.availableSizes().first();

        drawImageTop = (szIcon.height() <= 16);

        if (drawImageTop)
        {
            if (!drawTitle)
                szImage.setWidth(szIcon.height() + 3);
            else
                szTitle.setWidth(szTitle.width() + szIcon.width() + 1);
        }
        else
            szImage.setWidth(szIcon.width() + 5);
        szImage.setHeight(szIcon.height());
    }

    QSize szText;
    if (!d.m_label)
    {
        int nMaxTipWidth = d.m_nMaxTipWidth;
        QRect rcText(QPoint(0, 0), QSize(nMaxTipWidth - szMargin.width(), 0));
        rcText = fm.boundingRect(rcText.left(), rcText.top(), rcText.width(), rcText.height(), flags | Qt::TextWordWrap, d.m_text);
        szText = rcText.size();
    }
    else
        szText = d.m_label->sizeHint();

    QSize sz(0, 0);
    sz.setHeight(qMax(szImage.height(), szText.height()));
    sz.setWidth(szImage.width() + szText.width());

    if (drawTitle)
    {
        sz.setWidth(qMax(sz.width() + 25, szTitle.width()));
        sz.setHeight(sz.height() + szTitle.height());
    }

    sz += szMargin;
    return sz;
}

/*! \reimp */
void RibbonToolTip::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QTN_D(RibbonToolTip);

    QStylePainter p(this);
    drawFrame(&p);

    QStyleOptionFrame opt;
    opt.init(this);

    p.drawPrimitive(QStyle::PE_PanelTipLabel, opt);
    
    QRect rc = contentsRect();
    rc.adjust(margin(), margin(), -margin(), -margin());
    rc.adjust(3, 3, -3, -3);

    QRect rcTitle(rc.left() + 2, rc.top() + 2, rc.right() - 2, rc.bottom());

    int flags = Qt::TextExpandTabs | Qt::TextHideMnemonic;

    bool drawTitle = !d.m_textTitle.isEmpty();
    bool drawImage = !d.m_icon.isNull();
    bool drawImageTop = true;
    QSize szImage(0, 0);

    if (drawImage)
    {
        if (!d.m_icon.availableSizes().isEmpty())
            szImage = d.m_icon.availableSizes().first();

        drawImageTop = (szImage.height() <= 16);

        if (drawImageTop)
        {
            QPoint ptIcon = rc.topLeft();
            QPixmap pm = d.m_icon.pixmap(szImage, QIcon::Normal, QIcon::On);
            p.drawPixmap(ptIcon, pm);

            if (drawTitle) 
                rcTitle.setLeft(rcTitle.left()+szImage.width() + 1); 
            else 
                rc.setLeft(rc.left()+szImage.width() + 3);
        }
    }
    if (drawTitle)
    {
        QFont oldFont = p.font();
        QFont fnt = oldFont;
        fnt.setBold(true);
        p.setFont(fnt);
        style()->drawItemText(&p, rcTitle, flags | Qt::TextSingleLine, opt.palette, isEnabled(), d.m_textTitle, QPalette::ToolTipText);
        p.setFont(oldFont);

        rc.setTop(rc.top() + p.boundingRect(rcTitle, flags | Qt::TextSingleLine, d.m_textTitle).height());
        rc.adjust(10, 12, -15, 0);
    }

    if (drawImage && !drawImageTop)
    {
        QPoint ptIcon = rc.topLeft();
        QPixmap pm = d.m_icon.pixmap(szImage, QIcon::Normal, QIcon::On);
        p.drawPixmap(ptIcon, pm);
        rc.setLeft(rc.left() + szImage.width() + 5);
    }

    if (d.m_label)
        d.m_label->move(rc.topLeft());
    else
        style()->drawItemText(&p, rc, flags | Qt::TextWordWrap, opt.palette, isEnabled(), d.m_text, QPalette::ToolTipText);
    p.end();
}

/*! \reimp */
void RibbonToolTip::resizeEvent(QResizeEvent* event)
{
    QStyleHintReturnMask frameMask;
    QStyleOption option;
    option.init(this);
    if (style()->styleHint(QStyle::SH_ToolTip_Mask, &option, this, &frameMask))
        setMask(frameMask.region);

    QFrame::resizeEvent(event);
}

/*! \reimp */
void RibbonToolTip::mouseMoveEvent(QMouseEvent* event)
{
    QTN_D(RibbonToolTip);
    if (d.m_rect.isNull())
        return;
    QPoint pos = event->globalPos();
    if (d.m_widget)
        pos = d.m_widget->mapFromGlobal(pos);
    if (!d.m_rect.contains(pos))
        hideTip();
    QFrame::mouseMoveEvent(event);
}

void RibbonToolTip::hideTip()
{
    QTN_D(RibbonToolTip);
    if (!d.m_hideTimer.isActive())
        d.m_hideTimer.start(300, this);
}

void RibbonToolTip::closeToolTip()
{
    close(); // to trigger QEvent::Close which stops the animation
    deleteLater();
}

void RibbonToolTip::setTipRect(QWidget* w, const QRect& r)
{
    QTN_D(RibbonToolTip);
    if (!d.m_rect.isNull() && !w)
        qWarning("RibbonToolTip::setTipRect: Cannot pass null widget if rect is set");
    else
    {
        d.m_widget = w;
        d.m_rect = r;
    }
}

/*! \reimp */
void RibbonToolTip::timerEvent(QTimerEvent* event)
{
    QTN_D(RibbonToolTip);
    if (event->timerId() == d.m_hideTimer.timerId() || event->timerId() == d.m_expireTimer.timerId())
    {
        d.m_hideTimer.stop();
        d.m_expireTimer.stop();
        closeToolTip();
    }
}

/*! \reimp */
bool RibbonToolTip::eventFilter(QObject *o, QEvent* event)
{
    QTN_D(RibbonToolTip);
    switch (event->type()) 
    {
        case QEvent::Leave:
                hideTip();
            break;
        case QEvent::WindowActivate:
        case QEvent::WindowDeactivate:
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseButtonDblClick:
        case QEvent::FocusIn:
        case QEvent::FocusOut:
        case QEvent::Wheel:
                closeToolTip();
            break;
        case QEvent::MouseMove:
            if (o == d.m_widget && !d.m_rect.isNull() && !d.m_rect.contains(static_cast<QMouseEvent*>(event)->pos()))
                hideTip();
        default:
            break;
    }
    return false;
}

int RibbonToolTip::getTipScreen(const QPoint& pos, QWidget* w)
{
    if (QApplication::desktop()->isVirtualDesktop())
        return QApplication::desktop()->screenNumber(pos);
    else
        return QApplication::desktop()->screenNumber(w);
}

void RibbonToolTip::placeTip(const QPoint& pos, QWidget* w)
{
    QRect screen = QApplication::desktop()->screenGeometry(getTipScreen(pos, w));

    QPoint p = pos;
    if (p.x() + this->width() > screen.x() + screen.width())
        p.rx() -= 4 + this->width();
    if (p.y() + this->height() > screen.y() + screen.height())
        p.ry() -= 24 + this->height();
    if (p.y() < screen.y())
        p.setY(screen.y());
    if (p.x() + this->width() > screen.x() + screen.width())
        p.setX(screen.x() + screen.width() - this->width());
    if (p.x() < screen.x())
        p.setX(screen.x());
    if (p.y() + this->height() > screen.y() + screen.height())
        p.setY(screen.y() + screen.height() - this->height());

    this->move(p);
}

bool RibbonToolTip::tipChanged(const QPoint& pos, const QString& text, const QString& textTitle, QObject* o)
{
    QTN_D(RibbonToolTip);
    if (RibbonToolTipPrivate::m_instance->text() != text)
        return true;

    if (RibbonToolTipPrivate::m_instance->title() != textTitle)
        return true;

    if (o != d.m_widget)
        return true;

    if (!d.m_rect.isNull())
        return !d.m_rect.contains(pos);
    else
        return false;
}

bool RibbonToolTip::fadingOut() const
{
    QTN_D(const RibbonToolTip);
    return d.m_fadingOut;
}

QTITAN_BEGIN_NAMESPACE
/* RibbonKeyTipPrivate */
class RibbonKeyTipPrivate : public QObject
{
public:
    QTN_DECLARE_PUBLIC(RibbonKeyTip)
public:
    explicit RibbonKeyTipPrivate();
public:
    QString strTip_;
    QString strCaption_;
    QString strPrefix_;
    QWidget* owner_;
    QAction* action_;
    QPoint pt_;
    bool enabled_;
    bool visible_;
    QWidget* bars_;
    uint align_;
    bool explicit_;
};
QTITAN_END_NAMESPACE

RibbonKeyTipPrivate::RibbonKeyTipPrivate()
{
    owner_ = Q_NULL;
    action_ = Q_NULL;
    bars_ = Q_NULL;
    enabled_ = false;
    align_ = 0;
    visible_ = true;
    explicit_ = false;
}

/*!
    \class Qtitan::RibbonKeyTip
    \internal
*/
RibbonKeyTip::RibbonKeyTip(QWidget* bars, QWidget* owner, const QString& caption, 
    const QPoint& pt, uint align, bool enabled, QAction* action)
    : QFrame(bars, Qt::ToolTip | Qt::BypassGraphicsProxyWidget)
{
    QTN_INIT_PRIVATE(RibbonKeyTip);
    QTN_D(RibbonKeyTip);
    d.action_ = action;
    d.owner_ = owner;
    d.strCaption_ = caption;
    d.bars_ = bars;
    d.pt_ = pt;
    d.enabled_ = enabled;
    d.align_ = align;

    setForegroundRole(QPalette::ToolTipText);
    setBackgroundRole(QPalette::ToolTipBase);
    QPalette pal = QToolTip::palette();
    pal.setColor(QPalette::Light, pal.color(QPalette::ToolTipText));
    setPalette(pal);
    setFont(QToolTip::font());
    ensurePolished();

    if (!enabled)
        setWindowOpacity(0.5);
}

RibbonKeyTip::~RibbonKeyTip()
{
    QTN_FINI_PRIVATE();
}

QPoint RibbonKeyTip::posTip() const
{
    QTN_D(const RibbonKeyTip);
    return d.pt_;
}

void RibbonKeyTip::setVisibleTip(bool visible)
{
    QTN_D(RibbonKeyTip);
    d.visible_ = visible;
}

bool RibbonKeyTip::isVisibleTip() const
{
    QTN_D(const RibbonKeyTip);
    return d.visible_;
}

bool RibbonKeyTip::isEnabledTip() const
{
    QTN_D(const RibbonKeyTip);
    return d.enabled_;
}

QWidget* RibbonKeyTip::getBars() const
{
    QTN_D(const RibbonKeyTip);
    return d.bars_;
}

uint RibbonKeyTip::getAlign() const
{
    QTN_D(const RibbonKeyTip);
    return d.align_;
}

void RibbonKeyTip::setExplicit(bool exp)
{
    QTN_D(RibbonKeyTip);
    d.explicit_ = exp;
}

bool RibbonKeyTip::isExplicit() const
{
    QTN_D(const RibbonKeyTip);
    return d.explicit_;
}

QString RibbonKeyTip::getStringTip() const
{
    QTN_D(const RibbonKeyTip);
    return d.strTip_;
}

void RibbonKeyTip::setStringTip(const QString& str)
{
    QTN_D(RibbonKeyTip);
    d.strTip_ = str;
}

QString RibbonKeyTip::getCaption() const
{
    QTN_D(const RibbonKeyTip);
    return d.strCaption_;
}

void RibbonKeyTip::setCaption(const QString& str)
{
    QTN_D(RibbonKeyTip);
    d.strCaption_ = str;
}

QString RibbonKeyTip::getPrefix() const
{
    QTN_D(const RibbonKeyTip);
    return d.strPrefix_;
}

void RibbonKeyTip::setPrefix(const QString& pref)
{
    QTN_D(RibbonKeyTip);
    d.strPrefix_ = pref;
}

QWidget* RibbonKeyTip::getOwner() const
{
    QTN_D(const RibbonKeyTip);
    return d.owner_;
}

QAction* RibbonKeyTip::getAction() const
{
    QTN_D(const RibbonKeyTip);
    return d.action_;
}

void RibbonKeyTip::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QTN_D(RibbonKeyTip);

    QPainter p(this);
    QStyleOption opt;
    opt.init(this);

    if (d.enabled_)
        opt.state |= QStyle::State_Enabled;
    else
        opt.state &= ~QStyle::State_Enabled;

    style()->drawPrimitive((QStyle::PrimitiveElement)RibbonStyle::PE_RibbonKeyTip, &opt, &p, this);
    style()->drawItemText(&p, opt.rect, Qt::AlignVCenter | Qt::AlignCenter | Qt::TextSingleLine | Qt::TextWordWrap, 
        opt.palette, d.enabled_, d.strTip_, QPalette::ToolTipText);
}

void RibbonKeyTip::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event);

    QBitmap maskBitmap(width(), height());
    maskBitmap.clear();

    QPainter p(&maskBitmap);
    p.setBrush(Qt::black);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    p.drawRoundedRect(0, 0, width(), height(), 2, 2, Qt::AbsoluteSize);
#else
    p.drawRoundedRect(0, 0, width()-1, height()-1, 2, 2, Qt::AbsoluteSize);
#endif
    p.end();
    setMask(maskBitmap);
}
