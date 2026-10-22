# C3: Fluent's window materials are qxwindow's Mica / Acrylic / Mica Alt, so the
# one file that wraps them reaches sideways to qxwindow - and only when qxwindow
# is part of the build. Without it the wrapper still builds and still answers
# "no", which is the honest answer from a platform that has no such materials.
set(QXFLUENT_WINDOW_DEPENDS "")
set(QXFLUENT_WINDOW_DEFINES "")
if (WITH_QXWINDOW)
  list(APPEND QXFLUENT_WINDOW_DEPENDS qxwindow)
  list(APPEND QXFLUENT_WINDOW_DEFINES QX_FLUENT_HAS_WINDOW)
endif()

add_qtc_library(qxfluent
  VERSION 0.1.0
  DEFINES QX_FLUENT_LIBRARY ${QXFLUENT_WINDOW_DEFINES}
  PUBLIC_DEPENDS qxcontrols ${QXFLUENT_WINDOW_DEPENDS} Qt5::Core Qt5::Gui Qt5::Widgets
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
    fluentcommandbar.h fluentcommandbar.cpp
    fluentnavigationview.h fluentnavigationview.cpp
    fluenttabview.h fluenttabview.cpp
    fluentteachingtip.h fluentteachingtip.cpp
    fluentwindowmaterial.h fluentwindowmaterial.cpp
    fluentreveal.h fluentreveal.cpp
)
