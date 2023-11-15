/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 **/
#include "officepopupmenu.h"
#include "officestyle.h"
#include "ribbonbar_p.h"
#include "ribbongallery_p.h"
#include "ribbonstyle.h"
#include "stylehelpers.h"
#include <QAction>
#include <QApplication>
#include <QBitmap>
#include <QDesktopWidget>
#include <QPainter>
#include <QScrollBar>
#include <QStyleOption>
#include <QToolButton>
#include <QToolTip>
#include <QWidgetAction>
#include <qevent.h>
#include <qmath.h>
#include <qstyleoption.h>

QRIBBON_USE_NAMESPACE

/*!
\class QRibbon::RibbonGalleryItemPrivate
\internal
*/
RibbonGalleryItemPrivate::RibbonGalleryItemPrivate()
{
    m_index = -1;
    m_separator = false;
    m_enabled = true;
    m_visible = true;
}

void RibbonGalleryItemPrivate::init()
{
    Q_Q(RibbonGalleryItem);
    q->setSizeHint(QSize(0, 0));
}

/*!
\class QRibbon::RibbonGalleryGroupPrivate
\internal
*/
RibbonGalleryGroupPrivate::RibbonGalleryGroupPrivate()
{
    m_sizeItem = QSize(0, 0);
    m_clipItems = true;
}

RibbonGalleryGroupPrivate::~RibbonGalleryGroupPrivate()
{
    for (int i = 0, count = m_viewWidgets.size(); count > i; ++i) {
        if (RibbonGallery *gallery = qobject_cast<RibbonGallery *>(m_viewWidgets.at(i)))
            gallery->d_func()->m_items = Q_NULL;
    }
}

void RibbonGalleryGroupPrivate::init() {}

void RibbonGalleryGroupPrivate::updateIndexes(int start /*= 0*/)
{
    Q_Q(RibbonGalleryGroup);
    for (int i = start; i < m_arrItems.size(); ++i)
        q->item(i)->d_func()->m_index = i;
}

void RibbonGalleryGroupPrivate::itemsChanged()
{
    for (int i = 0; i < m_viewWidgets.size(); ++i) {
        if (RibbonGallery *gallery = qobject_cast<RibbonGallery *>(m_viewWidgets.at(i))) {
            gallery->d_func()->m_checkedItem = Q_NULL;
            gallery->d_func()->m_selected = -1;
            gallery->d_func()->m_scrollPos = 0;
            gallery->d_func()->layoutItems();

            if (gallery->d_func()->m_preview)
                gallery->d_func()->m_preview = false;
            gallery->update();
        }
    }
}

void RibbonGalleryGroupPrivate::redrawWidget()
{
    for (int i = 0, count = m_viewWidgets.size(); count > i; ++i)
        m_viewWidgets.at(i)->update();
}

void RibbonGalleryGroupPrivate::clear()
{
    RibbonGalleryItem *item = Q_NULL;
    foreach(item, m_arrItems) delete item;
    m_arrItems.clear();
}

/*!
\class QRibbon::RibbonGalleryPrivate
\internal
*/
RibbonGalleryPrivate::RibbonGalleryPrivate()
{
    m_showBorders = false;
    m_showLabels = true;
    m_hideSelection = false;
    m_keyboardSelected = false;
    m_preview = false;
    m_pressed = false;
    m_animation = false;
    m_autoWidth = false;
    m_scrollPos = 0;
    m_scrollPosTarget = 0;
    m_totalHeight = 0;
    m_selected = -1;
    m_checkedItem = Q_NULL;
    m_items = Q_NULL;
    m_ptPressed = QPoint(0, 0);
    m_scrollBar = Q_NULL;
    m_menuBar = Q_NULL;
    m_buttonPopup = Q_NULL;
    m_buttonScrollUp = Q_NULL;
    m_buttonScrollDown = Q_NULL;
    m_animationStep = 0.0;
    m_timerElapse = 0;
    m_minimumColumnCount = -1;
    m_maximumColumnCount = -1;
    m_currentColumnCount = -1;
}

RibbonGalleryPrivate::~RibbonGalleryPrivate()
{
    Q_Q(RibbonGallery);
    if (m_items && m_items->d_func()->m_viewWidgets.size() > 0)
        m_items->d_func()->m_viewWidgets.removeOne(q);
}

void RibbonGalleryPrivate::init()
{
    Q_Q(RibbonGallery);
    q->setAttribute(Qt::WA_MouseTracking);
    q->setObjectName("RibbonGallery");
    q->setProperty(_qrn_WidgetGallery, true);
    setScrollBarPolicy(Qt::ScrollBarAlwaysOn);
}

void RibbonGalleryPrivate::layoutItems()
{
    Q_Q(RibbonGallery);

    if (q->isShowAsButton())
        return;

    QRect rcItems = q->getItemsRect();

    if (q->autoWidth() && q->itemCount() > 0 && m_currentColumnCount != -1) {
        RibbonGalleryItem *item = q->item(0);
        int width = m_currentColumnCount * item->sizeHint().width() + q->borders().width();
        rcItems.setWidth(width);
    }

    int x = rcItems.left();
    int y = rcItems.top();
    bool firstItem = true;

    int count = q->itemCount();

    m_arrRects.resize(count);
    int rowHeight = 0;

    for (int i = 0; i < count; ++i) {
        RibbonGalleryItem *item = q->item(i);
        m_arrRects[i].item = item;
        m_arrRects[i].beginRow = false;

        if (!item->isVisible()) {
            m_arrRects[i].rect = QRect(QPoint(0, 0), QPoint(0, 0));
            continue;
        }

        if (item->isSeparator()) {
            if (!m_showLabels) {
                m_arrRects[i].rect = QRect(QPoint(0, 0), QPoint(0, 0));
                continue;
            }
            QPainter pp(q);
            QFontMetrics qfm(pp.font());
            const int marginH = (q->style()->pixelMetric(QStyle::PM_FocusFrameHMargin) + 1) * 2;
            const int heightLabel = qfm.height() + marginH;

            if (!firstItem)
                y += rowHeight;

            QRect rcItem(QPoint(rcItems.left(), y), QPoint(rcItems.right(), y + heightLabel));
            m_arrRects[i].rect = rcItem;
            m_arrRects[i].beginRow = true;

            y += heightLabel;
            rowHeight = 0;
            x = rcItems.left();
            firstItem = true;
        } else {
            QSize szItem = item->sizeHint();
            if (szItem.width() == 0)
                szItem.setWidth(rcItems.width());

            if (firstItem)
                m_arrRects[i].beginRow = true;

            if (!firstItem && x + szItem.width() > rcItems.right()) {
                y += rowHeight;
                x = rcItems.left();
                m_arrRects[i].beginRow = true;
                rowHeight = 0;
            }

            QRect rcItem(QPoint(x, y), szItem);
            m_arrRects[i].rect = rcItem;

            rowHeight = qMax(rowHeight, szItem.height());

            firstItem = false;
            x += szItem.width();
        }
    }

    if (!firstItem)
        y += rowHeight;

    m_totalHeight = y - rcItems.top();

    if (m_scrollPos > m_totalHeight - rcItems.height())
        m_scrollPos = qMax(0, m_totalHeight - rcItems.height());

    setScrollBarValue();
}

void RibbonGalleryPrivate::layoutScrollBar()
{
    Q_Q(RibbonGallery);

    if (m_scrollBar) {
        QSize size = m_scrollBar->sizeHint();

        QRect rectScroll(q->rect());
        rectScroll.setLeft(rectScroll.right() - size.width());

        if (qobject_cast<OfficePopupMenu *>(q->parentWidget()))
            rectScroll.adjust(0, 0, 0, 0);
        else if (m_showBorders)
            rectScroll.adjust(0, 1, -1, -1);

        m_scrollBar->setGeometry(rectScroll);
    } else if (m_buttonPopup && m_buttonScrollUp && m_buttonScrollDown) {
        QRect rectScroll(q->rect());

        if (m_showBorders)
            rectScroll.adjust(1, 0, 0, 0);

        rectScroll.setLeft(rectScroll.right() - 14);

        QRect rectUp(QPoint(rectScroll.left(), rectScroll.top()), QPoint(rectScroll.right(), rectScroll.top() + 20));
        m_buttonScrollUp->setGeometry(rectUp);
        QRect rectDown(QPoint(rectScroll.left(), rectUp.bottom() + 1),
                       QPoint(rectScroll.right(), rectUp.bottom() + 19));
        m_buttonScrollDown->setGeometry(rectDown);
        QRect rectScrollPopup(QPoint(rectScroll.left(), rectDown.bottom() + 1),
                              QPoint(rectScroll.right(), rectScroll.bottom()));
        m_buttonPopup->setGeometry(rectScrollPopup);
    }
}

void RibbonGalleryPrivate::setScrollBarValue()
{
    if (m_scrollBar) {
        Q_Q(RibbonGallery);
        QRect rcItems = q->getItemsRect();

        int nRangeMax = qMax(0, (m_totalHeight + 2) - m_scrollBar->rect().height());
        m_scrollBar->setRange(0, nRangeMax);
        m_scrollBar->setPageStep(rcItems.height());
        m_scrollBar->setSingleStep(20);
        m_scrollBar->setValue(m_scrollPos);
    } else if (m_buttonPopup && m_buttonScrollUp && m_buttonScrollDown) {
        m_buttonScrollUp->setEnabled(isScrollButtonEnabled(true));
        m_buttonScrollDown->setEnabled(isScrollButtonEnabled(false));
    }
}

void RibbonGalleryPrivate::setScrollPos(int scrollPos)
{
    Q_Q(RibbonGallery);
    QRect rcItems = q->getItemsRect();

    if (scrollPos > m_totalHeight - rcItems.height())
        scrollPos = m_totalHeight - rcItems.height();

    if (scrollPos < 0)
        scrollPos = 0;

    if (m_animation) {
        m_animation = false;
        m_timerElapse = 0;
        m_scrollTimer.stop();
        m_scrollPos = -1;
    }

    if (m_scrollPos == scrollPos)
        return;

    m_scrollPos = scrollPos;
    layoutItems();

    repaintItems(NULL, false);
}

void RibbonGalleryPrivate::actionTriggered(int action)
{
    Q_Q(RibbonGallery);
    int y = m_animation ? m_scrollPosTarget : m_scrollPos;
    QRect rcItems = q->getItemsRect();

    RibbonGalleryGroup *items = q->galleryGroup();
    if (!items)
        return;

    switch (action) {
    case QAbstractSlider::SliderSingleStepAdd:
        y += items->size().height();
        break;
    case QAbstractSlider::SliderSingleStepSub:
        y -= items->size().height();
        break;
    case QAbstractSlider::SliderPageStepAdd:
        y += rcItems.height();
        break;
    case QAbstractSlider::SliderPageStepSub:
        y -= rcItems.height();
        break;
    case QAbstractSlider::SliderToMinimum:
        y = 0;
        break;
    case QAbstractSlider::SliderToMaximum:
        y = m_totalHeight;
        break;
    case QAbstractSlider::SliderMove:
        y = m_scrollBar->sliderPosition();
        break;
    default:
        break;
    }

    if (m_menuBar && (action == QAbstractSlider::SliderSingleStepSub || action == QAbstractSlider::SliderSingleStepAdd))
        startAnimation(y);
    else
        setScrollPos(y);
}

void RibbonGalleryPrivate::pressedScrollUp()
{
    m_timerElapse = QApplication::doubleClickInterval() * 4 / 5;
    actionTriggered(QAbstractSlider::SliderSingleStepSub);
}

void RibbonGalleryPrivate::pressedScrollDown()
{
    m_timerElapse = QApplication::doubleClickInterval() * 4 / 5;
    actionTriggered(QAbstractSlider::SliderSingleStepAdd);
}

void RibbonGalleryPrivate::setScrollBarPolicy(Qt::ScrollBarPolicy policy)
{
    Q_Q(RibbonGallery);

    if (policy == Qt::ScrollBarAlwaysOn) {
        delete m_buttonScrollUp;
        m_buttonScrollUp = Q_NULL;
        delete m_buttonScrollDown;
        m_buttonScrollDown = Q_NULL;
        delete m_buttonPopup;
        m_buttonPopup = Q_NULL;
        if (m_menuBar)
            m_menuBar->removeEventFilter(this);
        m_menuBar = Q_NULL;

        if (!m_scrollBar) {
            m_scrollBar = new QScrollBar(q);
            setScrollBarValue();
            connect(m_scrollBar, SIGNAL(actionTriggered(int)), this, SLOT(actionTriggered(int)));
        }
    } else {
        if (m_scrollBar) {
            disconnect(m_scrollBar, SIGNAL(actionTriggered(int)), this, SLOT(actionTriggered(int)));
            delete m_scrollBar;
            m_scrollBar = Q_NULL;
        }
    }
}

Qt::ScrollBarPolicy RibbonGalleryPrivate::scrollBarPolicy() const
{
    return m_scrollBar ? Qt::ScrollBarAlwaysOn : Qt::ScrollBarAlwaysOff;
}

QAction *RibbonGalleryPrivate::setPopupMenu(OfficePopupMenu *popup)
{
    Q_Q(RibbonGallery);
    QAction *act = new QAction(q);
    setScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_menuBar = popup;
    m_menuBar->installEventFilter(this);

    m_buttonScrollUp = new QToolButton(q);
    m_buttonScrollUp->setProperty(_qrn_ScrollUpButtonGallery, true);
    m_buttonScrollDown = new QToolButton(q);
    m_buttonScrollDown->setProperty(_qrn_ScrollDownButtonGallery, true);
    m_buttonPopup = new QToolButton(q);
    m_buttonPopup->setProperty(_qrn_PopupButtonGallery, true);

    layoutScrollBar();
    m_buttonPopup->setPopupMode(QToolButton::InstantPopup);
    m_buttonPopup->setMenu(m_menuBar);
    m_menuBar->setWidgetBar(q);
    m_menuBar->setDefaultAction(act);

    connect(m_buttonScrollUp, SIGNAL(pressed()), this, SLOT(pressedScrollUp()));
    connect(m_buttonScrollDown, SIGNAL(pressed()), this, SLOT(pressedScrollDown()));
    return act;
}

void RibbonGalleryPrivate::drawItems(QPainter *painter)
{
    Q_Q(RibbonGallery);

    RibbonGalleryGroup *items = q->galleryGroup();
    if (!items)
        return;

    Q_ASSERT(m_arrRects.size() == items->itemCount());

    QRect rcItems = q->getItemsRect();

    painter->setClipRect(rcItems);

    int selected = m_hideSelection ? -1 : m_selected;
    for (int i = 0; i < m_arrRects.size(); ++i) {
        const qrn_galleryitem_rect &pos = m_arrRects[i];

        QRect rcItem = pos.rect;
        rcItem.translate(0, -m_scrollPos);

        if (rcItem.bottom() < rcItems.top())
            continue;

        bool enabled = pos.item->isEnabled() && q->isEnabled();

        if (!rcItem.isEmpty())
            pos.item->draw(painter, q, rcItem, enabled, selected == i, selected == i && m_pressed,
                           isItemChecked(pos.item));

        if (rcItem.top() > rcItems.bottom())
            break;
    }
}

void RibbonGalleryPrivate::repaintItems(QRect *pRect, bool bAnimate)
{
    Q_UNUSED(bAnimate);
    Q_Q(RibbonGallery);
#ifdef Q_OS_MAC
    Q_UNUSED(pRect);
    q->update();
#else
    q->update(pRect ? *pRect : q->rect());
#endif
}

bool RibbonGalleryPrivate::isItemChecked(RibbonGalleryItem *pItem) const { return m_checkedItem == pItem; }

bool RibbonGalleryPrivate::isScrollButtonEnabled(bool buttonUp)
{
    Q_Q(RibbonGallery);

    if (!q->isEnabled())
        return false;
    return buttonUp ? m_scrollPos > 0 : m_scrollPos < m_totalHeight - q->getItemsRect().height();
}

int RibbonGalleryPrivate::scrollWidth() const
{
    return m_scrollBar ? m_scrollBar->sizeHint().width() : m_buttonScrollUp ? m_buttonScrollUp->width() : 0;
}

void RibbonGalleryPrivate::startAnimation(int scrollPos)
{
    Q_Q(RibbonGallery);
    QRect rcItems = q->getItemsRect();

    if (scrollPos > m_totalHeight - rcItems.height())
        scrollPos = m_totalHeight - rcItems.height();

    if (scrollPos < 0)
        scrollPos = 0;

    if (m_animation && m_scrollPosTarget == scrollPos)
        return;

    if (!m_animation && m_scrollPos == scrollPos)
        return;

    m_animation = true;
    m_scrollPosTarget = scrollPos;
    m_animationStep = double(m_scrollPosTarget - m_scrollPos) / (m_timerElapse > 200 ? 8.0 : 3.0);

    if (m_animationStep > 0 && m_animationStep < 1)
        m_animationStep = 1;
    if (m_animationStep < 0 && m_animationStep > -1)
        m_animationStep = -1;

    int nms = 40;
    m_scrollTimer.start(nms, this);
    startAnimate();
}

void RibbonGalleryPrivate::startAnimate()
{
    if (qAbs(m_scrollPos - m_scrollPosTarget) > qFabs(m_animationStep)) {
        m_scrollPos = int((double)m_scrollPos + m_animationStep);
    } else {
        m_animation = false;
        m_scrollPos = m_scrollPosTarget;
        m_timerElapse = 0;
        m_scrollTimer.stop();
    }
    layoutItems();
    setScrollBarValue();
    repaintItems(Q_NULL, false);
}

bool RibbonGalleryPrivate::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::Timer: {
        QTimerEvent *timerEvent = (QTimerEvent *)event;
        if (m_scrollTimer.timerId() == timerEvent->timerId()) {
            startAnimate();
            event->accept();
            return true;
        }
    } break;
    default:
        break;
    }
    return QObject::event(event);
}

bool RibbonGalleryPrivate::eventFilter(QObject *object, QEvent *event)
{
    bool bResult = QObject::eventFilter(object, event);

    if (event->type() == QEvent::Show) {
        if (OfficePopupMenu *popup = qobject_cast<OfficePopupMenu *>(object)) {
            Q_Q(RibbonGallery);
            QPoint pos;
            QRect screen = QApplication::desktop()->availableGeometry(m_buttonPopup);
            QSize sh = popup->sizeHint();
            QRect rect = q->rect();
            if (q->mapToGlobal(QPoint(0, rect.bottom())).y() + sh.height() <= screen.height())
                pos = q->mapToGlobal(rect.topLeft());
            else
                pos = q->mapToGlobal(rect.topLeft() - QPoint(0, sh.height()));
            popup->move(pos.x(), pos.y());
        }
    } else if (event->type() == QEvent::Hide) {
        /*
        if (OfficePopupMenu* popup = qobject_cast<OfficePopupMenu*>(object)) {
            popup->setTearOffEnabled(true);
            popup->setGeometry(QRect(QPoint(0,0), QSize(0, 0)));
            popup->resize(QSize(0,0));
        }
        */
    }
    return bResult;
}

/* RibbonGalleryItem */
RibbonGalleryItem::RibbonGalleryItem()
{
    QRN_INIT_PRIVATE(RibbonGalleryItem);
    Q_D(RibbonGalleryItem);
    d->init();
}

RibbonGalleryItem::~RibbonGalleryItem() { QRN_FINI_PRIVATE(); }

/*!
\brief Returns the size hint for the item.
*/
QSize RibbonGalleryItem::sizeHint() const
{
    Q_D(const RibbonGalleryItem);

    QSize size = qvariant_cast<QSize>(data(Qt::SizeHintRole));

    if (!size.isNull() && size.isValid())
        return size;

    return d->m_items->d_func()->m_sizeItem;
}

/*!
\brief Sets the \a size hint for the item.
*/
void RibbonGalleryItem::setSizeHint(const QSize &size) { setData(Qt::SizeHintRole, size); }

QIcon RibbonGalleryItem::icon() const { return qvariant_cast<QIcon>(data(Qt::DecorationRole)); }

void RibbonGalleryItem::setIcon(const QIcon &icon) { setData(Qt::DecorationRole, icon); }

QString RibbonGalleryItem::caption() const { return data(Qt::DisplayRole).toString(); }

void RibbonGalleryItem::setCaption(const QString &caption) { setData(Qt::DisplayRole, caption); }

QString RibbonGalleryItem::toolTip() const { return data(Qt::ToolTipRole).toString(); }

void RibbonGalleryItem::setToolTip(const QString &toolTip) { setData(Qt::ToolTipRole, toolTip); }

QString RibbonGalleryItem::statusTip() const { return data(Qt::StatusTipRole).toString(); }

void RibbonGalleryItem::setStatusTip(const QString &statusTip) { setData(Qt::StatusTipRole, statusTip); }

int RibbonGalleryItem::getIndex() const
{
    Q_D(const RibbonGalleryItem);
    return d->m_index;
}

/*!
\brief Returns true if the item is the separator.
*/
bool RibbonGalleryItem::isSeparator() const
{
    Q_D(const RibbonGalleryItem);
    return d->m_separator;
}

/*!
\brief Sets the representation for the item as a separator if parameter \a b is true.
*/
void RibbonGalleryItem::setSeparator(bool b)
{
    Q_D(RibbonGalleryItem);
    d->m_separator = b;
}

void RibbonGalleryItem::setEnabled(bool enabled)
{
    Q_D(RibbonGalleryItem);
    if (d->m_enabled == enabled)
        return;

    d->m_enabled = enabled;

    if (d->m_items)
        d->m_items->d_func()->redrawWidget();
}

bool RibbonGalleryItem::isEnabled() const
{
    Q_D(const RibbonGalleryItem);
    return d->m_enabled;
}

void RibbonGalleryItem::setVisible(bool visible)
{
    Q_D(RibbonGalleryItem);
    if (d->m_visible == visible)
        return;

    d->m_visible = visible;

    if (d->m_items)
        d->m_items->d_func()->itemsChanged();
}

bool RibbonGalleryItem::isVisible() const
{
    Q_D(const RibbonGalleryItem);
    return d->m_visible;
}

void RibbonGalleryItem::setData(int role, const QVariant &value)
{
    Q_D(RibbonGalleryItem);
    bool found = false;
    role = (role == Qt::EditRole ? Qt::DisplayRole : role);
    for (int i = 0; i < d->m_values.count(); ++i) {
        if (d->m_values.at(i).role == role) {
            if (d->m_values.at(i).value == value)
                return;
            d->m_values[i].value = value;
            found = true;
            break;
        }
    }
    if (!found)
        d->m_values.append(WidgetItemData(role, value));

    // if (QListModel *model = (view ? qobject_cast<QListModel*>(view->model()) : 0))
    //     model->itemChanged(this);
}

QVariant RibbonGalleryItem::data(int role) const
{
    Q_D(const RibbonGalleryItem);
    role = (role == Qt::EditRole ? Qt::DisplayRole : role);
    for (int i = 0; i < d->m_values.count(); ++i)
        if (d->m_values.at(i).role == role)
            return d->m_values.at(i).value;
    return QVariant();
}

void RibbonGalleryItem::draw(QPainter *p, RibbonGallery *gallery, QRect rectItem, bool enabled, bool selected,
                             bool pressed, bool checked)
{
    Q_UNUSED(gallery);
    if (OfficeStyle *style = qobject_cast<OfficeStyle *>(gallery->style())) {
        if (isSeparator()) {
            style->drawLabelGallery(p, this, rectItem);
            return;
        }

        if (!icon().isNull()) {
            style->drawRectangle(p, rectItem, selected, pressed, enabled, checked, false, TypeNormal, BarPopup);
            icon().paint(p, rectItem, Qt::AlignCenter, enabled ? QIcon::Normal : QIcon::Disabled);
        } else {
            style->drawRectangle(p, rectItem, selected, false, enabled, checked, false, TypePopup, BarPopup);
            QRect rcText(rectItem);
            rcText.setLeft(rcText.left() + 3);
            int alignment = 0;
            alignment |= Qt::TextSingleLine | Qt::AlignVCenter | Qt::TextHideMnemonic;
            QPalette palette;
            palette.setColor(QPalette::WindowText,
                             style->getTextColor(selected, false, enabled, checked, false, TypePopup, BarPopup));
            style->drawItemText(p, rcText, alignment, palette, enabled, caption(), QPalette::WindowText);
        }
    }
}

/* RibbonGalleryGroup */
/*!
\brief Constructs RibbonGalleryGroup object with the given \a parent.
*/
RibbonGalleryGroup::RibbonGalleryGroup(QObject *parent) : QObject(parent)
{
    setObjectName("RibbonGalleryGroup");
    QRN_INIT_PRIVATE(RibbonGalleryGroup);
    Q_D(RibbonGalleryGroup);

    d->init();
}

/*!
\brief Destructor of the RibbonGalleryGroup object.
*/
RibbonGalleryGroup::~RibbonGalleryGroup()
{
    clear();
    QRN_FINI_PRIVATE();
}

/*!
\brief Creates a new instance of the RibbonGalleryGroup, and returns the reference to it.
*/
RibbonGalleryGroup *RibbonGalleryGroup::createGalleryGroup()
{
    RibbonGalleryGroup *group = new RibbonGalleryGroup();
    return group;
}

/*!
Creates and adds new item to the gallery with \a caption, \a pixmap. Additionaly it is possible to specify transparent
color \a transparentColor in the \a pixmap.
*/
RibbonGalleryItem *RibbonGalleryGroup::addItem(const QString &caption, const QPixmap &pixmap,
                                               const QColor &transparentColor)
{
    RibbonGalleryItem *item = new RibbonGalleryItem();

    if (!pixmap.isNull()) {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        QImage copyImage = pixmap.copy(0, 0, pixmap.width(), pixmap.height()).toImage();
        if (transparentColor.isValid())
            copyImage.setAlphaChannel(copyImage.createMaskFromColor(transparentColor.rgb(), Qt::MaskOutColor));
        item->setIcon(QPixmap::fromImage(copyImage));
#else
        QPixmap copyPix = pixmap.copy(0, 0, pixmap.width(), pixmap.height());
        if (transparentColor.isValid())
            copyPix.setAlphaChannel(QBitmap(copyPix.createMaskFromColor(transparentColor, Qt::MaskOutColor)));
        item->setIcon(QIcon(copyPix));
#endif
    }

    if (!caption.isEmpty()) {
        item->setCaption(caption);
        item->setToolTip(caption);
    }
    appendItem(item);
    return item;
}

/*!
Creates and adds new item to the gallery with \a caption and pixmap from pixmap \a map. Parameter \a mapIndex is a index
in the map. Parameter \a mapItemSize is a size of the one item in the map. Additionaly it is possible to specify
transparent color \a transparentColor in the pixmap.
*/
RibbonGalleryItem *RibbonGalleryGroup::addItemFromMap(const QString &caption, int mapIndex, const QPixmap &map,
                                                      const QSize &mapItemSize, const QColor &clrTransparent)
{
    QRect rcImage(mapIndex * mapItemSize.width(), 0, mapItemSize.width(), mapItemSize.height());
    QPixmap copyPix = map.copy(rcImage.left(), rcImage.top(), rcImage.width(), rcImage.height());

    Q_ASSERT(!copyPix.isNull());

    if (copyPix.isNull())
        return Q_NULL;

    return addItem(caption, copyPix, clrTransparent);
}

/*! Adds existing \a item to the gallery.
Note: RibbonGalleryGroup object is assigned as a owner for the item.
*/
void RibbonGalleryGroup::appendItem(RibbonGalleryItem *item) { insertItem(itemCount(), item); }

/*! Inserts existing \a item to the gallery in pos \a index.
Note: RibbonGalleryGroup object is assigned as a owner for the item.
*/
void RibbonGalleryGroup::insertItem(int index, RibbonGalleryItem *item)
{
    Q_D(RibbonGalleryGroup);

    if (!item) {
        Q_ASSERT(false);
        return;
    }

    if (index < 0 || index > itemCount())
        index = itemCount();

    d->m_arrItems.insert(index, item);
    d->updateIndexes(index);

    item->d_func()->m_items = this;
    d->itemsChanged();
}

/*!
Adds separator to the group with given \a caption and returns reference to them.
*/
RibbonGalleryItem *RibbonGalleryGroup::addSeparator(const QString &caption)
{
    RibbonGalleryItem *item = new RibbonGalleryItem();
    appendItem(item);
    item->setCaption(caption);
    item->setSeparator(true);
    return item;
}

void RibbonGalleryGroup::clear()
{
    Q_D(RibbonGalleryGroup);
    d->clear();
    d->itemsChanged();
}

void RibbonGalleryGroup::remove(int index)
{
    Q_D(RibbonGalleryGroup);

    RibbonGalleryItem *galleryItem = item(index);
    if (!galleryItem)
        return;

    d->m_arrItems.remove(index);
    delete galleryItem;

    d->itemsChanged();
}

/*!
\brief Returns the count of the items.
*/
int RibbonGalleryGroup::itemCount() const
{
    Q_D(const RibbonGalleryGroup);
    return d->m_arrItems.size();
}

/*!
\brief Returns reference to the RibbonGalleryItem by the given \a index.
*/
RibbonGalleryItem *RibbonGalleryGroup::item(int index) const
{
    Q_D(const RibbonGalleryGroup);
    return index >= 0 && index < itemCount() ? d->m_arrItems.at(index) : Q_NULL;
}

/*!
\brief Removes item at index from item list without deleting it
*/
RibbonGalleryItem *RibbonGalleryGroup::takeItem(int index)
{
    Q_D(RibbonGalleryGroup);

    RibbonGalleryItem *galleryItem = item(index);
    if (!galleryItem)
        return NULL;

    d->m_arrItems.remove(index);
    d->updateIndexes(0);

    d->itemsChanged();
    return galleryItem;
}

/*!
\brief Returns size of the group.
*/
QSize RibbonGalleryGroup::size() const
{
    Q_D(const RibbonGalleryGroup);
    return d->m_sizeItem;
}

/*!
\brief Sets \a size of the group.
*/
void RibbonGalleryGroup::setSize(const QSize &size)
{
    Q_D(RibbonGalleryGroup);
    d->m_sizeItem = size;
}

/*!
\brief Sets clipping items if selection was changed-> Paramter \a clipItems - true to enable clipping or false to
disable. By default clipping is enabled.
*/
void RibbonGalleryGroup::setClipItems(bool clipItems)
{
    Q_D(RibbonGalleryGroup);
    d->m_clipItems = clipItems;
}

/* RibbonGallery */
RibbonGallery::RibbonGallery(QWidget *parent) : QWidget(parent)
{
    QRN_INIT_PRIVATE(RibbonGallery);
    Q_D(RibbonGallery);
    d->init();
}

RibbonGallery::~RibbonGallery() { QRN_FINI_PRIVATE(); }

/*!
\brief Sets group for the gallery. group - is a pointer to the elements group.
*/
void RibbonGallery::setGalleryGroup(RibbonGalleryGroup *items)
{
    Q_D(RibbonGallery);
    if (d->m_items) {
        d->m_arrRects.clear();
        d->m_items->d_func()->m_viewWidgets.removeOne(this);
        d->m_items = Q_NULL;
    }

    if (items) {
        d->m_items = items;
        d->m_items->d_func()->m_viewWidgets.append(this);
    }
    d->layoutItems();
    update();
}

void RibbonGallery::setBorderVisible(bool bShowBorders)
{
    Q_D(RibbonGallery);
    d->m_showBorders = bShowBorders;
    update();
}

bool RibbonGallery::isBorderVisible() const
{
    Q_D(const RibbonGallery);
    return d->m_showBorders;
}

void RibbonGallery::setLabelsVisible(bool showLabels)
{
    Q_D(RibbonGallery);
    d->m_showLabels = showLabels;
}

bool RibbonGallery::isLabelsVisible() const
{
    Q_D(const RibbonGallery);
    return d->m_showLabels;
}

void RibbonGallery::setScrollBarPolicy(Qt::ScrollBarPolicy policy)
{
    Q_D(RibbonGallery);
    d->setScrollBarPolicy(policy);
}

Qt::ScrollBarPolicy RibbonGallery::scrollBarPolicy() const
{
    Q_D(const RibbonGallery);
    return d->scrollBarPolicy();
}

void RibbonGallery::ensureVisible(int index)
{
    Q_D(RibbonGallery);

    if (itemCount() == 0 || index == -1 || index >= itemCount() || index >= d->m_arrRects.size())
        return;

    const qrn_galleryitem_rect &pos = d->m_arrRects.at(index);

    QRect rcItem = pos.rect;
    rcItem.translate(0, -d->m_scrollPos);

    QRect rcItems = getItemsRect();

    if (rcItem.top() < rcItems.top()) {
        int scrollPos = pos.rect.top() - rcItems.top();
        if (d->m_showLabels) {
            for (int i = index - 1; i >= 0; i--) {
                const qrn_galleryitem_rect &posLabel = d->m_arrRects.at(i);
                if (posLabel.item->isSeparator() &&
                    pos.rect.top() - posLabel.rect.top() <= rcItems.height() - rcItem.height()) {
                    scrollPos = posLabel.rect.top() - rcItems.top();
                    break;
                }
                if (pos.rect.top() - posLabel.rect.top() > rcItems.height() - rcItem.height())
                    break;
            }
        }
        d->setScrollPos(scrollPos);
    } else if (rcItem.bottom() > rcItems.bottom()) {
        d->setScrollPos(pos.rect.bottom() - rcItems.bottom());
    }
}

/*!
\brief Sets \a popup menu for the gallery.
*/
QAction *RibbonGallery::setPopupMenu(OfficePopupMenu *popupMenu)
{
    Q_D(RibbonGallery);
    return d->setPopupMenu(popupMenu);
}

/*!
\brief Returns gallery's popup menu.
*/
OfficePopupMenu *RibbonGallery::popupMenu() const
{
    Q_D(const RibbonGallery);
    return d->m_menuBar;
}

int RibbonGallery::itemCount() const
{
    RibbonGalleryGroup *pItems = galleryGroup();
    return pItems ? pItems->itemCount() : 0;
}

RibbonGalleryItem *RibbonGallery::item(int index) const
{
    RibbonGalleryGroup *items = galleryGroup();
    return items ? items->item(index) : NULL;
}

/*!
\brief Returns reference to the RibbonGalleryGroup object.
*/
RibbonGalleryGroup *RibbonGallery::galleryGroup() const
{
    Q_D(const RibbonGallery);
    return d->m_items;
}

void RibbonGallery::setSelectedItem(int indexItem)
{
    Q_D(RibbonGallery);

    int indSelected = d->m_selected;

    d->m_hideSelection = false;
    d->m_selected = indexItem;
    d->m_pressed = false;
    d->m_keyboardSelected = true;

    RibbonGalleryGroup *items = galleryGroup();

    if (items && items->d_func()->m_clipItems && indSelected != -1) {
        QRect rect(getDrawItemRect(indSelected));
        d->repaintItems(&rect, true);
    }

    ensureVisible(d->m_selected);

    if (items && items->d_func()->m_clipItems && d->m_selected != -1) {
        QRect rect(getDrawItemRect(d->m_selected));
        d->repaintItems(&rect, false);
    }

    if (!items || !items->d_func()->m_clipItems)
        d->repaintItems();

    if (d->m_selected != -1) {
    }
    if (d->m_selected != -1 && !d->m_preview)
        d->m_preview = true;

    if (d->m_preview) {
    }

    if (d->m_selected == -1 && d->m_preview)
        d->m_preview = false;
    selectedItemChanged();
}

int RibbonGallery::selectedItem() const
{
    Q_D(const RibbonGallery);
    return d->m_selected;
}

/*!
\brief Sets checked item with the given \a index.
*/
void RibbonGallery::setCheckedIndex(int index)
{
    Q_D(RibbonGallery);

    RibbonGalleryItem *previous = NULL;
    int previousInd = checkedIndex();
    if (previousInd != -1)
        previous = item(previousInd);

    RibbonGalleryItem *currentItem = item(index);
    if (currentItem && d->m_checkedItem != currentItem) {
        d->m_checkedItem = currentItem;
        d->repaintItems();
        emit currentItemChanged(currentItem, previous);
    }
    if (currentItem)
        ensureVisible(index);
}

/*!
\brief Returns chcked item index in the gallery.
*/
int RibbonGallery::checkedIndex() const
{
    Q_D(const RibbonGallery);
    if (d->m_checkedItem)
        return d->m_checkedItem->getIndex();
    return -1;
}

/*!
\brief Sets checked \a item with the given reference to the RibbonGalleryItem object.
See setCheckedIndex(int index).
*/
void RibbonGallery::setCheckedItem(const RibbonGalleryItem *item)
{
    Q_ASSERT(item != Q_NULL);
    if (item->getIndex() != -1)
        setCheckedIndex(item->getIndex());
}

/*!
\brief Returs the reference to the RibbonGalleryItem object of the chcked item in the gallery.
See int getCheckedIndex();
*/
RibbonGalleryItem *RibbonGallery::checkedItem() const
{
    Q_D(const RibbonGallery);
    return d->m_checkedItem;
}

bool RibbonGallery::isShowAsButton() const { return false; }

bool RibbonGallery::isItemSelected() const
{
    Q_D(const RibbonGallery);
    return !d->m_hideSelection && selectedItem() != -1;
}

QRect RibbonGallery::borders() const
{
    Q_D(const RibbonGallery);
    QRect rcBorders(0, 0, 0, 0);

    if (d->m_showBorders) {
        rcBorders.setTopLeft(QPoint(1, 1));
        rcBorders.setBottomRight(QPoint(1, 1));
    }

    rcBorders.setRight(rcBorders.right() + d->scrollWidth());

#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
    int left = 0, top = 0, right = 0, bottom = 0;
    getContentsMargins(&left, &top, &right, &bottom);
    rcBorders.adjust(left, top, right, bottom);
#else
    QMargins m = contentsMargins();
    rcBorders.adjust(m.left(), m.top(), m.right(), m.bottom());
#endif

    return rcBorders;
}

/*!
\brief Returns the bounded rectangle of the item by the given \a index.
*/
QRect RibbonGallery::getDrawItemRect(int index)
{
    Q_D(RibbonGallery);
    if (index < 0 || index >= d->m_arrRects.size())
        return QRect(0, 0, 0, 0);

    QRect rc(d->m_arrRects[index].rect);
    rc.translate(0, -d->m_scrollPos);

    rc = rc.intersected(getItemsRect());
    return rc;
}

bool RibbonGallery::isResizable() const
{
    bool isResizable = sizePolicy().horizontalPolicy() == QSizePolicy::Expanding ||
                       sizePolicy().horizontalPolicy() == QSizePolicy::MinimumExpanding;
    return !isShowAsButton() && galleryGroup() ? isResizable : false;
}

bool RibbonGallery::autoWidth() const
{
    Q_D(const RibbonGallery);
    return d->m_autoWidth;
}

void RibbonGallery::setAutoWidth(bool width)
{
    Q_D(RibbonGallery);
    d->m_autoWidth = width;
}

int RibbonGallery::hitTestItem(QPoint point, QRect *pRect) const
{
    if (!isEnabled())
        return -1;

    if (isShowAsButton())
        return -1;

    Q_D(const RibbonGallery);
    Q_ASSERT(d->m_arrRects.size() == itemCount());

    QRect rcItems = getItemsRect();

    for (int i = 0; i < d->m_arrRects.size(); ++i) {
        const qrn_galleryitem_rect &pos = d->m_arrRects[i];

        QRect rcItem = pos.rect;
        rcItem.translate(0, -d->m_scrollPos);

        if (!pos.item->isEnabled())
            continue;

        if (rcItem.top() >= rcItems.bottom())
            break;

        if (rcItem.contains(point)) {
            if (pRect)
                *pRect = rcItem;
            return pos.item->isSeparator() ? -1 : i;
        }
    }

    return -1;
}

QRect RibbonGallery::getItemsRect() const
{
    QRect rc = rect();

    if (rc.isNull()) {
        rc.setWidth(32000);
        rc.setHeight(32000);
    }

    QRect rcBorders = borders();
    rc.adjust(rcBorders.left(), rcBorders.top(), -rcBorders.right(), -rcBorders.bottom());
    return rc;
}

void RibbonGallery::hideSelection()
{
    Q_D(RibbonGallery);
    d->m_hideSelection = true;
    d->m_keyboardSelected = false;

    if (d->m_selected != -1) {
        RibbonGalleryGroup *pItems = galleryGroup();
        if (pItems && pItems->d_func()->m_clipItems) {
            QRect rect(getDrawItemRect(d->m_selected));
            d->repaintItems(&rect, false);
        } else
            d->repaintItems();
    }

    if (d->m_preview && d->m_selected != -1) {
    }

    if (d->m_preview)
        d->m_preview = false;
    selectedItemChanged();
}

void RibbonGallery::updatelayout()
{
    Q_D(RibbonGallery);
    d->layoutItems();
    d->layoutScrollBar();
}

void RibbonGallery::bestFit()
{
    if (itemCount() > 0) {
        RibbonGalleryItem *galleryItem = item(0);
        QSize szItem = galleryItem->sizeHint();
        int totalWidth = width() - borders().width();
        if (szItem.width() < totalWidth) {
            int columnCount = totalWidth / szItem.width();
            setColumnCount(columnCount);
        }
    }
}

void RibbonGallery::selectedItemChanged() {}

/*!
Assigns a minimum \a count of column in the ribbon gallery.
*/
void RibbonGallery::setMinimumColumnCount(int count)
{
    Q_D(RibbonGallery);
    if (d->m_minimumColumnCount != count) {
        d->m_minimumColumnCount = count;

        if (d->m_minimumColumnCount > d->m_currentColumnCount)
            d->m_currentColumnCount = d->m_minimumColumnCount;

        updatelayout();
    }
}

/*!
Returns a minimum count of column in the ribbon gallery.
*/
int RibbonGallery::minimumColumnCount() const
{
    Q_D(const RibbonGallery);
    return d->m_minimumColumnCount;
}

/*!
Assigns a maximum \a count of column in the ribbon gallery.
*/
void RibbonGallery::setMaximumColumnCount(int count)
{
    Q_D(RibbonGallery);
    if (d->m_maximumColumnCount != count) {
        d->m_maximumColumnCount = count;

        if (d->m_maximumColumnCount < d->m_currentColumnCount || d->m_currentColumnCount == -1)
            d->m_currentColumnCount = d->m_maximumColumnCount;

        updatelayout();
    }
}

/*!
Returns a maximum count of column in the ribbon gallery.
*/
int RibbonGallery::maximumColumnCount() const
{
    Q_D(const RibbonGallery);
    return d->m_maximumColumnCount;
}

/*!
Assigns a current \a count of column in the ribbon gallery.
*/
void RibbonGallery::setColumnCount(int count)
{
    Q_D(RibbonGallery);

    if (d->m_maximumColumnCount != -1 && d->m_maximumColumnCount < count)
        count = d->m_maximumColumnCount;
    else if (d->m_minimumColumnCount != -1 && d->m_minimumColumnCount > count)
        count = d->m_minimumColumnCount;

    d->m_currentColumnCount = count;

    if (autoWidth())
        updatelayout();
}

/*!
Returns a current count of column in the ribbon gallery.
*/
int RibbonGallery::columnCount() const
{
    Q_D(const RibbonGallery);
    return d->m_currentColumnCount;
}

/*! \reimp */
QSize RibbonGallery::sizeHint() const
{
    Q_D(const RibbonGallery);

    QSize size = minimumSizeHint();

    if (columnCount() != -1) {
        if (RibbonGalleryGroup *group = galleryGroup()) {
            size.setWidth(columnCount() * group->size().width());
            size.setHeight(d->m_totalHeight);
            return size;
        }
    }

    int widthGallery = qMax(isResizable() ? size.width() : 0, geometry().width());
    int heightGallery = geometry().height();

    if (OfficePopupMenu *popupBar = qobject_cast<OfficePopupMenu *>(parentWidget()))
        widthGallery = popupBar->width() - 2;

    return QSize(widthGallery, heightGallery);
}

/*! \reimp */
QSize RibbonGallery::minimumSizeHint() const
{
    Q_D(const RibbonGallery);

    if (!parentWidget() || !galleryGroup())
        return QSize(0, 0);

    QRect rcBorders(borders());
    int width = qMax(16, galleryGroup()->d_func()->m_sizeItem.width()) + rcBorders.left() + rcBorders.right();

    if (RibbonGalleryGroup *group = galleryGroup())
        width = d->m_minimumColumnCount != -1 ? d->m_minimumColumnCount * group->size().width() : width;

    int height = qMax(galleryGroup()->d_func()->m_sizeItem.height() + rcBorders.top() + rcBorders.bottom(), 32);

    return QSize(width, height);
}

/*! \reimp */
void RibbonGallery::paintEvent(QPaintEvent *)
{
    Q_D(RibbonGallery);
    QPainter p(this);

    QStyleOption opt;
    opt.init(this);

    style()->drawPrimitive((QStyle::PrimitiveElement)RibbonStyle::PE_RibbonFillRect, &opt, &p, this);

    if (!parentWidget()->property(_qrn_PopupBar).toBool() && isBorderVisible()) {
        opt.rect.adjust(0, 0, -1, -1);
        style()->drawPrimitive((QStyle::PrimitiveElement)RibbonStyle::PE_RibbonRect, &opt, &p, this);
    }
    d->drawItems(&p);
}

/*! \reimp */
void RibbonGallery::mousePressEvent(QMouseEvent *event)
{
    Q_D(RibbonGallery);

    if (isShowAsButton()) {
        QWidget::mousePressEvent(event);
        return;
    }

    if (d->m_selected != -1 && !d->m_keyboardSelected) {
        d->m_pressed = true;
        d->m_ptPressed = event->pos();
        d->repaintItems();

        int index = hitTestItem(event->pos());
        if (index != -1)
            emit itemPressed(item(index));
    }
}

/*! \reimp */
void RibbonGallery::mouseReleaseEvent(QMouseEvent *event)
{
    Q_D(RibbonGallery);
    if (d->m_pressed || isItemSelected()) {
        d->m_pressed = false;
        d->repaintItems();

        int index = hitTestItem(event->pos());
        if (index != -1) {
            d->repaintItems();
            emit itemSelectionChanged();
            RibbonGalleryItem *galleryItem = item(index);

            emit itemClicked(galleryItem);

            bool handled = true;
            emit itemClicking(galleryItem, handled);

            if (handled) {
                if (OfficePopupMenu *popupBar = qobject_cast<OfficePopupMenu *>(parentWidget())) {
                    popupBar->close();
                    for (QWidget *widget = QApplication::activePopupWidget(); widget;) {
                        if (QMenu *qmenu = qobject_cast<QMenu *>(widget)) {
                            qmenu->hide();
                            break;
                        }
                    }
                }
            }
        }
    }
}

/*! \reimp */
void RibbonGallery::mouseMoveEvent(QMouseEvent *event)
{
    Q_D(RibbonGallery);

    if (isShowAsButton()) {
        QWidget::mouseMoveEvent(event);
        return;
    }

    int nItem = hitTestItem(event->pos());

    if (nItem == -1 && d->m_keyboardSelected)
        return;

    if (nItem == -1 && event->pos() == QPoint(-1, -1) && d->m_selected != -1) {
        hideSelection();
        return;
    }

    int nSelected = d->m_selected;
    if (nItem != d->m_selected || d->m_hideSelection || d->m_keyboardSelected) {
        d->m_selected = nItem;
        d->m_hideSelection = false;
        d->m_keyboardSelected = false;
        d->m_pressed = false;

        RibbonGalleryGroup *pItems = galleryGroup();

        if (pItems && pItems->d_func()->m_clipItems) {
            if (nSelected != -1) {
                QRect rect(getDrawItemRect(nSelected));
                d->repaintItems(&rect, true);
            }

            if (d->m_selected != -1) {
                QRect rect(getDrawItemRect(d->m_selected));
                d->repaintItems(&rect, false);
            }
        } else
            d->repaintItems();

        if (!d->m_preview && d->m_selected != -1) {
        }

        if (d->m_preview) {
        }

        if (d->m_selected == -1 && d->m_preview) {
            d->m_preview = false;
        }
        selectedItemChanged();
    }
    if (d->m_pressed && (d->m_ptPressed != QPoint(0, 0)) && (d->m_selected != -1) &&
        (abs(d->m_ptPressed.x() - event->pos().x()) > 4 || abs(d->m_ptPressed.y() - event->pos().y()) > 4)) {
        d->m_ptPressed = QPoint(0, 0);
    }
}

/*! \reimp */
void RibbonGallery::leaveEvent(QEvent *event)
{
    QWidget::leaveEvent(event);

    Q_D(RibbonGallery);
    if (d->m_selected != -1)
        hideSelection();
}

/*! \reimp */
void RibbonGallery::focusOutEvent(QFocusEvent *event) { QWidget::focusOutEvent(event); }

/*! \reimp */
void RibbonGallery::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    if (autoWidth())
        bestFit();
}
