// Common configuration and include platform implementation

#ifndef MINGL_ASSERT
    #include <cassert>
    #define MINGL_ASSERT assert
#endif

// can be customized to assert, etc.
#ifndef MINGL_ERR
    //#define MINGL_ERR(e) do { ctx.Error = e; return; } while(0)
    //#define MINGL_ERR_WRET(e,r) do { ctx.Error = e; return r; } while(0)
    #define MINGL_ERR(e) do { MINGL_ASSERT(false && #e); return; } while(0)
    #define MINGL_ERR_WRET(e,r) do { MINGL_ASSERT(false && #e); return r; } while(0)
#endif

// todo; detect other platforms
// exactly one of these PLATFORM defines must be defined

#ifdef unix
    #define MINGL_PLATFORM_UNIX
#endif

#include "mingl_impl_setup_unix.h"


namespace mingl
{

template <class T> void swap(T& a, T& b)
{
    T c = a;
    a = b;
    b = c;
}

template <class T> T clamp(T toclamp, T lower = 0, T upper = 1)
{
    if (toclamp < lower) toclamp = lower;
    if (toclamp > upper) toclamp = upper;
    return toclamp;
}

enum MatrixModeE
{
    MM_ModelView = 0,
    MM_Projection = 1,
    MM_Texture = 2,
    MM_NumMatrixModes,
};

enum TextureUnitsE
{
    TU_TexUnit0 = 0,
    TU_TexUnit1 = 1,
    TU_NumTextureUnits,
};

enum CompareFuncE
{
    CF_Never,
    CF_Less,
    CF_Equal,
    CF_LessEqual,
    CF_Greater,
    CF_NotEqual,
    CF_GreaterEqual,
    CF_Always,
};

enum { MaxMatrixStackDepth = 16 };

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

#include "mingl_impl_vec.h"
#include "mingl_impl_mat.h"

}
