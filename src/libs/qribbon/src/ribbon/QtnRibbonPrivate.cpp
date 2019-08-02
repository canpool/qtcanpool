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
#include <QStyleOption>
#include <QResizeEvent>
#include <QDesktopWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPainter>
#include <QLayout>
#include <QEvent>

#include "QtnRibbonPrivate.h"
#include "QtnRibbonGroupPrivate.h"
#include "QtnRibbonGroup.h"
#include "QtnRibbonStyle.h"
#include "QtnRibbonPage.h"

QTITAN_USE_NAMESPACE

static const int hMargin = 4;

static int keyTipEventNumber = -1;
static int showKeyTipEventNumber = -1;
static int hideKeyTipEventNumber = -1;
static int minimizedEventNumber = -1;

QTITAN_BEGIN_NAMESPACE
/* ExWidgetWrapperPrivate */
class ExWidgetWrapperPrivate : public QObject
{
public:
    QTN_DECLARE_PUBLIC(ExWidgetWrapper)
public:
    explicit ExWidgetWrapperPrivate();

public:
    void init(QWidget* widget);

public:
    QIcon m_icon;
    QWidget* m_widget;
    QString m_labelText;
    int m_length;
    bool m_align;
};
QTITAN_END_NAMESPACE

/* ExWidgetWrapperPrivate */
ExWidgetWrapperPrivate::ExWidgetWrapperPrivate()
    : m_widget(Q_NULL)
{
}

void ExWidgetWrapperPrivate::init(QWidget* widget)
{
    Q_ASSERT(widget != Q_NULL);
    QTN_P(ExWidgetWrapper);

    m_widget = widget;
    m_align = false;
    m_length = -1;
    m_widget->setParent(&p);

    p.setMinimumWidth(m_widget->minimumWidth());
    widget->setMinimumWidth(0);
    p.setMaximumWidth(m_widget->maximumWidth());

    p.setFocusPolicy(m_widget->focusPolicy());
    p.setAttribute(Qt::WA_InputMethodEnabled);
    p.setSizePolicy(m_widget->sizePolicy());
    p.setMouseTracking(true);
    p.setAcceptDrops(true);
    p.setAttribute(Qt::WA_MacShowFocusRect, true);

    m_widget->setFocusProxy(&p);
    m_widget->setAttribute(Qt::WA_MacShowFocusRect, false);

    p.setToolTip(m_widget->toolTip());
    m_widget->setToolTip("");
}

/*!
\class Qtitan::ExWidgetWrapper
\internal
*/
ExWidgetWrapper::ExWidgetWrapper(QWidget* parent, QWidget* widget)
    : QWidget(parent)
{
    QTN_INIT_PRIVATE(ExWidgetWrapper);
    QTN_D(ExWidgetWrapper);
    d.init(widget);
}

ExWidgetWrapper::~ExWidgetWrapper()
{
    QTN_FINI_PRIVATE();
}

QString ExWidgetWrapper::labelText() const
{
    QTN_D(const ExWidgetWrapper);
    return d.m_labelText;
}

void ExWidgetWrapper::setLabelText(const QString& text)
{
    QTN_D(ExWidgetWrapper);
    d.m_labelText = text;
}

void ExWidgetWrapper::setIcon(const QIcon& icon)
{
    QTN_D(ExWidgetWrapper);
    d.m_icon = icon;
}

QIcon ExWidgetWrapper::icon() const
{
    QTN_D(const ExWidgetWrapper);
    return d.m_icon;
}

void ExWidgetWrapper::setAlignWidget(bool align)
{
    QTN_D(ExWidgetWrapper);
    d.m_align = align;
}

bool ExWidgetWrapper::alignWidget() const
{
    QTN_D(const ExWidgetWrapper);
    return d.m_align;
}

void ExWidgetWrapper::setLengthLabel(int length)
{
    QTN_D(ExWidgetWrapper);
    d.m_length = length;
}

void ExWidgetWrapper::resizeEvent(QResizeEvent* event)
{
    updateGeometries();
    QWidget::resizeEvent(event);
}

void ExWidgetWrapper::updateGeometries()
{
    QStyleOptionFrameV2 panel;
    initStyleOption(&panel);

    QRect rect = panel.rect;

    QTN_D(ExWidgetWrapper);
    if (!d.m_icon.isNull())
    {
        QSize sz = d.m_icon.actualSize(QSize(16, 16));
        rect.adjust(sz.width() + hMargin, 0, hMargin, 0);
    }

    if (!d.m_labelText.isEmpty())
    {
        if (d.m_length == -1)
        {
            QFontMetrics fm = fontMetrics();
            QSize sz = fm.size(Qt::TextHideMnemonic, d.m_labelText);
            int width =  sz.width() + fm.width(QLatin1Char('x'));
            rect.adjust(width, 0, -hMargin, 0);
            d.m_widget->setGeometry(rect);
        }
        else
        {
            rect.adjust(d.m_length + 1, 0, -hMargin, 0);
            d.m_widget->setGeometry(rect);
        }
    }
}

void ExWidgetWrapper::initStyleOption(QStyleOptionFrameV2* option) const
{
    QTN_D(const ExWidgetWrapper);

    option->initFrom(this);
    option->rect = contentsRect();
    option->lineWidth = qobject_cast<QLineEdit*>(d.m_widget) ? style()->pixelMetric(QStyle::PM_DefaultFrameWidth, option, this) : 0;
    option->midLineWidth = 0;
    option->state |= QStyle::State_Sunken;

    if (isReadOnly())
        option->state |= QStyle::State_ReadOnly;

#ifdef QT_KEYPAD_NAVIGATION
    if (hasEditFocus())
        option->state |= QStyle::State_HasEditFocus;
#endif
    option->features = QStyleOptionFrameV2::None;
}

bool ExWidgetWrapper::isReadOnly() const
{
    QTN_D(const ExWidgetWrapper);

    bool readOnly = false;
    if (QLineEdit* pEdit = qobject_cast<QLineEdit*>(d.m_widget))
        readOnly = pEdit->isReadOnly();
    else if (QComboBox* pComboBox = qobject_cast<QComboBox*>(d.m_widget))
        readOnly = pComboBox->isEditable();
    else if (QAbstractSpinBox* pSpinBox = qobject_cast<QAbstractSpinBox*>(d.m_widget))
        readOnly = pSpinBox->isReadOnly();

    return readOnly;
}

void ExWidgetWrapper::focusInEvent(QFocusEvent* event)
{
    QTN_D(ExWidgetWrapper);
    QCoreApplication::sendEvent(d.m_widget, event);
    QWidget::focusInEvent(event);
}

void ExWidgetWrapper::focusOutEvent(QFocusEvent* event)
{
    QTN_D(ExWidgetWrapper);
    QCoreApplication::sendEvent(d.m_widget, event);
    QWidget::focusOutEvent(event);
}

bool ExWidgetWrapper::event(QEvent* event)
{
    QTN_D(ExWidgetWrapper);

    if (!d.m_widget)
        return false;

    switch(event->type()) 
    {
        case QEvent::ShortcutOverride:
        case QEvent::KeyPress :
            {
                class QtnWidget : public QWidget { friend class Qtitan::ExWidgetWrapper; };
                return ((QtnWidget*)d.m_widget)->event(event);
            }
        default:
            break;
    }
    return QWidget::event(event);
}

void ExWidgetWrapper::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QTN_D(ExWidgetWrapper);

    QPainter p(this);
    QStyleOptionFrameV2 panel;
    initStyleOption(&panel);

    if (!d.m_icon.isNull())
    {
        QRect rc = panel.rect;
        QPixmap pm;
        QSize pmSize = d.m_icon.actualSize(QSize(16, 16));
        QIcon::State state = panel.state & QStyle::State_On ? QIcon::On : QIcon::Off;
        QIcon::Mode mode;
        if (!(panel.state & QStyle::State_Enabled))
            mode = QIcon::Disabled;
        else if ((panel.state & QStyle::State_MouseOver) && (panel.state & QStyle::State_AutoRaise))
            mode = QIcon::Active;
        else
            mode = QIcon::Normal;
        pm = d.m_icon.pixmap(panel.rect.size().boundedTo(pmSize), mode, state);
        pmSize = pm.size();

        rc.setWidth(pmSize.width());
        rc.adjust(hMargin, 0, hMargin, 0);
        style()->drawItemPixmap(&p, rc, Qt::AlignCenter, pm);
        panel.rect.setLeft(panel.rect.left() + pmSize.width() + 3 + hMargin);
    }
    if (!d.m_labelText.isEmpty())
    {
        style()->drawItemText(&p, panel.rect, Qt::AlignLeft | Qt::AlignVCenter, panel.palette, 
            panel.state & QStyle::State_Enabled, d.m_labelText, QPalette::ButtonText);
    }
}

QVariant ExWidgetWrapper::inputMethodQuery(Qt::InputMethodQuery property) const
{
    QTN_D(const ExWidgetWrapper);
    return d.m_widget->inputMethodQuery(property);
}

QSize ExWidgetWrapper::sizeHint() const
{
    QTN_D(const ExWidgetWrapper);
    QSize sz = d.m_widget->sizeHint();
    return sz;
}

void ExWidgetWrapper::inputMethodEvent(QInputMethodEvent* event)
{
    QTN_D(const ExWidgetWrapper);
    QCoreApplication::sendEvent(d.m_widget, (QEvent*)event);
}




/* ReducedGroupPopupMenu */
ReducedGroupPopupMenu::ReducedGroupPopupMenu(QWidget* p)
    : QWidget(p, Qt::Window | Qt::Popup)
    , m_group(Q_NULL)
    , m_parent(Q_NULL)
    , m_noReplayFor(Q_NULL)
{
    setAttribute(Qt::WA_CustomWhatsThis);
    setAttribute(Qt::WA_X11NetWmWindowTypePopupMenu);
    setMouseTracking(style()->styleHint(QStyle::SH_Menu_MouseTracking, 0, this));
}

ReducedGroupPopupMenu::~ReducedGroupPopupMenu()
{
}

void ReducedGroupPopupMenu::setNoReplayFor(QWidget* noReplayFor)
{
    m_noReplayFor = noReplayFor;
}

void ReducedGroupPopupMenu::setGroup(RibbonGroup* gr)
{
    m_group = gr;
    m_parent = m_group->parentWidget();
    m_group->setParent(this);
    connect(m_group, SIGNAL(actionTriggered(QAction*)), this, SLOT(actionTriggered(QAction*)));

    m_group->setFocusProxy(this);
    m_group->show();
}

RibbonGroup* ReducedGroupPopupMenu::getGroup() const
{
    return m_group;
}

void ReducedGroupPopupMenu::actionTriggered(QAction *action)
{
    Q_UNUSED(action);
    hide();
}

QSize ReducedGroupPopupMenu::sizeHint() const
{
    if (m_group) 
        return m_group->sizeHint();
    return QWidget::sizeHint();
}

void ReducedGroupPopupMenu::showEvent(QShowEvent* event)
{
    if (m_group)
        m_group->show();
    QWidget::showEvent(event);
}

void ReducedGroupPopupMenu::hideEvent(QHideEvent* event)
{
    emit aboutToHide();
    m_group->disconnect(this);
    m_group->setParent(m_parent);
    m_parent = Q_NULL;
    m_group = Q_NULL;
    parentWidget()->update();
    QWidget::hideEvent(event);
}

void ReducedGroupPopupMenu::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter p(this);
    QStyleOption opt;
    opt.init(this);
    style()->drawControl((QStyle::ControlElement)RibbonStyle::CE_RibbonGroups, &opt, &p, this);
}

void ReducedGroupPopupMenu::mousePressEvent(QMouseEvent* event)
{
    if (!rect().contains(event->pos())) 
    {
        if (QRect(m_noReplayFor->mapToGlobal(QPoint()), m_noReplayFor->size()).contains(event->globalPos()))
            setAttribute(Qt::WA_NoMouseReplay);
        hide();
        return;
    }
}

/*!
\class Qtitan::RibbonGroupWrapper
\internal
*/
RibbonGroupWrapper::RibbonGroupWrapper(QWidget* parent)
    : QToolButton(parent)
{
    m_ribbonGroup = Q_NULL;
    m_eventLoop = Q_NULL;
//    m_reduced = false;
    m_hasPopup = false;

    ensurePolished();

    m_popupMenu = new ReducedGroupPopupMenu(this);
    setAttribute(Qt::WA_LayoutUsesWidgetRect);

    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
    setPopupMode(QToolButton::MenuButtonPopup);
    m_popupMenu->setNoReplayFor(this);
}

RibbonGroupWrapper::~RibbonGroupWrapper()
{
    if (m_eventLoop)
        m_eventLoop->exit();

    if (m_ribbonGroup)
        m_ribbonGroup->removeEventFilter(this);
}

void RibbonGroupWrapper::setGroup(RibbonGroup* rg)
{
    m_ribbonGroup = rg;
    m_ribbonGroup->installEventFilter(this);
    setText(m_ribbonGroup->title());
}

RibbonGroup* RibbonGroupWrapper::getGroup() const
{
    return m_ribbonGroup;
}

void RibbonGroupWrapper::setReducedGroup(bool reduced)
{
    if (m_ribbonGroup->isVisible())
    {
        m_ribbonGroup->qtn_d().m_reduced = reduced;
        m_ribbonGroup->hide();
        resize(sizeHint());
    }
}

bool RibbonGroupWrapper::isReducedGroup() const 
{ 
    return m_ribbonGroup ? m_ribbonGroup->qtn_d().m_reduced : false; 
}

void RibbonGroupWrapper::resetReducedGroup()
{
    if (m_ribbonGroup->qtn_d().m_reduced)
    {
        m_ribbonGroup->qtn_d().m_reduced = false;
        m_ribbonGroup->show();
        m_ribbonGroup->resize(m_realSize);
    }
}

void RibbonGroupWrapper::resetPopopGroup()
{
    if (m_eventLoop)
        m_eventLoop->exit(); 
    m_hasPopup = false;
}

QSize RibbonGroupWrapper::realSize() const
{
    return m_realSize;
}

QSize RibbonGroupWrapper::reducedSize() const
{
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    return QSize(style()->pixelMetric((QStyle::PixelMetric)RibbonStyle::PM_RibbonReducedGroupWidth, &opt, this), m_realSize.height());
}

void RibbonGroupWrapper::startLayout()
{
    m_ribbonGroup->qtn_d().startLayout();
}

void RibbonGroupWrapper::finalLayout()
{
    m_ribbonGroup->qtn_d().finalLayout();
}

bool RibbonGroupWrapper::extendSize(int width)
{
    return m_ribbonGroup->qtn_d().extendSize(width);
}

bool RibbonGroupWrapper::reduceSize(int level, int width)
{
    return m_ribbonGroup->qtn_d().reduceSize(level, width);
}

void RibbonGroupWrapper::updateLayout(int* width, bool reset)
{
    if (!m_ribbonGroup)
        return;

    if (reset)
        m_ribbonGroup->qtn_d().resetLayout();

    if (width)
    {
        if (m_ribbonGroup->qtn_d().extendSize(*width))
        {
            m_ribbonGroup->layout()->update();
            m_ribbonGroup->layout()->activate();
        }
    }
    else
    {
        m_ribbonGroup->layout()->update();
        m_ribbonGroup->layout()->activate();
    }

    if (reset)
        m_realSize = m_ribbonGroup->sizeHint();
}

QSize RibbonGroupWrapper::sizeHint() const
{
    RibbonGroupWrapper* that = const_cast<RibbonGroupWrapper*>(this);

    that->m_realSize = m_ribbonGroup ? m_ribbonGroup->sizeHint() : QSize();
    if (m_ribbonGroup && !m_ribbonGroup->qtn_d().m_reduced)
        return m_realSize;
    return reducedSize();
}

bool RibbonGroupWrapper::eventFilter(QObject* object, QEvent* event)
{
    bool result = QToolButton::eventFilter(object, event);

    switch (event->type()) 
    {
        case QEvent::ActionAdded: 
            {
                if (QWidget* parent = parentWidget())
                    QApplication::postEvent(parent, new QResizeEvent(parent->size(), QSize(-1, -1)));
            }
            break;
        case QEvent::Show :
            if (m_ribbonGroup->qtn_d().m_reduced && m_ribbonGroup == object)
            {
                m_ribbonGroup->adjustSize();
            }
            break;

        default:
            break;
    }

    return result;
}

bool RibbonGroupWrapper::event(QEvent* event)
{
    switch(event->type())
    {
        case QEvent::LayoutRequest :
               m_realSize = m_ribbonGroup->sizeHint();
            break;
        default:
            break;
    }
    return QToolButton::event(event);
}

void RibbonGroupWrapper::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    if (m_ribbonGroup && m_ribbonGroup->qtn_d().m_reduced) 
    {
        QPainter p(this );
        QStyleOptionToolButton opt;
        initStyleOption(&opt);
        if (m_hasPopup)
            opt.features |= QStyleOptionToolButton::HasMenu;
        QRect rcGroup = opt.rect;
        rcGroup.setBottom(rcGroup.bottom() - 1);
        opt.rect = rcGroup;
        style()->drawControl((QStyle::ControlElement)RibbonStyle::CE_ReducedGroup, &opt, &p, this);
    }
}

void RibbonGroupWrapper::mousePressEvent(QMouseEvent* event)
{
    if (!m_ribbonGroup)
        return;

    if (m_ribbonGroup->qtn_d().m_reduced && !m_hasPopup )
    {
        m_popupMenu->setGroup(m_ribbonGroup);
        m_popupMenu->setFont(parentWidget()->font());
        m_popupMenu->adjustSize();

        QStyleOptionToolButton opt;
        initStyleOption(&opt);
        if (event->button() == Qt::LeftButton && (popupMode() == QToolButton::MenuButtonPopup)) 
        {
            QRect popupr = style()->subControlRect(QStyle::CC_ToolButton, &opt, QStyle::SC_ToolButtonMenu, this);
            if (popupr.isValid() && popupr.contains(event->pos())) 
            {
                QPoint pnt(0, 0);
                QRect rect = m_popupMenu->rect();

                QRect screen = QApplication::desktop()->availableGeometry(this);
                int h = rect.height();
                if (this->mapToGlobal(QPoint(0, rect.bottom())).y() + h <= screen.height()) 
                    pnt = this->mapToGlobal(rect.bottomLeft());
                else 
                    pnt = this->mapToGlobal(rect.topLeft() - QPoint(0, h));

                QSize size = m_popupMenu->sizeHint();
                const int desktopFrame = style()->pixelMetric(QStyle::PM_MenuDesktopFrameWidth, 0, this);

                if (pnt.x() + size.width() - 1 > screen.right() - desktopFrame)
                    pnt.setX(screen.right() - desktopFrame - size.width() + 1);
                if (pnt.x() < screen.left() + desktopFrame)
                    pnt.setX(screen.left() + desktopFrame);

                m_popupMenu->setGeometry(QRect(pnt, size));

                m_popupMenu->show();
                m_popupMenu->repaint();
                m_hasPopup = true;
                repaint();
                QObject::connect(m_popupMenu, SIGNAL(aboutToHide()), this, SLOT(resetPopopGroup()));
                QEventLoop eventLoop;
                m_eventLoop = &eventLoop;
                (void) eventLoop.exec();
                QObject::disconnect(m_popupMenu, SIGNAL(aboutToHide()), this, SLOT(resetPopopGroup()));

                m_eventLoop = Q_NULL;
                return;
            }
        }
    }
}

void RibbonGroupWrapper::mouseReleaseEvent(QMouseEvent* event)
{
    m_hasPopup = false;
    QToolButton::mouseReleaseEvent(event);
}

void RibbonGroupWrapper::resizeEvent(QResizeEvent* event)
{
    QToolButton::resizeEvent(event);

    if (m_ribbonGroup && !m_ribbonGroup->qtn_d().m_reduced && m_ribbonGroup->isVisible() && event->size() != event->oldSize())
        m_ribbonGroup->setGeometry(rect());
}

/*!
\class Qtitan::RibbonGroupOption
\internal
*/
RibbonGroupOption::RibbonGroupOption(QWidget* parent)
    : QToolButton(parent)
{
}

RibbonGroupOption::~RibbonGroupOption()
{
}

QString RibbonGroupOption::text() const
{
    return QString("");
}

QSize RibbonGroupOption::sizeHint() const
{
    return QSize(15, 14);
}

void RibbonGroupOption::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter p(this);
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    opt.iconSize = opt.icon.actualSize(QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX));
    style()->drawPrimitive((QStyle::PrimitiveElement)RibbonStyle::PE_RibbonOptionButton, &opt, &p, this);
}

void RibbonGroupOption::actionEvent(QActionEvent* event)
{
    QToolButton::actionEvent(event);
    if (event->type() == QEvent::ActionChanged)
    {
        QAction* action = event->action();
        setPopupMode(action->menu() ? QToolButton::MenuButtonPopup : QToolButton::DelayedPopup);
    }
}


/*!
\class Qtitan::RibbonGroupScroll
\internal
*/
RibbonGroupScroll::RibbonGroupScroll(QWidget* parent, bool scrollLeft)
    : QToolButton(parent)
{
    m_scrollLeft = scrollLeft;
}

RibbonGroupScroll::~RibbonGroupScroll()
{
}

void RibbonGroupScroll::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter p(this);
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    opt.arrowType = m_scrollLeft ? Qt::LeftArrow : Qt::RightArrow;
    style()->drawPrimitive((QStyle::PrimitiveElement)RibbonStyle::PE_RibbonGroupScrollButton, &opt, &p, this);
}


/*!
\class Qtitan::KeyTipEvent
\internal
*/
KeyTipEvent::KeyTipEvent(RibbonKeyTip* kTip) 
    : QEvent(KeyTipEvent::eventNumber())
{
    keyTip = kTip;
}

RibbonKeyTip* KeyTipEvent::getKeyTip() const
{
    return keyTip;
}

QEvent::Type KeyTipEvent::eventNumber()
{
    if (keyTipEventNumber < 0)
        keyTipEventNumber = QEvent::registerEventType();
    return (QEvent::Type) keyTipEventNumber;
}

/*!
\class Qtitan::ShowKeyTipEvent
\internal
*/
ShowKeyTipEvent::ShowKeyTipEvent(QWidget* w) 
    : QEvent(ShowKeyTipEvent::eventNumber())
{
    widget = w;
}

QWidget* ShowKeyTipEvent::getWidget() const
{
    return widget;
}

QEvent::Type ShowKeyTipEvent::eventNumber()
{
    if (showKeyTipEventNumber < 0)
        showKeyTipEventNumber = QEvent::registerEventType();
    return (QEvent::Type) showKeyTipEventNumber;
}

/*!
\class Qtitan::HideKeyTipEvent
\internal
*/
HideKeyTipEvent::HideKeyTipEvent() 
    : QEvent(HideKeyTipEvent::eventNumber())
{
}

QEvent::Type HideKeyTipEvent::eventNumber()
{
    if (hideKeyTipEventNumber < 0)
        hideKeyTipEventNumber = QEvent::registerEventType();
    return (QEvent::Type) hideKeyTipEventNumber;
}

/*!
\class Qtitan::HideKeyTipEvent
\internal
*/
MinimizedEvent::MinimizedEvent() 
    : QEvent(MinimizedEvent::eventNumber())
{
}

QEvent::Type MinimizedEvent::eventNumber()
{
    if (minimizedEventNumber < 0)
        minimizedEventNumber = QEvent::registerEventType();
    return (QEvent::Type) minimizedEventNumber;
}
