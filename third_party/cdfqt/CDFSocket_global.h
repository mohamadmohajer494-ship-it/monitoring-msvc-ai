#ifndef CDFSOCKET_GLOBAL_H
#define CDFSOCKET_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(CDFSOCKET_LIBRARY)
#  define CDFSOCKET_EXPORT Q_DECL_EXPORT
#else
#  define CDFSOCKET_EXPORT Q_DECL_IMPORT
#endif

#endif // CDFSOCKET_GLOBAL_H
