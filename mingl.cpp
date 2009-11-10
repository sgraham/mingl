// See mingl.h for copyright and documentation.

#include "mingl.h"

// todo; remove all these
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <map>

#ifndef MINGL_ASSERT
    #include <cassert>
    #define MINGL_ASSERT assert
#endif

#ifdef unix
    #define MINGL_PLATFORM_UNIX
#endif

#ifdef MINGL_PLATFORM_UNIX
    #include <X11/Xlib.h>
    #include <X11/Xutil.h>
    #include <X11/keysymdef.h>
    #include <xmmintrin.h>
#endif

namespace mingl
{
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
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
};

// can be customized to assert, etc.
#ifndef MINGL_ERR
    //#define MINGL_ERR(e) do { ctx.Error = e; return; } while(0)
    //#define MINGL_ERR_WRET(e,r) do { ctx.Error = e; return r; } while(0)
    #define MINGL_ERR(e) do { MINGL_ASSERT(false && #e); return; } while(0)
    #define MINGL_ERR_WRET(e,r) do { MINGL_ASSERT(false && #e); return r; } while(0)
#endif

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

struct Context
{
    Context()
        : Execing(true)
        , Printing(true)
    {
        for (int i = 0; i < NUM_MATRIX_MODES; ++i)
            CurMatrix[i] = &MatrixStack[i][0];
    }

    bool Execing;
    bool Printing;

    MatrixModeE MatrixMode;
    Mat44 MatrixStack[NUM_MATRIX_MODES][MAX_MATRIX_STACK_DEPTH];
    Mat44* CurMatrix[NUM_MATRIX_MODES];

    float ClearDepth;
    GLuint ClearColor;

    GLenum Error;

    // buffer, filled out by Display
    struct Buffer
    {
        float* Z;
        GLuint* C;
        int Width;
        int Height;
        int Stride;
    };
    Buffer Buf;

    struct VertexState
    {
        Vec4 Color;
        GLuint ColorInt;
        Vec4 Normal;
        Vec4 TexCoord;
    };
    VertexState Vert;

    int ClientState;
    struct ArrayState
    {
        const float* Data;
        int Size;
        int Stride;
    };
    ArrayState VertexArray;
    ArrayState ColorArray;
    ArrayState TexCoordArray;

    bool Texture2DEnabled;
    Texture* CurrentTexture;
    std::map<GLuint, Texture*> AllTextures;
};
static Context ctx;

template <class T> void swap(T& a, T& b)
{
    T c = a;
    a = b;
    b = c;
}

struct TriVert
{
    Vec4 pos;
    Vec4 tex;
    Vec4 clr;
    unsigned char debug;
};

class Gradients
{
    public:
        Gradients(const TriVert& v1, const TriVert& v2, const TriVert& v3)
        {
            const float dx23 = v2.pos.x - v3.pos.x;
            const float dy13 = v1.pos.y - v3.pos.y;
            const float dx13 = v1.pos.x - v3.pos.x;
            const float dy23 = v2.pos.y - v3.pos.y;
            const float oodX = 1.f / ((dx23 * dy13) - (dx13 * dy23));

            float oodY = -oodX;

            OOZ1 = 1.f / v1.pos.z;
            OOZ2 = 1.f / v2.pos.z;
            OOZ3 = 1.f / v3.pos.z;
            UOZ1 = v1.tex.x * OOZ1;
            UOZ2 = v2.tex.x * OOZ2;
            UOZ3 = v3.tex.x * OOZ3;
            VOZ1 = v1.tex.y * OOZ1;
            VOZ2 = v2.tex.y * OOZ2;
            VOZ3 = v3.tex.y * OOZ3;

            const float dooz13 = OOZ1 - OOZ3;
            const float dooz23 = OOZ2 - OOZ3;
            const float duoz13 = UOZ1 - UOZ3;
            const float duoz23 = UOZ2 - UOZ3;
            const float dvoz13 = VOZ1 - VOZ3;
            const float dvoz23 = VOZ2 - VOZ3;

            dOOZdX = oodX * ((dooz23 * dy13) - (dooz13 * dy23));
            dOOZdY = oodY * ((dooz23 * dx13) - (dooz13 * dx23));

            dUOZdX = oodX * ((duoz23 * dy13) - (duoz13 * dy23));
            dUOZdY = oodY * ((duoz23 * dx13) - (duoz13 * dx23));

            dVOZdX = oodX * ((dvoz23 * dy13) - (dvoz13 * dy23));
            dVOZdY = oodY * ((dvoz23 * dx13) - (dvoz13 * dx23));
        }

        float OOZ1, OOZ2, OOZ3;
        float UOZ1, UOZ2, UOZ3;
        float VOZ1, VOZ2, VOZ3;
        float dOOZdX, dOOZdY;
        float dUOZdX, dUOZdY;
        float dVOZdX, dVOZdY;
};
class Edge
{
    public:
        Edge(const Gradients& grads, const TriVert& v1, const TriVert& v2, float topOOZ, float topUOZ, float topVOZ)
        {
            Y = ceil(v1.pos.y);
            int yend = ceil(v2.pos.y);
            Height = yend - Y;

            const float yprestep = Y - v1.pos.y;
            const float realWidth = v2.pos.x - v1.pos.x;
            const float realHeight = v2.pos.y - v1.pos.y;
            printf("realwidth/height: %f, %f\n", realWidth, realHeight);

            X = ceil((realWidth * yprestep) / realHeight + v1.pos.x);
            XStep = realWidth / realHeight;
            printf("x, xstep: %f, %f\n", X, XStep);

            float xprestep = X - v1.pos.x;

            OOZ = topOOZ +
                yprestep * grads.dOOZdY +
                xprestep * grads.dOOZdX;
            OOZStep = XStep * grads.dOOZdX +
                grads.dOOZdY;

            UOZ = topUOZ +
                yprestep * grads.dUOZdY +
                xprestep * grads.dUOZdX;
            UOZStep = XStep * grads.dUOZdX + grads.dUOZdY;

            VOZ = topVOZ +
                yprestep * grads.dVOZdY +
                xprestep * grads.dVOZdX;
            VOZStep = XStep * grads.dVOZdX + grads.dVOZdY;
        }

        void Step()
        {
            X += XStep;
            Y += 1;
            Height -= 1;
            OOZ += OOZStep;
            UOZ += UOZStep;
            VOZ += VOZStep;
        }

        float X, XStep;
        int Y, Height;
        float OOZ, OOZStep;
        float UOZ, UOZStep;
        float VOZ, VOZStep;
};
int pixelCount;
bool firstScanLine;
static void drawScanLine(Context::Buffer& buf, const Gradients& grads, const Edge* left, const Edge* right, const Texture* texture)
{
    int xstart = ceil(left->X);
    int xprestep = xstart - left->X;

    GLuint* dest = buf.C + left->Y * buf.Stride + xstart;
    const GLuint* texdata = texture->C;
    const int texWidth = texture->Width;
    const int texStride = texWidth;
    //const int texHeight = texture->Height;
    int width = ceil(right->X) - xstart;

    float ooz = left->OOZ + xprestep * grads.dOOZdX;
    float uoz = left->UOZ + xprestep * grads.dUOZdX;
    float voz = left->VOZ + xprestep * grads.dVOZdX;

    while (width-- > 0)
    {
        float z = 1 / ooz;
        float fu = (uoz * z + 0.5f);
        float fv = (voz * z + 0.5f);
        int u = (int)fu;// * texWidth;
        int v = (int)fv;// * texHeight;
        //printf("u, v: %d %d\n", u, v);
        *dest++ += *(texdata + u + (v * texStride));
        pixelCount++;

        ooz += grads.dOOZdX;
        uoz += grads.dUOZdX;
        voz += grads.dVOZdX;
        if (firstScanLine && width == 0)
        {
            printf("end of first = %d %d (%f %f) ending %f %f\n", u, v, fu, fv, uoz*z+0.5f, voz*z*0.5f);
        }
    }
    firstScanLine = false;
}

static void renderTriangle(const TriVert* V1, const TriVert* V2, const TriVert* V3)
{
    pixelCount = 0;
    // v1 top, v2 middle, v3 bottom
    if (V1->pos.y > V3->pos.y) swap(V1, V3);
    if (V2->pos.y > V3->pos.y) swap(V2, V3);
    if (V1->pos.y > V2->pos.y) swap(V1, V2);

    TriVert v1 = *V1;
    TriVert v2 = *V2;
    TriVert v3 = *V3;
/*    printf("input pos: (%f,%f) (%f,%f) (%f,%f)\n",
            v1.pos.x, v1.pos.y,
            v2.pos.x, v2.pos.y,
            v3.pos.x, v3.pos.y);
    printf("input tex: (%f,%f) (%f,%f) (%f,%f)\n",
            v1.tex.x, v1.tex.y,
            v2.tex.x, v2.tex.y,
            v3.tex.x, v3.tex.y);
            */

    /*v1.pos.x -= 0.5f;
    v2.pos.x -= 0.5f;
    v3.pos.x -= 0.5f;
    v1.pos.y -= 0.5f;
    v2.pos.y -= 0.5f;
    v3.pos.y -= 0.5f;*/

    Gradients grads(v1, v2, v3);
    Edge edge12(grads, v1, v2, grads.OOZ1, grads.UOZ1, grads.VOZ1);
    Edge edge13(grads, v1, v3, grads.OOZ1, grads.UOZ1, grads.VOZ1);
    Edge edge23(grads, v2, v3, grads.OOZ2, grads.UOZ2, grads.VOZ2);

    // figure out where v2.x is on long edge
    const float xOnLong = (((v2.pos.y - v1.pos.y) * (v3.pos.x - v1.pos.x)) / (v3.pos.y - v1.pos.y)) + v1.pos.x;
    //printf("xOnLong: %f\n", xOnLong);

    //printf("scanning in %c %c %c\n", v1.debug, v2.debug, v3.debug);
    const Edge* left1;
    const Edge* right1;
    const Edge* left2;
    const Edge* right2;
    if (v2.pos.x > xOnLong)
    {
        left1 = left2 = &edge13;
        right1 = &edge12;
        right2 = &edge23;
        printf("here\n");
    }
    else
    {
        printf("else\n");
        left1 = &edge12;
        left2 = &edge23;
        right1 = right2 = &edge13;
    }
    firstScanLine = true;
    int height = edge12.Height;
    while (height)
    {
        drawScanLine(ctx.Buf, grads, left1, right1, ctx.CurrentTexture);
        edge12.Step(); edge13.Step();
        height -= 1;
    }

    height = edge23.Height;
    while (height)
    {
        drawScanLine(ctx.Buf, grads, left2, right2, ctx.CurrentTexture);
        edge23.Step(); edge13.Step();
        height -= 1;
    }
    printf("pixels: %d\n", pixelCount);
}

static GLuint floatColorToUint(float r, float g, float b, float a)
{
    // todo; *256 + biasing and clamping
    GLuint ri = (GLuint)(r * 255.f);
    GLuint gi = (GLuint)(g * 255.f);
    GLuint bi = (GLuint)(b * 255.f);
    GLuint ai = (GLuint)(a * 255.f);
    return (bi << 24) | (gi << 16) | (ri << 8) | ai;
}
static GLuint floatColorToUint(Vec4 v) { return floatColorToUint(v.x, v.y, v.z, v.w); }

void glActiveTexture(GLenum texture) { MINGL_ASSERT(0); }
void glAlphaFunc(GLenum func, GLclampf ref) { MINGL_ASSERT(0); }

void glBindTexture(GLenum target, GLuint texture)
{
    if (target != GL_TEXTURE_2D) MINGL_ERR(GL_INVALID_ENUM);
    std::map<GLuint, Texture*>::const_iterator i = ctx.AllTextures.find(texture);
    if (i == ctx.AllTextures.end())
    {
        ctx.AllTextures[texture] = ctx.CurrentTexture = new Texture();
    }
    else
    {
        ctx.CurrentTexture = i->second;
    }
}

void glBlendFunc(GLenum sfactor, GLenum dfactor) { MINGL_ASSERT(0); }

void glClear(GLbitfield mask)
{
    int dim = ctx.Buf.Width * ctx.Buf.Height;
    if ((mask & GL_DEPTH_BUFFER_BIT) != 0)
    {
        for (int i = 0; i < dim; ++i)
            ctx.Buf.Z[i] = ctx.ClearDepth;
    }
    if ((mask & GL_COLOR_BUFFER_BIT) != 0)
    {
        for (int i = 0; i < dim; ++i)
            ctx.Buf.C[i] = ctx.ClearColor;
    }
}

void glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
    ctx.ClearColor = floatColorToUint(red, green, blue, alpha);
}

void glClearDepthf(GLclampf depth) { MINGL_ASSERT(0); }
void glClearStencil(GLint s) { MINGL_ASSERT(0); }
void glClientActiveTexture(GLenum texture) { MINGL_ASSERT(0); }

void glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    ctx.Vert.Color.v = _mm_setr_ps(red, green, blue, alpha);
    ctx.Vert.ColorInt = floatColorToUint(ctx.Vert.Color);
}

void glColorMask(bool red, bool green, bool blue, bool alpha) { MINGL_ASSERT(0); }
void glColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer) { MINGL_ASSERT(0); }
void glCompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data) { MINGL_ASSERT(0); }
void glCompressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data) { MINGL_ASSERT(0); }
void glCopyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border) { MINGL_ASSERT(0); }
void glCopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height) { MINGL_ASSERT(0); }
void glCullFace(GLenum mode) { MINGL_ASSERT(0); }
void glDeleteTextures(GLsizei n, const GLuint *textures) { MINGL_ASSERT(0); }
void glDepthFunc(GLenum func) { MINGL_ASSERT(0); }
void glDepthMask(bool flag) { MINGL_ASSERT(0); }
void glDepthRangef(GLclampf zNear, GLclampf zFar) { MINGL_ASSERT(0); }
void glDisable(GLenum cap) { MINGL_ASSERT(0); }

void glDisableClientState(GLenum array)
{
    switch(array)
    {
        case GL_VERTEX_ARRAY: ctx.ClientState &= ~VERTEX_ARRAY; break;
        case GL_NORMAL_ARRAY: ctx.ClientState &= ~NORMAL_ARRAY; break;
        case GL_COLOR_ARRAY: ctx.ClientState &= ~COLOR_ARRAY; break;
        case GL_TEXTURE_COORD_ARRAY: ctx.ClientState &= ~TEXCOORD_ARRAY; break;
        default: MINGL_ERR(GL_INVALID_ENUM);
    }
}

void glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
    TriVert a, b, c;
    if (mode == GL_TRIANGLES)
    {
        const GLint vstep = (ctx.VertexArray.Size + ctx.VertexArray.Stride);
        const GLint tstep = (ctx.TexCoordArray.Size + ctx.TexCoordArray.Stride);
        const GLint vlast = first + count;
        GLint vi = first;
        GLint ti = first;
        while (vi < vlast)
        {
            a.pos.v = _mm_loadu_ps(&ctx.VertexArray.Data[vi]); vi += vstep;
            b.pos.v = _mm_loadu_ps(&ctx.VertexArray.Data[vi]); vi += vstep;
            c.pos.v = _mm_loadu_ps(&ctx.VertexArray.Data[vi]); vi += vstep; // todo; this is going to load past end on last one.
            if (ctx.Texture2DEnabled)
            {
                a.tex.v = _mm_loadu_ps(&ctx.TexCoordArray.Data[ti]); ti += tstep;
                b.tex.v = _mm_loadu_ps(&ctx.TexCoordArray.Data[ti]); ti += tstep;
                c.tex.v = _mm_loadu_ps(&ctx.TexCoordArray.Data[ti]); ti += tstep; // todo; this is going to load past end on last one.
            }
            // todo; this is silly
            //if (ctx.VertexArray.Size == 2)
            {
                a.pos.z = -100.f;
                b.pos.z = -100.f;
                c.pos.z = -100.f;
            }
            a.debug = 'a';
            b.debug = 'b';
            c.debug = 'c';
            renderTriangle(&a, &b, &c);
        }
    }
    else if (mode == GL_TRIANGLE_STRIP)
    {
        const GLint vstep = (ctx.VertexArray.Size + ctx.VertexArray.Stride);
        const GLint tstep = (ctx.TexCoordArray.Size + ctx.TexCoordArray.Stride);
        const GLint vlast = first + count;
        GLint vi = first;
        GLint ti = first;
        TriVert a, b, c;
        bool odd = true;
        while (vi < vlast)
        {
            a.pos.v = _mm_loadu_ps(&ctx.VertexArray.Data[vi]); vi += vstep;
            b.pos.v = _mm_loadu_ps(&ctx.VertexArray.Data[vi]);
            c.pos.v = _mm_loadu_ps(&ctx.VertexArray.Data[vi + vstep]); // todo; this is going to load past end on last one.
            if (ctx.Texture2DEnabled)
            {
                a.tex.v = _mm_loadu_ps(&ctx.TexCoordArray.Data[ti]); ti += tstep;
                b.tex.v = _mm_loadu_ps(&ctx.TexCoordArray.Data[ti]);
                c.tex.v = _mm_loadu_ps(&ctx.TexCoordArray.Data[ti + tstep]); // todo; this is going to load past end on last one.
            }
            // todo; this is silly
            //if (ctx.VertexArray.Size == 2)
            {
                a.pos.z = -100.f;
                b.pos.z = -100.f;
                c.pos.z = -100.f;
            }
            if (odd)
                renderTriangle(&a, &b, &c);
            else
                renderTriangle(&a, &c, &b);
            odd = !odd;
        }
    }
    else
    {
        MINGL_ERR(GL_INVALID_ENUM);
    }
}

void glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices) { MINGL_ASSERT(0); }

static void enableOrDisable(GLenum cap, bool val)
{
    switch (cap)
    {
        case GL_TEXTURE_2D:
            ctx.Texture2DEnabled = val;
            break;
        default:
            MINGL_ASSERT(0);
    }
}

void glEnable(GLenum cap)
{
    enableOrDisable(cap, true);
}

void glEnableClientState(GLenum array)
{
    switch(array)
    {
        case GL_VERTEX_ARRAY: ctx.ClientState |= VERTEX_ARRAY; break;
        case GL_NORMAL_ARRAY: ctx.ClientState |= NORMAL_ARRAY; break;
        case GL_COLOR_ARRAY: ctx.ClientState |= COLOR_ARRAY; break;
        case GL_TEXTURE_COORD_ARRAY: ctx.ClientState |= TEXCOORD_ARRAY; break;
        default: MINGL_ERR(GL_INVALID_ENUM);
    }
}

void glFinish(void) { MINGL_ASSERT(0); }
void glFlush(void) { MINGL_ASSERT(0); }
void glFogf(GLenum pname, GLfloat param) { MINGL_ASSERT(0); }
void glFogfv(GLenum pname, const GLfloat *params) { MINGL_ASSERT(0); }
void glFrontFace(GLenum mode) { MINGL_ASSERT(0); }
void glFrustumf(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar) { MINGL_ASSERT(0); }
void glGenTextures(GLsizei n, GLuint *textures)
{
    static GLuint counter = 1;
    for (GLsizei i = 0; i < n; ++i)
        textures[i] = counter++;
}

GLenum glGetError()
{
    GLenum ret = ctx.Error;
    ctx.Error = 0;
    return ret;
}

void glGetIntegerv(GLenum pname, GLint *params) { MINGL_ASSERT(0); }

const GLubyte* glGetString(GLenum name)
{
    switch (name)
    {
        case GL_VENDOR: return reinterpret_cast<const GLubyte*>("Scott Graham");
        case GL_RENDERER: return reinterpret_cast<const GLubyte*>("mingl");
        case GL_VERSION: return reinterpret_cast<const GLubyte*>("OpenGL ES-Cm 1.0");
        case GL_EXTENSIONS: return reinterpret_cast<const GLubyte*>("");
        default: MINGL_ERR_WRET(GL_INVALID_ENUM, 0);
    }
}

void glHint(GLenum target, GLenum mode) { MINGL_ASSERT(0); }
void glLightModelf(GLenum pname, GLfloat param) { MINGL_ASSERT(0); }
void glLightModelfv(GLenum pname, const GLfloat *params) { MINGL_ASSERT(0); }
void glLightf(GLenum light, GLenum pname, GLfloat param) { MINGL_ASSERT(0); }
void glLightfv(GLenum light, GLenum pname, const GLfloat *params) { MINGL_ASSERT(0); }
void glLineWidth(GLfloat width) { MINGL_ASSERT(0); }

void glLoadIdentity(void)
{
    Mat44& m = *ctx.CurMatrix[ctx.MatrixMode];
    m.l1 = _mm_setr_ps(1.f, 0.f, 0.f, 0.f);
    m.l2 = _mm_setr_ps(0.f, 1.f, 0.f, 0.f);
    m.l3 = _mm_setr_ps(0.f, 0.f, 1.f, 0.f);
    m.l4 = _mm_setr_ps(0.f, 0.f, 0.f, 1.f);
}

void glLoadMatrixf(const GLfloat *m) { MINGL_ASSERT(0); }
void glLogicOp(GLenum opcode) { MINGL_ASSERT(0); }
void glMaterialf(GLenum face, GLenum pname, GLfloat param) { MINGL_ASSERT(0); }
void glMaterialfv(GLenum face, GLenum pname, const GLfloat *params) { MINGL_ASSERT(0); }
void glMatrixMode(GLenum mode) { MINGL_ASSERT(0); }
void glMultMatrixf(const GLfloat *m) { MINGL_ASSERT(0); }
void glMultiTexCoord4f(GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q) { MINGL_ASSERT(0); }
void glNormal3f(GLfloat nx, GLfloat ny, GLfloat nz) { MINGL_ASSERT(0); }
void glNormalPointer(GLenum type, GLsizei stride, const GLvoid *pointer) { MINGL_ASSERT(0); }
void glOrthof(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar) { MINGL_ASSERT(0); }
void glPixelStorei(GLenum pname, GLint param) { MINGL_ASSERT(0); }
void glPointSize(GLfloat size) { MINGL_ASSERT(0); }
void glPolygonOffset(GLfloat factor, GLfloat units) { MINGL_ASSERT(0); }
void glPopMatrix(void) { MINGL_ASSERT(0); }
void glPushMatrix(void) { MINGL_ASSERT(0); }
void glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels) { MINGL_ASSERT(0); }
void glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z) { MINGL_ASSERT(0); }
void glScalef(GLfloat x, GLfloat y, GLfloat z) { MINGL_ASSERT(0); }
void glScissor(GLint x, GLint y, GLsizei width, GLsizei height) { MINGL_ASSERT(0); }
void glShadeModel(GLenum mode) { MINGL_ASSERT(0); }
void glStencilFunc(GLenum func, GLint ref, GLuint mask) { MINGL_ASSERT(0); }
void glStencilMask(GLuint mask) { MINGL_ASSERT(0); }
void glStencilOp(GLenum fail, GLenum zfail, GLenum zpass) { MINGL_ASSERT(0); }

void glTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
    ctx.TexCoordArray.Data = reinterpret_cast<const float*>(pointer);
    ctx.TexCoordArray.Size = size;
    ctx.TexCoordArray.Stride = stride;
    if (size < 2 || size > 4) MINGL_ERR(GL_INVALID_VALUE);
    if (type != GL_FLOAT) MINGL_ERR(GL_INVALID_ENUM);
    if (stride < 0) MINGL_ERR(GL_INVALID_VALUE);
}

void glTexEnvf(GLenum target, GLenum pname, GLfloat param) { MINGL_ASSERT(0); }
void glTexEnvfv(GLenum target, GLenum pname, const GLfloat *params) { MINGL_ASSERT(0); }
void glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels)
{
    if (target != GL_TEXTURE_2D) MINGL_ERR(GL_INVALID_ENUM);
    if (format != GL_RGBA) MINGL_ERR(GL_INVALID_ENUM);
    if (type != GL_UNSIGNED_BYTE) MINGL_ERR(GL_INVALID_ENUM);
    if (level < 0) MINGL_ERR(GL_INVALID_VALUE);
    if (internalformat != (GLint)format) MINGL_ERR(GL_INVALID_OPERATION);
    if (border != 0) MINGL_ERR(GL_INVALID_VALUE);

    if (level != 0) MINGL_ERR(GL_INVALID_VALUE); // todo; mipmaps

    if (ctx.CurrentTexture->C)
    {
        delete[] ctx.CurrentTexture->C;
    }
    ctx.CurrentTexture->C = new GLuint[width * height];
    const unsigned char* input = reinterpret_cast<const unsigned char*>(pixels);
    for (int i = 0; i < width * height; ++i)
    {
        int j = i * 4;
        // ARGB to RGBA
        // todo; not sure if this is caused by 1. endianness, 2. tex fmt, or 3. X11 buf format.
        ctx.CurrentTexture->C[i] = (input[j] << 8) | (input[j+1] << 16) | (input[j+2] << 24) | input[j+3];
    }
    ctx.CurrentTexture->Width = width;
    ctx.CurrentTexture->Height = height;
}
void glTexParameterf(GLenum target, GLenum pname, GLfloat param) { MINGL_ASSERT(0); }
void glTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels) { MINGL_ASSERT(0); }
void glTranslatef(GLfloat x, GLfloat y, GLfloat z) { MINGL_ASSERT(0); }

void glVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
    ctx.VertexArray.Data = reinterpret_cast<const float*>(pointer);
    ctx.VertexArray.Size = size;
    ctx.VertexArray.Stride = stride;
    if (size < 2 || size > 4) MINGL_ERR(GL_INVALID_VALUE);
    if (type != GL_FLOAT) MINGL_ERR(GL_INVALID_ENUM);
    if (stride < 0) MINGL_ERR(GL_INVALID_VALUE);
}

void glViewport(GLint x, GLint y, GLsizei width, GLsizei height) { MINGL_ASSERT(0); }


#ifdef MINGL_PLATFORM_UNIX

    class Display
    {
        public:
            Display(const char* title, int width, int height)
                : mIsOpen(false)
                , mIsShuttingDown(false)
                , mWidth(width)
                , mHeight(height)
            {
                mDisplay = ::XOpenDisplay(0);
                if (!mDisplay)
                {
                    Close();
                    return;
                }

                int screen = XDefaultScreen(mDisplay);
                ::Visual* visual = XDefaultVisual(mDisplay, screen);
                if (!visual)
                {
                    Close();
                    return;
                }

                Window root = XDefaultRootWindow(mDisplay);
                int screenWidth = XDisplayWidth(mDisplay, screen);
                int screenHeight = XDisplayHeight(mDisplay, screen);
                int left = (screenWidth - width) / 2;
                int top = (screenHeight - height) / 2;

                ::XSetWindowAttributes attributes;
                attributes.border_pixel = attributes.background_pixel = XBlackPixel(mDisplay, screen);
                attributes.backing_store = NotUseful;

                int displayDepth = DefaultDepth(mDisplay, screen);

                mWindow = ::XCreateWindow(mDisplay, root, left, top, width, height, 0,
                        displayDepth, InputOutput, visual,
                        CWBackPixel | CWBorderPixel | CWBackingStore, &attributes);

                ::XStoreName(mDisplay, mWindow, title);

                mWMProtocols = XInternAtom(mDisplay, "WM_PROTOCOLS", True);
                mWMDeleteWindow = XInternAtom(mDisplay, "WM_DELETE_WINDOW", True);
                if (mWMProtocols == 0 || mWMDeleteWindow == 0)
                {
                    Close();
                    return;
                }
                if (::XSetWMProtocols(mDisplay, mWindow, &mWMDeleteWindow, 1) == 0)
                {
                    Close();
                    return;
                }

                ::XSizeHints sizeHints;
                sizeHints.flags = PPosition | PMinSize | PMaxSize;
                sizeHints.x = sizeHints.y = 0;
                sizeHints.min_width = sizeHints.max_width = width;
                sizeHints.min_height = sizeHints.max_height = height;
                ::XSetNormalHints(mDisplay, mWindow, &sizeHints);
                ::XClearWindow(mDisplay, mWindow);

                // create (image) buffer

                /*
                buffer_.reset(width * height * bytesPerPixel);
                if (buffer_.isEmpty())
                {
                    close();
                    return false;
                }
                */

                int bufferDepth = 32;
                int bytesPerPixel = (bufferDepth + 7) / 8;
                int bitsPerPixel = 8 * bytesPerPixel;
                mGC = DefaultGC(mDisplay, screen);
                mImage = ::XCreateImage(mDisplay, CopyFromParent, displayDepth, ZPixmap, 0, 0,
                    width, height, bitsPerPixel, width * bytesPerPixel);
                #if defined(MINGL_LITTLE_ENDIAN)
                    mImage->byte_order = LSBFirst;
                #else
                    mImage->byte_order = MSBFirst;
                #endif  
                if (!mImage)
                {
                    Close();
                    return;
                }

                ::XMapRaised(mDisplay, mWindow);
                ::XFlush(mDisplay);

                // todo; configurable mem alloc
                ctx.Buf.Z = new float[width * height];
                ctx.Buf.C = new GLuint[width * height];
                ctx.Buf.Width = width;
                ctx.Buf.Stride = width;
                ctx.Buf.Height = height;

                mIsOpen = true;
            }

            ~Display()
            {
                Close();
            }

            void Close()
            {
                if (mImage)
                {
                    XDestroyImage(mImage);
                    mImage = 0;
                }

                if (mDisplay && mWindow)
                {
                    XDestroyWindow(mDisplay, mWindow);
                    mWindow = 0;
                }

                if (mDisplay)
                {
                    XCloseDisplay(mDisplay);
                    mDisplay = 0;
                }

                mIsOpen = false;
            }

            void SwapBuffers()
            {
                if (mIsShuttingDown)
                {
                    Close();
                    return;
                }

                if (!mDisplay || !mWindow || !mImage)
                    return;

                mImage->data = (char*)ctx.Buf.C;

                ::XPutImage(mDisplay, mWindow, mGC, mImage, 0, 0, 0, 0, mWidth, mHeight);
                ::XFlush(mDisplay);

                pumpEvents();
            }

            bool IsOpen()
            {
                return mIsOpen;
            }

        private:

            void pumpEvents()
            {
                ::XEvent event;
                while (true)
                {
                    if (::XCheckWindowEvent(mDisplay, mWindow, -1, &event))
                    {
                        handleEvent(event);
                    }
                    else if (::XCheckTypedEvent(mDisplay, ClientMessage, &event))
                    {
                        handleEvent(event);
                    }
                    else
                    {
                        break;
                    }
                }
            }

            void handleEvent(const ::XEvent& event)
            {
                switch (event.type)
                {
                    case ClientMessage:
                    {
                        if (event.xclient.message_type == mWMProtocols &&
                            event.xclient.format == 32 &&
                            event.xclient.data.l[0] == (long)mWMDeleteWindow)
                        {
                            mIsShuttingDown = true;
                        }
                        break;
                    }
                }
            }


            ::Display* mDisplay;
            ::Window mWindow;
            ::GC mGC;
            ::XImage* mImage;
            bool mIsOpen;
            bool mIsShuttingDown;
            Atom mWMProtocols;
            Atom mWMDeleteWindow;
            int mWidth;
            int mHeight;
    };

#endif




Display* DisplayOpen(const char* title, int width, int height)
{
    return new Display(title, width, height);
}

bool DisplayIsOpen(Display* display)
{
    return display->IsOpen();
}

void DisplayClose(Display* display)
{
    display->Close();
}

void DisplaySwapBuffers(Display* display)
{
    display->SwapBuffers();
}

}

