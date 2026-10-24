/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "materialdrawer.h"

#include "materialtokens.h"

#include "qxcontrols/qxstatelayer.h"

#include "qxtheme/qxthememotion.h"

#include <QtCore/QEvent>
#include <QtCore/QVariantAnimation>
#include <QtGui/QFontMetrics>
#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>
#include <QtGui/QResizeEvent>
#include <QtGui/QWheelEvent>
#include <QtWidgets/QSizePolicy>
// QAction is in Widgets on Qt 5 and in Gui on Qt 6, so it is asked for without
// naming a module - the same way the rest of the libraries ask.
#include <QAction>

QX_MATERIAL_BEGIN_NAMESPACE

namespace
{
// Material's drawer measures: a panel two hundred and fifty-six wide, a
// destination forty-eight tall with a pill forty of it, an icon of twenty-four
// and the name a dozen to its right.
constexpr int kPanelWidth = 256;
constexpr int kTopPadding = 8;
constexpr int kItemHeight = 48;
constexpr int kSectionHeight = 32;
constexpr int kDividerHeight = 17;
constexpr int kSidePadding = 12;
constexpr int kPillHeight = 40;
constexpr int kIconInset = 16;
constexpr int kIconSize = 24;
constexpr int kTextGap = 16;

// The pill behind the destination in force is the accent's own shape at the
// strength Material gives an active indicator; one nobody can use is the same
// drawing thinned. The ones that are not in force are the page's ink, nearly
// whole: a drawer is a list of names, and names are read.
constexpr qreal kIndicatorOpacity = 0.12;
constexpr qreal kDisabledOpacity = 0.38;
constexpr qreal kRestingOpacity = 0.87;
constexpr qreal kSectionOpacity = 0.6;

QPointF eventPos(const QMouseEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return event->position();
#else
    return event->localPos();
#endif
}

/*!
 * What a temporary drawer dims the page with: a plain sheet over the room the
 * drawer was put in, dark enough that the page reads as behind something and
 * not as turned off.
 *
 * It is a widget of its own rather than something the drawer paints, because it
 * has to be under the drawer and over everything else, and one widget cannot be
 * in two places in a stack.
 */
class DrawerScrim : public QWidget
{
public:
    explicit DrawerScrim(QWidget *parent)
        : QWidget(parent)
    {
        setAttribute(Qt::WA_NoSystemBackground, true);
        setAttribute(Qt::WA_TransparentForMouseEvents, false);
        setCursor(Qt::ArrowCursor);
    }
protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter painter(this);
        painter.fillRect(rect(), QColor(0, 0, 0, 82));
    }
};
}   // namespace

class MaterialDrawerPrivate
{
public:
    QX_DECLARE_PUBLIC(MaterialDrawer)

    enum class Kind {
        Destination,
        Section,
        Divider
    };
    struct Entry {
        Kind kind = Kind::Destination;
        QAction *action = Q_NULLPTR;
        QString title;
        int destination = -1;
    };

    QList<Entry> entries;
    QList<QAction *> items;
    MaterialDrawer::Variant variant = MaterialDrawer::Variant::Temporary;
    bool open = false;
    int hovered = -1;
    int current = -1;
    int focused = -1;
    QWidget *header = Q_NULLPTR;
    DrawerScrim *scrim = Q_NULLPTR;
    QWidget *host = Q_NULLPTR;
    qreal offset = 0.0;
    QVariantAnimation animation;

    qreal headerHeight() const;
    qreal contentTop() const;
    qreal contentHeight() const;
    qreal entryTop(int index) const;
    QRectF entryRect(int index) const;
    QRectF pillOf(const QRectF &entry) const;
    int entryAt(const QPointF &pos) const;
    int destinationEntry(int destination) const;
    void layoutHeader();
    void relayoutHost();
    void installScrim();
    void dropScrim();
    void clampOffset();
    void ensureVisible(int entry);
    void slideTo(bool open);
    void placeTemporarily(bool open);
    void resizePersistently(bool open);
    QSize headerSize() const;
};

QSize MaterialDrawerPrivate::headerSize() const
{
    if (!header) {
        return QSize();
    }
    const QSize hint = header->sizeHint();
    return hint.isValid() ? hint : header->size();
}

qreal MaterialDrawerPrivate::headerHeight() const
{
    if (!header) {
        return 0.0;
    }
    return qreal(headerSize().height()) + qreal(kTopPadding);
}

qreal MaterialDrawerPrivate::contentTop() const
{
    return headerHeight() + kTopPadding;
}

qreal MaterialDrawerPrivate::contentHeight() const
{
    qreal height = 0.0;
    for (const Entry &entry : entries) {
        height += entry.kind == Kind::Destination ? kItemHeight
                                                  : (entry.kind == Kind::Section ? kSectionHeight : kDividerHeight);
    }
    return height;
}

qreal MaterialDrawerPrivate::entryTop(int index) const
{
    qreal top = contentTop();
    for (int i = 0; i < index && i < entries.count(); ++i) {
        const Kind kind = entries.at(i).kind;
        top += kind == Kind::Destination ? kItemHeight : (kind == Kind::Section ? kSectionHeight : kDividerHeight);
    }
    return top - offset;
}

QRectF MaterialDrawerPrivate::entryRect(int index) const
{
    Q_Q(const MaterialDrawer);
    if (index < 0 || index >= entries.count()) {
        return QRectF();
    }
    const Kind kind = entries.at(index).kind;
    const qreal height =
        kind == Kind::Destination ? kItemHeight : (kind == Kind::Section ? kSectionHeight : kDividerHeight);
    return QRectF(0.0, entryTop(index), qreal(q->width()), height);
}

QRectF MaterialDrawerPrivate::pillOf(const QRectF &entry) const
{
    return QRectF(entry.left() + kSidePadding, entry.top() + (kItemHeight - kPillHeight) / 2.0,
                  entry.width() - 2 * kSidePadding, kPillHeight);
}

int MaterialDrawerPrivate::entryAt(const QPointF &pos) const
{
    if (pos.y() < contentTop()) {
        return -1;
    }
    for (int i = 0; i < entries.count(); ++i) {
        if (entryRect(i).contains(pos)) {
            return entries.at(i).kind == Kind::Destination ? i : -1;
        }
    }
    return -1;
}

int MaterialDrawerPrivate::destinationEntry(int destination) const
{
    for (int i = 0; i < entries.count(); ++i) {
        if (entries.at(i).kind == Kind::Destination && entries.at(i).destination == destination) {
            return i;
        }
    }
    return -1;
}

void MaterialDrawerPrivate::layoutHeader()
{
    Q_Q(MaterialDrawer);
    if (!header) {
        return;
    }
    const QSize size = headerSize();
    header->setGeometry(kSidePadding + kIconInset, kTopPadding, qMax(0, q->width() - 2 * (kSidePadding + kIconInset)),
                        size.height());
}

void MaterialDrawerPrivate::relayoutHost()
{
    Q_Q(MaterialDrawer);
    if (scrim && host) {
        scrim->setGeometry(host->rect());
    }
    if (variant == MaterialDrawer::Variant::Temporary && host) {
        q->setGeometry(q->x(), 0, kPanelWidth, host->height());
    }
}

void MaterialDrawerPrivate::installScrim()
{
    Q_Q(MaterialDrawer);
    host = q->parentWidget();
    if (!host) {
        return;
    }
    if (!scrim) {
        scrim = new DrawerScrim(host);
        scrim->installEventFilter(q);
    }
    scrim->setGeometry(host->rect());
    scrim->show();
    scrim->raise();
    host->installEventFilter(q);
    q->raise();
}

void MaterialDrawerPrivate::dropScrim()
{
    if (scrim) {
        scrim->hide();
    }
}

void MaterialDrawerPrivate::clampOffset()
{
    Q_Q(MaterialDrawer);
    const qreal room = qreal(q->height()) - contentTop() - kTopPadding;
    offset = qBound(0.0, offset, qMax(0.0, contentHeight() - room));
}

void MaterialDrawerPrivate::ensureVisible(int entry)
{
    Q_Q(MaterialDrawer);
    if (entry < 0) {
        return;
    }
    const QRectF rect = entryRect(entry);
    if (rect.top() < contentTop()) {
        offset -= contentTop() - rect.top();
    } else if (rect.bottom() > qreal(q->height()) - kTopPadding) {
        offset += rect.bottom() - (qreal(q->height()) - kTopPadding);
    }
    clampOffset();
}

void MaterialDrawerPrivate::slideTo(bool open)
{
    Q_Q(MaterialDrawer);
    animation.stop();
    const QxTheme::QxMotion motion = MaterialMotion::forDuration(MaterialMotion::Duration::Medium);
    const qreal to = open ? 0.0 : qreal(-kPanelWidth);
    if (motion.duration <= 0) {
        q->move(qRound(to), 0);
        if (!open) {
            q->hide();
            dropScrim();
        }
        return;
    }
    animation.setDuration(motion.duration);
    animation.setEasingCurve(motion.curve);
    // Both ends are reals: an animation whose ends are of two different types
    // has nothing to interpolate between, and would sit at its first one.
    animation.setStartValue(qreal(q->x()));
    animation.setEndValue(to);
    animation.start();
}

void MaterialDrawerPrivate::placeTemporarily(bool open)
{
    Q_Q(MaterialDrawer);
    if (!q->parentWidget()) {
        // A drawer with nowhere to be drawn has nothing to cover: opening it
        // would be a promise the drawer cannot keep.
        return;
    }
    if (open) {
        installScrim();
        relayoutHost();
        q->move(-kPanelWidth, 0);
        q->show();
        q->raise();
    }
    slideTo(open);
}

void MaterialDrawerPrivate::resizePersistently(bool open)
{
    Q_Q(MaterialDrawer);
    // A persistent drawer is part of the page's geometry, not a thing sliding
    // over it: the page beside it has to be told, once, that the room changed.
    q->setFixedWidth(open ? kPanelWidth : 0);
    q->setVisible(open);
}

MaterialDrawer::MaterialDrawer(QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new MaterialDrawerPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    setFixedWidth(kPanelWidth);
    hide();

    connect(&d_ptr->animation, &QVariantAnimation::valueChanged, this, [this](const QVariant &value) {
        move(qRound(value.toReal()), 0);
    });
    connect(&d_ptr->animation, &QVariantAnimation::finished, this, [this]() {
        if (!d_ptr->open) {
            hide();
            d_ptr->dropScrim();
        }
    });
}

MaterialDrawer::~MaterialDrawer(){QX_FINI_PRIVATE()}

MaterialDrawer::Variant MaterialDrawer::variant() const
{
    return d_ptr->variant;
}

void MaterialDrawer::setVariant(Variant variant)
{
    if (d_ptr->variant == variant) {
        return;
    }
    const bool wasOpen = d_ptr->open;
    d_ptr->animation.stop();
    if (d_ptr->variant == Variant::Temporary) {
        // Leaving the variant that covers the page: the sheet goes with it, and
        // the panel stops standing where the covers put it.
        d_ptr->dropScrim();
        move(0, 0);
    }
    d_ptr->variant = variant;
    if (variant == Variant::Persistent) {
        setFixedWidth(wasOpen ? kPanelWidth : 0);
        setVisible(wasOpen);
    } else {
        setFixedWidth(kPanelWidth);
        setVisible(wasOpen);
    }
    update();
}

QAction *MaterialDrawer::addDestination(const QString &label, const QIcon &icon)
{
    QAction *action = new QAction(icon, label, this);
    MaterialDrawerPrivate::Entry entry;
    entry.kind = MaterialDrawerPrivate::Kind::Destination;
    entry.action = action;
    entry.destination = d_ptr->items.count();
    d_ptr->items.append(action);
    d_ptr->entries.append(entry);
    if (d_ptr->focused < 0) {
        d_ptr->focused = 0;
    }
    if (d_ptr->current < 0) {
        d_ptr->current = 0;
    }
    updateGeometry();
    update();
    return action;
}

void MaterialDrawer::addSection(const QString &title)
{
    MaterialDrawerPrivate::Entry entry;
    entry.kind = MaterialDrawerPrivate::Kind::Section;
    entry.title = title;
    d_ptr->entries.append(entry);
    updateGeometry();
    update();
}

void MaterialDrawer::addDivider()
{
    MaterialDrawerPrivate::Entry entry;
    entry.kind = MaterialDrawerPrivate::Kind::Divider;
    d_ptr->entries.append(entry);
    updateGeometry();
    update();
}

void MaterialDrawer::removeDestination(QAction *action)
{
    int destination = -1;
    for (int i = 0; i < d_ptr->items.count(); ++i) {
        if (d_ptr->items.at(i) == action) {
            destination = i;
            break;
        }
    }
    if (destination < 0) {
        return;
    }
    d_ptr->items.removeAt(destination);
    // The entries remember where their destination stood, and every one after
    // the one going moves up by one.
    for (int i = d_ptr->entries.count() - 1; i >= 0; --i) {
        MaterialDrawerPrivate::Entry &entry = d_ptr->entries[i];
        if (entry.kind != MaterialDrawerPrivate::Kind::Destination) {
            continue;
        }
        if (entry.destination == destination) {
            d_ptr->entries.removeAt(i);
        } else if (entry.destination > destination) {
            --entry.destination;
        }
    }
    if (d_ptr->current >= d_ptr->items.count()) {
        d_ptr->current = d_ptr->items.isEmpty() ? -1 : d_ptr->items.count() - 1;
    }
    if (d_ptr->focused >= d_ptr->items.count()) {
        d_ptr->focused = d_ptr->items.isEmpty() ? -1 : d_ptr->items.count() - 1;
    }
    d_ptr->hovered = -1;
    d_ptr->clampOffset();
    delete action;
    updateGeometry();
    update();
}

void MaterialDrawer::clear()
{
    if (d_ptr->entries.isEmpty()) {
        return;
    }
    const QList<QAction *> items = d_ptr->items;
    d_ptr->entries.clear();
    d_ptr->items.clear();
    d_ptr->current = -1;
    d_ptr->focused = -1;
    d_ptr->hovered = -1;
    d_ptr->offset = 0.0;
    for (QAction *action : items) {
        delete action;
    }
    updateGeometry();
    update();
}

QList<QAction *> MaterialDrawer::destinations() const
{
    QList<QAction *> actions;
    actions.reserve(d_ptr->items.count());
    for (QAction *action : d_ptr->items) {
        actions.append(action);
    }
    return actions;
}

QAction *MaterialDrawer::currentDestination() const
{
    if (d_ptr->current < 0 || d_ptr->current >= d_ptr->items.count()) {
        return Q_NULLPTR;
    }
    return d_ptr->items.at(d_ptr->current);
}

int MaterialDrawer::currentIndex() const
{
    return d_ptr->current;
}

bool MaterialDrawer::isOpen() const
{
    return d_ptr->open;
}

QRect MaterialDrawer::destinationRect(int index) const
{
    const int entry = d_ptr->destinationEntry(index);
    if (entry < 0) {
        return QRect();
    }
    const QRectF room = d_ptr->entryRect(entry);
    if (!QRectF(0.0, d_ptr->contentTop(), qreal(width()), qMax(0.0, qreal(height()) - d_ptr->contentTop()))
             .intersects(room)) {
        return QRect();
    }
    return room.toRect();
}

void MaterialDrawer::setHeaderWidget(QWidget *widget)
{
    if (d_ptr->header == widget) {
        return;
    }
    if (d_ptr->header) {
        d_ptr->header->setParent(Q_NULLPTR);
        d_ptr->header->hide();
    }
    d_ptr->header = widget;
    if (widget) {
        widget->setParent(this);
        widget->show();
    }
    d_ptr->layoutHeader();
    d_ptr->clampOffset();
    updateGeometry();
    update();
}

QWidget *MaterialDrawer::headerWidget() const
{
    return d_ptr->header;
}

int MaterialDrawer::panelWidth()
{
    return kPanelWidth;
}

void MaterialDrawer::setCurrentIndex(int index)
{
    if (d_ptr->current == index) {
        return;
    }
    if (index >= 0 && index >= d_ptr->items.count()) {
        return;
    }
    d_ptr->current = index;
    const int entry = d_ptr->destinationEntry(index);
    if (entry >= 0) {
        d_ptr->ensureVisible(entry);
    }
    update();
    Q_EMIT currentIndexChanged(index);
    Q_EMIT currentDestinationChanged(currentDestination());
}

void MaterialDrawer::setCurrentDestination(QAction *action)
{
    for (int i = 0; i < d_ptr->items.count(); ++i) {
        if (d_ptr->items.at(i) == action) {
            setCurrentIndex(i);
            return;
        }
    }
}

void MaterialDrawer::setOpen(bool open)
{
    if (d_ptr->open == open) {
        return;
    }
    d_ptr->open = open;
    if (d_ptr->variant == Variant::Temporary) {
        d_ptr->placeTemporarily(open);
    } else {
        d_ptr->resizePersistently(open);
    }
    Q_EMIT openChanged(open);
}

void MaterialDrawer::open()
{
    setOpen(true);
}

void MaterialDrawer::close()
{
    setOpen(false);
}

void MaterialDrawer::toggle()
{
    setOpen(!d_ptr->open);
}

QSize MaterialDrawer::sizeHint() const
{
    return QSize(kPanelWidth, int(d_ptr->contentTop() + d_ptr->contentHeight() + kTopPadding));
}

QSize MaterialDrawer::minimumSizeHint() const
{
    return QSize(kPanelWidth, int(d_ptr->contentTop() + kItemHeight + kTopPadding));
}

QRectF MaterialDrawer::chromeRect() const
{
    return QRectF();
}

void MaterialDrawer::paintSurface(QPainter *painter)
{
    const QRectF room(0.0, 0.0, qreal(width()), qreal(height()));
    painter->fillRect(room, palette().color(QPalette::Window));
    // A drawer is a surface the page ends at, so it says where it starts with
    // the hairline Material draws between surfaces - along the edge it moved
    // in from, which is the one a reader looks at.
    painter->setPen(Qt::NoPen);
    painter->setBrush(QxControls::QxStateLayer::layered(palette().color(QPalette::Window), textColor(), 0.12));
    painter->drawRect(QRectF(room.right() - 1.0, room.top(), 1.0, room.height()));
}

void MaterialDrawer::paintContent(QPainter *painter)
{
    const bool enabled = isEnabled();
    const QColor accent = accentColor();
    const QColor ink = textColor();
    const qreal radius = MaterialShape::radius(MaterialShape::Radius::Circular);

    painter->save();
    painter->setClipRect(
        QRectF(0.0, d_ptr->contentTop(), qreal(width()), qMax(0.0, qreal(height()) - d_ptr->contentTop())));
    for (int i = 0; i < d_ptr->entries.count(); ++i) {
        const MaterialDrawerPrivate::Entry &entry = d_ptr->entries.at(i);
        const QRectF room = d_ptr->entryRect(i);
        if (!room.intersects(QRectF(0.0, 0.0, qreal(width()), qreal(height())))) {
            continue;
        }

        if (entry.kind == MaterialDrawerPrivate::Kind::Section) {
            painter->setOpacity(enabled ? kSectionOpacity : kDisabledOpacity);
            painter->setFont(MaterialType::font(MaterialType::Scale::Label, true));
            painter->setPen(ink);
            painter->drawText(room.adjusted(kSidePadding + kIconInset, 0.0, -kSidePadding - kIconInset, 0.0),
                              Qt::AlignLeft | Qt::AlignBottom | Qt::TextSingleLine, entry.title);
            continue;
        }
        if (entry.kind == MaterialDrawerPrivate::Kind::Divider) {
            painter->setOpacity(enabled ? 1.0 : kDisabledOpacity);
            painter->setPen(Qt::NoPen);
            painter->setBrush(QxControls::QxStateLayer::layered(palette().color(QPalette::Window), ink, 0.12));
            painter->drawRect(QRectF(kSidePadding, room.center().y(), qreal(width()) - 2 * kSidePadding, 1.0));
            continue;
        }

        QAction *action = entry.action;
        const bool usable = action && action->isEnabled();
        const bool inForce = entry.destination == d_ptr->current;
        const QRectF pill = d_ptr->pillOf(room);

        const auto wash = [painter, &pill, radius, &ink](qreal opacity) {
            QPainterPath clip;
            clip.addRoundedRect(pill, radius, radius);
            painter->save();
            painter->setClipPath(clip);
            QxControls::QxStateLayer::paint(painter, pill, ink, opacity);
            painter->restore();
        };

        painter->setOpacity(enabled ? (usable ? 1.0 : kDisabledOpacity) : kDisabledOpacity);
        if (inForce) {
            // The pill: the destination in force wears a shape, and the shape
            // is the accent's - the drawer's answer is the rail's answer.
            painter->setPen(Qt::NoPen);
            painter->setBrush(
                QxControls::QxStateLayer::layered(palette().color(QPalette::Window), accent, kIndicatorOpacity));
            painter->drawRoundedRect(pill, radius, radius);
        } else if (i == d_ptr->hovered) {
            wash(QxControls::QxStateLayer::hoverOpacity);
        }
        if (hasFocus() && entry.destination == d_ptr->focused) {
            wash(QxControls::QxStateLayer::focusOpacity);
        }

        painter->setOpacity(enabled ? (usable ? (inForce ? 1.0 : kRestingOpacity) : kDisabledOpacity)
                                    : kDisabledOpacity);
        const QColor drawn = inForce ? accent : ink;
        if (action && !action->icon().isNull()) {
            const QRectF iconRoom(pill.left() + kIconInset, pill.center().y() - kIconSize / 2.0, kIconSize, kIconSize);
            action->icon().paint(painter, iconRoom.toRect(), Qt::AlignCenter, usable ? QIcon::Normal : QIcon::Disabled);
        }
        if (action && !action->text().isEmpty()) {
            const QFontMetrics metrics(MaterialType::font(MaterialType::Scale::Body));
            const qreal left = pill.left() + kIconInset + kIconSize + kTextGap;
            const qreal right = qreal(width()) - kSidePadding - kIconInset;
            const QString shortened = metrics.elidedText(action->text(), Qt::ElideRight, qMax(0, int(right - left)));
            painter->setFont(MaterialType::font(MaterialType::Scale::Body));
            painter->setPen(drawn);
            painter->drawText(QRectF(left, pill.top(), qMax(0.0, right - left), pill.height()),
                              Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine, shortened);
        }
    }
    painter->restore();
}

void MaterialDrawer::resizeEvent(QResizeEvent *event)
{
    d_ptr->clampOffset();
    d_ptr->layoutHeader();
    QxControls::QxControl::resizeEvent(event);
}

void MaterialDrawer::mousePressEvent(QMouseEvent *event)
{
    d_ptr->hovered = d_ptr->entryAt(eventPos(event));
    update();
}

void MaterialDrawer::mouseReleaseEvent(QMouseEvent *event)
{
    const int entry = d_ptr->entryAt(eventPos(event));
    if (entry >= 0 && entry == d_ptr->hovered) {
        const int destination = d_ptr->entries.at(entry).destination;
        QAction *action = d_ptr->items.value(destination);
        if (action && action->isEnabled()) {
            d_ptr->focused = destination;
            setCurrentIndex(destination);
        }
    }
    update();
}

void MaterialDrawer::mouseMoveEvent(QMouseEvent *event)
{
    const int hovered = d_ptr->entryAt(eventPos(event));
    if (hovered != d_ptr->hovered) {
        d_ptr->hovered = hovered;
        update();
    }
}

void MaterialDrawer::wheelEvent(QWheelEvent *event)
{
    const int delta = event->angleDelta().y();
    if (delta == 0) {
        QxControls::QxControl::wheelEvent(event);
        return;
    }
    d_ptr->offset -= delta > 0 ? kItemHeight : -kItemHeight;
    d_ptr->clampOffset();
    update();
    event->accept();
}

void MaterialDrawer::leaveEvent(QEvent *event)
{
    Q_UNUSED(event)
    d_ptr->hovered = -1;
    update();
}

void MaterialDrawer::keyPressEvent(QKeyEvent *event)
{
    const int count = d_ptr->items.count();
    if (count == 0) {
        QWidget::keyPressEvent(event);
        return;
    }
    if (d_ptr->focused < 0) {
        d_ptr->focused = 0;
    }
    switch (event->key()) {
    case Qt::Key_Down:
        d_ptr->focused = qMin(d_ptr->focused + 1, count - 1);
        setCurrentIndex(d_ptr->focused);
        event->accept();
        return;
    case Qt::Key_Up:
        d_ptr->focused = qMax(d_ptr->focused - 1, 0);
        setCurrentIndex(d_ptr->focused);
        event->accept();
        return;
    case Qt::Key_Escape:
        if (d_ptr->open && d_ptr->variant == Variant::Temporary) {
            close();
            event->accept();
            return;
        }
        break;
    default:
        break;
    }
    QxControls::QxControl::keyPressEvent(event);
}

void MaterialDrawer::changeEvent(QEvent *event)
{
    QxControls::QxControl::changeEvent(event);
    switch (event->type()) {
    case QEvent::PaletteChange:
    case QEvent::FontChange:
    case QEvent::EnabledChange:
    case QEvent::StyleChange:
        update();
        break;
    default:
        break;
    }
}

bool MaterialDrawer::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == d_ptr->scrim && event->type() == QEvent::MouseButtonRelease) {
        // The sheet is what a temporary drawer comes out over, and pressing it
        // is pressing "not this one": Material's own way out of a modal thing.
        close();
        return true;
    }
    if (watched == d_ptr->host && event->type() == QEvent::Resize) {
        // The drawer covers the room it was put in, so when that room changes
        // the drawer and its sheet have to change with it.
        d_ptr->relayoutHost();
    }
    return QxControls::QxControl::eventFilter(watched, event);
}

QAccessible::Role MaterialDrawer::accessibleRole() const
{
    // A column of destinations that comes and goes: an assistive tool walks a
    // list, the same list the rail is.
    return QAccessible::List;
}

QX_MATERIAL_END_NAMESPACE
