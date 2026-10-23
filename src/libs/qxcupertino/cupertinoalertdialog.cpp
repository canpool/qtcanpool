/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "cupertinoalertdialog.h"

#include "cupertinotokens.h"

#include "qxcontrols/qxshadowpainter.h"
#include "qxcontrols/qxstatelayer.h"
#include "qxtheme/qxthememanager.h"

#include <QtCore/QStringList>
#include <QtGui/QFontMetrics>
#include <QtGui/QKeyEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>

QX_CUPERTINO_BEGIN_NAMESPACE

namespace
{
/*! The card's width: a hand's width, and not the page's - an alert is read in
 *  one glance and a wider card would be a different thing. */
constexpr int kCardWidth = 270;
/*! The room the question keeps inside the card, above and below the words. */
constexpr int kCardPadding = 20;
/*! The gap between the title and the sentence under it, when there are two. */
constexpr int kTitleGap = 6;
/*! An answer's cell, in the row or in the stack. */
constexpr int kActionHeight = 44;
/*! The room the card's shadow needs round it, so that a dialog with no page
 *  under it is still the size of its card plus the light it casts. */
constexpr int kCardRoom = 40;
/*! At most this many answers: a card that stacked one more would be taller than
 *  the question it asks. */
constexpr int kMaximumActions = 3;
/*! The veil over the page. Dark enough that the page reads as set aside, thin
 *  enough that it is still recognisably there. */
constexpr qreal kScrimAlpha = 0.4;
/*! The hairlines: the one between the question and the answers, and the ones
 *  between the answers themselves. The same value the list rows use, because it
 *  is the same line seen from another control. */
constexpr qreal kLineOpacity = 0.2;
/*! A sentence under the title is the same ink thinned, so it reads as the
 *  second half of one statement rather than as another paragraph. */
constexpr qreal kMessageOpacity = 0.75;
/*! A disabled answer keeps its cell and loses its ink. */
constexpr qreal kDisabledOpacity = 0.4;

bool onDarkFace(const QWidget *widget)
{
    return widget->palette().color(QPalette::Window).lightness() < 128;
}

/*! Where a mouse event happened, in widget coordinates. Qt 6 renamed the
 *  accessor; the answer is the same one. */
QPointF eventPos(const QMouseEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return event->position();
#else
    return event->localPos();
#endif
}
}   // namespace

class CupertinoAlertDialogPrivate
{
public:
    QX_DECLARE_PUBLIC(CupertinoAlertDialog)

    QString title;
    QString message;
    QStringList texts;
    QList<CupertinoAlertDialog::ActionRole> roles;
    QList<QRect> rects;
    /*! The answer the pointer is over, and the one being pushed: -1 for none,
     *  which is every place in the card that is not an answer. */
    int hovered = -1;
    int pressed = -1;
    /*! True between a press on the veil and its release. */
    bool veilPressed = false;
    /*! True while the dialog is the size of the page it was opened over, which
     *  is when it has room to grow without asking for a new size. */
    bool coversPage = false;

    /*! The height of the strip the answers take: one cell across the card for
     *  one or two of them, one cell each stacked for three. */
    int actionBlockHeight() const
    {
        if (texts.isEmpty()) {
            return 0;
        }
        return texts.size() <= 2 ? kActionHeight : texts.size() * kActionHeight;
    }
    /*! The line between the question and the answers, and the top of the first
     *  answer: one pixel above the strip, so the strip starts under the line. */
    int actionBlockTop(const QRect &card) const
    {
        return card.bottom() + 1 - actionBlockHeight();
    }
    bool stacked() const
    {
        return texts.size() > 2;
    }
};

CupertinoAlertDialog::CupertinoAlertDialog(QWidget *parent)
    : QDialog(parent)
    , d_ptr(new CupertinoAlertDialogPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    // The alert paints its own surface, so Qt's frame would be a second one
    // drawn round it.
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground, false);
    setModal(true);
}

CupertinoAlertDialog::~CupertinoAlertDialog(){QX_FINI_PRIVATE()}

QString CupertinoAlertDialog::title() const
{
    return d_ptr->title;
}

QString CupertinoAlertDialog::message() const
{
    return d_ptr->message;
}

int CupertinoAlertDialog::maximumActionCount()
{
    return kMaximumActions;
}

bool CupertinoAlertDialog::addAction(const QString &text, ActionRole role)
{
    if (text.isEmpty() || d_ptr->texts.size() >= kMaximumActions) {
        return false;
    }
    d_ptr->texts.append(text);
    d_ptr->roles.append(role);
    relayout();
    return true;
}

int CupertinoAlertDialog::actionCount() const
{
    return d_ptr->texts.size();
}

QString CupertinoAlertDialog::actionText(int index) const
{
    if (index < 0 || index >= d_ptr->texts.size()) {
        return QString();
    }
    return d_ptr->texts.at(index);
}

CupertinoAlertDialog::ActionRole CupertinoAlertDialog::actionRole(int index) const
{
    if (index < 0 || index >= d_ptr->roles.size()) {
        return ActionRole::Default;
    }
    return d_ptr->roles.at(index);
}

QRect CupertinoAlertDialog::actionRect(int index) const
{
    if (index < 0 || index >= d_ptr->rects.size()) {
        return QRect();
    }
    return d_ptr->rects.at(index);
}

void CupertinoAlertDialog::setTitle(const QString &title)
{
    if (d_ptr->title == title) {
        return;
    }
    d_ptr->title = title;
    // The title is what the dialog is called: a reader that arrives at the
    // window before it reads anything inside it asks this.
    setAccessibleName(title);
    relayout();
}

void CupertinoAlertDialog::setMessage(const QString &message)
{
    if (d_ptr->message == message) {
        return;
    }
    d_ptr->message = message;
    relayout();
}

void CupertinoAlertDialog::clearActions()
{
    if (d_ptr->texts.isEmpty()) {
        return;
    }
    d_ptr->texts.clear();
    d_ptr->roles.clear();
    relayout();
}

int CupertinoAlertDialog::cardHeight() const
{
    const int textWidth = kCardWidth - 2 * kCardPadding;
    // The flags are an int in both Qt 5 and Qt 6: the two enumerations they come
    // from are not the same one.
    const int wrapping = int(Qt::AlignHCenter | Qt::TextWordWrap);
    int height = kCardPadding;
    if (!d_ptr->title.isEmpty()) {
        height += QFontMetrics(CupertinoType::font(CupertinoType::Ramp::Body, true))
                      .boundingRect(QRect(0, 0, textWidth, 0), wrapping, d_ptr->title)
                      .height();
    }
    if (!d_ptr->message.isEmpty()) {
        if (!d_ptr->title.isEmpty()) {
            height += kTitleGap;
        }
        height += QFontMetrics(CupertinoType::font(CupertinoType::Ramp::Body))
                      .boundingRect(QRect(0, 0, textWidth, 0), wrapping, d_ptr->message)
                      .height();
    }
    height += kCardPadding;
    if (!d_ptr->texts.isEmpty()) {
        // The hairline is a pixel of the card too.
        height += 1 + d_ptr->actionBlockHeight();
    }
    return height;
}

QRect CupertinoAlertDialog::cardRect() const
{
    const int room = 2 * kCardRoom;
    const int available = qMax(0, height() - room);
    const int here = qBound(0, cardHeight(), available);
    return QRect(QPoint((width() - kCardWidth) / 2, (height() - here) / 2), QSize(kCardWidth, here));
}

QRect CupertinoAlertDialog::textRect() const
{
    const QRect card = cardRect();
    const int textWidth = kCardWidth - 2 * kCardPadding;
    const int bottom = d_ptr->texts.isEmpty() ? card.bottom() + 1 : d_ptr->actionBlockTop(card);
    return QRect(card.left() + kCardPadding, card.top() + kCardPadding, textWidth,
                 qMax(0, bottom - card.top() - 2 * kCardPadding));
}

QSize CupertinoAlertDialog::sizeHint() const
{
    return QSize(kCardWidth + 2 * kCardRoom, cardHeight() + 2 * kCardRoom);
}

void CupertinoAlertDialog::relayout()
{
    // A dialog that is not standing on a page is as big as its card plus the
    // light it casts, and every question or answer added changes that. Without
    // this the card would be clamped to the room a dialog sized before the
    // answers existed - the sentence under the title was the first thing to go.
    if (!d_ptr->coversPage && size() != sizeHint()) {
        resize(sizeHint());
    }

    const QRect card = cardRect();
    d_ptr->rects.clear();
    if (!d_ptr->texts.isEmpty()) {
        const int top = d_ptr->actionBlockTop(card);
        const int count = d_ptr->texts.size();
        if (d_ptr->stacked()) {
            // Three answers are three cells down the card: a row of three
            // words in a hand's width would be a row of abbreviations.
            for (int i = 0; i < count; ++i) {
                d_ptr->rects.append(QRect(card.left(), top + i * kActionHeight, card.width(), kActionHeight));
            }
        } else {
            // One is the card; two are halves of it, split evenly and with the
            // remainder on the first so the line between them is on a pixel.
            const int half = card.width() / count;
            for (int i = 0; i < count; ++i) {
                const int left = card.left() + i * half;
                const int width = i == count - 1 ? card.right() + 1 - left : half;
                d_ptr->rects.append(QRect(left, top, width, kActionHeight));
            }
        }
    }
    update();
}

int CupertinoAlertDialog::actionAt(const QPoint &pos) const
{
    for (int i = 0; i < d_ptr->rects.size(); ++i) {
        if (d_ptr->rects.at(i).contains(pos)) {
            return i;
        }
    }
    return -1;
}

void CupertinoAlertDialog::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // The page, and the veil over it. Both are painted by the dialog because it
    // covers the page it was opened over - a scrim as a separate widget would
    // be a second window and a second way for the two to disagree.
    painter.fillRect(rect(), palette().color(QPalette::Window));
    QColor veil(Qt::black);
    veil.setAlphaF(kScrimAlpha);
    painter.fillRect(rect(), veil);

    const QRectF card(cardRect());
    const qreal radius = CupertinoShape::radius(CupertinoShape::Radius::XLarge);
    const QxTheme::QxShadow shadow = CupertinoElevation::shadow(CupertinoElevation::Level::Sheet);
    if (shadow.alpha > 0.0 && shadow.blurRadius > 0.0) {
        QxControls::QxShadowPainter::paint(&painter, card, radius, palette().color(QPalette::Shadow), shadow);
    }
    painter.setPen(Qt::NoPen);
    painter.setBrush(palette().color(QPalette::Base));
    painter.drawRoundedRect(card, radius, radius);

    const QColor ink = palette().color(QPalette::WindowText);
    const int wrapping = int(Qt::AlignHCenter | Qt::TextWordWrap | Qt::AlignTop);

    // The question: the title in the heavier cut of the body, the sentence under
    // it in the body thinned, both wrapped to the card and centred.
    QRect text = textRect();
    if (!d_ptr->title.isEmpty()) {
        painter.setFont(CupertinoType::font(CupertinoType::Ramp::Body, true));
        painter.setPen(ink);
        const int height =
            QFontMetrics(painter.font()).boundingRect(QRect(0, 0, text.width(), 0), wrapping, d_ptr->title).height();
        painter.drawText(QRect(text.left(), text.top(), text.width(), height), wrapping, d_ptr->title);
        text.setTop(text.top() + height + kTitleGap);
    }
    if (!d_ptr->message.isEmpty()) {
        painter.setFont(CupertinoType::font(CupertinoType::Ramp::Body));
        QColor sentence = ink;
        sentence.setAlphaF(kMessageOpacity);
        painter.setPen(sentence);
        painter.drawText(text, wrapping, d_ptr->message);
    }

    if (d_ptr->texts.isEmpty()) {
        return;
    }

    // The hairline that says the answers are not part of the sentence, and then
    // one between every two answers - the only thing that says where one ends.
    QColor line = CupertinoColor::color(CupertinoColor::Role::Gray, onDarkFace(this));
    line.setAlphaF(kLineOpacity);
    const QRect cardPixels = cardRect();
    const int blockTop = d_ptr->actionBlockTop(cardPixels);
    painter.setPen(QPen(line, 1));
    painter.drawLine(QPointF(cardPixels.left(), blockTop - 0.5), QPointF(cardPixels.right() + 1, blockTop - 0.5));
    for (int i = 1; i < d_ptr->rects.size(); ++i) {
        const QRect current = d_ptr->rects.at(i);
        if (d_ptr->stacked()) {
            painter.drawLine(QPointF(cardPixels.left(), current.top() - 0.5),
                             QPointF(cardPixels.right() + 1, current.top() - 0.5));
        } else {
            painter.drawLine(QPointF(current.left() - 0.5, current.top()),
                             QPointF(current.left() - 0.5, current.bottom()));
        }
    }

    // The answers themselves: a cell each, in the family's ink for what the
    // answer means. The pointer's wash is the shared state layer, so an answer
    // reads the same under the pointer as a row of a list does.
    for (int i = 0; i < d_ptr->rects.size(); ++i) {
        const QRectF cell(d_ptr->rects.at(i));
        if (isEnabled() && (d_ptr->hovered == i || d_ptr->pressed == i)) {
            namespace StateLayer = QxControls::QxStateLayer;
            const qreal opacity = d_ptr->pressed == i ? StateLayer::pressedOpacity : StateLayer::hoverOpacity;
            painter.save();
            painter.setClipRect(cardPixels);
            StateLayer::paint(&painter, cell, ink, opacity);
            painter.restore();
        }

        QColor answerInk;
        switch (d_ptr->roles.at(i)) {
        case ActionRole::Destructive:
            answerInk = CupertinoColor::color(CupertinoColor::Role::Red, onDarkFace(this));
            break;
        default:
            answerInk = QxTheme::QxThemeManager::instance()->accentColor();
            break;
        }
        if (!isEnabled()) {
            answerInk.setAlphaF(kDisabledOpacity);
        }
        painter.setFont(CupertinoType::font(CupertinoType::Ramp::Body, d_ptr->roles.at(i) == ActionRole::Cancel));
        painter.setPen(answerInk);
        painter.drawText(
            cell, Qt::AlignCenter,
            QFontMetrics(painter.font()).elidedText(d_ptr->texts.at(i), Qt::ElideRight, int(cell.width()) - 8));
    }
}

void CupertinoAlertDialog::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        QDialog::mousePressEvent(event);
        return;
    }
    const QPointF pos = eventPos(event);
    const int action = actionAt(pos.toPoint());
    d_ptr->pressed = action;
    // A press on the veil is the beginning of "put it away", and the release is
    // what decides whether that is what happened.
    d_ptr->veilPressed = action < 0 && !cardRect().contains(pos.toPoint());
    update();
}

void CupertinoAlertDialog::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        QDialog::mouseReleaseEvent(event);
        return;
    }
    const QPointF pos = eventPos(event);
    const int pressed = d_ptr->pressed;
    const bool veil = d_ptr->veilPressed;
    d_ptr->pressed = -1;
    d_ptr->veilPressed = false;
    update();

    // A press is answered by where it started: one that began on an answer and
    // was released somewhere else has not answered anything.
    if (pressed >= 0 && actionAt(pos.toPoint()) == pressed) {
        Q_EMIT actionChosen(pressed);
        return;
    }
    if (veil && !cardRect().contains(pos.toPoint())) {
        reject();
        return;
    }
}

void CupertinoAlertDialog::mouseMoveEvent(QMouseEvent *event)
{
    const int hovered = actionAt(eventPos(event).toPoint());
    if (hovered != d_ptr->hovered) {
        d_ptr->hovered = hovered;
        update();
    }
    QDialog::mouseMoveEvent(event);
}

void CupertinoAlertDialog::leaveEvent(QEvent *event)
{
    if (d_ptr->hovered != -1) {
        d_ptr->hovered = -1;
        update();
    }
    QDialog::leaveEvent(event);
}

void CupertinoAlertDialog::keyPressEvent(QKeyEvent *event)
{
    const int key = event->key();
    // Return answers the question the alert is asking - the first answer that is
    // the affirmative - and Escape is the way out, which is QDialog's own.
    if ((key == Qt::Key_Return || key == Qt::Key_Enter) && isEnabled()) {
        for (int i = 0; i < d_ptr->roles.size(); ++i) {
            if (d_ptr->roles.at(i) == ActionRole::Default) {
                Q_EMIT actionChosen(i);
                return;
            }
        }
    }
    QDialog::keyPressEvent(event);
}

void CupertinoAlertDialog::changeEvent(QEvent *event)
{
    QDialog::changeEvent(event);
    const QEvent::Type type = event->type();
    // The card is measured from the font, so a change of font or of palette
    // changes how tall it is.
    if (type == QEvent::FontChange || type == QEvent::PaletteChange || type == QEvent::StyleChange) {
        relayout();
    }
}

void CupertinoAlertDialog::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);
    relayout();
}

void CupertinoAlertDialog::showEvent(QShowEvent *event)
{
    coverThePage();
    QDialog::showEvent(event);
    relayout();
}

void CupertinoAlertDialog::coverThePage()
{
    QWidget *owner = parentWidget() ? parentWidget()->window() : Q_NULLPTR;
    if (!owner || owner == this || !owner->isVisible()) {
        // Nothing to dim: the dialog stands at its own size, and the veil it
        // paints is then the card's own room and nothing else.
        d_ptr->coversPage = false;
        resize(sizeHint());
        return;
    }
    // The client area of the window it was opened over, so that the veil covers
    // the page rather than a part of it.
    d_ptr->coversPage = true;
    setGeometry(owner->geometry());
}

QX_CUPERTINO_END_NAMESPACE
