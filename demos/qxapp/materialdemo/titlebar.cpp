/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "titlebar.h"

#include "qxmaterial/materialripple.h"
#include "qxmaterial/materialtopappbar.h"
#include "qxwindow/windowagentwidget.h"

#include <QtGui/QContextMenuEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>
#include <QtGui/QWindow>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>

QX_MATERIAL_USE_NAMESPACE
QX_WINDOW_USE_NAMESPACE

namespace
{
/*! The circle a Material caption button wears. 48 is not a choice about
 *  captions: it is the square the bar's own trailing icons use, so the window's
 *  buttons and the app's buttons in the same bar are the same size. */
constexpr int kButtonSize = 48;
constexpr int kGlyphSize = 10;

/*! The stroke the close glyph is drawn with. The other glyphs are horizontal
 *  and vertical strokes that land on whole pixel rows, but the close glyph is
 *  two 45 degree diagonals: anti-aliasing spreads each of them over two rows at
 *  roughly half coverage, which reads a step lighter than its brothers at the
 *  same 1 px. The compensation is the geometry's own - a diagonal needs sqrt(2)
 *  times the perpendicular stroke to put the same ink on a pixel row. */
constexpr qreal kCloseStroke = 1.4;

/*! The washes an M3 icon button wears, as fractions of the ink over the
 *  surface - the same three the library's own buttons use. */
constexpr qreal kHoverOpacity = 0.08;
constexpr qreal kPressedOpacity = 0.12;

/*! What a close button turns when the pointer is on it, on every platform that
 *  draws one: the only red on an otherwise monochrome caption. The two demos use
 *  the same two colours - a caption button is the window's, not the family's. */
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
    setFixedSize(kButtonSize, kButtonSize);
    m_ripple = new MaterialRipple(this);
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
    return QSize(kButtonSize, kButtonSize);
}

bool CaptionButton::event(QEvent *event)
{
    // Hover and leave are the whole of the visual state besides pressed, and
    // neither changes the geometry: a repaint is all that is needed, and
    // underMouse() is current by the time the event arrives.
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
    return rect().contains(pos);
}

void CaptionButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const bool closeButton = m_glyph == Glyph::Close;
    const bool lit = underMouse() || isDown();
    QColor ink = palette().color(QPalette::WindowText);
    if (closeButton && lit) {
        // White on the red, not the palette's highlighted text: the disc is red
        // on every theme, so the glyph on it is white on every theme too.
        ink = QColor(Qt::white);
    }

    // The wash is a circle, not the rectangle: that is the whole difference
    // between an M3 icon button and a Fluent caption button. The close button is
    // the one that is not a wash - it turns red, whole, because closing is not a
    // state of the surface but the one way out of it.
    const QPointF centre = QRectF(rect()).center();
    const qreal radius = kButtonSize / 2.0 - 2.0;
    const QRectF disc(centre.x() - radius, centre.y() - radius, radius * 2.0, radius * 2.0);
    if (closeButton && lit) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(isDown() ? closePressed() : closeHover());
        painter.drawEllipse(disc);
    } else if (lit) {
        const QColor layer = palette().color(QPalette::Highlight);
        const qreal opacity = isDown() ? kPressedOpacity : kHoverOpacity;
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(layer.red(), layer.green(), layer.blue(), qRound(255 * opacity)));
        painter.drawEllipse(disc);
    }

    // The ripple is the press answer on top of the wash, and it lands under the
    // glyph for the same reason the wash does: what the button is telling the
    // user about is the glyph, not the disc.
    m_ripple->paint(&painter, disc, radius);

    QPen pen(ink);
    pen.setWidthF(1.0);
    pen.setCapStyle(Qt::FlatCap);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);

    const qreal left = centre.x() - kGlyphSize / 2.0;
    const qreal top = centre.y() - kGlyphSize / 2.0;

    switch (m_glyph) {
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
    : QWidget(parent)
{
    setObjectName(QStringLiteral("materialTitleBar"));

    auto *layout = new QHBoxLayout(this);
    // Eight on the right, the same as the Fluent demo gives its own caption: a
    // caption is the window's edge rather than a part of either design language,
    // so both demos leave the same room there. Without it the close button's red
    // disc sat two pixels off the frame - a disc that looks poured over the edge
    // rather than a button standing on the bar.
    layout->setContentsMargins(QMargins(0, 0, 8, 0));
    layout->setSpacing(0);

    m_bar = new MaterialTopAppBar(this);
    m_bar->setObjectName(QStringLiteral("appBar"));
    layout->addWidget(m_bar, 1);

    // The buttons sit in a column of their own at the window's edge, centred in
    // it the way the bar centres its own icons: a 48 button in a 56 bar is four
    // above and four below, and the row lines up with the trailing icons it
    // stands next to instead of riding the top edge.
    auto *column = new QWidget(this);
    auto *columnLayout = new QVBoxLayout(column);
    columnLayout->setContentsMargins(QMargins());
    columnLayout->setSpacing(0);

    auto *buttons = new QWidget(column);
    auto *buttonLayout = new QHBoxLayout(buttons);
    buttonLayout->setContentsMargins(QMargins());
    buttonLayout->setSpacing(0);

    m_minimize = new CaptionButton(CaptionButton::Glyph::Minimize, buttons);
    m_minimize->setObjectName(QStringLiteral("captionMinimize"));
    m_maximize = new CaptionButton(CaptionButton::Glyph::Maximize, buttons);
    m_maximize->setObjectName(QStringLiteral("captionMaximize"));
    m_maximize->setCheckable(true);
    m_close = new CaptionButton(CaptionButton::Glyph::Close, buttons);
    m_close->setObjectName(QStringLiteral("captionClose"));

    for (CaptionButton *button : {m_minimize, m_maximize, m_close}) {
        buttonLayout->addWidget(button);
        // The bar draws the ink the buttons paint with: a caption button on a
        // bar is ink on the bar's surface, and the bar is the one that knows
        // which surface that is.
        button->setPalette(m_bar->palette());
    }

    columnLayout->addStretch(1);
    columnLayout->addWidget(buttons);
    columnLayout->addStretch(1);
    layout->addWidget(column);

    // The bar answers for the whole of its own rectangle in the hit test, which
    // is what keeps its trailing icons clickable - and the price of that is that
    // a press anywhere on the bar reaches the bar instead of starting a window
    // move. Watching the bar is how the wrapper takes the move back: what the
    // hit test used to do on the first press, this does once the pointer has
    // travelled far enough to have said it is a drag and not a click.
    m_bar->installEventFilter(this);

    connect(m_maximize, &QAbstractButton::toggled, m_maximize, [this](bool checked) {
        m_maximize->setGlyph(checked ? CaptionButton::Glyph::Restore : CaptionButton::Glyph::Maximize);
    });
}

bool TitleBar::eventFilter(QObject *watched, QEvent *event)
{
    if (watched != m_bar) {
        return QWidget::eventFilter(watched, event);
    }

    switch (event->type()) {
    case QEvent::MouseButtonPress: {
        const auto *mouse = static_cast<QMouseEvent *>(event);
        if (mouse->button() == Qt::LeftButton) {
            m_pressPos = mouse->pos();
            m_moving = false;
        }
        break;
    }
    case QEvent::MouseMove: {
        const auto *mouse = static_cast<QMouseEvent *>(event);
        // The threshold is the platform's own: the same distance a drag has to
        // travel before a press stops counting as a click, which is what makes
        // "press an icon, release on it" still a click and "press, drag" a move.
        if (m_moving || !(mouse->buttons() & Qt::LeftButton) ||
            (mouse->pos() - m_pressPos).manhattanLength() < QApplication::startDragDistance()) {
            break;
        }
        m_moving = true;
        if (QWindow *handle = window()->windowHandle()) {
            handle->startSystemMove();
        }
        break;
    }
    case QEvent::MouseButtonRelease:
        m_moving = false;
        break;
    case QEvent::MouseButtonDblClick: {
        const auto *mouse = static_cast<QMouseEvent *>(event);
        // A double click on a caption maximizes the window, or puts it back -
        // the gesture a title bar has on every desktop. On an icon it is not
        // this gesture: the icon is a button in the caption, and a button
        // pressed twice is a button, not the caption being asked for. That is
        // the one thing the bar answers and the wrapper cannot, which is why
        // the bar is asked where its icons stand.
        if (mouse->button() != Qt::LeftButton || m_bar->actionAt(mouse->pos())) {
            break;
        }
        QWidget *top = window();
        if (top->isMaximized()) {
            top->showNormal();
        } else {
            top->showMaximized();
        }
        return true;
    }
#ifdef Q_OS_WIN
    case QEvent::ContextMenu: {
        // The system menu is the other half of what a caption has and a client
        // area does not: right clicking a title bar asks the window itself,
        // with Move, Size, Minimize, Maximize and Close in it. The agent is the
        // only thing here that can still ask Windows for it, so it is asked
        // rather than a menu of our own being invented.
        const auto *context = static_cast<QContextMenuEvent *>(event);
        if (!m_agent) {
            break;
        }
        m_agent->showSystemMenu(context->globalPos());
        return true;
    }
#endif
    default:
        break;
    }
    return QWidget::eventFilter(watched, event);
}

void TitleBar::setWindowAgent(WindowAgentWidget *agent)
{
    m_agent = agent;
}

MaterialTopAppBar *TitleBar::appBar() const
{
    return m_bar;
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
