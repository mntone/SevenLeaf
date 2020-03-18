#ifndef _OSUTIL_ATOMIC_H
#define _OSUTIL_ATOMIC_H

#include "osutil_base.h"

#ifdef _WIN32
#include "osutil_atomic_win.h"
#else
#include "osutil_atomic_posix.h"
#endif

#endif  // _OSUTIL_ATOMIC_H
