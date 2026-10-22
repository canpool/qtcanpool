/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "fluentcontentdialog.h"

#include "fluentcard.h"
#include "fluenttokens.h"

#include "qxcontrols/qxshadowpainter.h"
#include "qxcontrols/qxstatelayer.h"

#include <QtCore/QVariantAnimation>
#include <QtGui/QPainter>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

QX_FLUENT_BEGIN_NAMESPACE

namespace
{
// The card in the middle: four hundred wide, twenty-four in from its own edge
// at every side, and twelve between the things stacked in it. The dialog sits
// at the 64 level of the elevation scale, which is the one the vocabulary says
// dims what is under it - and the room that shadow needs is the same room
// FluentCard keeps, asked of FluentCard.
constexpr int kCardWidth = 400;
constexpr int kCardPadding = 24;
constexpr int kGap = 12;
constexpr int kButtonHeight = 32;
constexpr int kButtonGap = 8;

// Fluent's smoke layer: a black veil at about a third. It is a black veil in
// both themes and not a colour either of them chose, because it is not paint -
// it is the light going away from everything that is not the card.
constexpr int kScrimAlpha = 80;

// How far the card is lifted off the page it covers. On a dark theme a shadow
// under a surface is invisible against a dark page, so what carries the lift is
// the surface itself: the page's own colour with a little light mixed into it -
// which in a light theme is a change of about one step, because there the veil
// is already doing the whole job.
constexpr qreal kSurfaceLift = 0.06;

/*! The room the card keeps between itself and the edge of the dialog - the
 *  reach of the shadow it casts. */
int cardRoom()
{
    return FluentCard::shadowMargin(FluentElevation::Level::Depth64);
}
}   // namespace

class FluentContentDialogPrivate
{
public:
    QX_DECLARE_PUBLIC(FluentContentDialog)

    QWidget *card = Q_NULLPTR;
    QVBoxLayout *cardLayout = Q_NULLPTR;
    QLabel *titleLabel = Q_NULLPTR;
    QLabel *messageLabel = Q_NULLPTR;
    QWidget *content = Q_NULLPTR;
    QWidget *bar = Q_NULLPTR;
    QHBoxLayout *barLayout = Q_NULLPTR;
    QPushButton *buttons[3] = {Q_NULLPTR, Q_NULLPTR, Q_NULLPTR};
    QVariantAnimation entrance;
};

FluentContentDialog::FluentContentDialog(QWidget *parent)
    : QDialog(parent)
    , d_ptr(new FluentContentDialogPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    // A dialog drawn by Fluent has no frame of the platform's: the card is the
    // frame, and a title bar above it would be a second answer to what this is.
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    // It covers the page it was opened over and dims it. The veil says the page
    // is out of reach; modality is what makes that true, and it is true of that
    // window and not of the application - the page under the veil is what was
    // taken away.
    setWindowModality(Qt::WindowModal);

    d_ptr->card = new QWidget(this);
    d_ptr->card->setFixedWidth(kCardWidth);
    d_ptr->cardLayout = new QVBoxLayout(d_ptr->card);
    d_ptr->cardLayout->setContentsMargins(kCardPadding, kCardPadding, kCardPadding, kCardPadding);
    d_ptr->cardLayout->setSpacing(kGap);

    // The title is a label and not painted text, because a question can be a
    // sentence long and a sentence has to wrap: the layout is what measures it,
    // and the card is as tall as that came out.
    d_ptr->titleLabel = new QLabel(d_ptr->card);
    d_ptr->titleLabel->setTextFormat(Qt::PlainText);
    d_ptr->titleLabel->setFont(FluentType::font(FluentType::Ramp::Subtitle, true));
    d_ptr->titleLabel->hide();
    d_ptr->cardLayout->addWidget(d_ptr->titleLabel);

    d_ptr->messageLabel = new QLabel(d_ptr->card);
    d_ptr->messageLabel->setTextFormat(Qt::PlainText);
    d_ptr->messageLabel->setFont(FluentType::font(FluentType::Ramp::Body));
    d_ptr->messageLabel->setWordWrap(true);
    d_ptr->messageLabel->hide();
    d_ptr->cardLayout->addWidget(d_ptr->messageLabel);

    // The buttons are the end of the card and the stretch above them is what
    // keeps them there when the card is taller than what it holds.
    d_ptr->cardLayout->addStretch(1);
    d_ptr->bar = new QWidget(d_ptr->card);
    d_ptr->bar->setFixedHeight(kButtonHeight);
    d_ptr->barLayout = new QHBoxLayout(d_ptr->bar);
    d_ptr->barLayout->setContentsMargins(0, 0, 0, 0);
    d_ptr->barLayout->setSpacing(kButtonGap);
    // The row is read to the right: what to do, what else could be done, and
    // the way out of being asked.
    d_ptr->barLayout->addStretch(1);
    // A bar with nothing in it is not a row: it holds room the card would
    // otherwise give back, so it only exists once a button does.
    d_ptr->bar->hide();
    d_ptr->cardLayout->addWidget(d_ptr->bar);

    connect(&d_ptr->entrance, &QVariantAnimation::valueChanged, this, [this](const QVariant &value) {
        setWindowOpacity(value.toReal());
    });
}

FluentContentDialog::~FluentContentDialog(){QX_FINI_PRIVATE()}

QString FluentContentDialog::title() const
{
    return d_ptr->titleLabel->text();
}

QString FluentContentDialog::message() const
{
    return d_ptr->messageLabel->text();
}

QWidget *FluentContentDialog::contentWidget() const
{
    return d_ptr->content;
}

void FluentContentDialog::setTitle(const QString &title)
{
    if (d_ptr->titleLabel->text() == title) {
        return;
    }
    d_ptr->titleLabel->setText(title);
    // An empty line still measures a line, and a dialog with nothing to say
    // should not hold the room for saying it.
    d_ptr->titleLabel->setVisible(!title.isEmpty());
    // The title is what the dialog is called: a reader that arrives at the
    // window before it reads anything inside it asks this.
    setAccessibleName(title);
    relayout();
}

void FluentContentDialog::setMessage(const QString &message)
{
    if (d_ptr->messageLabel->text() == message) {
        return;
    }
    d_ptr->messageLabel->setText(message);
    d_ptr->messageLabel->setVisible(!message.isEmpty());
    relayout();
}

void FluentContentDialog::setContent(QWidget *widget)
{
    if (d_ptr->content == widget) {
        return;
    }
    if (d_ptr->content) {
        // Orphaned rather than deleted: the dialog was handed it and never
        // owned it, which is the same bargain FluentNavigationView makes. Out
        // of the layout first, or the layout would go on placing a widget that
        // is no longer in it.
        d_ptr->cardLayout->removeWidget(d_ptr->content);
        d_ptr->content->setParent(Q_NULLPTR);
    }
    d_ptr->content = widget;
    if (widget) {
        widget->setParent(d_ptr->card);
        // Before the bar, so the buttons stay the last thing in the card.
        d_ptr->cardLayout->insertWidget(d_ptr->cardLayout->count() - 2, widget);
    }
    relayout();
}

QPushButton *FluentContentDialog::primaryButton()
{
    return ensureButton(Role::Primary);
}

QPushButton *FluentContentDialog::secondaryButton()
{
    return ensureButton(Role::Secondary);
}

QPushButton *FluentContentDialog::closeButton()
{
    return ensureButton(Role::Close);
}

QPushButton *FluentContentDialog::ensureButton(Role role)
{
    const int index = int(role);
    if (d_ptr->buttons[index]) {
        return d_ptr->buttons[index];
    }

    auto *button = new QPushButton(d_ptr->bar);
    button->setFixedHeight(kButtonHeight);
    // Every button is tall enough to be aimed at without the row changing
    // shape as the words in it change.
    button->setMinimumWidth(2 * kCardPadding);
    // Return reaches the affirmative and nothing else: a stray Return that
    // landed on the way out would answer the question by closing it.
    button->setDefault(role == Role::Primary);
    button->setAutoDefault(false);
    d_ptr->buttons[index] = button;

    // Where the button belongs in the row is decided by the row's own order and
    // not by the order the application happened to ask for them in: the stretch
    // is first, so the slot is one plus however many of the earlier roles
    // already exist.
    int slot = 1;
    for (int earlier = 0; earlier < index; ++earlier) {
        if (d_ptr->buttons[earlier]) {
            ++slot;
        }
    }
    d_ptr->barLayout->insertWidget(slot, button);
    d_ptr->bar->show();
    relayout();
    return button;
}

int FluentContentDialog::cardHeight() const
{
    // A wrapped label has no height of its own - it has one per width - so the
    // card answers for the width it is always drawn at. The fallback is for a
    // card whose contents have no such answer, which is everything that is not
    // a wrapped label.
    const int height = d_ptr->card->heightForWidth(kCardWidth);
    return height >= 0 ? height : d_ptr->card->sizeHint().height();
}

QRect FluentContentDialog::cardRect() const
{
    // The card keeps the shadow's reach off the edge of the dialog, so a page
    // too short for it shortens the card rather than letting the shadow be cut.
    const int room = 2 * cardRoom();
    const int wanted = cardHeight();
    const int available = qMax(room, height() - room);
    const int cardHeightHere = qBound(0, wanted, available);

    return QRect(QPoint((width() - kCardWidth) / 2, (height() - cardHeightHere) / 2),
                 QSize(kCardWidth, cardHeightHere));
}

QSize FluentContentDialog::sizeHint() const
{
    // A dialog with no page under it is the size of its card and the room the
    // shadow needs round it - and that is the size it stands at.
    const int room = 2 * cardRoom();
    return QSize(kCardWidth + room, cardHeight() + room);
}

void FluentContentDialog::relayout()
{
    d_ptr->cardLayout->invalidate();
    d_ptr->card->setGeometry(cardRect());
    update();
}

void FluentContentDialog::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // The page, and then the light going away from it. The card is the page's
    // own colour: what lifts it is not a different paint but the veil around
    // it, which is the whole reason a scrim is drawn rather than a border.
    painter.fillRect(rect(), palette().color(QPalette::Window));
    painter.fillRect(rect(), QColor(0, 0, 0, kScrimAlpha));

    const QRectF surface(cardRect());
    const qreal radius = FluentShape::radius(FluentShape::Radius::XLarge);
    const QxTheme::QxShadow shadow = FluentElevation::shadow(FluentElevation::Level::Depth64);
    if (shadow.alpha > 0 && shadow.blurRadius > 0.0) {
        // The shadow falls on the veil and not on the page, so it is the
        // palette's shadow rather than the page's ink: what is under a card
        // lifted this far is dark, and a card does not glow.
        QxControls::QxShadowPainter::paint(&painter, surface, radius, palette().color(QPalette::Shadow), shadow);
    }
    painter.setPen(Qt::NoPen);
    painter.setBrush(QxControls::QxStateLayer::layered(palette().color(QPalette::Window), Qt::white, kSurfaceLift));
    painter.drawRoundedRect(surface, radius, radius);
}

void FluentContentDialog::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);
    relayout();
}

void FluentContentDialog::showEvent(QShowEvent *event)
{
    coverThePage();
    QDialog::showEvent(event);
    relayout();
    // The keyboard starts on the answer rather than on the way out.
    if (d_ptr->buttons[int(Role::Primary)]) {
        d_ptr->buttons[int(Role::Primary)]->setFocus();
    }
    beginTheEntrance();
}

void FluentContentDialog::hideEvent(QHideEvent *event)
{
    // A hidden window that is still fading would arrive at whatever is left of
    // the fade when it is shown again, so the fade ends with the hiding.
    d_ptr->entrance.stop();
    setWindowOpacity(1.0);
    QDialog::hideEvent(event);
}

void FluentContentDialog::coverThePage()
{
    QWidget *owner = parentWidget() ? parentWidget()->window() : Q_NULLPTR;
    if (!owner || owner == this || !owner->isVisible()) {
        // Nothing to dim: the dialog stands at its own size, veil and all.
        resize(sizeHint());
        return;
    }
    // The client area of the window it was opened over, in screen coordinates -
    // a veil smaller than the page would say it about part of it.
    setGeometry(owner->geometry());
}

void FluentContentDialog::beginTheEntrance()
{
    const QxTheme::QxMotion motion = FluentMotion::forDuration(FluentMotion::Duration::Slow);
    d_ptr->entrance.stop();
    if (motion.duration <= 0) {
        // Reduced motion is not a shorter fade, it is no fade: the dialog is
        // simply there, which is what the switch asked for.
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

QX_FLUENT_END_NAMESPACE
