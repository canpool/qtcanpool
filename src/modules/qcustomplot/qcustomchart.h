#ifndef QCUSTOMCHART_H
#define QCUSTOMCHART_H

#include <QWidget>
#include "qcustomplot.h"

class QLabel;
class QToolBar;
class QToolButton;
class QStackedWidget;
class QRubberBand;
class CustomPlotZoom;
class QTextBrowser;

///////////////////////////////////////////////////////////////
/// \brief The QCustomChart class
///
class QCustomChart : public QWidget
{
    Q_OBJECT
public:
    explicit QCustomChart(QWidget *parent = 0);
    ~QCustomChart();

    void setTitle(const QString &text);

    void setXaxisLabel(const QString &text);
    void setYaxisLabel(const QString &text);

    void setYaxisRange(double lower, double upper);

//    void setData(const QVector<double> &keys, const QVector<double> &values);

private:
    void createWindow();
//    void initCustomPlot();

signals:

public slots:
    void slotRefresh(); // restore
    void slotDownload();
    void slotPrimaryData();
    void slotChart();

private slots:
    void mousePress(QMouseEvent* mevent);
    void mouseMove(QMouseEvent *mevent);
    void mouseRelease(QMouseEvent *mevent);

public:
    CustomPlotZoom *m_pCustomPlot;
    QTextBrowser *m_pTextBrowser;

private:
    QStackedWidget *m_pStackedWidget;
    QRubberBand *m_pRubberBand;
    QPoint m_rubberOrigin;

    QLabel *m_pTitleLabel;
    QToolBar *m_pToolBar;

    QAction *m_pPrimaryAction;
    QAction *m_pChartAction;
    QAction *m_pRefreshAction;
    QAction *m_pDownloadAction;

    double m_yLower;
    double m_yUpper;
};
///////////////////////////////////////////////////////////
/// \brief The CustomPlotZoom class
///
class CustomPlotZoom : public QCustomPlot
{
    Q_OBJECT

public:
    CustomPlotZoom(QWidget * parent = 0);
    virtual ~CustomPlotZoom();

    void setZoomMode(bool mode);

protected:
    void mousePressEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);

private:
    bool m_bZoomMode;
    QRubberBand * m_pRubberBand;
    QPoint m_origin;
};

#endif // QCUSTOMCHART_H
