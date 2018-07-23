TEMPLATE = subdirs
CONFIG += ordered

THIRDPARTY_DIR = $$PWD/../thirdparty
QTITAN_DIR = $$THIRDPARTY_DIR/qtitan

SUBDIRS +=  \
            fancydemo \
            qtoffice \
            qtqrcode \
            $$QTITAN_DIR/demos/ribbon \
            $$QTITAN_DIR/demos/chart
