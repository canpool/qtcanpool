/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 **/
#include "ribboncontrols.h"
#include "officepopupcolorbutton.h"
#include "ribbonbar_p.h"
#include "ribboncontrols_p.h"
#include "ribbongroup.h"
#include "ribbonstyle.h"
#include <QAction>
#include <QApplication>
#include <QDesktopWidget>
#include <QPainter>
#include <QStyleOption>
#include <QToolBar>

QRIBBON_USE_NAMESPACE

RibbonControlSizeDefinitionPrivate::RibbonControlSizeDefinitionPrivate() : m_parentControl(Q_NULL) {}

void RibbonControlSizeDefinitionPrivate::init(RibbonControl *parentControl, RibbonControlSizeDefinition::GroupSize size)
{
    m_parentControl = parentControl;
    if (size == RibbonControlSizeDefinition::GroupLarge || size == RibbonControlSizeDefinition::GroupPopup) {
        m_imageSize = RibbonControlSizeDefinition::ImageLarge;
        m_labelVisible = true;
    } else {
        m_imageSize = RibbonControlSizeDefinition::ImageSmall;
        if (size == RibbonControlSizeDefinition::GroupMedium)
            m_labelVisible = true;
        else
            m_labelVisible = false;
    }
    m_popup = size == RibbonControlSizeDefinition::GroupPopup;
    m_showSeparator = true;
    m_visible = true;
    m_enabled = true;
    m_stretchable = true;
    m_visualIndex = -1;
    m_minItemCount = -1;
    m_maxItemCount = -1;
}

void RibbonControlSizeDefinitionPrivate::updateSizeDefinition()
{
    m_parentControl->sizeChanged(m_parentControl->currentSize());
}

RibbonControlSizeDefinitionPrivate::~RibbonControlSizeDefinitionPrivate() {}

/*!
\property RibbonGroup::isControlsGrouping
*/
/*!
\property RibbonGroup::isControlsCentering
*/
/*!
\property RibbonGroup::title
*/
/*!
\property RibbonGroup::isOptionButtonVisible
*/

/* RibbonControlSizeDefinition */
RibbonControlSizeDefinition::RibbonControlSizeDefinition(RibbonControl *parentControl,
                                                         RibbonControlSizeDefinition::GroupSize size)
    : QObject()
{
    QTC_INIT_PRIVATE(RibbonControlSizeDefinition);
    QTC_D(RibbonControlSizeDefinition);
    d->init(parentControl, size);
}

RibbonControlSizeDefinition::~RibbonControlSizeDefinition() { QTC_FINI_PRIVATE(); }

RibbonControlSizeDefinition::ControlImageSize RibbonControlSizeDefinition::imageSize() const
{
    QTC_D(const RibbonControlSizeDefinition);
    return d->m_imageSize;
}

void RibbonControlSizeDefinition::setImageSize(RibbonControlSizeDefinition::ControlImageSize size)
{
    QTC_D(RibbonControlSizeDefinition);
    if (d->m_imageSize != size) {
        d->m_imageSize = size;
        d->updateSizeDefinition();
    }
}

bool RibbonControlSizeDefinition::isLabelVisible() const
{
    QTC_D(const RibbonControlSizeDefinition);
    return d->m_labelVisible;
}

void RibbonControlSizeDefinition::setLabelVisible(bool visible)
{
    QTC_D(RibbonControlSizeDefinition);
    if (d->m_labelVisible != visible) {
        d->m_labelVisible = visible;
        d->updateSizeDefinition();
    }
}

bool RibbonControlSizeDefinition::isPopup() const
{
    QTC_D(const RibbonControlSizeDefinition);
    return d->m_popup;
}

void RibbonControlSizeDefinition::setPopup(bool popup)
{
    QTC_D(RibbonControlSizeDefinition);
    d->m_popup = popup;
}

int RibbonControlSizeDefinition::visualIndex() const
{
    QTC_D(const RibbonControlSizeDefinition);
    return d->m_visualIndex;
}

void RibbonControlSizeDefinition::setVisualIndex(int index)
{
    QTC_D(RibbonControlSizeDefinition);
    d->m_visualIndex = index;
}

bool RibbonControlSizeDefinition::showSeparator() const
{
    QTC_D(const RibbonControlSizeDefinition);
    return d->m_showSeparator;
}

void RibbonControlSizeDefinition::setShowSeparator(bool showSeparator)
{
    QTC_D(RibbonControlSizeDefinition);
    d->m_showSeparator = showSeparator;
}

bool RibbonControlSizeDefinition::isVisible() const
{
    QTC_D(const RibbonControlSizeDefinition);
    return d->m_visible;
}

void RibbonControlSizeDefinition::setVisible(bool visible)
{
    QTC_D(RibbonControlSizeDefinition);
    d->m_visible = visible;
}

bool RibbonControlSizeDefinition::isEnabled() const
{
    QTC_D(const RibbonControlSizeDefinition);
    return d->m_enabled;
}

void RibbonControlSizeDefinition::setEnabled(bool enabled)
{
    QTC_D(RibbonControlSizeDefinition);
    d->m_enabled = enabled;
}

bool RibbonControlSizeDefinition::isStretchable() const
{
    QTC_D(const RibbonControlSizeDefinition);
    return d->m_stretchable;
}

void RibbonControlSizeDefinition::setStretchable(bool stretchable)
{
    QTC_D(RibbonControlSizeDefinition);
    d->m_stretchable = stretchable;
}

int RibbonControlSizeDefinition::minimumItemCount() const
{
    QTC_D(const RibbonControlSizeDefinition);
    return d->m_minItemCount;
}

void RibbonControlSizeDefinition::setMinimumItemCount(int count)
{
    QTC_D(RibbonControlSizeDefinition);
    d->m_minItemCount = count;
}

int RibbonControlSizeDefinition::maximumItemCount() const
{
    QTC_D(const RibbonControlSizeDefinition);
    return d->m_maxItemCount;
}

void RibbonControlSizeDefinition::setMaximumItemCount(int count)
{
    QTC_D(RibbonControlSizeDefinition);
    d->m_maxItemCount = count;
}

/* RibbonControlPrivate */
RibbonControlPrivate::RibbonControlPrivate()
    : m_parentGroup(Q_NULL), m_size(RibbonControlSizeDefinition::GroupLarge), m_action(Q_NULL)
{
}

RibbonControlPrivate::~RibbonControlPrivate()
{
    delete m_allSizes[0];
    delete m_allSizes[1];
    delete m_allSizes[2];
    delete m_allSizes[3];
    m_allSizes.clear();
}

void RibbonControlPrivate::init(RibbonGroup *parentGroup)
{
    QTC_Q(RibbonControl)
    m_parentGroup = parentGroup;
    m_allSizes.append(new RibbonControlSizeDefinition(q, RibbonControlSizeDefinition::GroupLarge));    // Large
    m_allSizes.append(new RibbonControlSizeDefinition(q, RibbonControlSizeDefinition::GroupMedium));   // Medium
    m_allSizes.append(new RibbonControlSizeDefinition(q, RibbonControlSizeDefinition::GroupSmall));    // Small
    m_allSizes.append(new RibbonControlSizeDefinition(q, RibbonControlSizeDefinition::GroupPopup));    // Popup
}

void RibbonControlPrivate::changed() {}

/* RibbonControl */
RibbonControl::RibbonControl(RibbonGroup *parentGroup) : QWidget(Q_NULL)
{
    QTC_INIT_PRIVATE(RibbonControl);
    QTC_D(RibbonControl);
    d->init(parentGroup);
}

RibbonControl::~RibbonControl()
{
    if (parentGroup())
        parentGroup()->removeControl(this);
    QTC_FINI_PRIVATE();
}

RibbonGroup *RibbonControl::parentGroup() const
{
    QTC_D(const RibbonControl);
    return d->m_parentGroup;
}

QAction *RibbonControl::defaultAction() const
{
    QTC_D(const RibbonControl);
    return d->m_action;
}

void RibbonControl::setDefaultAction(QAction *action)
{
    QTC_D(RibbonControl);
    if (d->m_action == action)
        return;
    d->m_action = action;
    actionChanged();
}

RibbonControlSizeDefinition *RibbonControl::sizeDefinition(RibbonControlSizeDefinition::GroupSize size) const
{
    QTC_D(const RibbonControl);
    return d->m_allSizes[(int)size];
}

void RibbonControl::adjustCurrentSize(bool expand) { Q_UNUSED(expand); }

void RibbonControl::sizeChanged(RibbonControlSizeDefinition::GroupSize size)
{
    QTC_D(RibbonControl);
    d->m_size = size;
}

void RibbonControl::actionChanged()
{
    QTC_D(RibbonControl);
    d->changed();
}

void RibbonControl::updateLayout() {}

RibbonControlSizeDefinition::GroupSize RibbonControl::currentSize() const
{
    QTC_D(const RibbonControl);
    return d->m_size;
}

/* RibbonWidgetControlPrivate */
RibbonWidgetControlPrivate::RibbonWidgetControlPrivate()
    : m_widget(Q_NULL), m_visibleText(false), m_ignoreActionSettings(false), m_margin(4)
{
}

RibbonWidgetControlPrivate::~RibbonWidgetControlPrivate() {}

void RibbonWidgetControlPrivate::init(bool ignoreActionSettings)
{
    QTC_Q(RibbonWidgetControl);
    m_ignoreActionSettings = ignoreActionSettings;

    q->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setLabelVisible(true);
    q->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setLabelVisible(false);
    q->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setLabelVisible(false);
    q->sizeDefinition(RibbonControlSizeDefinition::GroupPopup)->setLabelVisible(true);

    if (!ignoreActionSettings)
        q->setAttribute(Qt::WA_Hover);
}

void RibbonWidgetControlPrivate::initStyleOption(QStyleOptionButton *option) const
{
    QTC_Q(const RibbonWidgetControl);
    option->init(q);
    QAction *action = q->defaultAction();
    if (action && !m_ignoreActionSettings) {
        option->icon = action->icon();
        option->text = m_visibleText ? action->text() : QString("");
        QSize szIcon = option->icon.actualSize(option->rect.size());
        option->iconSize = szIcon;
    }
}

void RibbonWidgetControlPrivate::clearWidget()
{
    /*
    Q_ASSERT(m_widget != Q_NULL);
    m_widget->setParent(Q_NULL);
    m_widget = Q_NULL;
    */
}

/* RibbonWidgetControl */
RibbonWidgetControl::RibbonWidgetControl(RibbonGroup *parentGroup, bool ignoreActionSettings)
    : RibbonControl(parentGroup)
{
    QTC_INIT_PRIVATE(RibbonWidgetControl);
    QTC_D(RibbonWidgetControl);
    d->init(ignoreActionSettings);
}

RibbonWidgetControl::~RibbonWidgetControl() { QTC_FINI_PRIVATE(); }

int RibbonWidgetControl::margin() const
{
    QTC_D(const RibbonWidgetControl);
    return d->m_margin;
}

void RibbonWidgetControl::setMargin(int margin)
{
    QTC_D(RibbonWidgetControl);
    d->m_margin = margin;
}

QWidget *RibbonWidgetControl::contentWidget() const
{
    QTC_D(const RibbonWidgetControl);
    return d->m_widget;
}

void RibbonWidgetControl::setContentWidget(QWidget *widget)
{
    QTC_D(RibbonWidgetControl);
    if (d->m_widget != Q_NULL)
        delete d->m_widget;

    d->m_widget = widget;

    if (d->m_widget != Q_NULL) {
        d->m_widget->setParent(this);
        d->m_widget->setVisible(true);
        d->m_widget->setAttribute(Qt::WA_MacSmallSize);
        // QFont f = d->m_widget->font();
        // QSize s = d->m_widget->sizeHint();
        // s = s;
    }
}

void RibbonWidgetControl::sizeChanged(RibbonControlSizeDefinition::GroupSize size)
{
    QTC_D(RibbonWidgetControl);
    RibbonControl::sizeChanged(size);
    d->m_visibleText = sizeDefinition(size)->isLabelVisible();
}

static QSize getBoundedSizeHint(const QWidget *widget)
{
    QSize ret = widget->sizeHint();
    ret.rwidth() = qMin(widget->maximumWidth(), ret.width());
    ret.rheight() = qMin(widget->maximumHeight(), ret.height());
    ret.rwidth() = qMax(widget->minimumWidth(), ret.width());
    ret.rheight() = qMax(widget->minimumHeight(), ret.height());
    return ret;
}

QSize RibbonWidgetControl::sizeHint() const
{
    QTC_D(const RibbonWidgetControl);
    if (d->m_widget != Q_NULL) {
        QSize sz = getBoundedSizeHint(d->m_widget);

        QStyleOptionButton opt;
        d->initStyleOption(&opt);

        if (!opt.icon.isNull())
            sz.setWidth(sz.width() + opt.iconSize.width() + d->m_margin);

        if (!opt.text.isEmpty()) {
            QFontMetrics fm = opt.fontMetrics;
            QSize szText = fm.boundingRect(opt.text).size();
            sz.setWidth(sz.width() + szText.width() + d->m_margin);
        }

        return sz;
    }
    return RibbonControl::sizeHint();
}

void RibbonWidgetControl::childEvent(QChildEvent *event)
{
    QTC_D(RibbonWidgetControl);
    RibbonControl::childEvent(event);

    if (event->type() == QEvent::ChildRemoved) {
        if (RibbonGroup *group = parentGroup()) {
            if (QAction *action = defaultAction()) {
                QChildEvent *childEvent = (QChildEvent *)event;
                if (childEvent->child()->isWidgetType()) {
                    if (childEvent->child() == contentWidget()) {
                        d->m_widget = Q_NULL;
                        group->removeAction(action);
                    }
                }
            }
        }
    }
}

void RibbonWidgetControl::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QTC_D(RibbonWidgetControl);
    if (!d->m_ignoreActionSettings && d->m_widget != Q_NULL) {
        QPainter painter(this);

        QStyleOptionButton opt;
        d->initStyleOption(&opt);

        bool enabled = d->m_widget->isEnabled();
        int posText = 0;

        if (!opt.icon.isNull()) {
            QRect rcIcon = opt.rect;
            rcIcon.setWidth(opt.iconSize.width());
            opt.icon.paint(&painter, rcIcon, Qt::AlignLeft | Qt::AlignVCenter,
                           enabled ? QIcon::Normal : QIcon::Disabled);
            posText = rcIcon.width() + d->m_margin;
        }

        if (!opt.text.isEmpty()) {
            QRect rectText = opt.rect;
            rectText.setLeft(posText);

            QFontMetrics fm = opt.fontMetrics;
            QSize szText = fm.boundingRect(opt.text).size();
            rectText.setWidth(szText.width());

            int flags = 0;   // opt.textAlignment;
            flags |= Qt::TextHideMnemonic | Qt::AlignVCenter;

            // opt.text = q->fontMetrics().elidedText(opt.text, Qt::ElideRight,
            // rcEntryCaption.adjusted(2,0,-2,0).width());
            style()->drawItemText(&painter, rectText, flags, opt.palette, opt.state & QStyle::State_Enabled, opt.text,
                                  QPalette::WindowText);
        }
    }
}

void RibbonWidgetControl::resizeEvent(QResizeEvent *event)
{
    QTC_D(RibbonWidgetControl);
    if (d->m_widget != Q_NULL) {
        int x = 0;
        int y = 0;

        QSize sz = event->size();

        QStyleOptionButton opt;
        d->initStyleOption(&opt);

        if (!opt.icon.isNull()) {
            x = opt.iconSize.width();
            x += d->m_margin;
        }
        if (!opt.text.isEmpty()) {
            QFontMetrics fm = opt.fontMetrics;
            QSize szText = fm.boundingRect(opt.text).size();
            x += szText.width();
            x += d->m_margin;
        }

        sz.rwidth() -= x;
        QRect r = QRect(x, y, sz.width(), sz.height());
        sz.rwidth() = qMin(sz.width(), d->m_widget->maximumWidth());
        QRect controlRect = QStyle::alignedRect(layoutDirection(), Qt::AlignRight, sz, r);
        d->m_widget->setGeometry(controlRect);
    }

    RibbonControl::resizeEvent(event);
}

/* RibbonColumnBreakControlPrivate */
RibbonColumnBreakControlPrivate::RibbonColumnBreakControlPrivate() : m_textAlignment(Qt::AlignVCenter) {}

RibbonColumnBreakControlPrivate::~RibbonColumnBreakControlPrivate() {}

void RibbonColumnBreakControlPrivate::init()
{
    QTC_Q(RibbonColumnBreakControl);
    QFont fnt = QApplication::font("QMenuBar");
    qreal pointSize = 0.8 * fnt.pointSizeF();
    fnt.setPointSizeF(pointSize);
    q->setFont(fnt);
}

void RibbonColumnBreakControlPrivate::initStyleOption(QStyleOptionGroupBox *option) const
{
    QTC_Q(const RibbonColumnBreakControl);
    option->initFrom(q);
    option->state |= QStyle::State_Horizontal;
    option->textAlignment = m_textAlignment;
    if (QAction *action = q->defaultAction())
        option->text = action->text();
}

/* RibbonWidgetControl */
RibbonColumnBreakControl::RibbonColumnBreakControl(RibbonGroup *parentGroup) : RibbonWidgetControl(parentGroup, true)
{
    QTC_INIT_PRIVATE(RibbonColumnBreakControl);
    QTC_D(RibbonColumnBreakControl);
    d->init();
}

RibbonColumnBreakControl::~RibbonColumnBreakControl() { QTC_FINI_PRIVATE(); }

Qt::Alignment RibbonColumnBreakControl::alignmentText() const
{
    QTC_D(const RibbonColumnBreakControl);
    return d->m_textAlignment;
}

void RibbonColumnBreakControl::setAlignmentText(Qt::Alignment alignment)
{
    QTC_D(RibbonColumnBreakControl);
    if (d->m_textAlignment != alignment) {
        switch (alignment) {
        case Qt::AlignTop:
            d->m_textAlignment = alignment;
            break;
        case Qt::AlignBottom:
            d->m_textAlignment = alignment;
            break;
        case Qt::AlignVCenter:
            d->m_textAlignment = alignment;
            break;
        default:
            Q_ASSERT(false);
            break;
        }
        update();
    }
}

void RibbonColumnBreakControl::sizeChanged(RibbonControlSizeDefinition::GroupSize size)
{
    RibbonControl::sizeChanged(size);
    if (sizeDefinition(size)->showSeparator()) {
    } else {
    }
}

QSize RibbonColumnBreakControl::sizeHint() const
{
    QTC_D(const RibbonColumnBreakControl);
    if (!parentGroup() || !parentGroup()->ribbonBar())
        return QWidget::sizeHint();

    QStyleOptionGroupBox opt;
    d->initStyleOption(&opt);
    const int height = parentGroup()->ribbonBar()->rowItemHeight() * parentGroup()->ribbonBar()->rowItemCount();
    int extent = style()->pixelMetric(QStyle::PM_ToolBarSeparatorExtent, &opt, parentGroup());

    if (!opt.text.isEmpty()) {
        QFontMetrics fm = opt.fontMetrics;
        extent += fm.height();
    }

    return QSize(extent, height);
}

void RibbonColumnBreakControl::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QTC_D(RibbonColumnBreakControl);
    QPainter p(this);
    QStyleOptionGroupBox opt;
    d->initStyleOption(&opt);
    style()->drawPrimitive(QStyle::PE_IndicatorToolBarSeparator, &opt, &p, parentGroup());
}

/* RibbonLabelControl */
RibbonLabelControl::RibbonLabelControl(RibbonGroup *parentGroup)
    : RibbonWidgetControl(parentGroup, true /*ignoreActionSettings*/)
{
    setContentWidget(new QLabel(this));
}

RibbonLabelControl::RibbonLabelControl(const QString &text, RibbonGroup *parentGroup)
    : RibbonWidgetControl(parentGroup, true /*ignoreActionSettings*/)
{
    setContentWidget(new QLabel(text, this));
}

RibbonLabelControl::~RibbonLabelControl() {}

QLabel *RibbonLabelControl::widget() const { return qobject_cast<QLabel *>(contentWidget()); }

/*RibbonToolBarSeparatorControlPrivate*/
RibbonToolBarSeparatorControlPrivate::RibbonToolBarSeparatorControlPrivate(RibbonToolBarControl *parent)
    : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}

QSize RibbonToolBarSeparatorControlPrivate::sizeHint() const
{
    QSize sz = QWidget::sizeHint();
    QStyleOption opt;
    opt.init(this);
    opt.state |= QStyle::State_Horizontal;
    const int extent = style()->pixelMetric(QStyle::PM_ToolBarSeparatorExtent, &opt, parentWidget());
    return QSize(extent, sz.width());
}

void RibbonToolBarSeparatorControlPrivate::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QStyleOption opt;
    opt.init(this);
    opt.state |= QStyle::State_Horizontal;
    style()->drawPrimitive(QStyle::PE_IndicatorToolBarSeparator, &opt, &p, parentWidget());
}

/* RibbonColumnBreakControlPrivate */
RibbonToolBarControlPrivate::RibbonToolBarControlPrivate()
    : m_margin(1), m_rowsCount(2), m_isCalcWidth(false), m_dirty(false), m_calcRows(-1), m_fullWidth(0)
{
}

RibbonToolBarControlPrivate::~RibbonToolBarControlPrivate() {}

void RibbonToolBarControlPrivate::recalcWidths(int height)
{
    QTC_Q(RibbonToolBarControl);
    if (q->parentGroup() == Q_NULL)
        return;

    m_vecWidths.clear();
    m_mapNonOptWidths.clear();

    QStyleOptionToolButton opt;
    opt.init(q->parentGroup());
    opt.text = q->parentGroup()->title();

    q->setRowsCount(2);
    m_isCalcWidth = true;
    QDesktopWidget desktop;
    QRect rect = desktop.screenGeometry();
    QRect rectScreen(QPoint(0, 0), rect.size());

    bool m_floatyMode = false;
    const int maxRows = 3;
    const int widthDefaultButton = q->parentGroup()->style()->pixelMetric(
        (QStyle::PixelMetric)RibbonStyle::PM_RibbonReducedGroupWidth, &opt, q->parentGroup());
    int lastWidth = -1;
    int dx = 16;

    for (int rows = 1; rows <= maxRows; rows++) {
        QRect rect(QPoint(0, 0), QSize(widthDefaultButton + 1, height));

        for (;; rect.setRight(rect.right() + dx)) {
            if (rect.width() >= rectScreen.width()) {
                if (m_vecWidths.size() == 0)
                    m_vecWidths.append(32767);
                break;
            }

            updateGroupsGeometry(rect);

            if (lastWidth != -1 && m_fullWidth > lastWidth)
                break;

            if (m_calcRows == rows && m_fullWidth > 0) {
                if (m_calcRows == maxRows - 1 && !m_floatyMode)
                    m_vecWidths.clear();

                m_vecWidths.append(m_fullWidth);
                m_mapNonOptWidths.insert(m_fullWidth + m_margin, rect.width());
                lastWidth = m_fullWidth;
                break;
            }
        }
    }
    if (m_vecWidths.count() == 1)
        m_vecWidths.append(m_vecWidths[0]);
    m_vecWidths.append(widthDefaultButton);
    m_isCalcWidth = false;
}

void RibbonToolBarControlPrivate::showDefaultButton()
{
    QTC_Q(RibbonToolBarControl);
    if (q->parentGroup() == Q_NULL)
        return;
    QStyleOptionToolButton opt;
    opt.init(q->parentGroup());
    opt.text = q->parentGroup()->title();
    const int widthDefaultButton = q->parentGroup()->style()->pixelMetric(
        (QStyle::PixelMetric)RibbonStyle::PM_RibbonReducedGroupWidth, &opt, q->parentGroup());
    RibbonToolBarActionGroupList &groupsRef = m_groups;
    for (RibbonToolBarActionGroupList::iterator it = groupsRef.begin(); it != groupsRef.end(); ++it) {
        RibbonToolBarActionsGroup &actionGroup = *it;
        actionGroup.rect = QRect(0, 0, 0, 0);
    }
    m_rect.setRight(m_rect.left() + widthDefaultButton);
    m_calcRows = 0;
}

void RibbonToolBarControlPrivate::addActionToGroup(QAction *action, QWidget *widget)
{
    bool endgroup = m_groups.size() == 0 ||
                    (m_groups.last().items.size() > 0 && m_groups.last().items.last().action->isSeparator());
    if (endgroup)
        m_groups.append(RibbonToolBarActionsGroup());
    RibbonToolBarActionGroupItems &lastGroup = m_groups.last().items;
    RibbonToolBarActionGroupItem item(action, widget);
    lastGroup.append(item);
}

static int indexOfAction(const RibbonToolBarControlPrivate::RibbonToolBarActionGroupItems &group, QAction *action)
{
    int index = 0;
    for (RibbonToolBarControlPrivate::RibbonToolBarActionGroupItems::const_iterator it = group.constBegin();
         it != group.constEnd(); ++it) {
        if (it->action == action)
            return index;
        index++;
    }
    return -1;
}

void RibbonToolBarControlPrivate::removeActionFromGroup(QAction *action)
{
    for (RibbonToolBarActionGroupList::iterator it = m_groups.begin(); it != m_groups.end(); ++it) {
        RibbonToolBarActionsGroup group1 = *it;
        RibbonToolBarActionGroupItems &items = group1.items;
        int index = indexOfAction(items, action);
        if (index != -1) {
            delete items.at(index).widget;
            items.removeAt(index);
            if (items.size() == 0) {
                m_groups.erase(it);
            } else if (action->isSeparator() && (it + 1) != m_groups.end()) {
                RibbonToolBarActionsGroup group2 = *(it + 1);
                items += group2.items;   //*(it + 1);
                //  updateGroupProperties(items);
                m_groups.erase(it + 1);
            }
            return;
        }
    }
}

void RibbonToolBarControlPrivate::updateGroupsGeometry(const QRect &rect)
{
    QTC_Q(RibbonToolBarControl);

    if (q->parentGroup() == Q_NULL)
        return;

    RibbonBar *ribbonBar = q->parentGroup()->ribbonBar();
    if (ribbonBar == Q_NULL)
        return;

    bool m_floatyMode = false;
    m_rect = rect;
    m_rect.setWidth(rect.width());

    m_fullWidth = 0;
    m_calcRows = 0;

    QFontMetrics fm(q->parentGroup()->font());
    const QSize sizeCaption = fm.boundingRect(q->parentGroup()->title()).size();

    bool m_bTrancateCaption = false;
    if (!m_bTrancateCaption)
        m_rect.setRight(m_rect.left() + qMax(rect.width(), sizeCaption.width()));

    QSize size = rect.size();
    size.setWidth(size.width() - m_margin);

    int rowHeight = ribbonBar->rowItemHeight();

    for (RibbonToolBarActionGroupList::iterator it = m_groups.begin(); it != m_groups.end(); ++it) {
        RibbonToolBarActionsGroup &actionGroup = *it;
        QSize sizeElem = calculateSizeHint(actionGroup.items);
        sizeElem.setHeight(sizeElem.height() < rowHeight ? rowHeight : sizeElem.height());
        actionGroup.rect = QRect(QPoint(0, 0), sizeElem);
        actionGroup.row = -1;
    }

    int x = 0;
    int y = 0;

    int xStart = 0;
    bool isFullHeight = m_rect.bottom() == 32767;
    int cx = size.width();

    if (!m_isCalcWidth && m_mapNonOptWidths.size() > 0 && m_mapNonOptWidths.contains(cx))
        cx = m_mapNonOptWidths.value(cx);

    if (isMixedWidgets()) {
        for (RibbonToolBarActionGroupList::iterator it = m_groups.begin(); it != m_groups.end(); ++it) {
            RibbonToolBarActionsGroup &actionGroup = *it;
            QSize sizeElem = calculateSizeHint(actionGroup.items);

            const bool isLargeButton = sizeElem.height() > rowHeight;

            if (isLargeButton) {
                QRect rectElem(QPoint(m_rect.left() + x + m_margin, m_rect.top() /*+ m_margin / 2*/),
                               QSize(sizeElem.width(), isFullHeight ? sizeElem.height() : size.height()));

                actionGroup.rect = rectElem;
                actionGroup.row = 999;

                x += sizeElem.width() + m_margin;
                xStart = x;
                y = 0;
            }
        }
    }

    int bottom = 0;
    QVector<int> vecRowWidths;
    for (RibbonToolBarActionGroupList::iterator it = m_groups.begin(); it != m_groups.end(); ++it) {
        RibbonToolBarActionsGroup &actionGroup = *it;
        QSize sizeElem = calculateSizeHint(actionGroup.items);
        sizeElem.setHeight(sizeElem.height() < rowHeight ? rowHeight : sizeElem.height());

        if (sizeElem.isNull()) {
            actionGroup.rect = QRect(0, 0, 0, 0);
            continue;
        }

        if (actionGroup.row != -1)
            continue;

        if (x + sizeElem.width() + m_margin - 1 > cx) {
            if (x == xStart) {
                showDefaultButton();
                return;
            }

            y += rowHeight;

            if (m_floatyMode)
                y += m_margin / 2;

            vecRowWidths.append(x);

            m_calcRows++;

            x = xStart;
            rowHeight = 0;
        }

        if (y + sizeElem.height() > size.height()) {
            showDefaultButton();
            return;
        }

        QRect rectElem(QPoint(m_rect.left() + x + m_margin, m_rect.top() + y /*+ m_margin / 2*/), sizeElem);

        actionGroup.rect = rectElem;
        actionGroup.row = m_calcRows;

        rowHeight = qMax(rowHeight, sizeElem.height());
        x += sizeElem.width() + m_margin - 1;

        m_fullWidth = qMax(m_fullWidth, x - 1);

        bottom = qMax(bottom, rectElem.bottom());
    }

    vecRowWidths.append(x);
    m_calcRows++;

    if (isFullHeight) {
        m_rect.setBottom(bottom + sizeCaption.height() + m_margin / 2);
        size.setHeight(m_rect.height() - sizeCaption.height() - m_margin / 2);
    }

    int i = 0;
    bool m_preserveElementOrder = false;
    if (m_calcRows > 1 && !m_preserveElementOrder) {
        bool recalcFullWidth = false;

        while (true) {
            int maxRowWidth = 0;
            int maxRowIndex = -1;

            for (i = 0; i < vecRowWidths.size(); i++) {
                if (vecRowWidths[i] > maxRowWidth) {
                    maxRowWidth = vecRowWidths[i];
                    maxRowIndex = i;
                }
            }

            if (maxRowIndex < 0)
                break;

            int minWidth = 9999;
            RibbonToolBarActionsGroup *pMinElem = Q_NULL;

            for (RibbonToolBarActionGroupList::iterator it = m_groups.begin(); it != m_groups.end(); ++it) {
                RibbonToolBarActionsGroup &actionGroup = *it;

                if (actionGroup.row == maxRowIndex) {
                    QRect rectElem = actionGroup.rect;

                    if (!rectElem.isEmpty() && rectElem.width() < minWidth) {
                        minWidth = rectElem.width();
                        pMinElem = &actionGroup;
                    }
                }
            }

            if (pMinElem == Q_NULL)
                break;

            bool bMoved = false;

            for (i = maxRowIndex + 1; i < vecRowWidths.size(); i++) {
                if (vecRowWidths[i] + minWidth < maxRowWidth) {
                    int x = 0;
                    int y = 0;

                    for (RibbonToolBarActionGroupList::iterator it = m_groups.begin(); it != m_groups.end(); ++it) {
                        RibbonToolBarActionsGroup &actionGroup = *it;

                        if (actionGroup.row == i) {
                            x = qMax(actionGroup.rect.right() + m_margin, x);
                            y = actionGroup.rect.top();
                        } else if (actionGroup.row == maxRowIndex) {
                            QRect rectElem = actionGroup.rect;

                            if (rectElem.left() > pMinElem->rect.left()) {
                                int dx = -(minWidth + m_margin);
                                rectElem.adjust(dx, 0, dx, 0);
                                actionGroup.rect = rectElem;
                            }
                        }
                    }

                    pMinElem->rect = QRect(QPoint(x, y), pMinElem->rect.size());
                    pMinElem->row = i;

                    vecRowWidths[i] += minWidth;
                    vecRowWidths[maxRowIndex] -= minWidth;

                    recalcFullWidth = true;
                    bMoved = true;
                    break;
                }
            }

            if (!bMoved)
                break;
        }

        if (recalcFullWidth) {
            m_fullWidth = 0;
            for (RibbonToolBarActionGroupList::iterator it = m_groups.begin(); it != m_groups.end(); ++it)
                m_fullWidth = qMax(m_fullWidth, (*it).rect.right());
            m_fullWidth -= m_rect.left() + m_margin;
        }
    }

    if (!isFullHeight && !m_floatyMode && m_calcRows > 1) {
        int yOffset = (size.height() - m_calcRows * rowHeight) / (m_calcRows + 1);
        if (yOffset > 0) {
            for (RibbonToolBarActionGroupList::iterator it = m_groups.begin(); it != m_groups.end(); ++it) {
                RibbonToolBarActionsGroup &actionGroup = *it;

                int row = actionGroup.row;
                QRect rectElem = actionGroup.rect;

                if (row != 999 && !rectElem.isEmpty()) {
                    int dy = (yOffset * (row + 1) - row);
                    rectElem.adjust(0, dy - 1 /* - 3*/, 0, dy);
                    actionGroup.rect = rectElem;
                }
            }
        }
    }

    if (!m_isCalcWidth)
        updateGeometryWidget();
}

void RibbonToolBarControlPrivate::updateGeometryWidget()
{
    QTC_Q(RibbonToolBarControl);

    if (q->parentGroup() == Q_NULL)
        return;

    RibbonBar *ribbonBar = q->parentGroup()->ribbonBar();
    if (ribbonBar == Q_NULL)
        return;

    int rowHeight = ribbonBar->rowItemHeight();
    QList<QWidget *> showWidgets, hideWidgets;

    int currentRow = 0;
    QWidget *lastWidget = Q_NULL;
    RibbonToolBarSeparatorControlPrivate *currentSeparator = Q_NULL;
    for (RibbonToolBarActionGroupList::iterator it = m_groups.begin(); it != m_groups.end(); ++it) {
        RibbonToolBarActionsGroup &actionGroup = *it;
        if (currentRow != actionGroup.row && actionGroup.row != 999) {
            currentRow = actionGroup.row;
            if (currentSeparator)
                hideWidgets << currentSeparator;
        }
        QList<RibbonToolBarActionGroupItem> items = actionGroup.items;

        QRect rect = actionGroup.rect;
        int x = rect.left();
        for (QList<RibbonToolBarActionGroupItem>::iterator itItem = items.begin(); itItem != items.end(); ++itItem) {
            RibbonToolBarActionGroupItem &item = *itItem;
            if (item.widget) {
                QSize sizeElem = getBoundedSizeHint(item.widget);
                if (lastWidget) {
                    if (RibbonToolBarSeparatorControlPrivate *separatorControl =
                            qobject_cast<RibbonToolBarSeparatorControlPrivate *>(item.widget)) {
                        QSize sz = lastWidget->sizeHint();
                        sizeElem.setHeight(sz.height());
                        currentSeparator = separatorControl;
                        showWidgets << separatorControl;
                    } else
                        sizeElem.setHeight(sizeElem.height() < rowHeight ? rowHeight : sizeElem.height());
                } else
                    sizeElem.setHeight(sizeElem.height() < rowHeight ? rowHeight : sizeElem.height());

                if (item.widget)
                    lastWidget = item.widget;

                if (item.isEmpty())
                    hideWidgets << item.widget;
                else {
                    item.widget->setGeometry(QRect(QPoint(x, rect.top()), sizeElem));
                    showWidgets << item.widget;
                    x += sizeElem.width();
                }
            }
        }
    }

    if (currentSeparator != Q_NULL && currentSeparator == lastWidget)
        hideWidgets << currentSeparator;

    // we have to do the show/hide here, because it triggers more calls to setGeometry
    for (int i = 0, count = showWidgets.count(); i < count; ++i)
        showWidgets.at(i)->show();

    for (int i = 0, count = hideWidgets.count(); i < count; ++i)
        hideWidgets.at(i)->hide();
}

void RibbonToolBarControlPrivate::updateGroupProperties(RibbonToolBarActionGroupItems &group)
{
    QWidget *firstWidget = Q_NULL;
    QWidget *lastWidget = Q_NULL;
    for (RibbonToolBarActionGroupItems::iterator it = group.begin(); it != group.end(); ++it) {
        if (it->isEmpty())
            continue;

        if (it->widget == Q_NULL)
            continue;

        if (it->action->isSeparator())
            continue;

        if (firstWidget == Q_NULL)
            firstWidget = it->widget;

        lastWidget = it->widget;
        lastWidget->setProperty(_qtc_Begin_Group, firstWidget == it->widget);
        lastWidget->setProperty(_qtc_Group, true);
        lastWidget->setProperty(_qtc_End_Group, false);
    }

    if (lastWidget != Q_NULL)
        lastWidget->setProperty(_qtc_End_Group, true);
}

QSize RibbonToolBarControlPrivate::calculateSizeHint(const RibbonToolBarActionGroupItems &group) const
{
    QTC_Q(const RibbonToolBarControl);

    if (q->parentGroup() == Q_NULL)
        return QSize();

    RibbonBar *ribbonBar = q->parentGroup()->ribbonBar();
    Q_ASSERT(ribbonBar != Q_NULL);

    QSize ret = QSize(0, 0);
    for (RibbonToolBarControlPrivate::RibbonToolBarActionGroupItems::const_iterator it = group.constBegin();
         it != group.constEnd(); ++it) {
        if (it->action->isSeparator()) {
            if (it->action->isVisible()) {
                QStyleOption opt;
                opt.init(q);
                const int widthSeparator =
                    q->parentGroup()->style()->pixelMetric(QStyle::PM_ToolBarSeparatorExtent, &opt, q);
                ret.rwidth() += widthSeparator;
            }
            continue;
        }

        Q_ASSERT(it->widget != Q_NULL);
        if (it->widget == Q_NULL)
            continue;

        if (/*it->action->isVisible()*/ !it->isEmpty()) {
            QSize s = getBoundedSizeHint(it->widget);
            s.setHeight(s.height() < ribbonBar->rowItemHeight() ? ribbonBar->rowItemHeight() : s.height());
            ret.rwidth() += s.width();
            ret.rheight() = qMax(ret.height(), s.height());
        }
    }
    return ret;
}

void RibbonToolBarControlPrivate::updateLayoutParent(bool calcLayoutParent)
{
    QTC_Q(RibbonToolBarControl);

    RibbonGroup *parentGroup = q->parentGroup();
    if (parentGroup == Q_NULL)
        return;

    RibbonBar *ribbonBar = parentGroup->ribbonBar();
    if (ribbonBar == Q_NULL)
        return;

    recalcWidths(ribbonBar->rowItemHeight() * ribbonBar->rowItemCount());

    for (RibbonToolBarActionGroupList::iterator itGroup = m_groups.begin(); itGroup != m_groups.end(); ++itGroup) {
        RibbonToolBarActionsGroup &group = *itGroup;
        for (RibbonToolBarControlPrivate::RibbonToolBarActionGroupItems::iterator itItem = group.items.begin();
             itItem != group.items.end(); ++itItem)
            updateGroupProperties(group.items);
    }
    if (calcLayoutParent) {
        if (RibbonPage *page = qobject_cast<RibbonPage *>(parentGroup->parentWidget()))
            page->updateLayout();
    }
}

bool RibbonToolBarControlPrivate::isMixedWidgets() const
{
    QTC_Q(const RibbonToolBarControl);
    if (q->parentGroup() == Q_NULL)
        return false;

    RibbonBar *ribbonBar = q->parentGroup()->ribbonBar();
    if (ribbonBar == Q_NULL)
        return false;

    bool largeButton = false;
    bool smallButton = false;
    for (RibbonToolBarActionGroupList::const_iterator it = m_groups.begin(); it != m_groups.end(); ++it) {
        const RibbonToolBarActionsGroup &actionGroup = *it;
        QSize sizeElem = calculateSizeHint(actionGroup.items);
        bool isLargeButton = sizeElem.height() > ribbonBar->rowItemHeight();
        if (isLargeButton)
            largeButton = true;
        else
            smallButton = true;
        if (largeButton && smallButton)
            return true;
    }
    return false;
}

/* RibbonToolBarControl*/
RibbonToolBarControl::RibbonToolBarControl(RibbonGroup *parentGroup) : RibbonControl(parentGroup)
{
    QTC_INIT_PRIVATE(RibbonToolBarControl);
}

RibbonToolBarControl::~RibbonToolBarControl() { QTC_FINI_PRIVATE(); }

void RibbonToolBarControl::sizeChanged(RibbonControlSizeDefinition::GroupSize size)
{
    RibbonControl::sizeChanged(size);
    QTC_D(RibbonToolBarControl);
    if (d->m_vecWidths.isEmpty())
        return;

    switch (size) {
    case RibbonControlSizeDefinition::GroupLarge:
    case RibbonControlSizeDefinition::GroupMedium: {
        setRowsCount(2);
    } break;
    case RibbonControlSizeDefinition::GroupPopup: {
        setRowsCount(2);
    } break;
    case RibbonControlSizeDefinition::GroupSmall: {
        setRowsCount(3);
    } break;
    default:
        break;
    }
}

int RibbonToolBarControl::rowsCount()
{
    QTC_D(RibbonToolBarControl);
    return d->m_rowsCount;
}

void RibbonToolBarControl::setRowsCount(int count)
{
    QTC_D(RibbonToolBarControl);
    d->m_rowsCount = count;
}

void RibbonToolBarControl::clear()
{
    QList<QAction *> actions = this->actions();
    for (int i = 0; i < actions.size(); i++)
        removeAction(actions.at(i));
}

QAction *RibbonToolBarControl::addWidget(QWidget *widget)
{
    QWidgetAction *action = new QWidgetAction(this);
    action->setDefaultWidget(widget);
    RibbonControl::addAction(action);
    return action;
}

QAction *RibbonToolBarControl::addWidget(const QIcon &icon, const QString &text, QWidget *widget)
{
    QWidgetAction *action = new QWidgetAction(this);
    action->setIcon(icon);
    action->setText(text);
    action->setDefaultWidget(widget);
    RibbonControl::addAction(action);
    return action;
}

QAction *RibbonToolBarControl::addMenu(const QIcon &icon, const QString &text, QMenu *menu,
                                       QToolButton::ToolButtonPopupMode mode)
{
    Q_ASSERT(menu != Q_NULL);
    Q_ASSERT_X(mode != QToolButton::DelayedPopup, "RibbonToolBarControl::addMenu", "The use of mode is not valid");

    if (mode == QToolButton::DelayedPopup)
        return Q_NULL;

    QAction *action = menu->menuAction();
    action->setIcon(icon);
    menu->setTitle(text);

    RibbonControl::addAction(action);

    RibbonButton *button = qobject_cast<RibbonButton *>(widgetByAction(action));
    if (button != Q_NULL) {
        button->setPopupMode(mode);
        button->setToolButtonStyle(Qt::ToolButtonIconOnly);
    }
    return action;
}

QAction *RibbonToolBarControl::addAction(QAction *action, Qt::ToolButtonStyle style, QMenu *menu,
                                         QToolButton::ToolButtonPopupMode mode)
{
    addAction(action);
    RibbonButton *button = qobject_cast<RibbonButton *>(widgetByAction(action));
    Q_ASSERT(button != Q_NULL);
    button->setToolButtonStyle(style);
    if (menu || action->menu())
        button->setPopupMode(mode);
    if (menu)
        button->setMenu(menu);
    return action;
}

QAction *RibbonToolBarControl::insertAction(QAction *before, QAction *action, Qt::ToolButtonStyle style, QMenu *menu,
                                            QToolButton::ToolButtonPopupMode mode)
{
    insertAction(before, action);
    RibbonButton *button = qobject_cast<RibbonButton *>(widgetByAction(action));
    Q_ASSERT(button != Q_NULL);
    button->setToolButtonStyle(style);
    if (menu || action->menu())
        button->setPopupMode(mode);
    if (menu)
        button->setMenu(menu);
    return action;
}

QAction *RibbonToolBarControl::addAction(const QString &text)
{
    QAction *action = new QAction(text, this);
    RibbonControl::addAction(action);
    return action;
}

QAction *RibbonToolBarControl::addAction(const QIcon &icon, const QString &text)
{
    QAction *action = new QAction(icon, text, this);
    RibbonControl::addAction(action);
    return action;
}

QAction *RibbonToolBarControl::addAction(const QString &text, const QObject *receiver, const char *member)
{
    QAction *action = new QAction(text, this);
    QObject::connect(action, SIGNAL(triggered(bool)), receiver, member);
    RibbonControl::addAction(action);
    return action;
}

QAction *RibbonToolBarControl::addAction(const QIcon &icon, const QString &text, const QObject *receiver,
                                         const char *member)
{
    QAction *action = new QAction(icon, text, this);
    QObject::connect(action, SIGNAL(triggered(bool)), receiver, member);
    RibbonControl::addAction(action);
    return action;
}

QAction *RibbonToolBarControl::addAction(const QIcon &icon, const QString &text, Qt::ToolButtonStyle style, QMenu *menu,
                                         QToolButton::ToolButtonPopupMode mode)
{
    QAction *action = Q_NULL;
    if (mode == QToolButton::InstantPopup && menu) {
        action = menu->menuAction();
        action->setIcon(icon);
        menu->setTitle(text);
    } else {
        action = new QAction(icon, text, this);
    }

    RibbonControl::addAction(action);

    RibbonButton *button = qobject_cast<RibbonButton *>(widgetByAction(action));
    if (button != Q_NULL)
        button->setToolButtonStyle(style);

    return action;
}

QAction *RibbonToolBarControl::addSeparator()
{
    QAction *action = new QAction(this);
    action->setSeparator(true);
    RibbonControl::addAction(action);
    return action;
}

QSize RibbonToolBarControl::sizeHint() const
{
    QTC_D(const RibbonToolBarControl);
    return d->m_sizeHint;
}

QWidget *RibbonToolBarControl::widgetByAction(QAction *action)
{
    QTC_D(RibbonToolBarControl);
    for (RibbonToolBarControlPrivate::RibbonToolBarActionGroupList::iterator it = d->m_groups.begin();
         it != d->m_groups.end(); ++it) {
        RibbonToolBarControlPrivate::RibbonToolBarActionsGroup &group = *it;
        RibbonToolBarControlPrivate::RibbonToolBarActionGroupItems &items = group.items;
        int index = indexOfAction(items, action);
        if (index != -1)
            return items[index].widget;
    }
    return Q_NULL;
}

void RibbonToolBarControl::updateLayout()
{
    QTC_D(RibbonToolBarControl);
    if (parentGroup() == NULL)
        return;
    if (d->m_vecWidths.isEmpty())
        return;

    if (RibbonBar *ribbonBar = parentGroup()->ribbonBar()) {
        QSize sz;
        const int currWidthIndex = rowsCount() == 2 ? 0 : 1;
        const int currPanelWidth = d->m_vecWidths[currWidthIndex] + 2 * d->m_margin;
        sz.setWidth(currPanelWidth);
        const int vertMargin = 3;
        const int height =
            (ribbonBar->rowItemHeight() * ribbonBar->rowItemCount()) + 2 * (d->m_margin / 2) + vertMargin;
        sz.setHeight(height);
        d->updateGroupsGeometry(QRect(QPoint(0, 0), sz));
        d->m_sizeHint = sz;
        d->m_sizeHint.setWidth(sz.width() - d->m_margin / 2);
    }
}

bool RibbonToolBarControl::event(QEvent *event)
{
    QTC_D(RibbonToolBarControl);
    if (d->m_dirty && event->type() == QEvent::LayoutRequest) {
        d->updateLayoutParent();
        d->m_dirty = false;
    }
    return RibbonControl::event(event);
}

void RibbonToolBarControl::actionEvent(QActionEvent *event)
{
    QTC_D(RibbonToolBarControl);
    QAction *action = event->action();

    switch (event->type()) {
    case QEvent::ActionAdded: {
        QWidget *widget = Q_NULL;
        if (!action->isSeparator()) {
            QWidgetAction *widgetAction = qobject_cast<QWidgetAction *>(action);
            if (widgetAction)
                widget = widgetAction->requestWidget(this);

            if (widget == Q_NULL) {
                RibbonButton *button = new RibbonButton(this);
                button->setAutoRaise(true);
                button->setFocusPolicy(Qt::NoFocus);
                button->setDefaultAction(action);
                button->setToolButtonStyle(Qt::ToolButtonIconOnly);
                // QObject::connect(button, SIGNAL(triggered(QAction*)), parentWidget(),
                // SIGNAL(actionTriggered(QAction*)));
                widget = button;
            }
            widget->hide();
        } else {
            RibbonToolBarSeparatorControlPrivate *separator = new RibbonToolBarSeparatorControlPrivate(this);
            widget = separator;
        }

        d->addActionToGroup(action, widget);
        d->m_dirty = true;
        break;
    }

    case QEvent::ActionChanged:
        d->m_dirty = true;
        break;
    case QEvent::ActionRemoved: {
        d->removeActionFromGroup(action);
        d->m_dirty = true;
    } break;
    default:
        break;
    }
}

void RibbonToolBarControl::changeEvent(QEvent *event)
{
    QTC_D(RibbonToolBarControl);
    switch (event->type()) {
    case QEvent::StyleChange:
        d->updateLayoutParent(false);
        updateLayout();
        break;
    case QEvent::FontChange:
        d->updateLayoutParent(false);
        updateLayout();
        break;
    default:
        break;
    }
    RibbonControl::changeEvent(event);
}

void RibbonToolBarControl::setMargin(int margin)
{
    QTC_D(RibbonToolBarControl);
    d->m_margin = margin;
}

int RibbonToolBarControl::margin() const
{
    QTC_D(const RibbonToolBarControl);
    return d->m_margin;
}
