/**
 * Copyright (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "windowlogo.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>

QX_WIDGET_BEGIN_NAMESPACE

class WindowLogoPrivate
{
    QX_DECLARE_PUBLIC(WindowLogo)
public:
    WindowLogoPrivate();
public:
    QPixmap m_pixmap;
    bool m_isRound;
    bool m_hasMenu;
};

WindowLogoPrivate::WindowLogoPrivate()
    : m_isRound(false)
    , m_hasMenu(false)
{

}

WindowLogo::WindowLogo(const QPixmap &pixmap, QWidget *parent)
    : QWidget{parent}
{
    QX_INIT_PRIVATE(WindowLogo);
    Q_D(WindowLogo);
    d->m_pixmap = pixmap;
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
}

WindowLogo::~WindowLogo()
{
    QX_FINI_PRIVATE()
}

void WindowLogo::setPixmap(const QPixmap &pixmap)
{
    Q_D(WindowLogo);
    d->m_pixmap = pixmap;
    update();
}

QPixmap WindowLogo::pixmap() const
{
    Q_D(const WindowLogo);
    return d->m_pixmap;
}

void WindowLogo::setHasMenu(bool has)
{
    Q_D(WindowLogo);
    d->m_hasMenu = has;
}

bool WindowLogo::hasMenu()
{
    Q_D(WindowLogo);
    return d->m_hasMenu;
}

void WindowLogo::setRound(bool round)
{
    Q_D(WindowLogo);
    d->m_isRound = round;
    update();
}

QSize WindowLogo::sizeHint() const
{
    int w = qMin(width(), height());
    return QSize(w, w);
}

QSize WindowLogo::minimumSizeHint() const
{
    return sizeHint();
}

void WindowLogo::mousePressEvent(QMouseEvent *event)
{
    Q_D(WindowLogo);
    if (event->button() == Qt::LeftButton) {
        if (d->m_hasMenu) {
            emit menuTriggered(event);
        }
    }
}

void WindowLogo::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        // don't emit when move out
        if (this->rect().contains(event->pos())) {
            emit clicked();
        }
    }
}

void WindowLogo::paintEvent(QPaintEvent *event)
{
    Q_D(WindowLogo);
    if (d->m_pixmap.isNull()) {
        QWidget::paintEvent(event);
    } else {
        QPainter painter(this);
        painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
        int di = qMin(width(), height());
        int r = di >> 1;
        QPoint center = rect().center();
        QRect rc(center - QPoint(r, r), QSize(di, di));
        if (d->m_isRound) {
            QPainterPath path;
            if (r > 1) r -= 1;
            path.addEllipse(center, r, r);
            painter.setClipPath(path);
        }
        painter.drawPixmap(rc, d->m_pixmap);
    }
}

QX_WIDGET_END_NAMESPACE
