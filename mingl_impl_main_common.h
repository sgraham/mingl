namespace mingl
{


// --------------------------------------------------------------------------
inline void MinGL::ActiveTexture(GLenum texture)
{
    if (texture > GL_TEXTURE0 + TU_NumTextureUnits) MINGL_ERR(GL_INVALID_ENUM);
    ctx.CurActiveTexture = texture;
}

// --------------------------------------------------------------------------
inline void MinGL::AlphaFunc(GLenum func, GLclampf ref)
{
    if (!compareFuncAssign(func, ctx.AlphaCompareFunc)) return;
    ctx.AlphaCompareValue = clamp(ref);
}

// --------------------------------------------------------------------------
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

// --------------------------------------------------------------------------
inline void MinGL::BlendFunc(GLenum sfactor, GLenum dfactor) { MINGL_ASSERT(0); }

// --------------------------------------------------------------------------
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

// --------------------------------------------------------------------------
inline void MinGL::ClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
    ctx.ClearColor = floatColorToUint(clamp(red), clamp(green), clamp(blue), clamp(alpha));
}

// --------------------------------------------------------------------------
inline void MinGL::ClearDepth(GLclampf depth) { ctx.ClearDepth = depth; }

// --------------------------------------------------------------------------
inline void MinGL::ClearStencil(GLint s) { ctx.ClearStencil = s; }

// --------------------------------------------------------------------------
inline void MinGL::ClientActiveTexture(GLenum texture)
{
    texture -= GL_TEXTURE0;
    if (texture > TU_NumTextureUnits) MINGL_ERR(GL_INVALID_ENUM);
    ctx.CurClientActiveTexture = texture;
}

// --------------------------------------------------------------------------
inline void MinGL::Color(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    ctx.Vert.color = Vec4(red, green, blue, alpha);
}

// --------------------------------------------------------------------------
inline void MinGL::ColorMask(bool red, bool green, bool blue, bool alpha) { MINGL_ASSERT(0); }

// --------------------------------------------------------------------------
inline void MinGL::ColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
    if (size != 4) MINGL_ERR(GL_INVALID_VALUE);
    if (stride < 0) MINGL_ERR(GL_INVALID_VALUE);
    if (type != GL_FLOAT) MINGL_ERR(GL_INVALID_ENUM);
    ctx.ColorArray.Data = reinterpret_cast<const float*>(pointer);
    ctx.ColorArray.Size = size;
    ctx.ColorArray.Stride = stride;
}

// --------------------------------------------------------------------------
inline void MinGL::CompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data) { MINGL_ASSERT(0); }
// --------------------------------------------------------------------------
inline void MinGL::CompressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data) { MINGL_ASSERT(0); }
// --------------------------------------------------------------------------
inline void MinGL::CopyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border) { MINGL_ASSERT(0); }
// --------------------------------------------------------------------------
inline void MinGL::CopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height) { MINGL_ASSERT(0); }
// --------------------------------------------------------------------------
inline void MinGL::CullFace(GLenum mode) { MINGL_ASSERT(0); }
// --------------------------------------------------------------------------
inline void MinGL::DeleteTextures(GLsizei n, const GLuint *textures) { MINGL_ASSERT(0); }

// --------------------------------------------------------------------------
inline void MinGL::DepthFunc(GLenum func)
{
    compareFuncAssign(func, ctx.DepthCompareFunc);
}

// --------------------------------------------------------------------------
inline void MinGL::DepthMask(bool flag) { MINGL_ASSERT(0); }

// --------------------------------------------------------------------------
inline void MinGL::DepthRange(GLclampf zNear, GLclampf zFar)
{
    zNear = clamp(zNear, 0.f, 1.f);
    zFar = clamp(zFar, 0.f, 1.f);
    ctx.Viewport.N = zNear;
    ctx.Viewport.F = zFar;
}

// --------------------------------------------------------------------------
inline void MinGL::Disable(GLenum cap) { MINGL_ASSERT(0); }

// --------------------------------------------------------------------------
inline void MinGL::DisableClientState(GLenum array)
{
    enableOrDisableCS(array, false);
}


// --------------------------------------------------------------------------
inline Vec4 MinGL::getVec4FromArrayPtr(const float* p, const int size)
{
    MINGL_ASSERT(size == 2 || size == 3 || size == 4);
    if (size == 2)
        return Vec4(p[0], p[1], 0.f, 1.f);
    else if (size == 3)
        return Vec4(p[0], p[1], p[2], 1.f);
    else if (size == 4)
        return Vec4(p[0], p[1], p[2], p[3]);
    return Vec4();
}

// --------------------------------------------------------------------------
inline void MinGL::advanceArrayPtr(const float*& p, const DisplayImplContext::ArrayState& as)
{
    p += as.Size;
    if (as.Stride != 0)
    {
        const unsigned char*& pAsChar = reinterpret_cast<const unsigned char*&>(p);
        pAsChar += as.Stride;
    }
}

// --------------------------------------------------------------------------
inline void MinGL::processVertTriangles()
{
    if (ctx.TriPrimGenState.Counter == 0)
    {
        ctx.TriPrimGenState.A = ctx.Vert;
        ctx.TriPrimGenState.Counter = 1;
    }
    else if (ctx.TriPrimGenState.Counter == 1)
    {
        ctx.TriPrimGenState.B = ctx.Vert;
        ctx.TriPrimGenState.Counter = 2;
    }
    else
    {
        MINGL_ASSERT(ctx.TriPrimGenState.Counter == 2);
        (this->*ctx.Funcs.Tri)(&ctx.TriPrimGenState.A, &ctx.TriPrimGenState.B, &ctx.Vert);
        ctx.TriPrimGenState.Counter = 0;
    }
}

// --------------------------------------------------------------------------
inline void MinGL::processVertTristripSetup()
{
    *ctx.TriPrimGenState.Next = ctx.Vert;
    ctx.TriPrimGenState.Next = (ctx.TriPrimGenState.Next == &ctx.TriPrimGenState.A)
        ? &ctx.TriPrimGenState.B
        : &ctx.TriPrimGenState.A;
}

// --------------------------------------------------------------------------
inline void MinGL::processVertTristrip()
{
    (this->*ctx.Funcs.Tri)(&ctx.TriPrimGenState.A, &ctx.TriPrimGenState.B, &ctx.Vert);
    *ctx.TriPrimGenState.Next = ctx.Vert;
    ctx.TriPrimGenState.Next = (ctx.TriPrimGenState.Next == &ctx.TriPrimGenState.A)
        ? &ctx.TriPrimGenState.B
        : &ctx.TriPrimGenState.A;
}

// --------------------------------------------------------------------------
inline void MinGL::transformCurrentVert()
{
    // todo; cache a combined version
    //pvec("object", ctx.Vert.pos);
    Vec4 eye = *ctx.CurMatrix[MM_ModelView] * ctx.Vert.pos;
    //pvec("eye", eye);
    Vec4 clip = *ctx.CurMatrix[MM_Projection] * eye;
    //pvec("clip", clip);
    Vec4 dev = Vec4(clip.X() / clip.W(),
                    clip.Y() / clip.W(),
                    clip.Z() / clip.W(),
                    0.f);
    //pvec("dev", dev);
    float f = ctx.Viewport.F;
    float n = ctx.Viewport.N;
    ctx.Vert.pos = Vec4((ctx.Viewport.Px/2) * dev.X() + ctx.Viewport.Ox,
                        -((ctx.Viewport.Py/2) * dev.Y() + ctx.Viewport.Oy) + ctx.Viewport.Py, // todo; we flip here rather than in rasterize. problematic?
                        ((f-n)/2.f) * dev.Z() + (n+f)/2.f,
                        0.f);
    //pvec("final", ctx.Vert.pos);
}

// --------------------------------------------------------------------------
inline void MinGL::DrawArrays(GLenum mode, GLint first, GLsizei count)
{
    if (first < 0) MINGL_ERR(GL_INVALID_VALUE);

    if (!ctx.VertexArray.Enabled)
    {
        // sort of silly, but there's nothing to do.
        return;
    }
    const int vs = ctx.VertexArray.Size;
    const float* vp = ctx.VertexArray.Data + first * vs;

    const float* np = ctx.NormalArray.Data + first * 3;
    const bool npe = ctx.NormalArray.Enabled;

    const float* cp = ctx.ColorArray.Data + first * 4;
    const bool cpe = ctx.ColorArray.Enabled;

    MINGL_ASSERT(TU_NumTextureUnits == 2);
    const int t0s = ctx.TexCoordArray[0].Size;
    const float* t0p = ctx.TexCoordArray[0].Data + first * t0s;
    const bool t0pe = ctx.TexCoordArray[0].Enabled;

    const int t1s = ctx.TexCoordArray[1].Size;
    const float* t1p = ctx.TexCoordArray[1].Data + first * t1s;
    const bool t1pe = ctx.TexCoordArray[1].Enabled;

    VertProcFn initialVertProc = 0;
    VertProcFn mainVertProc = 0;
    switch (mode)
    {
        case GL_TRIANGLES:
            mainVertProc = &MinGL::processVertTriangles;
            ctx.TriPrimGenState.Counter = 0;
            break;
        case GL_TRIANGLE_STRIP:
            initialVertProc = &MinGL::processVertTristripSetup;
            mainVertProc = &MinGL::processVertTristrip;
            ctx.TriPrimGenState.Next = &ctx.TriPrimGenState.A;
            break;
        default: MINGL_ERR(GL_INVALID_ENUM); // todo;
    }
    // if mode doesn't need any special handling (i.e. not tristrip/trifan)
    if (initialVertProc == 0)
        initialVertProc = mainVertProc;

#define MINGL_PROCESS_VERT_DRAWARRAYS(vertprocfunc)                         \
    {                                                                       \
        if (npe)                                                            \
        {                                                                   \
            ctx.Vert.normal = Vec4(np[0], np[1], np[2], 0.f);               \
            advanceArrayPtr(np, ctx.NormalArray);                           \
        }                                                                   \
        if (cpe)                                                            \
        {                                                                   \
            ctx.Vert.color = Vec4(cp);                                      \
            advanceArrayPtr(cp, ctx.ColorArray);                            \
        }                                                                   \
        if (ctx.Texture2DEnabled)                                           \
        {                                                                   \
            if (t0pe)                                                       \
            {                                                               \
                ctx.Vert.tex[0] = getVec4FromArrayPtr(t0p, t0s);            \
                advanceArrayPtr(t0p, ctx.TexCoordArray[0]);                 \
            }                                                               \
            if (t1pe)                                                       \
            {                                                               \
                ctx.Vert.tex[1] = getVec4FromArrayPtr(t1p, t1s);            \
                advanceArrayPtr(t1p, ctx.TexCoordArray[1]);                 \
            }                                                               \
        }                                                                   \
        ctx.Vert.pos = getVec4FromArrayPtr(vp, vs);                         \
        advanceArrayPtr(vp, ctx.VertexArray);                               \
        transformCurrentVert();                                             \
        (this->*vertprocfunc)();                                            \
    }

    GLint i;
    for (i = 0; i < count && i < 2; ++i)
    {
        MINGL_PROCESS_VERT_DRAWARRAYS(initialVertProc)
    }
    for (; i < count; ++i)
    {
        MINGL_PROCESS_VERT_DRAWARRAYS(mainVertProc)
    }

#undef MINGL_PROCESS_VERT_DRAWARRAYS
}

// --------------------------------------------------------------------------
inline void MinGL::DrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices) { MINGL_ASSERT(0); }

// --------------------------------------------------------------------------
inline void MinGL::Enable(GLenum cap)
{
    enableOrDisable(cap, true);
}

// --------------------------------------------------------------------------
inline void MinGL::EnableClientState(GLenum array)
{
    enableOrDisableCS(array, true);
}

// --------------------------------------------------------------------------
inline void MinGL::Finish()
{
    // nothing's deferred right now
}

// --------------------------------------------------------------------------
inline void MinGL::Flush()
{
    // nothing's deferred right now
}

// --------------------------------------------------------------------------
inline void MinGL::Fog(GLenum pname, GLfloat param) { MINGL_ASSERT(0); }
// --------------------------------------------------------------------------
inline void MinGL::Fog(GLenum pname, const GLfloat *params) { MINGL_ASSERT(0); }
// --------------------------------------------------------------------------
inline void MinGL::FrontFace(GLenum mode) { MINGL_ASSERT(0); }

// --------------------------------------------------------------------------
inline void MinGL::Frustum(GLfloat l, GLfloat r, GLfloat b, GLfloat t, GLfloat n, GLfloat f)
{
    if (n <= 0.f
            || f <= 0.f
            || l == r
            || b == t
            || n ==f)
        MINGL_ERR(GL_INVALID_VALUE);

    *ctx.CurMatrix[ctx.CurMatrixMode] *= Mat44(
            (2*n)/(r-l), 0.f, (r+l)/(r-l), 0.f,
            0.f, (2*n)/(t-b), (t+b)/(t-b), 0.f,
            0.f, 0.f, -((f+n)/(f-n)), -((2*f*n)/(f-n)),
            0.f, 0.f, -1.f, 0.f);
}

// --------------------------------------------------------------------------
inline void MinGL::GenTextures(GLsizei n, GLuint *textures)
{
    for (GLsizei i = 0; i < n; ++i)
        textures[i] = ctx.GenTexIdCounter++;
}

// --------------------------------------------------------------------------
inline GLenum MinGL::GetError()
{
    GLenum ret = ctx.Error;
    ctx.Error = 0;
    return ret;
}

// --------------------------------------------------------------------------
inline void MinGL::GetInteger(GLenum pname, GLint *params) { MINGL_ASSERT(0); }

// --------------------------------------------------------------------------
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

// --------------------------------------------------------------------------
inline void MinGL::Hint(GLenum target, GLenum mode) { MINGL_ASSERT(0); }
// --------------------------------------------------------------------------
inline void MinGL::LightModel(GLenum pname, GLfloat param) { MINGL_ASSERT(0); }
// --------------------------------------------------------------------------
inline void MinGL::LightModel(GLenum pname, const GLfloat *params) { MINGL_ASSERT(0); }
// --------------------------------------------------------------------------
inline void MinGL::Light(GLenum light, GLenum pname, GLfloat param) { MINGL_ASSERT(0); }
// --------------------------------------------------------------------------
inline void MinGL::Light(GLenum light, GLenum pname, const GLfloat *params) { MINGL_ASSERT(0); }
// --------------------------------------------------------------------------
inline void MinGL::LineWidth(GLfloat width) { MINGL_ASSERT(0); }

// --------------------------------------------------------------------------
inline void MinGL::LoadIdentity()
{
    *ctx.CurMatrix[ctx.CurMatrixMode] = Mat44(
            1.f, 0.f, 0.f, 0.f,
            0.f, 1.f, 0.f, 0.f,
            0.f, 0.f, 1.f, 0.f,
            0.f, 0.f, 0.f, 1.f);
}

// --------------------------------------------------------------------------
inline void MinGL::LoadMatrix(const GLfloat *m)
{
    *ctx.CurMatrix[ctx.CurMatrixMode] = Mat44(m);
}

// --------------------------------------------------------------------------
inline void MinGL::LogicOp(GLenum opcode) { MINGL_ASSERT(0); }
// --------------------------------------------------------------------------
inline void MinGL::Material(GLenum face, GLenum pname, GLfloat param) { MINGL_ASSERT(0); }
// --------------------------------------------------------------------------
inline void MinGL::Material(GLenum face, GLenum pname, const GLfloat *params) { MINGL_ASSERT(0); }

// --------------------------------------------------------------------------
inline void MinGL::MatrixMode(GLenum mode)
{
    switch (mode)
    {
        case GL_MODELVIEW: ctx.CurMatrixMode = MM_ModelView; break;
        case GL_PROJECTION: ctx.CurMatrixMode = MM_Projection; break;
        case GL_TEXTURE: ctx.CurMatrixMode = MM_Texture; break;
        default: MINGL_ERR(GL_INVALID_ENUM);
    }
}

// --------------------------------------------------------------------------
inline void MinGL::MultMatrix(const GLfloat *m)
{
    Mat44 mm(m);
    *ctx.CurMatrix[ctx.CurMatrixMode] *= mm;
}

// --------------------------------------------------------------------------
inline void MinGL::MultiTexCoord(GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
    target -= GL_TEXTURE0;
    if (target > TU_NumTextureUnits) MINGL_ERR(GL_INVALID_ENUM);
    ctx.Vert.tex[target] = Vec4(s, t, r, q);
}

// --------------------------------------------------------------------------
inline void MinGL::Normal(GLfloat nx, GLfloat ny, GLfloat nz)
{
    ctx.Vert.normal = Vec4(nx, ny, nz, 0.f);
}

// --------------------------------------------------------------------------
inline void MinGL::NormalPointer(GLenum type, GLsizei stride, const GLvoid *pointer) { MINGL_ASSERT(0); }

// --------------------------------------------------------------------------
inline void MinGL::Ortho(GLfloat l, GLfloat r, GLfloat b, GLfloat t, GLfloat n, GLfloat f)
{
    if (l == r
            || b == t
            || n ==f)
        MINGL_ERR(GL_INVALID_VALUE);

    *ctx.CurMatrix[ctx.CurMatrixMode] *= Mat44(
            2.f/(r-l), 0.f, 0.f, -((r+l)/(r-l)),
            0.f, 2.f/(t-b), 0.f, -((t+b)/(t-b)),
            0.f, 0.f, -(2.f/(f-n)), -((f+n)/(f-n)),
            0.f, 0.f, 0.f, 1.f);
}

// --------------------------------------------------------------------------
inline void MinGL::PixelStore(GLenum pname, GLint param) { MINGL_ASSERT(0); }
// --------------------------------------------------------------------------
inline void MinGL::PointSize(GLfloat size) { MINGL_ASSERT(0); }
// --------------------------------------------------------------------------
inline void MinGL::PolygonOffset(GLfloat factor, GLfloat units) { MINGL_ASSERT(0); }

// --------------------------------------------------------------------------
inline void MinGL::PopMatrix()
{
    if (ctx.CurMatrix[ctx.CurMatrixMode] == &ctx.MatrixStack[ctx.CurMatrixMode][0])
        MINGL_ERR(GL_STACK_UNDERFLOW);
    ctx.CurMatrix[ctx.CurMatrixMode] -= 1;
}

// --------------------------------------------------------------------------
inline void MinGL::PushMatrix()
{
    if (ctx.CurMatrix[ctx.CurMatrixMode] == &ctx.MatrixStack[ctx.CurMatrixMode][MaxMatrixStackDepth - 1])
        MINGL_ERR(GL_STACK_OVERFLOW);
    *(ctx.CurMatrix[ctx.CurMatrixMode] + 1) = *ctx.CurMatrix[ctx.CurMatrixMode];
    ctx.CurMatrix[ctx.CurMatrixMode] += 1;
}

// --------------------------------------------------------------------------
inline void MinGL::ReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels) { MINGL_ASSERT(0); }

// --------------------------------------------------------------------------
inline void MinGL::Rotate(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
    const float theta = angle * M_PI / 180.f;
    const float len2 = x*x + y*y + z*z;
    const float len = 1.f / sqrt(len2);
    x *= len; y *= len; z *= len;

    const float c = cos(theta);
    const float s = sin(theta);

    Mat44 tmp(
            x*x*(1-c)+c,   x*y*(1-c)-z*s, x*z*(1-c)+y*s, 0.f,
            x*y*(1-c)+z*s, y*y*(1-c)+c,   y*z*(1-c)-x*s, 0.f,
            x*z*(1-c)-y*s, y*z*(1-c)+x*s, z*z*(1-c)+c,   0.f,
            0.f,           0.f,           0.f,           1.f);
    *ctx.CurMatrix[ctx.CurMatrixMode] *= tmp;
}

// --------------------------------------------------------------------------
inline void MinGL::Scale(GLfloat x, GLfloat y, GLfloat z)
{
    *ctx.CurMatrix[ctx.CurMatrixMode] *= Mat44(
            x, 0.f, 0.f, 0.f,
            0.f, y, 0.f, 0.f,
            0.f, 0.f, z, 0.f,
            0.f, 0.f, 0., 1.f);
}

// --------------------------------------------------------------------------
inline void MinGL::Scissor(GLint x, GLint y, GLsizei width, GLsizei height) { MINGL_ASSERT(0); }
// --------------------------------------------------------------------------
inline void MinGL::ShadeModel(GLenum mode)
{
    // todo;
}

// --------------------------------------------------------------------------
inline void MinGL::StencilFunc(GLenum func, GLint ref, GLuint mask)
{
    if (!compareFuncAssign(func, ctx.StencilCompareFunc)) return;
    ctx.StencilCompareValue = ref;
    ctx.StencilCompareMask = mask;
}

// --------------------------------------------------------------------------
inline void MinGL::StencilMask(GLuint mask) { MINGL_ASSERT(0); }
// --------------------------------------------------------------------------
inline void MinGL::StencilOp(GLenum fail, GLenum zfail, GLenum zpass) { MINGL_ASSERT(0); }

// --------------------------------------------------------------------------
inline void MinGL::TexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
    ctx.TexCoordArray[0].Data = reinterpret_cast<const float*>(pointer);
    ctx.TexCoordArray[0].Size = size;
    ctx.TexCoordArray[0].Stride = stride;
    if (size < 2 || size > 4) MINGL_ERR(GL_INVALID_VALUE);
    if (type != GL_FLOAT) MINGL_ERR(GL_INVALID_ENUM);
    if (stride < 0) MINGL_ERR(GL_INVALID_VALUE);
}

// --------------------------------------------------------------------------
inline void MinGL::TexEnv(GLenum target, GLenum pname, GLfloat param) { MINGL_ASSERT(0); }
// --------------------------------------------------------------------------
inline void MinGL::TexEnv(GLenum target, GLenum pname, const GLfloat *params) { MINGL_ASSERT(0); }
// --------------------------------------------------------------------------
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

// --------------------------------------------------------------------------
inline void MinGL::TexParameter(GLenum target, GLenum pname, GLfloat param) { MINGL_ASSERT(0); }
// --------------------------------------------------------------------------
inline void MinGL::TexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels) { MINGL_ASSERT(0); }
// --------------------------------------------------------------------------
inline void MinGL::Translate(GLfloat x, GLfloat y, GLfloat z)
{
    *ctx.CurMatrix[ctx.CurMatrixMode] *= Mat44(
            1.f, 0.f, 0.f, x,
            0.f, 1.f, 0.f, y,
            0.f, 0.f, 1.f, z,
            0.f, 0.f, 0.f, 1.f);
}

// --------------------------------------------------------------------------
inline void MinGL::VertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
    ctx.VertexArray.Data = reinterpret_cast<const float*>(pointer);
    ctx.VertexArray.Size = size;
    ctx.VertexArray.Stride = stride;
    if (size < 2 || size > 4) MINGL_ERR(GL_INVALID_VALUE);
    if (type != GL_FLOAT) MINGL_ERR(GL_INVALID_ENUM);
    if (stride < 0) MINGL_ERR(GL_INVALID_VALUE);
}

// --------------------------------------------------------------------------
inline void MinGL::Viewport(GLint x, GLint y, GLsizei w, GLsizei h)
{
    if (w < 0 || h < 0) MINGL_ERR(GL_INVALID_VALUE);
    ctx.Viewport.Ox = x + w/2;
    ctx.Viewport.Oy = y + h/2;
    ctx.Viewport.Px = w;
    ctx.Viewport.Py = h;
}

// --------------------------------------------------------------------------
inline void MinGL::LookAt(GLfloat eyex, GLfloat eyey, GLfloat eyez, GLfloat centerx, GLfloat centery, GLfloat centerz, GLfloat upx, GLfloat upy, GLfloat upz)
{
    // http://www.opengl.org/sdk/docs/man/xhtml/gluLookAt.xml
    Vec4 eye(eyex, eyey, eyez, 0.f);
    Vec4 center(centerx, centery, centerz, 0.f);
    Vec4 up(upx, upy, upz, 0.f);

    Vec4 F = Normalize(center - eye);

    Vec4 S = CrossXYZ(F, Normalize(up));
    Vec4 U = CrossXYZ(S, F);

    *ctx.CurMatrix[ctx.CurMatrixMode] *= Mat44(S, U, -F, Vec4(0.f, 0.f, 0.f, 1.f));
    Translate(-eyex, -eyey, -eyez);
}

// --------------------------------------------------------------------------
inline void MinGL::Perspective(GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar)
{
    // http://www.opengl.org/sdk/docs/man/xhtml/gluPerpsective.xml
    const float dz = zNear - zFar;
    const float rads = (fovy / 2.f) * M_PI / 180.f;
    const float tangent = tan(rads);

    if (tangent == 0.f || aspect == 0.f || dz == 0.f)
        return;

    const float f = 1.f / tangent;

    *ctx.CurMatrix[ctx.CurMatrixMode] *= Mat44(
            f/aspect, 0.f, 0.f, 0.f,
            0, f, 0.f, 0.f,
            0.f, 0.f, (zFar+zNear)/dz, (2.f*zFar*zNear)/dz,
            0.f, 0.f, -1.f, 0.f);
}

// --------------------------------------------------------------------------
inline bool MinGL::compareFuncAssign(GLenum func, CompareFuncE& into)
{
    switch (func)
    {
        case GL_NEVER: into = CF_Never; return true;
        case GL_LESS: into = CF_Less; return true;
        case GL_EQUAL: into = CF_Equal; return true;
        case GL_LEQUAL: into = CF_LessEqual; return true;
        case GL_GREATER: into = CF_Greater; return true;
        case GL_NOTEQUAL: into = CF_NotEqual; return true;
        case GL_GEQUAL: into = CF_GreaterEqual; return true;
        case GL_ALWAYS: into = CF_Always; return true;
        default: MINGL_ERR_WRET(GL_INVALID_ENUM, false);
    }
}

// --------------------------------------------------------------------------
inline void MinGL::enableOrDisable(GLenum cap, bool val)
{
    switch (cap)
    {
        case GL_TEXTURE_2D:
            ctx.Texture2DEnabled = val;
            break;
        case GL_DEPTH_TEST:
            ctx.DepthTestEnabled = val;
            break;
        default:
            MINGL_ASSERT(0);
    }
}
// --------------------------------------------------------------------------
inline void MinGL::enableOrDisableCS(GLenum array, bool val)
{
    switch(array)
    {
        case GL_VERTEX_ARRAY: ctx.VertexArray.Enabled = val; break;
        case GL_NORMAL_ARRAY: ctx.NormalArray.Enabled = val; break;
        case GL_COLOR_ARRAY: ctx.ColorArray.Enabled = val; break;
        case GL_TEXTURE_COORD_ARRAY: ctx.TexCoordArray[ctx.CurClientActiveTexture].Enabled = val; break;
        default: MINGL_ERR(GL_INVALID_ENUM);
    }
}


}
