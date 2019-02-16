#include <QWidget>
#include <QtitanChart.h>
#include "demomainwindow.h"


class QComboBox;
class QGroupBox;
class QVBoxLayout;
class QFormLayout;

/* ChartWindow */
class ChartWindow : public DemoMainWindow
{
    Q_OBJECT
public:
    ChartWindow(const QString& series, Qtitan::Chart* chart = Q_NULL);
    virtual ~ChartWindow();

protected:
    void createTitle(const QString& title);
    void createChartGroup();
    void createMarkersGroup();
    void createLablesGroup();
    virtual void updateValueParameters();

    QGroupBox* createGpoupParameters(const QString& nameGroup, bool checkable = false);
    QFormLayout* formLayout() const;

protected slots:
    void themeChanged(int);
    void showDataMarkers(bool);
    void markerShapeChanged(int);
    void markerShapeSizeChanged(int);
    void showDataLables(bool);
    void lablesAngleChanged(int);
    void chartExport();

protected:
    void readSettings();
    void writeSettings();

protected:
    virtual void closeEvent(QCloseEvent* event);
    virtual void showEvent(QShowEvent* event);

protected:
    Chart* m_chart;
    QWidget* m_mainGroup;
    QGroupBox* m_dataMarkersGroup;
    QGroupBox* m_dataLablesGroup;
    QComboBox* m_themeSwitcher;
    QComboBox* m_markerShapeSwitcher;
    QLabel* m_markerShapeSizeTitleSwitcher;
    QComboBox* m_markerShapeSizeSwitcher;
    QComboBox* m_angleDataLablesSwitcher;

private:
    Q_DISABLE_COPY(ChartWindow)
};


inline qreal qtnRand(qreal dblStart, qreal dblFinish)
{
    qreal minVal = qMin(dblStart, dblFinish);
    qreal maxVal = qMax(dblStart, dblFinish);

    return (maxVal - minVal) * qreal(qrand()) / RAND_MAX  + minVal;
}
