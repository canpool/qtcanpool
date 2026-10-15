/**
 * Copyleft (C) 2025 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#include "ribbonkeytip.h"

#include <QPainter>

QX_RIBBON_BEGIN_NAMESPACE

RibbonKeyTip::RibbonKeyTip(QWidget *parent)
    : QWidget(parent)
    , m_target(Q_NULLPTR)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_NoSystemBackground);
}

void RibbonKeyTip::setKey(const QString &key)
{
    if (m_key != key) {
        m_key = key;
        update();
    }
}

QString RibbonKeyTip::key() const
{
    return m_key;
}

void RibbonKeyTip::setTarget(QWidget *target)
{
    m_target = target;
}

QWidget *RibbonKeyTip::target() const
{
    return m_target;
}

QSize RibbonKeyTip::sizeHint() const
{
    return QSize(20, 18);
}

void RibbonKeyTip::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    const QRect r = rect();
    // 角标底色（深底浅字，贴近 Office 风格）
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(45, 45, 48));
    p.drawRoundedRect(r.adjusted(0, 0, -1, -1), 3, 3);
    // 边框
    p.setPen(QColor(0, 120, 215));
    p.drawRoundedRect(r.adjusted(0, 0, -1, -1), 3, 3);
    // 字母
    p.setPen(Qt::white);
    p.setBrush(Qt::NoBrush);
    p.drawText(r, Qt::AlignCenter, m_key);
}

QX_RIBBON_END_NAMESPACE
