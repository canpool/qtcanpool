VERSION = 0.1.0

QT += widgets

greaterThan(QT_MAJOR_VERSION, 5) {
    # Need to install Qt State Machines (Additional Libraries)
    QT += statemachine
}

equals(QX_MATERIAL_CONFIG_BUILD_STATIC, 1) {
    DEFINES += QX_MATERIAL_LIBRARY_STATIC
} else {
    DEFINES += QX_MATERIAL_LIBRARY
}

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES = \
    $$PWD/materialappbar.cpp \
    $$PWD/materialautocomplete.cpp \
    $$PWD/materialavatar.cpp \
    $$PWD/materialbadge.cpp \
    $$PWD/materialcheckable.cpp \
    $$PWD/materialcheckbox.cpp \
    $$PWD/materialcircularprogress.cpp \
    $$PWD/materialdialog.cpp \
    $$PWD/materialdrawer.cpp \
    $$PWD/materialfab.cpp \
    $$PWD/materialflatbutton.cpp \
    $$PWD/materialiconbutton.cpp \
    $$PWD/materialoverlaywidget.cpp \
    $$PWD/materialprogress.cpp \
    $$PWD/materialradiobutton.cpp \
    $$PWD/materialraisedbutton.cpp \
    $$PWD/materialripple.cpp \
    $$PWD/materialrippleoverlay.cpp \
    $$PWD/materialscrollbar.cpp \
    $$PWD/materialslider.cpp \
    $$PWD/materialsnackbar.cpp \
    $$PWD/materialstatetransition.cpp \
    $$PWD/materialstyle.cpp \
    $$PWD/materialtabbar.cpp \
    $$PWD/materialtextfield.cpp \
    $$PWD/materialtheme.cpp \
    $$PWD/materialtoggle.cpp

HEADERS = \
    $$PWD/materialappbar.h \
    $$PWD/materialappbar_p.h \
    $$PWD/materialautocomplete.h \
    $$PWD/materialautocomplete_p.h \
    $$PWD/materialavatar.h \
    $$PWD/materialavatar_p.h \
    $$PWD/materialbadge.h \
    $$PWD/materialbadge_p.h \
    $$PWD/materialcheckable.h \
    $$PWD/materialcheckable_p.h \
    $$PWD/materialcheckbox.h \
    $$PWD/materialcheckbox_p.h \
    $$PWD/materialcircularprogress.h \
    $$PWD/materialcircularprogress_p.h \
    $$PWD/materialdialog.h \
    $$PWD/materialdialog_p.h \
    $$PWD/materialdrawer.h \
    $$PWD/materialdrawer_p.h \
    $$PWD/materialfab.h \
    $$PWD/materialfab_p.h \
    $$PWD/materialflatbutton.h \
    $$PWD/materialflatbutton_p.h \
    $$PWD/materialiconbutton.h \
    $$PWD/materialiconbutton_p.h \
    $$PWD/materialnamespace.h \
    $$PWD/materialoverlaywidget.h \
    $$PWD/materialprogress.h \
    $$PWD/materialprogress_p.h \
    $$PWD/materialradiobutton.h \
    $$PWD/materialradiobutton_p.h \
    $$PWD/materialraisedbutton.h \
    $$PWD/materialraisedbutton_p.h \
    $$PWD/materialripple.h \
    $$PWD/materialrippleoverlay.h \
    $$PWD/materialscrollbar.h \
    $$PWD/materialscrollbar_p.h \
    $$PWD/materialslider.h \
    $$PWD/materialslider_p.h \
    $$PWD/materialsnackbar.h \
    $$PWD/materialsnackbar_p.h \
    $$PWD/materialstatetransition.h \
    $$PWD/materialstatetransitionevent.h \
    $$PWD/materialstyle.h \
    $$PWD/materialstyle_p.h \
    $$PWD/materialtabbar.h \
    $$PWD/materialtabbar_p.h \
    $$PWD/materialtextfield.h \
    $$PWD/materialtextfield_p.h \
    $$PWD/materialtheme.h \
    $$PWD/materialtheme_p.h \
    $$PWD/materialtoggle.h \
    $$PWD/materialtoggle_p.h \
    $$PWD/qxmaterial_global.h

RESOURCES += \
    $$PWD/qxmaterial.qrc
