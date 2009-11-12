#ifdef MINGL_PLATFORM_UNIX

// todo; remove all these
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <map>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysymdef.h>
#include <xmmintrin.h>

#ifndef MINGL_PRE_ALIGN
    #define MINGL_PRE_ALIGN(a)
#endif

#ifndef MINGL_POST_ALIGN
    #define MINGL_POST_ALIGN(a) __attribute__((aligned(a)))
#endif

#ifndef MINGL_ALLOCATE_MEMORY
    #define MINGL_ALLOCATE_MEMORY(size) malloc(size)
#endif

#ifndef MINGL_FREE_MEMORY
    #define MINGL_FREE_MEMORY(ptr) free(ptr)
#endif

namespace mingl
{
    typedef __m128 NativeVectorType;
}

#define MINGL_VECTOR_LOAD_UNALIGNED(r, fptr) do { r = _mm_loadu_ps(fptr); } while(0)

#define MINGL_VECTOR_SET_FROM_FLOATS(r, a, b, c, d) do { r = _mm_setr_ps(a, b, c, d); } while(0)

#define MINGL_VECTOR_SPLAT_1(r, f) do { r = _mm_set1_ps(f); } while(0)

#define MINGL_VECTOR_SPLAT_I(r, v, index) do { r = _mm_shuffle_ps(v, v, _MM_SHUFFLE(index, index, index, index)); } while(0)

// todo; presumably there's a non-ass way to do this
#define MINGL_VECTOR_CEIL(r, v0) do {                   \
    float af;                                           \
    _mm_store_ss(&af, v0);                              \
    af = ceil(af);                                      \
    r = _mm_set1_ps(af);                                \
} while(0)

// todo; presumably there's a non-ass way to do this
#define MINGL_VECTOR_FLOOR(r, v0) do {                  \
    float af;                                           \
    _mm_store_ss(&af, v0);                              \
    af = floor(af);                                     \
    r = _mm_set1_ps(af);                                \
} while(0)

#define MINGL_VECTOR_ADD(r, v0, v1) do { r = _mm_add_ps(v0, v1); } while(0)

#define MINGL_VECTOR_SUBTRACT(r, v0, v1) do { r = _mm_sub_ps(v0, v1); } while(0)

#define MINGL_VECTOR_MULTIPLY(r, v0, v1) do { r = _mm_mul_ps(v0, v1); } while(0)

#define MINGL_VECTOR_DIVIDE(r, v0, v1) do { r = _mm_div_ps(v0, v1); } while(0)

#define MINGL_VECTOR_SCALAR_IS_LESS(r, v0, v1) do {     \
    float af, bf;                                       \
    _mm_store_ss(&af, v0);                              \
    _mm_store_ss(&bf, v1);                              \
    return af < bf;                                     \
} while(0)

#define MINGL_VECTOR_SCALAR_IS_GREATER(r, v0, v1) do {  \
    float af, bf;                                       \
    _mm_store_ss(&af, v0);                              \
    _mm_store_ss(&bf, v1);                              \
    return af > bf;                                     \
} while(0)

#endif
