/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#include "ribbongallery.h"
#include "ribbongallery_p.h"
#include "ribbongallerygroup_p.h"
#include "ribboncontrols.h"
#include "ribbonutils.h"

#include <QEvent>
#include <QIcon>
#include <QAction>
#include <QActionGroup>
#include <QBoxLayout>
#include <QDebug>
#include <QKeyEvent>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QResizeEvent>
#include <QScrollArea>
#include <QScreen>

QX_RIBBON_BEGIN_NAMESPACE

/*
 * RibbonGalleryPreview - one group, every row of it.
 *
 * The popup has to show the whole group while the gallery is still showing the
 * same group a few rows at a time, and a widget cannot be in two layouts at
 * once. So the popup gets a view of the group instead of the group: it reads
 * the items and the metrics off it and paints them with the same painter the
 * group uses, which is also why the two never disagree about where an item is.
 */
class RibbonGalleryPreview : public QWidget
{
public:
    explicit RibbonGalleryPreview(RibbonGallery *gallery, RibbonGalleryGroup *group, QWidget *parent = Q_NULLPTR);
    QSize sizeHint() const override;
    int heightForWidth(int w) const override;
    bool hasHeightForWidth() const override;
    void refresh();
    /// The grid the popup paints with. A group that has never been laid out keeps
    /// a stale one, and the popup cannot borrow the width of a group it does not
    /// show - so the gallery hands every preview the grid one strip row's worth of
    /// items is drawn with.
    void setGrid(const QSize &grid);
protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
private:
    void fill(RibbonGalleryPaintSpec &spec) const;
    QRect area() const;
    QSize grid() const;
    void syncHeight();
private:
    RibbonGallery *m_gallery;
    RibbonGalleryGroup *m_group;
    QSize m_grid;
    int m_hoverIndex;
    int m_pressedIndex;
    bool m_syncingHeight;
};

RibbonGalleryPreview::RibbonGalleryPreview(RibbonGallery *gallery, RibbonGalleryGroup *group, QWidget *parent)
    : QWidget(parent)
    , m_gallery(gallery)
    , m_group(group)
    , m_hoverIndex(-1)
    , m_pressedIndex(-1)
    , m_syncingHeight(false)
{
    setMouseTracking(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
}

QRect RibbonGalleryPreview::area() const
{
    const int sp = m_group ? m_group->spacing() : 1;
    return rect().adjusted(sp, sp, -sp, -sp);
}

QSize RibbonGalleryPreview::grid() const
{
    if (m_grid.isValid()) {
        return m_grid;
    }
    return (m_group && m_group->gridSize().isValid()) ? m_group->gridSize() : QSize(64, 64);
}

void RibbonGalleryPreview::setGrid(const QSize &grid)
{
    if (m_grid == grid) {
        return;
    }
    m_grid = grid;
    syncHeight();
    updateGeometry();
    update();
}

/*!
 * States the height this preview needs at the width it has.
 *
 * heightForWidth() is the right answer and the popup cannot ask for it: a widget
 * inside a QScrollArea is laid out by its size hint, and a scroll area does not
 * propagate heightForWidth to the widget it scrolls. So the preview works it out
 * itself whenever it is given a width - which is also what keeps the answer true
 * when the vertical scroll bar appears and takes 14 pixels off that width.
 *
 * The guard is for the round trip: setFixedHeight() resizes the widget, and the
 * resize asks this again. One pass is enough; a second one would only re-derive
 * the number that was just written.
 */
void RibbonGalleryPreview::syncHeight()
{
    if (m_syncingHeight) {
        return;
    }
    const int w = width();
    if (w <= 0) {
        return;
    }
    const int h = heightForWidth(w);
    if (h <= 0 || (minimumHeight() == h && maximumHeight() == h)) {
        return;
    }
    m_syncingHeight = true;
    setFixedHeight(h);
    m_syncingHeight = false;
}

void RibbonGalleryPreview::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    syncHeight();
}

void RibbonGalleryPreview::fill(RibbonGalleryPaintSpec &spec) const
{
    spec.actions.clear();
    if (Q_NULLPTR == m_group) {
        return;
    }
    const int count = m_group->itemCount();
    for (int i = 0; i < count; ++i) {
        spec.actions.append(m_group->itemAction(i));
    }
    spec.grid = grid();
    spec.spacing = m_group->spacing();
    spec.style = m_group->galleryGroupStyle();
    spec.palette = palette();
    spec.font = font();
    spec.hoverIndex = m_hoverIndex;
    spec.selectedIndex = m_group->selectedIndex();
    spec.pressedIndex = m_pressedIndex;
    spec.firstRow = 0;
    spec.visibleRows = 0;
    spec.columns = galleryColumnCount(area(), spec);
}

void RibbonGalleryPreview::refresh()
{
    updateGeometry();
    update();
}

bool RibbonGalleryPreview::hasHeightForWidth() const
{
    return true;
}

int RibbonGalleryPreview::heightForWidth(int w) const
{
    if (Q_NULLPTR == m_group) {
        return 0;
    }
    RibbonGalleryPaintSpec spec;
    fill(spec);
    const int sp = spec.spacing;
    const int stepX = spec.grid.width() + sp;
    const int stepY = spec.grid.height() + sp;
    const int columns = (stepX <= 0) ? 1 : qMax(1, (w - 2 * sp + sp) / stepX);
    const int rows = qMax(1, (spec.actions.count() + columns - 1) / columns);
    return rows * stepY - sp + 2 * sp;
}

QSize RibbonGalleryPreview::sizeHint() const
{
    const int w = (width() > 0) ? width() : 240;
    return QSize(w, heightForWidth(w));
}

void RibbonGalleryPreview::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    if (Q_NULLPTR == m_group) {
        return;
    }
    QPainter painter(this);
    RibbonGalleryPaintSpec spec;
    fill(spec);
    galleryPaintGrid(&painter, area(), spec);
}

void RibbonGalleryPreview::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        QWidget::mousePressEvent(event);
        return;
    }
    RibbonGalleryPaintSpec spec;
    fill(spec);
    m_pressedIndex = galleryItemAt(area(), spec, event->pos());
    update();
    event->accept();
}

void RibbonGalleryPreview::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        QWidget::mouseReleaseEvent(event);
        return;
    }
    RibbonGalleryPaintSpec spec;
    fill(spec);
    const int index = galleryItemAt(area(), spec, event->pos());
    const int pressed = m_pressedIndex;
    m_pressedIndex = -1;
    if (index >= 0 && index == pressed && m_group) {
        // Picking out of another group makes that group the one the strip shows,
        // which is what the Office gallery does: the strip follows the choice
        // rather than staying on the group it happened to start with.
        if (m_gallery && m_gallery->currentViewGroup() != m_group) {
            m_gallery->setCurrentViewGroup(m_group);
        }
        m_group->selectByIndex(index);
        QAction *act = m_group->itemAction(index);
        if (act && act->isEnabled()) {
            act->trigger();
        }
    }
    update();
    event->accept();
}

void RibbonGalleryPreview::mouseMoveEvent(QMouseEvent *event)
{
    RibbonGalleryPaintSpec spec;
    fill(spec);
    const int index = galleryItemAt(area(), spec, event->pos());
    if (index == m_hoverIndex) {
        event->accept();
        return;
    }
    m_hoverIndex = index;
    update();
    QAction *act = m_group ? m_group->itemAction(index) : Q_NULLPTR;
    if (act && act->isEnabled()) {
        act->activate(QAction::Hover);
    }
    event->accept();
}

void RibbonGalleryPreview::leaveEvent(QEvent *event)
{
    if (m_hoverIndex != -1) {
        m_hoverIndex = -1;
        update();
    }
    QWidget::leaveEvent(event);
}

/* RibbonGalleryPrivate */
class RibbonGalleryPrivate
{
    QX_DECLARE_PUBLIC(RibbonGallery)
public:
    RibbonGalleryPrivate();
public:
    void init();
    void setCurrentGroup(RibbonGalleryGroup *g);
    RibbonGalleryViewport *getPopupViewPort();
    void updateButtonIcons();
    void updateButtonState();
    void refreshPreviews();
public:
    RibbonControlButton *m_buttonUp;
    RibbonControlButton *m_buttonDown;
    RibbonControlButton *m_buttonMore;
    RibbonGalleryViewport *m_popupWidget;
    RibbonGalleryGroup *m_currentGroup;
    QList<RibbonGalleryGroup *> m_groups;
    QList<RibbonGalleryPreview *> m_previews;
    QMap<RibbonGalleryGroup *, RibbonGalleryPreview *> m_groupToPreview;
    QBoxLayout *m_btnLayout;
    QBoxLayout *m_layout;
    static int s_galleryButtonMaximumWidth;
};

int RibbonGalleryPrivate::s_galleryButtonMaximumWidth = 15;

RibbonGalleryPrivate::RibbonGalleryPrivate()
    : m_buttonUp(Q_NULLPTR)
    , m_buttonDown(Q_NULLPTR)
    , m_buttonMore(Q_NULLPTR)
    , m_popupWidget(Q_NULLPTR)
    , m_currentGroup(Q_NULLPTR)
    , m_btnLayout(Q_NULLPTR)
    , m_layout(Q_NULLPTR)
{
}

void RibbonGalleryPrivate::init()
{
    Q_Q(RibbonGallery);
    m_buttonUp = new RibbonControlButton(q);
    m_buttonDown = new RibbonControlButton(q);
    m_buttonMore = new RibbonControlButton(q);
    m_buttonUp->setToolButtonStyle(Qt::ToolButtonIconOnly);
    m_buttonDown->setToolButtonStyle(Qt::ToolButtonIconOnly);
    m_buttonMore->setToolButtonStyle(Qt::ToolButtonIconOnly);
    m_buttonUp->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    m_buttonDown->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    m_buttonMore->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    m_buttonUp->setObjectName(QStringLiteral("RibbonGalleryButtonUp"));
    m_buttonDown->setObjectName(QStringLiteral("RibbonGalleryButtonDown"));
    m_buttonMore->setObjectName(QStringLiteral("RibbonGalleryButtonMore"));
    m_buttonUp->setMaximumWidth(s_galleryButtonMaximumWidth);
    m_buttonDown->setMaximumWidth(s_galleryButtonMaximumWidth);
    m_buttonMore->setMaximumWidth(s_galleryButtonMaximumWidth);
    updateButtonIcons();
    QObject::connect(m_buttonUp, &QAbstractButton::clicked, q, &RibbonGallery::pageUp);
    QObject::connect(m_buttonDown, &QAbstractButton::clicked, q, &RibbonGallery::pageDown);
    QObject::connect(m_buttonMore, &QAbstractButton::clicked, q, &RibbonGallery::showMoreDetail);
    QObject::connect(q, &RibbonGallery::triggered, q, &RibbonGallery::onTriggered);   // signal forwarding

    m_btnLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    m_btnLayout->setSpacing(0);
    m_btnLayout->setContentsMargins(0, 0, 0, 0);
    m_btnLayout->addWidget(m_buttonUp);
    m_btnLayout->addWidget(m_buttonDown);
    m_btnLayout->addWidget(m_buttonMore);
    m_layout = new QBoxLayout(QBoxLayout::RightToLeft);
    m_layout->setSpacing(0);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->addLayout(m_btnLayout);
    m_layout->addStretch();
    q->setLayout(m_layout);
}

/*!
 * The three scroll glyphs are masks painted in a palette colour, so they follow
 * the application's theme without a style sheet naming a light or a dark file.
 */
void RibbonGalleryPrivate::updateButtonIcons()
{
    Q_Q(RibbonGallery);
    const QColor color = q->palette().color(QPalette::ButtonText);
    m_buttonUp->setIcon(tintedIcon(QStringLiteral(":/qxribbon/res/arrow_up.png"), color, m_buttonUp->iconSize()));
    m_buttonDown->setIcon(tintedIcon(QStringLiteral(":/qxribbon/res/arrow_down.png"), color, m_buttonDown->iconSize()));
    m_buttonMore->setIcon(tintedIcon(QStringLiteral(":/qxribbon/res/arrow_more.png"), color, m_buttonMore->iconSize()));
}

void RibbonGalleryPrivate::updateButtonState()
{
    const bool canScroll = m_currentGroup && (m_currentGroup->maxScrollRow() > 0);
    m_buttonUp->setEnabled(canScroll && (m_currentGroup->scrollRow() > 0));
    m_buttonDown->setEnabled(canScroll && (m_currentGroup->scrollRow() < m_currentGroup->maxScrollRow()));
}

void RibbonGalleryPrivate::refreshPreviews()
{
    Q_Q(RibbonGallery);
    // The size of an item in the popup is one row of the strip's - not the grid
    // the group the strip happens to show was compressed to. A group with three
    // display rows divides the strip's height by three, and taking that answer as
    // the popup's item size made every group in the popup shrink each time the
    // strip's group changed: the size of an item has nothing to do with how many
    // rows its group asked for, and everything to do with the strip's height.
    const int strip = q->contentsRect().height();
    for (int i = 0; i < m_previews.count(); ++i) {
        RibbonGalleryPreview *pv = m_previews.at(i);
        RibbonGalleryGroup *g = m_groups.value(i, Q_NULLPTR);
        const int sp = g ? g->spacing() : 1;
        int h = strip - 2 * sp;
        if (h <= 0) {
            h = 64;   // never laid out yet: a sane item, not a failed measurement
        }
        int w = h;
        if (g) {
            if (g->gridMinimumWidth() > 0 && w < g->gridMinimumWidth()) {
                w = g->gridMinimumWidth();
            }
            if (g->gridMaximumWidth() > 0 && w > g->gridMaximumWidth()) {
                w = g->gridMaximumWidth();
            }
        }
        pv->setGrid(QSize(w, h));
        pv->refresh();
    }
}

/*
 * Puts the group in the strip on the left of the buttons.
 *
 * The group is shown as it is - the gallery used to keep a second view fed from
 * the same model, which is why nothing could agree with anything: the strip was
 * the copy, the popup held the original, and a click had to be forwarded between
 * them. One widget, shown once, is the whole point of the rewrite.
 */
void RibbonGalleryPrivate::setCurrentGroup(RibbonGalleryGroup *g)
{
    Q_Q(RibbonGallery);
    if (m_currentGroup == g) {
        return;
    }
    if (m_currentGroup) {
        m_layout->removeWidget(m_currentGroup);
        m_currentGroup->setViewportRows(0);
        m_currentGroup->hide();
    }
    m_currentGroup = g;
    if (Q_NULLPTR == g) {
        updateButtonState();
        return;
    }
    if (g->parent() != q) {
        g->setParent(q);
    }
    m_layout->addWidget(g, 1);
    // How many rows the strip shows is what DisplayRow already says: the grid is
    // sized so that many fit the height, and the rest are scrolled through.
    g->setViewportRows(g->displayRow());
    g->show();
    updateButtonState();
}

RibbonGalleryViewport *RibbonGalleryPrivate::getPopupViewPort()
{
    Q_Q(RibbonGallery);
    if (Q_NULLPTR == m_popupWidget) {
        m_popupWidget = new RibbonGalleryViewport(q);
    }
    return m_popupWidget;
}

//////////////////////////////////////////////

RibbonGallery::RibbonGallery(QWidget *parent)
    : QFrame(parent)
{
    QX_INIT_PRIVATE(RibbonGallery)
    Q_D(RibbonGallery);
    d->init();
    setFrameShape(QFrame::Box);
    // The frame keeps the strip off its own pixels by itself: QFrame insets the
    // layout by the frame width, so a child laid inside it already starts at
    // (frameWidth, frameWidth). Adding margins of our own only wastes a pixel on
    // each side and leaves the strip looking loosely fitted.
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumWidth(200);
    // B9: the gallery can be reached and driven from the keyboard: arrows move the
    // current item, Enter/Space activates it, PageUp/PageDown scroll like the buttons.
    setFocusPolicy(Qt::StrongFocus);
}

RibbonGallery::~RibbonGallery()
{
    QX_FINI_PRIVATE()
}

void RibbonGallery::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::PaletteChange) {
        Q_D(RibbonGallery);
        d->updateButtonIcons();
    }
    QWidget::changeEvent(event);
}

/**
 * @brief Gets an empty RibbonGalleryGroup
 * @return
 */
RibbonGalleryGroup *RibbonGallery::addGalleryGroup()
{
    RibbonGalleryGroup *group = new RibbonGalleryGroup(this);
    addGalleryGroup(group);
    return group;
}

/**
 * @brief Adds a RibbonGalleryGroup
 * @param group
 */
void RibbonGallery::addGalleryGroup(RibbonGalleryGroup *group)
{
    Q_D(RibbonGallery);
    if (Q_NULLPTR == group) {
        return;
    }
    if (d->m_groups.contains(group)) {
        return;
    }
    if (group->parent() != this) {
        group->setParent(this);
    }
    d->m_groups.append(group);
    RibbonGalleryViewport *viewport = d->getPopupViewPort();
    RibbonGalleryPreview *preview = new RibbonGalleryPreview(this, group, viewport);
    viewport->addWidget(preview, group->groupTitle());
    d->m_previews.append(preview);
    d->m_groupToPreview.insert(group, preview);
    connect(group, &RibbonGalleryGroup::groupTitleChanged, this, [this, group](const QString &t) {
        Q_D(RibbonGallery);
        RibbonGalleryPreview *pv = d->m_groupToPreview.value(group, Q_NULLPTR);
        if (pv && d->m_popupWidget) {
            d->m_popupWidget->widgetTitleChanged(pv, t);
        }
    });
    connect(group, &RibbonGalleryGroup::triggered, this, &RibbonGallery::triggered);
    connect(group, &RibbonGalleryGroup::hovered, this, &RibbonGallery::hovered);
    setCurrentViewGroup(group);
}

RibbonGalleryGroup *RibbonGallery::addGalleryGroup(const QString &title, QList<QAction *> actions)
{
    RibbonGalleryGroup *group = new RibbonGalleryGroup(this);
    if (!title.isEmpty()) {
        group->setGroupTitle(title);
    }
    group->addItems(actions);
    addGalleryGroup(group);
    return group;
}

/**
 * @brief Gets the group the strip is showing
 *
 * This is the group itself and not a view of it: it is the widget in the strip
 * and the one scrolled by the buttons and the keyboard.
 */
RibbonGalleryGroup *RibbonGallery::currentViewGroup() const
{
    Q_D(const RibbonGallery);
    return d->m_currentGroup;
}

void RibbonGallery::setCurrentViewGroup(RibbonGalleryGroup *group)
{
    Q_D(RibbonGallery);
    d->setCurrentGroup(group);
    if (d->m_currentGroup) {
        d->m_currentGroup->recalcGridSize(d->m_currentGroup->height());
    }
}

QSize RibbonGallery::sizeHint() const
{
    return QSize(100, 62);
}

/**
 * @brief Sets the maximum width of the three rightmost control buttons (default 15)
 * @param w
 */
void RibbonGallery::setGalleryButtonMaximumWidth(int w)
{
    RibbonGalleryPrivate::s_galleryButtonMaximumWidth = w;
}

void RibbonGallery::pageDown()
{
    Q_D(RibbonGallery);
    if (d->m_currentGroup) {
        d->m_currentGroup->scrollByRows(1);
        d->updateButtonState();
    }
}

void RibbonGallery::pageUp()
{
    Q_D(RibbonGallery);
    if (d->m_currentGroup) {
        d->m_currentGroup->scrollByRows(-1);
        d->updateButtonState();
    }
}

/**
 * @brief Triggered by "show more"; by default pops up the internally managed RibbonGalleryViewport. Users can override
 * this function to implement a custom popup.
 */
void RibbonGallery::showMoreDetail()
{
    Q_D(RibbonGallery);
    if (Q_NULLPTR == d->m_popupWidget) {
        return;
    }
    d->refreshPreviews();

    const int width = (d->m_currentGroup && d->m_currentGroup->width() > 0) ? d->m_currentGroup->width() : 240;
    // Measure, do not guess. The previews answer with the height they need at the
    // width they are given (RibbonGalleryPreview::syncHeight), so the popup is
    // given that width and its layout is run until the answers have settled. This
    // used to be stated the other way round - a width was guessed for the previews
    // (the popup's minus a scroll bar and a fudge), a height was derived from it,
    // and the layout then gave them a different width; one column of difference is
    // an extra row of items, which is the empty band under the last group.
    //
    // resize() rather than setGeometry(): a popup that is already open must not
    // jump to (0, 0) while it is being measured, and it should not be seen
    // shrinking to a measuring rectangle either - the height it has is kept.
    // Two passes: the first hands every preview a width, and the height they
    // answer with is what the second pass lays out.
    d->m_popupWidget->resize(width, qMax(100, d->m_popupWidget->height()));
    d->m_popupWidget->layout()->activate();
    d->m_popupWidget->layout()->activate();

    QSize popupSize = d->m_popupWidget->sizeHint();
    if (popupSize.isEmpty()) {
        popupSize = QSize(width, 100);
    }

    // The popup opens under the strip, but only if it fits: it flips above when
    // the room below is not enough, and is cut down to the side that has more room
    // when neither side can hold it. It also has to stay on the screen sideways -
    // the width comes from a gallery that may be almost as wide as the screen.
    const QRect avail = screen() ? screen()->availableGeometry() : QRect();
    const QPoint stripTopLeft = mapToGlobal(QPoint(0, 0));
    const QPoint stripBottom = mapToGlobal(QPoint(0, QWidget::height() - 1));

    int popupWidth = width;
    int popupHeight = qMin(qMax(popupSize.height(), 60), avail.isValid() ? (avail.height() * 7 / 10) : 480);
    int x = stripBottom.x();
    int y = stripBottom.y();

    if (avail.isValid()) {
        popupWidth = qMin(popupWidth, avail.width());
        const int roomBelow = avail.bottom() - stripBottom.y() + 1;
        const int roomAbove = stripTopLeft.y() - avail.top();
        if (popupHeight > roomBelow) {
            if (popupHeight <= roomAbove) {
                // Its bottom edge against the strip's top edge: opening upwards.
                y = stripTopLeft.y() - popupHeight;
            } else if (roomAbove > roomBelow) {
                popupHeight = roomAbove;
                y = avail.top();
            } else {
                popupHeight = roomBelow;
            }
        }
        popupHeight = qBound(1, popupHeight, avail.height());
        x = qBound(avail.left(), x, qMax(avail.left(), avail.right() - popupWidth + 1));
    }

    d->m_popupWidget->setGeometry(x, y, popupWidth, popupHeight);
    d->m_popupWidget->show();
}

/**
 * @brief Moves the selected item by @a step, clamped to the group (B9)
 *
 * With nothing selected yet, the first Down/Right selects the first item and the
 * first Up/Left selects the last one, which is what a list is expected to do.
 */
void RibbonGallery::moveCurrentItem(int step)
{
    Q_D(RibbonGallery);
    RibbonGalleryGroup *group = d->m_currentGroup;
    if (Q_NULLPTR == group) {
        return;
    }
    const int count = group->itemCount();
    if (count == 0) {
        return;
    }
    const int current = group->selectedIndex();
    const int from = (current < 0) ? (step > 0 ? -1 : count) : current;
    group->selectByIndex(qBound(0, from + step, count - 1));
}

void RibbonGallery::keyPressEvent(QKeyEvent *event)
{
    Q_D(RibbonGallery);
    RibbonGalleryGroup *group = d->m_currentGroup;
    if (!group) {
        QFrame::keyPressEvent(event);
        return;
    }
    switch (event->key()) {
    case Qt::Key_PageUp:
        pageUp();
        return;
    case Qt::Key_PageDown:
        pageDown();
        return;
    case Qt::Key_Up:
    case Qt::Key_Left:
        moveCurrentItem(-1);
        return;
    case Qt::Key_Down:
    case Qt::Key_Right:
        moveCurrentItem(1);
        return;
    case Qt::Key_Return:
    case Qt::Key_Enter:
    case Qt::Key_Space:
        group->triggerCurrentItem();
        return;
    default:
        break;
    }
    QFrame::keyPressEvent(event);
}

void RibbonGallery::onTriggered(QAction *action)
{
    Q_D(RibbonGallery);
    Q_UNUSED(action);
    // Close the popup after a click
    if (d->m_popupWidget && d->m_popupWidget->isVisible()) {
        d->m_popupWidget->hide();
    }
}

void RibbonGallery::resizeEvent(QResizeEvent *event)
{
    QFrame::resizeEvent(event);
    Q_D(RibbonGallery);
    if (d->m_currentGroup) {
        d->m_currentGroup->setViewportRows(d->m_currentGroup->displayRow());
        d->m_currentGroup->recalcGridSize(d->m_currentGroup->height());
    }
    d->updateButtonState();
}

void RibbonGallery::paintEvent(QPaintEvent *event)
{
    QFrame::paintEvent(event);
}

/* RibbonGalleryViewportPrivate */
class RibbonGalleryViewportPrivate
{
    QX_DECLARE_PUBLIC(RibbonGalleryViewport)
public:
    RibbonGalleryViewportPrivate();
    void init();
public:
    QVBoxLayout *m_layout;
    QScrollArea *m_scroll;
    QWidget *m_content;
    QMap<QWidget *, QLabel *> m_widgetToTitleLabel;
};

RibbonGalleryViewportPrivate::RibbonGalleryViewportPrivate()
    : m_layout(Q_NULLPTR)
    , m_scroll(Q_NULLPTR)
    , m_content(Q_NULLPTR)
{
}

void RibbonGalleryViewportPrivate::init()
{
    Q_Q(RibbonGalleryViewport);
    m_scroll = new QScrollArea(q);
    m_scroll->setWidgetResizable(true);
    m_scroll->setFrameShape(QFrame::NoFrame);
    m_scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_content = new QWidget(m_scroll);
    m_layout = new QVBoxLayout(m_content);
    m_layout->setSpacing(2);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_scroll->setWidget(m_content);

    QVBoxLayout *top = new QVBoxLayout(q);
    top->setSpacing(0);
    top->setContentsMargins(0, 0, 0, 0);
    top->addWidget(m_scroll);
}

RibbonGalleryViewport::RibbonGalleryViewport(QWidget *parent)
    : QWidget(parent)
{
    QX_INIT_PRIVATE(RibbonGalleryViewport)
    Q_D(RibbonGalleryViewport);
    d->init();

    setWindowFlags(Qt::Popup);
    QPalette pl = palette();
    pl.setBrush(QPalette::Window, pl.brush(QPalette::Base));
    setPalette(pl);
}

RibbonGalleryViewport::~RibbonGalleryViewport()
{
    QX_FINI_PRIVATE()
}

void RibbonGalleryViewport::addWidget(QWidget *w)
{
    Q_D(RibbonGalleryViewport);
    w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    d->m_layout->addWidget(w);
}

void RibbonGalleryViewport::addWidget(QWidget *w, const QString &title)
{
    Q_D(RibbonGalleryViewport);
    QLabel *label = new QLabel(this);
    label->setText(title);
    label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    label->setObjectName(QLatin1String("RibbonGalleryGroupTitle"));
    d->m_layout->addWidget(label);
    w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    d->m_layout->addWidget(w);
    d->m_widgetToTitleLabel[w] = label;
}

void RibbonGalleryViewport::removeWidget(QWidget *w)
{
    Q_D(RibbonGalleryViewport);
    QLabel *label = getWidgetTitleLabel(w);
    if (label) {
        d->m_layout->removeWidget(label);
        d->m_widgetToTitleLabel.remove(w);
        label->hide();
        label->deleteLater();
    }
    d->m_layout->removeWidget(w);
}

/**
 * @brief Gets the corresponding title for a RibbonGalleryGroup; users can set QLabel properties through this function
 * @param w
 * @return Q_NULLPTRTR if no group is managed
 */
QLabel *RibbonGalleryViewport::getWidgetTitleLabel(QWidget *w)
{
    Q_D(RibbonGalleryViewport);
    return d->m_widgetToTitleLabel.value(w, Q_NULLPTR);
}

/*!
 * The popup is as tall as the groups it holds, which is the only height it can
 * be measured at: the groups wrap according to the width they are given, so
 * their answer changes with it. The gallery gives the width first and asks after.
 */
QSize RibbonGalleryViewport::sizeHint() const
{
    Q_D(const RibbonGalleryViewport);
    if (Q_NULLPTR == d->m_content) {
        return QSize(240, 100);
    }
    return d->m_content->sizeHint() + QSize(2, 2);
}

void RibbonGalleryViewport::widgetTitleChanged(QWidget *w, const QString &title)
{
    QLabel *label = getWidgetTitleLabel(w);
    if (label) {
        label->setText(title);
    }
}

QX_RIBBON_END_NAMESPACE
