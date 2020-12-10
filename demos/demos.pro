TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS +=  \
    fancydemo \
    fancyribbon \
    qtqrcode \
    litedemo \
    controldemo

win32 {
SUBDIRS += qtoffice
}
