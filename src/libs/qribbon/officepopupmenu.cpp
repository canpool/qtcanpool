/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 **/
#include "officepopupmenu.h"
#include "ribbonbar_p.h"
#include "ribbongallery.h"
#include "ribbonstyle.h"
#include <QApplication>
#include <QLayout>
#include <QPainter>
#include <QStyleOption>
#include <QWidgetAction>
#include <qevent.h>

QRIBBON_USE_NAMESPACE

/*!
\property OfficePopupMenu::gripVisible
*/

QRIBBON_BEGIN_NAMESPACE

class OfficePopupMenuPrivate : public QObject
{
public:
    QRN_DECLARE_PUBLIC(OfficePopupMenu)
public:
    explicit OfficePopupMenuPrivate();
public:
    void init();
    int calcMinimumHeight(bool without = false);
    QWidget *findWidget(const char *nameWidget) const;
    void udateSizeGallery(const QSize &sz);
public:
    QWidget *m_widgetPopup;
    bool m_resizable : 1;
    bool m_pressSizeGrip : 1;
    bool m_showGrip : 1;
    int m_lastWidth;
};

QRIBBON_END_NAMESPACE

OfficePopupMenuPrivate::OfficePopupMenuPrivate()
{
    m_widgetPopup = Q_NULL;
    m_resizable = false;
    m_pressSizeGrip = false;
    m_showGrip = true;
    m_lastWidth = -1;
}

void OfficePopupMenuPrivate::init()
{
    Q_Q(OfficePopupMenu);
    q->setProperty(_qrn_PopupBar, true);
    q->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    q->setMouseTracking(true);
}

int OfficePopupMenuPrivate::calcMinimumHeight(bool without)
{
    Q_Q(OfficePopupMenu);

    int height = 11;

    QList<QAction *> listActions = q->actions();
    for (int i = 0; i < listActions.count(); i++) {
        if (QWidgetAction *widgetAction = qobject_cast<QWidgetAction *>(listActions.at(i))) {
            if (QWidget *w = widgetAction->defaultWidget()) {
                if (!without) {
                    int minHeight = w->minimumSizeHint().height();
                    if (minHeight == -1)
                        minHeight = w->sizeHint().height();
                    height += minHeight;
                }
            }
        } else
            height += q->actionGeometry(listActions.at(i)).height();
    }
    return height;
}

QWidget *OfficePopupMenuPrivate::findWidget(const char *nameWidget) const
{
    Q_Q(const OfficePopupMenu);
    const QObjectList &listChildren = q->children();
    for (int i = 0; i < listChildren.size(); ++i) {
        QWidget *w = qobject_cast<QWidget *>(listChildren.at(i));

        if (w && w->property(nameWidget).toBool())
            return w;
    }
    return Q_NULL;
}

void OfficePopupMenuPrivate::udateSizeGallery(const QSize &sz)
{
    Q_Q(const OfficePopupMenu);
    if (QWidget *w = findWidget(_qrn_WidgetGallery)) {
        if (!w->isWindow() && !w->isHidden()) {
            QSize szOld = q->sizeHint();
            QSize sizeWidget = w->geometry().size();
            int offset = szOld.height() - sz.height();
            sizeWidget.setHeight(sizeWidget.height() - offset);
            RibbonGallery *gallery = (RibbonGallery *)w;
            gallery->updatelayout();
            QRect rect(QPoint(0, 0), sizeWidget);
            w->setGeometry(rect.adjusted(1, 1, -1, 1));
        }
    }
}

/*!
class OfficePopupMenu
\brief Constructs the OfficePopupMenu object with the given \a parent.
*/
OfficePopupMenu::OfficePopupMenu(QWidget *parent) : QMenu(parent)
{
    QRN_INIT_PRIVATE(OfficePopupMenu);
    Q_D(OfficePopupMenu);
    d->init();

    connect(this, SIGNAL(aboutToShow()), this, SLOT(aboutToShowBar()));
}

/*!
\brief Destructor of the OfficePopupMenu object.
*/
OfficePopupMenu::~OfficePopupMenu() { QRN_FINI_PRIVATE(); }

/*!
\brief Creates an instance of the OfficePopupMenu object with given \a parent.
*/
OfficePopupMenu *OfficePopupMenu::createPopupMenu(QWidget *parent)
{
    OfficePopupMenu *popupBar = new OfficePopupMenu(parent);
    return popupBar;
}

/*!
\brief Adds \a widget to the Office Popup Window and return an action associated with them.
*/
QAction *OfficePopupMenu::addWidget(QWidget *widget)
{
    Q_D(OfficePopupMenu);

    QWidgetAction *action = new QWidgetAction(this);
    action->setDefaultWidget(widget);
    addAction(action);

    if (widget && widget->property("isResizable").toBool())
        d->m_resizable = true;

    if (RibbonGallery *gallery = qobject_cast<RibbonGallery *>(d->findWidget(_qrn_WidgetGallery))) {
        gallery->adjustSize();
        gallery->updatelayout();
    }
    // sizeHint();
    return action;
}

/*!
\brief Sets visibility of the Popup Window Grip to \a visible.
*/
void OfficePopupMenu::setGripVisible(bool visible)
{
    Q_D(OfficePopupMenu);
    d->m_showGrip = visible;
}

/*!
\brief Returns the visibility of the Popup Window Grip.
*/
bool OfficePopupMenu::isGripVisible() const
{
    Q_D(const OfficePopupMenu);
    return d->m_showGrip;
}

/*! \internal */
void OfficePopupMenu::setWidgetBar(QWidget *widget)
{
    Q_D(OfficePopupMenu);
    Q_ASSERT(widget != Q_NULL && d->m_widgetPopup == Q_NULL);
    d->m_widgetPopup = widget;
}

/*! \internal */
void OfficePopupMenu::aboutToShowBar()
{
    Q_D(OfficePopupMenu);

    if (RibbonGallery *gallery = qobject_cast<RibbonGallery *>(d->findWidget(_qrn_WidgetGallery))) {
        if (!gallery->isWindow() && !gallery->isHidden())
            gallery->updatelayout();
    }
}

/*! \reimp */
QSize OfficePopupMenu::sizeHint() const
{
    QSize size = QMenu::sizeHint();

    Q_D(const OfficePopupMenu);

    int height = 0;
    if (d->m_resizable)
        height = 11;

    QSize sz;
    if (d->m_widgetPopup)
        sz = d->m_widgetPopup->geometry().size();
    else if (QWidget *w = d->findWidget(_qrn_WidgetGallery)) {
        sz = w->geometry().size();
        if (sz.isNull()) {
            sz = size;
            sz = QSize(qMin(size.width(), sz.width()), size.height());
        }
    }
    size.setWidth(d->m_lastWidth != -1 ? d->m_lastWidth : sz.width() + 1);
    size.setHeight(size.height() + height);
    return size;
}

/*! \reimp */
void OfficePopupMenu::paintEvent(QPaintEvent *event)
{
    QMenu::paintEvent(event);
    Q_D(OfficePopupMenu);

    if (d->m_resizable) {
        QPainter p(this);
        QStyleOptionSizeGrip opt;
        opt.init(this);
        opt.rect.adjust(1, 0, -1, -1);
        opt.rect.setTop(opt.rect.bottom() - 11);
        style()->drawControl((QStyle::ControlElement)RibbonStyle::CE_PopupSizeGrip, &opt, &p, this);
    }
}

/*! \reimp */
void OfficePopupMenu::mousePressEvent(QMouseEvent *event)
{
    Q_D(OfficePopupMenu);
    if (d->m_resizable) {
        if (event->buttons() == Qt::LeftButton) {
            QRect rcResizeGripper(rect());
            rcResizeGripper.setTop(rcResizeGripper.bottom() - 11);
            QRect rcResizeGripperAll = rcResizeGripper;
            rcResizeGripper.setLeft(rcResizeGripper.right() - rcResizeGripper.height());
            if (rcResizeGripper.contains(event->pos())) {
                if (QWidget *w = d->findWidget(_qrn_WidgetGallery)) {
                    if (w && !w->isWindow() && !w->isHidden())
                        w->setAttribute(Qt::WA_TransparentForMouseEvents, true);
                }
                d->m_pressSizeGrip = true;
                return;
            } else if (rcResizeGripperAll.contains(event->pos()))
                return;
        }
    }
    QMenu::mousePressEvent(event);
}

/*! \reimp */
void OfficePopupMenu::mouseMoveEvent(QMouseEvent *event)
{
    Q_D(OfficePopupMenu);

    if (QWidget *w = d->findWidget(_qrn_WidgetGallery)) {
        QMouseEvent evPress(event->type(), event->pos(), event->globalPos(), event->button(), event->buttons(),
                            event->modifiers());
        QApplication::sendEvent(w, &evPress);
    }

    if (d->m_resizable) {
        QRect rcResizeGripper(rect());
        rcResizeGripper.setTop(rcResizeGripper.bottom() - 11);
        rcResizeGripper.setLeft(rcResizeGripper.right() - rcResizeGripper.height());

        if (d->m_pressSizeGrip || rcResizeGripper.contains(event->pos()))
            setCursor(Qt::SizeFDiagCursor);
        else
            unsetCursor();

        if (d->m_pressSizeGrip) {
            QPoint np(event->globalPos());

            QRect rect = geometry();
            rect.setRight(np.x());
            rect.setBottom(np.y());

            d->m_lastWidth = rect.width();
            setGeometry(rect);
            return;
        }
    }

    if (!d->m_pressSizeGrip)
        QMenu::mouseMoveEvent(event);
}

/*! \reimp */
void OfficePopupMenu::mouseReleaseEvent(QMouseEvent *event)
{
    Q_D(OfficePopupMenu);
    d->m_pressSizeGrip = false;

    if (QWidget *w = d->findWidget(_qrn_WidgetGallery)) {
        if (w && !w->isWindow() && !w->isHidden())
            w->setAttribute(Qt::WA_TransparentForMouseEvents, false);
    }
    QMenu::mouseReleaseEvent(event);
}

/*! \reimp */
void OfficePopupMenu::moveEvent(QMoveEvent *event)
{
    Q_UNUSED(event);
    Q_D(OfficePopupMenu);
    if (d->m_pressSizeGrip)
        setCursor(Qt::SizeFDiagCursor);
}

/*! \reimp */
void OfficePopupMenu::showEvent(QShowEvent *event)
{
    Q_D(OfficePopupMenu);
    QMenu::showEvent(event);

    if (d->m_widgetPopup && d->m_resizable) {
        setMinimumWidth(d->m_widgetPopup->geometry().width());
        setMinimumHeight(d->calcMinimumHeight());
    }
}

/*! \reimp */
void OfficePopupMenu::resizeEvent(QResizeEvent *event)
{
    Q_D(OfficePopupMenu);
    d->udateSizeGallery(event->size());
    QMenu::resizeEvent(event);
}
