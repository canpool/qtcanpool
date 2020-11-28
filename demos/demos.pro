TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS +=  \
    fancydemo \
    fancyribbon \
    qtqrcode \
    qtitanribbon \
    litedemo \
    controldemo

win32 {
SUBDIRS += qtoffice
}
