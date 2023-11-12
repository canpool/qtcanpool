/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#include "ribboncolorbutton.h"
#include "ribboncolorbutton_p.h"

#include <QPainter>
#include <QStyleOptionToolButton>
#include <QStyle>

QX_RIBBON_BEGIN_NAMESPACE

const int c_colorHeight = 5;   ///< 颜色块的高度

RibbonColorButtonPrivate::RibbonColorButtonPrivate()
    : RibbonButtonPrivate()
    , m_color(QColor())
{
}

QPixmap RibbonColorButtonPrivate::createIconPixmap(const QStyleOptionToolButton &opt, const QSize &iconsize)
{
    Q_Q(RibbonColorButton);
    if (opt.icon.isNull()) {
        return QPixmap();
    }
    QIcon::State state = opt.state & QStyle::State_On ? QIcon::On : QIcon::Off;
    QIcon::Mode mode;
    if (!(opt.state & QStyle::State_Enabled)) {
        mode = QIcon::Disabled;
    } else if ((opt.state & QStyle::State_MouseOver) && (opt.state & QStyle::State_AutoRaise)) {
        mode = QIcon::Active;
    } else {
        mode = QIcon::Normal;
    }
    QSize realIconSize = iconsize - QSize(0, c_colorHeight + 1);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QPixmap pixmap = opt.icon.pixmap(q->window()->windowHandle(), realIconSize, mode, state);
#else
    QPixmap pixmap = opt.icon.pixmap(realIconSize, q->window()->devicePixelRatio(), mode, state);
#endif
    QPixmap res(pixmap.size() + QSize(4, c_colorHeight + 4));   // 宽度上，颜色块多出2px
    res.fill(Qt::transparent);
    QPainter painter(&res);
    int xPixmap = (res.width() - pixmap.width()) / 2;
    int yPixmap = (res.height() - c_colorHeight - 2 - pixmap.height()) / 2; // 这里要减去2而不是1，这样奇数偶数都不会影响
    QRect rPixmap = QRect(xPixmap, yPixmap, pixmap.width(), pixmap.height());
    painter.drawPixmap(rPixmap, pixmap);
    QRect colorRect = rPixmap.adjusted(0, pixmap.height() + 1, 0, c_colorHeight + 1);
    if (m_color.isValid()) {
        painter.fillRect(colorRect, m_color);
    } else {
        QPen pen(Qt::red, 1, Qt::SolidLine, Qt::RoundCap);
        painter.setPen(pen);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
        painter.setRenderHint(QPainter::Antialiasing, true);
        int ss = colorRect.width() / 3;
        painter.drawLine(QPoint(colorRect.x() + ss, colorRect.bottom()),
                         QPoint(colorRect.right() - ss, colorRect.top()));
        pen.setColor(Qt::black);
        painter.setPen(pen);
        painter.drawRect(colorRect);
    }
    return res;
}

RibbonColorButton::RibbonColorButton(QWidget *parent)
    : RibbonButton(new RibbonColorButtonPrivate(), parent)
{
    Q_D(RibbonColorButton);
    d->setPublic(this);

    connect(this, &QAbstractButton::clicked, this, &RibbonColorButton::onButtonClicked);
}

RibbonColorButton::RibbonColorButton(QAction *defaultAction, QWidget *parent)
    : RibbonColorButton(parent)
{
    setDefaultAction(defaultAction);
}

RibbonColorButton::~RibbonColorButton()
{
}

const QColor &RibbonColorButton::color() const
{
    Q_D(const RibbonColorButton);
    return d->m_color;
}

void RibbonColorButton::setColor(const QColor &color)
{
    Q_D(RibbonColorButton);
    if (d->m_color == color) {
        return;
    }
    d->m_color = color;
    update();
    emit colorChanged(color);
}

void RibbonColorButton::onButtonClicked(bool checked)
{
    Q_D(RibbonColorButton);
    emit colorClicked(d->m_color, checked);
}

QX_RIBBON_END_NAMESPACE
