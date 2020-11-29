/***************************************************************************
 **
 **  Copyright (C) 2020 MaMinJie <canpool@163.com>
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
#include "fancyapplicationwidget.h"
#include "fancypopupwidget.h"
#include "fancybutton.h"

#include <QMouseEvent>
#include <QApplication>
#include <QHBoxLayout>

QCANPOOL_BEGIN_NAMESPACE

class FancyAppPopupWidget : public FancyPopupWidget
{
    Q_OBJECT
public:
    FancyAppPopupWidget(FancyButton *appButton, QWidget *parent = nullptr);
    virtual ~FancyAppPopupWidget();

protected:
    virtual bool event(QEvent* event);

private:
    FancyButton *m_appButton;
};

FancyAppPopupWidget::FancyAppPopupWidget(FancyButton *appButton, QWidget *parent)
    : FancyPopupWidget(parent)
    , m_appButton(appButton)
{}

FancyAppPopupWidget::~FancyAppPopupWidget()
{}

bool FancyAppPopupWidget::event(QEvent *event)
{
    switch (event->type()) {
        case QEvent::MouseButtonDblClick:
        case QEvent::MouseButtonPress: {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            QWidget* widget = QApplication::widgetAt(mouseEvent->globalPos());
            FancyButton *button = qobject_cast<FancyButton *>(widget);
            if (button != nullptr && button == m_appButton) {
                if (mouseEvent->type() == QEvent::MouseButtonDblClick)
                    QMetaObject::invokeMethod(button, "doubleClicked", Qt::QueuedConnection);
                else
                    QMetaObject::invokeMethod(button, "clicked", Qt::QueuedConnection);
                return true;
            }
        }
        break;
        default:
        break;
    }
    return FancyPopupWidget::event(event);
}

class FancyApplicationWidgetPrivate : public QObject
{
    Q_OBJECT
public:
    FancyApplicationWidgetPrivate();
    virtual ~FancyApplicationWidgetPrivate();

    void init();

private slots:
    void slotAppMenuTriggered(QMouseEvent *e);
    void slotPopupWidgetHide();
    void slotPopupWidgetShow();

public:
    FancyApplicationWidget *q;
    FancyButton *m_appButton;
    FancyAppPopupWidget *m_popupWidget;
    QWidget *m_appWidget;
};

FancyApplicationWidgetPrivate::FancyApplicationWidgetPrivate()
    : m_popupWidget(nullptr)
    , m_appWidget(nullptr)
{}

FancyApplicationWidgetPrivate::~FancyApplicationWidgetPrivate()
{
    if (m_popupWidget) {
        delete m_popupWidget;
        m_popupWidget = nullptr;
    }
}

void FancyApplicationWidgetPrivate::init()
{
    m_appButton = new FancyButton();
    m_appButton->setHasMenu(true);
    m_appButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    m_appButton->setIconSize(QSize(18, 18));
    m_appButton->setRound(false);
    connect(m_appButton, SIGNAL(menuTriggered(QMouseEvent *)), this, SLOT(slotAppMenuTriggered(QMouseEvent *)));

    m_popupWidget = new FancyAppPopupWidget(m_appButton);
    QHBoxLayout *lay = new QHBoxLayout();
    lay->setMargin(0);
    lay->setSpacing(0);
    m_popupWidget->setLayout(lay);
    connect(m_popupWidget, SIGNAL(aboutToHide()), this, SLOT(slotPopupWidgetHide()));
    connect(m_popupWidget, SIGNAL(aboutToShow()), this, SLOT(slotPopupWidgetShow()));
}

void FancyApplicationWidgetPrivate::slotAppMenuTriggered(QMouseEvent *e)
{
    if (m_popupWidget == nullptr) {
        return;
    }
    if (m_popupWidget->layout()->itemAt(0) == nullptr) {
        return;
    }
    FancyButton *button = qobject_cast<FancyButton *>(sender());
    if (button == nullptr) {
        return;
    }

    int x = e->x();
    int y = e->y();
    QPoint pos = e->globalPos();
    pos.setX(pos.x() - x - 1);
    pos.setY(pos.y() - y + button->height());
    m_popupWidget->setGeometry(pos.x(), pos.y(), m_appWidget->width(), m_appWidget->height());
    m_popupWidget->setWindowModality(Qt::WindowModal);
    m_popupWidget->show();
}

void FancyApplicationWidgetPrivate::slotPopupWidgetHide()
{
    if (m_appButton) {
        m_appButton->select(false);
    }
}

void FancyApplicationWidgetPrivate::slotPopupWidgetShow()
{
    if (m_appButton) {
        m_appButton->select(true);
    }
}

FancyApplicationWidget::FancyApplicationWidget(QObject *parent)
    : QObject(parent)
    , d(new FancyApplicationWidgetPrivate())
{
    d->q = this;
    d->init();
}

FancyApplicationWidget::~FancyApplicationWidget()
{
    delete d;
}

QToolButton *QCanpool::FancyApplicationWidget::appButton() const
{
    return d->m_appButton;
}

void FancyApplicationWidget::setAppWidget(QWidget *widget)
{
    if (widget == nullptr || widget == d->m_appWidget) {
        return;
    }
    if (d->m_appWidget == nullptr) {
        d->m_popupWidget->layout()->addWidget(widget);
    } else {
        d->m_popupWidget->layout()->removeWidget(widget);
        d->m_popupWidget->layout()->addWidget(widget);
    }
    d->m_appWidget = widget;
}

QCANPOOL_END_NAMESPACE

#include "fancyapplicationwidget.moc"
