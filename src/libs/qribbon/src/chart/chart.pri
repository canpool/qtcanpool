

INCLUDEPATH += $$quote($$PWD/../../include)

SOURCES += \
    $$PWD/QtnChart.cpp \
    $$PWD/QtnChartPainter.cpp \
    $$PWD/QtnChartComponent.cpp \
    $$PWD/QtnGraphicsItem.cpp \
    $$PWD/QtnChartTheme.cpp \
    $$PWD/QtnChartTitle.cpp \
    $$PWD/QtnChartTextPainter.cpp \
    $$PWD/QtnChartPalette.cpp \
    $$PWD/QtnChartDataPoint.cpp \
    $$PWD/QtnAbstractSeries.cpp \
    $$PWD/QtnChartArea.cpp \
    $$PWD/QtnChartLegend.cpp \
    $$PWD/QtnChartPointSeries.cpp \
    $$PWD/QtnChart2DSeries.cpp \
    $$PWD/QtnChartArea2D.cpp \
    $$PWD/QtnChartAxis.cpp \
    $$PWD/QtnChartMarker.cpp \
    $$PWD/QtnChartAxisItem.cpp \
    $$PWD/QtnChartScaleTypeMap.cpp \
    $$PWD/QtnChartMath.cpp \
    $$PWD/QtnChartLineSeries.cpp \
    $$PWD/QtnChartBarSeries.cpp \
    $$PWD/QtnChartAreaSeries.cpp \
    $$PWD/QtnChartPieSeries.cpp \
    $$PWD/QtnChartBubbleSeries.cpp \
    $$PWD/QtnChartPyramidSeries.cpp \
    $$PWD/QtnChartFunnelSeries.cpp \

HEADERS += \
    $$PWD/QtnChartDefines.h \
    $$PWD/QtnChart.h \
    $$PWD/QtnChartPrivate.h \
    $$PWD/QtnChartPainter.h \
    $$PWD/QtnChartComponent.h \
    $$PWD/QtnGraphicsItem.h \
    $$PWD/QtnChartThemePrivate.h \
    $$PWD/QtnChartTitle.h \
    $$PWD/QtnChartTitlePrivate.h \
    $$PWD/QtnChartTextPainter.h \
    $$PWD/QtnChartPalette.h \
    $$PWD/QtnChartDataPoint.h \
    $$PWD/QtnChartDataPointPrivate.h \
    $$PWD/QtnAbstractSeries.h \
    $$PWD/QtnSeriesPrivate.h \
    $$PWD/QtnChartArea.h \
    $$PWD/QtnChartAreaPrivate.h \
    $$PWD/QtnChartLegend.h \
    $$PWD/QtnChartLegendPrivate.h \
    $$PWD/QtnChartPointSeries.h \
    $$PWD/QtnChartPointSeriesPrivate.h \
    $$PWD/QtnChart2DSeries.h \
    $$PWD/Qtn2DSeriesPrivate.h \
    $$PWD/QtnChartArea2D.h \
    $$PWD/QtnChartAxis.h \
    $$PWD/QtnChartAxisPrivate.h \
    $$PWD/QtnChartMarker.h \
    $$PWD/QtnChartAxisItemPrivate.h \
    $$PWD/QtnChartScaleTypeMap.h \
    $$PWD/QtnChartMath.h \
    $$PWD/QtnChartLineSeries.h \
    $$PWD/QtnChartBarSeries.h \
    $$PWD/QtnChartAreaSeriesPrivate.h \
    $$PWD/QtnChartAreaSeries.h \
    $$PWD/QtnChartPieSeries.h \
    $$PWD/QtnChartPieSeriesPrivate.h \
    $$PWD/QtnChartBubbleSeries.h \
    $$PWD/QtnChartPyramidSeries.h \
    $$PWD/QtnChartPyramidSeriesPrivate.h \
    $$PWD/QtnChartFunnelSeries.h \
    $$PWD/QtnChartFunnelSeriesPrivate.h \


win32 {
    SOURCES +=
}

unix {
    SOURCES += 
}

RESOURCES += \
    $$PWD/QtnChartResources.qrc


win32 {
    DEFINES += _WINDOWS
    LIBS += -luser32 -lgdi32 -lshell32
}

QT += xml
