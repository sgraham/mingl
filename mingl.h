// -----------------------------------------------------------------------
// FOR LICENSE INFORMATION, PLEASE SEE THE BOTTOM OF THIS FILE.
// -----------------------------------------------------------------------

// mingl is a minimal graphics/demo library. Its goals are definitely not to
// be fast, take advantage of fancy hardware features, or to show off neat
// effects. Instead, its only goals are to be servicable to draw simple
// dianostic scenes on many different platforms, while being distributed as
// only one header and one c++ file, and not requiring any configuration or
// having any external dependencies.
//
// The API of mingls graphics is modelled after OpenGL ES 1.x. It is by no
// means complete, but a programmer familiar with OpenGL shouldn't have any
// trouble getting something on the screen. mingl does expect to have a
// floating point unit though, and drop all the fixed-related API functions
// from the ES spec.
//
// In addition, there is a simple interface for demo-type behaviour of setting
// up a rendering context, basic user input, and basic camera controls.

namespace mingl
{
typedef unsigned int GLenum;
typedef unsigned int GLbitfield;
typedef int GLint;
typedef int GLsizei;
typedef unsigned char GLubyte;
typedef unsigned int GLuint;
typedef float GLfloat;
typedef float GLclampf;
typedef void GLvoid;

// endianness
#if defined(__LITTLE_ENDIAN__)
	#define MINGL_LITTLE_ENDIAN
#endif

#if defined(__BIG_ENDIAN__)
	#define MINGL_BIG_ENDIAN
#endif

// todo; some magical platform detection here

#if defined(MINGL_LITTLE_ENDIAN) && defined(MINGL_BIG_ENDIAN)
	#error cannot define both big and little endian!
#endif

#if !defined(MINGL_LITTLE_ENDIAN) && !defined(MINGL_BIG_ENDIAN)
	#error endianness not defined
#endif

// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
// The framebuffer API follows
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
class Pixel
{
    public:
        Pixel()
            : r(0.f)
            , g(0.f)
            , b(0.f)
            , a(0.f)
        {}
        Pixel(float r, float g, float b, float a = 0.f)
        {
            this->r = r;
            this->g = g;
            this->b = b;
            this->a = a;
        }
        float r;
        float g;
        float b;
        float a;
};

class Display
{
    public:
        Display();
        Display(const char* title, int width, int height);
        ~Display();

        bool Open(const char* title, int width, int height);
        void Close();
        bool IsOpen() const;
        bool Update(const Pixel* pixels);
};

// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
// The rendering API follows
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------

/*************************************************************/

enum ClearBufferMask {
    GL_DEPTH_BUFFER_BIT =             0x00000100,
    GL_STENCIL_BUFFER_BIT =           0x00000400,
    GL_COLOR_BUFFER_BIT =             0x00004000,
};

enum BeginMode {
    GL_POINTS =                       0x0000,
    GL_LINES =                        0x0001,
    GL_LINE_LOOP =                    0x0002,
    GL_LINE_STRIP =                   0x0003,
    GL_TRIANGLES =                    0x0004,
    GL_TRIANGLE_STRIP =               0x0005,
    GL_TRIANGLE_FAN =                 0x0006,
};

enum AlphaFunction {
    GL_NEVER =                        0x0200,
    GL_LESS =                         0x0201,
    GL_EQUAL =                        0x0202,
    GL_LEQUAL =                       0x0203,
    GL_GREATER =                      0x0204,
    GL_NOTEQUAL =                     0x0205,
    GL_GEQUAL =                       0x0206,
    GL_ALWAYS =                       0x0207,
};

enum BlendingFactorDest {
    GL_ZERO =                         0,
    GL_ONE =                          1,
    GL_SRC_COLOR =                    0x0300,
    GL_ONE_MINUS_SRC_COLOR =          0x0301,
    GL_SRC_ALPHA =                    0x0302,
    GL_ONE_MINUS_SRC_ALPHA =          0x0303,
    GL_DST_ALPHA =                    0x0304,
    GL_ONE_MINUS_DST_ALPHA =          0x0305,
};

enum BlendingFactorSrc {
/*  GL_ZERO */
/*  GL_ONE */
    GL_DST_COLOR =                    0x0306,
    GL_ONE_MINUS_DST_COLOR =          0x0307,
    GL_SRC_ALPHA_SATURATE =           0x0308,
/*  GL_SRC_ALPHA */
/*  GL_ONE_MINUS_SRC_ALPHA */
/*  GL_DST_ALPHA */
/*  GL_ONE_MINUS_DST_ALPHA */
};
 
/* ColorMaterialFace */
/*      GL_FRONT_AND_BACK */

/* ColorMaterialParameter */
/*      GL_AMBIENT_AND_DIFFUSE */

/* ColorPointerType */
/*      GL_UNSIGNED_BYTE */
/*      GL_FLOAT */
/*      GL_FIXED */

enum CullFaceMode {
    GL_FRONT =                        0x0404,
    GL_BACK =                         0x0405,
    GL_FRONT_AND_BACK =               0x0408,
};

/* DepthFunction */
/*      GL_NEVER */
/*      GL_LESS */
/*      GL_EQUAL */
/*      GL_LEQUAL */
/*      GL_GREATER */
/*      GL_NOTEQUAL */
/*      GL_GEQUAL */
/*      GL_ALWAYS */

enum EnableCap {
    GL_FOG =                          0x0B60,
    GL_LIGHTING =                     0x0B50,
    GL_TEXTURE_2D =                   0x0DE1,
    GL_CULL_FACE =                    0x0B44,
    GL_ALPHA_TEST =                   0x0BC0,
    GL_BLEND =                        0x0BE2,
    GL_COLOR_LOGIC_OP =               0x0BF2,
    GL_DITHER =                       0x0BD0,
    GL_STENCIL_TEST =                 0x0B90,
    GL_DEPTH_TEST =                   0x0B71,
/*      GL_LIGHT0 */
/*      GL_LIGHT1 */
/*      GL_LIGHT2 */
/*      GL_LIGHT3 */
/*      GL_LIGHT4 */
/*      GL_LIGHT5 */
/*      GL_LIGHT6 */
/*      GL_LIGHT7 */
    GL_POINT_SMOOTH =                 0x0B10,
    GL_LINE_SMOOTH =                  0x0B20,
    GL_SCISSOR_TEST =                 0x0C11,
    GL_COLOR_MATERIAL =               0x0B57,
    GL_NORMALIZE =                    0x0BA1,
    GL_RESCALE_NORMAL =               0x803A,
    GL_POLYGON_OFFSET_FILL =          0x8037,
    GL_VERTEX_ARRAY =                 0x8074,
    GL_NORMAL_ARRAY =                 0x8075,
    GL_COLOR_ARRAY =                  0x8076,
    GL_TEXTURE_COORD_ARRAY =          0x8078,
    GL_MULTISAMPLE =                  0x809D,
    GL_SAMPLE_ALPHA_TO_COVERAGE =     0x809E,
    GL_SAMPLE_ALPHA_TO_ONE =          0x809F,
    GL_SAMPLE_COVERAGE =              0x80A0,
};

enum ErrorCode {
    GL_NO_ERROR =                     0,
    GL_INVALID_ENUM =                 0x0500,
    GL_INVALID_VALUE =                0x0501,
    GL_INVALID_OPERATION =            0x0502,
    GL_STACK_OVERFLOW =               0x0503,
    GL_STACK_UNDERFLOW =              0x0504,
    GL_OUT_OF_MEMORY =                0x0505,
};

enum FogMode {
/*      GL_LINEAR */
    GL_EXP =                          0x0800,
    GL_EXP2 =                         0x0801,
};

enum FogParameter {
    GL_FOG_DENSITY =                  0x0B62,
    GL_FOG_START =                    0x0B63,
    GL_FOG_END =                      0x0B64,
    GL_FOG_MODE =                     0x0B65,
    GL_FOG_COLOR =                    0x0B66,
};

enum FrontFaceDirection {
    GL_CW =                           0x0900,
    GL_CCW =                          0x0901,
};

enum GetPName {
    GL_SMOOTH_POINT_SIZE_RANGE =      0x0B12,
    GL_SMOOTH_LINE_WIDTH_RANGE =      0x0B22,
    GL_ALIASED_POINT_SIZE_RANGE =     0x846D,
    GL_ALIASED_LINE_WIDTH_RANGE =     0x846E,
    GL_IMPLEMENTATION_COLOR_READ_TYPE_OES = 0x8B9A,
    GL_IMPLEMENTATION_COLOR_READ_FORMAT_OES = 0x8B9B,
    GL_MAX_LIGHTS =                   0x0D31,
    GL_MAX_TEXTURE_SIZE =             0x0D33,
    GL_MAX_MODELVIEW_STACK_DEPTH =    0x0D36,
    GL_MAX_PROJECTION_STACK_DEPTH =   0x0D38,
    GL_MAX_TEXTURE_STACK_DEPTH =      0x0D39,
    GL_MAX_VIEWPORT_DIMS =            0x0D3A,
    GL_MAX_ELEMENTS_VERTICES =        0x80E8,
    GL_MAX_ELEMENTS_INDICES =         0x80E9,
    GL_MAX_TEXTURE_UNITS =            0x84E2,
    GL_NUM_COMPRESSED_TEXTURE_FORMATS = 0x86A2,
    GL_COMPRESSED_TEXTURE_FORMATS =   0x86A3,
    GL_SUBPIXEL_BITS =                0x0D50,
    GL_RED_BITS =                     0x0D52,
    GL_GREEN_BITS =                   0x0D53,
    GL_BLUE_BITS =                    0x0D54,
    GL_ALPHA_BITS =                   0x0D55,
    GL_DEPTH_BITS =                   0x0D56,
    GL_STENCIL_BITS =                 0x0D57,
};

enum HintMode {
    GL_DONT_CARE =                    0x1100,
    GL_FASTEST =                      0x1101,
    GL_NICEST =                       0x1102,
};

enum HintTarget {
    GL_PERSPECTIVE_CORRECTION_HINT =  0x0C50,
    GL_POINT_SMOOTH_HINT =            0x0C51,
    GL_LINE_SMOOTH_HINT =             0x0C52,
    GL_POLYGON_SMOOTH_HINT =          0x0C53,
    GL_FOG_HINT =                     0x0C54,
};

enum LightModelParameter {
    GL_LIGHT_MODEL_AMBIENT =          0x0B53,
    GL_LIGHT_MODEL_TWO_SIDE =         0x0B52,
};

enum LightParameter {
    GL_AMBIENT =                      0x1200,
    GL_DIFFUSE =                      0x1201,
    GL_SPECULAR =                     0x1202,
    GL_POSITION =                     0x1203,
    GL_SPOT_DIRECTION =               0x1204,
    GL_SPOT_EXPONENT =                0x1205,
    GL_SPOT_CUTOFF =                  0x1206,
    GL_CONSTANT_ATTENUATION =         0x1207,
    GL_LINEAR_ATTENUATION =           0x1208,
    GL_QUADRATIC_ATTENUATION =        0x1209,
};

enum DataType {
    GL_BYTE =                         0x1400,
    GL_UNSIGNED_BYTE =                0x1401,
    GL_SHORT =                        0x1402,
    GL_UNSIGNED_SHORT =               0x1403,
    GL_FLOAT =                        0x1406,
    GL_FIXED =                        0x140C,
};

enum LogicOp {
    GL_CLEAR =                        0x1500,
    GL_AND =                          0x1501,
    GL_AND_REVERSE =                  0x1502,
    GL_COPY =                         0x1503,
    GL_AND_INVERTED =                 0x1504,
    GL_NOOP =                         0x1505,
    GL_XOR =                          0x1506,
    GL_OR =                           0x1507,
    GL_NOR =                          0x1508,
    GL_EQUIV =                        0x1509,
    GL_INVERT =                       0x150A,
    GL_OR_REVERSE =                   0x150B,
    GL_COPY_INVERTED =                0x150C,
    GL_OR_INVERTED =                  0x150D,
    GL_NAND =                         0x150E,
    GL_SET =                          0x150F,
};

/* MaterialFace */
/*      GL_FRONT_AND_BACK */

enum MaterialParameter {
    GL_EMISSION =                     0x1600,
    GL_SHININESS =                    0x1601,
    GL_AMBIENT_AND_DIFFUSE =          0x1602,
/*      GL_AMBIENT */
/*      GL_DIFFUSE */
/*      GL_SPECULAR */
};

enum MatrixMode {
    GL_MODELVIEW =                    0x1700,
    GL_PROJECTION =                   0x1701,
    GL_TEXTURE =                      0x1702,
};

/* NormalPointerType */
/*      GL_BYTE */
/*      GL_SHORT */
/*      GL_FLOAT */
/*      GL_FIXED */

enum PixelFormat {
    GL_ALPHA =                        0x1906,
    GL_RGB =                          0x1907,
    GL_RGBA =                         0x1908,
    GL_LUMINANCE =                    0x1909,
    GL_LUMINANCE_ALPHA =              0x190A,
};

enum PixelStoreParameter {
    GL_UNPACK_ALIGNMENT =             0x0CF5,
    GL_PACK_ALIGNMENT =               0x0D05,
};

enum PixelType {
/*      GL_UNSIGNED_BYTE */
    GL_UNSIGNED_SHORT_4_4_4_4 =       0x8033,
    GL_UNSIGNED_SHORT_5_5_5_1 =       0x8034,
    GL_UNSIGNED_SHORT_5_6_5 =         0x8363,
};

enum ShadingModel {
    GL_FLAT =                         0x1D00,
    GL_SMOOTH =                       0x1D01,
};

/* StencilFunction */
/*      GL_NEVER */
/*      GL_LESS */
/*      GL_EQUAL */
/*      GL_LEQUAL */
/*      GL_GREATER */
/*      GL_NOTEQUAL */
/*      GL_GEQUAL */
/*      GL_ALWAYS */

enum StencilOp {
/*      GL_ZERO */
    GL_KEEP =                         0x1E00,
    GL_REPLACE =                      0x1E01,
    GL_INCR =                         0x1E02,
    GL_DECR =                         0x1E03,
/*      GL_INVERT */
};

enum StringName {
    GL_VENDOR =                       0x1F00,
    GL_RENDERER =                     0x1F01,
    GL_VERSION =                      0x1F02,
    GL_EXTENSIONS =                   0x1F03,
};

/* TexCoordPointerType */
/*      GL_SHORT */
/*      GL_FLOAT */
/*      GL_FIXED */
/*      GL_BYTE */

enum TextureEnvMode {
    GL_MODULATE =                     0x2100,
    GL_DECAL =                        0x2101,
/*      GL_BLEND */
    GL_ADD =                          0x0104,
/*      GL_REPLACE */
};

enum TextureEnvParameter {
    GL_TEXTURE_ENV_MODE =             0x2200,
    GL_TEXTURE_ENV_COLOR =            0x2201,
};

enum TextureEnvTarget {
    GL_TEXTURE_ENV =                  0x2300,
};

enum TextureMagFilter {
    GL_NEAREST =                      0x2600,
    GL_LINEAR =                       0x2601,
};

enum TextureMinFilter {
/*      GL_NEAREST */
/*      GL_LINEAR */
    GL_NEAREST_MIPMAP_NEAREST =       0x2700,
    GL_LINEAR_MIPMAP_NEAREST =        0x2701,
    GL_NEAREST_MIPMAP_LINEAR =        0x2702,
    GL_LINEAR_MIPMAP_LINEAR =         0x2703,
};

enum TextureParameterName {
    GL_TEXTURE_MAG_FILTER =           0x2800,
    GL_TEXTURE_MIN_FILTER =           0x2801,
    GL_TEXTURE_WRAP_S =               0x2802,
    GL_TEXTURE_WRAP_T =               0x2803,
};

/* TextureTarget */
/*      GL_TEXTURE_2D */

enum TextureUnit {
    GL_TEXTURE0 =                     0x84C0,
    GL_TEXTURE1 =                     0x84C1,
    GL_TEXTURE2 =                     0x84C2,
    GL_TEXTURE3 =                     0x84C3,
    GL_TEXTURE4 =                     0x84C4,
    GL_TEXTURE5 =                     0x84C5,
    GL_TEXTURE6 =                     0x84C6,
    GL_TEXTURE7 =                     0x84C7,
    GL_TEXTURE8 =                     0x84C8,
    GL_TEXTURE9 =                     0x84C9,
    GL_TEXTURE10 =                    0x84CA,
    GL_TEXTURE11 =                    0x84CB,
    GL_TEXTURE12 =                    0x84CC,
    GL_TEXTURE13 =                    0x84CD,
    GL_TEXTURE14 =                    0x84CE,
    GL_TEXTURE15 =                    0x84CF,
    GL_TEXTURE16 =                    0x84D0,
    GL_TEXTURE17 =                    0x84D1,
    GL_TEXTURE18 =                    0x84D2,
    GL_TEXTURE19 =                    0x84D3,
    GL_TEXTURE20 =                    0x84D4,
    GL_TEXTURE21 =                    0x84D5,
    GL_TEXTURE22 =                    0x84D6,
    GL_TEXTURE23 =                    0x84D7,
    GL_TEXTURE24 =                    0x84D8,
    GL_TEXTURE25 =                    0x84D9,
    GL_TEXTURE26 =                    0x84DA,
    GL_TEXTURE27 =                    0x84DB,
    GL_TEXTURE28 =                    0x84DC,
    GL_TEXTURE29 =                    0x84DD,
    GL_TEXTURE30 =                    0x84DE,
    GL_TEXTURE31 =                    0x84DF,
};

enum TextureWrapMode {
    GL_REPEAT =                       0x2901,
    GL_CLAMP_TO_EDGE =                0x812F,
};

enum PixelInternalFormat {
    GL_PALETTE4_RGB8_OES =            0x8B90,
    GL_PALETTE4_RGBA8_OES =           0x8B91,
    GL_PALETTE4_R5_G6_B5_OES =        0x8B92,
    GL_PALETTE4_RGBA4_OES =           0x8B93,
    GL_PALETTE4_RGB5_A1_OES =         0x8B94,
    GL_PALETTE8_RGB8_OES =            0x8B95,
    GL_PALETTE8_RGBA8_OES =           0x8B96,
    GL_PALETTE8_R5_G6_B5_OES =        0x8B97,
    GL_PALETTE8_RGBA4_OES =           0x8B98,
    GL_PALETTE8_RGB5_A1_OES =         0x8B99,
};

/* VertexPointerType */
/*      GL_SHORT */
/*      GL_FLOAT */
/*      GL_FIXED */
/*      GL_BYTE */

enum LightName {
    GL_LIGHT0 =                       0x4000,
    GL_LIGHT1 =                       0x4001,
    GL_LIGHT2 =                       0x4002,
    GL_LIGHT3 =                       0x4003,
    GL_LIGHT4 =                       0x4004,
    GL_LIGHT5 =                       0x4005,
    GL_LIGHT6 =                       0x4006,
    GL_LIGHT7 =                       0x4007,
};


void glActiveTexture(GLenum texture);
void glAlphaFunc(GLenum func, GLclampf ref);
void glBindTexture(GLenum target, GLuint texture);
void glBlendFunc(GLenum sfactor, GLenum dfactor);
void glClear(GLbitfield mask);
void glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
void glClearDepthf(GLclampf depth);
void glClearStencil(GLint s);
void glClientActiveTexture(GLenum texture);
void glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
void glColorMask(bool red, bool green, bool blue, bool alpha);
void glColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
void glCompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data);
void glCompressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data);
void glCopyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
void glCopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
void glCullFace(GLenum mode);
void glDeleteTextures(GLsizei n, const GLuint *textures);
void glDepthFunc(GLenum func);
void glDepthMask(bool flag);
void glDepthRangef(GLclampf zNear, GLclampf zFar);
void glDisable(GLenum cap);
void glDisableClientState(GLenum array);
void glDrawArrays(GLenum mode, GLint first, GLsizei count);
void glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);
void glEnable(GLenum cap);
void glEnableClientState(GLenum array);
void glFinish(void);
void glFlush(void);
void glFogf(GLenum pname, GLfloat param);
void glFogfv(GLenum pname, const GLfloat *params);
void glFrontFace(GLenum mode);
void glFrustumf(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar);
void glGenTextures(GLsizei n, GLuint *textures);
GLenum glGetError(void);
void glGetIntegerv(GLenum pname, GLint *params);
const GLubyte * glGetString(GLenum name);
void glHint(GLenum target, GLenum mode);
void glLightModelf(GLenum pname, GLfloat param);
void glLightModelfv(GLenum pname, const GLfloat *params);
void glLightf(GLenum light, GLenum pname, GLfloat param);
void glLightfv(GLenum light, GLenum pname, const GLfloat *params);
void glLineWidth(GLfloat width);
void glLoadIdentity(void);
void glLoadMatrixf(const GLfloat *m);
void glLogicOp(GLenum opcode);
void glMaterialf(GLenum face, GLenum pname, GLfloat param);
void glMaterialfv(GLenum face, GLenum pname, const GLfloat *params);
void glMatrixMode(GLenum mode);
void glMultMatrixf(const GLfloat *m);
void glMultiTexCoord4f(GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
void glNormal3f(GLfloat nx, GLfloat ny, GLfloat nz);
void glNormalPointer(GLenum type, GLsizei stride, const GLvoid *pointer);
void glOrthof(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar);
void glPixelStorei(GLenum pname, GLint param);
void glPointSize(GLfloat size);
void glPolygonOffset(GLfloat factor, GLfloat units);
void glPopMatrix(void);
void glPushMatrix(void);
void glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels);
void glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
void glRotatex(GLfixed angle, GLfixed x, GLfixed y, GLfixed z);
void glSampleCoverage(GLclampf value, bool invert);
void glScalef(GLfloat x, GLfloat y, GLfloat z);
void glScissor(GLint x, GLint y, GLsizei width, GLsizei height);
void glShadeModel(GLenum mode);
void glStencilFunc(GLenum func, GLint ref, GLuint mask);
void glStencilMask(GLuint mask);
void glStencilOp(GLenum fail, GLenum zfail, GLenum zpass);
void glTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
void glTexEnvf(GLenum target, GLenum pname, GLfloat param);
void glTexEnvfv(GLenum target, GLenum pname, const GLfloat *params);
void glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
void glTexParameterf(GLenum target, GLenum pname, GLfloat param);
void glTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
void glTranslatef(GLfloat x, GLfloat y, GLfloat z);
void glVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
void glViewport(GLint x, GLint y, GLsizei width, GLsizei height);
}

// -----------------------------------------------------------------------
// LICENSE INFORMATION FOLLOWS
// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
// Portions of mingl are based on the PixelToaster Framebuffer Library by
// Glenn Fielder. Its original license follows:
/*
 	PixelToaster Framebuffer Library.

	Copyright © 2004-2007 Glenn Fiedler

	This software is provided 'as-is', without any express or implied
	warranty. In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	
	3. This notice may not be removed or altered from any source distribution.

	Glenn Fiedler
	gaffer@gaffer.org
*/
// -----------------------------------------------------------------------

// -----------------------------------------------------------------------
// Portions of mingl are based on TinyGL by Fabrice Bellard. Its original
// license follows:
/*
    (C) 1997-1998 Fabrice Bellard

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
        claim that you wrote the original software. If you use this software
        in a product, an acknowledgment in the product and its documentation 
        *is* required.
    2. Altered source versions must be plainly marked as such, and must not be
        misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.

    If you redistribute modified sources, I would appreciate that you
    include in the files history information documenting your changes.
*/
// -----------------------------------------------------------------------

// -----------------------------------------------------------------------
// The remainder of mingl is written by Scott Graham. It is also licensed
// under a 'zlib'-style license, which follows:
/*
    Copyright (C) 2009 Scott Graham

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
        claim that you wrote the original software. If you use this software
        in a product, an acknowledgment in the product documentation would be
        appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be
        misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.

    Scott Graham scott.mingl@h4ck3r.net
*/
// -----------------------------------------------------------------------
