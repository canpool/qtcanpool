#
#  W A R N I N G
#  -------------
#
# This file is only lib-pro of qtcanpool.
#
include(../../library.pri)


# remove some macros in qtproject.pri
DEFINES -= \
    QT_NO_CAST_TO_ASCII \
    QT_RESTRICTED_CAST_FROM_ASCII \
    "QT_DISABLE_DEPRECATED_BEFORE=0x050900"

include(qtitan-lib.pri)
