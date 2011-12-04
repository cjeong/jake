/**
 * @file types.h
 * @desc type definitions
 */
#ifndef TYPES_H
#define TYPES_H

#ifndef NULL
#define NULL ((void *) 0)
#endif

/* for true-or-false values */
typedef int bool;

/* integer types */
typedef unsigned char uchar;
typedef unsigned char uint8_t;
typedef __signed char int8_t;

typedef unsigned short ushort;
typedef unsigned short uint16_t;
typedef short int16_t;

typedef unsigned int uint;
typedef unsigned int uint32_t;
typedef int int32_t;

typedef unsigned long long uint64_t;
typedef long long int64_t;

/* size_t is used for memory object sizes */
typedef uint32_t size_t;

/* ssize_t is a signed version of ssize_t, used in case there might be an
   error return */
typedef int32_t ssize_t;

/* pointers and addresses are 32 bits long; we use pointer types to 
   represent virtual addresses; uintptr_t to represent the numerical 
   values of virtual addresses, and physaddr_t to represent physical 
   addresses (NOTE: used for memory management) */
typedef int32_t intptr_t;
typedef uint32_t uintptr_t;
typedef uint32_t physaddr_t;


/* efficient min and max operations */
#define MIN(a, b)                             \
  ({                                          \
    __typeof__(a) _a = (a);                   \
    __typeof__(b) _b = (b);                   \
    _a <= _b ? _a : _b;                       \
  })
#define MAX(a, b)                             \
  ({                                          \
    __typeof__(a) _a = (a);                   \
    __typeof__(b) _b = (b);                   \
    _a >= _b ? _a : _b;                       \
  })

/* rounding operations (efficient when n is a power of 2); round down to 
   the nearest multiple of n */
#define ROUNDDOWN(a, n)                       \
  ({                                          \
    uint32_t _a = (uint32_t) (a);             \
    (typeof(a)) (_a - _a % (n));              \
  })

/* round up to the nearest multiple of n */
#define ROUNDUP(a, n)                                       \
  ({                                                        \
    uint32_t _n = (uint32_t) (n);                           \
    (typeof(a)) (ROUNDDOWN((uint32_t) (a) + _n - 1, _n));   \
  })

/* returns the offset of 'member' relative to the beginning of a struct type */
#define offsetof(type, member)  ((size_t) (&((type *) 0)->member))

#endif /* TYPES_H */
