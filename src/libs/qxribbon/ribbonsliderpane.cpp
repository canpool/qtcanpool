/**
 * Copyleft (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#include "ribbonsliderpane.h"
#include "ribbonsliderpane_p.h"

#include <QEvent>
#include <QHBoxLayout>
#include <QPainter>
#include <QPen>
#include <QSlider>

QX_RIBBON_BEGIN_NAMESPACE

// The scroll buttons draw their own glyphs: two-pixel strokes on a 16x16 canvas,
// in the palette's window text colour, so the buttons read on any theme without a
// theme carrying icons for them.
static QPixmap sliderPaneGlyph(bool plus, const QColor &color)
{
    QPixmap pixmap(16, 16);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setPen(QPen(color, 2));
    painter.drawLine(3, 8, 12, 8);
    if (plus)
        painter.drawLine(8, 3, 8, 12);
    return pixmap;
}

RibbonSliderPanePrivate::RibbonSliderPanePrivate()
    : m_minusButton(nullptr)
    , m_slider(nullptr)
    , m_plusButton(nullptr)
{
}

void RibbonSliderPanePrivate::init()
{
    Q_Q(RibbonSliderPane);
    q->setObjectName(QStringLiteral("qx_RibbonSliderPane"));

    QHBoxLayout *layout = new QHBoxLayout(q);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(2);

    m_minusButton = new QToolButton(q);
    m_minusButton->setObjectName(QStringLiteral("qx_RibbonSliderPaneButton"));
    m_minusButton->setAutoRaise(true);
    m_minusButton->setAutoRepeat(true);
    layout->addWidget(m_minusButton);

    m_slider = new QSlider(Qt::Horizontal, q);
    m_slider->setObjectName(QStringLiteral("qx_RibbonSliderPaneSlider"));
    m_slider->setRange(0, 100);
    m_slider->setPageStep(10);
    m_slider->setSingleStep(5);
    // A track that is too narrow makes a drag a jump; the host can always widen
    // the pane through its layout, this is only the floor.
    m_slider->setMinimumWidth(100);
    layout->addWidget(m_slider);

    m_plusButton = new QToolButton(q);
    m_plusButton->setObjectName(QStringLiteral("qx_RibbonSliderPaneButton"));
    m_plusButton->setAutoRaise(true);
    m_plusButton->setAutoRepeat(true);
    layout->addWidget(m_plusButton);

    updateButtonIcons();

    QObject::connect(m_minusButton, &QToolButton::clicked, q, &RibbonSliderPane::decrement);
    QObject::connect(m_plusButton, &QToolButton::clicked, q, &RibbonSliderPane::increment);
    // Signal-to-signal: the pane reports what the slider reports, and the host
    // never has to reach for the slider inside.
    QObject::connect(m_slider, &QSlider::valueChanged, q, &RibbonSliderPane::valueChanged);
    QObject::connect(m_slider, &QSlider::sliderMoved, q, &RibbonSliderPane::sliderMoved);
}

void RibbonSliderPanePrivate::updateButtonIcons()
{
    const QColor color = m_slider->palette().color(QPalette::WindowText);
    m_minusButton->setIcon(sliderPaneGlyph(false, color));
    m_plusButton->setIcon(sliderPaneGlyph(true, color));
}

RibbonSliderPane::RibbonSliderPane(QWidget *parent)
    : QWidget(parent)
{
    QX_INIT_PRIVATE(RibbonSliderPane);
    Q_D(RibbonSliderPane);
    d->init();
}

RibbonSliderPane::~RibbonSliderPane()
{
    QX_FINI_PRIVATE();
}

void RibbonSliderPane::setRange(int min, int max)
{
    Q_D(RibbonSliderPane);
    d->m_slider->setRange(min, max);
}

void RibbonSliderPane::setScrollButtons(bool on)
{
    Q_D(RibbonSliderPane);
    d->m_minusButton->setVisible(on);
    d->m_plusButton->setVisible(on);
}

void RibbonSliderPane::setSliderPosition(int position)
{
    Q_D(RibbonSliderPane);
    d->m_slider->setSliderPosition(position);
}

int RibbonSliderPane::sliderPosition() const
{
    Q_D(const RibbonSliderPane);
    return d->m_slider->sliderPosition();
}

void RibbonSliderPane::setSingleStep(int step)
{
    Q_D(RibbonSliderPane);
    d->m_slider->setSingleStep(step);
}

int RibbonSliderPane::singleStep() const
{
    Q_D(const RibbonSliderPane);
    return d->m_slider->singleStep();
}

int RibbonSliderPane::value() const
{
    Q_D(const RibbonSliderPane);
    return d->m_slider->value();
}

void RibbonSliderPane::setValue(int value)
{
    Q_D(RibbonSliderPane);
    d->m_slider->setValue(value);
}

void RibbonSliderPane::decrement()
{
    Q_D(RibbonSliderPane);
    d->m_slider->setValue(d->m_slider->value() - d->m_slider->singleStep());
}

void RibbonSliderPane::increment()
{
    Q_D(RibbonSliderPane);
    d->m_slider->setValue(d->m_slider->value() + d->m_slider->singleStep());
}

void RibbonSliderPane::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);
    if (event->type() == QEvent::PaletteChange) {
        Q_D(RibbonSliderPane);
        d->updateButtonIcons();
    }
}

QX_RIBBON_END_NAMESPACE
