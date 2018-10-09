#ifndef REP_GLOBAL_H
#define REP_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(REP_LIBRARY)
#  define REPSHARED_EXPORT Q_DECL_EXPORT
#else
#  define REPSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // REP_GLOBAL_H
