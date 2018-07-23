#ifndef QFTP_GLOBAL_H
#define QFTP_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(QFTP_LIBRARY)
#  define QFTP_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define QFTP_SHARED_EXPORT Q_DECL_IMPORT
#endif



#endif // QFTP_GLOBAL_H
