TEMPLATE = subdirs
CONFIG += ordered

# The examples for the retired legacy widgets (fancytitlebar, minitabwidget,
# windowlogo, windowtoolbar) went away with them in 3.1 (A2). The four left
# demonstrate widgets that now live in qxapp and are still reachable through
# the deprecated qcanpool names.
SUBDIRS = \
    tinytabbar \
    tinytabwidget \
    extensionbutton \
    fancytoolbutton
