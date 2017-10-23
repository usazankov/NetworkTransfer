#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(NETWORKTRANSFER_LIB)
#  define NETWORKTRANSFER_EXPORT Q_DECL_EXPORT
# else
#  define NETWORKTRANSFER_EXPORT Q_DECL_IMPORT
# endif
#else
# define NETWORKTRANSFER_EXPORT
#endif
