QTC_LIB_NAME = qcanpool
QTC_LIB_VERSION = 2.0.2
# The forwarding headers of the widgets that moved to qxapp in 3.1 (K5) derive
# from their QxApp:: counterparts, so the frozen qmake build needs qxapp here.
QTC_LIB_DEPENDS += qxapp
