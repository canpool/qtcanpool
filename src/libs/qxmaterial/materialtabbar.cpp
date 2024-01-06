/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#include "materialtabbar.h"
#include "materialtabbar_p.h"
#include "materialstyle.h"
#include <QtWidgets/QHBoxLayout>
#include <QEvent>
#include <QPainter>
#include <QPropertyAnimation>
#include <QtWidgets/QLayout>
#include <QtWidgets/QLayoutItem>

QX_MATERIAL_BEGIN_NAMESPACE

/*!
 *  \class MaterialTabBarInkBar
 *  \internal
 */

MaterialTabBarInkBar::MaterialTabBarInkBar(MaterialTabBar *parent)
    : MaterialOverlayWidget(parent)
    , m_tabBar(parent)
    , m_animation(new QPropertyAnimation(parent))
    , m_tween(0)
{
    Q_ASSERT(parent);

    m_animation->setPropertyName("tweenValue");
    m_animation->setEasingCurve(QEasingCurve::OutCirc);
    m_animation->setTargetObject(this);
    m_animation->setDuration(700);

    m_tabBar->installEventFilter(this);

    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_NoSystemBackground);
}

MaterialTabBarInkBar::~MaterialTabBarInkBar()
{
}

void MaterialTabBarInkBar::refreshGeometry()
{
    QLayoutItem *item = m_tabBar->layout()->itemAt(m_tabBar->currentIndex());

    if (item) {
        const QRect r(item->geometry());
        const qreal s = 1 - m_tween;

        if (QAbstractAnimation::Running != m_animation->state()) {
            m_geometry = QRect(r.left(), r.bottom() - 1, r.width(), 2);
        } else {
            const qreal left = m_previousGeometry.left() * s + r.left() * m_tween;
            const qreal width = m_previousGeometry.width() * s + r.width() * m_tween;
            m_geometry = QRect(left, r.bottom() - 1, width, 2);
        }
        m_tabBar->update();
    }
}

void MaterialTabBarInkBar::animate()
{
    raise();

    m_previousGeometry = m_geometry;

    m_animation->stop();
    m_animation->setStartValue(0);
    m_animation->setEndValue(1);
    m_animation->start();
}

bool MaterialTabBarInkBar::eventFilter(QObject *obj, QEvent *event)
{
    switch (event->type()) {
    case QEvent::Move:
    case QEvent::Resize: {
        refreshGeometry();
        break;
    }
    default:
        break;
    }
    return MaterialOverlayWidget::eventFilter(obj, event);
}

void MaterialTabBarInkBar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);

    painter.setOpacity(1);
    painter.fillRect(m_geometry, m_tabBar->inkColor());
}

/*!
 *  \class MaterialTab
 *  \internal
 */

MaterialTab::MaterialTab(MaterialTabBar *parent) : MaterialFlatButton(parent), m_tabBar(parent), m_active(false)
{
    Q_ASSERT(parent);

    setMinimumHeight(50);

    QFont f(font());
    f.setStyleName("Normal");
    setFont(f);

    setCornerRadius(0);
    setRole(QxMaterial::Primary);
    setBackgroundMode(Qt::OpaqueMode);
    setBaseOpacity(0.25);

    connect(this, SIGNAL(clicked(bool)), this, SLOT(activateTab()));
}

MaterialTab::~MaterialTab()
{
}

QSize MaterialTab::sizeHint() const
{
    if (icon().isNull()) {
        return MaterialFlatButton::sizeHint();
    } else {
        return QSize(40, iconSize().height() + 46);
    }
}

void MaterialTab::activateTab()
{
    m_tabBar->setCurrentTab(this);
}

void MaterialTab::paintForeground(QPainter *painter)
{
    painter->setPen(foregroundColor());

    if (!icon().isNull()) {
        painter->translate(0, 12);
    }

    QSize textSize(fontMetrics().size(Qt::TextSingleLine, text()));
    QSize base(size() - textSize);

    QRect textGeometry(QPoint(base.width(), base.height()) / 2, textSize);

    painter->drawText(textGeometry, Qt::AlignCenter, text());

    if (!icon().isNull()) {
        const QSize &size = iconSize();
        QRect iconRect(QPoint((width() - size.width()) / 2, 0), size);

        QPixmap pixmap = icon().pixmap(iconSize());
        QPainter icon(&pixmap);
        icon.setCompositionMode(QPainter::CompositionMode_SourceIn);
        icon.fillRect(pixmap.rect(), painter->pen().color());
        painter->drawPixmap(iconRect, pixmap);
    }

    if (!m_active) {
        if (!icon().isNull()) {
            painter->translate(0, -12);
        }
        QBrush overlay;
        overlay.setStyle(Qt::SolidPattern);
        overlay.setColor(backgroundColor());
        painter->setOpacity(0.36);
        painter->fillRect(rect(), overlay);
    }
}

/*!
 *  \MaterialTabBarPrivate
 *  \internal
 */

MaterialTabBarPrivate::MaterialTabBarPrivate(MaterialTabBar *q)
    : q_ptr(q)
{
}

MaterialTabBarPrivate::~MaterialTabBarPrivate()
{
}

void MaterialTabBarPrivate::MaterialTabBarPrivate::init()
{
    Q_Q(MaterialTabBar);

    inkBar = new MaterialTabBarInkBar(q);
    tabLayout = new QHBoxLayout;
    rippleStyle = QxMaterial::CenteredRipple;
    tab = -1;
    showHalo = true;
    useThemeColors = true;

    q->setLayout(tabLayout);
    q->setStyle(&MaterialStyle::instance());

    tabLayout->setSpacing(0);
    tabLayout->setContentsMargins(0, 0, 0, 0);
}

/*!
 *  \MaterialTabBar
 */

MaterialTabBar::MaterialTabBar(QWidget *parent) : QWidget(parent), d_ptr(new MaterialTabBarPrivate(this))
{
    d_func()->init();
}

MaterialTabBar::~MaterialTabBar()
{
}

void MaterialTabBar::setUseThemeColors(bool value)
{
    Q_D(MaterialTabBar);

    d->useThemeColors = value;
}

bool MaterialTabBar::useThemeColors() const
{
    Q_D(const MaterialTabBar);

    return d->useThemeColors;
}

void MaterialTabBar::setHaloVisible(bool value)
{
    Q_D(MaterialTabBar);

    d->showHalo = value;
    updateTabs();
}

bool MaterialTabBar::isHaloVisible() const
{
    Q_D(const MaterialTabBar);

    return d->showHalo;
}

void MaterialTabBar::setRippleStyle(QxMaterial::RippleStyle style)
{
    Q_D(MaterialTabBar);

    d->rippleStyle = style;
    updateTabs();
}

QxMaterial::RippleStyle MaterialTabBar::rippleStyle() const
{
    Q_D(const MaterialTabBar);

    return d->rippleStyle;
}

void MaterialTabBar::setInkColor(const QColor &color)
{
    Q_D(MaterialTabBar);

    d->inkColor = color;

    MATERIAL_DISABLE_THEME_COLORS
    d->inkBar->update();
    update();
}

QColor MaterialTabBar::inkColor() const
{
    Q_D(const MaterialTabBar);

    if (d->useThemeColors || !d->inkColor.isValid()) {
        return MaterialStyle::instance().themeColor(MK_accent1);
    } else {
        return d->inkColor;
    }
}

void MaterialTabBar::setBackgroundColor(const QColor &color)
{
    Q_D(MaterialTabBar);

    d->backgroundColor = color;

    MATERIAL_DISABLE_THEME_COLORS
    updateTabs();
    update();
}

QColor MaterialTabBar::backgroundColor() const
{
    Q_D(const MaterialTabBar);

    if (d->useThemeColors || !d->backgroundColor.isValid()) {
        return MaterialStyle::instance().themeColor(MK_primary1);
    } else {
        return d->backgroundColor;
    }
}

void MaterialTabBar::setTextColor(const QColor &color)
{
    Q_D(MaterialTabBar);

    d->textColor = color;

    MATERIAL_DISABLE_THEME_COLORS
    updateTabs();
    update();
}

QColor MaterialTabBar::textColor() const
{
    Q_D(const MaterialTabBar);

    if (d->useThemeColors || !d->textColor.isValid()) {
        return MaterialStyle::instance().themeColor(MK_canvas);
    } else {
        return d->textColor;
    }
}

void MaterialTabBar::setCurrentTab(MaterialTab *tab)
{
    Q_D(MaterialTabBar);

    setCurrentTab(d->tabLayout->indexOf(tab));
}

void MaterialTabBar::setCurrentTab(int index)
{
    Q_D(MaterialTabBar);

    setTabActive(d->tab, false);
    d->tab = index;
    setTabActive(index, true);
    d->inkBar->animate();

    emit currentChanged(index);
}

void MaterialTabBar::addTab(const QString &text, const QIcon &icon)
{
    Q_D(MaterialTabBar);

    MaterialTab *tab = new MaterialTab(this);
    tab->setText(text);
    tab->setHaloVisible(isHaloVisible());
    tab->setRippleStyle(rippleStyle());

    if (!icon.isNull()) {
        tab->setIcon(icon);
        tab->setIconSize(QSize(22, 22));
    }

    d->tabLayout->addWidget(tab);

    if (-1 == d->tab) {
        d->tab = 0;
        d->inkBar->refreshGeometry();
        d->inkBar->raise();
        tab->setActive(true);
    }
}

int MaterialTabBar::currentIndex() const
{
    Q_D(const MaterialTabBar);

    return d->tab;
}

void MaterialTabBar::setTabActive(int index, bool active)
{
    Q_D(MaterialTabBar);

    MaterialTab *tab;

    if (index > -1) {
        tab = static_cast<MaterialTab *>(d->tabLayout->itemAt(index)->widget());
        if (tab) {
            tab->setActive(active);
        }
    }
}

void MaterialTabBar::updateTabs()
{
    Q_D(MaterialTabBar);

    MaterialTab *tab;
    for (int i = 0; i < d->tabLayout->count(); ++i) {
        QLayoutItem *item = d->tabLayout->itemAt(i);
        if ((tab = static_cast<MaterialTab *>(item->widget()))) {
            tab->setRippleStyle(d->rippleStyle);
            tab->setHaloVisible(d->showHalo);
            tab->setBackgroundColor(backgroundColor());
            tab->setForegroundColor(textColor());
        }
    }
}

QX_MATERIAL_END_NAMESPACE
