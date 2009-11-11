#ifndef INCLUDED_mingl_H
#define INCLUDED_mingl_H

// -----------------------------------------------------------------------
// Please see license and copyright information at the end of this file.
// -----------------------------------------------------------------------

// -----------------------------------------------------------------------
// "mingl" is a minimal graphics/demo library.
//
// Its goals are definitely not to be fast, take advantage of fancy hardware
// features, or to show off the latest neat effects.
//
// Instead, its goals are to be servicable to draw simple dianostic scenes on
// many different platforms, while being distributed as merely one C++ header
// file, not requiring any configuration, and not having any external
// dependencies other than the standard platform libraries.
//
// The API of mingls graphics is modelled after OpenGL ES 1.0. mingl does
// expect to have a floating point unit though, and drops all the
// fixed-related API functions and types from the ES spec. While MinGL is
// definitely _not_ a certified implementation of the OpenGL ES specification,
// its goal is to conform to that specification nonetheless (again, minus the
// fixed point functions and types).
//
// In addition, there is a simple interface for demo-type behaviour of setting
// up a rendering context, basic user input, and basic camera controls. It is
// completely ignorable though, if you're only interested in the 'GL' part.
//
// A minimal application is simply:
/*

    #include "mingl.h"
    using namespace mingl;

    int main()
    {
        MinGL gl;
        while (gl.IsOpen())
        {
            gl.ClearColor(1.f, 0.f, 0.f, 0.f);
            gl.Clear(GL_COLOR_BUFFER_BIT);
            gl.SwapBuffers();
        }
    }

*/
// You might also find it more pleasant to derive from MinGL and avoid the
// "gl." prefix on calling API functions.
//
// MinGL is intended to work on Windows (DirectDraw), Linux (X11), OSX,
// Xbox360, PS3, and Wii.
//
// Platform-specific notes:
//
// - Under X11, you will need to add "-lX11" to your link line. 
//
// If you know of a way to avoid any of these platform-specific configuration
// requirements, please let me know.
//
// Scott Graham <scott.mingl@h4ck3r.net>


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
}

#include <new> // we need to override operator new/delete to ensure alignment
#include <stdint.h>
#include "mingl_impl_setup.h"

namespace mingl
{

class Key
{
    public:
        enum Code
        {
            Enter          = '\n',      ///< enter key
            Backspace      = '\b',      ///< backspace key
            Tab            = '\t',      ///< tab key
            Cancel         = 0x03,      ///< cancel key
            Clear          = 0x0C,      ///< clear key
            Shift          = 0x10,      ///< shift key
            Control        = 0x11,      ///< control key
            Alt            = 0x12,      ///< alt key
            Pause          = 0x13,      ///< pause key
            CapsLock       = 0x14,      ///< capslock key
            Escape         = 0x1B,      ///< escape key
            Space          = 0x20,      ///< space key
            PageUp         = 0x21,      ///< page up key
            PageDown       = 0x22,      ///< page down key
            End            = 0x23,      ///< end key
            Home           = 0x24,      ///< home key
            Left           = 0x25,      ///< left key
            Up             = 0x26,      ///< up arrow key
            Right          = 0x27,      ///< right arrow key
            Down           = 0x28,      ///< down arrow key
            Comma          = 0x2C,      ///< comma key ','
            Period         = 0x2E,      ///< period key '.'
            Slash          = 0x2F,      ///< slash key '/'
            Zero           = 0x30,      ///< zero key
            One            = 0x31,      ///< one key
            Two            = 0x32,      ///< two key
            Three          = 0x33,      ///< three key
            Four           = 0x34,      ///< four key
            Five           = 0x35,      ///< five key
            Six            = 0x36,      ///< six key
            Seven          = 0x37,      ///< seven key
            Eight          = 0x38,      ///< eight key
            Nine           = 0x39,      ///< nine key
            SemiColon      = 0x3B,      ///< semicolon key ';'
            Equals         = 0x3D,      ///< equals key '='
            A              = 0x41,      ///< a key
            B              = 0x42,      ///< b key
            C              = 0x43,      ///< c key
            D              = 0x44,      ///< d key
            E              = 0x45,      ///< e key
            F              = 0x46,      ///< f key
            G              = 0x47,      ///< g key
            H              = 0x48,      ///< h key
            I              = 0x49,      ///< i key
            J              = 0x4A,      ///< j key
            K              = 0x4B,      ///< k key
            L              = 0x4C,      ///< l key
            M              = 0x4D,      ///< m key
            N              = 0x4E,      ///< n key
            O              = 0x4F,      ///< o key
            P              = 0x50,      ///< p key
            Q              = 0x51,      ///< q key
            R              = 0x52,      ///< r key
            S              = 0x53,      ///< s key
            T              = 0x54,      ///< t key
            U              = 0x55,      ///< u key
            V              = 0x56,      ///< v key
            W              = 0x57,      ///< w key
            X              = 0x58,      ///< x key
            Y              = 0x59,      ///< y key
            Z              = 0x5A,      ///< z key
            OpenBracket    = 0x5B,      ///< open bracket key '['
            BackSlash      = 0x5C,      ///< back slash key '\'
            CloseBracket   = 0x5D,      ///< close bracket key ']'
            NumPad0        = 0x60,      ///< numpad 0 key
            NumPad1        = 0x61,      ///< numpad 1 key
            NumPad2        = 0x62,      ///< numpad 2 key
            NumPad3        = 0x63,      ///< numpad 3 key
            NumPad4        = 0x64,      ///< numpad 4 key
            NumPad5        = 0x65,      ///< numpad 5 key
            NumPad6        = 0x66,      ///< numpad 6 key
            NumPad7        = 0x67,      ///< numpad 7 key
            NumPad8        = 0x68,      ///< numpad 8 key
            NumPad9        = 0x69,      ///< numpad 9 key
            Multiply       = 0x6A,      ///< multiply key '*'
            Add            = 0x6B,      ///< add key '+'
            Separator      = 0x6C,      ///< separator key '-'
            Subtract       = 0x6D,      ///< subtract key '-'
            Decimal        = 0x6E,      ///< decimal key '.'
            Divide         = 0x6F,      ///< divide key '/'
            F1             = 0x70,      ///< F1 key
            F2             = 0x71,      ///< F2 key
            F3             = 0x72,      ///< F3 key
            F4             = 0x73,      ///< F4 key
            F5             = 0x74,      ///< F5 key
            F6             = 0x75,      ///< F6 key
            F7             = 0x76,      ///< F7 key
            F8             = 0x77,      ///< F8 key
            F9             = 0x78,      ///< F9 key
            F10            = 0x79,      ///< F10 key
            F11            = 0x7A,      ///< F11 key
            F12            = 0x7B,      ///< F12 key
            Delete         = 0x7F,      ///< delete key
            NumLock        = 0x90,      ///< numlock key
            ScrollLock     = 0x91,      ///< scroll lock key
            PrintScreen    = 0x9A,      ///< print screen key
            Insert         = 0x9B,      ///< insert key
            Help           = 0x9C,      ///< help key
            Meta           = 0x9D,      ///< meta key
            BackQuote      = 0xC0,      ///< backquote key
            Quote          = 0xDE,      ///< quote key
            Final          = 0x18,      ///< final key
            Convert        = 0x1C,      ///< convert key
            NonConvert     = 0x1D,      ///< non convert key
            Accept         = 0x1E,      ///< accept key
            ModeChange     = 0x1F,      ///< mode change key
            Kana           = 0x15,      ///< kana key
            Kanji          = 0x19,      ///< kanji key
            Undefined      = 0x0        ///< undefined key
        };

        Key() : code(Undefined) {}
        Key(Code code) : code(code) {}
        operator Code() const { return code; }

    private:
        Code code;
} MINGL_POST_ALIGN(16);

enum ClearBufferMask {
    GL_DEPTH_BUFFER_BIT =             0x00000100,
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

// override whatever you're interested in and call SetListener on the MinGL
// object to register for events.
class EventListener
{
    public:
        virtual ~EventListener() {}
        virtual void OnActivate() {}
        virtual void OnClose() {}
        virtual void OnKeyDown(Key key) {}
        virtual void OnKeyPressed(Key key) {}
        virtual void OnKeyUp(Key key) {}

        // should the default key handlers be called (Esc to quit, etc.)
        virtual bool DoDefaultKeyHandlers() { return true; }
};


//
// This is the main entry point.
//
class MinGL
{
    public:
        MinGL(const char* title = "mingl app", int width = 1280, int height = 720);
        virtual ~MinGL();

        bool IsOpen() const;
        void Close();
        void SwapBuffers();

        void SetListener(EventListener* listener) { mListener = listener; }

        void* operator new(size_t size);
        void operator delete(void* p);

        // ---------------------------------------------- 
        // ---------------------------------------------- 
        // ---------------------------------------------- 
        // Rendering api follows
        // ---------------------------------------------- 
        // ---------------------------------------------- 
        // ---------------------------------------------- 


        void ActiveTexture(GLenum texture);
        void AlphaFunc(GLenum func, GLclampf ref);
        void BindTexture(GLenum target, GLuint texture);
        void BlendFunc(GLenum sfactor, GLenum dfactor);
        void Clear(GLbitfield mask);
        void ClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
        void ClearDepthf(GLclampf depth);
        void ClearStencil(GLint s);
        void ClientActiveTexture(GLenum texture);
        void Color4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
        void ColorMask(bool red, bool green, bool blue, bool alpha);
        void ColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
        void CompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data);
        void CompressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data);
        void CopyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
        void CopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
        void CullFace(GLenum mode);
        void DeleteTextures(GLsizei n, const GLuint *textures);
        void DepthFunc(GLenum func);
        void DepthMask(bool flag);
        void DepthRangef(GLclampf zNear, GLclampf zFar);
        void Disable(GLenum cap);
        void DisableClientState(GLenum array);
        void DrawArrays(GLenum mode, GLint first, GLsizei count);
        void DrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);
        void Enable(GLenum cap);
        void EnableClientState(GLenum array);
        void Finish(void);
        void Flush(void);
        void Fogf(GLenum pname, GLfloat param);
        void Fogfv(GLenum pname, const GLfloat *params);
        void FrontFace(GLenum mode);
        void Frustumf(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar);
        void GenTextures(GLsizei n, GLuint *textures);
        GLenum GetError(void);
        void GetIntegerv(GLenum pname, GLint *params);
        const GLubyte * GetString(GLenum name);
        void Hint(GLenum target, GLenum mode);
        void LightModelf(GLenum pname, GLfloat param);
        void LightModelfv(GLenum pname, const GLfloat *params);
        void Lightf(GLenum light, GLenum pname, GLfloat param);
        void Lightfv(GLenum light, GLenum pname, const GLfloat *params);
        void LineWidth(GLfloat width);
        void LoadIdentity(void);
        void LoadMatrixf(const GLfloat *m);
        void LogicOp(GLenum opcode);
        void Materialf(GLenum face, GLenum pname, GLfloat param);
        void Materialfv(GLenum face, GLenum pname, const GLfloat *params);
        void MatrixMode(GLenum mode);
        void MultMatrixf(const GLfloat *m);
        void MultiTexCoord4f(GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
        void Normal3f(GLfloat nx, GLfloat ny, GLfloat nz);
        void NormalPointer(GLenum type, GLsizei stride, const GLvoid *pointer);
        void Orthof(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar);
        void PixelStorei(GLenum pname, GLint param);
        void PointSize(GLfloat size);
        void PolygonOffset(GLfloat factor, GLfloat units);
        void PopMatrix(void);
        void PushMatrix(void);
        void ReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels);
        void Rotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
        void Scalef(GLfloat x, GLfloat y, GLfloat z);
        void Scissor(GLint x, GLint y, GLsizei width, GLsizei height);
        void ShadeModel(GLenum mode);
        void StencilFunc(GLenum func, GLint ref, GLuint mask);
        void StencilMask(GLuint mask);
        void StencilOp(GLenum fail, GLenum zfail, GLenum zpass);
        void TexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
        void TexEnvf(GLenum target, GLenum pname, GLfloat param);
        void TexEnvfv(GLenum target, GLenum pname, const GLfloat *params);
        void TexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
        void TexParameterf(GLenum target, GLenum pname, GLfloat param);
        void TexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
        void Translatef(GLfloat x, GLfloat y, GLfloat z);
        void VertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
        void Viewport(GLint x, GLint y, GLsizei width, GLsizei height);

    private:
        #include "mingl_impl_internal.h"
        EventListener* mListener;
};

}

#include "mingl_impl.h"

#endif

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
// The remainder of mingl is written by Scott Graham. It is also licensed
// under a 'zlib'-style license, as follows:
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
