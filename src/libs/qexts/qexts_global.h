#ifndef QTEXTS_GLOBAL_H
#define QTEXTS_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(QEXTS_LIBRARY)
#  define QEXTSSHARED_EXPORT Q_DECL_EXPORT
#else
#  define QEXTSSHARED_EXPORT Q_DECL_IMPORT
#endif


#endif // QTEXTS_GLOBAL_H
