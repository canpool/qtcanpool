/**
 * Copyleft (C) 2025 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/

#include "ribbonscreentip.h"

#include <QApplication>
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

    // No hardcoded colours, but no palette(role) references either: see updateStyle().
    updateStyle();
}

void RibbonScreenTip::updateStyle()
{
    // A tip is a top-level window of its own, so it is painted from the application
    // palette. Naming a role (palette(tooltiptext)) inside a style sheet would look like
    // the same thing, but a style sheet resolves it once, when the widget is polished -
    // and this instance lives for the whole run, so "once" means "in whatever theme was
    // up the first time a tip appeared". After a switch to a dark theme the tip kept the
    // light theme's black text. The colours are resolved here instead, every time the
    // tip is about to be shown.
    const QPalette pal = QApplication::palette();
    const QString base = pal.color(QPalette::ToolTipBase).name();
    const QString text = pal.color(QPalette::ToolTipText).name();
    const QString edge = pal.color(QPalette::Mid).name();

    QString sheet = QStringLiteral("QFrame#RibbonScreenTip { background: %1; border: 1px solid %2; }"
                                   "QLabel { background: transparent; color: %3; }"
                                   "QLabel#RibbonScreenTipTitle { font-weight: bold; }"
                                   "QLabel#RibbonScreenTipShortcut { border: 1px solid %4; padding: 1px 4px; }");
    setStyleSheet(sheet.arg(base).arg(edge).arg(text).arg(edge));
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
    // The theme may have changed since the last tip, and the palette is the only thing
    // that says so for a window nobody re-polishes.
    tip->updateStyle();
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
