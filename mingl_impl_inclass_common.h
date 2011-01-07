struct ProcVert
{
    Vec4 pos;
    Vec4 normal;
    Vec4 color;
    Vec4 tex[TU_NumTextureUnits];
};

typedef void (MinGL::*VertProcFn)();
typedef void (MinGL::*TriRenderFn)(const ProcVert*, const ProcVert*, const ProcVert*);

struct DisplayImplContext
{
    DisplayImplContext()
    {
        for (int i = 0; i < MM_NumMatrixModes; ++i)
            CurMatrix[i] = &MatrixStack[i][0];
        ClearColor = 0;
        ClearDepth = 1.f;
        ClearStencil = 0;
        Error = 0;
        GenTexIdCounter = 1;
        Vert.color = Vec4(1.f, 1.f, 1.f, 1.f);
        Vert.normal = Vec4(0.f, 0.f, 1.f, 0.f);
        for (int i = 0; i < TU_NumTextureUnits; ++i)
            Vert.tex[i] = Vec4(0.f, 0.f, 0.f, 1.f);
        CurActiveTexture = 0;
        CurClientActiveTexture = 0;
        Texture2DEnabled = false;
        VertexArray.Enabled = false;
        ColorArray.Enabled = false;
        NormalArray.Enabled = false;
        for (int i = 0; i < TU_NumTextureUnits; ++i)
            TexCoordArray[i].Enabled = false;
        CullFaceEnabled = false;
        FrontFace = FF_CounterClockwise;
        CullFace = CF_Back;
        NormalizeEnabled = false;
        ColorMaterialEnabled = false;

        LightingEnabled = false;
        for (int i = 0; i < MaxNumLights; ++i)
            LightNEnabled[i] = false;

        Funcs.Tri = &MinGL::renderTriangleGeneric;
    }

    ~DisplayImplContext()
    {
        MINGL_FREE_MEMORY(Buf.C);
        MINGL_FREE_MEMORY(Buf.Z);
    }

    void Init(MinGL* self, int w, int h)
    {
        Buf.Z = reinterpret_cast<float*>(MINGL_ALLOCATE_MEMORY(sizeof(float) * w * h));
        Buf.C = reinterpret_cast<GLuint*>(MINGL_ALLOCATE_MEMORY(sizeof(GLuint) * w * h));
        Buf.Width = w;
        Buf.Stride = w;
        Buf.Height = h;

        self->DepthRange(0.f, 1.f);
        self->Viewport(0, 0, w, h);
        self->MatrixMode(GL_MODELVIEW);
        self->LoadIdentity();
        self->MatrixMode(GL_PROJECTION);
        self->LoadIdentity();
        self->MatrixMode(GL_TEXTURE);
        self->LoadIdentity();
    }

    MatrixModeE CurMatrixMode;
    Mat44 MatrixStack[MM_NumMatrixModes][MaxMatrixStackDepth];
    Mat44* CurMatrix[MM_NumMatrixModes];

    GLuint ClearColor;
    float ClearDepth;
    GLuint ClearStencil;

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

    struct ArrayState
    {
        const float* Data;
        int Size;
        int Stride;
        bool Enabled;
    };
    ArrayState VertexArray;
    ArrayState ColorArray;
    ArrayState NormalArray;
    ArrayState TexCoordArray[TU_NumTextureUnits];
    int CurActiveTexture;
    int CurClientActiveTexture;

    ProcVert Vert;

    struct RenderFunctions
    {
        TriRenderFn Tri;
    };
    RenderFunctions Funcs;

    struct TrianglePrimitiveGenerateState
    {
        ProcVert A;
        ProcVert B;
        ProcVert* Next;
        bool DidFirst;
        int Counter;
    };
    TrianglePrimitiveGenerateState TriPrimGenState;

    bool Texture2DEnabled;
    Texture* CurrentTexture;
    std::map<GLuint, Texture*> AllTextures;
    int GenTexIdCounter;

    CompareFuncE AlphaCompareFunc;
    float AlphaCompareValue;

    bool DepthTestEnabled;
    CompareFuncE DepthCompareFunc;

    CompareFuncE StencilCompareFunc;
    GLint StencilCompareValue;
    GLuint StencilCompareMask;

    bool CullFaceEnabled;
    FrontFaceE FrontFace;
    CullFaceE CullFace;

    bool NormalizeEnabled;

    bool LightingEnabled;
    bool LightNEnabled[4];

    bool ColorMaterialEnabled;

    struct ViewportState
    {
        int Ox;
        int Oy;
        int Px;
        int Py;
        float F;
        float N;
    };
    ViewportState Viewport;

};

#include "mingl_impl_draw_triangle.h"

inline GLuint floatColorToUint(float r, float g, float b, float a)
{
    // todo; *256 + biasing and clamping
    GLuint ri = (GLuint)(r * 255.f);
    GLuint gi = (GLuint)(g * 255.f);
    GLuint bi = (GLuint)(b * 255.f);
    GLuint ai = (GLuint)(a * 255.f);
    return (bi << 24) | (gi << 16) | (ri << 8) | ai;
}

// --------------------------------------------------------------------------
// Debugging functionality
// --------------------------------------------------------------------------
void pvec(const char* id, Vec4::Arg v)
{
    printf("%s: (%f, %f, %f, %f)\n", id, (float)v.X(), (float)v.Y(), (float)v.Z(), (float)v.W());
}

public:
void SaveZBuffer(const char* id, bool convertToPngWithImagemagick=true)
{
    char fngray[256];
    char fnpng[256];
    sprintf(fngray, "zbuf_%s.gray", id);
    sprintf(fnpng, "zbuf_%s.png", id);
    FILE* f = fopen(fngray, "wb");
    MINGL_ASSERT(ctx.Buf.Width == ctx.Buf.Stride);
    fwrite(ctx.Buf.Z, ctx.Buf.Width * ctx.Buf.Height * sizeof(float), 1, f);
    fclose(f);
    if (convertToPngWithImagemagick)
    {
        char buf[256];
        sprintf(buf, "convert -define quantum:format=floating-point -depth 32 -size %dx%d %s %s", ctx.Buf.Width, ctx.Buf.Height, fngray, fnpng);
        //printf("running '%s'\n", buf);
        int rc = system(buf);
        if (rc == 0)
            printf("wrote %s and converted to %s\n", fngray, fnpng);
    }
    else
    {
        printf("wrote %s\n", fngray);
    }
}
private:

// these are nicer implemented near their api functions
bool* enableOrDisablePtr(GLenum cap);
void enableOrDisableCS(GLenum array, bool val);
bool compareFuncAssign(GLenum func, CompareFuncE& into);
Vec4 getVec4FromArrayPtr(const float* p, const int size);
void advanceArrayPtr(const float*& p, const DisplayImplContext::ArrayState& as);
void processVertTriangles();
void processVertTristripSetup();
void processVertTristrip();
void transformCurrentVert();
