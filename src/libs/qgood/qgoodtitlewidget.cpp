/**
 * Copyright (C) 2023 maminjie <canpool@163.com> (https://github.com/canpool)
 * Copyright (C) 2018-2023 Antonio Dias (https://github.com/antonypro)
 * SPDX-License-Identifier: MIT
 **/

#include "qgoodtitlewidget.h"
#include "qgoodtitlebar.h"

#include <QPainter>
#include <QApplication>

QGoodTitleWidget::QGoodTitleWidget(QGoodTitleBar *titleBar, QWidget *parent) : QWidget(parent)
{
    m_titleBar = titleBar;

    m_active = false;
    m_alignment = Qt::AlignLeft;
}

void QGoodTitleWidget::setText(const QString &text)
{
    m_title = text;
    update();
}

void QGoodTitleWidget::setActive(bool active)
{
    m_active = active;
    update();
}

void QGoodTitleWidget::setTitleAlignment(const Qt::Alignment &alignment)
{
    switch (alignment) {
    case Qt::AlignLeft:
    case Qt::AlignCenter:
    case Qt::AlignRight: {
        m_alignment = alignment;
        break;
    }
    default: {
        m_alignment = Qt::AlignLeft;
        break;
    }
    }

    update();
}

void QGoodTitleWidget::setTitleColor(const QColor &activeColor, const QColor &inactiveColor)
{
    m_activeColor = activeColor;
    m_inactiveColor = inactiveColor;

    update();
}

Qt::Alignment QGoodTitleWidget::titleAlignment()
{
    return m_alignment;
}

void QGoodTitleWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    const int spacing = m_titleBar->layoutSpacing();

    QRect left_rect;
    QRect right_rect;
    QRect center_rect;

    if (m_titleBar->m_iconWidget->isVisible())
        left_rect = left_rect.united(m_titleBar->m_iconWidget->geometry());
    if (m_titleBar->m_leftWidgetPlaceHolder->isVisible())
        left_rect = left_rect.united(m_titleBar->m_leftWidgetPlaceHolder->geometry());

    if (m_titleBar->m_rightWidgetPlaceHolder->isVisible())
        right_rect = right_rect.united(m_titleBar->m_rightWidgetPlaceHolder->geometry());
    if (m_titleBar->m_captionButtons->isVisible())
        right_rect = right_rect.united(m_titleBar->m_captionButtons->geometry());
    else
        right_rect = right_rect.united(QRect(width(), 0, 1, height()));

    if (m_titleBar->m_centerWidgetPlaceHolder->isVisible())
        center_rect = center_rect.united(m_titleBar->m_centerWidgetPlaceHolder->geometry());

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);

    QFont font = this->font();
#ifdef Q_OS_WIN
    font.setFamily("Segoe UI");
#else
    font.setFamily(qApp->font().family());
#endif

    painter.setFont(font);

    QPen pen;
    pen.setColor(m_active ? m_activeColor : m_inactiveColor);

    painter.setPen(pen);

    bool center_widget_visible = m_titleBar->m_centerWidgetPlaceHolder->isVisible();

    Qt::Alignment alignment = m_alignment;

    if (center_widget_visible && alignment == Qt::AlignCenter)
        alignment = Qt::AlignLeft;

    QFontMetrics metrics(painter.font());

    int title_space_width;
    if (center_widget_visible) {
        if (alignment == Qt::AlignLeft)
            title_space_width = center_rect.left() - left_rect.right();
        else
            title_space_width = right_rect.left() - center_rect.right();
    } else {
        title_space_width = right_rect.left() - left_rect.right();
    }

    QString title_elided = metrics.elidedText(m_title, Qt::ElideRight, title_space_width - spacing);
    QSize title_size = metrics.size(0, title_elided);
    int title_width = title_size.width();
    int title_height = title_size.height();
    QRect title_rect = QRect(0, 0, title_width, title_height);

    switch (alignment) {
    case Qt::AlignLeft: {
        title_rect.setTop((height() - title_height) / 2);
        title_rect.setHeight(title_height);
        title_rect.setLeft(left_rect.right());
        title_rect.setRight(left_rect.right() + title_width);
        break;
    }
    case Qt::AlignRight: {
        title_rect.setTop((height() - title_height) / 2);
        title_rect.setHeight(title_height);
        title_rect.setLeft(right_rect.left() - title_width - spacing);
        title_rect.setRight(right_rect.left());
        break;
    }
    case Qt::AlignCenter: {
        title_rect.setWidth(title_width + spacing);
        title_rect.setHeight(title_height);
        title_rect.moveTop((height() - title_height) / 2);
        title_rect.moveLeft((width() - title_width) / 2);

        bool left_collide = (title_rect.left() < left_rect.right());
        bool right_collide = (title_rect.right() > right_rect.left());

        if (left_collide || (m_title != title_elided)) {
            title_rect.moveLeft(left_rect.right());
        } else if (right_collide) {
            title_rect.moveRight(right_rect.left());
        }
        break;
    }
    default:
        break;
    }

    painter.drawText(title_rect, title_elided);
}
