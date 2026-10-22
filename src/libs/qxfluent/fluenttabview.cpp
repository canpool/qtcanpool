/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "fluenttabview.h"

#include "fluenttokens.h"

#include "qxcontrols/qxstatelayer.h"

#include <QtGui/QFontMetrics>
#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>
#include <QtGui/QResizeEvent>
#include <QtWidgets/QWidget>
// QAction is in Widgets on Qt 5 and in Gui on Qt 6, so it is asked for without
// naming a module - the same way the rest of the libraries ask.
#include <QAction>

QX_FLUENT_BEGIN_NAMESPACE

namespace
{
// Fluent's tab measures: a row forty tall holding thirty-two tall tabs, an
// icon of sixteen, a close mark of sixteen, and a line three deep under the
// tab being looked at.
constexpr qreal kBarHeight = 40.0;
constexpr qreal kTabHeight = 32.0;
constexpr qreal kTabPadding = 12.0;
constexpr qreal kIconSize = 16.0;
constexpr qreal kIconGap = 8.0;
constexpr qreal kCloseSize = 16.0;
constexpr qreal kCloseGap = 4.0;
constexpr qreal kCloseGlyph = 8.0;
constexpr qreal kTabSpacing = 2.0;
constexpr qreal kIndicatorHeight = 3.0;
constexpr qreal kIndicatorInset = 12.0;

// The line under the tab being looked at is the accent; a tab nobody can use is
// the same drawing thinned. Neither moves the geometry.
constexpr qreal kDisabledOpacity = 0.4;
constexpr qreal kDividerOpacity = 0.10;

// What hitTest answers for the close mark, which is a place inside a tab.
constexpr int kCloseOf = -3;

QPointF eventPos(const QMouseEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return event->position();
#else
    return event->localPos();
#endif
}

/*! The cross that closes a tab: drawn, not typed, for the reason every glyph
 *  here is - a glyph in a font cannot be coloured by the theme and would be
 *  read aloud by an assistive tool. */
void paintCross(QPainter *painter, const QRectF &rect, const QColor &color)
{
    const qreal left = rect.center().x() - kCloseGlyph / 2.0;
    const qreal top = rect.center().y() - kCloseGlyph / 2.0;
    painter->setPen(QPen(color, 1.5, Qt::SolidLine, Qt::RoundCap));
    painter->drawLine(QLineF(left, top, left + kCloseGlyph, top + kCloseGlyph));
    painter->drawLine(QLineF(left + kCloseGlyph, top, left, top + kCloseGlyph));
}
}   // namespace

struct TabItem {
    QAction *action = Q_NULLPTR;
    QWidget *content = Q_NULLPTR;
    QRectF rect;
    /*! Empty while the tab is not closable. */
    QRectF close;
};

class FluentTabViewPrivate
{
public:
    QX_DECLARE_PUBLIC(FluentTabView)

    QList<TabItem> tabs;
    int hovered = -1;
    int armed = -1;
    int current = -1;
    int focused = -1;
    bool closeHovered = false;
    bool closable = false;

    qreal tabWidth(const TabItem &tab) const;
    void relayout();
    int tabAt(const QPointF &pos) const;
    bool onClose(const QPointF &pos) const;
    void showCurrent();
};

qreal FluentTabViewPrivate::tabWidth(const TabItem &tab) const
{
    const QFontMetrics metrics(FluentType::font(FluentType::Ramp::Body));
    qreal width = kTabPadding + qreal(metrics.horizontalAdvance(tab.action->text())) + kTabPadding;
    if (!tab.action->icon().isNull()) {
        width += kIconSize + kIconGap;
    }
    if (closable) {
        width += kCloseGap + kCloseSize;
    }
    return qMax(width, kTabHeight);
}

void FluentTabViewPrivate::relayout()
{
    Q_Q(FluentTabView);
    qreal left = 0.0;
    for (TabItem &tab : tabs) {
        const qreal width = tabWidth(tab);
        tab.rect = QRectF(left, (kBarHeight - kTabHeight) / 2.0, width, kTabHeight);
        if (closable) {
            tab.close = QRectF(left + width - kTabPadding / 2.0 - kCloseSize, tab.rect.center().y() - kCloseSize / 2.0,
                               kCloseSize, kCloseSize);
        } else {
            tab.close = QRectF();
        }
        left += width + kTabSpacing;
    }
    const int top = int(kBarHeight);
    for (TabItem &tab : tabs) {
        if (tab.content) {
            tab.content->setGeometry(0, top, q->width(), qMax(0, q->height() - top));
        }
    }
    showCurrent();
}

int FluentTabViewPrivate::tabAt(const QPointF &pos) const
{
    for (int i = 0; i < tabs.count(); ++i) {
        if (tabs.at(i).rect.contains(pos)) {
            return i;
        }
    }
    return -1;
}

bool FluentTabViewPrivate::onClose(const QPointF &pos) const
{
    for (const TabItem &tab : tabs) {
        if (!tab.close.isEmpty() && tab.close.contains(pos)) {
            return true;
        }
    }
    return false;
}

void FluentTabViewPrivate::showCurrent()
{
    for (int i = 0; i < tabs.count(); ++i) {
        if (tabs.at(i).content) {
            tabs.at(i).content->setVisible(i == current);
        }
    }
}

FluentTabView::FluentTabView(QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new FluentTabViewPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    // Which tab the pointer is on is part of what the row says, and the close
    // mark is only there while the pointer is: moves are wanted without a
    // button, the same reason a rating wants them.
    setMouseTracking(true);
}

FluentTabView::~FluentTabView(){QX_FINI_PRIVATE()}

QAction *FluentTabView::addTab(QWidget *content, const QString &text, const QIcon &icon)
{
    QAction *action = new QAction(icon, text, this);
    TabItem tab;
    tab.action = action;
    tab.content = content;
    if (content) {
        content->setParent(this);
    }
    d_ptr->tabs.append(tab);
    if (d_ptr->focused < 0) {
        d_ptr->focused = 0;
    }
    if (d_ptr->current < 0) {
        d_ptr->current = 0;
    }
    d_ptr->relayout();
    updateGeometry();
    update();
    return action;
}

void FluentTabView::removeTab(QAction *action)
{
    for (int i = 0; i < d_ptr->tabs.count(); ++i) {
        if (d_ptr->tabs.at(i).action != action) {
            continue;
        }
        QWidget *content = d_ptr->tabs.at(i).content;
        d_ptr->tabs.removeAt(i);
        // What was being looked at may be the tab that is going, and an index
        // is a position in a row that just got shorter.
        if (d_ptr->current >= d_ptr->tabs.count()) {
            d_ptr->current = d_ptr->tabs.isEmpty() ? -1 : d_ptr->tabs.count() - 1;
        }
        if (d_ptr->focused >= d_ptr->tabs.count()) {
            d_ptr->focused = d_ptr->tabs.isEmpty() ? -1 : d_ptr->tabs.count() - 1;
        }
        d_ptr->hovered = -1;
        // The content was the application's: it is let go, not deleted.
        if (content) {
            content->setParent(Q_NULLPTR);
            content->hide();
        }
        delete action;
        d_ptr->relayout();
        updateGeometry();
        update();
        return;
    }
}

void FluentTabView::clear()
{
    if (d_ptr->tabs.isEmpty()) {
        return;
    }
    const QList<TabItem> tabs = d_ptr->tabs;
    d_ptr->tabs.clear();
    d_ptr->current = -1;
    d_ptr->focused = -1;
    d_ptr->hovered = -1;
    for (const TabItem &tab : tabs) {
        if (tab.content) {
            tab.content->setParent(Q_NULLPTR);
            tab.content->hide();
        }
        delete tab.action;
    }
    d_ptr->relayout();
    updateGeometry();
    update();
}

QList<QAction *> FluentTabView::tabs() const
{
    QList<QAction *> actions;
    actions.reserve(d_ptr->tabs.count());
    for (const TabItem &tab : d_ptr->tabs) {
        actions.append(tab.action);
    }
    return actions;
}

QWidget *FluentTabView::contentOf(QAction *action) const
{
    for (const TabItem &tab : d_ptr->tabs) {
        if (tab.action == action) {
            return tab.content;
        }
    }
    return Q_NULLPTR;
}

QAction *FluentTabView::currentTab() const
{
    if (d_ptr->current < 0 || d_ptr->current >= d_ptr->tabs.count()) {
        return Q_NULLPTR;
    }
    return d_ptr->tabs.at(d_ptr->current).action;
}

int FluentTabView::currentIndex() const
{
    return d_ptr->current;
}

bool FluentTabView::isClosable() const
{
    return d_ptr->closable;
}

void FluentTabView::setClosable(bool closable)
{
    if (d_ptr->closable == closable) {
        return;
    }
    d_ptr->closable = closable;
    d_ptr->relayout();
    updateGeometry();
    update();
    Q_EMIT closableChanged(closable);
}

void FluentTabView::setCurrentIndex(int index)
{
    if (d_ptr->current == index) {
        return;
    }
    if (index >= 0 && index >= d_ptr->tabs.count()) {
        return;
    }
    d_ptr->current = index;
    d_ptr->showCurrent();
    update();
    Q_EMIT currentIndexChanged(index);
    Q_EMIT currentTabChanged(currentTab());
}

void FluentTabView::setCurrentTab(QAction *action)
{
    for (int i = 0; i < d_ptr->tabs.count(); ++i) {
        if (d_ptr->tabs.at(i).action == action) {
            setCurrentIndex(i);
            return;
        }
    }
}

QSize FluentTabView::sizeHint() const
{
    return QSize(400, int(kBarHeight) + 240);
}

QSize FluentTabView::minimumSizeHint() const
{
    return QSize(120, int(kBarHeight) + kTabHeight);
}

QRectF FluentTabView::chromeRect() const
{
    // Nothing about this view is the control: it is a row of tabs and a room
    // under it, and a wash round the whole of it would be a wash round the
    // content too. A tab draws its own.
    return QRectF();
}

void FluentTabView::paintSurface(QPainter *painter)
{
    // The row is where the choice is made and the room under it is where the
    // thing chosen shows; a line between them is all that says so.
    QColor divider = textColor();
    divider.setAlphaF(kDividerOpacity);
    painter->setPen(divider);
    painter->drawLine(QLineF(0.0, kBarHeight - 0.5, qreal(width()), kBarHeight - 0.5));
}

void FluentTabView::paintContent(QPainter *painter)
{
    const bool enabled = isEnabled();
    const QFont font = FluentType::font(FluentType::Ramp::Body);
    const QFontMetrics metrics(font);
    const QColor accent = accentColor();

    for (int i = 0; i < d_ptr->tabs.count(); ++i) {
        const TabItem &tab = d_ptr->tabs.at(i);
        if (tab.rect.right() < 0 || tab.rect.left() > width()) {
            continue;
        }
        const bool usable = tab.action->isEnabled();
        painter->setOpacity(enabled ? (usable ? 1.0 : kDisabledOpacity) : kDisabledOpacity);

        if (i == d_ptr->hovered && !d_ptr->closeHovered) {
            QxControls::QxStateLayer::paint(painter, tab.rect, textColor(), QxControls::QxStateLayer::hoverOpacity);
        }
        if (hasFocus() && i == d_ptr->focused) {
            QxControls::QxStateLayer::paint(painter, tab.rect, textColor(), QxControls::QxStateLayer::focusOpacity);
        }
        if (d_ptr->closable && !tab.close.isEmpty() && i == d_ptr->hovered && d_ptr->closeHovered) {
            QxControls::QxStateLayer::paint(painter, tab.close, textColor(), QxControls::QxStateLayer::hoverOpacity);
        }

        qreal left = tab.rect.left() + kTabPadding;
        if (!tab.action->icon().isNull()) {
            const QRectF iconRect(left, tab.rect.center().y() - kIconSize / 2.0, kIconSize, kIconSize);
            tab.action->icon().paint(painter, iconRect.toRect(), Qt::AlignCenter,
                                     usable ? QIcon::Normal : QIcon::Disabled);
            left += kIconSize + kIconGap;
        }
        const qreal textRight =
            d_ptr->closable && !tab.close.isEmpty() ? tab.close.left() - kCloseGap : tab.rect.right() - kTabPadding;
        const qreal room = textRight - left;
        const QString text = room > 0 ? metrics.elidedText(tab.action->text(), Qt::ElideRight, int(room)) : QString();
        if (!text.isEmpty()) {
            painter->setFont(font);
            painter->setPen(textColor());
            painter->drawText(QRectF(left, tab.rect.top(), room, tab.rect.height()),
                              Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine, text);
        }
        if (d_ptr->closable && !tab.close.isEmpty()) {
            paintCross(painter, tab.close, textColor());
        }

        // The line under the tab: Fluent marks the one being looked at here
        // rather than by shaping the tab, so a tab is a tab whichever one it
        // is, and what moves is the line.
        if (i == d_ptr->current) {
            const qreal barLeft = tab.rect.left() + kIndicatorInset;
            const qreal barWidth = qMax(qreal(0), tab.rect.width() - 2 * kIndicatorInset);
            painter->setPen(Qt::NoPen);
            painter->setBrush(accent);
            painter->drawRoundedRect(QRectF(barLeft, tab.rect.bottom() - kIndicatorHeight, barWidth, kIndicatorHeight),
                                     kIndicatorHeight / 2.0, kIndicatorHeight / 2.0);
        }
    }
    painter->setOpacity(1.0);
}

void FluentTabView::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
    d_ptr->relayout();
    QxControls::QxControl::resizeEvent(event);
}

void FluentTabView::mousePressEvent(QMouseEvent *event)
{
    const QPointF pos = eventPos(event);
    const int index = d_ptr->tabAt(pos);
    d_ptr->armed = (index >= 0 && d_ptr->onClose(pos)) ? kCloseOf : index;
    update();
}

void FluentTabView::mouseReleaseEvent(QMouseEvent *event)
{
    const QPointF pos = eventPos(event);
    const int armed = d_ptr->armed;
    d_ptr->armed = -1;
    const int index = d_ptr->tabAt(pos);
    // The mark and the tab are two answers to one press, and the mark's answer
    // is the one that is not a number: it says "close", not "the third one".
    if (armed == kCloseOf) {
        if (index >= 0 && d_ptr->onClose(pos)) {
            Q_EMIT closeRequested(d_ptr->tabs.at(index).action);
        }
    } else if (armed >= 0 && index == armed && d_ptr->tabs.at(index).action->isEnabled()) {
        setCurrentIndex(index);
        d_ptr->focused = index;
    }
    update();
}

void FluentTabView::mouseMoveEvent(QMouseEvent *event)
{
    const QPointF pos = eventPos(event);
    const int hovered = d_ptr->tabAt(pos);
    const bool close = hovered >= 0 && d_ptr->onClose(pos);
    if (hovered != d_ptr->hovered || close != d_ptr->closeHovered) {
        d_ptr->hovered = hovered;
        d_ptr->closeHovered = close;
        update();
    }
}

void FluentTabView::leaveEvent(QEvent *event)
{
    Q_UNUSED(event)
    d_ptr->hovered = -1;
    d_ptr->closeHovered = false;
    update();
}

void FluentTabView::keyPressEvent(QKeyEvent *event)
{
    const int count = d_ptr->tabs.count();
    if (count == 0) {
        QWidget::keyPressEvent(event);
        return;
    }
    if (d_ptr->focused < 0) {
        d_ptr->focused = 0;
    }
    switch (event->key()) {
    case Qt::Key_Right:
        d_ptr->focused = qMin(d_ptr->focused + 1, count - 1);
        setCurrentIndex(d_ptr->focused);
        event->accept();
        return;
    case Qt::Key_Left:
        d_ptr->focused = qMax(d_ptr->focused - 1, 0);
        setCurrentIndex(d_ptr->focused);
        event->accept();
        return;
    default:
        break;
    }
    QxControls::QxControl::keyPressEvent(event);
}

QAccessible::Role FluentTabView::accessibleRole() const
{
    // Several things open, one showing: what an assistive tool walks is the
    // row, and it is announced as a page tab list.
    return QAccessible::PageTabList;
}

QX_FLUENT_END_NAMESPACE
