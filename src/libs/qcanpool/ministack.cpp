/***************************************************************************
 **
 **  Copyright (C) 2018 MaMinJie <canpool@163.com>
 **  Contact: https://github.com/canpool
 **
 **  This program is free software: you can redistribute it and/or modify
 **  it under the terms of the GNU General Public License as published by
 **  the Free Software Foundation, either version 3 of the License, or
 **  (at your option) any later version.
 **
 **  This program is distributed in the hope that it will be useful,
 **  but WITHOUT ANY WARRANTY; without even the implied warranty of
 **  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 **  GNU General Public License for more details.
 **
 **  You should have received a copy of the GNU General Public License
 **  along with this program.  If not, see http://www.gnu.org/licenses/.
 **
****************************************************************************/
#include "ministack.h"
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
#include <QtGui>
#else
#include <QtWidgets>
#endif

#include "qcanpool.h"

class MiniStackPrivate : public QObject
{
    Q_OBJECT
public:
    MiniStackPrivate(QObject *parent = 0);
    void init();
    void createWindow();

    void setTabColor();

    void addAction(QAction *action);

public:
    MiniStack *q;

    QTabBar *m_pTabBar;
    QToolBar *m_pToolBar;
    QStackedWidget *m_pStackedWidget;
    QStackedWidget *m_pToolBarWidgets;
    QLabel *m_pTitleLabel;
    QAction *m_pExpandAction;
    QAction *m_pSetupAction;

    QWidget *m_pTitleWidget;
    QWidget *m_pBodyWidget;
    QWidget *m_pSpacer;

    QColor m_titleColor;
    QColor m_spacerColor;
    QColor m_backgroundColor;

    QColor m_tabBackgroundColor;
    QColor m_tabTextColor;
    QColor m_tabHoverBackgroundColor;
    QColor m_tabHoverTextColor;
    QColor m_tabSelectedBackgroundColor;
    QColor m_tabSelectedTextColor;

    bool m_bExpand;

private slots:
    void slotExpand();
    void slotCurrentChanged();
};

MiniStackPrivate::MiniStackPrivate(QObject *parent)
    : QObject(parent)
{
    m_bExpand = true;

    m_spacerColor = QColor(230,230,230);
    m_titleColor = QColor(50,50,50);
    m_backgroundColor = QColor(255,255,255);

    m_tabBackgroundColor = QColor(35,198,200);
    m_tabTextColor = QColor(255,255,255);
    m_tabHoverBackgroundColor = QColor(71,218,222);
    m_tabHoverTextColor = QColor(255,255,255);
    m_tabSelectedBackgroundColor = QColor(28,156,159);
    m_tabSelectedTextColor = QColor(255,255,255);
}

void MiniStackPrivate::init()
{
    createWindow();

    q->setTitleColor(m_titleColor);
    q->setSpacerColor(m_spacerColor);
    q->setBackgroundColor(m_backgroundColor);

    setTabColor();

    q->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
}

void MiniStackPrivate::createWindow()
{
    QFont font("Segoe UI",10);

    m_pTitleWidget = new QWidget();

    m_pTitleLabel = new QLabel(tr("Title"));
    font.setBold(true);
    font.setPointSize(12);
    m_pTitleLabel->setFont(font);

    m_pTabBar = new QTabBar();
    m_pToolBarWidgets = new QStackedWidget();
    m_pToolBarWidgets->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    m_pToolBar = new QToolBar();
    m_pToolBar->setIconSize(QSize(20,20));

    QAction *action = new QAction(QIcon(":/tools/arrow_up"), tr("collapse"), this);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(slotExpand()));
    m_pExpandAction = action;
    action = new QAction(QIcon(":/tools/setup"), tr("setup"), this);
    connect(action, SIGNAL(triggered(bool)), q, SIGNAL(sigSetupClicked()));
    m_pSetupAction = action;
    m_pToolBar->addAction(m_pExpandAction);
    m_pToolBar->addAction(m_pSetupAction);

    QHBoxLayout *pTitleLayout = new QHBoxLayout();
    pTitleLayout->setSpacing(0);
    pTitleLayout->setContentsMargins(5,1,5,1);
    pTitleLayout->addWidget(m_pTitleLabel);
    pTitleLayout->addSpacing(10);
    pTitleLayout->addWidget(m_pToolBarWidgets);
    pTitleLayout->addStretch();
    pTitleLayout->addWidget(m_pTabBar);
    pTitleLayout->addSpacing(2);
    pTitleLayout->addWidget(m_pToolBar);
    m_pTitleWidget->setLayout(pTitleLayout);

    m_pSpacer = new QWidget();
    m_pSpacer->setFixedHeight(1);

    m_pBodyWidget = new QWidget();
    m_pStackedWidget = new QStackedWidget();
    QHBoxLayout *pBodyLayout = new QHBoxLayout();
    pBodyLayout->setSpacing(0);
    pBodyLayout->setContentsMargins(5,5,5,5);
    pBodyLayout->addWidget(m_pStackedWidget);
    m_pBodyWidget->setLayout(pBodyLayout);

    QVBoxLayout *pMainLayout = new QVBoxLayout();
    pMainLayout->setSpacing(0);
    pMainLayout->setContentsMargins(0,0,0,0);
    pMainLayout->addWidget(m_pTitleWidget);
    pMainLayout->addWidget(m_pSpacer);
    pMainLayout->addWidget(m_pBodyWidget);

    q->setLayout(pMainLayout);

    connect(m_pTabBar, SIGNAL(currentChanged(int)), m_pStackedWidget, SLOT(setCurrentIndex(int)));
    connect(m_pTabBar, SIGNAL(currentChanged(int)), m_pToolBarWidgets, SLOT(setCurrentIndex(int)));
    connect(m_pTabBar, SIGNAL(currentChanged(int)), this, SLOT(slotCurrentChanged()));
}

void MiniStackPrivate::setTabColor()
{
    q->setStyleSheet(QString(""
                "QTabBar::tab {"
                "    background-color: #%1;"
                "    color: #%2;"
                "    padding-left: 8px;"
                "    padding-right: 8px;"
                "    padding-top: 4px;"
                "    padding-bottom: 4px;"
                "}"
                "QTabBar::tab:hover {"
                "    background-color: #%3;"
                "    color: #%4;"
                "}"
                "QTabBar::tab:selected {"
                "    background-color: #%5;"
                "    color: #%6;"
                "}")
                  .arg(QCanpool::colorToArgb(m_tabBackgroundColor))
                  .arg(QCanpool::colorToArgb(m_tabTextColor))
                  .arg(QCanpool::colorToArgb(m_tabHoverBackgroundColor))
                  .arg(QCanpool::colorToArgb(m_tabHoverTextColor))
                  .arg(QCanpool::colorToArgb(m_tabSelectedBackgroundColor))
                  .arg(QCanpool::colorToArgb(m_tabSelectedTextColor))
                  );
}


void MiniStackPrivate::addAction(QAction *action)
{
    if(action == NULL) return;
    m_pToolBar->insertAction(m_pExpandAction, action);
}

void MiniStackPrivate::slotExpand()
{
    m_bExpand = !m_bExpand;
    m_pBodyWidget->setHidden(!m_bExpand);
    m_pSpacer->setHidden(!m_bExpand);
    if(m_bExpand){
        m_pExpandAction->setIcon(QIcon(":/tools/arrow_up"));
        m_pExpandAction->setToolTip(tr("collapse"));
        q->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    }else{
        m_pExpandAction->setIcon(QIcon(":/tools/arrow_down"));
        m_pExpandAction->setToolTip(tr("expand"));
        q->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    }
}

void MiniStackPrivate::slotCurrentChanged()
{
    if(!m_bExpand){
        slotExpand();
    }
}

MiniStack::MiniStack(QWidget *parent)
    : QWidget(parent),d(new MiniStackPrivate(this))
{
    d->q = this;
    d->init();
}

void MiniStack::addTab(const QString &text, QWidget *widget, QWidget *appendix)
{
    d->m_pTabBar->addTab(text);
    d->m_pStackedWidget->addWidget(widget);
    if(appendix){
        d->m_pToolBarWidgets->addWidget(appendix);
    }else{
        d->m_pToolBarWidgets->addWidget(new QWidget);
    }
    d->m_pTabBar->setVisible(d->m_pTabBar->count() > 1);
}

void MiniStack::addAction(QAction *action)
{
    d->addAction(action);
}

void MiniStack::setIconSize(const QSize &size)
{
    d->m_pToolBar->setIconSize(size);
}

void MiniStack::setBodySpacing(int spacing)
{
    d->m_pBodyWidget->layout()->setSpacing(spacing);
}

void MiniStack::setBodyMargins(int left, int top, int right, int bottom)
{
    d->m_pBodyWidget->layout()->setContentsMargins(left,top,right,bottom);
}

void MiniStack::setTitleSpacing(int spacing)
{
    d->m_pTitleWidget->layout()->setSpacing(spacing);
}

void MiniStack::setTitleMargins(int left, int top, int right, int bottom)
{
    d->m_pTitleWidget->layout()->setContentsMargins(left,top,right,bottom);
}

void MiniStack::setTitle(const QString &text)
{
    d->m_pTitleLabel->setText(text);
}

void MiniStack::setTitleColor(const QColor &color)
{
    d->m_titleColor = color;
    d->m_pTitleLabel->setStyleSheet(QString("background-color:transparent;color:#%1;")
                                 .arg(QCanpool::colorToArgb(color))
                                 );
}

void MiniStack::setSpacerColor(const QColor &color)
{
    d->m_spacerColor = color;
    d->m_pSpacer->setStyleSheet(QString("background-color: #%1;")
                             .arg(QCanpool::colorToArgb(color))
                             );
}

void MiniStack::setBackgroundColor(const QColor &color)
{
    d->m_backgroundColor = color;
    QPalette palette;
    palette.setColor(QPalette::Background, color);
    this->setPalette(palette);
    this->setAutoFillBackground(true);
}

void MiniStack::setTabBackgroundColor(const QColor &color)
{
    d->m_tabBackgroundColor = color;
    d->setTabColor();
}

void MiniStack::setTabTextColor(const QColor &color)
{
    d->m_tabTextColor = color;
    d->setTabColor();
}

void MiniStack::setTabHoverBackgroundColor(const QColor &color)
{
    d->m_tabHoverBackgroundColor = color;
    d->setTabColor();
}

void MiniStack::setTabHoverTextColor(const QColor &color)
{
    d->m_tabHoverTextColor = color;
    d->setTabColor();
}

void MiniStack::setTabSelectedBackgroundColor(const QColor &color)
{
    d->m_tabSelectedBackgroundColor = color;
    d->setTabColor();
}

void MiniStack::setTabSelectedTextColor(const QColor &color)
{
    d->m_tabSelectedTextColor = color;
    d->setTabColor();
}

#include "ministack.moc"
