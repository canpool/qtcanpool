/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "fluentnavigationview.h"

#include "fluenttokens.h"

#include "qxcontrols/qxstatelayer.h"

#include <QtCore/QCoreApplication>
#include <QtGui/QFontMetrics>
#include <QtGui/QGuiApplication>
#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>
#include <QtGui/QResizeEvent>
#include <QtGui/QScreen>
#include <QtGui/QWheelEvent>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
// QAction is in Widgets on Qt 5 and in Gui on Qt 6, so it is asked for without
// naming a module - the same way the rest of the libraries ask.
#include <QAction>

#include <functional>

QX_FLUENT_BEGIN_NAMESPACE

namespace
{
// Fluent's navigation measures: a pane of three hundred and twenty with its
// words, forty-eight without them, a menu button and destinations forty tall,
// an icon of sixteen, and a bar three wide marking the one in force.
constexpr qreal kOpenPaneWidth = 320.0;
constexpr qreal kCompactPaneWidth = 48.0;
constexpr qreal kMenuButtonHeight = 40.0;
constexpr qreal kItemHeight = 40.0;
constexpr qreal kItemPadding = 16.0;
constexpr qreal kIconSize = 16.0;
constexpr qreal kIconGap = 12.0;
constexpr qreal kSelectionWidth = 3.0;
constexpr qreal kSelectionHeight = 16.0;
constexpr qreal kHamburgerGlyph = 16.0;
constexpr qreal kHamburgerStep = 5.0;

// A heading over a group of destinations: its own height, the small print of
// the ramp in the heavier cut, and a rule of its own width when the pane is
// folded. The height is the same folded as open, so that folding does not
// move the destinations under it.
constexpr qreal kHeaderHeight = 36.0;
constexpr qreal kFoldedHeaderWidth = 20.0;
constexpr qreal kHeaderOpacity = 0.6;

// A destination under another one steps in by this much, and the arrow that
// opens it sits at its trailing edge. A parent that is carrying the
// destination in force somewhere under it is marked - quietly, because the
// mark is a hint about a place that is not on the pane.
constexpr qreal kIndent = 24.0;
constexpr qreal kChevronSize = 9.0;
constexpr qreal kChevronStroke = 1.4;
constexpr qreal kChildSelectedBarOpacity = 0.45;

// A folded pane opens what is under an icon beside itself: a panel of its own,
// with the pane's rows at the pane's height and the pane's indentation, and a
// hairline edge because a panel over the page needs one to be a panel.
constexpr int kFlyoutWidth = 240;
constexpr qreal kFlyoutPadding = 6.0;
constexpr qreal kFlyoutRadius = 8.0;
constexpr qreal kFlyoutBorderOpacity = 0.14;
constexpr qreal kFlyoutHoverOpacity = 0.06;

// The mark of the destination in force is the accent, both as the bar and as a
// wash under the whole row; a destination nobody can use is the same drawing
// thinned. Neither moves the geometry.
constexpr qreal kSelectedOpacity = 0.08;
constexpr qreal kDisabledOpacity = 0.4;
constexpr qreal kDividerOpacity = 0.10;

// What itemAt() answers for the menu button, which is a place on the pane no
// destination owns.
constexpr int kMenuIndex = -2;

// The pane scrolls when its destinations do not fit, and what it scrolls with
// is the overlay bar a Fluent pane wears: three wide, two off the divider, no
// track under it, and no longer than the part of the list that is in sight.
constexpr qreal kScrollBarWidth = 3.0;
constexpr qreal kScrollBarMargin = 2.0;
constexpr qreal kScrollBarMinLength = 24.0;
constexpr qreal kScrollBarOpacity = 0.3;
constexpr qreal kScrollBarActiveOpacity = 0.55;
//! How far to the side of the bar a press still counts as grabbing it: three
//! wide is a target to look at, not one to hit.
constexpr qreal kScrollBarReach = 3.0;

QPointF eventPos(const QMouseEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return event->position();
#else
    return event->localPos();
#endif
}

/*! Where on the screen \a event landed: the panel has to be judged against the
 *  window it hangs beside, and a position in any one widget says nothing about
 *  the rest of the screen. */
QPoint eventScreenPos(const QMouseEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return event->globalPosition().toPoint();
#else
    return event->globalPos();
#endif
}

/*! The three bars that fold and unfold the pane: drawn rather than typed,
 *  because a glyph in a font is a glyph the theme cannot colour and an
 *  assistive tool would read. */
void paintHamburger(QPainter *painter, const QRectF &rect, const QColor &color)
{
    const qreal left = rect.center().x() - kHamburgerGlyph / 2.0;
    const qreal middle = rect.center().y();
    painter->setPen(QPen(color, 1.5, Qt::SolidLine, Qt::RoundCap));
    for (int step = -1; step <= 1; ++step) {
        const qreal y = middle + step * kHamburgerStep;
        painter->drawLine(QLineF(left, y, left + kHamburgerGlyph, y));
    }
}
/*! The arrow at the trailing edge of a parent, pointing the way its group
 *  will go: down while the group is open, right while it is closed. */
void paintChevron(QPainter *painter, const QRectF &row, bool expanded, const QColor &color)
{
    const QRectF arrow(row.right() - kItemPadding - kChevronSize, row.center().y() - kChevronSize / 2.0, kChevronSize,
                       kChevronSize);
    const qreal third = kChevronSize / 3.0;
    QPainterPath path;
    if (expanded) {
        path.moveTo(arrow.left(), arrow.center().y() - third);
        path.lineTo(arrow.center().x(), arrow.center().y() + third);
        path.lineTo(arrow.right(), arrow.center().y() - third);
    } else {
        path.moveTo(arrow.center().x() - third, arrow.top());
        path.lineTo(arrow.center().x() + third, arrow.center().y());
        path.lineTo(arrow.center().x() - third, arrow.bottom());
    }
    painter->setPen(QPen(color, kChevronStroke, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(path);
}
}   // namespace

/*! One line of the pane. A line either holds a destination or is a heading
 *  over the destinations that follow it, and which of the two it is is what
 *  the null action says: a heading has nothing to trigger and nothing to put
 *  in force, so it has no action at all rather than one that does nothing.
 *
 *  A destination may stand under another one, which is the view's second
 *  shape: the parent keeps how deep it is and whether what stands under it is
 *  showing, and the rows are kept in the order they are drawn in, so a child
 *  comes after its parent and before whatever follows the parent. */
/*! The destinations under one destination, shown beside a folded pane.
 *
 *  A pane forty-eight wide is its icons; what stands under an icon has no
 *  room there, and a destination that could not be reached at all would be a
 *  destination the fold lost. So it steps out instead, into a panel whose rows
 *  are the pane's rows and whose indentation is the pane's indentation - the
 *  same list, in a window over the page.
 *
 *  It is a popup rather than a child widget: it stands over the window, and a
 *  press anywhere else puts it away, which is already what Qt::Popup means.
 *  That is also why it is not drawn with a shadow: a popup window is the one
 *  surface the window manager may already be casting one for, and laying a
 *  second under it in a panel this narrow reads as a smudge.
 */
class NavigationFlyout : public QWidget
{
public:
    NavigationFlyout(QWidget *owner, const QColor &face, const QColor &ink, const QColor &accent)
        // The frame is not the panel's: it is what keeps the panel's own
        // window from being translucent on Windows at all. A frameless window
        // is a layered one there, and a layered one is drawn over the desktop
        // rather than over a background the system fills in - which is why the
        // four corners this panel leaves unpainted came out black.
        : QWidget(owner, Qt::Popup | Qt::FramelessWindowHint)
        , m_face(face)
        , m_ink(ink)
        , m_accent(accent)
    {
        // The panel turns its corners, and a window that is not translucent
        // has square ones whatever is painted in them.
        setAttribute(Qt::WA_TranslucentBackground);
        setMouseTracking(true);
        setFocusPolicy(Qt::StrongFocus);
        // A panel is put away by a press anywhere else, and the popup flag is
        // what normally arranges that: Qt grabs the mouse for a popup and ends
        // it on the first press outside. A window that moves itself by its own
        // caption can take that grab back, and then a press elsewhere reaches
        // the application without the panel ever being asked. So the panel asks
        // as well, from the one place every dispatched event passes through.
        if (QCoreApplication *app = QCoreApplication::instance()) {
            app->installEventFilter(this);
        }
    }

    ~NavigationFlyout() override
    {
        if (QCoreApplication *app = QCoreApplication::instance()) {
            app->removeEventFilter(this);
        }
    }

    void setEntries(const QList<QAction *> &actions, const QList<int> &depths, QAction *current)
    {
        m_actions = actions;
        m_depths = depths;
        m_current = current;
        m_hovered = -1;
        m_armed = -1;
        resize(kFlyoutWidth, int(kFlyoutPadding * 2.0 + m_actions.count() * kItemHeight));
        update();
    }

    /*! What a press on a row means, told to whoever put the panel up. */
    std::function<void(QAction *)> chosen;
protected:
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        // Only a press can end the panel, and only one that landed somewhere
        // else: a press on a row is the press the rows below are waiting for.
        // The panel also ends when it stops being the window in front, because
        // a panel over a window nobody is looking at is a panel left behind.
        if (event->type() == QEvent::MouseButtonPress) {
            if (!frameGeometry().contains(eventScreenPos(static_cast<QMouseEvent *>(event)))) {
                close();
            }
        } else if (event->type() == QEvent::WindowDeactivate && watched == this) {
            close();
        }
        return QWidget::eventFilter(watched, event);
    }

    void paintEvent(QPaintEvent *event) override
    {
        Q_UNUSED(event)
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        const QRectF panel = QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5);
        QColor edge = m_ink;
        edge.setAlphaF(kFlyoutBorderOpacity);
        painter.setPen(QPen(edge, 1.0));
        painter.setBrush(m_face);
        painter.drawRoundedRect(panel, kFlyoutRadius, kFlyoutRadius);

        const QFont font = FluentType::font(FluentType::Ramp::Body);
        const QFontMetrics metrics(font);
        painter.setFont(font);

        for (int i = 0; i < m_actions.count(); ++i) {
            const QAction *action = m_actions.at(i);
            const QRectF row = rowRect(i);
            if (i == m_hovered) {
                QColor wash = m_ink;
                wash.setAlphaF(kFlyoutHoverOpacity);
                painter.setPen(Qt::NoPen);
                painter.setBrush(wash);
                painter.drawRoundedRect(row, 4.0, 4.0);
            }
            if (action == m_current) {
                const QRectF bar(row.left() + 4.0, row.center().y() - kSelectionHeight / 2.0, kSelectionWidth,
                                 kSelectionHeight);
                painter.setPen(Qt::NoPen);
                painter.setBrush(m_accent);
                painter.drawRoundedRect(bar, kSelectionWidth / 2.0, kSelectionWidth / 2.0);
            }
            const bool usable = action->isEnabled();
            painter.setOpacity(usable ? 1.0 : kDisabledOpacity);
            const qreal left = row.left() + kItemPadding + m_depths.value(i) * kIndent;
            const QRectF iconRect(left, row.center().y() - kIconSize / 2.0, kIconSize, kIconSize);
            if (!action->icon().isNull()) {
                action->icon().paint(&painter, iconRect.toRect(), Qt::AlignCenter,
                                     usable ? QIcon::Normal : QIcon::Disabled);
            }
            const qreal room = row.right() - iconRect.right() - kIconGap - kItemPadding;
            if (room > 0.0) {
                const QString text = metrics.elidedText(action->text(), Qt::ElideRight, int(room));
                painter.setPen(m_ink);
                painter.drawText(QRectF(iconRect.right() + kIconGap, row.top(), room, row.height()),
                                 Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine, text);
            }
            painter.setOpacity(1.0);
        }
    }

    void mouseMoveEvent(QMouseEvent *event) override
    {
        const int row = rowAt(eventPos(event));
        if (row != m_hovered) {
            m_hovered = row;
            update();
        }
    }

    void mousePressEvent(QMouseEvent *event) override
    {
        m_armed = rowAt(eventPos(event));
        update();
    }

    void mouseReleaseEvent(QMouseEvent *event) override
    {
        const int armed = m_armed;
        m_armed = -1;
        if (armed >= 0 && armed == rowAt(eventPos(event)) && m_actions.at(armed)->isEnabled()) {
            QAction *action = m_actions.at(armed);
            // The callback outlives this object: the panel goes first, because
            // what the application does with the destination may take the pane
            // apart.
            const std::function<void(QAction *)> callback = chosen;
            close();
            if (callback) {
                callback(action);
            }
            return;
        }
        update();
    }

    void leaveEvent(QEvent *event) override
    {
        Q_UNUSED(event)
        m_hovered = -1;
        update();
    }

    void keyPressEvent(QKeyEvent *event) override
    {
        if (m_actions.isEmpty()) {
            QWidget::keyPressEvent(event);
            return;
        }
        switch (event->key()) {
        case Qt::Key_Down:
            m_hovered = qMin(m_hovered + 1, m_actions.count() - 1);
            update();
            event->accept();
            return;
        case Qt::Key_Up:
            m_hovered = qMax(m_hovered - 1, 0);
            update();
            event->accept();
            return;
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Space:
            if (m_hovered >= 0 && m_actions.at(m_hovered)->isEnabled()) {
                QAction *action = m_actions.at(m_hovered);
                const std::function<void(QAction *)> callback = chosen;
                close();
                if (callback) {
                    callback(action);
                }
            }
            event->accept();
            return;
        case Qt::Key_Escape:
            close();
            event->accept();
            return;
        default:
            break;
        }
        QWidget::keyPressEvent(event);
    }
private:
    QRectF rowRect(int row) const
    {
        return QRectF(kFlyoutPadding, kFlyoutPadding + row * kItemHeight, width() - 2 * kFlyoutPadding, kItemHeight);
    }

    int rowAt(const QPointF &pos) const
    {
        for (int row = 0; row < m_actions.count(); ++row) {
            if (rowRect(row).contains(pos)) {
                return row;
            }
        }
        return -1;
    }

    QList<QAction *> m_actions;
    QList<int> m_depths;
    QColor m_face;
    QColor m_ink;
    QColor m_accent;
    int m_hovered = -1;
    int m_armed = -1;
    QAction *m_current = Q_NULLPTR;
};

struct NavigationRow {
    QAction *action = Q_NULLPTR;
    QAction *parent = Q_NULLPTR;
    QString heading;
    QRectF rect;
    int depth = 0;
    bool expanded = true;

    bool isHeading() const
    {
        return action == Q_NULLPTR;
    }
};

class FluentNavigationViewPrivate
{
public:
    QX_DECLARE_PUBLIC(FluentNavigationView)

    /*! The pane, top to bottom. Headings are in here because they take room
     *  on it; they are not in any list of destinations. */
    QList<NavigationRow> rows;
    int hovered = -1;
    int armed = -1;
    int current = -1;
    int focused = -1;
    bool paneOpen = true;
    bool menuHovered = false;
    QWidget *content = Q_NULLPTR;
    /*! How far up the destinations have been scrolled, in pixels. The menu
     *  button does not move: it is the pane's header, not its first row. */
    qreal offset = 0.0;
    bool draggingBar = false;
    qreal dragOrigin = 0.0;
    QPointF dragPos;

    qreal paneWidth() const;
    QRectF menuButtonRect() const;
    qreal viewportTop() const;
    qreal viewportHeight() const;
    qreal contentHeight() const;
    qreal maximumOffset() const;
    qreal scrollBarLength() const;
    QRectF scrollBarRect() const;
    void setOffset(qreal value);
    void ensureVisible(int index);
    void relayout();
    int itemAt(const QPointF &pos) const;
    void activate(int index);

    /*! The rows between the menu button and the foot of the pane, counted as
     *  destinations: a heading is room on the pane and not a place in this
     *  count, which is the count currentIndex() speaks in. Destinations
     *  folded away are still counted - an index is a name, not a place on the
     *  pane, and one that moved when a group opened would be a different
     *  destination. */
    int destinationCount() const;
    int rowOfDestination(int destination) const;
    int destinationOfRow(int row) const;
    int rowOfAction(QAction *action) const;
    static qreal rowHeight(const NavigationRow &row);

    /*! True while every destination above \a row is open, which is when the
     *  row is on the pane at all. A folded pane shows the top level only. */
    bool isRowShowing(int row) const;
    int nextShowing(int destination, int step) const;
    bool hasChildren(QAction *action) const;
    bool childrenOf(QAction *action, QList<QAction *> *out) const;
    bool hasSelectedDescendant(QAction *action) const;
    bool isUnder(int row, QAction *ancestor) const;
    void setExpandedRow(int row, bool expanded);
    void expandTo(int destination);
    void appendFlyoutRows(QAction *parent, int depth, QList<QAction *> *actions, QList<int> *depths) const;
    QAction *parentOf(QAction *action) const;
    int depthOf(QAction *action) const;
};

qreal FluentNavigationViewPrivate::paneWidth() const
{
    return paneOpen ? kOpenPaneWidth : kCompactPaneWidth;
}

QRectF FluentNavigationViewPrivate::menuButtonRect() const
{
    return QRectF(0.0, 0.0, paneWidth(), kMenuButtonHeight);
}

qreal FluentNavigationViewPrivate::viewportTop() const
{
    return kMenuButtonHeight;
}

qreal FluentNavigationViewPrivate::viewportHeight() const
{
    Q_Q(const FluentNavigationView);
    return qMax(0.0, qreal(q->height()) - kMenuButtonHeight);
}

qreal FluentNavigationViewPrivate::rowHeight(const NavigationRow &row)
{
    return row.isHeading() ? kHeaderHeight : kItemHeight;
}

int FluentNavigationViewPrivate::destinationCount() const
{
    int count = 0;
    for (const NavigationRow &row : rows) {
        if (!row.isHeading()) {
            ++count;
        }
    }
    return count;
}

int FluentNavigationViewPrivate::rowOfDestination(int destination) const
{
    if (destination < 0) {
        return -1;
    }
    int seen = 0;
    for (int row = 0; row < rows.count(); ++row) {
        if (rows.at(row).isHeading()) {
            continue;
        }
        if (seen == destination) {
            return row;
        }
        ++seen;
    }
    return -1;
}

int FluentNavigationViewPrivate::destinationOfRow(int row) const
{
    if (row < 0 || row >= rows.count() || rows.at(row).isHeading()) {
        return -1;
    }
    int seen = 0;
    for (int each = 0; each < row; ++each) {
        if (!rows.at(each).isHeading()) {
            ++seen;
        }
    }
    return seen;
}

int FluentNavigationViewPrivate::rowOfAction(QAction *action) const
{
    if (action == Q_NULLPTR) {
        return -1;
    }
    for (int row = 0; row < rows.count(); ++row) {
        if (rows.at(row).action == action) {
            return row;
        }
    }
    return -1;
}

QAction *FluentNavigationViewPrivate::parentOf(QAction *action) const
{
    const int row = rowOfAction(action);
    return row < 0 ? Q_NULLPTR : rows.at(row).parent;
}

int FluentNavigationViewPrivate::depthOf(QAction *action) const
{
    // A destination cannot be its own ancestor - addItem() refuses a parent
    // that stands under the one being added - but a chain that walked itself
    // would hang the pane rather than draw it, so it is capped.
    int depth = 0;
    for (QAction *up = parentOf(action); up != Q_NULLPTR; up = parentOf(up)) {
        if (++depth >= 16) {
            break;
        }
    }
    return depth;
}

bool FluentNavigationViewPrivate::isRowShowing(int row) const
{
    if (row < 0 || row >= rows.count()) {
        return false;
    }
    const NavigationRow &line = rows.at(row);
    // A heading, and every destination of the top level, is the pane itself.
    if (line.isHeading() || line.parent == Q_NULLPTR) {
        return true;
    }
    // Folded, the pane is its top level and nothing else: what stands under a
    // destination has no room beside an icon, and comes out as a flyout
    // instead.
    if (!paneOpen) {
        return false;
    }
    // Open, a destination shows while everything above it is open.
    for (QAction *up = line.parent; up != Q_NULLPTR; up = parentOf(up)) {
        const int parentRow = rowOfAction(up);
        if (parentRow < 0 || !rows.at(parentRow).expanded) {
            return false;
        }
    }
    return true;
}

int FluentNavigationViewPrivate::nextShowing(int destination, int step) const
{
    // An index folded away is still a destination, and the arrows walk the
    // destinations that are on the pane: stepping over a closed group is what
    // makes Down mean "the next thing I can see".
    for (int each = destination + step; each >= 0 && each < destinationCount(); each += step) {
        const int row = rowOfDestination(each);
        if (row >= 0 && isRowShowing(row)) {
            return each;
        }
    }
    return destination;
}

bool FluentNavigationViewPrivate::childrenOf(QAction *action, QList<QAction *> *out) const
{
    if (action == Q_NULLPTR) {
        return false;
    }
    bool found = false;
    for (const NavigationRow &row : rows) {
        if (row.isHeading() || row.parent != action) {
            continue;
        }
        found = true;
        if (out != Q_NULLPTR) {
            out->append(row.action);
        } else {
            return true;
        }
    }
    return found;
}

bool FluentNavigationViewPrivate::hasChildren(QAction *action) const
{
    return childrenOf(action, Q_NULLPTR);
}

bool FluentNavigationViewPrivate::hasSelectedDescendant(QAction *action) const
{
    const int row = rowOfDestination(current);
    if (row < 0 || action == Q_NULLPTR) {
        return false;
    }
    for (QAction *up = rows.at(row).parent; up != Q_NULLPTR; up = parentOf(up)) {
        if (up == action) {
            return true;
        }
    }
    return false;
}

bool FluentNavigationViewPrivate::isUnder(int row, QAction *ancestor) const
{
    if (row < 0 || row >= rows.count() || ancestor == Q_NULLPTR) {
        return false;
    }
    for (QAction *up = rows.at(row).parent; up != Q_NULLPTR; up = parentOf(up)) {
        if (up == ancestor) {
            return true;
        }
    }
    return false;
}

void FluentNavigationViewPrivate::expandTo(int destination)
{
    // Naming a destination that is folded away is asking to be shown it: the
    // groups above it open, rather than the destination quietly staying out of
    // sight while currentIndex() says it is in force.
    const int row = rowOfDestination(destination);
    if (row < 0) {
        return;
    }
    for (QAction *up = rows.at(row).parent; up != Q_NULLPTR; up = parentOf(up)) {
        const int parentRow = rowOfAction(up);
        if (parentRow >= 0 && !rows.at(parentRow).expanded) {
            setExpandedRow(parentRow, true);
        }
    }
}

void FluentNavigationViewPrivate::appendFlyoutRows(QAction *parent, int depth, QList<QAction *> *actions,
                                                   QList<int> *depths) const
{
    for (const NavigationRow &row : rows) {
        if (row.isHeading() || row.parent != parent) {
            continue;
        }
        actions->append(row.action);
        depths->append(depth);
        // What is under a destination comes out with it, at its own depth: a
        // panel that showed one level and hid the rest would be a fold inside
        // a fold, and a press further in than that is a press nobody expects
        // to make. A closed group inside it stays closed, because the panel
        // says what the pane says.
        if (row.expanded) {
            appendFlyoutRows(row.action, depth + 1, actions, depths);
        }
    }
}

void FluentNavigationViewPrivate::setExpandedRow(int row, bool expanded)
{
    Q_Q(FluentNavigationView);
    if (row < 0 || row >= rows.count() || rows.at(row).isHeading()) {
        return;
    }
    NavigationRow &line = rows[row];
    // Nothing under it means nothing to open: a destination that answers
    // "expanded" while showing nothing would be a state the pane cannot tell
    // from a closed one.
    if (line.expanded == expanded || !hasChildren(line.action)) {
        return;
    }
    line.expanded = expanded;
    // The room the pane needs just changed, and so did where every row under
    // this one stands.
    relayout();
    // The keyboard cannot stay on a destination that was folded away: it goes
    // up to the group that closed over it, which is where the eye goes too.
    const int focusedRow = rowOfDestination(focused);
    if (focusedRow >= 0 && !isRowShowing(focusedRow)) {
        focused = destinationOfRow(row);
    }
    q->updateGeometry();
    q->update();
    Q_EMIT q->expandedChanged(line.action, expanded);
}

qreal FluentNavigationViewPrivate::contentHeight() const
{
    qreal height = 0.0;
    for (int row = 0; row < rows.count(); ++row) {
        if (isRowShowing(row)) {
            height += rowHeight(rows.at(row));
        }
    }
    return height;
}

qreal FluentNavigationViewPrivate::maximumOffset() const
{
    return qMax(0.0, contentHeight() - viewportHeight());
}

qreal FluentNavigationViewPrivate::scrollBarLength() const
{
    const qreal view = viewportHeight();
    const qreal content = contentHeight();
    // As much of the bar as the list has in sight, and never so little that
    // there is nothing left to take hold of.
    return content > view ? qMax(kScrollBarMinLength, view * view / content) : view;
}

QRectF FluentNavigationViewPrivate::scrollBarRect() const
{
    const qreal view = viewportHeight();
    const qreal content = contentHeight();
    if (view <= 0.0 || content <= view) {
        return QRectF();
    }
    const qreal length = scrollBarLength();
    const qreal travel = view - length;
    const qreal span = content - view;
    const qreal y = kMenuButtonHeight + (span > 0.0 ? offset / span * travel : 0.0);
    return QRectF(paneWidth() - kScrollBarMargin - kScrollBarWidth, y, kScrollBarWidth, length);
}

void FluentNavigationViewPrivate::setOffset(qreal value)
{
    Q_Q(FluentNavigationView);
    const qreal wanted = qBound(0.0, value, maximumOffset());
    if (qFuzzyIsNull(wanted - offset)) {
        return;
    }
    offset = wanted;
    relayout();
    q->update();
}

void FluentNavigationViewPrivate::ensureVisible(int index)
{
    const qreal view = viewportHeight();
    // A view that has not been given its height yet has no room to be visible
    // in, and an application that names a destination in its constructor does so
    // before the first layout. Answering with a scroll then would be answering a
    // question nobody asked - and the answer would be wrong: the list would be
    // left scrolled by one row once the height did arrive, with the destination
    // it was scrolled for half out of sight above the fold.
    const int row = rowOfDestination(index);
    if (view <= 0.0 || row < 0 || !isRowShowing(row)) {
        return;
    }
    // A destination reached by the keyboard is one the user has to be able to
    // see: the row it is in is scrolled into the room below the menu button,
    // and the list never turns further than it has to. A heading above it is
    // not scrolled for - the destination is what was named - but it comes into
    // sight when it can, which is what scrolling this far up does.
    qreal top = 0.0;
    for (int each = 0; each < row; ++each) {
        if (isRowShowing(each)) {
            top += rowHeight(rows.at(each));
        }
    }
    const qreal bottom = top + rowHeight(rows.at(row));
    if (top < offset) {
        setOffset(top);
    } else if (bottom > offset + view) {
        setOffset(bottom - view);
    }
}

void FluentNavigationViewPrivate::relayout()
{
    Q_Q(FluentNavigationView);
    const qreal width = paneWidth();
    // The scroll is clamped here rather than only where it is moved: a
    // destination leaving, a pane unfolding and a view growing all change how
    // far there is left to scroll, and none of them is the wheel.
    offset = qBound(0.0, offset, maximumOffset());
    qreal top = kMenuButtonHeight - offset;
    for (int row = 0; row < rows.count(); ++row) {
        NavigationRow &line = rows[row];
        // How far in a destination stands is a question about the rows above
        // it, so it is answered here, where the pane is worked out - and not
        // kept as a number that an application moving a destination would
        // have to keep in step itself.
        line.depth = line.isHeading() ? 0 : depthOf(line.action);
        if (!isRowShowing(row)) {
            // A row folded away has no room on the pane: an empty rectangle is
            // a row nothing can be drawn on and nothing can be aimed at.
            line.rect = QRectF();
            continue;
        }
        line.rect = QRectF(0.0, top, width, rowHeight(line));
        top += rowHeight(line);
    }
    if (content) {
        const int pane = int(width);
        content->setGeometry(pane, 0, qMax(0, q->width() - pane), q->height());
    }
}

int FluentNavigationViewPrivate::itemAt(const QPointF &pos) const
{
    // The pane may have been resized since it was last asked; where its rows
    // stand is answered now, because a hit test against stale geometry is a
    // lie - and a view that has never been shown is never told it was resized.
    const_cast<FluentNavigationViewPrivate *>(this)->relayout();
    // The menu button is not a row, and a row scrolled half under it is not
    // reachable through the part that is hidden.
    if (pos.y() < viewportTop()) {
        return -1;
    }
    for (int row = 0; row < rows.count(); ++row) {
        if (rows.at(row).rect.contains(pos)) {
            // A heading is room the pointer may pass over; it is not a place
            // the pointer can be on, so it answers the same as the gaps do.
            return destinationOfRow(row);
        }
    }
    return -1;
}

void FluentNavigationViewPrivate::activate(int index)
{
    Q_Q(FluentNavigationView);
    const int row = rowOfDestination(index);
    if (row < 0) {
        return;
    }
    const NavigationRow &line = rows.at(row);
    if (!line.action->isEnabled()) {
        return;
    }
    // Pressing a parent opens or closes what is under it, and is itself a
    // journey rather than only a fold: a parent is a destination too, which is
    // what WinUI's SelectsOnInvoked says and what an application that wanted a
    // group to be only a group can say by not connecting to its action.
    if (hasChildren(line.action)) {
        setExpandedRow(row, !line.expanded);
    }
    q->setCurrentIndex(index);
    line.action->trigger();
}

FluentNavigationView::FluentNavigationView(QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new FluentNavigationViewPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    // Which destination the pointer is on is part of what the pane says, so
    // moves are wanted without a button - the same reason a rating wants them.
    setMouseTracking(true);
}

FluentNavigationView::~FluentNavigationView(){QX_FINI_PRIVATE()}

QAction *FluentNavigationView::addItem(const QString &text, const QIcon &icon)
{
    return addItem(text, icon, Q_NULLPTR);
}

QAction *FluentNavigationView::addItem(const QString &text, const QIcon &icon, QAction *parent)
{
    QAction *action = new QAction(icon, text, this);
    NavigationRow row;
    row.action = action;
    // A parent this view does not hold is no parent: a destination under a
    // destination that is not on the pane could never be reached, so it goes
    // to the top level instead of into a group nobody can open.
    row.parent = d_ptr->rowOfAction(parent) >= 0 ? parent : Q_NULLPTR;
    if (row.parent == Q_NULLPTR) {
        d_ptr->rows.append(row);
    } else {
        // Under its parent and after everything already under it, which is
        // where a list of children appends.
        const int parentRow = d_ptr->rowOfAction(row.parent);
        int insertAt = parentRow + 1;
        while (insertAt < d_ptr->rows.count() && d_ptr->isUnder(insertAt, row.parent)) {
            ++insertAt;
        }
        d_ptr->rows.insert(insertAt, row);
    }
    if (d_ptr->focused < 0) {
        d_ptr->focused = 0;
    }
    d_ptr->relayout();
    updateGeometry();
    update();
    return action;
}

void FluentNavigationView::addHeader(const QString &text)
{
    NavigationRow row;
    row.heading = text;
    d_ptr->rows.append(row);
    d_ptr->relayout();
    updateGeometry();
    update();
}

void FluentNavigationView::removeItem(QAction *action)
{
    for (int row = 0; row < d_ptr->rows.count(); ++row) {
        if (d_ptr->rows.at(row).action != action) {
            continue;
        }
        d_ptr->rows.removeAt(row);
        // The destination in force may be the one leaving, and an index is a
        // position in a list that just got shorter: what was said of the one
        // that left is said of its neighbour now, or of nothing.
        const int count = d_ptr->destinationCount();
        if (d_ptr->current >= count) {
            d_ptr->current = count == 0 ? -1 : count - 1;
        }
        if (d_ptr->focused >= count) {
            d_ptr->focused = count == 0 ? -1 : count - 1;
        }
        d_ptr->hovered = -1;
        delete action;
        d_ptr->relayout();
        updateGeometry();
        update();
        return;
    }
}

void FluentNavigationView::clear()
{
    if (d_ptr->rows.isEmpty()) {
        return;
    }
    const QList<NavigationRow> rows = d_ptr->rows;
    d_ptr->rows.clear();
    d_ptr->current = -1;
    d_ptr->focused = -1;
    d_ptr->hovered = -1;
    for (const NavigationRow &row : rows) {
        delete row.action;
    }
    d_ptr->relayout();
    updateGeometry();
    update();
}

QList<QAction *> FluentNavigationView::items() const
{
    QList<QAction *> actions;
    actions.reserve(d_ptr->rows.count());
    for (const NavigationRow &row : d_ptr->rows) {
        if (!row.isHeading()) {
            actions.append(row.action);
        }
    }
    return actions;
}

QAction *FluentNavigationView::currentItem() const
{
    const int row = d_ptr->rowOfDestination(d_ptr->current);
    if (row < 0) {
        return Q_NULLPTR;
    }
    return d_ptr->rows.at(row).action;
}

int FluentNavigationView::currentIndex() const
{
    return d_ptr->current;
}

QAction *FluentNavigationView::parentItem(QAction *item) const
{
    const int row = d_ptr->rowOfAction(item);
    return row < 0 ? Q_NULLPTR : d_ptr->rows.at(row).parent;
}

bool FluentNavigationView::isExpanded(QAction *item) const
{
    const int row = d_ptr->rowOfAction(item);
    if (row < 0 || !d_ptr->hasChildren(item)) {
        return false;
    }
    return d_ptr->rows.at(row).expanded;
}

void FluentNavigationView::setExpanded(QAction *item, bool expanded)
{
    d_ptr->setExpandedRow(d_ptr->rowOfAction(item), expanded);
}

void FluentNavigationView::showFlyout(QAction *parent, const QRectF &anchor)
{
    // One at a time: while a panel is up, the window is grabbed and a press
    // that is not on it belongs to it - which Qt has already arranged.
    if (QWidget *open = QApplication::activePopupWidget()) {
        open->close();
    }
    QList<QAction *> actions;
    QList<int> depths;
    d_ptr->appendFlyoutRows(parent, 0, &actions, &depths);
    if (actions.isEmpty()) {
        return;
    }

    auto *flyout = new NavigationFlyout(this, palette().color(QPalette::Window), textColor(), accentColor());
    flyout->setAttribute(Qt::WA_DeleteOnClose);
    flyout->setEntries(actions, depths, currentItem());
    flyout->chosen = [this](QAction *action) {
        // The panel has already gone; the pane takes it from here, the same
        // way a press on the pane itself would.
        setCurrentItem(action);
        if (action != Q_NULLPTR) {
            action->trigger();
        }
    };

    // Beside the icon it belongs to, and kept on the screen it opened on: a
    // panel half off the edge is one whose last rows cannot be pressed.
    const QPoint spot = mapToGlobal(QPoint(int(anchor.right()) + 2, int(anchor.top())));
    flyout->move(spot);
    if (const QScreen *screen = QGuiApplication::screenAt(spot)) {
        const QRect room = screen->availableGeometry();
        QPoint at = spot;
        if (at.x() + flyout->width() > room.right() + 1) {
            at.setX(room.right() + 1 - flyout->width());
        }
        if (at.y() + flyout->height() > room.bottom() + 1) {
            at.setY(room.bottom() + 1 - flyout->height());
        }
        flyout->move(at);
    }
    flyout->show();
    flyout->setFocus();
}

void FluentNavigationView::setContent(QWidget *widget)
{
    if (d_ptr->content == widget) {
        return;
    }
    if (d_ptr->content) {
        d_ptr->content->setParent(Q_NULLPTR);
    }
    d_ptr->content = widget;
    if (widget) {
        widget->setParent(this);
    }
    d_ptr->relayout();
    update();
}

QWidget *FluentNavigationView::contentWidget() const
{
    return d_ptr->content;
}

bool FluentNavigationView::isPaneOpen() const
{
    return d_ptr->paneOpen;
}

void FluentNavigationView::setPaneOpen(bool open)
{
    if (d_ptr->paneOpen == open) {
        return;
    }
    d_ptr->paneOpen = open;
    d_ptr->relayout();
    updateGeometry();
    update();
    Q_EMIT paneOpenChanged(open);
}

void FluentNavigationView::setCurrentIndex(int index)
{
    if (d_ptr->current == index) {
        return;
    }
    if (index >= 0 && index >= d_ptr->destinationCount()) {
        return;
    }
    // Naming a destination that is folded away opens the groups above it: the
    // destination then goes into force on a pane that has stopped moving,
    // which is the pane ensureVisible() has to scroll.
    d_ptr->expandTo(index);
    d_ptr->current = index;
    // A destination the application selects is one it means the user to look
    // at, even when the list is longer than the pane.
    d_ptr->ensureVisible(index);
    update();
    Q_EMIT currentIndexChanged(index);
    Q_EMIT currentItemChanged(currentItem());
}

void FluentNavigationView::setCurrentItem(QAction *action)
{
    const QList<QAction *> destinations = items();
    for (int i = 0; i < destinations.count(); ++i) {
        if (destinations.at(i) == action) {
            setCurrentIndex(i);
            return;
        }
    }
}

QSize FluentNavigationView::sizeHint() const
{
    return QSize(int(kOpenPaneWidth) + 400, int(kMenuButtonHeight) + 8 * int(kItemHeight));
}

QSize FluentNavigationView::minimumSizeHint() const
{
    return QSize(int(kCompactPaneWidth) + 80, int(kMenuButtonHeight) + int(kItemHeight));
}

QRectF FluentNavigationView::chromeRect() const
{
    // Nothing about this view is the control: it is a column of destinations
    // and a room beside them, and a wash round the whole of it would be a wash
    // round the content as well. The destinations draw their own.
    return QRectF();
}

void FluentNavigationView::paintSurface(QPainter *painter)
{
    // The pane is a layer of its own: the destinations stand on it, and it is
    // told from the content beside it by that and by the line between them.
    const qreal pane = d_ptr->paneWidth();
    painter->fillRect(QRectF(0.0, 0.0, pane, qreal(height())), palette().color(QPalette::AlternateBase));
    QColor divider = textColor();
    divider.setAlphaF(kDividerOpacity);
    painter->setPen(divider);
    painter->drawLine(QLineF(pane - 0.5, 0.0, pane - 0.5, qreal(height())));
}

void FluentNavigationView::paintContent(QPainter *painter)
{
    const bool enabled = isEnabled();
    if (d_ptr->menuHovered) {
        QxControls::QxStateLayer::paint(painter, d_ptr->menuButtonRect(), textColor(),
                                        QxControls::QxStateLayer::hoverOpacity);
    }
    painter->setOpacity(enabled ? 1.0 : kDisabledOpacity);
    paintHamburger(painter, d_ptr->menuButtonRect(), textColor());

    const QFont font = FluentType::font(FluentType::Ramp::Body);
    const QFont headerFont = FluentType::font(FluentType::Ramp::Caption, true);
    const QFontMetrics metrics(font);
    const QFontMetrics headerMetrics(headerFont);
    const bool withWords = d_ptr->paneOpen;
    const QColor accent = accentColor();
    // A heading is quieter than the destinations it heads: the same ink, thinned.
    QColor headerInk = textColor();
    headerInk.setAlphaF(kHeaderOpacity);

    // The destinations live under the menu button, not beside it: the first row
    // of a scrolled list is half above the fold, and half of it drawn over the
    // header would be a second header.
    const QRectF viewport(0.0, d_ptr->viewportTop(), d_ptr->paneWidth(), d_ptr->viewportHeight());
    painter->save();
    painter->setClipRect(viewport);

    for (int row = 0; row < d_ptr->rows.count(); ++row) {
        const NavigationRow &line = d_ptr->rows.at(row);
        if (!line.rect.intersects(viewport)) {
            continue;
        }
        painter->setOpacity(enabled ? 1.0 : kDisabledOpacity);
        if (line.isHeading()) {
            // Open, a heading is its words; folded, there is no room for words
            // and it is a rule. Both take the same height, so nothing under it
            // moves when the pane folds.
            if (withWords) {
                painter->setFont(headerFont);
                painter->setPen(headerInk);
                const int room = int(line.rect.width()) - 2 * int(kItemPadding);
                const QString text =
                    room > 0 ? headerMetrics.elidedText(line.heading, Qt::ElideRight, room) : QString();
                if (!text.isEmpty()) {
                    painter->drawText(line.rect.adjusted(kItemPadding, 0.0, -kItemPadding, 0.0),
                                      Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine, text);
                }
            } else {
                QColor rule = textColor();
                rule.setAlphaF(kDividerOpacity);
                const qreal middle = line.rect.center().y();
                painter->setPen(QPen(rule, 1.0));
                painter->drawLine(QLineF(line.rect.center().x() - kFoldedHeaderWidth / 2.0, middle,
                                         line.rect.center().x() + kFoldedHeaderWidth / 2.0, middle));
            }
            continue;
        }
        const int i = d_ptr->destinationOfRow(row);
        const bool usable = line.action->isEnabled();
        painter->setOpacity(enabled ? (usable ? 1.0 : kDisabledOpacity) : kDisabledOpacity);

        if (i == d_ptr->current) {
            QxControls::QxStateLayer::paint(painter, line.rect, accent, kSelectedOpacity);
            // The bar at the leading edge: Fluent marks the destination in
            // force here rather than by filling the row, so the mark survives
            // at a glance and does not read as a pressed button.
            const QRectF bar(line.rect.left() + (kItemPadding - kSelectionWidth) / 2.0,
                             line.rect.center().y() - kSelectionHeight / 2.0, kSelectionWidth, kSelectionHeight);
            painter->setPen(Qt::NoPen);
            painter->setBrush(accent);
            painter->drawRoundedRect(bar, kSelectionWidth / 2.0, kSelectionWidth / 2.0);
        } else if (d_ptr->hasSelectedDescendant(line.action)) {
            // The destination in force is under this one and out of sight: the
            // parent says so with the bar and none of the wash, because it is
            // not the destination itself - WinUI's IsChildSelected.
            const QRectF hint(line.rect.left() + (kItemPadding - kSelectionWidth) / 2.0,
                              line.rect.center().y() - kSelectionHeight / 2.0, kSelectionWidth, kSelectionHeight);
            QColor faint = accent;
            faint.setAlphaF(kChildSelectedBarOpacity);
            painter->setPen(Qt::NoPen);
            painter->setBrush(faint);
            painter->drawRoundedRect(hint, kSelectionWidth / 2.0, kSelectionWidth / 2.0);
        }
        if (i == d_ptr->hovered) {
            QxControls::QxStateLayer::paint(painter, line.rect, textColor(), QxControls::QxStateLayer::hoverOpacity);
        }
        if (hasFocus() && i == d_ptr->focused) {
            QxControls::QxStateLayer::paint(painter, line.rect, textColor(), QxControls::QxStateLayer::focusOpacity);
        }

        // Folded, the pane is its icons and nothing else: the icon takes the
        // row's middle, and the words are not drawn at all. Open, a
        // destination under another one steps in by its depth, which is what
        // says where the group it belongs to ended.
        const bool parent = d_ptr->hasChildren(line.action);
        const qreal iconLeft = withWords ? kItemPadding + line.depth * kIndent : (line.rect.width() - kIconSize) / 2.0;
        const QRectF iconRect(line.rect.left() + iconLeft, line.rect.center().y() - kIconSize / 2.0, kIconSize,
                              kIconSize);
        if (!line.action->icon().isNull()) {
            line.action->icon().paint(painter, iconRect.toRect(), Qt::AlignCenter,
                                      usable ? QIcon::Normal : QIcon::Disabled);
        }

        if (withWords) {
            // The arrow that opens a group stands at the trailing edge, and the
            // words stop short of it: a destination whose name ran under the
            // arrow would be a name read at the cost of the way in.
            const qreal room = line.rect.right() - iconRect.right() - kIconGap - kItemPadding -
                               (parent ? kChevronSize + kIconGap : 0.0);
            const QString text =
                room > 0 ? metrics.elidedText(line.action->text(), Qt::ElideRight, int(room)) : QString();
            if (!text.isEmpty()) {
                painter->setFont(font);
                painter->setPen(textColor());
                painter->drawText(QRectF(iconRect.right() + kIconGap, line.rect.top(), room, line.rect.height()),
                                  Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine, text);
            }
            if (parent) {
                paintChevron(painter, line.rect, line.expanded, textColor());
            }
        }
    }
    painter->restore();
    painter->setOpacity(1.0);

    // The bar is drawn last and outside the clip: it stands against the pane's
    // own edge, which the rows stop short of.
    const QRectF thumb = d_ptr->scrollBarRect();
    if (!thumb.isEmpty()) {
        QColor ink = textColor();
        ink.setAlphaF(d_ptr->draggingBar || underMouse() ? kScrollBarActiveOpacity : kScrollBarOpacity);
        painter->setPen(Qt::NoPen);
        painter->setBrush(ink);
        painter->drawRoundedRect(thumb, kScrollBarWidth / 2.0, kScrollBarWidth / 2.0);
    }
}

void FluentNavigationView::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
    d_ptr->relayout();
    QxControls::QxControl::resizeEvent(event);
}

void FluentNavigationView::mousePressEvent(QMouseEvent *event)
{
    const QPointF pos = eventPos(event);
    // The bar is a handle and lies over the rows it scrolls, so it is grabbed
    // before any of them is looked at.
    const QRectF thumb = d_ptr->scrollBarRect();
    if (!thumb.isEmpty() && thumb.adjusted(-kScrollBarReach, 0.0, kScrollBarReach, 0.0).contains(pos)) {
        d_ptr->draggingBar = true;
        d_ptr->dragOrigin = d_ptr->offset;
        d_ptr->dragPos = pos;
        update();
        return;
    }
    if (d_ptr->menuButtonRect().contains(pos)) {
        d_ptr->armed = kMenuIndex;
    } else {
        d_ptr->armed = d_ptr->itemAt(pos);
    }
    update();
}

void FluentNavigationView::mouseReleaseEvent(QMouseEvent *event)
{
    const QPointF pos = eventPos(event);
    if (d_ptr->draggingBar) {
        d_ptr->draggingBar = false;
        update();
        return;
    }
    const int armed = d_ptr->armed;
    d_ptr->armed = -1;
    if (armed == kMenuIndex) {
        if (d_ptr->menuButtonRect().contains(pos)) {
            setPaneOpen(!d_ptr->paneOpen);
        }
    } else if (armed >= 0 && d_ptr->itemAt(pos) == armed) {
        const int row = d_ptr->rowOfDestination(armed);
        const NavigationRow &line = d_ptr->rows.at(row);
        if (!d_ptr->paneOpen && d_ptr->hasChildren(line.action)) {
            // Folded, a parent has no room to open in place: what stands under
            // it steps out beside the pane, and the press is a way in rather
            // than a journey. That is LeftCompact's own reading of a parent,
            // and the reason a fold on a hierarchical pane is not a dead end.
            d_ptr->focused = armed;
            showFlyout(line.action, line.rect);
        } else {
            d_ptr->activate(armed);
            d_ptr->focused = armed;
        }
    }
    update();
}

void FluentNavigationView::mouseMoveEvent(QMouseEvent *event)
{
    const QPointF pos = eventPos(event);
    if (d_ptr->draggingBar) {
        // What the hand moved over the bar is asked back in rows: the bar's
        // travel is shorter than the list's, and the ratio between them is what
        // makes a small drag move the list as far as the same drag moves the
        // bar.
        const qreal travel = d_ptr->viewportHeight() - d_ptr->scrollBarLength();
        if (travel > 0.0) {
            d_ptr->setOffset(d_ptr->dragOrigin + (pos.y() - d_ptr->dragPos.y()) * d_ptr->maximumOffset() / travel);
        }
        return;
    }
    const bool menu = d_ptr->menuButtonRect().contains(pos);
    const int hovered = menu ? -1 : d_ptr->itemAt(pos);
    if (menu != d_ptr->menuHovered || hovered != d_ptr->hovered) {
        d_ptr->menuHovered = menu;
        d_ptr->hovered = hovered;
        update();
    }
}

void FluentNavigationView::leaveEvent(QEvent *event)
{
    Q_UNUSED(event)
    d_ptr->hovered = -1;
    d_ptr->menuHovered = false;
    update();
}

void FluentNavigationView::keyPressEvent(QKeyEvent *event)
{
    // What the arrows walk is the destinations: a heading is passed over the
    // way the eye passes over it, which is what this count leaves out.
    const int count = d_ptr->destinationCount();
    if (count == 0) {
        QWidget::keyPressEvent(event);
        return;
    }
    if (d_ptr->focused < 0) {
        d_ptr->focused = 0;
    }
    switch (event->key()) {
    case Qt::Key_Down:
        d_ptr->focused = d_ptr->nextShowing(d_ptr->focused, 1);
        d_ptr->ensureVisible(d_ptr->focused);
        update();
        event->accept();
        return;
    case Qt::Key_Up:
        d_ptr->focused = d_ptr->nextShowing(d_ptr->focused, -1);
        d_ptr->ensureVisible(d_ptr->focused);
        update();
        event->accept();
        return;
    case Qt::Key_Right: {
        // Right opens a closed group, and walks into an open one: the same
        // press means "go on" whether the way on is an arrow or a step.
        const int row = d_ptr->rowOfDestination(d_ptr->focused);
        if (row < 0 || !d_ptr->hasChildren(d_ptr->rows.at(row).action)) {
            break;
        }
        if (d_ptr->rows.at(row).expanded) {
            d_ptr->focused = d_ptr->nextShowing(d_ptr->focused, 1);
        } else {
            d_ptr->setExpandedRow(row, true);
        }
        d_ptr->ensureVisible(d_ptr->focused);
        update();
        event->accept();
        return;
    }
    case Qt::Key_Left: {
        // And Left closes an open group, and walks back out of a closed one -
        // to the group it is in, which is where it came from.
        const int row = d_ptr->rowOfDestination(d_ptr->focused);
        if (row < 0) {
            break;
        }
        const NavigationRow &line = d_ptr->rows.at(row);
        if (d_ptr->hasChildren(line.action) && line.expanded) {
            d_ptr->setExpandedRow(row, false);
        } else if (line.parent != Q_NULLPTR) {
            d_ptr->focused = d_ptr->destinationOfRow(d_ptr->rowOfAction(line.parent));
        } else {
            break;
        }
        d_ptr->ensureVisible(d_ptr->focused);
        update();
        event->accept();
        return;
    }
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

void FluentNavigationView::wheelEvent(QWheelEvent *event)
{
    if (d_ptr->maximumOffset() <= 0.0) {
        QxControls::QxControl::wheelEvent(event);
        return;
    }
    // The wheel is asked what it moved rather than assumed to have notches in
    // it: a trackpad reports pixels, a wheel reports eighths of a turn, and a
    // notch here is the rows the platform's own setting asks for.
    const QPoint pixels = event->pixelDelta();
    qreal delta = 0.0;
    if (!pixels.isNull()) {
        delta = pixels.y();
    } else {
        const qreal notches = event->angleDelta().y() / 120.0;
        delta = notches * kItemHeight * QApplication::wheelScrollLines();
    }
    if (qFuzzyIsNull(delta)) {
        event->ignore();
        return;
    }
    d_ptr->setOffset(d_ptr->offset - delta);
    event->accept();
}

QAccessible::Role FluentNavigationView::accessibleRole() const
{
    // It is a list of destinations with a room beside them, and what an
    // assistive tool is meant to walk is the list.
    return QAccessible::List;
}

QX_FLUENT_END_NAMESPACE
