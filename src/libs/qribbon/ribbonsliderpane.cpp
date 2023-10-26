/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 **/
#include "ribbonsliderpane.h"
#include "ribbonstyle.h"
#include <QBasicTimer>
#include <QEvent>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QPushButton>
#include <QStyleOption>

QRIBBON_USE_NAMESPACE

QRIBBON_BEGIN_NAMESPACE

class RibbonSliderButton : public QPushButton
{
public:
    RibbonSliderButton(QWidget *parent, QStyle::PrimitiveElement typeBut = QStyle::PE_IndicatorArrowUp);
    ~RibbonSliderButton();
protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual QSize sizeHint() const;
protected:
    QStyle::PrimitiveElement m_typeBut;
private:
    Q_DISABLE_COPY(RibbonSliderButton)
};

class RibbonSliderPanePrivate : QObject
{
public:
    QRN_DECLARE_PUBLIC(RibbonSliderPane)
public:
    explicit RibbonSliderPanePrivate();
public:
    void initSlider();
public:
    QSlider *m_slider;
    RibbonSliderButton *m_buttonUp;
    RibbonSliderButton *m_buttonDown;
    QHBoxLayout *m_layout;
};

QRIBBON_END_NAMESPACE

/* RibbonSliderButton */
RibbonSliderButton::RibbonSliderButton(QWidget *parent, QStyle::PrimitiveElement typeBut) : QPushButton(parent)
{
    m_typeBut = typeBut;
}

RibbonSliderButton::~RibbonSliderButton() {}

void RibbonSliderButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    QStyleOptionButton option;
    initStyleOption(&option);
    option.direction = m_typeBut == QStyle::PE_IndicatorArrowUp ? Qt::LeftToRight : Qt::RightToLeft;
    style()->drawPrimitive((QStyle::PrimitiveElement)RibbonStyle::PE_RibbonSliderButton, &option, &painter, this);
}

QSize RibbonSliderButton::sizeHint() const
{
    QSize sz(12, 12);
    QStyleOptionButton option;
    initStyleOption(&option);
    option.direction = m_typeBut == QStyle::PE_IndicatorArrowUp ? Qt::LeftToRight : Qt::RightToLeft;
    sz = style()->sizeFromContents((QStyle::ContentsType)RibbonStyle::CT_RibbonSliderButton, &option, sz, this);
    return sz;
}

/* RibbonSliderPanePrivate */
RibbonSliderPanePrivate::RibbonSliderPanePrivate()
    : m_slider(Q_NULL), m_buttonUp(Q_NULL), m_buttonDown(Q_NULL), m_layout(Q_NULL)
{
}

void RibbonSliderPanePrivate::initSlider()
{
    Q_Q(RibbonSliderPane);
    m_slider = new QSlider(Qt::Horizontal, q);
    m_slider->setTickPosition(QSlider::NoTicks);

    m_layout = new QHBoxLayout();
    m_layout->setMargin(0);
    m_layout->setSpacing(4);

    m_layout->addWidget(m_slider);
    q->setLayout(m_layout);
}

/* RibbonSliderPane */
RibbonSliderPane::RibbonSliderPane(QWidget *parent) : QWidget(parent)
{
    QRN_INIT_PRIVATE(RibbonSliderPane);
    Q_D(RibbonSliderPane);
    d->initSlider();
    connect(d->m_slider, SIGNAL(valueChanged(int)), this, SIGNAL(valueChanged(int)));
    connect(d->m_slider, SIGNAL(sliderMoved(int)), this, SIGNAL(sliderMoved(int)));
}

RibbonSliderPane::~RibbonSliderPane() { QRN_FINI_PRIVATE(); }

void RibbonSliderPane::setRange(int min, int max)
{
    Q_D(RibbonSliderPane);
    d->m_slider->setMinimum(min);   // min - Range
    d->m_slider->setMaximum(max);   // max - Range
}

void RibbonSliderPane::setScrollButtons(bool on)
{
    Q_D(RibbonSliderPane);
    if (on) {
        d->m_buttonDown = new RibbonSliderButton(this, QStyle::PE_IndicatorArrowDown);
        d->m_layout->insertWidget(0, d->m_buttonDown, 0, Qt::AlignLeft);
        d->m_buttonUp = new RibbonSliderButton(this, QStyle::PE_IndicatorArrowUp);
        d->m_layout->addWidget(d->m_buttonUp, 0, Qt::AlignRight);
        connect(d->m_buttonDown, SIGNAL(pressed()), this, SLOT(decrement()));
        connect(d->m_buttonUp, SIGNAL(pressed()), this, SLOT(increment()));
    } else {
        disconnect(d->m_buttonUp, SIGNAL(pressed()), this, SLOT(increment()));
        disconnect(d->m_buttonDown, SIGNAL(pressed()), this, SLOT(decrement()));
        delete d->m_buttonUp;
        d->m_buttonUp = NULL;
        delete d->m_buttonDown;
        d->m_buttonDown = NULL;
    }
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

void RibbonSliderPane::setSingleStep(int nPos)
{
    Q_D(RibbonSliderPane);
    d->m_slider->setSingleStep(nPos);
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

void RibbonSliderPane::increment()
{
    Q_D(RibbonSliderPane);
    int nPos = d->m_slider->sliderPosition() + d->m_slider->singleStep();
    if (nPos <= d->m_slider->maximum())
        d->m_slider->setSliderPosition(nPos);
}

void RibbonSliderPane::decrement()
{
    Q_D(RibbonSliderPane);
    int nPos = d->m_slider->sliderPosition() - d->m_slider->singleStep();
    if (nPos >= d->m_slider->minimum())
        d->m_slider->setSliderPosition(nPos);
}
