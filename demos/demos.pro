TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS +=  \
    fancydemo \
    qtqrcode \
    qtitanribbon \
    litedemo \
    controldemo

win32 {
SUBDIRS += qtoffice
}
