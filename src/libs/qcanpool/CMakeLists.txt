# qcanpool is the frozen legacy library (decision K3/K5). In 3.1 the
# general-purpose widgets it used to own (buttons, tab bar, navigation bar) were
# moved into qxapp and are re-exported from here as deprecated forwarding
# classes, which is why this library now depends on qxapp. What is left is the
# window-chrome family and the names above it; the former is scheduled for
# removal (A2), the latter in 3.2.
add_qtc_library(qcanpool
  PUBLIC_DEPENDS qxapp Qt5::Core Qt5::Widgets
  SOURCES
    extensionbutton.h
    fancybar.cpp fancybar.h fancybar_p.h
    fancydialog.cpp fancydialog.h
    fancytabbar.cpp fancytabbar.h fancytabbar_p.h
    fancytabwidget.cpp fancytabwidget.h
    fancytitlebar.cpp fancytitlebar.h fancytitlebar_p.h
    fancytoolbutton.h
    fancywindow.cpp fancywindow.h
    menuaccessbutton.h
    menubutton.h
    minitabbar.cpp minitabbar.h
    minitabwidget.cpp minitabwidget.h
    qcanpool.h
    qcanpool.qrc
    quickaccessbar.cpp quickaccessbar.h quickaccessbar_p.h
    tinynavbar.h
    tinytabbar.h
    tinytabwidget.h
    windowlogo.cpp windowlogo.h
    windowtoolbar.cpp windowtoolbar.h windowtoolbar_p.h
)
