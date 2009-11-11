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

namespace mingl
{
template <class T> void swap(T& a, T& b)
{
    T c = a;
    a = b;
    b = c;
}
}

// todo; detect other platforms
// exactly one of these PLATFORM defines must be defined

#ifdef unix
    #define MINGL_PLATFORM_UNIX
#endif

#include "mingl_impl_setup_unix.h"
