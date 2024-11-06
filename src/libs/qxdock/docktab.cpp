/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "docktab.h"
#include "dockwidget.h"
#include "dockpanel.h"
#include "docklabel.h"
#include "dockmanager.h"
#include "dockutils.h"

#include <QBoxLayout>
#include <QToolButton>
#include <QPushButton>

QX_DOCK_BEGIN_NAMESPACE

class DockTabPrivate
{
public:
    QX_DECLARE_PUBLIC(DockTab)
public:
    DockTabPrivate();
    void init();

    bool testConfigFlag(DockManager::ConfigFlag flag) const;
    QAbstractButton *createCloseButton() const;
    void updateCloseButtonSizePolicy();
    void updateCloseButtonVisibility(bool active);
public:
    DockWidget *m_dockWidget = nullptr;
    DockPanel *m_panel = nullptr;
    DockLabel *m_label = nullptr;
    QAbstractButton *m_closeButton = nullptr;
    QIcon m_icon;
    bool m_isActive = false;
};

DockTabPrivate::DockTabPrivate()
{

}

void DockTabPrivate::init()
{
    Q_Q(DockTab);

    m_label = new DockLabel();
    m_label->setText(m_dockWidget->windowTitle());
    m_label->setObjectName("dockTabLabel");
    m_label->setAlignment(Qt::AlignCenter);

    m_closeButton = createCloseButton();
    m_closeButton->setObjectName("dockTabCloseButton");
    internal::setButtonIcon(m_closeButton, QStyle::SP_TitleBarCloseButton, Qx::DockTabCloseIcon);
    m_closeButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_closeButton->setFocusPolicy(Qt::NoFocus);
    updateCloseButtonSizePolicy();
    internal::setToolTip(m_closeButton, QObject::tr("Close Tab"));

    QFontMetrics fm(m_label->font());
    int spacing = qRound(fm.height() / 4.0);

    // Fill the layout
    QBoxLayout *layout = new QBoxLayout(QBoxLayout::LeftToRight);
    layout->setContentsMargins(2 * spacing, 0, 0, 0);
    layout->setSpacing(0);
    q->setLayout(layout);

    layout->addWidget(m_label, 1);
    layout->addSpacing(spacing);
    layout->addWidget(m_closeButton);
    layout->addSpacing(qRound(spacing * 4.0 / 3.0));
    layout->setAlignment(Qt::AlignCenter);

    m_label->setVisible(true);
}

bool DockTabPrivate::testConfigFlag(DockManager::ConfigFlag flag) const
{
    return DockManager::testConfigFlag(flag);
}

QAbstractButton *DockTabPrivate::createCloseButton() const
{
    if (testConfigFlag(DockManager::TabCloseButtonIsToolButton)) {
        auto b = new QToolButton();
        b->setAutoRaise(true);
        return b;
    } else {
        return new QPushButton();
    }
}

void DockTabPrivate::updateCloseButtonSizePolicy()
{
    auto features = m_dockWidget->features();
    auto sizePolicy = m_closeButton->sizePolicy();
    sizePolicy.setRetainSizeWhenHidden(features.testFlag(DockWidget::DockWidgetClosable) &&
                                       testConfigFlag(DockManager::RetainTabSizeWhenCloseButtonHidden));
    m_closeButton->setSizePolicy(sizePolicy);
}

void DockTabPrivate::updateCloseButtonVisibility(bool active)
{
    bool dockWidgetClosable = m_dockWidget->features().testFlag(DockWidget::DockWidgetClosable);
    bool activeTabHasCloseButton = testConfigFlag(DockManager::ActiveTabHasCloseButton);
    bool allTabsHaveCloseButton = testConfigFlag(DockManager::AllTabsHaveCloseButton);
    bool tabHasCloseButton = (activeTabHasCloseButton && active) | allTabsHaveCloseButton;
    m_closeButton->setVisible(dockWidgetClosable && tabHasCloseButton);
}

DockTab::DockTab(DockWidget *w, QWidget *parent)
    : QWidget(parent)
{
    QX_INIT_PRIVATE(DockTab);

    Q_D(DockTab);
    d->m_dockWidget = w;
    d->init();

    setAttribute(Qt::WA_NoMousePropagation, true);
    setFocusPolicy(Qt::NoFocus);
}

DockTab::~DockTab()
{
    QX_FINI_PRIVATE()
}

bool DockTab::isActive() const
{
    Q_D(const DockTab);
    return d->m_isActive;
}

void DockTab::setActive(bool active)
{
    Q_D(DockTab);
    d->updateCloseButtonVisibility(active);
    if (DockManager::testConfigFlag(DockManager::ShowTabTextOnlyForActiveTab) && !d->m_icon.isNull()) {
        if (active) {
            d->m_label->setVisible(true);
        } else {
            d->m_label->setVisible(false);
        }
    }
    // Focus related stuff
    if (DockManager::testConfigFlag(DockManager::FocusHighlighting)) {
        // TODO
    } else if (d->m_isActive == active) {
        return;
    }

    d->m_isActive = active;
    updateStyle();
    update();
    updateGeometry();
    Q_EMIT activeTabChanged();
}

void DockTab::setDockPanel(DockPanel *panel)
{
    Q_D(DockTab);
    d->m_panel = panel;
}

DockWidget *DockTab::dockWidget() const
{
    Q_D(const DockTab);
    return d->m_dockWidget;
}

const QIcon &DockTab::icon() const
{
    Q_D(const DockTab);
    return d->m_icon;
}

QString DockTab::text() const
{
    Q_D(const DockTab);
    return d->m_label->text();
}

void DockTab::updateStyle()
{
    internal::repolishStyle(this, internal::RepolishDirectChildren);
}

QX_DOCK_END_NAMESPACE
