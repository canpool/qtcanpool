/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "materialdialog.h"

#include "materialtokens.h"

#include "qxcontrols/qxshadowpainter.h"
#include "qxcontrols/qxstatelayer.h"
#include "qxtheme/qxthememanager.h"

#include <QtCore/QVariantAnimation>
#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <QtMath>

QX_MATERIAL_BEGIN_NAMESPACE

namespace
{
// The card in the middle: three hundred and twenty wide, twenty-four in from
// its own edge, sixteen between the things stacked in it, and a row of answers
// at the bottom that is a little taller than the text in it so that a press
// has somewhere to land.
constexpr int kCardWidth = 320;
constexpr int kCardPadding = 24;
constexpr int kGap = 16;
constexpr int kAnswerHeight = 36;
constexpr int kAnswerPadding = 12;
constexpr int kAnswerMinWidth = 48;
constexpr int kAnswerGap = 8;

// The full-screen shape: a bar across the top that carries the name of the
// place and the way out of it, and the room the bar takes from what is under
// it. The cross is a touch target first and a glyph second.
constexpr int kBarHeight = 56;
constexpr int kBarInset = 16;
constexpr int kCrossSize = 48;
constexpr int kCrossGlyph = 18;

// M3's scrim: the page with the light taken out of it, and - once a surface is
// lifted this far - a little light mixed back into the surface, because on a
// dark theme a shadow under a dark page is not there at all.
constexpr qreal kScrimAlpha = 0.32;
constexpr qreal kSurfaceLift = 0.06;

// How far above the page the card sits: level 4 of the scale is the one the
// vocabulary names for a dialog - and the room its shadow needs is the room
// the card keeps off the edge of the page.
int cardRoom()
{
    const QxTheme::QxShadow shadow = MaterialElevation::shadow(MaterialElevation::Level::Level4);
    if (shadow.alpha <= 0 || shadow.blurRadius <= 0.0) {
        return 0;
    }
    return int(qCeil(shadow.blurRadius)) + 1 + int(qCeil(shadow.yOffset));
}

QPoint eventPos(const QMouseEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return event->position().toPoint();
#else
    return event->pos();
#endif
}
}   // namespace

class MaterialDialogPrivate
{
public:
    QX_DECLARE_PUBLIC(MaterialDialog)

    MaterialDialog::Variant variant = MaterialDialog::Variant::Basic;
    QWidget *card = Q_NULLPTR;
    QVBoxLayout *cardLayout = Q_NULLPTR;
    QLabel *titleLabel = Q_NULLPTR;
    QLabel *messageLabel = Q_NULLPTR;
    QWidget *content = Q_NULLPTR;

    QString answers[2];
    QRect answerRects[2];
    int hovered = -1;
    int pressed = -1;

    QRect crossRect;
    bool crossHover = false;
    bool crossPressed = false;

    QVariantAnimation entrance;
};

MaterialDialog::MaterialDialog(QWidget *parent)
    : QDialog(parent)
    , d_ptr(new MaterialDialogPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    // A Material dialog has no frame of the platform's: the surface is the
    // frame, and a title bar above it would be a second answer to what this is.
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    setWindowModality(Qt::WindowModal);
    // The answers are painted, so the pointer has to be watched to say where
    // it is over them.
    setMouseTracking(true);

    d_ptr->card = new QWidget(this);
    d_ptr->card->setFixedWidth(kCardWidth);
    // The answers are painted on the card rather than being widgets in it, so
    // the card is what a pointer over them reaches - and a widget without mouse
    // tracking is never told where the pointer is while no button is down. The
    // dialog would then draw the washes it has no reason to: a move the card
    // does not want travels up to the dialog, which does.
    d_ptr->card->setMouseTracking(true);
    d_ptr->cardLayout = new QVBoxLayout(d_ptr->card);
    d_ptr->cardLayout->setSpacing(kGap);

    // The title is a label rather than painted text, because a question can be
    // a sentence long and a sentence has to wrap; the full-screen shape puts
    // the name in the bar instead, where a name is one line.
    d_ptr->titleLabel = new QLabel(d_ptr->card);
    d_ptr->titleLabel->setTextFormat(Qt::PlainText);
    d_ptr->titleLabel->setFont(MaterialType::font(MaterialType::Scale::Title));
    d_ptr->titleLabel->setWordWrap(true);
    d_ptr->titleLabel->hide();
    d_ptr->cardLayout->addWidget(d_ptr->titleLabel);

    d_ptr->messageLabel = new QLabel(d_ptr->card);
    d_ptr->messageLabel->setTextFormat(Qt::PlainText);
    d_ptr->messageLabel->setFont(MaterialType::font(MaterialType::Scale::Body));
    d_ptr->messageLabel->setWordWrap(true);
    d_ptr->messageLabel->hide();
    d_ptr->cardLayout->addWidget(d_ptr->messageLabel);

    // Whatever the application put in it goes above the answers and keeps the
    // whole of the room left over.
    d_ptr->cardLayout->addStretch(1);
    // The answers are painted, not widgets, so the card keeps their room as a
    // spacer - the row is the last thing in the card in both shapes.
    d_ptr->cardLayout->addSpacing(kAnswerHeight);

    connect(&d_ptr->entrance, &QVariantAnimation::valueChanged, this, [this](const QVariant &value) {
        setWindowOpacity(value.toReal());
    });

    relayout();
}

MaterialDialog::~MaterialDialog(){QX_FINI_PRIVATE()}

QString MaterialDialog::title() const
{
    return d_ptr->titleLabel->text();
}

QString MaterialDialog::message() const
{
    return d_ptr->messageLabel->text();
}

MaterialDialog::Variant MaterialDialog::variant() const
{
    return d_ptr->variant;
}

QWidget *MaterialDialog::contentWidget() const
{
    return d_ptr->content;
}

QString MaterialDialog::buttonText(Role role) const
{
    return d_ptr->answers[int(role)];
}

QRect MaterialDialog::buttonRect(Role role) const
{
    return d_ptr->answerRects[int(role)];
}

void MaterialDialog::setTitle(const QString &title)
{
    if (d_ptr->titleLabel->text() == title) {
        return;
    }
    d_ptr->titleLabel->setText(title);
    // An empty line still measures a line, and a dialog with nothing to say
    // should not hold the room for saying it.
    d_ptr->titleLabel->setVisible(!title.isEmpty() && d_ptr->variant == Variant::Basic);
    // The title is what the dialog is called: a reader that arrives at the
    // window before it reads anything inside it asks this.
    setAccessibleName(title);
    relayout();
}

void MaterialDialog::setMessage(const QString &message)
{
    if (d_ptr->messageLabel->text() == message) {
        return;
    }
    d_ptr->messageLabel->setText(message);
    d_ptr->messageLabel->setVisible(!message.isEmpty());
    relayout();
}

void MaterialDialog::setVariant(Variant variant)
{
    if (d_ptr->variant == variant) {
        return;
    }
    d_ptr->variant = variant;
    // The two shapes carry the name in different places: a card has room to
    // wrap one and the bar has one line.
    d_ptr->titleLabel->setVisible(variant == Variant::Basic && !title().isEmpty());
    relayout();
}

void MaterialDialog::setButtonText(Role role, const QString &text)
{
    if (d_ptr->answers[int(role)] == text) {
        return;
    }
    d_ptr->answers[int(role)] = text;
    relayout();
}

void MaterialDialog::setContent(QWidget *widget)
{
    if (d_ptr->content == widget) {
        return;
    }
    if (d_ptr->content) {
        // Orphaned rather than deleted: the dialog was handed it and never
        // owned it, which is the same bargain the Fluent content dialog makes.
        d_ptr->cardLayout->removeWidget(d_ptr->content);
        d_ptr->content->setParent(Q_NULLPTR);
    }
    d_ptr->content = widget;
    if (widget) {
        widget->setParent(d_ptr->card);
        // Above the stretch, so it is between the message and the answers.
        d_ptr->cardLayout->insertWidget(d_ptr->cardLayout->count() - 2, widget);
    }
    relayout();
}

int MaterialDialog::cardHeight() const
{
    const int height = d_ptr->card->heightForWidth(kCardWidth);
    return height >= 0 ? height : d_ptr->card->sizeHint().height();
}

QRect MaterialDialog::cardRect() const
{
    if (d_ptr->variant == Variant::FullScreen) {
        // The place is the page: there is nothing beside it to dim.
        return rect();
    }
    const int room = 2 * cardRoom();
    const int wanted = cardHeight();
    const int available = qMax(room, height() - room);
    const int here = qBound(0, wanted, available);
    return QRect(QPoint((width() - kCardWidth) / 2, (height() - here) / 2), QSize(kCardWidth, here));
}

QSize MaterialDialog::sizeHint() const
{
    if (d_ptr->variant == Variant::FullScreen) {
        return QSize(kCardWidth, qMax(kAnswerHeight + 2 * kCardPadding + kBarHeight, cardHeight()));
    }
    // A dialog with no page under it is the size of its card and the room its
    // shadow needs round it.
    const int room = 2 * cardRoom();
    return QSize(kCardWidth + room, cardHeight() + room);
}

void MaterialDialog::relayout()
{
    const bool fullScreen = d_ptr->variant == Variant::FullScreen;
    d_ptr->cardLayout->setContentsMargins(
        fullScreen ? kBarInset : kCardPadding, fullScreen ? kBarHeight + kGap : kCardPadding,
        fullScreen ? kBarInset : kCardPadding, fullScreen ? kCardPadding : kCardPadding);
    d_ptr->cardLayout->invalidate();

    const QRect card = cardRect();
    d_ptr->card->setGeometry(card);

    // The answers are read to the right, and the answer to the question is the
    // last thing in the row: the row is placed from the right, primary first.
    const QFontMetrics metrics(MaterialType::font(MaterialType::Scale::Body, true));
    const int y = card.bottom() + 1 - kCardPadding - kAnswerHeight;
    int right = card.right() + 1 - kCardPadding;
    for (int i = 0; i < 2; ++i) {
        if (d_ptr->answers[i].isEmpty()) {
            d_ptr->answerRects[i] = QRect();
            continue;
        }
        const int width =
            qMax(kAnswerMinWidth, qCeil(metrics.horizontalAdvance(d_ptr->answers[i])) + 2 * kAnswerPadding);
        right -= width;
        d_ptr->answerRects[i] = QRect(right, y, width, kAnswerHeight);
        right -= kAnswerGap;
    }

    // The cross belongs to the full-screen shape, where it is the only way out
    // that is not the keyboard.
    d_ptr->crossRect =
        fullScreen ? QRect(width() - kBarInset - kCrossSize, (kBarHeight - kCrossSize) / 2, kCrossSize, kCrossSize)
                   : QRect();
    update();
}

void MaterialDialog::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const bool fullScreen = d_ptr->variant == Variant::FullScreen;
    const QColor page = palette().color(QPalette::Window);
    const QColor surface = QxControls::QxStateLayer::layered(page, Qt::white, kSurfaceLift);

    if (fullScreen) {
        // A place, not a card: there is no page beside it to say anything with.
        painter.fillRect(rect(), surface);
    } else {
        painter.fillRect(rect(), page);
        QColor veil(Qt::black);
        veil.setAlphaF(kScrimAlpha);
        painter.fillRect(rect(), veil);

        const QRectF area(cardRect());
        const qreal radius = MaterialShape::radius(MaterialShape::Radius::XLarge);
        const QxTheme::QxShadow shadow = MaterialElevation::shadow(MaterialElevation::Level::Level4);
        if (shadow.alpha > 0 && shadow.blurRadius > 0.0) {
            // The shadow falls on the veil and not on the page, so it is the
            // palette's shadow: what is under a lifted surface is dark.
            QxControls::QxShadowPainter::paint(&painter, area, radius, palette().color(QPalette::Shadow), shadow);
        }
        painter.setPen(Qt::NoPen);
        painter.setBrush(surface);
        painter.drawRoundedRect(area, radius, radius);
    }

    if (fullScreen) {
        const QColor ink = palette().color(QPalette::WindowText);
        // The name of the place, on one line: a bar does not grow for it.
        const QRect nameArea(kBarInset, 0, width() - 2 * (kBarInset + kCrossSize), kBarHeight);
        painter.setFont(MaterialType::font(MaterialType::Scale::Title));
        painter.setPen(ink);
        painter.drawText(nameArea, Qt::AlignVCenter | Qt::AlignLeft,
                         QFontMetrics(painter.font()).elidedText(title(), Qt::ElideRight, nameArea.width()));

        // A hairline is what makes the bar a bar rather than the first line of
        // what it holds.
        QColor line = ink;
        line.setAlphaF(0.12);
        painter.setPen(QPen(line, 1));
        painter.drawLine(QPointF(0, kBarHeight), QPointF(width(), kBarHeight));

        // The cross: a touch target with a state layer, and two strokes in it.
        if (d_ptr->crossHover || d_ptr->crossPressed) {
            QColor wash = ink;
            wash.setAlphaF(d_ptr->crossPressed ? 0.12 : 0.08);
            painter.setPen(Qt::NoPen);
            painter.setBrush(wash);
            painter.drawEllipse(QPointF(d_ptr->crossRect.center()), kCrossSize / 2.0, kCrossSize / 2.0);
        }
        const QPointF centre(d_ptr->crossRect.center());
        const qreal half = kCrossGlyph / 2.0;
        painter.setPen(QPen(ink, 1.6, Qt::SolidLine, Qt::RoundCap));
        painter.drawLine(centre + QPointF(-half, -half), centre + QPointF(half, half));
        painter.drawLine(centre + QPointF(half, -half), centre + QPointF(-half, half));
    }

    // The answers: text in the accent, which is the whole of what a Material
    // dialog's answer is - a filled button would be a command, and this asks.
    painter.setFont(MaterialType::font(MaterialType::Scale::Body, true));
    for (int i = 0; i < 2; ++i) {
        if (d_ptr->answerRects[i].isEmpty()) {
            continue;
        }
        const QRectF area(d_ptr->answerRects[i]);
        if (d_ptr->hovered == i || d_ptr->pressed == i) {
            QColor wash = QxTheme::QxThemeManager::instance()->accentColor();
            wash.setAlphaF(d_ptr->pressed == i ? 0.12 : 0.08);
            const qreal radius = MaterialShape::radius(MaterialShape::Radius::Circular);
            painter.setPen(Qt::NoPen);
            painter.setBrush(wash);
            painter.drawRoundedRect(area, radius, radius);
        }
        painter.setPen(QxTheme::QxThemeManager::instance()->accentColor());
        painter.drawText(area, Qt::AlignCenter, d_ptr->answers[i]);
    }
}

int MaterialDialog::answerAt(const QPoint &pos) const
{
    for (int i = 0; i < 2; ++i) {
        if (d_ptr->answerRects[i].contains(pos)) {
            return i;
        }
    }
    return -1;
}

bool MaterialDialog::isOnTheCross(const QPoint &pos) const
{
    return !d_ptr->crossRect.isEmpty() && d_ptr->crossRect.contains(pos);
}

void MaterialDialog::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        const int answer = answerAt(eventPos(event));
        if (answer >= 0) {
            d_ptr->pressed = answer;
            update();
            return;
        }
        if (isOnTheCross(eventPos(event))) {
            d_ptr->crossPressed = true;
            update();
            return;
        }
    }
    QDialog::mousePressEvent(event);
}

void MaterialDialog::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        const QPoint pos = eventPos(event);
        const int pressed = d_ptr->pressed;
        const bool crossPressed = d_ptr->crossPressed;
        d_ptr->pressed = -1;
        d_ptr->crossPressed = false;
        update();
        // A press is answered by where it started: a press that began on an
        // answer and was released somewhere else has not answered anything.
        if (pressed >= 0 && answerAt(pos) == pressed) {
            Q_EMIT buttonClicked(Role(pressed));
            return;
        }
        if (crossPressed && isOnTheCross(pos)) {
            reject();
            return;
        }
    }
    QDialog::mouseReleaseEvent(event);
}

void MaterialDialog::mouseMoveEvent(QMouseEvent *event)
{
    const QPoint pos = eventPos(event);
    const int answer = answerAt(pos);
    const bool cross = isOnTheCross(pos);
    if (answer != d_ptr->hovered || cross != d_ptr->crossHover) {
        d_ptr->hovered = answer;
        d_ptr->crossHover = cross;
        update();
    }
    QDialog::mouseMoveEvent(event);
}

void MaterialDialog::leaveEvent(QEvent *event)
{
    if (d_ptr->hovered != -1 || d_ptr->crossHover) {
        d_ptr->hovered = -1;
        d_ptr->crossHover = false;
        update();
    }
    QDialog::leaveEvent(event);
}

void MaterialDialog::keyPressEvent(QKeyEvent *event)
{
    // Return reaches the affirmative, the way it reaches the button a dialog
    // would have focused: an answer the keyboard cannot give is not an answer.
    if ((event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) &&
        !d_ptr->answers[int(Role::Primary)].isEmpty()) {
        Q_EMIT buttonClicked(Role::Primary);
        return;
    }
    QDialog::keyPressEvent(event);
}

void MaterialDialog::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);
    relayout();
}

void MaterialDialog::showEvent(QShowEvent *event)
{
    coverThePage();
    QDialog::showEvent(event);
    relayout();
    beginTheEntrance();
}

void MaterialDialog::hideEvent(QHideEvent *event)
{
    d_ptr->entrance.stop();
    setWindowOpacity(1.0);
    QDialog::hideEvent(event);
}

void MaterialDialog::coverThePage()
{
    QWidget *owner = parentWidget() ? parentWidget()->window() : Q_NULLPTR;
    if (!owner || owner == this || !owner->isVisible()) {
        // Nothing to dim and nowhere to expand from: the dialog stands at its
        // own size.
        resize(sizeHint());
        return;
    }
    // The client area of the window it was opened over. A full-screen dialog
    // is then already the page; a basic one draws its card in the middle of it.
    setGeometry(owner->geometry());
}

void MaterialDialog::beginTheEntrance()
{
    const QxTheme::QxMotion motion =
        MaterialMotion::forDuration(MaterialMotion::Duration::Medium, MaterialMotion::Easing::Emphasized);
    d_ptr->entrance.stop();
    if (motion.duration <= 0) {
        // Reduced motion is not a shorter arrival, it is no arrival.
        setWindowOpacity(1.0);
        return;
    }
    setWindowOpacity(0.0);
    d_ptr->entrance.setDuration(motion.duration);
    d_ptr->entrance.setEasingCurve(motion.curve);
    d_ptr->entrance.setStartValue(0.0);
    d_ptr->entrance.setEndValue(1.0);
    d_ptr->entrance.start();
}

QX_MATERIAL_END_NAMESPACE
