/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "dockfloatingtitlebar.h"
#include "dockfloatingcontainer.h"
#include "docklabel.h"
#include "dockutils.h"

#include <QToolButton>
#include <QBoxLayout>
#include <QStyle>

QX_DOCK_BEGIN_NAMESPACE

class DockFloatingTitleBarPrivate
{
public:
    QX_DECLARE_PUBLIC(DockFloatingTitleBar)
public:
    DockFloatingTitleBarPrivate();

    void init();
public:
    DockFloatingContainer *m_floatingWidget = nullptr;
    DockLabel *m_titleLabel;
    QToolButton *m_closeButton = nullptr;
    QToolButton *m_maximizeButton = nullptr;
    Qx::DockDragState m_dragState = Qx::DockDraggingInactive;
    QIcon m_maximizeIcon;
    QIcon m_normalIcon;
    bool m_maximized = false;
};

DockFloatingTitleBarPrivate::DockFloatingTitleBarPrivate()
{
}

void DockFloatingTitleBarPrivate::init()
{
    Q_Q(DockFloatingTitleBar);

    m_titleLabel = new DockLabel();
    m_titleLabel->setElideMode(Qt::ElideRight);
    m_titleLabel->setText("DockWidget->windowTitle()");
    m_titleLabel->setObjectName("dockFloatingTitleLabel");
    m_titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    m_closeButton = new QToolButton();
    m_closeButton->setObjectName("dockFloatingTitleCloseButton");
    m_closeButton->setAutoRaise(true);

    m_maximizeButton = new QToolButton();
    m_maximizeButton->setObjectName("dockFloatingTitleMaximizeButton");
    m_maximizeButton->setAutoRaise(true);

    // The standard icons do does not look good on high DPI screens
    QIcon closeIcon;
    QPixmap normalPixmap = q->style()->standardPixmap(QStyle::SP_TitleBarCloseButton, 0, m_closeButton);
    closeIcon.addPixmap(normalPixmap, QIcon::Normal);
    closeIcon.addPixmap(internal::createTransparentPixmap(normalPixmap, 0.25), QIcon::Disabled);
    m_closeButton->setIcon(q->style()->standardIcon(QStyle::SP_TitleBarCloseButton));
    m_closeButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_closeButton->setVisible(true);
    m_closeButton->setFocusPolicy(Qt::NoFocus);
    q->connect(m_closeButton, SIGNAL(clicked()), SIGNAL(closeRequested()));

    q->setMaximizedIcon(false);
    m_maximizeButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_maximizeButton->setVisible(true);
    m_maximizeButton->setFocusPolicy(Qt::NoFocus);
    q->connect(m_maximizeButton, &QToolButton::clicked, q, &DockFloatingTitleBar::maximizeRequested);

    QFontMetrics fm(m_titleLabel->font());
    int spacing = qRound(fm.height() / 4.0);

    // Fill the layout
    QBoxLayout *lay = new QBoxLayout(QBoxLayout::LeftToRight);
    lay->setContentsMargins(6, 0, 0, 0);
    lay->setSpacing(0);
    q->setLayout(lay);
    lay->addWidget(m_titleLabel, 1);
    lay->addSpacing(spacing);
    lay->addWidget(m_maximizeButton);
    lay->addWidget(m_closeButton);
    lay->setAlignment(Qt::AlignCenter);

    m_titleLabel->setVisible(true);
}

DockFloatingTitleBar::DockFloatingTitleBar(DockFloatingContainer *parent)
    : QFrame(parent)
{
    QX_INIT_PRIVATE(DockFloatingTitleBar);
    Q_D(DockFloatingTitleBar);

    d->m_floatingWidget = parent;
    d->init();

    auto normalPixmap = this->style()->standardPixmap(QStyle::SP_TitleBarNormalButton, nullptr, d->m_maximizeButton);
    d->m_normalIcon.addPixmap(normalPixmap, QIcon::Normal);
    d->m_normalIcon.addPixmap(internal::createTransparentPixmap(normalPixmap, 0.25), QIcon::Disabled);

    auto maxPixmap = this->style()->standardPixmap(QStyle::SP_TitleBarMaxButton, nullptr, d->m_maximizeButton);
    d->m_maximizeIcon.addPixmap(maxPixmap, QIcon::Normal);
    d->m_maximizeIcon.addPixmap(internal::createTransparentPixmap(maxPixmap, 0.25), QIcon::Disabled);
    setMaximizedIcon(d->m_maximized);
}

DockFloatingTitleBar::~DockFloatingTitleBar()
{
    QX_FINI_PRIVATE();
}

void DockFloatingTitleBar::enableCloseButton(bool enable)
{
    Q_D(DockFloatingTitleBar);
    d->m_closeButton->setEnabled(enable);
}

void DockFloatingTitleBar::setTitle(const QString &text)
{
    Q_D(DockFloatingTitleBar);
    d->m_titleLabel->setText(text);
}

void DockFloatingTitleBar::updateStyle()
{
    internal::repolishStyle(this, internal::RepolishDirectChildren);
}

void DockFloatingTitleBar::setMaximizedIcon(bool maximized)
{
    Q_D(DockFloatingTitleBar);
    d->m_maximized = maximized;
    if (maximized) {
        d->m_maximizeButton->setIcon(d->m_normalIcon);
    } else {
        d->m_maximizeButton->setIcon(d->m_maximizeIcon);
    }
}

QIcon DockFloatingTitleBar::maximizeIcon() const
{
    Q_D(const DockFloatingTitleBar);
    return d->m_maximizeIcon;
}

void DockFloatingTitleBar::setMaximizeIcon(const QIcon &icon)
{
    Q_D(DockFloatingTitleBar);
    d->m_maximizeIcon = icon;
    if (d->m_maximized) {
        setMaximizedIcon(d->m_maximized);
    }
}

QIcon DockFloatingTitleBar::normalIcon() const
{
    Q_D(const DockFloatingTitleBar);
    return d->m_normalIcon;
}

void DockFloatingTitleBar::setNormalIcon(const QIcon &icon)
{
    Q_D(DockFloatingTitleBar);
    d->m_normalIcon = icon;
    if (!d->m_maximized) {
        setMaximizedIcon(d->m_maximized);
    }
}

void DockFloatingTitleBar::mousePressEvent(QMouseEvent *e)
{
    Q_D(DockFloatingTitleBar);
    if (e->button() == Qt::LeftButton) {
        d->m_dragState = Qx::DockDraggingFloatingWidget;
        d->m_floatingWidget->startFloating(e->pos(), d->m_floatingWidget->size(), Qx::DockDraggingFloatingWidget, this);
        return;
    }
    Super::mousePressEvent(e);
}

void DockFloatingTitleBar::mouseReleaseEvent(QMouseEvent *e)
{
    Q_D(DockFloatingTitleBar);
    d->m_dragState = Qx::DockDraggingInactive;
    if (d->m_floatingWidget) {
        d->m_floatingWidget->finishDragging();
    }
    Super::mouseReleaseEvent(e);
}

void DockFloatingTitleBar::mouseMoveEvent(QMouseEvent *e)
{
    Q_D(DockFloatingTitleBar);
    if (!(e->buttons() & Qt::LeftButton) || Qx::DockDraggingInactive == d->m_dragState) {
        d->m_dragState = Qx::DockDraggingInactive;
        Super::mouseMoveEvent(e);
        return;
    }

    // move floating window
    if (Qx::DockDraggingFloatingWidget == d->m_dragState) {
        if (d->m_floatingWidget->isMaximized()) {
            d->m_floatingWidget->showNormal(true);
        }
        d->m_floatingWidget->moveFloating();
        Super::mouseMoveEvent(e);
        return;
    }
    Super::mouseMoveEvent(e);
}

void DockFloatingTitleBar::mouseDoubleClickEvent(QMouseEvent *e)
{
    if (e->buttons() & Qt::LeftButton) {
        Q_EMIT maximizeRequested();
        e->accept();
    } else {
        QWidget::mouseDoubleClickEvent(e);
    }
}

QX_DOCK_END_NAMESPACE
