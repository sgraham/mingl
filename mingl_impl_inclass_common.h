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
            //printf("OOZ:  %f, %f, %f\n", OOZ1, OOZ2, OOZ3);
            UOZ1 = v1.tex.X() * OOZ1;
            UOZ2 = v2.tex.X() * OOZ2;
            UOZ3 = v3.tex.X() * OOZ3;
            VOZ1 = v1.tex.Y() * OOZ1;
            VOZ2 = v2.tex.Y() * OOZ2;
            VOZ3 = v3.tex.Y() * OOZ3;

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

            //printf("gradients:\n");
            //printf("  %f, %f, %f\n", OOZ1, OOZ2, OOZ3);
            //printf("  %f, %f, %f\n", UOZ1, UOZ2, UOZ3);
            //printf("  %f, %f, %f\n", VOZ1, VOZ2, VOZ3);
            //printf("  %f, %f\n", dOOZdX, dOOZdY);
            //printf("  %f, %f\n", dUOZdX, dUOZdY);
            //printf("  %f, %f\n", dVOZdX, dVOZdY);
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
            //printf("ooz, oozstep: %f, %f\n", OOZ, OOZStep);
            //printf("uoz, uozstep: %f, %f\n", UOZ, UOZStep);
            //printf("voz, vozstep: %f, %f\n", VOZ, VOZStep);
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
        CurTexId = 1;
        CurColor = Vec4(1.f, 1.f, 1.f, 1.f);
        CurNormal = Vec4(0.f, 0.f, 1.f, 0.f);
        for (int i = 0; i < TU_NumTextureUnits; ++i)
            CurTexCoords[i] = Vec4(0.f, 0.f, 0.f, 1.f);
        CurActiveTexture = 0;
        CurClientActiveTexture = 0;
        Texture2DEnabled = false;
    }

    MatrixModeE MatrixMode;
    Mat44 MatrixStack[MM_NumMatrixModes][MaxMatrixStackDepth] __attribute__((aligned(16)));
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

    struct VertexState
    {
        Vec4 Color;
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
    ArrayState TexCoordArray[TU_NumTextureUnits];
    int CurActiveTexture;
    int CurClientActiveTexture;

    bool Texture2DEnabled;
    Texture* CurrentTexture;
    std::map<GLuint, Texture*> AllTextures;
    int CurTexId;

    Vec4 CurColor;
    Vec4 CurNormal;
    Vec4 CurTexCoords[TU_NumTextureUnits];

    CompareFuncE AlphaCompareFunc;
    float AlphaCompareValue;

    CompareFuncE DepthCompareFunc;

    CompareFuncE StencilCompareFunc;
    GLint StencilCompareValue;
    GLuint StencilCompareMask;
};

inline void drawScanLine(const Gradients& grads, const Edge* left, const Edge* right)
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
    float uoz = left->UOZ + xprestep * grads.dUOZdX;
    float voz = left->VOZ + xprestep * grads.dVOZdX;

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
        uoz += grads.dUOZdX;
        voz += grads.dVOZdX;
    }
}

inline void renderTriangle(const TriVert* V1, const TriVert* V2, const TriVert* V3)
{
    // v1 top, v2 middle, v3 bottom
    if (V1->pos.Y() > V3->pos.Y()) swap(V1, V3);
    if (V2->pos.Y() > V3->pos.Y()) swap(V2, V3);
    if (V1->pos.Y() > V2->pos.Y()) swap(V1, V2);
    /*
    printf("(%f, %f), (%f, %f), (%f, %f)\n",
            (float)V1->pos.X(), (float)V1->pos.Y(),
            (float)V2->pos.X(), (float)V2->pos.Y(),
            (float)V3->pos.X(), (float)V3->pos.Y());
    */

    TriVert v1 = *V1;
    TriVert v2 = *V2;
    TriVert v3 = *V3;

    Gradients grads(v1, v2, v3);
    Edge edge12(grads, v1, v2, grads.OOZ1, grads.UOZ1, grads.VOZ1);
    Edge edge13(grads, v1, v3, grads.OOZ1, grads.UOZ1, grads.VOZ1);
    Edge edge23(grads, v2, v3, grads.OOZ2, grads.UOZ2, grads.VOZ2);

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
        drawScanLine(grads, left1, right1);
        edge12.Step(); edge13.Step();
        height -= 1;
    }

    height = edge23.Height;
    while (height)
    {
        drawScanLine(grads, left2, right2);
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


// these are nicer implemented near their api functions
void enableOrDisable(GLenum cap, bool val);
bool compareFuncAssign(GLenum func, CompareFuncE& into);

