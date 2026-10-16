/**
 * Copyleft (C) 2023-2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#include "ribbongroup.h"
#include "ribbongroup_p.h"
#include "ribbongallery.h"
#include "ribbongrouplayout.h"
#include "ribboncontrols.h"
#include "ribbonbutton.h"
#include "ribbonmanager.h"

#include <QAction>
#include <QApplication>
#include <QDebug>
#include <QFontMetrics>
#include <QGridLayout>
#include <QIcon>
#include <QMenu>
#include <QPainter>
#include <QResizeEvent>
#include <QWidgetAction>

QX_RIBBON_BEGIN_NAMESPACE

#ifndef QX_RIBBON_PROP_ROW_PROPORTION
#define QX_RIBBON_PROP_ROW_PROPORTION "_qx_RibbonGroupRowProportion"
#endif

// Opt-in layout debugging aid: define QX_RIBBON_DEBUG_HELP_DRAW (as ribbonbutton.cpp
// does) to draw the rectangles the group computes in blue dashes.
#ifdef QX_RIBBON_DEBUG_HELP_DRAW
#define HELP_DRAW_RECT(p, rect)                                                                                        \
    do {                                                                                                               \
        p.save();                                                                                                      \
        QPen _pen(Qt::DashLine);                                                                                       \
        _pen.setColor(Qt::blue);                                                                                       \
        p.setPen(_pen);                                                                                                \
        p.setBrush(QBrush());                                                                                          \
        p.drawRect(rect);                                                                                              \
        p.restore();                                                                                                   \
    } while (0)
#endif

RibbonGroupOptionButton::RibbonGroupOptionButton(QWidget *parent)
    : QToolButton(parent)
{
    setAutoRaise(true);
    setCheckable(false);
    setToolButtonStyle(Qt::ToolButtonIconOnly);
    setFixedSize(16, 16);
    setIconSize(QSize(10, 10));
    setIcon(QIcon(":/image/res/group_option.png"));
}

void RibbonGroupOptionButton::connectAction(QAction *action)
{
    connect(this, &RibbonGroupOptionButton::clicked, action, &QAction::trigger);
}

int RibbonGroupPrivate::s_groupTitleHeight = 21;
bool RibbonGroupPrivate::s_titleVisible = true;

RibbonGroupPrivate::RibbonGroupPrivate()
    : m_optionActionButton(Q_NULLPTR)
    , m_groupLayoutMode(RibbonGroup::ThreeRowMode)
    , m_isCanCustomize(true)
    , m_reduced(false)
    , m_widthsValid(false)
    , m_normalWidth(0)
    , m_reducedWidth(0)
    , m_widthsHeight(-1)
    , m_widthsTitleHeight(-1)
{
}

void RibbonGroupPrivate::init()
{
    Q_Q(RibbonGroup);
    m_layout = new RibbonGroupLayout(q);
    m_layout->setSpacing(2);
    m_layout->setContentsMargins(2, 2, 2, 2);
}

RibbonButton *RibbonGroupPrivate::lastAddedButton()
{
    QWidget *w = m_layout->lastWidget();

    return qobject_cast<RibbonButton *>(w);
}

/**
 * @brief Reset the large button type
 */
void RibbonGroupPrivate::resetLargeToolButtonStyle()
{
    Q_Q(RibbonGroup);
    QList<RibbonButton *> btns = q->ribbonButtons();

    for (RibbonButton *b : std::as_const(btns)) {
        if ((Q_NULLPTR == b) || (RibbonButton::LargeButton != b->buttonType())) {
            continue;
        }
        if (RibbonGroup::ThreeRowMode == m_groupLayoutMode) {
            if (RibbonButton::Normal != b->largeButtonType()) {
                b->setLargeButtonType(RibbonButton::Normal);
            }
        } else {
            if (RibbonButton::Lite != b->largeButtonType()) {
                b->setLargeButtonType(RibbonButton::Lite);
            }
        }
    }
}

/**
 * @brief B11: the proportion an item ends up with, resolving RowProportion::Auto
 *
 * RibbonGroupLayout resolves Auto from the item's expanding directions while it lays
 * the group out; the same rule is needed here to know which button, at its natural
 * presentation, is a large one.
 */
static RibbonGroup::RowProportion effectiveRowProportion(const RibbonGroupItem *item)
{
    RibbonGroup::RowProportion rp = item->rowProportion;

    if (RibbonGroup::Auto == rp) {
        rp = (item->expandingDirections() & Qt::Vertical) ? RibbonGroup::Large : RibbonGroup::Small;
    }
    return rp;
}

/**
 * @brief B11: bring the button presentation in line with the reduced state
 *
 * In the reduced presentation a button that would own a whole column is placed in a
 * fraction of the height, so it has to be drawn as a small button for the placement
 * and the painting to agree. Items that are not buttons keep whatever they are.
 */
void RibbonGroupPrivate::applyButtonTypes()
{
    const QList<RibbonGroupItem *> &items = m_layout->ribbonGroupItems();

    for (RibbonGroupItem *item : items) {
        RibbonButton *button = qobject_cast<RibbonButton *>(item->widget());
        if (Q_NULLPTR == button) {
            continue;
        }
        if (!m_reduced && (RibbonGroup::Large == effectiveRowProportion(item))) {
            button->setButtonType(RibbonButton::LargeButton);
        } else {
            button->setButtonType(RibbonButton::SmallButton);
        }
    }
    if (!m_reduced) {
        // The large buttons that just came back need the group's large-button style
        resetLargeToolButtonStyle();
    }
}

void RibbonGroupPrivate::invalidateWidths()
{
    m_widthsValid = false;
}

/**
 * @brief B11: measure the group in both presentations
 *
 * The widths cannot be derived from one another: a button's width depends on the
 * style it is drawn in, so each presentation has to be laid out to be measured. The
 * cached pair stays valid while the group keeps its height, its title height and its
 * contents, which is what makes this a one-off cost per change rather than a cost per
 * resize step.
 */
void RibbonGroupPrivate::ensureWidths()
{
    Q_Q(RibbonGroup);
    const int h = q->height();
    const int th = q->titleHeight();

    if (m_widthsValid && (m_widthsHeight == h) && (m_widthsTitleHeight == th)) {
        return;
    }
    const bool wasReduced = m_reduced;

    // setReducedInternal() refreshes the size hint, so each width can be read back
    // straight after the presentation it belongs to has been applied
    q->setReducedInternal(false);
    m_normalWidth = q->sizeHint().width();

    q->setReducedInternal(true);
    m_reducedWidth = q->sizeHint().width();

    q->setReducedInternal(wasReduced);

    m_widthsHeight = h;
    m_widthsTitleHeight = th;
    m_widthsValid = true;
}

/**
 * @brief Get the layout items
 * @return
 */
const QList<RibbonGroupItem *> &RibbonGroupPrivate::ribbonGroupItems() const
{
    return m_layout->ribbonGroupItems();
}

/* RibbonGroup */
RibbonGroup::RibbonGroup(QWidget *parent)
    : QWidget(parent)
{
    QX_INIT_PRIVATE(RibbonGroup);
    Q_D(RibbonGroup);
    d->init();
    setGroupLayoutMode(ThreeRowMode);
    // Accessibility (B2): expose the group name (sans the '&' mnemonic) as the accessible
    // name, so assistive tools can announce the group.
    connect(this, &QWidget::windowTitleChanged, this, [this](const QString &title) {
        setAccessibleName(QString(title).remove(QLatin1Char('&')));
    });
}

RibbonGroup::RibbonGroup(const QString &name, QWidget *parent)
    : RibbonGroup(parent)
{
    setGroupName(name);
}

RibbonGroup::~RibbonGroup()
{
    QX_FINI_PRIVATE();
}

QString RibbonGroup::groupName() const
{
    return windowTitle();
}

/**
 * @brief Set the group title
 * @note Note that this triggers the windowTitleChange signal
 * @param title Title
 */
void RibbonGroup::setGroupName(const QString &title)
{
    Q_D(RibbonGroup);
    setWindowTitle(title);
    // B11: the title is part of the width the group asks for
    d->invalidateWidths();
    update();
}

QString RibbonGroup::title() const
{
    return groupName();
}

void RibbonGroup::setTitle(const QString &title)
{
    setGroupName(title);
}

/**
 * @brief Get the row property of an action
 * @param action
 * @return
 */
RibbonGroup::RowProportion RibbonGroup::getActionRowProportionProperty(QAction *action)
{
    bool isok = false;
    int r = action->property(QX_RIBBON_PROP_ROW_PROPORTION).toInt(&isok);

    if (isok) {
        return static_cast<RibbonGroup::RowProportion>(r);
    }
    return RibbonGroup::Large;
}

/**
 * @brief Store the action row property into the action; the action carries the row property itself
 * @param action
 * @param rp
 */
void RibbonGroup::setActionRowProportionProperty(QAction *action, RibbonGroup::RowProportion rp)
{
    if (action == Q_NULLPTR) {
        return;
    }
    action->setProperty(QX_RIBBON_PROP_ROW_PROPORTION, int(rp));
}

/**
 * @brief Set the action row behavior; the row property determines how the ribbon group is displayed
 * @param action The action to set; it must already have been added to the group
 * @param rp Behavior
 */
void RibbonGroup::setActionRowProportion(QAction *action, RibbonGroup::RowProportion rp)
{
    Q_D(RibbonGroup);
    if (action == Q_NULLPTR) {
        return;
    }
    setActionRowProportionProperty(action, rp);

    RibbonGroupLayout *lay = d->m_layout;
    if (lay) {
        RibbonGroupItem *it = lay->groupItem(action);
        if (it) {
            it->rowProportion = rp;
            lay->invalidate();
        }
    }
}

/**
 * @brief Add an action
 * @param action action
 * @param rp Row proportion of the specified action
 * @return The corresponding RibbonButton
 */
RibbonButton *RibbonGroup::addAction(QAction *action, RibbonGroup::RowProportion rp)
{
    Q_D(RibbonGroup);
    if (action == Q_NULLPTR) {
        return Q_NULLPTR;
    }
    setActionRowProportionProperty(action, rp);
    QWidget::addAction(action);

    return d->lastAddedButton();
}

/**
 * @brief Add a large icon action
 *
 * @param action
 * @sa To get the RibbonButton for an action, use the @ref ribbonButtonForAction function
 */
RibbonButton *RibbonGroup::addLargeAction(QAction *action)
{
    return addAction(action, RibbonGroup::Large);
}

/**
 * @brief Force the action into a 2-row action in three-column mode
 * @note In two-row mode Medium and Small are equivalent
 * Mainly applies under ThreeRowMode
 * @param action
 * @sa To get the RibbonButton for an action, use the @ref ribbonButtonForAction function
 */
RibbonButton *RibbonGroup::addMediumAction(QAction *action)
{
    return addAction(action, RibbonGroup::Medium);
}

/**
 * @brief Add a small icon action
 * @param action
 * @sa To get the RibbonButton for an action, use the @ref ribbonButtonForAction function
 */
RibbonButton *RibbonGroup::addSmallAction(QAction *action)
{
    return addAction(action, RibbonGroup::Small);
}

/**
 * @brief Add an action
 * @param act
 * @param popMode Button style
 * @param rp How the action occupies space in the group; defaults to a large icon
 */
void RibbonGroup::addAction(QAction *act, QToolButton::ToolButtonPopupMode popMode, RibbonGroup::RowProportion rp)
{
    Q_D(RibbonGroup);
    if (act == Q_NULLPTR) {
        return;
    }
    setActionRowProportionProperty(act, rp);
    QWidget::addAction(act);
    RibbonButton *btn = d->lastAddedButton();
    if (btn) {
        btn->setPopupMode(popMode);
    }
}

/**
 * @brief Create and add an action
 *
 * If no further operation is performed on this action, RibbonGroup takes ownership of it
 *
 * @note The action's parent is set to RibbonGroup; when RibbonGroup is deleted it deletes its
 * child objects, so this action is deleted as well. To keep the action, reparent it.
 *
 * @param text Action text
 * @param icon Action icon
 * @param popMode Button style
 * @param rp How the action occupies space in the group; defaults to a large icon
 * @return The added action
 */
QAction *RibbonGroup::addAction(const QString &text, const QIcon &icon, QToolButton::ToolButtonPopupMode popMode,
                                RibbonGroup::RowProportion rp)
{
    QAction *action = new QAction(icon, text, this);
    addAction(action, popMode, rp);
    return action;
}

/**
 * @brief Add a normal menu
 * @param menu
 * @param rp
 * @param popMode Menu popup mode; defaults to InstantPopup mode
 * @return
 */
RibbonButton *RibbonGroup::addMenu(QMenu *menu, RibbonGroup::RowProportion rp, QToolButton::ToolButtonPopupMode popMode)
{
    Q_D(RibbonGroup);
    if (menu == Q_NULLPTR) {
        return Q_NULLPTR;
    }
    QAction *action = menu->menuAction();

    addAction(action, rp);
    RibbonButton *btn = d->lastAddedButton();
    if (btn) {
        btn->setPopupMode(popMode);
    }
    return btn;
}

RibbonButton *RibbonGroup::addLargeMenu(QMenu *menu, QToolButton::ToolButtonPopupMode popMode)
{
    return addMenu(menu, RibbonGroup::Large, popMode);
}

RibbonButton *RibbonGroup::addSmallMenu(QMenu *menu, QToolButton::ToolButtonPopupMode popMode)
{
    return addMenu(menu, RibbonGroup::Small, popMode);
}

/**
 * @brief Add an ActionMenu
 * @param action
 * @param menu
 * @param rp
 * @return
 */
RibbonButton *RibbonGroup::addActionMenu(QAction *action, QMenu *menu, RibbonGroup::RowProportion rp)
{
    RibbonButton *btn = addAction(action, rp);
    if (Q_NULLPTR == btn) {
        return Q_NULLPTR;
    }
    btn->setMenu(menu);
    btn->setPopupMode(QToolButton::MenuButtonPopup);
    return btn;
}

/**
 * @brief Add an action menu; an action menu is a special menu that both triggers an action on
 * click and pops up a menu
 * @param action The action triggered on click; in the group the icon is that of this action
 * @param menu The menu to pop up
 * @return
 */
RibbonButton *RibbonGroup::addLargeActionMenu(QAction *action, QMenu *menu)
{
    return addActionMenu(action, menu, RibbonGroup::Large);
}

/**
 * @brief Add a widget
 *
 * @param w
 * @param rp
 * @return Returns the action (QWidgetAction)
 * @note RibbonGroup does not manage this widget's memory; when RibbonGroup is deleted, the widget
 * is not deleted if its parent is not RibbonGroup
 */
QAction *RibbonGroup::addWidget(QWidget *w, RibbonGroup::RowProportion rp)
{
    QWidgetAction *action = new QWidgetAction(this);

    // The lifecycle of w is taken over by action
    action->setDefaultWidget(w);
    w->setAttribute(Qt::WA_Hover);
    setActionRowProportionProperty(action, rp);
    QWidget::addAction(action);
    return action;
}

/**
 * @brief Add a widget that occupies all rows
 * @param w
 * @return Returns the action (QWidgetAction)
 */
QAction *RibbonGroup::addLargeWidget(QWidget *w)
{
    return addWidget(w, RibbonGroup::Large);
}

/**
 * @brief Add a widget that occupies one row of the ribbon
 * @param w
 * @return
 */
QAction *RibbonGroup::addMediumWidget(QWidget *w)
{
    return addWidget(w, RibbonGroup::Medium);
}

/**
 * @brief Add a widget that occupies one row of the ribbon
 * @param w
 * @return Returns the action (QWidgetAction)
 */
QAction *RibbonGroup::addSmallWidget(QWidget *w)
{
    return addWidget(w, RibbonGroup::Small);
}

/**
 * @brief RibbonGroup::addGallery
 * @return
 * @note RibbonGroup takes ownership of the RibbonGallery
 */
RibbonGallery *RibbonGroup::addGallery()
{
    RibbonGallery *gallery = new RibbonGallery(this);

    addWidget(gallery, RibbonGroup::Large);

    setExpanding();
    return gallery;
}

/**
 * @brief Add a separator
 * @param top Top margin; defaults to 6
 * @param bottom Bottom margin; defaults to 6
 */
QAction *RibbonGroup::addSeparator(int top, int bottom)
{
    Q_D(RibbonGroup);
    QAction *action = new QAction(this);

    action->setSeparator(true);
    setActionRowProportionProperty(action, RibbonGroup::Large);
    QWidget::addAction(action);
    QWidget *w = d->m_layout->lastWidget();
    RibbonSeparator *sep = qobject_cast<RibbonSeparator *>(w);
    if (sep) {
        sep->setTopBottomMargins(top, bottom);
    }
    return action;
}

int RibbonGroup::ribbonButtonCount() const
{
    return ribbonButtons().count();
}

RibbonButton *RibbonGroup::ribbonButtonByIndex(int index) const
{
    int cnt = ribbonButtonCount();
    if (index < 0 || index >= cnt) {
        return nullptr;
    }
    return ribbonButtons().at(index);
}

/**
 * @brief Extract from the group the button corresponding to an action; returns Q_NULLPTR if the
 * action has no corresponding button
 * @param action
 * @return Returns Q_NULLPTR if the action has no corresponding button
 */
RibbonButton *RibbonGroup::ribbonButtonForAction(QAction *action) const
{
    RibbonGroupLayout *lay = qobject_cast<RibbonGroupLayout *>(layout());

    if (lay) {
        int index = lay->indexOf(action);
        if (index == -1) {
            return Q_NULLPTR;
        }
        QLayoutItem *item = lay->itemAt(index);
        if (item) {
            return qobject_cast<RibbonButton *>(item->widget());
        }
    }
    return Q_NULLPTR;
}

QList<RibbonButton *> RibbonGroup::ribbonButtons() const
{
    const QObjectList &objs = children();
    QList<RibbonButton *> res;

    for (QObject *o : objs) {
        RibbonButton *b = qobject_cast<RibbonButton *>(o);
        if (b) {
            res.append(b);
        }
    }
    return res;
}

bool RibbonGroup::hasOptionAction() const
{
    Q_D(const RibbonGroup);
    return (d->m_optionActionButton != Q_NULLPTR);
}

/**
 * @brief Add the option action; to remove it, pass a Q_NULLPTR pointer. RibbonGroup does not take
 * ownership of the QAction
 * @param action
 * @note To remove the OptionAction, simply pass Q_NULLPTR
 * @note RibbonGroup is not connected to the QAction's destruction; if the action is deleted
 * externally, Q_NULLPTR must first be passed to addOptionAction
 */
void RibbonGroup::addOptionAction(QAction *action)
{
    Q_D(RibbonGroup);
    if (Q_NULLPTR == action) {
        if (d->m_optionActionButton) {
            delete d->m_optionActionButton;
            d->m_optionActionButton = Q_NULLPTR;
            d->invalidateWidths();
        }
        return;
    }
    if (Q_NULLPTR == d->m_optionActionButton) {
        d->m_optionActionButton = new RibbonGroupOptionButton(this);
    }
    d->m_optionActionButton->setFixedSize(optionActionButtonSize());
    d->m_optionActionButton->setIconSize(optionActionButtonSize() - QSize(-2, -2));
    d->m_optionActionButton->connectAction(action);
    // B11: the option button is part of the width the group asks for
    d->invalidateWidths();
    updateGeometry();   // Notify the layout to relayout
    repaint();
}

/**
 * @brief The layout index corresponding to an action; this is generally used for moving and has
 * little other use
 * @param act
 * @return Returns -1 if not found
 */
int RibbonGroup::actionIndex(QAction *act) const
{
    Q_D(const RibbonGroup);
    return d->m_layout->indexOf(act);
}

/**
 * @brief Move an action
 * @param from Position of the action to move, obtained via @ref actionIndex
 * @param to The destination position
 */
void RibbonGroup::moveAction(int from, int to)
{
    Q_D(RibbonGroup);
    d->m_layout->move(from, to);
    updateGeometry();   // Notify the layout to relayout
}

RibbonGroup::GroupLayoutMode RibbonGroup::groupLayoutMode() const
{
    Q_D(const RibbonGroup);
    return d->m_groupLayoutMode;
}

void RibbonGroup::setGroupLayoutMode(RibbonGroup::GroupLayoutMode mode)
{
    Q_D(RibbonGroup);
    if (d->m_groupLayoutMode == mode) {
        return;
    }
    d->m_groupLayoutMode = mode;
    updateItemGeometry();
}

void RibbonGroup::updateItemGeometry()
{
    Q_D(RibbonGroup);
    // reset layout
    layout()->setSpacing(isTwoRow() ? 4 : 2);
    updateGeometry();

    d->resetLargeToolButtonStyle();
    // B11: the row mode and the spacing both feed into the widths
    d->invalidateWidths();
}

bool RibbonGroup::isTwoRow() const
{
    Q_D(const RibbonGroup);
    return (TwoRowMode == d->m_groupLayoutMode);
}

QSize RibbonGroup::optionActionButtonSize() const
{
    return (isTwoRow() ? QSize(12, 12) : QSize(16, 16));
}

QSize RibbonGroup::sizeHint() const
{
    Q_D(const RibbonGroup);
    QSize laySize = layout()->sizeHint();
    int maxWidth = laySize.width() + 2;

    if (titleVisible()) {
        QFontMetrics fm = fontMetrics();
        QSize titleSize = fm.size(Qt::TextShowMnemonic, groupName());
        if (d->m_optionActionButton) {
            // Space must be reserved for the optionActionButton width
            titleSize.setWidth(titleSize.width() + d->m_optionActionButton->width() + 4);
        }
        maxWidth = qMax(maxWidth, titleSize.width());
    }
    return QSize(maxWidth, laySize.height());
}

QSize RibbonGroup::minimumSizeHint() const
{
    return layout()->minimumSize();
}

/**
 * @brief Determine whether this group is in (horizontal) expanding mode
 * @return Returns true if in expanding mode
 */
bool RibbonGroup::isExpanding() const
{
    QSizePolicy sp = sizePolicy();

    return (sp.horizontalPolicy() == QSizePolicy::Expanding);
}

/**
 * @brief Set the group to expanding mode, which stretches the horizontal area
 * @param expanding
 */
void RibbonGroup::setExpanding(bool expanding)
{
    setSizePolicy(expanding ? QSizePolicy::Expanding : QSizePolicy::Preferred, QSizePolicy::Fixed);
}

/**
 * @brief B11: whether the group is laid out in its reduced presentation
 * @return
 */
bool RibbonGroup::isReduced() const
{
    Q_D(const RibbonGroup);
    return d->m_reduced;
}

/**
 * @brief B11: lay the group out in its reduced presentation
 *
 * A reduced group packs a button that would own a whole column as a small one
 * instead. It gives up some of the group's compactness for a narrower width, which is
 * how a page keeps its groups inside a window that has become too narrow for them:
 * RibbonPage reduces the groups before it falls back to its scroll buttons. An
 * application can also use this directly, for instance to collapse the groups of a
 * page it knows to be short of room.
 *
 * @param reduced
 */
void RibbonGroup::setReduced(bool reduced)
{
    Q_D(RibbonGroup);
    if (d->m_reduced == reduced) {
        return;
    }
    setReducedInternal(reduced);
    // The group is a different width now, so the page has to distribute again
    if (parentWidget()) {
        QApplication::postEvent(parentWidget(), new QEvent(QEvent::LayoutRequest));
    }
}

/**
 * @brief B11: change the presentation without notifying the page
 *
 * The page's own layout drives the state through here, as does the width measurement;
 * neither may post a LayoutRequest, or the page would be asked to lay out again while
 * it is laying out.
 *
 * The width the group asks for is refreshed before returning: it depends on the
 * presentation, and the caller reads it back as soon as this returns.
 */
void RibbonGroup::setReducedInternal(bool reduced)
{
    Q_D(RibbonGroup);
    if (d->m_reduced != reduced) {
        d->m_reduced = reduced;
        d->m_layout->setReduced(reduced);
        d->applyButtonTypes();
        update();
    }
    d->m_layout->recalcSizeHint();
}

/**
 * @brief B11: the width the group needs at its natural presentation
 * @return
 */
int RibbonGroup::normalWidth()
{
    Q_D(RibbonGroup);
    d->ensureWidths();
    return d->m_normalWidth;
}

/**
 * @brief B11: the width the group needs at its reduced presentation
 * @return
 */
int RibbonGroup::reducedWidth()
{
    Q_D(RibbonGroup);
    d->ensureWidths();
    return d->m_reducedWidth;
}

bool RibbonGroup::isCanCustomize() const
{
    Q_D(const RibbonGroup);
    return d->m_isCanCustomize;
}

void RibbonGroup::setCanCustomize(bool b)
{
    Q_D(RibbonGroup);
    d->m_isCanCustomize = b;
}

/**
 * @brief Get the height of the large icon
 * @return
 */
int RibbonGroup::largeHeight() const
{
    return RibbonGroupLayout::calcLargeHeight(rect(), this);
}

int RibbonGroup::titleHeight() const
{
    return (titleVisible() ? groupTitleHeight() : 0);
}

bool RibbonGroup::titleVisible()
{
    return RibbonGroupPrivate::s_titleVisible;
}

void RibbonGroup::setTitleVisible(bool visible)
{
    if (RibbonGroupPrivate::s_titleVisible != visible) {
        RibbonGroupPrivate::s_titleVisible = visible;
        RibbonElementStyleOpt.recalc();
    }
}

/**
 * @brief Define the title bar height of all groups; unlike @sa titleHeight this static function
 * gets the global height, whereas @sa titleHeight returns the title bar height depending on
 * whether the title bar is currently shown (0 when hidden)
 *
 * @return
 */
int RibbonGroup::groupTitleHeight()
{
    return RibbonGroupPrivate::s_groupTitleHeight;
}

/**
 * @brief Set the global group height; this function has a global effect
 * @note RibbonStyleOption uses groupTitleHeight; after calling the setter the RibbonStyleOption
 * contents must be recomputed manually,
 * @sa RibbonStyleOption::recalc
 * @param h
 */
void RibbonGroup::setGroupTitleHeight(int h)
{
    if (RibbonGroupPrivate::s_groupTitleHeight != h) {
        RibbonGroupPrivate::s_groupTitleHeight = h;
        RibbonElementStyleOpt.recalc();
    }
}

void RibbonGroup::paintEvent(QPaintEvent *event)
{
    Q_D(RibbonGroup);
    QPainter p(this);

#ifdef QX_RIBBON_DEBUG_HELP_DRAW
    HELP_DRAW_RECT(p, rect());
#endif
    if (titleVisible()) {
        const int th = titleHeight();
        QFont f = font();
        p.setFont(f);
        QRect r;
        if (d->m_optionActionButton) {
            r = QRect(1, height() - th, width() - d->m_optionActionButton->width() - 4, th);
        } else {
            r = QRect(1, height() - th, width(), th);
        }
        p.drawText(r, Qt::AlignCenter, groupName());
#ifdef QX_RIBBON_DEBUG_HELP_DRAW
        HELP_DRAW_RECT(p, r);
#endif
    }

    QWidget::paintEvent(event);
}

void RibbonGroup::resizeEvent(QResizeEvent *event)
{
    Q_D(RibbonGroup);
    //! 1. Move the option button to the corner
    if (d->m_optionActionButton) {
        if (titleVisible()) {
            d->m_optionActionButton->move(width() - d->m_optionActionButton->width() - 2,
                                          height() - titleHeight() +
                                              (titleHeight() - d->m_optionActionButton->height()) / 2);
        } else {
            d->m_optionActionButton->move(width() - d->m_optionActionButton->width(),
                                          height() - d->m_optionActionButton->height());
        }
    }
    //! 2. After resize, reset the separator height
    //! Since the separator is in the layout, it is enough for it to be tall enough; no reset needed
    return QWidget::resizeEvent(event);
}

/**
 * @brief Handle action events
 *
 * This handles the ActionAdded, ActionChanged and ActionRemoved events
 *
 * On ActionAdded the layout is asked to add the action, which in turn triggers the createItem
 * function; that function creates the widget, e.g. QRibbonButton
 *
 * On ActionChanged the layout recomputes its sizes and asks the page to relayout; everything on
 * the page may need to be resized
 *
 * On ActionRemoved the QLayoutItem managed by the layout is removed and memory is cleaned up; the
 * widget is also hidden and destroyed at this point
 *
 * @param event
 * @note All action events ask the page to relayout
 *
 */
void RibbonGroup::actionEvent(QActionEvent *event)
{
    Q_D(RibbonGroup);
    QAction *action = event->action();

    switch (event->type()) {
    case QEvent::ActionAdded: {
        d->invalidateWidths();
        RibbonGroupLayout *lay = d->m_layout;
        int index = layout()->count();
        if (event->before()) {
            // This is an insertion
            index = lay->indexOf(action);
            if (-1 == index) {
                index = layout()->count();   // Append to the end if not found
            }
        }
        lay->insertAction(index, action, getActionRowProportionProperty(action));
        // The group size changed, so the page must adjust as well
        if (parentWidget()) {
            QApplication::postEvent(parentWidget(), new QEvent(QEvent::LayoutRequest));
        }
    } break;

    case QEvent::ActionChanged: {
        d->invalidateWidths();
        // Make the layout repaint
        layout()->invalidate();
        // The group size changed, so the page must adjust as well
        if (parentWidget()) {
            QApplication::postEvent(parentWidget(), new QEvent(QEvent::LayoutRequest));
        }
    } break;

    case QEvent::ActionRemoved: {
        d->invalidateWidths();
        RibbonGroupLayout *lay = d->m_layout;
        action->disconnect(this);
        int index = lay->indexOf(action);
        if (index != -1) {
            QLayoutItem *item = lay->takeAt(index);
            delete item;
        }
        // The group size changed, so the page must adjust as well
        if (parentWidget()) {
            QApplication::postEvent(parentWidget(), new QEvent(QEvent::LayoutRequest));
        }
    } break;

    default:
        break;
    }
}

/* RibbonSeparator */
RibbonSeparator::RibbonSeparator(int height, QWidget *parent)
    : RibbonSeparator(parent)
{
    setFixedHeight(height);
}

RibbonSeparator::RibbonSeparator(QWidget *parent)
    : QWidget(parent)
    , m_topMargin(4)
    , m_bottomMargin(4)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    setFixedWidth(6);
}

QSize RibbonSeparator::sizeHint() const
{
    return QSize(6, height());
}

void RibbonSeparator::setTopBottomMargins(int top, int bottom)
{
    m_topMargin = top;
    m_bottomMargin = bottom;
}

void RibbonSeparator::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);

    painter.setPen(palette().window().color().darker(114));
    int x1 = rect().center().x();

    painter.drawLine(QPoint(x1, rect().top() + m_topMargin), QPoint(x1, rect().bottom() - m_bottomMargin));
}

QX_RIBBON_END_NAMESPACE
