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

        Funcs.Tri = &MinGL::renderTriangleGouraud;
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

    MatrixModeE MatrixMode;
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

    CompareFuncE DepthCompareFunc;

    CompareFuncE StencilCompareFunc;
    GLint StencilCompareValue;
    GLuint StencilCompareMask;

    struct ViewportState
    {
        int Ox;
        int Oy;
        int Px2;
        int Py2;
        float F;
        float N;
    };
    ViewportState Viewport;

};

class Gradients
{
    public:
        Gradients(const ProcVert& v1, const ProcVert& v2, const ProcVert& v3)
        {
            const float dx23 = v2.pos.X() - v3.pos.X();
            const float dy13 = v1.pos.Y() - v3.pos.Y();
            const float dx13 = v1.pos.X() - v3.pos.X();
            const float dy23 = v2.pos.Y() - v3.pos.Y();
            const float oodX = 1.f / ((dx23 * dy13) - (dx13 * dy23));

            const float oodY = -oodX;
            //printf("oodX, oodY: %f, %f\n", oodX, oodY);

            OOZ1 = 1.f / v1.pos.Z();
            OOZ2 = 1.f / v2.pos.Z();
            OOZ3 = 1.f / v3.pos.Z();

            #define MINGL_CALC_COZ(coz_name, c_name)                        \
                coz_name##1 = v1 . c_name * OOZ1;                           \
                coz_name##2 = v2 . c_name * OOZ2;                           \
                coz_name##3 = v3 . c_name * OOZ3;                           \
                const float d##coz_name##13 = coz_name##1 - coz_name##3;    \
                const float d##coz_name##23 = coz_name##2 - coz_name##3;

                MINGL_CALC_COZ(U0OZ, tex[0].X())
                MINGL_CALC_COZ(V0OZ, tex[0].Y())
                MINGL_CALC_COZ(U1OZ, tex[1].X())
                MINGL_CALC_COZ(V1OZ, tex[1].Y())
                MINGL_CALC_COZ(ROZ, color.X())
                MINGL_CALC_COZ(GOZ, color.Y())
                MINGL_CALC_COZ(BOZ, color.Z())
                MINGL_CALC_COZ(AOZ, color.W())
            #undef MINGL_CALC_COZ

            const float dOOZ13 = OOZ1 - OOZ3;
            const float dOOZ23 = OOZ2 - OOZ3;

            #define MINGL_CALC_DCOZ_BY_DXY(coz_name)                                                \
                d##coz_name##dX = oodX * ((d##coz_name##23 * dy13) - (d##coz_name##13 * dy23));     \
                d##coz_name##dY = oodY * ((d##coz_name##23 * dx13) - (d##coz_name##13 * dx23));     \

                MINGL_CALC_DCOZ_BY_DXY(OOZ);
                MINGL_CALC_DCOZ_BY_DXY(U0OZ);
                MINGL_CALC_DCOZ_BY_DXY(V0OZ);
                MINGL_CALC_DCOZ_BY_DXY(U1OZ);
                MINGL_CALC_DCOZ_BY_DXY(V1OZ);
                MINGL_CALC_DCOZ_BY_DXY(ROZ);
                MINGL_CALC_DCOZ_BY_DXY(GOZ);
                MINGL_CALC_DCOZ_BY_DXY(BOZ);
                MINGL_CALC_DCOZ_BY_DXY(AOZ);
            #undef MINGL_CALC_DCOZ_BY_DXY

            //printf("gradients:\n");
            //printf("  %f, %f, %f\n", OOZ1, OOZ2, OOZ3);
            //printf("  %f, %f, %f\n", UOZ1, UOZ2, UOZ3);
            //printf("  %f, %f, %f\n", VOZ1, VOZ2, VOZ3);
            //printf("  %f, %f\n", dOOZdX, dOOZdY);
            //printf("  %f, %f\n", dUOZdX, dUOZdY);
            //printf("  %f, %f\n", dVOZdX, dVOZdY);
        }

        #define MINGL_DECLARE_COZ(coz_name)                 \
            float coz_name##1, coz_name##2, coz_name##3;    \
            float d##coz_name##dX, d##coz_name##dY;

            MINGL_DECLARE_COZ(OOZ); // not really a C-over-Z, but same fields required

            MINGL_DECLARE_COZ(U0OZ);
            MINGL_DECLARE_COZ(V0OZ);
            MINGL_DECLARE_COZ(U1OZ);
            MINGL_DECLARE_COZ(V1OZ);

            MINGL_DECLARE_COZ(ROZ);
            MINGL_DECLARE_COZ(GOZ);
            MINGL_DECLARE_COZ(BOZ);
            MINGL_DECLARE_COZ(AOZ);
        #undef MINGL_DECLARE_COZ
};

class Edge
{
    public:
        Edge(const Gradients& grads, const ProcVert& v1, const ProcVert& v2, bool topIsV1)
        {
            //printf("Edge: (%f, %f), (%f, %f)\n", (float)v1.pos.X(), (float)v1.pos.Y(), (float)v2.pos.X(), (float)v2.pos.Y());
            Y = ceil((float)v1.pos.Y());
            //printf("  Y: %d\n", Y);
            int yend = ceil((float)v2.pos.Y());
            //printf("  yend: %d\n", yend);
            Height = yend - Y;
            //printf("  height: %d\n", Height);

            const float yprestep = Y - (float)VecFloat(v1.pos.Y());
            const float realWidth = v2.pos.X() - v1.pos.X();
            const float realHeight = v2.pos.Y() - v1.pos.Y();
            //printf("  realwidth/height: %f, %f\n", realWidth, realHeight);

            X = (realWidth * yprestep) / realHeight + v1.pos.X();
            XStep = realWidth / realHeight;
            //printf("  x, xstep: %f, %f\n", X, XStep);

            const float xprestep = X - v1.pos.X();
            //printf("xprestep: %f\n", xprestep);
            //printf("yprestep: %f\n", yprestep);

            #define MINGL_CALC_COZ(coz_name)                                                                            \
                float top##coz_name = topIsV1 ? grads.coz_name##1 : grads.coz_name##2;                                  \
                coz_name = top##coz_name + (yprestep * grads.d##coz_name##dY) + (xprestep * grads.d##coz_name##dX);     \
                coz_name##Step = XStep * grads.d##coz_name##dX + grads.d##coz_name##dY;

                MINGL_CALC_COZ(OOZ);
                MINGL_CALC_COZ(U0OZ);
                MINGL_CALC_COZ(V0OZ);
                MINGL_CALC_COZ(U1OZ);
                MINGL_CALC_COZ(V1OZ);
                MINGL_CALC_COZ(ROZ);
                MINGL_CALC_COZ(GOZ);
                MINGL_CALC_COZ(BOZ);
                MINGL_CALC_COZ(AOZ);
            #undef MINGL_CALC_COZ
        }

        void Step()
        {
            X += XStep;
            Y += 1;
            Height -= 1;
            #define MINGL_STEP_COZ(coz_name) coz_name += coz_name##Step;
                MINGL_STEP_COZ(OOZ);
                MINGL_STEP_COZ(U0OZ);
                MINGL_STEP_COZ(V0OZ);
                MINGL_STEP_COZ(U1OZ);
                MINGL_STEP_COZ(V1OZ);
                MINGL_STEP_COZ(ROZ);
                MINGL_STEP_COZ(GOZ);
                MINGL_STEP_COZ(BOZ);
                MINGL_STEP_COZ(AOZ);
            #undef MINGL_STEP_COZ
        }

        float X, XStep;
        int Y, Height;

        #define MINGL_DECLARE_COZ(coz_name) float coz_name, coz_name##Step;
            MINGL_DECLARE_COZ(OOZ);
            MINGL_DECLARE_COZ(U0OZ);
            MINGL_DECLARE_COZ(V0OZ);
            MINGL_DECLARE_COZ(U1OZ);
            MINGL_DECLARE_COZ(V1OZ);
            MINGL_DECLARE_COZ(ROZ);
            MINGL_DECLARE_COZ(GOZ);
            MINGL_DECLARE_COZ(BOZ);
            MINGL_DECLARE_COZ(AOZ);
        #undef MINGL_DECLARE_COZ
};

// todo; macros/genericize this stuff once it's clear what else needs to happen

inline void drawScanLineGouraud(const Gradients& grads, const Edge* left, const Edge* right)
{
    const int xstart = ceil(left->X);
    const float xprestep = xstart - left->X;

    GLuint* dest = ctx.Buf.C + left->Y * ctx.Buf.Stride + xstart;
    int width = ceil(right->X) - xstart;

    float ooz = left->OOZ + xprestep * grads.dOOZdX;
    float roz = left->ROZ + xprestep * grads.dROZdX;
    float goz = left->GOZ + xprestep * grads.dGOZdX;
    float boz = left->BOZ + xprestep * grads.dBOZdX;
    float aoz = left->AOZ + xprestep * grads.dAOZdX;

    //printf("width: %d, xstart: %d, lX: %f, rX: %f\n", width, xstart, left->X, right->X);
    //printf("ooz: %f, uoz: %f, voz: %f\n", ooz, uoz, voz);
    while (width-- > 0)
    {
        float z = 1 / ooz;
        float fr = (roz * z);
        float fg = (goz * z);
        float fb = (boz * z);
        float fa = (aoz * z);
        *dest++ += floatColorToUint(fr, fg, fb, fa);

        ooz += grads.dOOZdX;
        roz += grads.dROZdX;
        goz += grads.dGOZdX;
        boz += grads.dBOZdX;
        aoz += grads.dAOZdX;
    }
}

inline void renderTriangleGouraud(const ProcVert* V1, const ProcVert* V2, const ProcVert* V3)
{
    // v1 top, v2 middle, v3 bottom
    if (V1->pos.Y() > V3->pos.Y()) swap(V1, V3);
    if (V2->pos.Y() > V3->pos.Y()) swap(V2, V3);
    if (V1->pos.Y() > V2->pos.Y()) swap(V1, V2);

    //printf("(%f, %f), (%f, %f), (%f, %f)\n",
            //(float)V1->pos.X(), (float)V1->pos.Y(),
            //(float)V2->pos.X(), (float)V2->pos.Y(),
            //(float)V3->pos.X(), (float)V3->pos.Y());

    const ProcVert& v1 = *V1;
    const ProcVert& v2 = *V2;
    const ProcVert& v3 = *V3;

    Gradients grads(v1, v2, v3);
    Edge edge12(grads, v1, v2, true);
    Edge edge13(grads, v1, v3, true);
    Edge edge23(grads, v2, v3, false);

    // figure out where v2.x is on long edge
    const float xOnLong = (((v2.pos.Y() - v1.pos.Y()) * (v3.pos.X() - v1.pos.X())) / (v3.pos.Y() - v1.pos.Y())) + v1.pos.X();
    //printf("xOnLong: %f\n", xOnLong);

    //printf("scanning in %c %c %c\n", v1.debug, v2.debug, v3.debug);
    const Edge* left1;
    const Edge* right1;
    const Edge* left2;
    const Edge* right2;
    if (v2.pos.X() > xOnLong)
    {
        left1 = left2 = &edge13;
        right1 = &edge12;
        right2 = &edge23;
        //printf("here\n");
    }
    else
    {
        //printf("else\n");
        left1 = &edge12;
        left2 = &edge23;
        right1 = right2 = &edge13;
    }
    int height = edge12.Height;
    while (height)
    {
        drawScanLineGouraud(grads, left1, right1);
        edge12.Step(); edge13.Step();
        height -= 1;
    }

    height = edge23.Height;
    while (height)
    {
        drawScanLineGouraud(grads, left2, right2);
        edge23.Step(); edge13.Step();
        height -= 1;
    }
    //printf("pixels: %d\n", pixelCount);
}

inline void drawScanLineTex(const Gradients& grads, const Edge* left, const Edge* right)
{
    const int xstart = ceil(left->X);
    const float xprestep = xstart - left->X;

    GLuint* dest = ctx.Buf.C + left->Y * ctx.Buf.Stride + xstart;
    //GLuint* start = dest;
    const GLuint* texdata = ctx.CurrentTexture->C;
    const int texWidth = ctx.CurrentTexture->Width;
    const int texStride = texWidth;
    //const int texHeight = ctx.CurrentTexture->Height;
    int width = ceil(right->X) - xstart;

    float ooz = left->OOZ + xprestep * grads.dOOZdX;
    float uoz = left->U0OZ + xprestep * grads.dU0OZdX;
    float voz = left->V0OZ + xprestep * grads.dV0OZdX;

    //printf("width: %d, xstart: %d, lX: %f, rX: %f\n", width, xstart, left->X, right->X);
    //printf("ooz: %f, uoz: %f, voz: %f\n", ooz, uoz, voz);
    while (width-- > 0)
    {
        float z = 1 / ooz;
        float fu = (uoz * z + 0.5f);
        float fv = (voz * z + 0.5f);
        int u = (int)fu;// * texWidth;
        int v = (int)fv;// * texHeight;
        //printf("u, v: %d %d\n", u, v);
        *dest++ += *(texdata + u + (v * texStride));

        ooz += grads.dOOZdX;
        uoz += grads.dU0OZdX;
        voz += grads.dV0OZdX;
    }
}

inline void renderTriangleTex(const ProcVert* V1, const ProcVert* V2, const ProcVert* V3)
{
    // v1 top, v2 middle, v3 bottom
    if (V1->pos.Y() > V3->pos.Y()) swap(V1, V3);
    if (V2->pos.Y() > V3->pos.Y()) swap(V2, V3);
    if (V1->pos.Y() > V2->pos.Y()) swap(V1, V2);

    //printf("(%f, %f), (%f, %f), (%f, %f)\n",
            //(float)V1->pos.X(), (float)V1->pos.Y(),
            //(float)V2->pos.X(), (float)V2->pos.Y(),
            //(float)V3->pos.X(), (float)V3->pos.Y());

    const ProcVert& v1 = *V1;
    const ProcVert& v2 = *V2;
    const ProcVert& v3 = *V3;

    Gradients grads(v1, v2, v3);
    Edge edge12(grads, v1, v2, true);
    Edge edge13(grads, v1, v3, true);
    Edge edge23(grads, v2, v3, false);

    // figure out where v2.x is on long edge
    const float xOnLong = (((v2.pos.Y() - v1.pos.Y()) * (v3.pos.X() - v1.pos.X())) / (v3.pos.Y() - v1.pos.Y())) + v1.pos.X();
    //printf("xOnLong: %f\n", xOnLong);

    //printf("scanning in %c %c %c\n", v1.debug, v2.debug, v3.debug);
    const Edge* left1;
    const Edge* right1;
    const Edge* left2;
    const Edge* right2;
    if (v2.pos.X() > xOnLong)
    {
        left1 = left2 = &edge13;
        right1 = &edge12;
        right2 = &edge23;
        //printf("here\n");
    }
    else
    {
        //printf("else\n");
        left1 = &edge12;
        left2 = &edge23;
        right1 = right2 = &edge13;
    }
    int height = edge12.Height;
    while (height)
    {
        drawScanLineTex(grads, left1, right1);
        edge12.Step(); edge13.Step();
        height -= 1;
    }

    height = edge23.Height;
    while (height)
    {
        drawScanLineTex(grads, left2, right2);
        edge23.Step(); edge13.Step();
        height -= 1;
    }
    //printf("pixels: %d\n", pixelCount);
}

inline GLuint floatColorToUint(float r, float g, float b, float a)
{
    // todo; *256 + biasing and clamping
    GLuint ri = (GLuint)(r * 255.f);
    GLuint gi = (GLuint)(g * 255.f);
    GLuint bi = (GLuint)(b * 255.f);
    GLuint ai = (GLuint)(a * 255.f);
    return (bi << 24) | (gi << 16) | (ri << 8) | ai;
}

// debugging
void pvec(const char* id, Vec4::Arg v)
{
    printf("%s: (%f, %f, %f, %f)\n", id, (float)v.X(), (float)v.Y(), (float)v.Z(), (float)v.W());
}

// these are nicer implemented near their api functions
void enableOrDisable(GLenum cap, bool val);
void enableOrDisableCS(GLenum array, bool val);
bool compareFuncAssign(GLenum func, CompareFuncE& into);
Vec4 getVec4FromArrayPtr(const float* p, const int size);
void advanceArrayPtr(const float*& p, const DisplayImplContext::ArrayState& as);
void processVertTriangles();
void processVertTristripSetup();
void processVertTristrip();
void transformCurrentVert();
