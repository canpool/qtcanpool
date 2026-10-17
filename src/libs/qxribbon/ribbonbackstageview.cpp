/**
 * Copyleft (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#include "ribbonbackstageview.h"
#include "ribbonbackstageview_p.h"
#include "ribbonbar.h"
#include "ribbonutils.h"

#include <QAction>
#include <QActionEvent>
#include <QEvent>
#include <QFrame>
#include <QIcon>
#include <QKeyEvent>
#include <QToolButton>
#include <QVBoxLayout>

QX_RIBBON_BEGIN_NAMESPACE

// The width of the entry column. The entries take their height from the style and
// this width, so a theme that wants a wider column restyles the view instead.
const int backstageColumnWidth = 200;

// The view covers the host of the ribbon bar it belongs to; a bar that is a window
// itself has no parent to cover and gets the view over itself.
static QWidget *backstageHost(RibbonBar *ribbonBar)
{
    if (!ribbonBar)
        return nullptr;
    if (QWidget *host = ribbonBar->parentWidget())
        return host;
    return ribbonBar->window();
}

RibbonBackstageViewPrivate::RibbonBackstageViewPrivate()
    : m_ribbonBar(nullptr)
    , m_menu(nullptr)
    , m_menuLayout(nullptr)
    , m_closeButton(nullptr)
    , m_activePage(nullptr)
    , m_closePrevented(false)
{
}

void RibbonBackstageViewPrivate::init(RibbonBar *ribbonBar)
{
    Q_Q(RibbonBackstageView);
    m_ribbonBar = ribbonBar;

    q->setObjectName(QStringLiteral("qx_RibbonBackstageView"));
    // The panel takes the keyboard when it opens so that Esc reaches it, and it
    // starts hidden: a child widget is shown along with its host, and a backstage
    // that covered the window before being asked to would not be a backstage.
    q->setFocusPolicy(Qt::StrongFocus);
    q->hide();

    // The panel is only the menu column and the active page, and relayout() sizes
    // both of them to the host directly - so the panel keeps no layout of its own.
    // A QHBoxLayout that holds just the fixed-width column lets the box engine pick
    // the column's x, which puts it in the middle of the panel instead of at the
    // left edge where a backstage column belongs.
    m_menu = new QWidget(q);
    m_menu->setObjectName(QStringLiteral("qx_RibbonBackstageMenu"));
    m_menu->setFixedWidth(backstageColumnWidth);
    m_menu->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    m_menuLayout = new QVBoxLayout(m_menu);
    m_menuLayout->setSpacing(0);
    m_menuLayout->setContentsMargins(0, 0, 0, 0);

    m_closeButton = new QToolButton(m_menu);
    m_closeButton->setObjectName(QStringLiteral("qx_RibbonBackstageCloseButton"));
    updateCloseIcon();
    m_closeButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    m_closeButton->setAutoRaise(true);
    m_closeButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_closeButton->setToolTip(q->tr("Back"));
    m_menuLayout->addWidget(m_closeButton);
    m_menuLayout->addStretch();

    QObject::connect(m_closeButton, &QToolButton::clicked, q, &RibbonBackstageView::requestClose);

    // The panel answers the application button by itself: one click opens it, the
    // next one closes it again. A host that wants something else disconnects the
    // signal from the view.
    if (ribbonBar) {
        QObject::connect(ribbonBar, &RibbonBar::applicationButtonClicked, q, [q]() {
            q->isVisible() ? q->requestClose() : q->open();
        });
    }

    // The view is a child of its host, so it follows the host's size changes.
    if (QWidget *host = q->parentWidget())
        host->installEventFilter(q);
}

void RibbonBackstageViewPrivate::relayout()
{
    Q_Q(RibbonBackstageView);
    QWidget *host = q->parentWidget();
    if (!host)
        return;
    q->setGeometry(0, 0, host->width(), host->height());
    // The column hugs the left edge; the page fills what is left of the panel.
    m_menu->setGeometry(0, 0, backstageColumnWidth, q->height());
    QWidget *activePage = m_activePage;
    if (activePage)
        activePage->setGeometry(backstageColumnWidth, 0, qMax(0, q->width() - backstageColumnWidth), q->height());
}

QAction *RibbonBackstageViewPrivate::pageAction(QWidget *widget) const
{
    if (!widget)
        return nullptr;
    for (auto it = m_pages.constBegin(); it != m_pages.constEnd(); ++it) {
        QWidget *page = it.value();
        if (page == widget)
            return it.key();
    }
    return nullptr;
}

QWidget *RibbonBackstageViewPrivate::entryWidget(QAction *action) const
{
    if (QToolButton *button = m_buttons.value(action))
        return button;
    return m_separators.value(action);
}

void RibbonBackstageViewPrivate::addEntry(QAction *action)
{
    Q_Q(RibbonBackstageView);
    if (!action || entryWidget(action) || !q->actions().contains(action))
        return;

    if (action->isSeparator()) {
        QFrame *separator = new QFrame(m_menu);
        separator->setObjectName(QStringLiteral("qx_RibbonBackstageSeparator"));
        separator->setFrameShape(QFrame::HLine);
        m_separators.insert(action, separator);
        insertEntry(action, separator);
        return;
    }

    QToolButton *button = new QToolButton(m_menu);
    button->setObjectName(QStringLiteral("qx_RibbonBackstageButton"));
    button->setDefaultAction(action);
    button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    button->setAutoRaise(true);
    button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_buttons.insert(action, button);
    insertEntry(action, button);
}

void RibbonBackstageViewPrivate::insertEntry(QAction *action, QWidget *entry)
{
    Q_Q(RibbonBackstageView);
    const QList<QAction *> actions = q->actions();
    // The entry goes right after the entry of the action that precedes it, or after
    // the close button when there is none. Actions whose entry is not built yet - a
    // removal still waiting to be delivered - are stepped over, so the column keeps
    // the order of the action list even while events are in flight.
    QWidget *after = m_closeButton;
    int index = actions.indexOf(action);
    for (int i = index - 1; i >= 0; --i) {
        if (QWidget *preceding = entryWidget(actions.at(i))) {
            after = preceding;
            break;
        }
    }
    m_menuLayout->insertWidget(m_menuLayout->indexOf(after) + 1, entry);
}

void RibbonBackstageViewPrivate::removeEntry(QAction *action)
{
    QWidget *entry = entryWidget(action);
    if (!entry)
        return;
    if (m_buttons.contains(action))
        m_buttons.remove(action);
    else
        m_separators.remove(action);
    m_menuLayout->removeWidget(entry);
    entry->deleteLater();

    if (m_pages.contains(action)) {
        QWidget *page = m_pages.take(action);
        QWidget *activePage = m_activePage;
        if (activePage == page) {
            m_activePage = nullptr;
            page->hide();
            activateFirstPage();
        }
    }
}

void RibbonBackstageViewPrivate::activateFirstPage()
{
    Q_Q(RibbonBackstageView);
    const QList<QAction *> actions = q->actions();
    for (QAction *action : actions) {
        if (m_pages.contains(action)) {
            q->setActivePage(m_pages.value(action));
            return;
        }
    }
}

void RibbonBackstageViewPrivate::updateCloseIcon()
{
    Q_Q(RibbonBackstageView);
    m_closeButton->setIcon(tintedIcon(QStringLiteral(":/qxribbon/res/arrow_left.png"),
                                      q->palette().color(QPalette::ButtonText), m_closeButton->iconSize()));
}

RibbonBackstageView::RibbonBackstageView(RibbonBar *ribbonBar)
    : QWidget(backstageHost(ribbonBar))
{
    QX_INIT_PRIVATE(RibbonBackstageView);
    Q_D(RibbonBackstageView);
    d->init(ribbonBar);
}

RibbonBackstageView::~RibbonBackstageView()
{
    QX_FINI_PRIVATE();
}

bool RibbonBackstageView::isClosePrevented() const
{
    Q_D(const RibbonBackstageView);
    return d->m_closePrevented;
}

void RibbonBackstageView::setClosePrevented(bool prevent)
{
    Q_D(RibbonBackstageView);
    if (d->m_closePrevented == prevent)
        return;
    d->m_closePrevented = prevent;
}

QAction *RibbonBackstageView::addAction(const QIcon &icon, const QString &text)
{
    QAction *action = new QAction(this);
    action->setIcon(icon);
    action->setText(text);
    QWidget::addAction(action);
    Q_D(RibbonBackstageView);
    d->addEntry(action);
    return action;
}

QAction *RibbonBackstageView::addSeparator()
{
    QAction *action = new QAction(this);
    action->setSeparator(true);
    QWidget::addAction(action);
    Q_D(RibbonBackstageView);
    d->addEntry(action);
    return action;
}

QAction *RibbonBackstageView::addPage(QWidget *widget)
{
    QAction *action = new QAction(this);
    action->setText(widget ? widget->windowTitle() : QString());
    action->setCheckable(true);
    Q_D(RibbonBackstageView);
    if (widget) {
        // The panel shows the page while it is the active one, so it takes the
        // widget over: reparented to the view and hidden until its turn.
        widget->setParent(this);
        widget->hide();
        d->m_pages.insert(action, widget);
        QObject::connect(action, &QAction::triggered, this, [this, widget]() {
            setActivePage(widget);
        });
    }
    QWidget::addAction(action);
    d->addEntry(action);
    return action;
}

void RibbonBackstageView::setActivePage(QWidget *widget)
{
    Q_D(RibbonBackstageView);
    QAction *action = d->pageAction(widget);
    if (!action)
        return;
    QWidget *activePage = d->m_activePage;
    if (activePage == widget)
        return;
    if (activePage) {
        if (QAction *preceding = d->pageAction(activePage))
            preceding->setChecked(false);
        activePage->hide();
    }
    d->m_activePage = widget;
    action->setChecked(true);
    widget->show();
    d->relayout();
}

QWidget *RibbonBackstageView::activePage() const
{
    Q_D(const RibbonBackstageView);
    return d->m_activePage;
}

QRect RibbonBackstageView::actionGeometry(QAction *action) const
{
    Q_D(const RibbonBackstageView);
    if (QToolButton *button = d->m_buttons.value(action))
        return button->geometry();
    return QRect();
}

void RibbonBackstageView::open()
{
    Q_D(RibbonBackstageView);
    d->relayout();
    show();
    raise();
    setFocus(Qt::OtherFocusReason);
}

void RibbonBackstageView::requestClose()
{
    Q_D(RibbonBackstageView);
    if (d->m_closePrevented)
        return;
    close();
}

void RibbonBackstageView::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        requestClose();
        event->accept();
        return;
    }
    QWidget::keyPressEvent(event);
}

void RibbonBackstageView::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    Q_D(RibbonBackstageView);
    d->relayout();
    Q_EMIT aboutToShow();
}

void RibbonBackstageView::hideEvent(QHideEvent *event)
{
    QWidget::hideEvent(event);
    Q_EMIT aboutToHide();
}

bool RibbonBackstageView::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == parentWidget() && event->type() == QEvent::Resize) {
        Q_D(RibbonBackstageView);
        d->relayout();
    }
    return QWidget::eventFilter(watched, event);
}

void RibbonBackstageView::actionEvent(QActionEvent *event)
{
    Q_D(RibbonBackstageView);
    switch (event->type()) {
    case QEvent::ActionAdded:
        d->addEntry(event->action());
        break;
    case QEvent::ActionRemoved:
        d->removeEntry(event->action());
        break;
    default:
        break;
    }
}

void RibbonBackstageView::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::PaletteChange) {
        Q_D(RibbonBackstageView);
        d->updateCloseIcon();
    }
    QWidget::changeEvent(event);
}

QX_RIBBON_END_NAMESPACE
