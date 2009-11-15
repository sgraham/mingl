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

    
// todo; this seems horribly complicated
#define MINGL_VECTOR_DOT(r, v0, v1) do {                \
    NativeVectorType prod = _mm_mul_ps(v0, v1);         \
    NativeVectorType b = _mm_shuffle_ps(prod, prod, _MM_SHUFFLE(1, 1, 1, 1)); \
    NativeVectorType c = _mm_shuffle_ps(prod, prod, _MM_SHUFFLE(2, 2, 2, 2)); \
    NativeVectorType d = _mm_shuffle_ps(prod, prod, _MM_SHUFFLE(3, 3, 3, 3)); \
    NativeVectorType sum1 = _mm_add_ps(prod, b);        \
    NativeVectorType sum2 = _mm_add_ps(c, d);           \
    NativeVectorType sum = _mm_add_ps(sum1, sum2);      \
    MINGL_VECTOR_SPLAT_I(r, sum, 0);                    \
} while(0)

#define MINGL_VECTOR_RECIPROCAL_SQRT(r, v) do {         \
    r = _mm_rsqrt_ps(v);                                \
} while(0)

#define MINGL_VECTOR_NORMALIZE(r, v) do {               \
    NativeVectorType sqr;                               \
    MINGL_VECTOR_DOT(sqr, v, v);                        \
    NativeVectorType recipsqrt;                         \
    MINGL_VECTOR_RECIPROCAL_SQRT(recipsqrt, sqr);       \
    r = _mm_mul_ps(v, recipsqrt);                       \
} while(0);

#define MINGL_VECTOR_ADD(r, v0, v1) do { r = _mm_add_ps(v0, v1); } while(0)

#define MINGL_VECTOR_SUBTRACT(r, v0, v1) do { r = _mm_sub_ps(v0, v1); } while(0)

#define MINGL_VECTOR_NEGATE(r, v) do {                  \
    NativeVectorType zero = _mm_setzero_ps();           \
    r = _mm_sub_ps(zero, v);                            \
} while(0)

// in 'normal' order compared to _MM_SHUFFLE
#define MINGL_SHUF(x,y,z,w) (((w) << 6) | ((z) << 4) | ((y) << 2) | (x))

#define MINGL_VECTOR_CROSS3(r, v0, v1) do {                             \
    const int yzx = MINGL_SHUF(1, 2, 0, 0);                             \
    const int zxy = MINGL_SHUF(2, 0, 1, 0);                             \
    NativeVectorType v0yzx = _mm_shuffle_ps(v0, v0, yzx);               \
    NativeVectorType v0zxy = _mm_shuffle_ps(v0, v0, zxy);               \
    NativeVectorType v1yzx = _mm_shuffle_ps(v1, v1, yzx);               \
    NativeVectorType v1zxy = _mm_shuffle_ps(v1, v1, zxy);               \
    r = _mm_sub_ps(_mm_mul_ps(v0yzx, v1zxy), _mm_mul_ps(v1yzx, v0zxy)); \
} while(0)

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

#define MINGL_VECTOR_MULT(r, v0, v1) do {               \
    ret = _mm_mul_ps(v0, v1);                           \
} while(0)

#define MINGL_VECTOR_MADD(r, v0, v1, v2) do {           \
    ret = _mm_add_ps(_mm_mul_ps(v0, v1), v2);           \
} while(0)

#endif
