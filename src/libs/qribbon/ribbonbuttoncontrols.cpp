/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 **/
#include "ribbonbuttoncontrols.h"
#include "ribbonbutton_p.h"
#include "ribboncontrols_p.h"
#include "ribbongroup.h"

QRIBBON_USE_NAMESPACE

RibbonButtonControlPrivate::RibbonButtonControlPrivate() : m_toolButton(Q_NULL) {}

RibbonButtonControlPrivate::~RibbonButtonControlPrivate() {}

void RibbonButtonControlPrivate::init()
{
    Q_Q(RibbonButtonControl);
    q->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setStretchable(false);
    q->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setStretchable(false);
    q->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setStretchable(false);
    q->sizeDefinition(RibbonControlSizeDefinition::GroupPopup)->setStretchable(false);

    m_toolButton = new RibbonButton(q);
    q->setContentWidget(m_toolButton);
}

/* RibbonButtonControl */
RibbonButtonControl::RibbonButtonControl(RibbonGroup *parentGroup) : RibbonWidgetControl(parentGroup, true)
{
    QRN_INIT_PRIVATE(RibbonButtonControl);
    Q_D(RibbonButtonControl);
    d->init();
}

RibbonButtonControl::~RibbonButtonControl() { QRN_FINI_PRIVATE(); }

const QPixmap &RibbonButtonControl::largeIcon() const
{
    Q_D(const RibbonButtonControl);
    return d->m_largeIcon;
}

void RibbonButtonControl::setLargeIcon(const QPixmap &icon)
{
    Q_D(RibbonButtonControl);
    d->m_largeIcon = icon;
}

const QPixmap &RibbonButtonControl::smallIcon() const
{
    Q_D(const RibbonButtonControl);
    return d->m_smallIcon;
}

void RibbonButtonControl::setSmallIcon(const QPixmap &icon)
{
    Q_D(RibbonButtonControl);
    d->m_smallIcon = icon;
}

const QString &RibbonButtonControl::label() const
{
    Q_D(const RibbonButtonControl);
    return d->m_label;
}

void RibbonButtonControl::setLabel(const QString &label)
{
    Q_D(RibbonButtonControl);
    d->m_label = label;
}

void RibbonButtonControl::setMenu(QMenu *menu)
{
    Q_D(RibbonButtonControl);
    d->m_toolButton->setMenu(menu);
}

QMenu *RibbonButtonControl::menu() const
{
    Q_D(const RibbonButtonControl);
    return d->m_toolButton->menu();
}

void RibbonButtonControl::setPopupMode(QToolButton::ToolButtonPopupMode mode)
{
    Q_D(RibbonButtonControl);
    d->m_toolButton->setPopupMode(mode);
}

QToolButton::ToolButtonPopupMode RibbonButtonControl::popupMode() const
{
    Q_D(const RibbonButtonControl);
    return d->m_toolButton->popupMode();
}

void RibbonButtonControl::setToolButtonStyle(Qt::ToolButtonStyle style)
{
    Q_D(RibbonButtonControl);
    d->m_toolButton->setToolButtonStyle(style);
    if (style == Qt::ToolButtonIconOnly) {
        sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setLabelVisible(false);
        sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setLabelVisible(false);
        sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setLabelVisible(false);
        sizeDefinition(RibbonControlSizeDefinition::GroupPopup)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        sizeDefinition(RibbonControlSizeDefinition::GroupPopup)->setLabelVisible(false);
    } else if (style == Qt::ToolButtonTextOnly) {
        sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setImageSize(RibbonControlSizeDefinition::ImageNone);
        sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setLabelVisible(true);
        sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setImageSize(RibbonControlSizeDefinition::ImageNone);
        sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setLabelVisible(true);
        sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setImageSize(RibbonControlSizeDefinition::ImageNone);
        sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setLabelVisible(true);
        sizeDefinition(RibbonControlSizeDefinition::GroupPopup)->setImageSize(RibbonControlSizeDefinition::ImageNone);
        sizeDefinition(RibbonControlSizeDefinition::GroupPopup)->setLabelVisible(true);
    } else if (style == Qt::ToolButtonTextBesideIcon) {
        sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        sizeDefinition(RibbonControlSizeDefinition::GroupPopup)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
    }
}

Qt::ToolButtonStyle RibbonButtonControl::toolButtonStyle() const
{
    Q_D(const RibbonButtonControl);
    return d->m_toolButton->toolButtonStyle();
}

QToolButton *RibbonButtonControl::widget() const
{
    Q_D(const RibbonButtonControl);
    return d->m_toolButton;
}

QSize RibbonButtonControl::sizeHint() const
{
    Q_D(const RibbonButtonControl);
    QSize sz = RibbonWidgetControl::sizeHint();

    if (d->m_toolButton && parentGroup()) {
        if (RibbonBar *ribbonBar = parentGroup()->ribbonBar()) {
            Qt::ToolButtonStyle style = toolButtonStyle();
            if (style == Qt::ToolButtonTextBesideIcon || style == Qt::ToolButtonIconOnly ||
                style == Qt::ToolButtonTextOnly)
                sz.setHeight(ribbonBar->rowItemHeight());
            else if (style == Qt::ToolButtonTextUnderIcon)
                sz.setHeight(ribbonBar->rowItemHeight() * ribbonBar->rowItemCount());
        }
    }

    return sz;
}

void RibbonButtonControl::sizeChanged(RibbonControlSizeDefinition::GroupSize size)
{
    Q_D(RibbonButtonControl);
    RibbonControl::sizeChanged(size);

    d->m_toolButton->setDefaultAction(defaultAction());

    if (sizeDefinition(size)->imageSize() == RibbonControlSizeDefinition::ImageLarge) {
        const QPixmap &px = largeIcon();
        d->m_toolButton->setIconSize(px.size());
        d->m_toolButton->setIcon(px);
        d->m_toolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    } else if (sizeDefinition(size)->imageSize() == RibbonControlSizeDefinition::ImageSmall) {
        const QPixmap &px = smallIcon();
        d->m_toolButton->setIconSize(px.size());
        d->m_toolButton->setIcon(px);
        d->m_toolButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        if (sizeDefinition(size)->isLabelVisible())
            d->m_toolButton->setText(label());
        else
            d->m_toolButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    }
}

void RibbonButtonControl::actionChanged()
{
    const QSize bugImageSize = QSize(32, 32);
    const QSize smallImageSize = QSize(16, 16);

    if (defaultAction() != Q_NULL) {
        if (!defaultAction()->iconText().isEmpty())
            setLabel(defaultAction()->iconText());
        else
            setLabel(defaultAction()->text());

        setLargeIcon(defaultAction()->icon().pixmap(bugImageSize));
        setSmallIcon(defaultAction()->icon().pixmap(smallImageSize));
    } else {
        setLabel("Empty");
    }

    RibbonControl::actionChanged();
}

/* RibbonCheckBoxControl */
RibbonCheckBoxControl::RibbonCheckBoxControl(const QString &text, RibbonGroup *parentGroup)
    : RibbonWidgetControl(parentGroup, true /*ignoreActionSettings*/)
{
    QCheckBox *checkBox = new QCheckBox(text, this);
    setContentWidget(checkBox);
}

RibbonCheckBoxControl::~RibbonCheckBoxControl() {}

QCheckBox *RibbonCheckBoxControl::widget() const { return qobject_cast<QCheckBox *>(contentWidget()); }

/* RibbonCheckBoxControl */
RibbonRadioButtonControl::RibbonRadioButtonControl(const QString &text, RibbonGroup *parentGroup)
    : RibbonWidgetControl(parentGroup, true /*ignoreActionSettings*/)
{
    QRadioButton *radioButton = new QRadioButton(text, this);
    setContentWidget(radioButton);
}

RibbonRadioButtonControl::~RibbonRadioButtonControl() {}

QRadioButton *RibbonRadioButtonControl::widget() const { return qobject_cast<QRadioButton *>(contentWidget()); }
