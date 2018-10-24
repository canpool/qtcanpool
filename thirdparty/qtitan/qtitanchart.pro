TEMPLATE = subdirs

include(src/shared/platform.pri)

CONFIG += ordered
CONFIG += debug_and_release

CONFIG(build_qtitan) {

    SUBDIRS = src/shared/chart \
              src/chartdsgn
}

SUBDIRS += \
          demos/chart/advancedfeatures \
          demos/chart/areacharts \
          demos/chart/barcharts \
          demos/chart/largevaluecharts \
          demos/chart/linecharts \
          demos/chart/piecharts \
          demos/chart/funnelcharts \
          demos/chart/scattercharts \
          demos/chart/pyramidcharts

