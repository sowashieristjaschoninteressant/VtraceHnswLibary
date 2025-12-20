#ifndef VTRACE_COMMON_H
#define VTRACE_COMMON_H

/*
=====================
LIBARY INCLUDES
=====================
*/

#include "stdlib.h"
#include "stdio.h"
#include "fcntl.h"
#include "math.h"
#include "time.h"
#include "assert.h"
#include "errno.h"

/*
=====================
CUSTOM DEFINITIONS
=====================
*/


typedef unsigned char     uint8;
typedef unsigned short    uint16;
typedef unsigned int      uint32;
typedef unsigned long int uint64;

typedef char     int8;
typedef short    int16;
typedef int      int32;
typedef long int int64;
typedef float    float32;

/*
=====================
utility MACROS
=====================
*/
#define HNSW_INLINE static inline
#define HNSW_UNUSED(x) (void)(x)

/*
=====================
DEBUG / assertions
=====================
*/

#ifndef NDEBUG
#define HNSW_ASSERT(x) assert(x)
#else
#define HNSW_ASSERT(x) ((void)0)
#endif

#endif