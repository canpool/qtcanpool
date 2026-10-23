/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "fluentmaterialwindow.h"

#include "qxwindow/windowagentwidget.h"

#include <QtGui/QPainter>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>

QX_WINDOW_USE_NAMESPACE

namespace
{
/*! The caption is a row and nothing else: a name on the left, one button on
 *  the right. 40 rather than the 32 of a bare system caption, because the name
 *  in it is read rather than glanced at. */
constexpr int kCaptionHeight = 40;
constexpr int kCaptionButtonWidth = 46;
constexpr int kCaptionButtonHeight = 32;
/*! The square the close glyph is drawn in, centred in its button. */
constexpr int kGlyphSize = 10;

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

/*!
 * The window's one system button, drawn rather than taken from a style.
 *
 * The window has no system frame left - the agent took it - so the caption is
 * ours and the buttons on it are ours too. It is the window's only button and
 * it does one thing, so it is a handful of lines rather than a widget: a flat
 * rectangle that shows itself under the pointer, and a cross.
 */
class CaptionCloseButton : public QAbstractButton
{
public:
    explicit CaptionCloseButton(QWidget *parent = Q_NULLPTR)
        : QAbstractButton(parent)
    {
        // A caption is not part of the tab order, and clicking one must not take
        // the focus away from what the window is showing.
        setFocusPolicy(Qt::NoFocus);
        setCursor(Qt::ArrowCursor);
    }

    QSize sizeHint() const override
    {
        return QSize(kCaptionButtonWidth, kCaptionButtonHeight);
    }
protected:
    bool event(QEvent *e) override
    {
        // The wash is the whole answer to the pointer, so the button has to be
        // repainted when the pointer arrives or leaves rather than only when it
        // is clicked.
        switch (e->type()) {
        case QEvent::Enter:
        case QEvent::Leave:
        case QEvent::EnabledChange:
            update();
            break;
        default:
            break;
        }
        return QAbstractButton::event(e);
    }

    void paintEvent(QPaintEvent *) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);

        const QRect face =
            rect().adjusted(0, (height() - kCaptionButtonHeight) / 2, 0, -(height() - kCaptionButtonHeight) / 2);
        QColor wash;
        if (isDown()) {
            wash = closePressed();
        } else if (underMouse()) {
            wash = closeHover();
        }
        if (wash.alpha() > 0) {
            painter.setPen(Qt::NoPen);
            painter.setBrush(wash);
            painter.drawRoundedRect(face, 4, 4);
        }

        QColor glyph = palette().color(QPalette::WindowText);
        if (isDown() || underMouse()) {
            glyph = Qt::white;
        }
        const QPointF centre = QRectF(face).center();
        const qreal half = kGlyphSize / 2.0;
        painter.setPen(QPen(glyph, 1.2));
        painter.drawLine(QPointF(centre.x() - half, centre.y() - half), QPointF(centre.x() + half, centre.y() + half));
        painter.drawLine(QPointF(centre.x() - half, centre.y() + half), QPointF(centre.x() + half, centre.y() - half));
    }
};
}   // namespace

FluentMaterialWindow::FluentMaterialWindow(QWidget *parent)
    // A window of its own that the page owns: shown over the gallery, gone with
    // it, and never a child of the page it was opened from.
    : QWidget(parent, Qt::Window)
{
    setWindowTitle(tr("Window material"));
    setMinimumSize(380, 240);
    resize(520, 300);

    auto *column = new QVBoxLayout(this);
    column->setContentsMargins(0, 0, 0, 0);
    column->setSpacing(0);

    // The caption: the name, and the one button that closes the window.
    m_caption = new QWidget(this);
    m_caption->setFixedHeight(kCaptionHeight);
    m_caption->setAutoFillBackground(true);
    auto *captionLayout = new QHBoxLayout(m_caption);
    captionLayout->setContentsMargins(16, 0, 0, 0);
    captionLayout->setSpacing(0);
    auto *name = new QLabel(tr("Window material"), m_caption);
    QFont nameFont = name->font();
    nameFont.setWeight(QFont::DemiBold);
    name->setFont(nameFont);
    captionLayout->addWidget(name);
    captionLayout->addStretch(1);
    m_close = new CaptionCloseButton(m_caption);
    captionLayout->addWidget(m_close, 0, Qt::AlignVCenter);
    column->addWidget(m_caption);

    // The body says what this window is for, because a window with no system
    // frame and no menu bar has nothing else to explain itself with.
    auto *body = new QWidget(this);
    body->setAutoFillBackground(true);
    QPalette bodyPalette = body->palette();
    bodyPalette.setColor(QPalette::Window, palette().color(QPalette::Base));
    body->setPalette(bodyPalette);
    auto *bodyLayout = new QVBoxLayout(body);
    bodyLayout->setContentsMargins(16, 16, 16, 16);
    bodyLayout->setSpacing(12);
    auto *explanation = new QLabel(tr("This is the window a material is asked of: it draws a caption of its "
                                      "own, so the frame the window system takes away when a material goes on "
                                      "is not one it misses. Drag the caption to move it and an edge to "
                                      "resize it - the caption stays."),
                                   body);
    explanation->setWordWrap(true);
    bodyLayout->addWidget(explanation);
    m_material = new QLabel(tr("No material has been asked for."), body);
    m_material->setWordWrap(true);
    bodyLayout->addWidget(m_material);
    bodyLayout->addStretch(1);
    column->addWidget(body, 1);

    installWindowAgent();
}

void FluentMaterialWindow::setMaterialText(const QString &text)
{
    m_material->setText(text);
}

void FluentMaterialWindow::installWindowAgent()
{
    // The window takes itself over before any material is asked for. A material
    // finds the agent that is already here rather than making a second one -
    // two would each answer the hit test, and the one that knows nothing about
    // this caption would win.
    m_agent = new WindowAgentWidget(this);
    m_agent->setup(this);

    // The caption is the row above the body, so a press anywhere in it that is
    // not a system button starts a move.
    m_agent->setTitleBar(m_caption);
#ifndef Q_OS_MAC
    // macOS has no caption buttons the window places, so there is nothing to
    // name there; the button being hit test visible is what keeps it clickable.
    m_agent->setSystemButton(WindowAgentBase::Close, m_close);
#endif
    m_agent->setHitTestVisible(m_close, true);
    connect(m_close, &QAbstractButton::clicked, this, &QWidget::close);
}
