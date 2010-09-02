#ifndef __PLAT_TYPES_H__
#define __PLAT_TYPES_H__

#ifdef FAFT_CHEWING
#include "plat_posix.h"
#else
#include "sys/plat_posix.h"
#include "sys/plat_win32.h"
#endif

#endif /* __PLAT_TYPES_H__ */

