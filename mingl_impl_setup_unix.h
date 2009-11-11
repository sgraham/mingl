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

enum MatrixModeE
{
    MODELVIEW = 0,
    PROJECTION = 1,
    TEXTURE = 2,
    NUM_MATRIX_MODES
};

enum ClientStateE
{
    VERTEX_ARRAY   = 0x001,
    COLOR_ARRAY    = 0x002,
    NORMAL_ARRAY   = 0x004,
    TEXCOORD_ARRAY = 0x004,
};

enum { MAX_MATRIX_STACK_DEPTH = 32 };

struct Mat44
{
    __m128 l1, l2, l3, l4;
} MINGL_POST_ALIGN(16);

union Vec4
{
    __m128 v;
    struct
    {
        float x, y, z, w;
    };
};

struct Texture
{
    GLuint* C;
    int Width;
    int Height;
    ~Texture()
    {
        delete[] C;
    }
};


}

#endif
