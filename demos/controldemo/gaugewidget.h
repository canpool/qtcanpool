#ifndef GAUGEWIDGET_H
#define GAUGEWIDGET_H

#include <QWidget>

class QTabWidget;

class GaugeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GaugeWidget(QWidget *parent = nullptr);

private:
    void createWindow();
    void createGaugeProgress();

private:
    QTabWidget *m_pTabWidget;
};

#endif // GAUGEWIDGET_H
