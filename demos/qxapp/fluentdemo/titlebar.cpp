/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "titlebar.h"

#include "qxfluent/fluentcommandbar.h"
#include "qxfluent/fluenttokens.h"
#include "qxcontrols/qxstatelayer.h"

#include <QtGui/QPainter>
#include <QtGui/QPainterPath>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>

QX_FLUENT_USE_NAMESPACE

namespace
{
/*! The height of the bar, and the height of the buttons in it. 48 rather than
 *  the 32 of a bare caption: the search field and the command bar live here, and
 *  a command bar is a 40 tall row of 32 tall commands - giving it less than that
 *  cuts the bottom off every command in it. */
constexpr int kBarHeight = 48;
/*! A caption button is a 46 by 32 rectangle centred in the bar, which is the
 *  size WinUI gives them: the bar is the window's edge, the buttons are places
 *  on it, and a button as tall as the bar reads as a block rather than a button. */
constexpr int kCaptionWidth = 46;
constexpr int kCaptionHeight = 32;
constexpr int kIconWidth = 40;
constexpr int kIconSize = 16;
/*! The square the glyphs are drawn in, which is centred in the button. */
constexpr int kGlyphSize = 10;
constexpr int kFieldWidth = 220;
constexpr int kFieldHeight = 32;

/*! The stroke the close glyph is drawn with. The other glyphs are horizontal
 *  and vertical strokes that land on whole pixel rows, but the close glyph is
 *  two 45 degree diagonals: anti-aliasing spreads each of them over two rows at
 *  roughly half coverage, which reads a step lighter than its brothers at the
 *  same 1 px. The compensation is the geometry's own - a diagonal needs sqrt(2)
 *  times the perpendicular stroke to put the same ink on a pixel row. */
constexpr qreal kCloseStroke = 1.4;

/*! The wash a caption button wears: Fluent's own, which is fainter than a
 *  button's - a caption is not something to aim at. */
constexpr qreal kHoverOpacity = 0.06;
constexpr qreal kPressedOpacity = 0.09;

/*! What a close button turns when the pointer is on it, on every platform that
 *  draws one: the only red on an otherwise monochrome caption. */
QColor closeHover()
{
    return QColor(196, 43, 28);
}
QColor closePressed()
{
    return QColor(162, 34, 22);
}
}   // namespace

CaptionButton::CaptionButton(Glyph glyph, QWidget *parent)
    : QAbstractButton(parent)
    , m_glyph(glyph)
{
    // A caption button is not part of the tab order, and it must not take the
    // focus away from the window it belongs to when it is clicked.
    setFocusPolicy(Qt::NoFocus);
    setCursor(m_glyph == Glyph::App ? Qt::ArrowCursor : Qt::ArrowCursor);
    // A fixed height rather than a policy: the bar is taller than a caption
    // button by design, and the layout is what centres it.
    setFixedHeight(kCaptionHeight);
}

CaptionButton::Glyph CaptionButton::glyph() const
{
    return m_glyph;
}

void CaptionButton::setGlyph(Glyph glyph)
{
    if (m_glyph == glyph) {
        return;
    }
    m_glyph = glyph;
    update();
}

QSize CaptionButton::sizeHint() const
{
    return QSize(m_glyph == Glyph::App ? kIconWidth : kCaptionWidth, kCaptionHeight);
}

bool CaptionButton::event(QEvent *event)
{
    // Hover and leave are the whole of the visual state a caption button has
    // besides pressed, and neither changes the widget's geometry: a repaint is
    // all that is needed, and underMouse() is already current by the time the
    // event arrives.
    switch (event->type()) {
    case QEvent::Enter:
    case QEvent::Leave:
        update();
        break;
    default:
        break;
    }
    return QAbstractButton::event(event);
}

bool CaptionButton::hitButton(const QPoint &pos) const
{
    // The whole rectangle is the button, corner to corner: a caption button is
    // a rectangle with a glyph in it, not a glyph with a margin around it.
    return rect().contains(pos);
}

void CaptionButton::mouseDoubleClickEvent(QMouseEvent *event)
{
    // The base class is called first: it is what emits clicked() for the press
    // that came with the double click, and a system menu that opened on the
    // first click must still open on a click.
    QAbstractButton::mouseDoubleClickEvent(event);
    if (m_glyph == Glyph::App) {
        Q_EMIT doubleClicked();
    }
}

void CaptionButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const bool closeButton = m_glyph == Glyph::Close;
    QColor layer = palette().color(QPalette::WindowText);
    qreal opacity = 0.0;
    if (closeButton && (isDown() || underMouse())) {
        painter.fillRect(rect(), isDown() ? closePressed() : closeHover());
    } else if (underMouse() || isDown()) {
        opacity = isDown() ? kPressedOpacity : kHoverOpacity;
        QxControls::QxStateLayer::paint(&painter, rect(), layer, opacity);
    }

    QColor ink = palette().color(QPalette::WindowText);
    if (closeButton && (isDown() || underMouse())) {
        ink = QColor(Qt::white);
    }

    const QPointF centre = QRectF(rect()).center();
    QPen pen(ink);
    pen.setWidthF(1.0);
    pen.setCapStyle(Qt::FlatCap);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);

    const qreal left = centre.x() - kGlyphSize / 2.0;
    const qreal top = centre.y() - kGlyphSize / 2.0;

    switch (m_glyph) {
    case Glyph::App: {
        // The application mark: an accent square with a window drawn inside it.
        // Two rounded rectangles is enough to say "an application" without an
        // icon file, and the accent comes from the theme like everything else.
        const QRectF mark(centre.x() - kIconSize / 2.0, centre.y() - kIconSize / 2.0, kIconSize, kIconSize);
        painter.setPen(Qt::NoPen);
        painter.setBrush(palette().color(QPalette::Highlight));
        painter.drawRoundedRect(mark, 3.0, 3.0);

        painter.setBrush(Qt::NoBrush);
        QPen inner{QColor(Qt::white)};
        inner.setWidthF(1.2);
        painter.setPen(inner);
        painter.drawRoundedRect(mark.adjusted(3.5, 3.5, -3.5, -3.5), 1.5, 1.5);
        break;
    }
    case Glyph::Minimize:
        painter.drawLine(QPointF(left, centre.y()), QPointF(left + kGlyphSize, centre.y()));
        break;
    case Glyph::Maximize:
        painter.drawRect(QRectF(left, top, kGlyphSize, kGlyphSize));
        break;
    case Glyph::Restore: {
        const qreal offset = 2.0;
        painter.drawRect(QRectF(left, top + offset, kGlyphSize - offset, kGlyphSize - offset));
        painter.drawLine(QPointF(left + offset, top), QPointF(left + kGlyphSize, top));
        painter.drawLine(QPointF(left + kGlyphSize, top), QPointF(left + kGlyphSize, top + kGlyphSize - offset));
        break;
    }
    case Glyph::Close:
        // The one glyph that draws its own weight: at the shared 1 px the two
        // diagonals read lighter than the horizontal and vertical strokes
        // beside them, however equal the pen says they are.
        pen.setWidthF(kCloseStroke);
        painter.setPen(pen);
        painter.drawLine(QPointF(left, top), QPointF(left + kGlyphSize, top + kGlyphSize));
        painter.drawLine(QPointF(left, top + kGlyphSize), QPointF(left + kGlyphSize, top));
        break;
    }
}

TitleBar::TitleBar(QWidget *parent)
    : QFrame(parent)
{
    setObjectName(QStringLiteral("fluentTitleBar"));
    // A frame by inheritance only: the bar has no border of its own and takes
    // the window's background, so a change of theme repaints it without the bar
    // knowing what a theme is.
    setFrameShape(QFrame::NoFrame);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    setFixedHeight(kBarHeight);

    auto *layout = new QHBoxLayout(this);
    // Eight on the right as well as on the left: the bar centres its 32 tall
    // buttons in 48, which is eight above and eight below, so a close button
    // flush against the window's edge would be the one side of the caption with
    // no room in it. The gap is the same on all four sides, and the corner reads
    // as a corner rather than as a button cut off by the frame.
    layout->setContentsMargins(8, 0, 8, 0);
    layout->setSpacing(8);

    m_icon = new CaptionButton(CaptionButton::Glyph::App, this);
    m_icon->setObjectName(QStringLiteral("captionIcon"));
    layout->addWidget(m_icon);

    m_title = new QLabel(window()->windowTitle(), this);
    m_title->setObjectName(QStringLiteral("captionTitle"));
    // The name of the window is body-strong: the size the commands beside it are
    // set in, told apart by weight. Left to the application's default font it
    // came out at the ramp's caption step - right for a bare 32 tall title bar,
    // a size below everything else in this one, which is 48 tall and carries a
    // 40 tall command bar whose text is body.
    m_title->setFont(FluentType::font(FluentType::Ramp::Body, true));
    layout->addWidget(m_title);

    m_search = new QLineEdit(this);
    m_search->setObjectName(QStringLiteral("captureSearch"));
    m_search->setPlaceholderText(tr("Search"));
    m_search->setClearButtonEnabled(true);
    m_search->setFixedSize(kFieldWidth, kFieldHeight);
    layout->addWidget(m_search);
    connect(m_search, &QLineEdit::textChanged, this, &TitleBar::searchTextChanged);

    layout->addStretch(1);

    m_bar = new FluentCommandBar(this);
    m_bar->setObjectName(QStringLiteral("commandBar"));
    // No height is forced on the command bar: it measures forty for a row of
    // thirty-two tall commands and centres them in that, so a shorter row clips
    // the bottom off every command. The 48 high bar is what gives it its room.
    layout->addWidget(m_bar);

    layout->addSpacing(4);

    m_minimize = new CaptionButton(CaptionButton::Glyph::Minimize, this);
    m_minimize->setObjectName(QStringLiteral("captionMinimize"));
    m_maximize = new CaptionButton(CaptionButton::Glyph::Maximize, this);
    m_maximize->setObjectName(QStringLiteral("captionMaximize"));
    m_maximize->setCheckable(true);
    m_close = new CaptionButton(CaptionButton::Glyph::Close, this);
    m_close->setObjectName(QStringLiteral("captionClose"));

    // The bar hands the three over rather than deciding what they do: a window
    // is minimized, maximized and closed by the window, and a bar that did it
    // itself would be a second window.
    for (CaptionButton *button : {m_minimize, m_maximize, m_close}) {
        layout->addWidget(button);
    }

    // A maximize button shows the state it will produce rather than the state
    // the window is in: a checked button is an invitation to restore.
    connect(m_maximize, &QAbstractButton::toggled, m_maximize, [this](bool checked) {
        m_maximize->setGlyph(checked ? CaptionButton::Glyph::Restore : CaptionButton::Glyph::Maximize);
    });
}

FluentCommandBar *TitleBar::commandBar() const
{
    return m_bar;
}

QLineEdit *TitleBar::searchField() const
{
    return m_search;
}

CaptionButton *TitleBar::iconButton() const
{
    return m_icon;
}

CaptionButton *TitleBar::minimizeButton() const
{
    return m_minimize;
}

CaptionButton *TitleBar::maximizeButton() const
{
    return m_maximize;
}

CaptionButton *TitleBar::closeButton() const
{
    return m_close;
}

void TitleBar::setTitle(const QString &title)
{
    m_title->setText(title);
}
