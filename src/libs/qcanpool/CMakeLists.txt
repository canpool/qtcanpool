# qcanpool is what remains of the frozen legacy library (decisions K3/K5). In
# 3.1 its general-purpose widgets moved to qxapp (A1) and the window-chrome
# family was retired (A2), so all it still carries are the deprecated
# forwarding classes that keep 3.0 code compiling for one release; those names
# disappear in 3.2. That is also why the library depends on qxapp.
add_qtc_library(qcanpool
  PUBLIC_DEPENDS qxapp Qt5::Core Qt5::Widgets
  SOURCES
    extensionbutton.h
    fancytoolbutton.h
    menuaccessbutton.h
    menubutton.h
    qcanpool.h
    qcanpool.qrc
    tinynavbar.h
    tinytabbar.h
    tinytabwidget.h
)
