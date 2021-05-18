/***************************************************************************
 **
 **  Copyright (C) 2020 MaMinJie <canpool@163.com>
 **  Contact: https://github.com/canpool
 **           https://gitee.com/icanpool
 **
 **  GNU Lesser General Public License Usage
 **  Alternatively, this file may be used under the terms of the GNU Lesser
 **  General Public License version 3 as published by the Free Software
 **  Foundation and appearing in the file LICENSE.LGPL3 included in the
 **  packaging of this file. Please review the following information to
 **  ensure the GNU Lesser General Public License version 3 requirements
 **  will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
 **
 **  GNU General Public License Usage
 **  Alternatively, this file may be used under the terms of the GNU
 **  General Public License version 2.0 or (at your option) the GNU General
 **  Public license version 3 or any later version approved by the KDE Free
 **  Qt Foundation. The licenses are as published by the Free Software
 **  Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
 **  included in the packaging of this file. Please review the following
 **  information to ensure the GNU General Public License requirements will
 **  be met: https://www.gnu.org/licenses/gpl-2.0.html and
 **  https://www.gnu.org/licenses/gpl-3.0.html.
 **
****************************************************************************/
#include "fancyribbongroup.h"
#include "fancyribbonpage.h"
#include "fancybutton.h"

#include <QVBoxLayout>
#include <QToolButton>
#include <QLabel>
#include <QToolBar>

QCANPOOL_BEGIN_NAMESPACE

/* FancyRibbonInnerContainer */
class FancyRibbonInnerContainer : public QToolBar
{
    Q_OBJECT
public:
    FancyRibbonInnerContainer(FancyRibbonGroup::GroupSize size, QWidget *parent = nullptr);
    virtual ~FancyRibbonInnerContainer();
};

FancyRibbonInnerContainer::FancyRibbonInnerContainer(FancyRibbonGroup::GroupSize size, QWidget *parent)
    : QToolBar(parent)
{
    switch (size) {
        case FancyRibbonGroup::GroupLarge: {
            setIconSize(QSize(32, 32));
            setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
            setOrientation(Qt::Horizontal);
        }
        break;
        case FancyRibbonGroup::GroupMedium: {
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
}

FancyRibbonInnerContainer::~FancyRibbonInnerContainer()
{

}

/* FancyRibbonGroupPrivate */
class FancyRibbonGroupPrivate : public QObject
{
    Q_OBJECT
public:
    explicit FancyRibbonGroupPrivate();
    virtual ~FancyRibbonGroupPrivate();

    void init();

    FancyRibbonInnerContainer *getContainer(FancyRibbonGroup::GroupSize size);
    QAction *addAction(const QIcon &icon, const QString &text,
                       FancyRibbonGroup::GroupSize size);
    void addAction(QAction *action, FancyRibbonGroup::GroupSize size);
    void formatAction(FancyRibbonInnerContainer *container, QAction *action);

public:
    FancyRibbonGroup *q;
    QHBoxLayout *m_inLayout;
    QHBoxLayout *m_extLayout;
    QLabel *m_titleLabel;
    FancyButton *m_optionButton;

    FancyRibbonInnerContainer *m_largeContainer;
    FancyRibbonInnerContainer *m_mediumContainer;
    FancyRibbonInnerContainer *m_smallContainer;
};


FancyRibbonGroupPrivate::FancyRibbonGroupPrivate()
    : m_largeContainer(nullptr)
    , m_mediumContainer(nullptr)
    , m_smallContainer(nullptr)
{
}

FancyRibbonGroupPrivate::~FancyRibbonGroupPrivate()
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
void FancyRibbonGroupPrivate::init()
{
    m_titleLabel = new QLabel();
    m_titleLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    m_optionButton = new FancyButton();
    m_optionButton->setIcon(QIcon(":/ribbon/option"));
    m_optionButton->setIconSize(QSize(10, 10));
    m_optionButton->setHoverColor(QColor(0, 0, 0, 40));
    m_optionButton->setPressColor(QColor(0, 0, 0, 80));
    m_optionButton->setHasBorder(false);
    m_optionButton->setToolTip(tr("option"));
    m_optionButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_optionButton->setVisible(false);
    connect(m_optionButton, SIGNAL(clicked()), q, SIGNAL(optionClicked()));

    QHBoxLayout *titleLayout = new QHBoxLayout();
    titleLayout->setSpacing(0);
    titleLayout->setContentsMargins(0, 0, 2, 2);
    titleLayout->addStretch();
    titleLayout->addWidget(m_titleLabel);
    titleLayout->addStretch();
    titleLayout->addWidget(m_optionButton);

    QWidget *line = new QWidget();
    line->setFixedWidth(1);
    line->setAutoFillBackground(true);
    line->setBackgroundRole(QPalette::Mid);

    // internal controls layout
    m_inLayout = new QHBoxLayout();
    m_inLayout->setSpacing(0);
    m_inLayout->setMargin(0);
    // external controls layout
    m_extLayout = new QHBoxLayout();
    m_extLayout->setSpacing(0);
    m_extLayout->setMargin(0);
    QHBoxLayout *controlLayout = new QHBoxLayout();
    controlLayout->setSpacing(0);
    controlLayout->setMargin(0);
    controlLayout->addLayout(m_inLayout);
    controlLayout->addLayout(m_extLayout);
    QWidget *container = new QWidget();
    container->setLayout(controlLayout);

    QVBoxLayout *bodyLayout = new QVBoxLayout();
    bodyLayout->setSpacing(0);
    bodyLayout->setMargin(0);
    bodyLayout->addWidget(container);
    bodyLayout->addLayout(titleLayout);

    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);
    mainLayout->addLayout(bodyLayout);
    mainLayout->addWidget(line);

    q->setLayout(mainLayout);
    q->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
}

FancyRibbonInnerContainer *FancyRibbonGroupPrivate::getContainer(FancyRibbonGroup::GroupSize size)
{
    FancyRibbonInnerContainer *container = nullptr;
    switch (size) {
        case FancyRibbonGroup::GroupLarge: {
            if (m_largeContainer == nullptr) {
                m_largeContainer = new FancyRibbonInnerContainer(size);
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
        case FancyRibbonGroup::GroupMedium: {
            if (m_mediumContainer == nullptr) {
                m_mediumContainer = new FancyRibbonInnerContainer(size);
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
                m_smallContainer = new FancyRibbonInnerContainer(size);
                m_inLayout->addWidget(m_smallContainer);
            }
            container = m_smallContainer;
        }
        break;
    }
    return container;
}

QAction *FancyRibbonGroupPrivate::addAction(const QIcon &icon, const QString &text,
                                            FancyRibbonGroup::GroupSize size)
{
    FancyRibbonInnerContainer *container = getContainer(size);
    QAction *action = container->addAction(icon, text);
    formatAction(container, action);
    return action;
}

void FancyRibbonGroupPrivate::addAction(QAction *action, FancyRibbonGroup::GroupSize size)
{
    FancyRibbonInnerContainer *container = getContainer(size);
    container->addAction(action);
    formatAction(container, action);
}

void FancyRibbonGroupPrivate::formatAction(FancyRibbonInnerContainer *container, QAction *action)
{
    QToolButton *button = qobject_cast<QToolButton *>(container->widgetForAction(action));
    button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    container->layout()->setAlignment(button, Qt::AlignLeft);
}

FancyRibbonGroup::FancyRibbonGroup(const QString &title, QWidget *parent)
    : QWidget(parent)
    , d(new FancyRibbonGroupPrivate())
{
    d->q = this;
    d->init();
    setTitle(title);
}

FancyRibbonGroup::~FancyRibbonGroup()
{
    delete d;
}

void FancyRibbonGroup::setTitle(const QString &title)
{
    d->m_titleLabel->setText(title);
}

QString FancyRibbonGroup::title() const
{
    return d->m_titleLabel->text();
}

void FancyRibbonGroup::setTitleFont(const QFont &font)
{
    d->m_titleLabel->setFont(font);
}

QFont FancyRibbonGroup::titleFont() const
{
    return d->m_titleLabel->font();
}

void FancyRibbonGroup::setOptionButtonVisible(bool visible)
{
    d->m_optionButton->setVisible(visible);
}

bool FancyRibbonGroup::isOptionButtonVisible() const
{
    return d->m_optionButton->isVisible();
}

QAction *FancyRibbonGroup::addAction(const QIcon &icon, const QString &text,
                                     FancyRibbonGroup::GroupSize size)
{
    return d->addAction(icon, text, size);
}

void FancyRibbonGroup::addAction(QAction *action, FancyRibbonGroup::GroupSize size)
{
    return d->addAction(action, size);
}

void FancyRibbonGroup::addWidget(QWidget *widget)
{
    d->m_extLayout->addWidget(widget);
}

void FancyRibbonGroup::removeWidget(QWidget *widget)
{
    d->m_extLayout->removeWidget(widget);
}

QCANPOOL_END_NAMESPACE

#include "fancyribbongroup.moc"
