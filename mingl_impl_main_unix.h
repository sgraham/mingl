#ifdef MINGL_PLATFORM_UNIX

namespace mingl
{

inline void MinGL::ActiveTexture(GLenum texture) { MINGL_ASSERT(0); }
inline void MinGL::AlphaFunc(GLenum func, GLclampf ref) { MINGL_ASSERT(0); }

inline void MinGL::BindTexture(GLenum target, GLuint texture)
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

inline void MinGL::BlendFunc(GLenum sfactor, GLenum dfactor) { MINGL_ASSERT(0); }

inline void MinGL::Clear(GLbitfield mask)
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

inline void MinGL::ClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
    ctx.ClearColor = floatColorToUint(red, green, blue, alpha);
}

inline void MinGL::ClearDepthf(GLclampf depth) { MINGL_ASSERT(0); }
inline void MinGL::ClearStencil(GLint s) { MINGL_ASSERT(0); }
inline void MinGL::ClientActiveTexture(GLenum texture) { MINGL_ASSERT(0); }

inline void MinGL::Color4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    ctx.Vert.Color.v = _mm_setr_ps(red, green, blue, alpha);
    ctx.Vert.ColorInt = floatColorToUint(ctx.Vert.Color);
}

inline void MinGL::ColorMask(bool red, bool green, bool blue, bool alpha) { MINGL_ASSERT(0); }
inline void MinGL::ColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer) { MINGL_ASSERT(0); }
inline void MinGL::CompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data) { MINGL_ASSERT(0); }
inline void MinGL::CompressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data) { MINGL_ASSERT(0); }
inline void MinGL::CopyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border) { MINGL_ASSERT(0); }
inline void MinGL::CopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height) { MINGL_ASSERT(0); }
inline void MinGL::CullFace(GLenum mode) { MINGL_ASSERT(0); }
inline void MinGL::DeleteTextures(GLsizei n, const GLuint *textures) { MINGL_ASSERT(0); }
inline void MinGL::DepthFunc(GLenum func) { MINGL_ASSERT(0); }
inline void MinGL::DepthMask(bool flag) { MINGL_ASSERT(0); }
inline void MinGL::DepthRangef(GLclampf zNear, GLclampf zFar) { MINGL_ASSERT(0); }
inline void MinGL::Disable(GLenum cap) { MINGL_ASSERT(0); }

inline void MinGL::DisableClientState(GLenum array)
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

inline void MinGL::DrawArrays(GLenum mode, GLint first, GLsizei count)
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

inline void MinGL::DrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices) { MINGL_ASSERT(0); }

inline void MinGL::enableOrDisable(GLenum cap, bool val)
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

inline void MinGL::Enable(GLenum cap)
{
    enableOrDisable(cap, true);
}

inline void MinGL::EnableClientState(GLenum array)
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

inline void MinGL::Finish(void) { MINGL_ASSERT(0); }
inline void MinGL::Flush(void) { MINGL_ASSERT(0); }
inline void MinGL::Fogf(GLenum pname, GLfloat param) { MINGL_ASSERT(0); }
inline void MinGL::Fogfv(GLenum pname, const GLfloat *params) { MINGL_ASSERT(0); }
inline void MinGL::FrontFace(GLenum mode) { MINGL_ASSERT(0); }
inline void MinGL::Frustumf(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar) { MINGL_ASSERT(0); }
inline void MinGL::GenTextures(GLsizei n, GLuint *textures)
{
    // todo; fugly
    static GLuint counter = 1;
    for (GLsizei i = 0; i < n; ++i)
        textures[i] = counter++;
}

inline GLenum MinGL::GetError()
{
    GLenum ret = ctx.Error;
    ctx.Error = 0;
    return ret;
}

inline void MinGL::GetIntegerv(GLenum pname, GLint *params) { MINGL_ASSERT(0); }

inline const GLubyte* MinGL::GetString(GLenum name)
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

inline void MinGL::Hint(GLenum target, GLenum mode) { MINGL_ASSERT(0); }
inline void MinGL::LightModelf(GLenum pname, GLfloat param) { MINGL_ASSERT(0); }
inline void MinGL::LightModelfv(GLenum pname, const GLfloat *params) { MINGL_ASSERT(0); }
inline void MinGL::Lightf(GLenum light, GLenum pname, GLfloat param) { MINGL_ASSERT(0); }
inline void MinGL::Lightfv(GLenum light, GLenum pname, const GLfloat *params) { MINGL_ASSERT(0); }
inline void MinGL::LineWidth(GLfloat width) { MINGL_ASSERT(0); }

inline void MinGL::LoadIdentity(void)
{
    Mat44& m = *ctx.CurMatrix[ctx.MatrixMode];
    m.l1 = _mm_setr_ps(1.f, 0.f, 0.f, 0.f);
    m.l2 = _mm_setr_ps(0.f, 1.f, 0.f, 0.f);
    m.l3 = _mm_setr_ps(0.f, 0.f, 1.f, 0.f);
    m.l4 = _mm_setr_ps(0.f, 0.f, 0.f, 1.f);
}

inline void MinGL::LoadMatrixf(const GLfloat *m) { MINGL_ASSERT(0); }
inline void MinGL::LogicOp(GLenum opcode) { MINGL_ASSERT(0); }
inline void MinGL::Materialf(GLenum face, GLenum pname, GLfloat param) { MINGL_ASSERT(0); }
inline void MinGL::Materialfv(GLenum face, GLenum pname, const GLfloat *params) { MINGL_ASSERT(0); }
inline void MinGL::MatrixMode(GLenum mode) { MINGL_ASSERT(0); }
inline void MinGL::MultMatrixf(const GLfloat *m) { MINGL_ASSERT(0); }
inline void MinGL::MultiTexCoord4f(GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q) { MINGL_ASSERT(0); }
inline void MinGL::Normal3f(GLfloat nx, GLfloat ny, GLfloat nz) { MINGL_ASSERT(0); }
inline void MinGL::NormalPointer(GLenum type, GLsizei stride, const GLvoid *pointer) { MINGL_ASSERT(0); }
inline void MinGL::Orthof(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar) { MINGL_ASSERT(0); }
inline void MinGL::PixelStorei(GLenum pname, GLint param) { MINGL_ASSERT(0); }
inline void MinGL::PointSize(GLfloat size) { MINGL_ASSERT(0); }
inline void MinGL::PolygonOffset(GLfloat factor, GLfloat units) { MINGL_ASSERT(0); }
inline void MinGL::PopMatrix(void) { MINGL_ASSERT(0); }
inline void MinGL::PushMatrix(void) { MINGL_ASSERT(0); }
inline void MinGL::ReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels) { MINGL_ASSERT(0); }
inline void MinGL::Rotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z) { MINGL_ASSERT(0); }
inline void MinGL::Scalef(GLfloat x, GLfloat y, GLfloat z) { MINGL_ASSERT(0); }
inline void MinGL::Scissor(GLint x, GLint y, GLsizei width, GLsizei height) { MINGL_ASSERT(0); }
inline void MinGL::ShadeModel(GLenum mode) { MINGL_ASSERT(0); }
inline void MinGL::StencilFunc(GLenum func, GLint ref, GLuint mask) { MINGL_ASSERT(0); }
inline void MinGL::StencilMask(GLuint mask) { MINGL_ASSERT(0); }
inline void MinGL::StencilOp(GLenum fail, GLenum zfail, GLenum zpass) { MINGL_ASSERT(0); }

inline void MinGL::TexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
    ctx.TexCoordArray.Data = reinterpret_cast<const float*>(pointer);
    ctx.TexCoordArray.Size = size;
    ctx.TexCoordArray.Stride = stride;
    if (size < 2 || size > 4) MINGL_ERR(GL_INVALID_VALUE);
    if (type != GL_FLOAT) MINGL_ERR(GL_INVALID_ENUM);
    if (stride < 0) MINGL_ERR(GL_INVALID_VALUE);
}

inline void MinGL::TexEnvf(GLenum target, GLenum pname, GLfloat param) { MINGL_ASSERT(0); }
inline void MinGL::TexEnvfv(GLenum target, GLenum pname, const GLfloat *params) { MINGL_ASSERT(0); }
inline void MinGL::TexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels)
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

inline void MinGL::TexParameterf(GLenum target, GLenum pname, GLfloat param) { MINGL_ASSERT(0); }
inline void MinGL::TexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels) { MINGL_ASSERT(0); }
inline void MinGL::Translatef(GLfloat x, GLfloat y, GLfloat z) { MINGL_ASSERT(0); }

inline void MinGL::VertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
    ctx.VertexArray.Data = reinterpret_cast<const float*>(pointer);
    ctx.VertexArray.Size = size;
    ctx.VertexArray.Stride = stride;
    if (size < 2 || size > 4) MINGL_ERR(GL_INVALID_VALUE);
    if (type != GL_FLOAT) MINGL_ERR(GL_INVALID_ENUM);
    if (stride < 0) MINGL_ERR(GL_INVALID_VALUE);
}

inline void MinGL::Viewport(GLint x, GLint y, GLsizei width, GLsizei height) { MINGL_ASSERT(0); }


inline MinGL::MinGL(const char* title, int width, int height)
    : mIsOpen(false)
    , mIsShuttingDown(false)
    , mWidth(width)
    , mHeight(height)
    , mListener(0)
{
    initializeKeyMaps();


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
    ::XSelectInput(mDisplay, mWindow, EventMask);

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

inline MinGL::~MinGL()
{
    Close();
}

inline void MinGL::Close()
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

inline void MinGL::SwapBuffers()
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

inline bool MinGL::IsOpen() const
{
    return mIsOpen;
}

inline void* MinGL::operator new(size_t size)
{
    const int alignSize = 16;
    void* p = MINGL_ALLOCATE_MEMORY(size + alignSize - 1);
    uintptr_t addr = reinterpret_cast<uintptr_t>(p);
    addr += (alignSize - 1);
    addr &= ~(alignSize - 1);
    return reinterpret_cast<void*>(addr);
}

inline void MinGL::operator delete(void* p)
{
    MINGL_FREE_MEMORY(p);
}

}

#endif
