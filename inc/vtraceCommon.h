#ifndef VTRACE_COMMON_H
#define VTRACE_COMMON_H

/*
=====================
LIBARY INCLUDES
=====================
*/

#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "math.h"
#include "time.h"
#include "assert.h"
#include "errno.h"
#include "stdalign.h"
#include "stdbool.h"
#include "float.h"
#include "stddef.h"
#include "stdint.h"


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

#define MIN(a,b) a > b ? b : a
#define MAX(a,b) a > b ? a : b
/*
=====================
DEBUG / assertions
=====================
*/

#ifndef NDEBUG
#define HNSW_ASSERT(x) assert(x)
 #define HNSW_LOG(x) printf("[!]LOG: %s\n", x)
#else
#define HNSW_ASSERT(x) ((void)0)
#endif

#endif