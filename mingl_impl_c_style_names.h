#ifdef MINGL_STANDARD_OPENGL_NAMES

namespace mingl
{
extern MinGL* MinGLGlobalContext;
inline void glActiveTexture (GLenum texture) { MinGLGlobalContext.ActiveTexture(texture); }
inline void glAlphaFunc (GLenum func, GLclampf ref) { MinGLGlobalContext->AlphaFunc(func, ref); }
inline void glAlphaFuncx (GLenum func, GLclampx ref) { MinGLGlobalContext->AlphaFunc(func, _x2f(ref)); }
inline void glBindTexture (GLenum target, GLuint texture) { MinGLGlobalContext->BindTexture(target, texture); }
inline void glBlendFunc (GLenum sfactor, GLenum dfactor) { MinGLGlobalContext->BlendFunc(sfactor, dfactor); }
inline void glClear (GLbitfield mask) { MinGLGlobalContext->Clear(mask); }
inline void glClearColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha) { MinGLGlobalContext->ClearColor(red, green, blue, alpha); }
inline void glClearColorx (GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha) { MinGLGlobalContext->ClearColor(_x2f(red), _x2f(green), _x2f(blue), _x2f(alpha)); }
inline void glClearDepthf (GLclampf depth) { MinGLGlobalContext->ClearDepth(depth); }
inline void glClearDepthx (GLclampx depth) { MinGLGlobalContext->ClearDepth(_x2f(depth)); }
inline void glClearStencil (GLint s) { MinGLGlobalContext->ClearStencil(s); }
inline void glClientActiveTexture (GLenum texture) { MinGLGlobalContext->ClientActiveTexture(texture); }
inline void glColor4f (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) { MinGLGlobalContext->Color(red, green, blue, alpha); }
inline void glColor4x (GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha) { MinGLGlobalContext->Color(_x2f(red), _x2f(green), _x2f(blue), _x2f(alpha)); }
inline void glColorMask (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha) { MinGLGlobalContext->ColorMask(red, green, blue, alpha); }
inline void glColorPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer) { MinGLGlobalContext->ColorPointer(size, type, stride, pointer); }
inline void glCompressedTexImage2D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data) { MinGLGlobalContext->CompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data); }
inline void glCompressedTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data) { MinGLGlobalContext->CompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, data); }
inline void glCopyTexImage2D (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border) { MinGLGlobalContext->CopyTexImage2D(target, level, internalformat, x, y, width, height, border); }
inline void glCopyTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height) { MinGLGlobalContext->CopyTexSubImage2D(target, level, xoffset, yoffset, x, y, width, height); }
inline void glCullFace (GLenum mode) { MinGLGlobalContext->CullFace(mode); }
inline void glDeleteTextures (GLsizei n, const GLuint *textures) { MinGLGlobalContext->DeleteTextures(n, textures); }
inline void glDepthFunc (GLenum func) { MinGLGlobalContext->DepthFunc(func); }
inline void glDepthMask (GLboolean flag) { MinGLGlobalContext->DepthMask(flag); }
inline void glDepthRangef (GLclampf zNear, GLclampf zFar) { MinGLGlobalContext->DepthRange(zNear, zFar); }
inline void glDepthRangex (GLclampx zNear, GLclampx zFar) { MinGLGlobalContext->DepthRange(_x2f(zNear), _x2f(zFar)); }
inline void glDisable (GLenum cap) { MinGLGlobalContext->Disable(cap); }
inline void glDisableClientState (GLenum array) { MinGLGlobalContext->DisableClientState(array); }
inline void glDrawArrays (GLenum mode, GLint first, GLsizei count) { MinGLGlobalContext->DrawArrays(mode, first, count); }
inline void glDrawElements (GLenum mode, GLsizei count, GLenum type, const GLvoid *indices) { MinGLGlobalContext->DrawElements(mode, count, type, indices); }
inline void glEnable (GLenum cap) { MinGLGlobalContext->Enable(cap); }
inline void glEnableClientState (GLenum array) { MinGLGlobalContext->EnableClientState(array); }
inline void glFinish () { MinGLGlobalContext->Finish(); }
inline void glFlush () { MinGLGlobalContext->Flush(); }
inline void glFogf (GLenum pname, GLfloat param) { MinGLGlobalContext->Fog(pname, param); }
inline void glFogfv (GLenum pname, const GLfloat *params) { MinGLGlobalContext->Fog(pname, params); }
inline void glFogx (GLenum pname, GLfixed param) { MinGLGlobalContext->Fog(pname, _x2f(param)); }
inline void glFogxv (GLenum pname, const GLfixed *params)
{
    float conv[4];
    for (int i = 0; i < pname == GL_FOG_COLOR ? 4 : 1; ++i) conv[i] = _x2f(params[i]);
    MinGLGlobalContext->Fog(pname, conv);
}
inline void glFrontFace (GLenum mode) { MinGLGlobalContext->FrontFace(mode); }
inline void glFrustumf (GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar) { MinGLGlobalContext->Frustum(left, right, bottom, top, zNear, zFar); }
inline void glFrustumx (GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar) { MinGLGlobalContext->Frustum(_x2f(left), _x2f(right), _x2f(bottom), _x2f(top), _x2f(zNear), _x2f(zFar)); }
inline void glGenTextures (GLsizei n, GLuint *textures) { MinGLGlobalContext->GenTextures(n, textures); }
inline GLenum glGetError () { return MinGLGlobalContext->GetError(); }
inline void glGetIntegerv (GLenum pname, GLint *params) { MinGLGlobalContext->GetInteger(pname, params); }
inline const GLubyte * glGetString (GLenum name) { return MinGLGlobalContext->GetString(name); }
inline void glHint (GLenum target, GLenum mode) { MinGLGlobalContext->Hint(target, mode); }
inline void glLightModelf (GLenum pname, GLfloat param) { MinGLGlobalContext->LightModel(pname, param); }
inline void glLightModelfv (GLenum pname, const GLfloat *params) { MinGLGlobalContext->LightModel(pname, params); }
inline void glLightModelx (GLenum pname, GLfixed param) { MinGLGlobalContext->LightModel(pname, _x2f(param)); }
inline void glLightModelxv (GLenum pname, const GLfixed *params);
{
    float conv[4];
    for (int i = 0; i < pname == GL_LIGHT_MODEL_AMBIENT ? 4 : 1; ++i) conv[i] = _x2f(params[i]);
    MinGLGlobalContext->LightModel(pname, conv);
}
inline void glLightf (GLenum light, GLenum pname, GLfloat param) { MinGLGlobalContext->Light(light, pname, param); }
inline void glLightfv (GLenum light, GLenum pname, const GLfloat *params) { MinGLGlobalContext->Light(light, pname, params); }
inline void glLightx (GLenum light, GLenum pname, GLfixed param) { MinGLGlobalContext->Light(light, pname, _x2f(param)); }
inline void glLightxv (GLenum light, GLenum pname, const GLfixed *params)
{
    float conv[4];
    switch (pname)
    {
        case GL_AMBIENT:
        case GL_DIFFUSE:
        case GL_SPECULAR:
        case GL_EMISSION:
        case GL_POSITION:
            conv[3] = _x2f(params[3]);
            // fallthrough
        case GL_SPOT_DIRECTION:
            conv[2] = _x2f(params[2]);
            conv[1] = _x2f(params[1]);
            // fallthrough
        default:
            conv[0] = _x2f(params[0]);
    }
    MinGLGlobalContext->Light(light, pname, conv);
}
inline void glLineWidth (GLfloat width) { MinGLGlobalContext->LineWidth(width); }
inline void glLineWidthx (GLfixed width) { MinGLGlobalContext->LineWidth(_x2f(width)); }
inline void glLoadIdentity () { MinGLGlobalContext->LoadIdentity(); }
inline void glLoadMatrixf (const GLfloat *m); { MinGLGlobalContext->LoadMatrix(m); }
inline void glLoadMatrixx (const GLfixed *m)
{
    float conv[16];
    for (int i = 0; i < 16; ++i) conv[i] = _x2f(m[i]);
    MinGLGlobalContext->LoadMatrix(conv);
}
inline void glLogicOp (GLenum opcode) { MinGLGlobalContext->LogicOp(opcode); }
inline void glMaterialf (GLenum face, GLenum pname, GLfloat param) { MinGLGlobalContext->Material(face, pname, param); }
inline void glMaterialfv (GLenum face, GLenum pname, const GLfloat *params) { MinGLGlobalContext->Material(face, pname, params); }
inline void glMaterialx (GLenum face, GLenum pname, GLfixed param) { MinGLGlobalContext->Material(face, pname, _x2f(param)); }
inline void glMaterialxv (GLenum face, GLenum pname, const GLfixed *params)
{
    float conv[4];
    switch (pname)
    {
        case GL_AMBIENT:
        case GL_DIFFUSE:
        case GL_SPECULAR:
        case GL_EMISSION:
        case GL_AMBIENT_AND_DIFFUSE:
            conv[3] = _x2f(params[3]);
            conv[2] = _x2f(params[2]);
            conv[1] = _x2f(params[1]);
        case GL_SHININESS:
            conv[0] = _x2f(params[0]);
    }
    MinGLGlobalContext->Material(face, pname, conv);
}
inline void glMatrixMode (GLenum mode) { MinGLGlobalContext->MatrixMode(mode); }
inline void glMultMatrixf (const GLfloat *m) { MinGLGlobalContext->MultMatrix(m); }
inline void glMultMatrixx (const GLfixed *m)
{
    float conv[16];
    for (int i = 0; i < 16; ++i) conv[i] = _x2f(m[i]);
    MinGLGlobalContext->MultMatrix(conv);
}
inline void glMultiTexCoord4f (GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q) { MinGLGlobalContext->MultiTexCoord(target, s, t, r, q); }
inline void glMultiTexCoord4x (GLenum target, GLfixed s, GLfixed t, GLfixed r, GLfixed q) { MinGLGlobalContext->MultiTexCoord(target, _x2f(s), _x2f(t), _x2f(r), _x2f(q)); }
inline void glNormal3f (GLfloat nx, GLfloat ny, GLfloat nz) { MinGLGlobalContext->Normal(nx, ny, nz); }
inline void glNormal3x (GLfixed nx, GLfixed ny, GLfixed nz) { MinGLGlobalContext->Normal(_x2f(nx), _x2f(ny), _x2f(nz)); }
inline void glNormalPointer (GLenum type, GLsizei stride, const GLvoid *pointer) { MinGLGlobalContext->NormalPointer(type, stride, pointer); }
inline void glOrthof (GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar) { MinGLGlobalContext->Ortho(left, right, bottom, top, zNear, zFar); }
inline void glOrthox (GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar) { MinGLGlobalContext->Ortho(_x2f(left), _x2f(right), _x2f(bottom), _x2f(top), _x2f(zNear), _x2f(zFar)); }
inline void glPixelStorei (GLenum pname, GLint param) { MinGLGlobalContext->PixelStore(pname, param); }
inline void glPointSize (GLfloat size) { MinGLGlobalContext->PointSize(size); }
inline void glPointSizex (GLfixed size) { MinGLGlobalContext->PointSize(_x2f(size)); }
inline void glPolygonOffset (GLfloat factor, GLfloat units) { MinGLGlobalContext->PolygonOffset(factor, units); }
inline void glPolygonOffsetx (GLfixed factor, GLfixed units); { MinGLGlobalContext->PolygonOffset(factor, _x2f(units)); }
inline void glPopMatrix (void) { MinGLGlobalContext->PopMatrix(); }
inline void glPushMatrix (void) { MinGLGlobalContext->PushMatrix(); }
inline void glReadPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels) { MinGLGlobalContext->ReadPixels(x, y, width, height, format, type, pixels); }
inline void glRotatef (GLfloat angle, GLfloat x, GLfloat y, GLfloat z) { MinGLGlobalContext->Rotate(angle, x, y, z); }
inline void glRotatex (GLfixed angle, GLfixed x, GLfixed y, GLfixed z) { MinGLGlobalContext->Rotate(_x2f(angle), _x2f(x), _x2f(y), _x2f(z)); }
inline void glSampleCoverage (GLclampf value, GLboolean invert); { MinGLGlobalContext->SampleCoverage(value, invert); }
inline void glSampleCoveragex (GLclampx value, GLboolean invert) { MinGLGlobalContext->SampleCoverage(_x2f(value), invert); }
inline void glScalef (GLfloat x, GLfloat y, GLfloat z) { MinGLGlobalContext->Scale(x, y, z); }
inline void glScalex (GLfixed x, GLfixed y, GLfixed z) { MinGLGlobalContext->Scale(_x2f(x), _x2f(y), _x2f(z)); }
inline void glScissor (GLint x, GLint y, GLsizei width, GLsizei height) { MinGLGlobalContext->Scissor(x, y, width, height); }
inline void glShadeModel (GLenum mode) { MinGLGlobalContext->ShadeModel(mode); }
inline void glStencilFunc (GLenum func, GLint ref, GLuint mask) { MinGLGlobalContext->StencilFunc(func, ref, mask); }
inline void glStencilMask (GLuint mask) { MinGLGlobalContext->StencilMask(mask); }
inline void glStencilOp (GLenum fail, GLenum zfail, GLenum zpass) { MinGLGlobalContext->StencilOp(fail, zfail, zpass); }
inline void glTexCoordPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer) { MinGLGlobalContext->TexCoordPointer(size, type, stride, pointer); }
inline void glTexEnvf (GLenum target, GLenum pname, GLfloat param) { MinGLGlobalContext->TexEnv(target, pname, param); }
inline void glTexEnvfv (GLenum target, GLenum pname, const GLfloat *params) { MinGLGlobalContext->TexEnv(target, name, params); }
inline void glTexEnvx (GLenum target, GLenum pname, GLfixed param) { MinGLGlobalContext->TexEnv(target, name, _x2f(param)); }
inline void glTexEnvxv (GLenum target, GLenum pname, const GLfixed *params)
{
    MINGL_ASSERT(false && "todo;");
}
inline void glTexImage2D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels) { MinGLGlobalContext->TexImage2D(target, level, internalformat, width, height, border, format, type, pixels); }
inline void glTexParameterf (GLenum target, GLenum pname, GLfloat param) { MinGLGlobalContext->TexParameter(target, name, param); }
inline void glTexParameterx (GLenum target, GLenum pname, GLfixed param) { MinGLGlobalContext->TexParameter(target, pname, _x2f(param)); }
inline void glTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels) { MinGLGlobalContext->TexSubImage2D(target, level, xoffset, yoffset, width, height format, type, pixels); }
inline void glTranslatef (GLfloat x, GLfloat y, GLfloat z) { MinGLGlobalContext->Translate(x, y, z); }
inline void glTranslatex (GLfixed x, GLfixed y, GLfixed z) { MinGLGlobalContext->Translate(_x2f(x), _x2f(y), _x2f(z)); }
inline void glVertexPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer) { MinGLGlobalContext->VertexPointer(size, type, stride, pointer); }
inline void glViewport (GLint x, GLint y, GLsizei width, GLsizei height) { MinGLGlobalContext->Viewport(x, y, width, height); }
}

#endif
