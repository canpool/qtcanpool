/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "materialtopappbar.h"

#include "materialtokens.h"

#include "qxcontrols/qxstatelayer.h"

#include <QtGui/QFontMetrics>
#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>
#include <QtGui/QResizeEvent>
#include <QtWidgets/QWidget>
// QAction is in Widgets on Qt 5 and in Gui on Qt 6, so it is asked for without
// naming a module - the same way the rest of the libraries ask.
#include <QAction>

QX_MATERIAL_BEGIN_NAMESPACE

namespace
{
// The three rooms a top app bar is given: fifty-six for the two that name the
// screen beside its icons, one hundred and twelve and one hundred and fifty-
// two for the two that give the name a row of its own.
constexpr qreal kHeightSmall = 56.0;
constexpr qreal kHeightMedium = 112.0;
constexpr qreal kHeightLarge = 152.0;

// An icon is twenty-four inside a target of forty-eight, and the target keeps
// four from the edge - so two icons side by side read twenty-four apart
// without a gap being asked for.
constexpr qreal kIconButton = 48.0;
constexpr qreal kIconSize = 24.0;
constexpr qreal kEdgePadding = 4.0;
constexpr qreal kTitleGap = 16.0;
constexpr qreal kTitlePadding = 16.0;

// A bar nobody can use is the same drawing thinned.
constexpr qreal kDisabledOpacity = 0.38;

QPointF eventPos(const QMouseEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return event->position();
#else
    return event->localPos();
#endif
}
}   // namespace

struct BarIcon {
    QAction *action = Q_NULLPTR;
    QRectF rect;
    QPointF iconPos;
};

class MaterialTopAppBarPrivate
{
public:
    QX_DECLARE_PUBLIC(MaterialTopAppBar)

    MaterialTopAppBar::Variant variant = MaterialTopAppBar::Variant::Small;
    QString title;
    bool scrolled = false;
    QAction *leading = Q_NULLPTR;
    QList<QAction *> trailing;
    QList<BarIcon> icons;
    int hovered = -1;
    int armed = -1;
    int focused = -1;

    qreal barHeight() const;
    MaterialType::Scale titleScale() const;
    void relayout();
    int itemAt(const QPointF &pos);
    void activate(int index);
    void dropAction(QAction *action);
};

qreal MaterialTopAppBarPrivate::barHeight() const
{
    switch (variant) {
    case MaterialTopAppBar::Variant::CenterAligned:
        return kHeightSmall;
    case MaterialTopAppBar::Variant::Medium:
        return kHeightMedium;
    case MaterialTopAppBar::Variant::Large:
        return kHeightLarge;
    default:
        return kHeightSmall;
    }
}

MaterialType::Scale MaterialTopAppBarPrivate::titleScale() const
{
    // The two tall shapes are the two that give the name room of its own, and
    // what they spend the room on is size: the name of a screen the user is
    // going to scroll a long way through is worth reading from further away.
    if (variant == MaterialTopAppBar::Variant::Large) {
        return MaterialType::Scale::Display;
    }
    return MaterialType::Scale::Title;
}

void MaterialTopAppBarPrivate::relayout()
{
    Q_Q(MaterialTopAppBar);
    icons.clear();

    // The icons stand in the top fifty-six whatever the shape is: a taller bar
    // is a bar with more room under them, not one with bigger icons.
    const qreal top = (kHeightSmall - kIconButton) / 2.0;
    if (leading) {
        BarIcon icon;
        icon.action = leading;
        icon.rect = QRectF(kEdgePadding, top, kIconButton, kIconButton);
        icons.append(icon);
    }
    // The trailing ones are placed from the right edge inwards, and kept in
    // the order they were added, which is the order the keyboard walks them
    // in: left to right, whatever order they were laid out in.
    QList<BarIcon> placed;
    qreal right = qreal(q->width()) - kEdgePadding;
    for (int i = trailing.count() - 1; i >= 0; --i) {
        BarIcon icon;
        icon.action = trailing.at(i);
        icon.rect = QRectF(right - kIconButton, top, kIconButton, kIconButton);
        placed.prepend(icon);
        right -= kIconButton;
    }
    icons.append(placed);
    for (BarIcon &icon : icons) {
        icon.iconPos = QPointF(icon.rect.center().x() - kIconSize / 2.0, icon.rect.center().y() - kIconSize / 2.0);
    }
}

int MaterialTopAppBarPrivate::itemAt(const QPointF &pos)
{
    // Where the icons stand is a function of the room the bar was given, so it
    // is worked out when it is asked for rather than when the room changed: a
    // bar that has never been shown is never told it was resized, and one that
    // answered a press from a layout it made before that would answer with
    // where its icons used to be.
    relayout();
    for (int i = 0; i < icons.count(); ++i) {
        if (icons.at(i).rect.contains(pos)) {
            return i;
        }
    }
    return -1;
}

void MaterialTopAppBarPrivate::activate(int index)
{
    if (index < 0 || index >= icons.count()) {
        return;
    }
    QAction *action = icons.at(index).action;
    if (!action || !action->isEnabled()) {
        return;
    }
    // What happens is the action's to say, not the bar's: the bar's only
    // answer to a press is which icon it landed on.
    action->trigger();
}

void MaterialTopAppBarPrivate::dropAction(QAction *action)
{
    if (!action) {
        return;
    }
    if (leading == action) {
        leading = Q_NULLPTR;
    } else {
        trailing.removeAll(action);
    }
    delete action;
}

MaterialTopAppBar::MaterialTopAppBar(QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new MaterialTopAppBarPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    // Which icon the pointer is on is what the disc behind it answers, so
    // moves are wanted without a button.
    setMouseTracking(true);
}

MaterialTopAppBar::~MaterialTopAppBar()
{
    clearLeadingAction();
    clearTrailingActions();
    QX_FINI_PRIVATE()
}

QString MaterialTopAppBar::title() const
{
    return d_ptr->title;
}

void MaterialTopAppBar::setTitle(const QString &title)
{
    if (d_ptr->title == title) {
        return;
    }
    d_ptr->title = title;
    update();
}

MaterialTopAppBar::Variant MaterialTopAppBar::variant() const
{
    return d_ptr->variant;
}

void MaterialTopAppBar::setVariant(Variant variant)
{
    if (d_ptr->variant == variant) {
        return;
    }
    d_ptr->variant = variant;
    d_ptr->relayout();
    updateGeometry();
    update();
}

bool MaterialTopAppBar::isScrolled() const
{
    return d_ptr->scrolled;
}

void MaterialTopAppBar::setScrolled(bool scrolled)
{
    if (d_ptr->scrolled == scrolled) {
        return;
    }
    d_ptr->scrolled = scrolled;
    update();
}

QAction *MaterialTopAppBar::setLeadingAction(const QString &text, const QIcon &icon)
{
    clearLeadingAction();
    d_ptr->leading = new QAction(icon, text, this);
    d_ptr->relayout();
    update();
    return d_ptr->leading;
}

void MaterialTopAppBar::clearLeadingAction()
{
    if (!d_ptr->leading) {
        return;
    }
    d_ptr->dropAction(d_ptr->leading);
    d_ptr->hovered = -1;
    d_ptr->focused = -1;
    d_ptr->relayout();
    update();
}

QAction *MaterialTopAppBar::leadingAction() const
{
    return d_ptr->leading;
}

QAction *MaterialTopAppBar::addTrailingAction(const QString &text, const QIcon &icon)
{
    QAction *action = new QAction(icon, text, this);
    d_ptr->trailing.append(action);
    if (d_ptr->focused < 0) {
        d_ptr->focused = 0;
    }
    d_ptr->relayout();
    update();
    return action;
}

void MaterialTopAppBar::removeTrailingAction(QAction *action)
{
    if (!d_ptr->trailing.contains(action)) {
        return;
    }
    d_ptr->dropAction(action);
    d_ptr->hovered = -1;
    d_ptr->relayout();
    update();
}

void MaterialTopAppBar::clearTrailingActions()
{
    if (d_ptr->trailing.isEmpty()) {
        return;
    }
    const QList<QAction *> actions = d_ptr->trailing;
    d_ptr->trailing.clear();
    for (QAction *action : actions) {
        delete action;
    }
    d_ptr->hovered = -1;
    d_ptr->focused = -1;
    d_ptr->relayout();
    update();
}

QList<QAction *> MaterialTopAppBar::trailingActions() const
{
    return d_ptr->trailing;
}

QAction *MaterialTopAppBar::actionAt(const QPoint &pos) const
{
    // The same layout a press is answered from, asked in the same way: a bar
    // that told a caller one thing about where an icon is and then answered a
    // press somewhere else would be two answers to one question.
    const int index = d_ptr->itemAt(pos);
    if (index < 0 || index >= d_ptr->icons.count()) {
        return Q_NULLPTR;
    }
    return d_ptr->icons.at(index).action;
}

int MaterialTopAppBar::heightFor(Variant variant)
{
    switch (variant) {
    case Variant::CenterAligned:
        return int(kHeightSmall);
    case Variant::Medium:
        return int(kHeightMedium);
    case Variant::Large:
        return int(kHeightLarge);
    default:
        return int(kHeightSmall);
    }
}

QSize MaterialTopAppBar::sizeHint() const
{
    return QSize(640, heightFor(d_ptr->variant));
}

QSize MaterialTopAppBar::minimumSizeHint() const
{
    // Wide enough for the icons there are and a word of the name, which is
    // what a bar squeezed to nothing would still owe the user.
    const int icons = int(d_ptr->icons.count()) * int(kIconButton) + 2 * int(kEdgePadding);
    return QSize(icons + 4 * int(kTitleGap), heightFor(d_ptr->variant));
}

qreal MaterialTopAppBar::cornerRadius() const
{
    // The disc behind an icon, which is what Material gives an icon button.
    // The bar's own corners are square and are drawn in the surface.
    return MaterialShape::radius(MaterialShape::Radius::Circular);
}

QRectF MaterialTopAppBar::chromeRect() const
{
    d_ptr->relayout();
    const int index = d_ptr->hovered >= 0 ? d_ptr->hovered : d_ptr->focused;
    if (index < 0 || index >= d_ptr->icons.count()) {
        return QRectF();
    }
    return d_ptr->icons.at(index).rect;
}

void MaterialTopAppBar::paintSurface(QPainter *painter)
{
    const QPalette palette = this->palette();
    // At rest the bar is the page; scrolled under, it is the surface one step
    // away. That change is the whole of what Material calls on-scroll
    // elevation - a line or a shadow would be a second answer to the same
    // question, and a bar that never changes is a bar the user cannot tell is
    // above what is moving under it.
    const QColor fill = d_ptr->scrolled ? palette.color(QPalette::AlternateBase) : palette.color(QPalette::Window);
    painter->fillRect(QRectF(0.0, 0.0, qreal(width()), qreal(height())), fill);
}

void MaterialTopAppBar::paintContent(QPainter *painter)
{
    d_ptr->relayout();
    const bool enabled = isEnabled();
    const QFont titleFont = MaterialType::font(d_ptr->titleScale(), true);
    const QFontMetrics metrics(titleFont);
    const bool tall = d_ptr->variant == Variant::Medium || d_ptr->variant == Variant::Large;

    qreal left = kEdgePadding;
    if (d_ptr->leading) {
        left += kIconButton + kTitleGap;
    } else {
        left = kTitleGap;
    }
    qreal right = qreal(width()) - kEdgePadding;
    if (!d_ptr->trailing.isEmpty()) {
        right -= qreal(d_ptr->trailing.count()) * kIconButton + kTitleGap;
    }
    QRectF titleRect(left, 0.0, qMax(right - left, 0.0), kHeightSmall);
    if (tall) {
        titleRect = QRectF(kTitlePadding, kHeightSmall, qMax(qreal(width()) - 2.0 * kTitlePadding, 0.0),
                           d_ptr->barHeight() - kHeightSmall);
    }

    if (!d_ptr->title.isEmpty()) {
        const QString text = metrics.elidedText(d_ptr->title, Qt::ElideRight, int(titleRect.width()));
        painter->setFont(titleFont);
        painter->setPen(textColor());
        painter->setOpacity(enabled ? 1.0 : kDisabledOpacity);
        const int flags = d_ptr->variant == Variant::CenterAligned && !tall
                              ? (Qt::AlignHCenter | Qt::AlignVCenter | Qt::TextSingleLine)
                              : (Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine);
        painter->drawText(titleRect, flags, text);
        painter->setOpacity(1.0);
    }

    for (int i = 0; i < d_ptr->icons.count(); ++i) {
        const BarIcon &icon = d_ptr->icons.at(i);
        const bool usable = icon.action->isEnabled();
        painter->setOpacity(enabled ? (usable ? 1.0 : kDisabledOpacity) : kDisabledOpacity);
        if (!icon.action->icon().isNull()) {
            icon.action->icon().paint(painter, QRectF(icon.iconPos, QSizeF(kIconSize, kIconSize)).toRect(),
                                      Qt::AlignCenter, usable ? QIcon::Normal : QIcon::Disabled);
        }
    }
    painter->setOpacity(1.0);
}

void MaterialTopAppBar::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
    d_ptr->relayout();
    QxControls::QxControl::resizeEvent(event);
}

void MaterialTopAppBar::mousePressEvent(QMouseEvent *event)
{
    QxControls::QxControl::mousePressEvent(event);
    if (!isEnabled() || event->button() != Qt::LeftButton) {
        return;
    }
    d_ptr->armed = d_ptr->itemAt(eventPos(event));
}

void MaterialTopAppBar::mouseReleaseEvent(QMouseEvent *event)
{
    QxControls::QxControl::mouseReleaseEvent(event);
    if (event->button() != Qt::LeftButton) {
        return;
    }
    // A press on one icon and a release on another is not a click on either -
    // the same reading a button gives, applied to a row of them.
    const int index = d_ptr->itemAt(eventPos(event));
    const int armed = d_ptr->armed;
    d_ptr->armed = -1;
    if (index < 0 || index != armed) {
        return;
    }
    d_ptr->focused = index;
    d_ptr->activate(index);
    update();
}

void MaterialTopAppBar::mouseMoveEvent(QMouseEvent *event)
{
    QxControls::QxControl::mouseMoveEvent(event);
    const int index = isEnabled() ? d_ptr->itemAt(eventPos(event)) : -1;
    if (index == d_ptr->hovered) {
        return;
    }
    d_ptr->hovered = index;
    update();
}

void MaterialTopAppBar::leaveEvent(QEvent *event)
{
    QxControls::QxControl::leaveEvent(event);
    if (d_ptr->hovered == -1) {
        return;
    }
    d_ptr->hovered = -1;
    update();
}

void MaterialTopAppBar::keyPressEvent(QKeyEvent *event)
{
    const int count = d_ptr->icons.count();
    if (count == 0) {
        QxControls::QxControl::keyPressEvent(event);
        return;
    }
    if (d_ptr->focused < 0) {
        d_ptr->focused = 0;
    }
    switch (event->key()) {
    case Qt::Key_Right:
        d_ptr->focused = qMin(d_ptr->focused + 1, count - 1);
        event->accept();
        update();
        return;
    case Qt::Key_Left:
        d_ptr->focused = qMax(d_ptr->focused - 1, 0);
        event->accept();
        update();
        return;
    case Qt::Key_Enter:
    case Qt::Key_Return:
    case Qt::Key_Space:
        d_ptr->activate(d_ptr->focused);
        event->accept();
        return;
    default:
        break;
    }
    QxControls::QxControl::keyPressEvent(event);
}

QAccessible::Role MaterialTopAppBar::accessibleRole() const
{
    // A row of things that can be done to the screen: what a toolbar is to an
    // assistive tool, and the name of the screen is read from the bar itself.
    return QAccessible::ToolBar;
}

QX_MATERIAL_END_NAMESPACE
