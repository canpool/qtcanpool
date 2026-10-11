/**
 * Copyleft (C) 2025 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/

#include "ribbonmainwindow.h"
#include "ribbonbar.h"

QX_RIBBON_BEGIN_NAMESPACE

class RibbonMainWindowPrivate
{
    QX_DECLARE_PUBLIC(RibbonMainWindow)
public:
    RibbonMainWindowPrivate();
    void setMenuWidget(QWidget *menuBar);
    void updateMinimumWidth();
public:
    RibbonBar *m_ribbonBar;
    int m_theme;
};

RibbonMainWindowPrivate::RibbonMainWindowPrivate()
    : m_ribbonBar(Q_NULLPTR)
    , m_theme(RibbonTheme::LightOffice2013Theme)
{
}

void RibbonMainWindowPrivate::setMenuWidget(QWidget *menuBar)
{
    Q_Q(RibbonMainWindow);
    RibbonBar *bar = qobject_cast<RibbonBar *>(menuBar);

    if (m_ribbonBar != Q_NULLPTR) {
        m_ribbonBar->removeEventFilter(q);
        delete m_ribbonBar;
    }

    if (bar) {
        m_ribbonBar = bar;
        m_ribbonBar->installEventFilter(q);
    } else {
        m_ribbonBar = Q_NULLPTR;
    }
}

/**
 * Keeps the window at least as wide as the ribbon bar needs it to be.
 *
 * The ribbon bar advertises that width through sizeHint(), and it is not a constant:
 * it grows with the content (a new quick access entry, a hidden ribbon, a style
 * change) and it is published as the minimum width of the window, which Windows
 * turns into the minimum tracking size of the native window. Publishing the value
 * alone is not enough, because the window is not resized by it when the content
 * changes: the window is then left narrower than its own minimum, and from that
 * state it cannot be resized normally any more. The system clamps the first
 * interactive resize to the tracking size, so the window jumps instead of following
 * the mouse, and the frameless helper of the window class that used to sit on top
 * of this one - removed in 4.3, see the migration page - pinned it in place and
 * stopped resizing altogether.
 *
 * So both sides are done here: the new minimum is published, and the window is grown
 * to satisfy it. The second step terminates because it leaves width() == need, which
 * makes the test false for the resize it triggers in turn.
 */
void RibbonMainWindowPrivate::updateMinimumWidth()
{
    Q_Q(RibbonMainWindow);

    if (!m_ribbonBar) {
        return;
    }

    const int need = m_ribbonBar->sizeHint().width();

    if (need != q->minimumWidth()) {
        q->setMinimumWidth(need);
    }

    // A maximized window is the size of the screen and restoring it is the system's
    // business; growing it here would fight the window manager.
    if (need > q->width() && !q->isMaximized() && !q->isFullScreen()) {
        q->resize(need, q->height());
    }
}

RibbonMainWindow::RibbonMainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QX_INIT_PRIVATE(RibbonMainWindow)
    setRibbonTheme(ribbonTheme());
    setMenuWidget(new RibbonBar(this));
    resizeRibbon();
}

RibbonMainWindow::~RibbonMainWindow()
{
    QX_FINI_PRIVATE();
}

RibbonBar *RibbonMainWindow::ribbonBar() const
{
    Q_D(const RibbonMainWindow);
    return d->m_ribbonBar;
}

void RibbonMainWindow::resizeRibbon()
{
    Q_D(RibbonMainWindow);
    if (d->m_ribbonBar) {
        d->m_ribbonBar->resizeRibbon();
    }
}

void RibbonMainWindow::setRibbonTheme(int theme)
{
    Q_D(RibbonMainWindow);
    RibbonTheme::setTheme(theme, this);
    d->m_theme = theme;

    if (d->m_ribbonBar) {
        d->m_ribbonBar->updateRibbonTheme();
        d->m_ribbonBar->resizeRibbon();
    }
}

int RibbonMainWindow::ribbonTheme() const
{
    Q_D(const RibbonMainWindow);
    return d->m_theme;
}

void RibbonMainWindow::updateWindowFlags(Qt::WindowFlags flags)
{
    setWindowFlags(flags);
    // Note: This function setWindowFlags calls setParent() when changing the flags for a window,
    // causing the widget to be hidden. You must call show() to make the widget visible again..
    show();
}

void RibbonMainWindow::setMenuWidget(QWidget *menuBar)
{
    Q_D(RibbonMainWindow);
    QMainWindow::setMenuWidget(menuBar);
    d->setMenuWidget(menuBar);
}

void RibbonMainWindow::setMenuBar(QMenuBar *menuBar)
{
    setMenuWidget(menuBar);
}

void RibbonMainWindow::loadTheme(const QString &themeFile)
{
    RibbonTheme::loadTheme(themeFile, this);
}

bool RibbonMainWindow::event(QEvent *e)
{
    Q_D(RibbonMainWindow);
    switch (e->type()) {
    case QEvent::Resize: {
        if (d->m_ribbonBar) {
            // When the main window uses the system native frame, when the main window
            // is restored from the maximum for the first time, the restored width is
            // smaller than the minimum width of the RibbonBar, but the Resize event of
            // the RibbonBar will not be sent. Therefore, the minimum width also needs
            // to be set here.
            d->updateMinimumWidth();
            if (d->m_ribbonBar->size().width() != this->size().width()) {
                d->m_ribbonBar->setFixedWidth(this->size().width());
            }
        }
    } break;
    default:
        break;
    }
    return QMainWindow::event(e);
}

bool RibbonMainWindow::eventFilter(QObject *obj, QEvent *event)
{
    Q_D(RibbonMainWindow);
    if (!isMaximized() && d->m_ribbonBar && d->m_ribbonBar == obj) {
        // LayoutRequest is how the RibbonBar announces that its content changed, and a
        // changed content moves the minimum width without the window being resized at
        // all - which is exactly the case that used to leave the two out of step. Both
        // events have to go through the same path for that reason.
        if (event->type() == QEvent::Resize || event->type() == QEvent::LayoutRequest) {
            d->updateMinimumWidth();
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

QX_RIBBON_END_NAMESPACE
