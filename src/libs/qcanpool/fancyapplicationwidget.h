#ifndef FANCYAPPLICATIONWIDGET_H
#define FANCYAPPLICATIONWIDGET_H

#include <QObject>
#include "qcanpool_global.h"

class QToolButton;

QCANPOOL_BEGIN_NAMESPACE

class FancyApplicationWidgetPrivate;
class QCANPOOL_SHARED_EXPORT FancyApplicationWidget : public QObject
{
    Q_OBJECT
public:
    explicit FancyApplicationWidget(QObject *parent = nullptr);
    virtual ~FancyApplicationWidget();

    QToolButton *appButton() const;

    void setAppWidget(QWidget *widget);

private:
    FancyApplicationWidgetPrivate *d;
};

QCANPOOL_END_NAMESPACE

#endif // FANCYAPPLICATIONWIDGET_H
