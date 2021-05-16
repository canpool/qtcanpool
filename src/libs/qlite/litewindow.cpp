/***************************************************************************
 **
 **  Copyright (C) 2019-2020 MaMinJie <canpool@163.com>
 **  Contact: https://github.com/canpool
 **           https://gitee.com/icanpool
 **
 **  GNU Lesser General Public License Usage
 **  Alternatively, this file may be used under the terms of the GNU Lesser
 **  General Public License version 3 as published by the Free Software
 **  Foundation and appearing in the file LICENSE.LGPL3 included in the
 **  packaging of this file. Please review the following information to
 **  ensure the GNU Lesser General Public License version 3 requirements
 **  will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
 **
 **  GNU General Public License Usage
 **  Alternatively, this file may be used under the terms of the GNU
 **  General Public License version 2.0 or (at your option) the GNU General
 **  Public license version 3 or any later version approved by the KDE Free
 **  Qt Foundation. The licenses are as published by the Free Software
 **  Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
 **  included in the packaging of this file. Please review the following
 **  information to ensure the GNU General Public License requirements will
 **  be met: https://www.gnu.org/licenses/gpl-2.0.html and
 **  https://www.gnu.org/licenses/gpl-3.0.html.
 **
****************************************************************************/
#include "litewindow.h"

#include <QLabel>
#include <QToolButton>
#include <QToolBar>
#include <QHBoxLayout>
#include <QStatusBar>
#include <QMenuBar>

#include "litebar.h"
#include "litescreen.h"

QLITE_BEGIN_NAMESPACE

class LiteWindowPrivate : public QObject
{
    Q_OBJECT
public:
    LiteWindowPrivate();
    ~LiteWindowPrivate();

    LiteBar *m_liteBar;
    QStatusBar *m_statusBar;
    QMenuBar *m_menuBar;
    QHBoxLayout *m_layout;

public slots:
    void slotResizable(bool resizable);
};

LiteWindowPrivate::LiteWindowPrivate()
    : m_liteBar(nullptr),
      m_statusBar(nullptr),
      m_menuBar(nullptr),
      m_layout(nullptr)
{
}

LiteWindowPrivate::~LiteWindowPrivate()
{
    m_statusBar = nullptr;
}

void LiteWindowPrivate::slotResizable(bool resizable)
{
    if (m_statusBar) {
        m_statusBar->setSizeGripEnabled(resizable);
    }
}

LiteWindow::LiteWindow(QWidget *parent, Qt::WindowFlags f)
    : QMainWindow(parent), d(new LiteWindowPrivate())
{
    QMainWindow::setWindowFlags(Qt::FramelessWindowHint |
                   Qt::WindowSystemMenuHint |
                   Qt::WindowMinimizeButtonHint |
                   Qt::WindowMaximizeButtonHint |
                   Qt::Window | f
                  );
    d->m_liteBar = new LiteBar(this);
    QObject::connect(d->m_liteBar, SIGNAL(sigWidgetResizable(bool)),
                     d, SLOT(slotResizable(bool)));

    QWidget *menuWidget = new QWidget(this);
    d->m_layout = new QHBoxLayout();
    d->m_layout->setSpacing(0);
    d->m_layout->setMargin(0);
    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->setContentsMargins(2, 0, 2, 0);
    mainLayout->setSpacing(1);
    mainLayout->addWidget(d->m_liteBar->logoButton(), 0, Qt::AlignCenter);
    mainLayout->addLayout(d->m_layout);
    mainLayout->addWidget(d->m_liteBar->titleLabel(), 0, Qt::AlignCenter);
    mainLayout->addStretch();
    mainLayout->addWidget(d->m_liteBar->sysToolBar(), 0, Qt::AlignTop);
    menuWidget->setLayout(mainLayout);
    menuWidget->setObjectName(QLatin1String("qlite_titlebar"));
    d->m_liteBar->setTitleBar(menuWidget);

    setMenuWidget(menuWidget);

    setMouseTracking(true);
    QRect geom = LiteScreen::normalRect();
    resize(geom.width(), geom.height());
    raise();
    activateWindow();

    setObjectName(QLatin1String("qlite_window"));
}

LiteWindow::~LiteWindow()
{
    delete d;
}

LiteBar *LiteWindow::liteBar() const
{
    return d->m_liteBar;
}

QMenuBar *LiteWindow::menuBar() const
{
    if (d->m_menuBar == nullptr) {
        d->m_menuBar = new QMenuBar();
#if !defined(Q_OS_WIN)
        d->m_menuBar->setNativeMenuBar(false);
        d->m_menuBar->setVisible(true);
#endif
        d->m_layout->addWidget(d->m_menuBar, 0, Qt::AlignCenter);
        d->m_layout->addStretch();
    }
    return d->m_menuBar;
}

void LiteWindow::setMenuBar(QMenuBar *menuBar)
{
    if (d->m_menuBar == menuBar || menuBar == nullptr) {
        return;
    }
    if (d->m_menuBar) {
        d->m_layout->removeWidget(d->m_menuBar);
        delete d->m_menuBar;
        d->m_menuBar = menuBar;
        d->m_layout->insertWidget(0, d->m_menuBar, 0, Qt::AlignCenter);
    } else {
        d->m_menuBar = menuBar;
        d->m_layout->addWidget(d->m_menuBar, 0, Qt::AlignCenter);
        d->m_layout->addStretch();
    }
#if !defined(Q_OS_WIN)
    d->m_menuBar->setNativeMenuBar(false);
    d->m_menuBar->setVisible(true);
#endif
}

QStatusBar *LiteWindow::statusBar() const
{
    d->m_statusBar = QMainWindow::statusBar();
    return d->m_statusBar;
}

void LiteWindow::setStatusBar(QStatusBar *statusbar)
{
    d->m_statusBar = statusbar;
    QMainWindow::setStatusBar(statusbar);
}

void LiteWindow::setFixedSize(const QSize &s)
{
    setFixedSize(s.width(), s.height());
}

void LiteWindow::setFixedSize(int w, int h)
{
    d->m_liteBar->setWidgetResizable(false);
    QWidget::setFixedSize(w, h);
}

void LiteWindow::setFixedWidth(int w)
{
    setFixedSize(w, this->height());
}

void LiteWindow::setFixedHeight(int h)
{
    setFixedSize(this->width(), h);
}

void LiteWindow::setWindowFlags(Qt::WindowFlags type)
{
    QMainWindow::setWindowFlags(type);
    d->m_liteBar->updateWidgetFlags();
}

QLITE_END_NAMESPACE

#include "litewindow.moc"
