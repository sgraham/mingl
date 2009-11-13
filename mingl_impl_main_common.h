namespace mingl
{

inline void MinGL::ActiveTexture(GLenum texture)
{
    MINGL_ASSERT(0);
}

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

inline void MinGL::ClearDepth(GLclampf depth) { MINGL_ASSERT(0); }
inline void MinGL::ClearStencil(GLint s) { MINGL_ASSERT(0); }
inline void MinGL::ClientActiveTexture(GLenum texture)
{
    if (texture > GL_TEXTURE0 + TU_NumTextureUnits) MINGL_ERR(GL_INVALID_ENUM);
    ctx.CurActiveTexture = texture;
}

inline void MinGL::Color4(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    ctx.Vert.Color = Vec4(red, green, blue, alpha);
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
inline void MinGL::DepthRange(GLclampf zNear, GLclampf zFar) { MINGL_ASSERT(0); }
inline void MinGL::Disable(GLenum cap) { MINGL_ASSERT(0); }

inline void MinGL::DisableClientState(GLenum array)
{
    switch(array)
    {
        case GL_VERTEX_ARRAY: ctx.ClientState &= ~CS_VertexArray; break;
        case GL_NORMAL_ARRAY: ctx.ClientState &= ~CS_NormalArray; break;
        case GL_COLOR_ARRAY: ctx.ClientState &= ~CS_ColorArray; break;
        case GL_TEXTURE_COORD_ARRAY: ctx.ClientState &= ~CS_TexCoordArray; break;
        default: MINGL_ERR(GL_INVALID_ENUM);
    }
}

inline void MinGL::DrawArrays(GLenum mode, GLint first, GLsizei count)
{
    TriVert a, b, c;
    if (mode == GL_TRIANGLES)
    {
        // todo; multitex
        const GLint vstep = (ctx.VertexArray.Size + ctx.VertexArray.Stride);
        const GLint tstep = (ctx.TexCoordArray[0].Size + ctx.TexCoordArray[0].Stride);
        const GLint vlast = first + count;
        GLint vi = first;
        GLint ti = first;
        while (vi < vlast)
        {
            a.pos = Vec4(&ctx.VertexArray.Data[vi]); vi += vstep;
            b.pos = Vec4(&ctx.VertexArray.Data[vi]); vi += vstep;
            c.pos = Vec4(&ctx.VertexArray.Data[vi]); vi += vstep; // todo; this is going to load past end on last one.
            if (ctx.Texture2DEnabled)
            {
                a.tex = Vec4(&ctx.TexCoordArray[0].Data[ti]); ti += tstep;
                b.tex = Vec4(&ctx.TexCoordArray[0].Data[ti]); ti += tstep;
                c.tex = Vec4(&ctx.TexCoordArray[0].Data[ti]); ti += tstep; // todo; this is going to load past end on last one.
            }
            // todo; this is silly
            if (ctx.VertexArray.Size == 2)
            {
                a.pos.SetZ(-100.f);
                b.pos.SetZ(-100.f);
                c.pos.SetZ(-100.f);
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
        const GLint tstep = (ctx.TexCoordArray[0].Size + ctx.TexCoordArray[0].Stride);
        const GLint vlast = first + count;
        GLint vi = first;
        GLint ti = first;
        TriVert a, b, c;
        bool odd = true;
        while (vi < vlast)
        {
            a.pos = Vec4(&ctx.VertexArray.Data[vi]); vi += vstep;
            b.pos = Vec4(&ctx.VertexArray.Data[vi]);
            c.pos = Vec4(&ctx.VertexArray.Data[vi + vstep]); // todo; this is going to load past end on last one.
            if (ctx.Texture2DEnabled)
            {
                a.tex = Vec4(&ctx.TexCoordArray[0].Data[ti]); ti += tstep;
                b.tex = Vec4(&ctx.TexCoordArray[0].Data[ti]);
                c.tex = Vec4(&ctx.TexCoordArray[0].Data[ti + tstep]); // todo; this is going to load past end on last one.
            }
            // todo; this is silly
            if (ctx.VertexArray.Size == 2)
            {
                a.pos.SetZ(-100.f);
                b.pos.SetZ(-100.f);
                c.pos.SetZ(-100.f);
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
        case GL_VERTEX_ARRAY: ctx.ClientState |= CS_VertexArray; break;
        case GL_NORMAL_ARRAY: ctx.ClientState |= CS_NormalArray; break;
        case GL_COLOR_ARRAY: ctx.ClientState |= CS_ColorArray; break;
        case GL_TEXTURE_COORD_ARRAY: ctx.ClientState |= CS_TexCoordArray; break;
        default: MINGL_ERR(GL_INVALID_ENUM);
    }
}

inline void MinGL::Finish() { MINGL_ASSERT(0); }
inline void MinGL::Flush() { MINGL_ASSERT(0); }
inline void MinGL::Fog(GLenum pname, GLfloat param) { MINGL_ASSERT(0); }
inline void MinGL::Fog(GLenum pname, const GLfloat *params) { MINGL_ASSERT(0); }
inline void MinGL::FrontFace(GLenum mode) { MINGL_ASSERT(0); }
inline void MinGL::Frustum(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar) { MINGL_ASSERT(0); }
inline void MinGL::GenTextures(GLsizei n, GLuint *textures)
{
    for (GLsizei i = 0; i < n; ++i)
        textures[i] = ctx.CurTexId++;
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
inline void MinGL::LightModel(GLenum pname, GLfloat param) { MINGL_ASSERT(0); }
inline void MinGL::LightModel(GLenum pname, const GLfloat *params) { MINGL_ASSERT(0); }
inline void MinGL::Light(GLenum light, GLenum pname, GLfloat param) { MINGL_ASSERT(0); }
inline void MinGL::Light(GLenum light, GLenum pname, const GLfloat *params) { MINGL_ASSERT(0); }
inline void MinGL::LineWidth(GLfloat width) { MINGL_ASSERT(0); }

inline void MinGL::LoadIdentity()
{
    *ctx.CurMatrix[ctx.MatrixMode] = Mat44(
            1.f, 0.f, 0.f, 0.f,
            0.f, 1.f, 0.f, 0.f,
            0.f, 0.f, 1.f, 0.f,
            0.f, 0.f, 0.f, 1.f);
}

inline void MinGL::LoadMatrix(const GLfloat *m)
{
    *ctx.CurMatrix[ctx.MatrixMode] = Mat44(m);
}

inline void MinGL::LogicOp(GLenum opcode) { MINGL_ASSERT(0); }
inline void MinGL::Material(GLenum face, GLenum pname, GLfloat param) { MINGL_ASSERT(0); }
inline void MinGL::Material(GLenum face, GLenum pname, const GLfloat *params) { MINGL_ASSERT(0); }

inline void MinGL::MatrixMode(GLenum mode)
{
    switch (mode)
    {
        case GL_MODELVIEW: ctx.MatrixMode = MM_ModelView; break;
        case GL_PROJECTION: ctx.MatrixMode = MM_Projection; break;
        case GL_TEXTURE: ctx.MatrixMode = MM_Texture; break;
        default: MINGL_ERR(GL_INVALID_ENUM);
    }
}

inline void MinGL::MultMatrix(const GLfloat *m)
{
    Mat44 mm(m);
    MINGL_ASSERT(false);
}

inline void MinGL::MultiTexCoord4(GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q) { MINGL_ASSERT(0); }
inline void MinGL::Normal3(GLfloat nx, GLfloat ny, GLfloat nz) { MINGL_ASSERT(0); }
inline void MinGL::NormalPointer(GLenum type, GLsizei stride, const GLvoid *pointer) { MINGL_ASSERT(0); }
inline void MinGL::Ortho(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar) { MINGL_ASSERT(0); }
inline void MinGL::PixelStorei(GLenum pname, GLint param) { MINGL_ASSERT(0); }
inline void MinGL::PointSize(GLfloat size) { MINGL_ASSERT(0); }
inline void MinGL::PolygonOffset(GLfloat factor, GLfloat units) { MINGL_ASSERT(0); }
inline void MinGL::PopMatrix() { MINGL_ASSERT(0); }
inline void MinGL::PushMatrix() { MINGL_ASSERT(0); }
inline void MinGL::ReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels) { MINGL_ASSERT(0); }
inline void MinGL::Rotate(GLfloat angle, GLfloat x, GLfloat y, GLfloat z) { MINGL_ASSERT(0); }
inline void MinGL::Scale(GLfloat x, GLfloat y, GLfloat z) { MINGL_ASSERT(0); }
inline void MinGL::Scissor(GLint x, GLint y, GLsizei width, GLsizei height) { MINGL_ASSERT(0); }
inline void MinGL::ShadeModel(GLenum mode) { MINGL_ASSERT(0); }
inline void MinGL::StencilFunc(GLenum func, GLint ref, GLuint mask) { MINGL_ASSERT(0); }
inline void MinGL::StencilMask(GLuint mask) { MINGL_ASSERT(0); }
inline void MinGL::StencilOp(GLenum fail, GLenum zfail, GLenum zpass) { MINGL_ASSERT(0); }

inline void MinGL::TexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
    ctx.TexCoordArray[0].Data = reinterpret_cast<const float*>(pointer);
    ctx.TexCoordArray[0].Size = size;
    ctx.TexCoordArray[0].Stride = stride;
    if (size < 2 || size > 4) MINGL_ERR(GL_INVALID_VALUE);
    if (type != GL_FLOAT) MINGL_ERR(GL_INVALID_ENUM);
    if (stride < 0) MINGL_ERR(GL_INVALID_VALUE);
}

inline void MinGL::TexEnv(GLenum target, GLenum pname, GLfloat param) { MINGL_ASSERT(0); }
inline void MinGL::TexEnv(GLenum target, GLenum pname, const GLfloat *params) { MINGL_ASSERT(0); }
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

inline void MinGL::TexParameter(GLenum target, GLenum pname, GLfloat param) { MINGL_ASSERT(0); }
inline void MinGL::TexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels) { MINGL_ASSERT(0); }
inline void MinGL::Translate(GLfloat x, GLfloat y, GLfloat z) { MINGL_ASSERT(0); }

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

}
