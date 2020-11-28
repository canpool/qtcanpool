#ifndef FANCYPOPUPWIDGET_H
#define FANCYPOPUPWIDGET_H

#include <QWidget>
#include "qcanpool_global.h"

QCANPOOL_BEGIN_NAMESPACE

class QCANPOOL_SHARED_EXPORT FancyPopupWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FancyPopupWidget(QWidget *parent = nullptr);
    virtual ~FancyPopupWidget();

signals:
    void aboutToShow();
    void aboutToHide();

protected:
    virtual bool event(QEvent* event);
};

QCANPOOL_END_NAMESPACE

#endif // FANCYPOPUPWIDGET_H
