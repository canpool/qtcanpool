/**
 * Copyleft (C) 2025 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/

#include "ribbonscreentip.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QScreen>
#include <QVBoxLayout>

QX_RIBBON_BEGIN_NAMESPACE

RibbonScreenTip::RibbonScreenTip()
    : QFrame(Q_NULLPTR)
{
    setObjectName(QStringLiteral("RibbonScreenTip"));
    setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_ShowWithoutActivating);
    setFrameShape(QFrame::NoFrame);

    m_titleLabel = new QLabel(this);
    m_titleLabel->setObjectName(QStringLiteral("RibbonScreenTipTitle"));
    m_descriptionLabel = new QLabel(this);
    m_descriptionLabel->setObjectName(QStringLiteral("RibbonScreenTipDescription"));
    m_descriptionLabel->setWordWrap(true);
    m_shortcutLabel = new QLabel(this);
    m_shortcutLabel->setObjectName(QStringLiteral("RibbonScreenTipShortcut"));

    auto *shortcutRow = new QHBoxLayout();
    shortcutRow->setContentsMargins(0, 0, 0, 0);
    shortcutRow->addStretch();
    shortcutRow->addWidget(m_shortcutLabel);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 6, 8, 6);
    layout->setSpacing(4);
    layout->addWidget(m_titleLabel);
    layout->addWidget(m_descriptionLabel);
    layout->addLayout(shortcutRow);

    setStyleSheet(QStringLiteral("QFrame#RibbonScreenTip { background: #fafafa; border: 1px solid #adadad; }"
                                 "QLabel { background: transparent; }"
                                 "QLabel#RibbonScreenTipTitle { font-weight: bold; color: #202020; }"
                                 "QLabel#RibbonScreenTipDescription { color: #404040; }"
                                 "QLabel#RibbonScreenTipShortcut { color: #505050; border: 1px solid #c0c0c0; "
                                 "background: #ffffff; padding: 1px 4px; }"));
}

QString RibbonScreenTip::title() const
{
    return m_titleLabel->text();
}

QString RibbonScreenTip::description() const
{
    return m_descriptionLabel->text();
}

QString RibbonScreenTip::shortcut() const
{
    return m_shortcutLabel->text();
}

/**
 * @brief The shared tip instance. It lives for the whole run, like the QToolTip one.
 */
RibbonScreenTip *RibbonScreenTip::instance()
{
    static RibbonScreenTip *s_tip = Q_NULLPTR;
    if (!s_tip) {
        s_tip = new RibbonScreenTip();
    }
    return s_tip;
}

void RibbonScreenTip::setContent(const QString &title, const QString &description, const QString &shortcut)
{
    m_titleLabel->setText(title);
    m_descriptionLabel->setText(description);
    m_shortcutLabel->setText(shortcut);

    m_titleLabel->setVisible(!title.isEmpty());
    m_descriptionLabel->setVisible(!description.isEmpty());
    m_shortcutLabel->setVisible(!shortcut.isEmpty());
}

void RibbonScreenTip::showTip(QWidget *anchor, const QString &title, const QString &description,
                              const QString &shortcut)
{
    if (!anchor) {
        return;
    }
    RibbonScreenTip *tip = instance();
    tip->setContent(title, description, shortcut);
    tip->adjustSize();

    const QRect anchorRect(anchor->mapToGlobal(QPoint(0, 0)), anchor->size());
    QPoint pos(anchorRect.left(), anchorRect.bottom() + 2);
    if (QScreen *screen = anchor->screen()) {
        const QRect available = screen->availableGeometry();
        const QSize size = tip->size();
        // Keep the tip on screen: clamp horizontally, and flip it above the anchor
        // when there is no room below.
        pos.setX(qBound(available.left(), pos.x(), qMax(available.left(), available.right() - size.width() + 1)));
        if (pos.y() + size.height() > available.bottom()) {
            pos.setY(anchorRect.top() - size.height() - 2);
        }
        pos.setY(qMax(available.top(), pos.y()));
    }
    tip->move(pos);
    tip->show();
    tip->raise();
}

void RibbonScreenTip::hideTip()
{
    RibbonScreenTip *tip = instance();
    if (tip->isVisible()) {
        tip->hide();
    }
}

QX_RIBBON_END_NAMESPACE
