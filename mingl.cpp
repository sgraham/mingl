// See mingl.h for copyright and documentation.

#include "mingl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    #define MINGL_ERR(e) do { ctx.Error = e; return; } while(0)
    #define MINGL_ERR_WRET(e,r) do { ctx.Error = e; return r; } while(0)
#endif

typedef __m128 Vec4;

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

    Vec4 ClearColor;

    GLenum Error;

    // buffer, filled out by Display
    struct Buffer
    {
        GLuint* Z;
        GLuint* C;
        int Width;
        int Height;
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
};
static Context ctx;

void glActiveTexture(GLenum texture) { MINGL_ASSERT(0); }
void glAlphaFunc(GLenum func, GLclampf ref) { MINGL_ASSERT(0); }
void glBindTexture(GLenum target, GLuint texture) { MINGL_ASSERT(0); }
void glBlendFunc(GLenum sfactor, GLenum dfactor) { MINGL_ASSERT(0); }

void glClear(GLbitfield mask)
{
    int dim = sizeof(GLuint) * ctx.Buf.Width * ctx.Buf.Height;
    if ((mask & GL_DEPTH_BUFFER_BIT) != 0)
        memset(ctx.Buf.Z, 0, dim);
    if ((mask & GL_COLOR_BUFFER_BIT) != 0)
        memset(ctx.Buf.C, 0, dim);
}

void glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
    ctx.ClearColor = _mm_setr_ps(red, green, blue, alpha);
}

void glClearDepthf(GLclampf depth) { MINGL_ASSERT(0); }
void glClearStencil(GLint s) { MINGL_ASSERT(0); }
void glClientActiveTexture(GLenum texture) { MINGL_ASSERT(0); }

void glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    ctx.Vert.Color = _mm_setr_ps(red, green, blue, alpha);
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
    if (mode == GL_TRIANGLES)
    {
        return;
    }
    else
    {
        MINGL_ERR(GL_INVALID_ENUM);
    }
}

void glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices) { MINGL_ASSERT(0); }
void glEnable(GLenum cap) { MINGL_ASSERT(0); }

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
void glGenTextures(GLsizei n, GLuint *textures) { MINGL_ASSERT(0); }

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
void glTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer) { MINGL_ASSERT(0); }
void glTexEnvf(GLenum target, GLenum pname, GLfloat param) { MINGL_ASSERT(0); }
void glTexEnvfv(GLenum target, GLenum pname, const GLfloat *params) { MINGL_ASSERT(0); }
void glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels) { MINGL_ASSERT(0); }
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
    if (size < 0) MINGL_ERR(GL_INVALID_VALUE);
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
                ctx.Buf.Z = new GLuint[width * height];
                ctx.Buf.C = new GLuint[width * height];
                ctx.Buf.Width = width;
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

                char* testdata = (char*)malloc(1280*720*4);
                char j = 0;
                for (int i = 0; i < 1280*720*4; i += 4)
                {
                    testdata[i+1] = j;
                    testdata[i+2] = 0;
                    testdata[i+3] = 0;
                    j += 1;
                }

                mImage->data = testdata;

                ::XPutImage(mDisplay, mWindow, mGC, mImage, 0, 0, 0, 0, mWidth, mHeight);
                ::XFlush(mDisplay);

                mImage->data = NULL;
                free(testdata);

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

