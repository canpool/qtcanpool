/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#include "ribbongallery.h"
#include "ribbongallery_p.h"
#include "ribboncontrols.h"

#include <QApplication>
#include <QIcon>
#include <QActionGroup>
#include <QDebug>
#include <QLabel>
#include <QResizeEvent>
#include <QScrollBar>
#include <QVBoxLayout>

QX_RIBBON_BEGIN_NAMESPACE

/* RibbonGalleryPrivate */
class RibbonGalleryPrivate
{
    QX_DECLARE_PUBLIC(RibbonGallery)
public:
    RibbonGalleryPrivate();
public:
    void init();
    void setViewPort(RibbonGalleryGroup *v);
    RibbonGalleryViewport *getPopupViewPort();
public:
    RibbonControlButton *m_buttonUp;
    RibbonControlButton *m_buttonDown;
    RibbonControlButton *m_buttonMore;
    RibbonGalleryViewport *m_popupWidget;
    RibbonGalleryGroup *m_viewportGroup;
    QBoxLayout *m_btnLayout;
    QBoxLayout *m_layout;
    static int s_galleryButtonMaximumWidth;
};

int RibbonGalleryPrivate::s_galleryButtonMaximumWidth = 15;

RibbonGalleryPrivate::RibbonGalleryPrivate()
    : m_popupWidget(Q_NULLPTR)
    , m_viewportGroup(Q_NULLPTR)
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
    m_buttonUp->setIcon(QIcon(":/image/res/arrow_up.png"));
    m_buttonDown->setIcon(QIcon(":/image/res/arrow_down.png"));
    m_buttonMore->setIcon(QIcon(":/image/res/arrow_more.png"));
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

void RibbonGalleryPrivate::setViewPort(RibbonGalleryGroup *v)
{
    Q_Q(RibbonGallery);
    if (Q_NULLPTR == m_viewportGroup) {
        m_viewportGroup = new RibbonGalleryGroup(q);
        m_layout->addWidget(m_viewportGroup, 1);
    }
    m_viewportGroup->setRecalcGridSizeBlock(true);
    m_viewportGroup->setGalleryGroupStyle(v->galleryGroupStyle());
    m_viewportGroup->setDisplayRow(v->displayRow());
    m_viewportGroup->setSpacing(v->spacing());
    m_viewportGroup->setGridMaximumWidth(v->gridMaximumWidth());
    m_viewportGroup->setGridMinimumWidth(v->gridMinimumWidth());
    m_viewportGroup->setRecalcGridSizeBlock(false);
    m_viewportGroup->recalcGridSize(m_viewportGroup->height());
    m_viewportGroup->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_viewportGroup->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_viewportGroup->setModel(v->model());
    m_viewportGroup->show();
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
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumWidth(200);
}

RibbonGallery::~RibbonGallery(){QX_FINI_PRIVATE()}

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
    RibbonGalleryViewport *viewport = d->getPopupViewPort();
    viewport->addWidget(group, group->groupTitle());
    connect(group, &QAbstractItemView::clicked, this, &RibbonGallery::onItemClicked);
    connect(group, &RibbonGalleryGroup::groupTitleChanged, this, [group, viewport](const QString &t) {
        viewport->widgetTitleChanged(group, t);
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

RibbonGalleryGroup *RibbonGallery::currentViewGroup() const
{
    Q_D(const RibbonGallery);
    return d->m_viewportGroup;
}

void RibbonGallery::setCurrentViewGroup(RibbonGalleryGroup *group)
{
    Q_D(RibbonGallery);
    d->setViewPort(group);
    QApplication::postEvent(this, new QResizeEvent(size(), size()));
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
    if (d->m_viewportGroup) {
        QScrollBar *vscrollBar = d->m_viewportGroup->verticalScrollBar();
        int v = vscrollBar->value();
        v += vscrollBar->singleStep();
        vscrollBar->setValue(v);
    }
}

void RibbonGallery::pageUp()
{
    Q_D(RibbonGallery);
    if (d->m_viewportGroup) {
        QScrollBar *vscrollBar = d->m_viewportGroup->verticalScrollBar();
        int v = vscrollBar->value();
        v -= vscrollBar->singleStep();
        vscrollBar->setValue(v);
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
    QSize popupSize = d->m_popupWidget->sizeHint();
    QPoint start = mapToGlobal(QPoint(0, 0));

    int width = d->m_viewportGroup->width();   // viewport

    width += qApp->style()->pixelMetric(QStyle::PM_ScrollBarExtent);   // scrollbar
    d->m_popupWidget->setGeometry(start.x(), start.y(), width, popupSize.height());
    d->m_popupWidget->show();
}

void RibbonGallery::onItemClicked(const QModelIndex &index)
{
    QObject *obj = sender();
    RibbonGalleryGroup *group = qobject_cast<RibbonGalleryGroup *>(obj);
    if (group) {
        setCurrentViewGroup(group);
        RibbonGalleryGroup *curGroup = currentViewGroup();
        curGroup->scrollTo(index);
        curGroup->setCurrentIndex(index);
    }
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
    Q_D(RibbonGallery);
    QFrame::resizeEvent(event);
    // Resize all RibbonGalleryGroups in the RibbonGalleryViewport
    int h = layout()->contentsRect().height();
    if (d->m_viewportGroup) {
        h = d->m_viewportGroup->height();
        d->m_viewportGroup->recalcGridSize();
    }
    if (d->m_popupWidget) {
        QLayout *lay = d->m_popupWidget->layout();
        if (!lay) {
            return;
        }
        int c = lay->count();
        for (int i = 0; i < c; ++i) {
            QLayoutItem *item = lay->itemAt(i);
            if (!item) {
                continue;
            }
            QWidget *w = item->widget();
            if (!w) {
                continue;
            }
            RibbonGalleryGroup *g = qobject_cast<RibbonGalleryGroup *>(w);
            if (!g) {
                continue;
            }
            g->recalcGridSize(h);
        }
    }
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
    QMap<QWidget *, QLabel *> m_widgetToTitleLabel;
};

RibbonGalleryViewportPrivate::RibbonGalleryViewportPrivate()
{
}

void RibbonGalleryViewportPrivate::init()
{
    Q_Q(RibbonGalleryViewport);
    m_layout = new QVBoxLayout(q);
    m_layout->setSpacing(2);
    m_layout->setContentsMargins(0, 0, 0, 0);
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
    w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    d->m_layout->addWidget(w);
}

void RibbonGalleryViewport::addWidget(QWidget *w, const QString &title)
{
    Q_D(RibbonGalleryViewport);
    QLabel *label = new QLabel(this);
    label->setText(title);
    label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    label->setObjectName(QLatin1String("RibbonGalleryGroupTitle"));
    d->m_layout->addWidget(label);
    w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
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

void RibbonGalleryViewport::widgetTitleChanged(QWidget *w, const QString &title)
{
    QLabel *label = getWidgetTitleLabel(w);
    if (label) {
        label->setText(title);
    }
}

QX_RIBBON_END_NAMESPACE
