#include "goodmainwindow.h"

#include <QAction>
#include <QEvent>
#include <QDebug>

#include "qcanpool/windowtoolbar.h"

QCANPOOL_USE_NAMESPACE

GoodMainWindow::GoodMainWindow(QWidget *parent)
    : QGoodWindow(parent)
{
    addCaptionClassName("QCanpool::WindowToolBar");

    QIcon icon(":/logo");

    WindowToolBar *wtb = new WindowToolBar(this);
    wtb->setSignalEnabled(true);
    wtb->setWindowStates(this->windowState());

    connect(wtb, &WindowToolBar::buttonMinimizeClicked, this, &GoodMainWindow::showMinimized);
    connect(wtb, &WindowToolBar::buttonMaximzieClicked, this, [this]() {
        if (this->isMaximized()) {
            this->showNormal();
        } else {
            this->showMaximized();
        }
    });
    connect(wtb, &WindowToolBar::buttonCloseClicked, this, &GoodMainWindow::close);
    connect(this, &GoodMainWindow::windowStateChanged, wtb, [wtb](Qt::WindowStates state) {
        wtb->setWindowStates(state);
    });

    QPalette palette;
    palette.setColor(QPalette::Window, QColor(250, 100, 100));
    wtb->setPalette(palette);
    wtb->setAutoFillBackground(true);

    setMenuWidget(wtb);

    wtb->addAction(icon, tr("action1"));
    wtb->addSeparator();
    QAction *act = wtb->addAction(icon, tr("action2"));
    wtb->insertAction(act, new QAction(icon, tr("action3")));

    setWindowTitle(tr("Good Window Demo"));
    setWindowIcon(icon);
    resize(400, 200);
}

GoodMainWindow::~GoodMainWindow()
{
}


bool GoodMainWindow::event(QEvent *e)
{
     switch (e->type()) {
     case QEvent::WindowStateChange: {
         emit windowStateChanged(windowState());
         break;
     }
     default:
         break;
     }
     return QGoodWindow::event(e);
}
