add_qtc_library(qxfluent
  VERSION 0.1.0
  DEFINES QX_FLUENT_LIBRARY
  PUBLIC_DEPENDS qxcontrols Qt5::Core Qt5::Gui Qt5::Widgets
  SOURCES
    qxfluent_global.h
    fluenttokens.h fluenttokens.cpp
    fluentswitch.h fluentswitch.cpp
    fluentslider.h fluentslider.cpp
    fluentbadge.h fluentbadge.cpp
    fluentcard.h fluentcard.cpp
    fluentprogressring.h fluentprogressring.cpp
    fluentrating.h fluentrating.cpp
    fluentexpander.h fluentexpander.cpp
    fluentinfobar.h fluentinfobar.cpp
    fluentbreadcrumbbar.h fluentbreadcrumbbar.cpp
)
