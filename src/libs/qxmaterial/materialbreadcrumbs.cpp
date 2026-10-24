/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "materialbreadcrumbs.h"

#include "materialtokens.h"

#include "qxcontrols/qxcontrol.h"

#include <QtCore/QVector>
#include <QtGui/QFontMetrics>
#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>
#include <QtGui/QResizeEvent>
#include <QtWidgets/QSizePolicy>

QX_MATERIAL_BEGIN_NAMESPACE

namespace
{
// The room a step takes round its words, and the room either side of the
// separator - Material writes the path as a line of text with the slash given
// its own space, which is also what keeps two steps from touching.
constexpr int kStepPadding = 8;
constexpr int kSeparatorGap = 8;

// The mark that says steps were left out: three dots, a little apart.
constexpr int kDots = 3;
constexpr qreal kDotSize = 3.0;
constexpr qreal kDotPitch = 7.0;

// Where MUI puts the fold and what it keeps either side of it.
constexpr int kDefaultMaxItems = 8;
constexpr int kDefaultItemsBeforeCollapse = 1;
constexpr int kDefaultItemsAfterCollapse = 1;

// The separator is the page's ink at the strength Material gives a secondary
// label: it is punctuation in the path, not a step of it.
constexpr qreal kSeparatorOpacity = 0.6;

// A disabled bar is read but not aimed at.
constexpr qreal kDisabledOpacity = 0.38;

/*! The place of the fold in a run of steps. Negative, because it stands where a
 *  step would and is not one. */
constexpr int kMoreMark = -1;
}   // namespace

class MaterialBreadcrumbsPrivate;
class Step;

/*!
 * One place in the row: a step of the path, or the mark that says some of it is
 * folded away.
 *
 * A place is a control of its own so that the pointer arriving, the press, the
 * focus ring and the role are answered for it - a link is a link, and the bar
 * is a list of them. What a place does not answer is where it stands: the bar
 * gives it a rectangle, and the bar folds and unrolls.
 */
class Step : public QxControls::QxControl
{
public:
    explicit Step(MaterialBreadcrumbsPrivate *owner);

    /*! Stand for \a index, or for the fold when \a index is \c kMoreMark. */
    void setStep(int index, bool more);

    int stepIndex() const
    {
        return m_index;
    }
    bool isMark() const
    {
        return m_more;
    }
    /*! The step the user is on: it is where they already are, so it is not a
     *  link and nothing about it is a move. */
    bool isCurrent() const;

    qreal cornerRadius() const override;
    QRectF chromeRect() const override;
    bool hitTest(const QPointF &pos) const override;

    QAccessible::Role accessibleRole() const override;

    /*! Answer the press: go back to the step, or unroll the path if this place
     *  is the mark. Public because the place hands its press back to itself,
     *  and a place is the bar's own business rather than the application's. */
    void take();
protected:
    void paintContent(QPainter *painter) override;

    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
private:
    MaterialBreadcrumbsPrivate *m_owner;
    int m_index = -1;
    bool m_more = false;
};

class MaterialBreadcrumbsPrivate
{
public:
    QX_DECLARE_PUBLIC(MaterialBreadcrumbs)

    QStringList items;
    QString expandText;
    int maxItems = kDefaultMaxItems;
    int before = kDefaultItemsBeforeCollapse;
    int after = kDefaultItemsAfterCollapse;
    bool expanded = false;
    /*! The places, in reading order. Grown as needed and never shrunk: a place
     *  that is not shown is hidden, so a fold or an unrolling cannot delete the
     *  place whose press is being answered. */
    QVector<Step *> pool;
    /*! How many of the pool are on show. */
    int shown = 0;

    QFont font() const;
    int separatorWidth() const;
    /*! The width place \a index asks for - \c kMoreMark for the mark. */
    int stepWidth(int index) const;
    /*! The steps on show, in reading order, with \c kMoreMark where the fold
     *  stands. */
    QVector<int> visible() const;

    Step *stepFor(int index) const;
    void take(int index);
    void unroll();

    void rebuild();
    void arrange();
    void relayout();
};

Step::Step(MaterialBreadcrumbsPrivate *owner)
    : QxControls::QxControl(owner->q_func())
    , m_owner(owner)
{
    show();
}

void Step::setStep(int index, bool more)
{
    m_index = index;
    m_more = more;
    // The keyboard stops on the ways back and on the fold - not on where the
    // user is, which is not a destination.
    setFocusPolicy(isCurrent() ? Qt::NoFocus : Qt::TabFocus);
    setAccessibleName(m_more ? m_owner->expandText : m_owner->items.at(index));
    update();
}

bool Step::isCurrent() const
{
    return !m_more && m_index >= 0 && m_index == m_owner->items.count() - 1;
}

qreal Step::cornerRadius() const
{
    return MaterialShape::radius(MaterialShape::Radius::Small);
}

QRectF Step::chromeRect() const
{
    // Where the user is has no wash and no ring, because there is nothing to
    // answer there: the ink alone says where they are.
    return isCurrent() ? QRectF() : QRectF(rect());
}

bool Step::hitTest(const QPointF &pos) const
{
    // A press that lands on where the user already is has not moved them; it
    // is left to the widget underneath rather than caught and dropped here.
    return !isCurrent() && QxControls::QxControl::hitTest(pos);
}

QAccessible::Role Step::accessibleRole() const
{
    if (m_more) {
        // It is a thing to press, and what it does is unroll the path.
        return QAccessible::Button;
    }
    // A way back is a link; where the user is is a piece of text saying so.
    return isCurrent() ? QAccessible::StaticText : QAccessible::Link;
}

void Step::paintContent(QPainter *painter)
{
    QColor ink = isCurrent() ? textColor() : accentColor();
    if (!isEnabled()) {
        ink.setAlphaF(ink.alphaF() * kDisabledOpacity);
    }
    painter->save();
    painter->setFont(m_owner->font());
    if (m_more) {
        painter->setPen(Qt::NoPen);
        painter->setBrush(ink);
        const qreal tall = height() / 2.0;
        const qreal left = (width() - ((kDots - 1) * kDotPitch + kDotSize)) / 2.0;
        for (int dot = 0; dot < kDots; ++dot) {
            const QPointF middle(left + dot * kDotPitch + kDotSize / 2.0, tall);
            painter->drawEllipse(middle, kDotSize / 2.0, kDotSize / 2.0);
        }
    } else {
        painter->setPen(ink);
        painter->drawText(QRectF(rect()).adjusted(kStepPadding, 0, -kStepPadding, 0), Qt::AlignLeft | Qt::AlignVCenter,
                          m_owner->items.at(m_index));
    }
    painter->restore();
}

void Step::keyPressEvent(QKeyEvent *event)
{
    const int key = event->key();
    if (key == Qt::Key_Enter || key == Qt::Key_Return) {
        take();
        event->accept();
        return;
    }
    if (key == Qt::Key_Space) {
        event->accept();
        return;
    }
    QxControls::QxControl::keyPressEvent(event);
}

void Step::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space && !event->isAutoRepeat()) {
        take();
        event->accept();
        return;
    }
    QxControls::QxControl::keyReleaseEvent(event);
}

void Step::take()
{
    if (m_more) {
        m_owner->unroll();
        return;
    }
    m_owner->take(m_index);
}

QFont MaterialBreadcrumbsPrivate::font() const
{
    return MaterialType::font(MaterialType::Scale::Body);
}

int MaterialBreadcrumbsPrivate::separatorWidth() const
{
    return QFontMetrics(font()).horizontalAdvance(QStringLiteral("/")) + 2 * kSeparatorGap;
}

int MaterialBreadcrumbsPrivate::stepWidth(int index) const
{
    if (index == kMoreMark) {
        return int((kDots - 1) * kDotPitch + kDotSize) + 2 * kStepPadding;
    }
    if (index < 0 || index >= items.count()) {
        return 0;
    }
    return QFontMetrics(font()).horizontalAdvance(items.at(index)) + 2 * kStepPadding;
}

QVector<int> MaterialBreadcrumbsPrivate::visible() const
{
    QVector<int> steps;
    steps.reserve(items.count());
    for (int i = 0; i < items.count(); ++i) {
        steps.append(i);
    }
    if (expanded || maxItems <= 0 || items.count() <= maxItems) {
        return steps;
    }
    // The head of the path and its tail, and the fold between them. Where the
    // user is belongs to the tail and is never folded away, so the tail is read
    // as at least one step.
    const int head = qMax(0, before);
    const int tail = qMax(1, after);
    if (head + tail >= items.count()) {
        // A fold that would leave nothing out is not a fold.
        return steps;
    }
    steps.clear();
    for (int i = 0; i < head; ++i) {
        steps.append(i);
    }
    steps.append(kMoreMark);
    for (int i = items.count() - tail; i < items.count(); ++i) {
        steps.append(i);
    }
    return steps;
}

Step *MaterialBreadcrumbsPrivate::stepFor(int index) const
{
    const QVector<int> steps = visible();
    for (int i = 0; i < qMin(steps.count(), pool.count()); ++i) {
        if (steps.at(i) == index) {
            return pool.at(i);
        }
    }
    return Q_NULLPTR;
}

void MaterialBreadcrumbsPrivate::take(int index)
{
    if (index < 0 || index >= items.count() - 1) {
        return;
    }
    Q_EMIT q_ptr->itemClicked(index, items.at(index));
}

void MaterialBreadcrumbsPrivate::unroll()
{
    if (expanded) {
        return;
    }
    const int was = shown;
    // A keyboard user pressing the fold is asking for the steps it hid; the
    // focus lands on the first of them rather than being left on a mark that is
    // about to go away. A pointer user's focus is wherever it was.
    const bool walked = q_ptr->focusWidget() != Q_NULLPTR;
    expanded = true;
    rebuild();
    if (walked) {
        Step *step = stepFor(qMax(0, before));
        if (step) {
            step->setFocus();
        }
    }
    if (shown != was) {
        Q_EMIT q_ptr->expanded();
    }
}

void MaterialBreadcrumbsPrivate::rebuild()
{
    const QVector<int> steps = visible();
    while (pool.count() < steps.count()) {
        Step *step = new Step(this);
        // A place hands back to itself rather than to its index: by the time
        // the press is answered the index may have been folded away.
        QObject::connect(step, &QxControls::QxControl::clicked, q_ptr, [step]() {
            step->take();
        });
        pool.append(step);
    }
    for (int i = 0; i < pool.count(); ++i) {
        Step *step = pool.at(i);
        if (i < steps.count()) {
            const int index = steps.at(i);
            step->setStep(index, index == kMoreMark);
            step->show();
        } else {
            step->hide();
        }
    }
    shown = steps.count();
    arrange();
    q_ptr->updateGeometry();
    q_ptr->update();
    // What an assistive tool reads is the path itself, in one sentence: the
    // same thing a sighted user reads off the row.
    q_ptr->setAccessibleName(items.join(QStringLiteral(" / ")));
}

void MaterialBreadcrumbsPrivate::arrange()
{
    const int tall = q_ptr->height();
    int left = 0;
    for (int i = 0; i < shown; ++i) {
        Step *step = pool.at(i);
        const int wide = stepWidth(step->stepIndex());
        step->setGeometry(left, 0, wide, tall);
        left += wide + separatorWidth();
    }
}

void MaterialBreadcrumbsPrivate::relayout()
{
    rebuild();
}

MaterialBreadcrumbs::MaterialBreadcrumbs(QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new MaterialBreadcrumbsPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    // The row is not a control of its own: nothing about the bar is something
    // the pointer lands on, and the keyboard stops on the steps.
    setFocusPolicy(Qt::NoFocus);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    d_ptr->expandText = QStringLiteral("Show path");
    d_ptr->rebuild();
}

MaterialBreadcrumbs::~MaterialBreadcrumbs()
{
    QX_FINI_PRIVATE();
}

QStringList MaterialBreadcrumbs::items() const
{
    return d_ptr->items;
}

int MaterialBreadcrumbs::maxItems() const
{
    return d_ptr->maxItems;
}

int MaterialBreadcrumbs::itemsBeforeCollapse() const
{
    return d_ptr->before;
}

int MaterialBreadcrumbs::itemsAfterCollapse() const
{
    return d_ptr->after;
}

QString MaterialBreadcrumbs::expandText() const
{
    return d_ptr->expandText;
}

bool MaterialBreadcrumbs::isExpanded() const
{
    return d_ptr->expanded;
}

QSize MaterialBreadcrumbs::sizeHint() const
{
    const QFontMetrics metrics(d_ptr->font());
    const int tall = metrics.height() + 2 * kStepPadding;
    const QVector<int> steps = d_ptr->visible();
    if (steps.isEmpty()) {
        return QSize(0, tall);
    }
    int wide = 0;
    for (int i = 0; i < steps.count(); ++i) {
        wide += d_ptr->stepWidth(steps.at(i));
    }
    wide += (steps.count() - 1) * d_ptr->separatorWidth();
    return QSize(wide, tall);
}

QSize MaterialBreadcrumbs::minimumSizeHint() const
{
    // A step is not shortened to fit: the bar asks for the width its path needs,
    // and a path too long for its room is what the fold is for.
    return sizeHint();
}

void MaterialBreadcrumbs::setItems(const QStringList &items)
{
    if (d_ptr->items == items) {
        return;
    }
    d_ptr->items = items;
    // A new path is not the old one: it is shown folded, whichever state the
    // old one was left in.
    d_ptr->expanded = false;
    d_ptr->rebuild();
}

void MaterialBreadcrumbs::setMaxItems(int count)
{
    if (d_ptr->maxItems == count) {
        return;
    }
    d_ptr->maxItems = count;
    d_ptr->rebuild();
}

void MaterialBreadcrumbs::setItemsBeforeCollapse(int count)
{
    const int wanted = qMax(0, count);
    if (d_ptr->before == wanted) {
        return;
    }
    d_ptr->before = wanted;
    d_ptr->rebuild();
}

void MaterialBreadcrumbs::setItemsAfterCollapse(int count)
{
    const int wanted = qMax(0, count);
    if (d_ptr->after == wanted) {
        return;
    }
    d_ptr->after = wanted;
    d_ptr->rebuild();
}

void MaterialBreadcrumbs::setExpandText(const QString &text)
{
    if (d_ptr->expandText == text) {
        return;
    }
    d_ptr->expandText = text;
    d_ptr->rebuild();
}

void MaterialBreadcrumbs::expand()
{
    d_ptr->unroll();
}

void MaterialBreadcrumbs::collapse()
{
    if (!d_ptr->expanded) {
        return;
    }
    // The keyboard may be on a step the fold takes away: it goes back to the
    // mark that stands where those steps were.
    const bool walked = focusWidget() != Q_NULLPTR;
    d_ptr->expanded = false;
    d_ptr->rebuild();
    if (walked) {
        Step *mark = d_ptr->stepFor(kMoreMark);
        if (mark) {
            mark->setFocus();
        }
    }
}

QRectF MaterialBreadcrumbs::chromeRect() const
{
    // The row itself is not something the pointer lands on.
    return QRectF();
}

void MaterialBreadcrumbs::paintContent(QPainter *painter)
{
    if (d_ptr->shown < 2) {
        return;
    }
    QColor ink = textColor();
    ink.setAlphaF(ink.alphaF() * kSeparatorOpacity);
    painter->save();
    painter->setFont(d_ptr->font());
    painter->setPen(ink);
    for (int i = 0; i + 1 < d_ptr->shown; ++i) {
        const QRect left = d_ptr->pool.at(i)->geometry();
        const QRect right = d_ptr->pool.at(i + 1)->geometry();
        // The separator is the room between two steps, drawn: it belongs to
        // neither of them, which is why it is not part of the words either.
        const QRectF room(left.right() + 1, 0, right.left() - left.right() - 1, height());
        painter->drawText(room, Qt::AlignCenter, QStringLiteral("/"));
    }
    painter->restore();
}

void MaterialBreadcrumbs::resizeEvent(QResizeEvent *event)
{
    QxControls::QxControl::resizeEvent(event);
    d_ptr->arrange();
}

void MaterialBreadcrumbs::changeEvent(QEvent *event)
{
    QxControls::QxControl::changeEvent(event);
    if (event->type() == QEvent::FontChange || event->type() == QEvent::PaletteChange) {
        // The steps are as wide as their words, so a new font is a new row.
        d_ptr->relayout();
    }
}

QAccessible::Role MaterialBreadcrumbs::accessibleRole() const
{
    // An assistive tool walks the path as a list of links.
    return QAccessible::List;
}

QX_MATERIAL_END_NAMESPACE
