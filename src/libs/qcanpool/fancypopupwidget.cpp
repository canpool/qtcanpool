#include "fancypopupwidget.h"
#include "fancytab.h"

#include <QEvent>

QCANPOOL_USE_NAMESPACE

FancyPopupWidget::FancyPopupWidget(QWidget *parent)
    : QWidget(parent, Qt::Popup)
{

}

FancyPopupWidget::~FancyPopupWidget()
{

}

bool FancyPopupWidget::event(QEvent *event)
{
    switch (event->type()) {
        case QEvent::Show: {
            emit aboutToShow();
        }
        break;
        case QEvent::Hide: {
            emit aboutToHide();
        }
        break;
        default:
        break;
    }
    return QWidget::event(event);
}
