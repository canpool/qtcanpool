/**
 * Copyright (C) 2020-2022 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#include "ribbongroup.h"
#include "ribbongroup_p.h"

#include <QVBoxLayout>
#include <QToolButton>
#include <QLabel>
#include <QToolBar>

QCANPOOL_BEGIN_NAMESPACE

/* RibbonBarInnerContainer */
RibbonBarInnerContainer::RibbonBarInnerContainer(RibbonGroup::GroupSize size, QWidget *parent)
    : QToolBar(parent)
{
    switch (size) {
        case RibbonGroup::GroupLarge: {
            setIconSize(QSize(32, 32));
            setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
            setOrientation(Qt::Horizontal);
        }
        break;
        case RibbonGroup::GroupMedium: {
            setIconSize(QSize(18, 18));
            setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            setOrientation(Qt::Vertical);
        }
        break;
        default: {
            setIconSize(QSize(18, 18));
            setToolButtonStyle(Qt::ToolButtonIconOnly);
            setOrientation(Qt::Vertical);
        }
        break;
    }
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
#ifdef Q_OS_LINUX
    layout()->setContentsMargins(0, 0, 0, 0);
    layout()->setSpacing(0);
    layout()->setSizeConstraint(QLayout::SetFixedSize);
#endif
}

RibbonBarInnerContainer::~RibbonBarInnerContainer()
{

}

/* RibbonGroupPrivate */
RibbonGroupPrivate::RibbonGroupPrivate()
    : m_largeContainer(nullptr)
    , m_mediumContainer(nullptr)
    , m_smallContainer(nullptr)
{
}

RibbonGroupPrivate::~RibbonGroupPrivate()
{
}

/**
 * group layout:
 *
 *   ,..Internal Layout ..,  ,.External Layout.,
 *  /                      \/                   \
 *  ---------------------------------------------
 *  | L1 L2 .. | M1   | S1 |  Ext ...           |
 *  |          | M2   | S2 |                    |
 *  |          | M3   | S3 |                    |
 *  |--------------------------------------------
 *  |               Title                   opt |
 *  ---------------------------------------------
 *
 * notes:
 *  1) Large container is Horizontal
 *  2) Medium and Small containers are Vertical
 *  3) External layout is open to users
 */
void RibbonGroupPrivate::init()
{
    m_titleLabel = new QLabel();
    m_titleLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    m_optionButton = new QToolButton();
    m_optionButton->setIcon(QIcon(":/res/group_option.png"));
    m_optionButton->setIconSize(QSize(10, 10));
    m_optionButton->setToolTip(tr("option"));
    m_optionButton->setObjectName(QLatin1String("option"));
    m_optionButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_optionButton->setVisible(false);
    connect(m_optionButton, SIGNAL(clicked()), q, SIGNAL(optionClicked()));

    m_titleLayout = new QHBoxLayout();
    m_titleLayout->setSpacing(0);
    m_titleLayout->setContentsMargins(0, 0, 1, 2);
    m_titleLayout->addStretch();
    m_titleLayout->addWidget(m_titleLabel);
    m_titleLayout->addStretch();
    m_titleLayout->addWidget(m_optionButton);

    QWidget *line = new QWidget();
    line->setFixedWidth(1);
    line->setAutoFillBackground(true);
    line->setBackgroundRole(QPalette::Mid);

    // internal controls layout
    m_inLayout = new QHBoxLayout();
    m_inLayout->setSpacing(0);
    m_inLayout->setContentsMargins(0, 0, 0, 0);
    // external controls layout
    m_extLayout = new QHBoxLayout();
    m_extLayout->setSpacing(0);
    m_extLayout->setContentsMargins(0, 0, 0, 0);
    QHBoxLayout *controlLayout = new QHBoxLayout();
    controlLayout->setSpacing(0);
    controlLayout->setContentsMargins(0, 0, 0, 0);
    controlLayout->addLayout(m_inLayout);
    controlLayout->addLayout(m_extLayout);

    QWidget *container = new QWidget();
    container->setLayout(controlLayout);

    QVBoxLayout *bodyLayout = new QVBoxLayout();
    bodyLayout->setSpacing(0);
    bodyLayout->setContentsMargins(0, 0, 0, 0);
    bodyLayout->addWidget(container);
    bodyLayout->addLayout(m_titleLayout);

    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addLayout(bodyLayout);
    mainLayout->addWidget(line);

    q->setLayout(mainLayout);
    q->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
}

RibbonBarInnerContainer *RibbonGroupPrivate::getContainer(RibbonGroup::GroupSize size)
{
    RibbonBarInnerContainer *container = nullptr;
    switch (size) {
        case RibbonGroup::GroupLarge: {
            if (m_largeContainer == nullptr) {
                m_largeContainer = new RibbonBarInnerContainer(size);
                if (m_mediumContainer != nullptr) {
                    m_inLayout->insertWidget(m_inLayout->indexOf(m_mediumContainer), m_largeContainer);
                } else if (m_smallContainer != nullptr) {
                    m_inLayout->insertWidget(m_inLayout->indexOf(m_smallContainer), m_largeContainer);
                } else {
                    m_inLayout->addWidget(m_largeContainer);
                }
            }
            container = m_largeContainer;
        }
        break;
        case RibbonGroup::GroupMedium: {
            if (m_mediumContainer == nullptr) {
                m_mediumContainer = new RibbonBarInnerContainer(size);
                if (m_smallContainer != nullptr) {
                    m_inLayout->insertWidget(m_inLayout->indexOf(m_smallContainer), m_mediumContainer);
                } else {
                    m_inLayout->addWidget(m_mediumContainer);
                }
            }
            container = m_mediumContainer;
        }
        break;
        default: {
            if (m_smallContainer == nullptr) {
                m_smallContainer = new RibbonBarInnerContainer(size);
                m_inLayout->addWidget(m_smallContainer);
            }
            container = m_smallContainer;
        }
        break;
    }
    return container;
}

QAction *RibbonGroupPrivate::addAction(const QIcon &icon, const QString &text,
                                       RibbonGroup::GroupSize size)
{
    RibbonBarInnerContainer *container = getContainer(size);
    QAction *action = container->addAction(icon, text);
    formatAction(container, action);
    return action;
}

void RibbonGroupPrivate::addAction(QAction *action, RibbonGroup::GroupSize size)
{
    RibbonBarInnerContainer *container = getContainer(size);
    container->addAction(action);
    formatAction(container, action);
}

void RibbonGroupPrivate::formatAction(RibbonBarInnerContainer *container, QAction *action)
{
    QToolButton *button = qobject_cast<QToolButton *>(container->widgetForAction(action));
    button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    container->layout()->setAlignment(button, Qt::AlignLeft);
}

void RibbonGroupPrivate::addToolButton(QToolButton *button, RibbonGroup::GroupSize size)
{
    RibbonBarInnerContainer *container = getContainer(size);
    container->addWidget(button);
    button->setIconSize(container->iconSize());
    button->setToolButtonStyle(container->toolButtonStyle());
    button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    container->layout()->setAlignment(button, Qt::AlignLeft);
}

RibbonGroup::RibbonGroup(const QString &title, QWidget *parent)
    : QWidget(parent)
    , d(new RibbonGroupPrivate())
{
    d->q = this;
    d->init();
    setTitle(title);
}

RibbonGroup::~RibbonGroup()
{
    delete d;
}

void RibbonGroup::setTitle(const QString &title)
{
    d->m_titleLabel->setText(title);
}

QString RibbonGroup::title() const
{
    return d->m_titleLabel->text();
}

void RibbonGroup::setTitleFont(const QFont &font)
{
    d->m_titleLabel->setFont(font);
}

QFont RibbonGroup::titleFont() const
{
    return d->m_titleLabel->font();
}

void RibbonGroup::setOptionButtonVisible(bool visible)
{
    d->m_optionButton->setVisible(visible);
}

bool RibbonGroup::isOptionButtonVisible() const
{
    return d->m_optionButton->isVisible();
}

void RibbonGroup::setOptionButtonToolTip(const QString &tip)
{
    d->m_optionButton->setToolTip(tip);
}

QAction *RibbonGroup::addAction(const QIcon &icon, const QString &text,
                                RibbonGroup::GroupSize size)
{
    return d->addAction(icon, text, size);
}

void RibbonGroup::addAction(QAction *action, RibbonGroup::GroupSize size)
{
    return d->addAction(action, size);
}

void RibbonGroup::addToolButton(QToolButton *button, GroupSize size)
{
    return d->addToolButton(button, size);
}

void RibbonGroup::addWidget(QWidget *widget)
{
    d->m_extLayout->addWidget(widget);
}

void RibbonGroup::removeWidget(QWidget *widget)
{
    d->m_extLayout->removeWidget(widget);
}

QSize RibbonGroup::sizeHint() const
{
    int controlWidth = d->m_inLayout->sizeHint().width() + d->m_extLayout->sizeHint().width();
    int titleWidth = d->m_titleLayout->sizeHint().width();
    int width = qMax(controlWidth, titleWidth) + 2; // 2 is for line
    int height = layout()->sizeHint().height();
    return QSize(width, height);
}

QCANPOOL_END_NAMESPACE
