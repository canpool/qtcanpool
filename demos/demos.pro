TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS = \
    fancydemo \
    fancyribbon \
    qtqrcode \
    litedemo \
    controldemo \
    qtoffice

!macx {
SUBDIRS += licenseDemo
}
