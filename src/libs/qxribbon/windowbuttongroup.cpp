/**
 * Copyleft (C) 2023-2025 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#include "windowbuttongroup.h"
#include "windowbuttongroup_p.h"
#include "ribbonmanager.h"

#include <QResizeEvent>
#include <QScopedPointer>
#include <QStyle>
#include <QToolButton>
#include <QHBoxLayout>

QX_RIBBON_BEGIN_NAMESPACE

#define MIN_BUTTON_WIDTH    30
#define MAX_BUTTON_WIDTH    30
#define CLS_BUTTON_WIDTH    40

WindowButton::WindowButton(QWidget *p)
    : QToolButton(p)
{
    setAutoRaise(true);
}


WindowButtonGroupPrivate::WindowButtonGroupPrivate()
    : m_closeButton(Q_NULLPTR)
    , m_minimizeButton(Q_NULLPTR)
    , m_maximizeButton(Q_NULLPTR)
    , m_iconScale(0.5)
    , m_signalEnabled(false)
{
}

void WindowButtonGroupPrivate::init()
{
    Q_Q(WindowButtonGroup);

    m_minimizeButton = new WindowButton(q);
    m_minimizeButton->setObjectName(QStringLiteral("qx_MinimizeWindowButton"));
    m_minimizeButton->setFixedWidth(MIN_BUTTON_WIDTH);
    m_minimizeButton->setFocusPolicy(Qt::NoFocus);
    m_minimizeButton->setIconSize(m_minimizeButton->size() * m_iconScale);
    m_minimizeButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    connect(m_minimizeButton, &QAbstractButton::clicked, this, &WindowButtonGroupPrivate::buttonClicked);

    m_maximizeButton = new WindowButton(q);
    m_maximizeButton->setObjectName(QStringLiteral("qx_MaximizeWindowButton"));
    m_maximizeButton->setFixedWidth(MAX_BUTTON_WIDTH);
    m_maximizeButton->setCheckable(true);
    m_maximizeButton->setFocusPolicy(Qt::NoFocus);
    m_maximizeButton->setIconSize(m_maximizeButton->size() * m_iconScale);
    m_maximizeButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    connect(m_maximizeButton, &QAbstractButton::clicked, this, &WindowButtonGroupPrivate::buttonClicked);

    m_closeButton = new WindowButton(q);
    m_closeButton->setObjectName(QStringLiteral("qx_CloseWindowButton"));
    m_closeButton->setFixedWidth(CLS_BUTTON_WIDTH);
    m_closeButton->setFocusPolicy(Qt::NoFocus);
    m_closeButton->setIconSize(m_closeButton->size() * m_iconScale);
    m_closeButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    connect(m_closeButton, &QAbstractButton::clicked, this, &WindowButtonGroupPrivate::buttonClicked);

    QHBoxLayout *lay = new QHBoxLayout(q);
    lay->setSpacing(0);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->addWidget(m_minimizeButton);
    lay->addWidget(m_maximizeButton);
    lay->addWidget(m_closeButton);
}

void WindowButtonGroupPrivate::updateButton(Qt::WindowFlags flags)
{
    m_minimizeButton->setVisible(flags & Qt::WindowMinimizeButtonHint);
    m_maximizeButton->setVisible(flags & Qt::WindowMaximizeButtonHint);
    m_closeButton->setVisible(flags & Qt::WindowCloseButtonHint);
}

void WindowButtonGroupPrivate::updateButtonIconSize()
{
    m_minimizeButton->setIconSize(m_minimizeButton->size() * m_iconScale);
    m_maximizeButton->setIconSize(m_maximizeButton->size() * m_iconScale);
    m_closeButton->setIconSize(m_closeButton->size() * m_iconScale);
}

QSize WindowButtonGroupPrivate::sizeHint() const
{
    int width = 0;
    int height = groupHeight();

    if (m_closeButton->isVisible()) {
        width += CLS_BUTTON_WIDTH;
    }
    if (m_maximizeButton->isVisible()) {
        width += MAX_BUTTON_WIDTH;
    }
    if (m_minimizeButton->isVisible()) {
        width += MIN_BUTTON_WIDTH;
    }
    return QSize(width, height);
}

int WindowButtonGroupPrivate::groupHeight() const
{
    // In the WpsLiteStyle style, tabBarHeight may be smaller than titleBarHeight,
    // which causes the window buttons to cover the ribbon area, so use the smallest one
    return qMin(RibbonElementStyleOpt.titleBarHeight(), RibbonElementStyleOpt.tabBarHeight());
}

void WindowButtonGroupPrivate::buttonClicked()
{
    Q_Q(WindowButtonGroup);
    WindowButton *button = qobject_cast<WindowButton *>(sender());

    if (m_signalEnabled) {
        if (button == m_minimizeButton) {
            Q_EMIT q->buttonMinimizeClicked();
        } else if (button == m_maximizeButton) {
            Q_EMIT q->buttonMaximzieClicked();
        } else if (button == m_closeButton) {
            Q_EMIT q->buttonCloseClicked();
        }
    } else {
        QWidget *pw = q->parentWidget();
        if (pw) {
            if (button == m_minimizeButton) {
                pw->showMinimized();
            } else if (button == m_maximizeButton) {
                if (pw->isMaximized()) {
                    pw->showNormal();
                } else {
                    pw->showMaximized();
                }
            } else if (button == m_closeButton) {
                pw->close();
            }
        }
    }
}

WindowButtonGroup::WindowButtonGroup(QWidget *parent, Qt::WindowFlags flags)
    : QWidget(parent)
{
    QX_INIT_PRIVATE(WindowButtonGroup)
    Q_D(WindowButtonGroup);
    d->init();
    updateWindowFlags(flags);
}

WindowButtonGroup::~WindowButtonGroup()
{
    if (this->parent()) {
        this->parent()->removeEventFilter(this);
    }
    QX_FINI_PRIVATE()
}

void WindowButtonGroup::updateWindowFlags(Qt::WindowFlags flags)
{
    Q_D(WindowButtonGroup);
    if (flags == Qt::WindowFlags()) {
        if (QWidget *pw = parentWidget()) {
            flags = pw->windowFlags();
        }
    }
    d->updateButton(flags);
}

void WindowButtonGroup::setIconScale(qreal scale)
{
    Q_D(WindowButtonGroup);
    if (d->m_iconScale == scale) {
        return;
    }
    d->m_iconScale = scale;
    d->updateButtonIconSize();
}

void WindowButtonGroup::setWindowStates(Qt::WindowStates s)
{
    Q_D(WindowButtonGroup);
    bool on = s.testFlag(Qt::WindowMaximized);
    d->m_maximizeButton->setChecked(on);
    d->m_maximizeButton->setToolTip(on ? tr("Restore") : tr("Maximize"));
}

QSize WindowButtonGroup::sizeHint() const
{
    Q_D(const WindowButtonGroup);
    return d->sizeHint();
}

QWidget *WindowButtonGroup::widgetForMaximizeButton() const
{
    Q_D(const WindowButtonGroup);
    return d->m_maximizeButton;
}

/**
 * If the signal is disabled, the window button's signal
 * will be directly applied to the parent window; otherwise,
 * only the button signal will be sent
 */
bool WindowButtonGroup::signalIsEnabled() const
{
    Q_D(const WindowButtonGroup);
    return d->m_signalEnabled;
}

void WindowButtonGroup::setSignalEnabled(bool enable)
{
    Q_D(WindowButtonGroup);
    d->m_signalEnabled = enable;
}

QX_RIBBON_END_NAMESPACE
