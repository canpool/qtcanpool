/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 **/
#include "ribbongroup.h"
#include "officepopupcolorbutton.h"
#include "officepopupmenu.h"
#include "ribbon_p.h"
#include "ribbonbar_p.h"
#include "ribbonbutton_p.h"
#include "ribbonbuttoncontrols.h"
#include "ribboncontrols_p.h"
#include "ribbongallery.h"
#include "ribbongallerycontrols.h"
#include "ribbongroup_p.h"
#include "ribbonpage.h"
#include "ribbonpage_p.h"
#include "ribbonstyle.h"
#include "stylehelpers.h"
#include <QApplication>
#include <QComboBox>
#include <QDesktopWidget>
#include <QLineEdit>
#include <QListView>
#include <QPainter>
#include <QToolBar>

QRIBBON_USE_NAMESPACE

#define ADDITIVE_WIDTH 20

static void setSizeDefinitionByStyle(RibbonButtonControl *control, Qt::ToolButtonStyle style)
{
    if (Qt::ToolButtonTextUnderIcon == style) {
        control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)
            ->setImageSize(RibbonControlSizeDefinition::ImageLarge);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setLabelVisible(true);
    } else if (Qt::ToolButtonTextBesideIcon == style) {
        control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)
            ->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupPopup)
            ->setImageSize(RibbonControlSizeDefinition::ImageSmall);
    } else if (Qt::ToolButtonIconOnly == style) {
        control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)
            ->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupPopup)
            ->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setLabelVisible(false);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setLabelVisible(false);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setLabelVisible(false);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupPopup)->setLabelVisible(false);
    }
}

/* RibbonButtonAction */
class RibbonButtonAction : public QWidgetAction
{
public:
    explicit RibbonButtonAction(QObject *parent, const QIcon &icon, const QString &text, Qt::ToolButtonStyle style,
                                QMenu *menu, QToolButton::ToolButtonPopupMode mode = QToolButton::DelayedPopup);
    virtual ~RibbonButtonAction();
protected:
    virtual QWidget *createWidget(QWidget *parent);
private:
    Qt::ToolButtonStyle m_style;
    QToolButton::ToolButtonPopupMode m_mode;
};

RibbonButtonAction::RibbonButtonAction(QObject *parent, const QIcon &icon, const QString &text,
                                       Qt::ToolButtonStyle style, QMenu *menu, QToolButton::ToolButtonPopupMode mode)
    : QWidgetAction(parent), m_style(style), m_mode(mode)
{
    setIcon(icon);
    setText(text);
    setMenu(menu);
}

RibbonButtonAction::~RibbonButtonAction() {}

QWidget *RibbonButtonAction::createWidget(QWidget *parent)
{
    if (QToolBar *toolBar = qobject_cast<QToolBar *>(parent)) {
        RibbonButton *button = new RibbonButton(parent);

        button->setAutoRaise(true);
        button->setFocusPolicy(Qt::NoFocus);
        button->setIconSize(toolBar->iconSize());
        button->setToolButtonStyle(toolBar->toolButtonStyle());
        QObject::connect(toolBar, SIGNAL(iconSizeChanged(QSize)), button, SLOT(setIconSize(QSize)));
        QObject::connect(toolBar, SIGNAL(toolButtonStyleChanged(Qt::ToolButtonStyle)), button,
                         SLOT(setToolButtonStyle(Qt::ToolButtonStyle)));
        button->setDefaultAction(this);
        QObject::connect(button, SIGNAL(triggered(QAction *)), toolBar, SIGNAL(actionTriggered(QAction *)));
        return button;
    } else if (RibbonGroup *ribbonGroup = qobject_cast<RibbonGroup *>(parent)) {
        RibbonButtonControl *control = new RibbonButtonControl(ribbonGroup);
        ::setSizeDefinitionByStyle(control, m_style);
        control->setDefaultAction(this);
        control->setToolButtonStyle(m_style);

        if (menu()) {
            control->setPopupMode(m_mode);
            control->setMenu(menu());
            menu()->setDefaultAction(this);
        }

        return control;
    }
    return Q_NULL;
}

/* RibbonGroupPrivate */
RibbonGroupPrivate::RibbonGroupPrivate()
{
    m_dirty = false;
    m_butOptionVisible = false;
    m_optionButton = Q_NULL;
    m_actOption = Q_NULL;
    m_groupPopup = Q_NULL;
    m_toolBar = Q_NULL;
    m_groupScrollPos = 0;
    m_pageScrollPos = 0;
    m_font = Q_NULL;
    m_currentSize = RibbonControlSizeDefinition::GroupLarge;
    m_titleElideMode = Qt::ElideRight;
    m_controlsAlignment = Qt::AlignHCenter;
}

RibbonGroupPrivate::~RibbonGroupPrivate()
{
    Q_DELETE_AND_NULL(m_groupPopup);
    Q_DELETE_AND_NULL(m_font);
}

void RibbonGroupPrivate::init()
{
    QTC_Q(RibbonGroup);
    q->setMouseTracking(true);
    q->setProperty(_qtc_TitleGroupsVisible, true);

    m_buttonScrollGroupLeft = new RibbonGroupScroll(q, true);
    m_buttonScrollGroupLeft->setVisible(false);

    m_buttonScrollGroupRight = new RibbonGroupScroll(q, false);
    m_buttonScrollGroupRight->setVisible(false);

    QObject::connect(m_buttonScrollGroupLeft, SIGNAL(pressed()), this, SLOT(pressLeftScrollButton()));
    QObject::connect(m_buttonScrollGroupRight, SIGNAL(pressed()), this, SLOT(pressRightScrollButton()));

    m_optionButton = new RibbonGroupOption(q);
    m_actOption = new QAction(q);
    m_actOption->setObjectName("__qtc_Action_Option");
    m_optionButton->setDefaultAction(m_actOption);
    QObject::connect(m_optionButton, SIGNAL(triggered(QAction *)), q, SIGNAL(actionTriggered(QAction *)));
    m_optionButton->hide();
}

void RibbonGroupPrivate::clear(bool deleteControls)
{
    QTC_Q(RibbonGroup);
    if (q->isControlsGrouping()) {
        m_toolBar->clear();
        return;
    }

    QList<QAction *> actions = q->actions();
    for (int i = 0; i < actions.size(); ++i)
        q->removeAction(actions.at(i));

    while (m_controls.size() > 0) {
        RibbonControl *control = m_controls[0];
        if (RibbonWidgetControl *widgetcontrol = qobject_cast<RibbonWidgetControl *>(control))
            widgetcontrol->qtc_d()->clearWidget();

        q->removeControl(control);

        if (deleteControls)
            delete control;
    }
}

void RibbonGroupPrivate::enableGroupScroll(bool scrollLeft, bool scrollRight)
{
    QTC_Q(RibbonGroup);

    Q_ASSERT(m_buttonScrollGroupLeft);
    Q_ASSERT(m_buttonScrollGroupRight);

    if (!m_buttonScrollGroupRight || !m_buttonScrollGroupLeft)
        return;

    const RibbonBar *rb = q->ribbonBar();
    Q_ASSERT(rb != NULL);

    const int heightGroup = rb->rowItemHeight() * rb->rowItemCount();

    QRect rcGroup(q->geometry());
    rcGroup.setHeight(heightGroup + 4);

    if (scrollLeft) {
        m_buttonScrollGroupLeft->raise();
        m_buttonScrollGroupLeft->setVisible(true);
        m_buttonScrollGroupLeft->setGeometry(
            QRect(QPoint(rcGroup.left() - 1, rcGroup.top()), QPoint(rcGroup.left() + 12, rcGroup.bottom())));
    } else
        m_buttonScrollGroupLeft->setVisible(false);

    if (scrollRight) {
        m_buttonScrollGroupRight->raise();
        m_buttonScrollGroupRight->setVisible(true);
        m_buttonScrollGroupRight->setGeometry(
            QRect(QPoint(rcGroup.right() - 12, rcGroup.top()), QPoint(rcGroup.right() + 1, rcGroup.bottom())));
    } else
        m_buttonScrollGroupRight->setVisible(false);
}

void RibbonGroupPrivate::showGroupScroll()
{
    QTC_Q(RibbonGroup);
    if (q->isReduced() && q->isVisible()) {
        QRect screen = QApplication::desktop()->availableGeometry(q);
        int totalWidth = q->layout()->minimumSize().width();
        int groupLength = screen.width();
        int scrollPos = m_groupScrollPos;

        if (totalWidth > groupLength) {
            if (scrollPos > totalWidth - groupLength)
                scrollPos = totalWidth - groupLength;
        } else
            scrollPos = 0;

        if (scrollPos < 0)
            scrollPos = 0;

        m_groupScrollPos = scrollPos;
        enableGroupScroll(scrollPos > 0, totalWidth - groupLength - scrollPos > 0);
    }
}

void RibbonGroupPrivate::initStyleOption(QStyleOptionGroupBox &opt) const
{
    QTC_Q(const RibbonGroup)
    opt.init(q);
    opt.text = m_title;
    if (m_font)
        opt.fontMetrics = QFontMetrics(*m_font);
    opt.lineWidth = q->style()->pixelMetric((QStyle::PixelMetric)RibbonStyle::PM_RibbonHeightCaptionGroup, &opt, q);
    opt.textAlignment = Qt::AlignHCenter | Qt::AlignVCenter;
}

int RibbonGroupPrivate::calcMinimumWidth(RibbonControlSizeDefinition::GroupSize size) const
{
    QTC_Q(const RibbonGroup)

    QStyleOptionToolButton opt;
    opt.init(q);   // opt.text = m_title;
    int width = q->style()->pixelMetric((QStyle::PixelMetric)RibbonStyle::PM_RibbonReducedGroupWidth, &opt, q);

    if (size == RibbonControlSizeDefinition::GroupSmall)
        width += 1;
    else if (size == RibbonControlSizeDefinition::GroupMedium)
        width += 2;
    else if (size == RibbonControlSizeDefinition::GroupLarge)
        width += 3;
    return width;
}

int RibbonGroupPrivate::arrangeRowContent(QList<RibbonControl *> &row, int leftOffset, int rowItemHeight,
                                          int rowItemCount, QWidget *parent) const
{
    QTC_Q(const RibbonGroup)
    static int margin = 4;

    int max = 0;
    int rowHeight = rowItemHeight;
    int topOffset = 0;

    if (q->contentAlignment() & Qt::AlignVCenter)
        topOffset = (rowItemHeight * (rowItemCount - row.size())) * 0.5;
    else if (q->contentAlignment() & Qt::AlignBottom)
        topOffset = rowItemHeight * (rowItemCount - row.size());

    for (QList<RibbonControl *>::const_iterator it = row.constBegin(); it != row.constEnd(); ++it)
        max = qMax((*it)->sizeHint().width(), max);

    if (parent == Q_NULL) {
        row.clear();
        return max;
    }

    topOffset += margin / 2;
    leftOffset += margin;

    for (QList<RibbonControl *>::const_iterator it = row.constBegin(); it != row.constEnd(); ++it) {
        QSize size = (*it)->sizeHint();
        size.rheight() = qMax(rowHeight, size.height());
        const bool stretchable = (*it)->sizeDefinition((*it)->currentSize())->isStretchable();
        QRect r = QRect(leftOffset, topOffset, max, rowHeight);
        size.rwidth() = stretchable ? max : size.width();

        QRect controlRect = QStyle::alignedRect(q->layoutDirection(), q->contentAlignment(), size, r);
        (*it)->setGeometry(controlRect);
        topOffset += rowHeight;
    }
    row.clear();
    return max;
}

void RibbonGroupPrivate::reposition(const QRect &rect)
{
    QTC_Q(RibbonGroup)
    QSize sz(rect.width(), q->sizeHint().height());
    q->setGeometry(QRect(rect.topLeft(), /*p.sizeHint()*/ sz));
    if (q->isReduced()) {
        if (m_groupPopup == Q_NULL) {
            q->setUpdatesEnabled(true);
            m_groupPopup = new RibbonGroupPopup(q);
            m_groupPopup->setFocusPolicy(Qt::NoFocus);
            QObject::connect(m_groupPopup, SIGNAL(aboutToHide()), q, SLOT(update()));
        }
    } else {
        if (m_groupPopup) {
            QObject::disconnect(m_groupPopup, SIGNAL(aboutToHide()), q, SLOT(update()));
            Q_DELETE_AND_NULL(m_groupPopup);
        }

#ifdef QRIBBON_DESIGNER
        if ((p.parentWidget()->windowFlags() & Qt::Popup))
            q->setFixedSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
#endif   // QRIBBON_DESIGNER
#ifdef QRIBBON_DESIGNER
        QSize sz = rect.size();
        if (!sz.isEmpty() && !(p.parentWidget()->windowFlags() & Qt::Popup))
            q->setFixedSize(sz);
#endif   // QRIBBON_DESIGNER
    }
}

void RibbonGroupPrivate::updateOptionButtonLayout()
{
    QTC_Q(RibbonGroup);
    if (m_optionButton && m_butOptionVisible) {
        QStyleOption opt;
        opt.init(q);
        int width = q->style()->pixelMetric((QStyle::PixelMetric)RibbonStyle::PM_RibbonHeightCaptionGroup, &opt, q);
        opt.rect.setWidth(q->width());

        QRect rc = opt.rect;
        QSize sz(qMax(width - 3, m_optionButton->width()), qMax(width - 3, m_optionButton->height()));
        QRect rectButOption(QPoint(rc.right() - sz.width() - 2, rc.bottom() - sz.height() - 1), sz);
        m_optionButton->setGeometry(rectButOption);
        m_optionButton->setVisible(!q->isReduced() && q->property(_qtc_TitleGroupsVisible).toBool());
    }
}

QSize RibbonGroupPrivate::updateControlsLayout(QWidget *parent, int leftOffset) const
{
    QTC_Q(const RibbonGroup);

    const RibbonBar *rb = q->ribbonBar();
    if (rb == NULL)
        return QSize();

    static int margin = 4;
    const int maxRows = rb->rowItemCount();   // Get max row count from RibbonBar here.
    const int rowItemHeight = rb->rowItemHeight();

    QList<RibbonControl *> rowControls;
    QList<RibbonControl *> controls = m_controls;

#if QT_VERSION < QT_VERSION_CHECK(5, 2, 0)
    qSort(controls.begin(), controls.end(), RibbonGroupPrivate::visualIndexLessThan);
#else
    std::sort(controls.begin(), controls.end(), RibbonGroupPrivate::visualIndexLessThan);
#endif

    for (QList<RibbonControl *>::const_iterator it = controls.constBegin(); it != controls.constEnd(); ++it) {
        RibbonControl *ribbonControl = *it;
        RibbonColumnBreakControl *sepatator = qobject_cast<RibbonColumnBreakControl *>(ribbonControl);
        bool visible = ribbonControl->sizeDefinition(ribbonControl->currentSize())->isVisible();
        if (ribbonControl->defaultAction() != Q_NULL)
            visible = visible && ribbonControl->defaultAction()->isVisible();
        if (parent != Q_NULL) {
            ribbonControl->setParent(parent);
            if (sepatator == Q_NULL)
                ribbonControl->setVisible(visible);
            else
                ribbonControl->setVisible(visible &&
                                          sepatator->sizeDefinition(sepatator->currentSize())->showSeparator());
        }

        if (!visible)
            continue;

        ribbonControl->updateLayout();
        QSize sizeHint = ribbonControl->sizeHint();
        if (sepatator != Q_NULL || sizeHint.height() > rowItemHeight) {
            if (rowControls.size() > 0)
                leftOffset += arrangeRowContent(rowControls, leftOffset, rowItemHeight, maxRows, parent);

            if (sepatator != Q_NULL) {
                if (!sepatator->sizeDefinition(sepatator->currentSize())->showSeparator())
                    sizeHint = QSize(0, 0);
            }

            if (parent != Q_NULL) {
                int maxHeight = sizeHint.height();
                if (maxHeight > rowItemHeight * maxRows)
                    maxHeight = rowItemHeight * maxRows;
                else
                    maxHeight = qMax(rowItemHeight, sizeHint.height());

                ribbonControl->setGeometry(QRect(leftOffset + margin, margin / 2, sizeHint.width(), maxHeight));
            }
            leftOffset += sizeHint.width();
        } else {
            rowControls.append(ribbonControl);
            if (rowControls.size() >= maxRows)
                leftOffset += arrangeRowContent(rowControls, leftOffset, rowItemHeight, maxRows, parent);
        }
    }

    if (rowControls.size() > 0)
        leftOffset += arrangeRowContent(rowControls, leftOffset, rowItemHeight, maxRows, parent);

    leftOffset += (margin * 2 + 1);

    int minWidth = calcMinimumWidth(q->currentSize());
    if (minWidth > leftOffset)
        leftOffset = minWidth;

    return QSize(leftOffset, rowItemHeight * maxRows);
}

void RibbonGroupPrivate::adjustCurrentSize(bool expand)
{
    for (QList<RibbonControl *>::const_iterator it = m_controls.constBegin(); it != m_controls.constEnd(); ++it)
        (*it)->adjustCurrentSize(expand);
}

void RibbonGroupPrivate::updateLayoutParent(bool first)
{
    QTC_Q(const RibbonGroup);
    if (RibbonPage *page = qobject_cast<RibbonPage *>(q->parentWidget())) {
        if (first)
            page->updateLayout();
        else
            page->updateGeometry();
    }
}

QSize RibbonGroupPrivate::sizeHint() const
{
    QTC_Q(const RibbonGroup);

    const RibbonBar *rb = q->ribbonBar();
    if (rb == NULL)
        return QSize();

    QStyleOptionGroupBox opt;
    initStyleOption(opt);

    QSize size = updateControlsLayout(Q_NULL);

    const int heightGroup = qMax(rb->rowItemHeight() * rb->rowItemCount(), size.height());
    const int heightCaptionGroup = opt.lineWidth;

    QSize textSize = opt.fontMetrics.size(Qt::TextShowMnemonic, q->title());
#if QT_VERSION < QT_VERSION_CHECK(5, 11, 0)
    textSize.rwidth() += opt.fontMetrics.width(QLatin1Char(' ')) * 4;
#else
    textSize.rwidth() += opt.fontMetrics.horizontalAdvance(QLatin1Char(' ')) * 4;
#endif

    if (m_optionButton->isVisible())
        textSize.rwidth() += m_optionButton->sizeHint().width();

    if (q->minimumWidth() == 0)
        size.rwidth() = qMax(textSize.width(), size.width());
    else
        size.rwidth() = qMax(q->minimumWidth(), size.width());

    //    else if (p.titleElideMode() == Qt::ElideNone)
    //        size.rwidth() = qMax(p.minimumWidth(), size.width());

    if (q->isMinimized() && q->isVisible()) {
        QRect screen = QApplication::desktop()->availableGeometry(q);
        size.rwidth() = qMin(screen.width(), size.width());
    }

    size.setHeight(heightGroup + heightCaptionGroup + 2 * 2);
    return size;
}

void RibbonGroupPrivate::setReductionSize(RibbonControlSizeDefinition::GroupSize size)
{
    if (m_currentSize == size)
        return;

    m_currentSize = size;

    for (QList<RibbonControl *>::const_iterator it = m_controls.constBegin(); it != m_controls.constEnd(); ++it)
        (*it)->sizeChanged(size);
}

void RibbonGroupPrivate::reduce()
{
    Q_ASSERT(canReduce());
    setReductionSize((RibbonControlSizeDefinition::GroupSize)(m_currentSize + 1));
}

void RibbonGroupPrivate::expand()
{
    Q_ASSERT(canExpand());
    setReductionSize((RibbonControlSizeDefinition::GroupSize)(m_currentSize - 1));
}

bool RibbonGroupPrivate::canReduce() { return m_currentSize < RibbonControlSizeDefinition::GroupPopup; }

bool RibbonGroupPrivate::canExpand() { return m_currentSize > RibbonControlSizeDefinition::GroupLarge; }

static void listPageWidth(int totalWidth, int realWidth, QList<int> &pagesWidth)
{
    if (totalWidth > realWidth) {
        pagesWidth << realWidth;
        listPageWidth(totalWidth - realWidth, realWidth, pagesWidth);
    } else
        pagesWidth << totalWidth;
    return;
}

void RibbonGroupPrivate::pressLeftScrollButton()
{
    QTC_Q(RibbonGroup)
    QList<int> pagesWidth;
    listPageWidth(q->layout()->sizeHint().width(), q->geometry().width(), pagesWidth);

    if (0 < m_pageScrollPos)
        m_pageScrollPos--;

    m_groupScrollPos -= pagesWidth[m_pageScrollPos];
    showGroupScroll();
    q->layout()->update();
}

void RibbonGroupPrivate::pressRightScrollButton()
{
    QTC_Q(RibbonGroup)
    QList<int> pagesWidth;
    listPageWidth(q->layout()->sizeHint().width(), q->geometry().width(), pagesWidth);

    m_groupScrollPos += pagesWidth[m_pageScrollPos];
    showGroupScroll();
    q->layout()->update();

    if (pagesWidth.size() - 1 > m_pageScrollPos)
        m_pageScrollPos++;
}

void RibbonGroupPrivate::hidePopupMode(QAction *action)
{
    Q_UNUSED(action);
    QTC_Q(RibbonGroup)
    q->setVisible(false);
}

bool RibbonGroupPrivate::visualIndexLessThan(RibbonControl *first, RibbonControl *second)
{
    int v1 = first->sizeDefinition(first->currentSize())->visualIndex();
    if (v1 == -1) {
        RibbonGroup *group = first->parentGroup();
        v1 = group->qtc_d()->m_controls.indexOf(first);
    }

    int v2 = second->sizeDefinition(second->currentSize())->visualIndex();
    if (v2 == -1) {
        RibbonGroup *group = second->parentGroup();
        v2 = group->qtc_d()->m_controls.indexOf(second);
    }
    return v1 < v2;
}

RibbonBar *RibbonGroup::ribbonBar() const
{
    if (RibbonPage *page = qobject_cast<RibbonPage *>(parentWidget()))
        return qobject_cast<RibbonBar *>(page->parentWidget());
    return Q_NULL;
}

/* RibbonGroup */
RibbonGroup::RibbonGroup(RibbonPage *page, const QString &title) : QWidget(page)
{
    QTC_INIT_PRIVATE(RibbonGroup);
    QTC_D(RibbonGroup);
    d->init();
    setTitle(title);
}

RibbonGroup::RibbonGroup(QWidget *parent) : QWidget(parent)
{
    QTC_INIT_PRIVATE(RibbonGroup);
    QTC_D(RibbonGroup);
    d->init();
}

RibbonGroup::~RibbonGroup()
{
    QTC_D(RibbonGroup);
    d->clear(true);
    QTC_FINI_PRIVATE();
}

/*!
Returns true if the group is reduced->
*/
bool RibbonGroup::isReduced() const
{
    QTC_D(const RibbonGroup);
    if (d->m_controls.size() == 1) {
        if (!d->m_controls.at(0)->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->isStretchable())
            return false;
    }
    return d->m_currentSize == RibbonControlSizeDefinition::GroupPopup;
}

/*!
Returns the title of the group.
*/
const QString &RibbonGroup::title() const
{
    QTC_D(const RibbonGroup);
    return d->m_title;
}

/*!
Sets the \a title for the group.
 */
void RibbonGroup::setTitle(const QString &title)
{
    QTC_D(RibbonGroup);

    if (d->m_title == title)
        return;

    d->m_title = title;
    adjustSize();
    emit titleChanged(d->m_title);
}

/*!
Returns the title font of the group.
*/
const QFont &RibbonGroup::titleFont() const
{
    QTC_D(const RibbonGroup);
    return d->m_font != Q_NULL ? *d->m_font : font();
}

/*!
Sets the \a title font for the group.
 */
void RibbonGroup::setTitleFont(const QFont &font)
{
    QTC_D(RibbonGroup);

    if (d->m_font && *d->m_font == font)
        return;

    Q_DELETE_AND_NULL(d->m_font);
    d->m_font = new QFont(font);

    if (RibbonBar *ribBar = ribbonBar()) {
        ribBar->updateLayout();
        if (ribBar->parentWidget()) {
            if (QLayout *layout = ribBar->parentWidget()->layout())
                layout->update();
        }
    }
    emit titleFontChanged(*d->m_font);
}

/*!
Returns the title color of the group.
*/
const QColor &RibbonGroup::titleColor() const
{
    QTC_D(const RibbonGroup);
    return d->m_color;
}

/*!
Sets the \a title color for the group.
 */
void RibbonGroup::setTitleColor(const QColor &color)
{
    QTC_D(RibbonGroup);

    if (d->m_color == color)
        return;
    d->m_color = color;
    update();
}

/*!
Returns the icon of the group.
*/
const QIcon &RibbonGroup::icon() const
{
    QTC_D(const RibbonGroup);
    return d->m_icon;
}

/*!
Sets the \a icon for the group.
 */
void RibbonGroup::setIcon(const QIcon &icon)
{
    QTC_D(RibbonGroup);
    d->m_icon = icon;
    adjustSize();
}

/*!
Returns the visible state for the option button used in the caption of the group.
*/
bool RibbonGroup::isOptionButtonVisible() const
{
    QTC_D(const RibbonGroup);
    return d->m_optionButton->isVisible();
}

/*!
Sets the visible state for the option button used in the group caption.
Notes: Option button is usually used to start the dialogue associated with the group.
*/
void RibbonGroup::setOptionButtonVisible(bool visible)
{
    QTC_D(RibbonGroup);
    d->m_butOptionVisible = visible;
    d->m_optionButton->setVisible(visible);
    d->updateOptionButtonLayout();
    adjustSize();
}

/*!
Returns the pointer to action associated with the option button.
Notes: Use this action if you want to initialize tooltips, the text in the status bar (StatusTip), icons or for to
define custom events.
*/
QAction *RibbonGroup::optionButtonAction() const
{
    QTC_D(const RibbonGroup);
    return d->m_actOption;
}

/*!
  Sets the given \a action for the option button of the group. Option button is a small button on the bottom of the
  group. If action is not set then the option button is not visible.
*/
void RibbonGroup::setOptionButtonAction(QAction *action)
{
    QTC_D(RibbonGroup);
    if (d->m_actOption != NULL)
        delete d->m_actOption;

    d->m_actOption = action;
    d->m_optionButton->setDefaultAction(d->m_actOption);
}

/*!
Returns the enum that describes the ribbon control alignment for the control's column within the given group.
*/
Qt::Alignment RibbonGroup::contentAlignment() const
{
    QTC_D(const RibbonGroup);
    return d->m_contentAlignment;
}

/*!
Sets the enum that describes the ribbon control alignment for the control's column within the given group.
*/
void RibbonGroup::setContentAlignment(Qt::Alignment alignment)
{
    QTC_D(RibbonGroup);
    d->m_contentAlignment = alignment;
}

/*!
Returns the alignment of block ribbon controls if width of the all controls is more than minmum width of the group.
*/
Qt::Alignment RibbonGroup::controlsAlignment() const
{
    QTC_D(const RibbonGroup);
    return d->m_controlsAlignment;
}

/*!
Sets the alignment of the block ribbon controls.
*/
void RibbonGroup::setControlsAlignment(Qt::Alignment alignment)
{
    QTC_D(RibbonGroup);
    if (d->m_controlsAlignment != alignment) {
        d->m_controlsAlignment = alignment;
        if (parentWidget()) {
            int controlsLeftOffset = 0;
            d->updateControlsLayout(parentWidget(), controlsLeftOffset);
        }
    }
}

/*!
Returns the count of ribbon controls within the group.
\sa controlByWidget, controlByIndex, controlByAction
*/
int RibbonGroup::controlCount() const
{
    QTC_D(const RibbonGroup);
    return d->m_controls.size();
}

void RibbonGroup::setControlsCentering(bool controlsCentering)
{
    QTC_D(RibbonGroup);
    if (controlsCentering)
        d->m_contentAlignment = Qt::AlignVCenter;
    else
        d->m_contentAlignment = Qt::Alignment();
}

bool RibbonGroup::isControlsCentering() const
{
    QTC_D(const RibbonGroup);
    return d->m_contentAlignment.testFlag(Qt::AlignVCenter);
}

void RibbonGroup::setControlsGrouping(bool controlsGrouping)
{
    QTC_D(RibbonGroup);
    if (controlsGrouping) {
        if (isControlsGrouping())
            return;
        d->m_toolBar = new RibbonToolBarControl(this);
        addControl(d->m_toolBar);
    } else {
        if (d->m_toolBar != Q_NULL)
            removeControl(d->m_toolBar);
        Q_DELETE_AND_NULL(d->m_toolBar);
    }
}

bool RibbonGroup::isControlsGrouping() const
{
    QTC_D(const RibbonGroup);
    return d->m_toolBar != Q_NULL;
}

/*!
Returns the ribbon control by \a index.
\sa controlByWidget, controlByIndex, controlByAction
*/
RibbonControl *RibbonGroup::controlByIndex(int index) const
{
    QTC_D(const RibbonGroup);
    return d->m_controls[index];
}

/*!
Returns the ribbon control by \a action.
\sa controlCount, controlByIndex, controlByWidget
*/
RibbonControl *RibbonGroup::controlByAction(QAction *action) const
{
    QTC_D(const RibbonGroup);
    Q_ASSERT(action != Q_NULL);
    for (QList<RibbonControl *>::const_iterator it = d->m_controls.constBegin(); it != d->m_controls.constEnd(); ++it) {
        if ((*it)->defaultAction() == action)
            return *it;
    }
    return Q_NULL;
}

/*!
Returns the widget ribbon control by \a widget.
\sa controlCount, controlByIndex, controlByAction
*/
RibbonWidgetControl *RibbonGroup::controlByWidget(QWidget *widget) const
{
    QTC_D(const RibbonGroup);
    Q_ASSERT(widget != Q_NULL);
    for (QList<RibbonControl *>::const_iterator it = d->m_controls.constBegin(); it != d->m_controls.constEnd(); ++it) {
        if (RibbonWidgetControl *widgetControl = qobject_cast<RibbonWidgetControl *>((*it))) {
            if (widgetControl->contentWidget() == widget)
                return widgetControl;
        }
    }
    return Q_NULL;
}

RibbonControlSizeDefinition::GroupSize RibbonGroup::currentSize() const
{
    QTC_D(const RibbonGroup);
    return d->m_currentSize;
}

/*!
Adds the ribbon \a control to the group. In most cases you have to use addAction to create and add ribbon control.
*/
void RibbonGroup::addControl(RibbonControl *control)
{
    QTC_D(RibbonGroup);
    if (d->m_controls.indexOf(control) != -1)
        return;   // Already was added.

    control->qtc_d()->m_parentGroup = this;

    d->m_controls.append(control);
    control->setFont(font());
    control->sizeChanged(currentSize());

    RibbonPage *page = qobject_cast<RibbonPage *>(parentWidget());
    if (page != Q_NULL)
        page->qtc_d()->updateLayout();
}

/*!
Removes the ribbon \a control from the group.
*/
void RibbonGroup::removeControl(RibbonControl *control)
{
    QTC_D(RibbonGroup);
    if (d->m_controls.removeAll(control) > 0) {
        control->qtc_d()->m_parentGroup = Q_NULL;
        control->setParent(Q_NULL);

        RibbonPage *page = qobject_cast<RibbonPage *>(parentWidget());
        if (page != Q_NULL)
            page->qtc_d()->updateLayout();
    }
}

/*!
  Creates and adds the action to the group with the given \a icon, \a text and \a style. If you specify the parameter \a
  menu, then will be constructed a special split button which will allow to cause this menu. Parameter \a mode defines
  the mode of the popup emerging.
*/
QAction *RibbonGroup::addAction(const QIcon &icon, const QString &text, Qt::ToolButtonStyle style, QMenu *menu,
                                QToolButton::ToolButtonPopupMode mode)
{
    QTC_D(const RibbonGroup);

    if (isControlsGrouping())
        return d->m_toolBar->addAction(icon, text, style, menu, mode);

    RibbonButtonAction *action = new RibbonButtonAction(this, icon, text, style, menu, mode);
    addAction(action);
    return action;
}

/*!
  Adds the \a action to the group with the given parameters \a style, \a menu and \a mode.
*/
QAction *RibbonGroup::addAction(QAction *action, Qt::ToolButtonStyle style, QMenu *menu,
                                QToolButton::ToolButtonPopupMode mode)
{
    QTC_D(RibbonGroup);

    if (isControlsGrouping())
        return d->m_toolBar->addAction(action, style, menu, mode);

    addAction(action);

    RibbonButtonControl *control = qobject_cast<RibbonButtonControl *>(controlByAction(action));
    Q_ASSERT(control != Q_NULL);

    control->setToolButtonStyle(style);
    if (menu || action->menu())
        control->setPopupMode(mode);
    if (menu)
        control->setMenu(menu);

    return action;
}

/*!
  Adds \a action before other action that specified in \a before argument. In result action's widget will be placed
  before widget that is associated with action \a before.
*/
QAction *RibbonGroup::insertAction(QAction *before, QAction *action, Qt::ToolButtonStyle style, QMenu *menu,
                                   QToolButton::ToolButtonPopupMode mode)
{
    QTC_D(RibbonGroup);

    if (isControlsGrouping())
        return d->m_toolBar->insertAction(before, action, style, menu, mode);

    insertAction(before, action);

    RibbonButtonControl *control = qobject_cast<RibbonButtonControl *>(controlByAction(action));
    Q_ASSERT(control != Q_NULL);

    control->setToolButtonStyle(style);
    if (menu || action->menu())
        control->setPopupMode(mode);
    if (menu)
        control->setMenu(menu);

    return action;
}

/*!
Adds the \a widget to the group. As alternative can be created QWidgetAction and added it via addAction method.
*/
QAction *RibbonGroup::addWidget(QWidget *widget)
{
    QTC_D(RibbonGroup);

    if (isControlsGrouping())
        return d->m_toolBar->addWidget(widget);

    return addWidget(QIcon(), QString(""), widget);
}

/*!
Overload method. Adds the \a widget to the group with given \a icon and \a text. As alternative can be created
QWidgetAction and added it via addAction method.
*/
QAction *RibbonGroup::addWidget(const QIcon &icon, const QString &text, QWidget *widget)
{
    QTC_D(RibbonGroup);

    if (isControlsGrouping())
        return d->m_toolBar->addWidget(icon, text, widget);

    QWidgetAction *action = new QWidgetAction(this);
    action->setIcon(icon);
    action->setText(text);

    RibbonWidgetControl *control = new RibbonWidgetControl(this, false);
    control->setContentWidget(widget);
    control->setDefaultAction(action);
    action->setDefaultWidget(control);
    addAction(action);

    return action;
}

/*!
Overload method. Adds the \a widget to the group with given \a icon and \a text.
Parameter \a stretch specifies a the ability to adjust the ribbon control which corresponds to the widget
on the control's column width within the group.
*/
QAction *RibbonGroup::addWidget(const QIcon &icon, const QString &text, bool stretch, QWidget *widget)
{
    QTC_D(RibbonGroup);

    if (isControlsGrouping())
        return d->m_toolBar->addWidget(icon, text, widget);

    QAction *action = addWidget(icon, text, widget);
    if (stretch) {
        RibbonControl *control = controlByAction(action);
        Q_ASSERT(control != Q_NULL);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setStretchable(true);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setStretchable(true);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setStretchable(true);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupPopup)->setStretchable(true);
    }
    return action;
}

QMenu *RibbonGroup::addMenu(const QIcon &icon, const QString &text, Qt::ToolButtonStyle style)
{
    QTC_D(RibbonGroup);

    QMenu *menu = new QMenu(text, this);
    if (isControlsGrouping()) {
        d->m_toolBar->addMenu(icon, text, menu, QToolButton::InstantPopup);
    } else {
        RibbonButtonAction *action = new RibbonButtonAction(this, icon, text, style, menu, QToolButton::InstantPopup);
        addAction(action);
    }
    return menu;
}

/*!
Adds a separator action to the given group.
*/
QAction *RibbonGroup::addSeparator()
{
    QTC_D(RibbonGroup);

    if (isControlsGrouping())
        return d->m_toolBar->addSeparator();

    QAction *action = new QAction(this);
    action->setSeparator(true);
    addAction(action);
    return action;
}

/*!
Removes ribbon control from the group which corresponds to the \a widget.
*/
void RibbonGroup::remove(QWidget *widget)
{
    if (isControlsGrouping()) {
        Q_ASSERT(false);
        // d->m_toolBar->removeWidget(widget);
        return;
    }

    if (!widget)
        return;

    if (RibbonWidgetControl *widgetcontrol = controlByWidget(widget))
        widgetcontrol->qtc_d()->clearWidget();

    QList<QAction *> actions = this->actions();
    for (int i = 0; i < actions.size(); i++) {
        if (QWidgetAction *widgetAction = qobject_cast<QWidgetAction *>(actions.at(i))) {
            if (widgetAction->defaultWidget() == widget) {
                removeAction(actions.at(i));
                return;
            }
        }
    }
}

/*!
Removes all ribbon controls from the given group.
*/
void RibbonGroup::clear()
{
    QTC_D(RibbonGroup);
    d->clear(false);
}

/*!
Returns the enum where ellipsis should be added for group title. If value is Qt::ElideNone then the text will not be
reduced and the group minimum width will equal the size of the text in the group title.
*/
Qt::TextElideMode RibbonGroup::titleElideMode() const
{
    QTC_D(const RibbonGroup);
    return d->m_titleElideMode;
}

/*!
Sets the enum where ellipsis should be added for group title.
*/
void RibbonGroup::setTitleElideMode(Qt::TextElideMode mode)
{
    QTC_D(RibbonGroup);
    if (d->m_titleElideMode != mode) {
        d->m_titleElideMode = mode;
        d->updateLayoutParent(true);
    }
}

QSize RibbonGroup::sizeHint() const
{
    QTC_D(const RibbonGroup);
    QSize size = d->sizeHint();

    if (isReduced()) {
        QStyleOptionToolButton opt;
        opt.init(this);
        opt.text = d->m_title;
        return QSize(style()->pixelMetric((QStyle::PixelMetric)RibbonStyle::PM_RibbonReducedGroupWidth, &opt, this),
                     size.height());
    } else
        return size.expandedTo(QApplication::globalStrut());
}

void RibbonGroup::currentIndexChanged(int index)
{
    Q_UNUSED(index);
    if (isVisible()) {
        bool doReleased = true;
        QComboBox *comboBox = qobject_cast<QComboBox *>(sender());
        if (comboBox) {
            QAbstractItemView *itemView = comboBox->view();
            if (itemView && !itemView->isVisible())
                doReleased = false;
        }
        if (doReleased)
            emit released();
    }
}

void qtc_set_font_to_group_children(RibbonGroup *group, const QFont &font)
{
    for (int i = 0, count = group->controlCount(); count > i; ++i) {
        if (RibbonControl *control = group->controlByIndex(i)) {
            control->setFont(font);
            qtc_set_font_to_ribbon_children(control, font);
        }
    }
}

/*! \reimp */
bool RibbonGroup::event(QEvent *event)
{
    QTC_D(RibbonGroup);

    if (d->m_dirty && event->type() == QEvent::LayoutRequest) {
        d->updateLayoutParent(true);
        d->m_dirty = false;
    }

    switch (event->type()) {
    case QEvent::ChildAdded: {
        QChildEvent *e = static_cast<QChildEvent *>(event);
        if (e->added()) {
            QWidget *widget = qobject_cast<QWidget *>(e->child());
            if (widget != Q_NULL) {
                widget->setFont(font());
                qtc_set_font_to_ribbon_children(widget, font());
            }
        }
    } break;
    case QEvent::StyleChange:
    case QEvent::FontChange:
        qtc_set_font_to_group_children(this, font());
        break;
    default:
        break;
    }

    if (isMinimized()) {
        if (event->type() == QEvent::Show)
            d->showGroupScroll();
        else if (event->type() == QEvent::Hide) {
            d->m_groupScrollPos = 0;
            d->m_pageScrollPos = 0;
            layout()->update();
            if (windowFlags() & Qt::Popup)
                emit hidePopup();
        }
    } else {
        switch (event->type()) {
        case QEvent::KeyPress: {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return) {
                if (QWidget *focus = focusWidget()) {
                    if (qobject_cast<QComboBox *>(focus) || qobject_cast<QLineEdit *>(focus))
                        emit released();
                }
            }
        } break;
        case QEvent::Show:
        case QEvent::Hide:
            updateGeometry();
            break;
        case QEvent::ParentChange:
            break;
        default:
            break;
        }
    }
    return QWidget::event(event);
}

/*! \reimp */
void RibbonGroup::paintEvent(QPaintEvent *event)
{
    QTC_D(RibbonGroup);

    QPainter p(this);
    if (isReduced()) {
        QStyleOptionToolButton opt;
        opt.init(this);
        opt.text = d->m_title;
        opt.icon = d->m_icon;
        opt.iconSize = opt.icon.actualSize(QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX));

        if (d->m_groupPopup && d->m_groupPopup->isVisible())
            opt.features |= QStyleOptionToolButton::HasMenu;

        QRect rcGroup = opt.rect;
        rcGroup.setBottom(rcGroup.bottom() - 1);
        opt.rect = rcGroup;
        style()->drawControl((QStyle::ControlElement)RibbonStyle::CE_ReducedGroup, &opt, &p, this);
    } else {
        QRect rectEvent = event->rect();
        p.setClipRect(rectEvent);

        QStyle *ribbonStyle = style();

        if (windowFlags() & Qt::Popup) {
            StyleOptionRibbon opt;
            opt.init(this);
            style()->drawControl((QStyle::ControlElement)RibbonStyle::CE_RibbonGroups, &opt, &p, this);
        }

        QStyleOptionGroupBox opt;
        d->initStyleOption(opt);
        if (d->m_font != Q_NULL)
            opt.fontMetrics = QFontMetrics(*d->m_font);

        if (rectEvent.intersects(opt.rect))
            ribbonStyle->drawControl((QStyle::ControlElement)RibbonStyle::CE_Group, &opt, &p, this);

        QFont saveFont = p.font();
        if (d->m_font != Q_NULL)
            p.setFont(*d->m_font);

        if (d->m_color.isValid())
            opt.palette.setColor(QPalette::WindowText, d->m_color);

        int flags = opt.textAlignment;
        flags |= Qt::TextHideMnemonic;

        QRect rcEntryCaption = opt.rect;
        rcEntryCaption.setTop(rcEntryCaption.bottom() - opt.lineWidth);

        int indent = 0;
        if (d->m_optionButton->isVisible())
            indent = d->m_optionButton->width();

        rcEntryCaption.adjust(-2, -1, -indent, -2);
        opt.text = p.fontMetrics().elidedText(opt.text, titleElideMode(), rcEntryCaption.width());
        ribbonStyle->drawItemText(&p, rcEntryCaption, flags, opt.palette, opt.state & QStyle::State_Enabled, opt.text,
                                  QPalette::WindowText);

        p.setFont(saveFont);
    }
}

/*! \reimp */
void RibbonGroup::actionEvent(QActionEvent *event)
{
    QTC_D(RibbonGroup);

    QAction *action = event->action();
    RibbonControl *control = controlByAction(action);

    switch (event->type()) {
    case QEvent::ActionAdded: {
        if (control == Q_NULL) {
            if (QWidgetAction *widgetAction = qobject_cast<QWidgetAction *>(action)) {
                QWidget *widget = widgetAction->requestWidget(this);
                if (widget != Q_NULL) {
                    control = qobject_cast<RibbonControl *>(widget);
                    if (control == Q_NULL) {
                        RibbonWidgetControl *widgetControl = new RibbonWidgetControl(this, false);
                        widgetControl->setContentWidget(widget);
                        control = widgetControl;
                        control->setDefaultAction(action);
                        widgetAction->setDefaultWidget(control);
                    }
                } else {
                    Q_ASSERT(false);
                    return;
                }
            } else if (action->isSeparator())
                control = new RibbonColumnBreakControl(this);

            if (control == Q_NULL)
                control = new RibbonButtonControl(this);

            control->setDefaultAction(action);
            addControl(control);
        }
    } break;
    case QEvent::ActionRemoved: {
        if (control != Q_NULL) {
            removeControl(control);
            if (QWidgetAction *widgetAction = qobject_cast<QWidgetAction *>(action)) {
                widgetAction->releaseWidget(control);
                if (widgetAction->defaultWidget() == Q_NULL)
                    delete control;
            } else {
                control->hide();
                delete control;
            }
        }
    } break;
    case QEvent::ActionChanged: {
        Q_ASSERT(control != Q_NULL);
        if (control == Q_NULL)
            control->actionChanged();
        d->m_dirty = true;
        QApplication::postEvent(this, new QEvent(QEvent::LayoutRequest));
        break;
    }
    default:
        Q_ASSERT_X(false, "RibbonGroup::actionEvent", "internal error");
    }
}

/*! \reimp */
void RibbonGroup::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    update();
}

/*! \reimp */
void RibbonGroup::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    update();
}

/*! \reimp */
void RibbonGroup::mousePressEvent(QMouseEvent *event)
{
    QTC_D(RibbonGroup);
    if (isReduced()) {
        Q_ASSERT(d->m_groupPopup != Q_NULL);
        if (event->button() == Qt::LeftButton) {
            QPoint pnt(0, 0);
            QRect rc = rect();

            QRect screen = QApplication::desktop()->availableGeometry(this);
            int h = rc.height();
            if (this->mapToGlobal(QPoint(0, rc.bottom())).y() + h <= screen.height())
                pnt = this->mapToGlobal(rc.bottomLeft());
            else
                pnt = this->mapToGlobal(rc.topLeft() - QPoint(0, h));

            const int desktopFrame = style()->pixelMetric(QStyle::PM_MenuDesktopFrameWidth, 0, this);
            if (pnt.x() + rc.width() - 1 > screen.right() - desktopFrame)
                pnt.setX(screen.right() - desktopFrame - rc.width() + 1);
            if (pnt.x() < screen.left() + desktopFrame)
                pnt.setX(screen.left() + desktopFrame);

            d->m_groupPopup->popup(pnt, 0);
            return;
        }
    }
    QWidget::mousePressEvent(event);
}
