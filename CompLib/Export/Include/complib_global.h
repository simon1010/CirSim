#ifndef COMPLIB_GLOBAL_H
#define COMPLIB_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef COMPLIB_LIB
# define COMPLIB_EXPORT Q_DECL_EXPORT
#else
# define COMPLIB_EXPORT Q_DECL_IMPORT
#endif

#endif // COMPLIB_GLOBAL_H
