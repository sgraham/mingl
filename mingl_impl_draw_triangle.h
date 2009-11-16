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

// todo; fnptr instead?
inline bool passesCompareFunc(float src, float dest, CompareFuncE cf)
{
    switch (cf)
    {
        case CF_Never: return false;
        case CF_Less: return src < dest;
        case CF_Equal: return src == dest;
        case CF_LessEqual: return src <= dest;
        case CF_Greater: return src > dest;
        case CF_NotEqual: return src != dest;
        case CF_GreaterEqual: return src >= dest;
        case CF_Always: return true;
        default: MINGL_ASSERT(0);
    }
    MINGL_ASSERT(false && "not reached");
}

inline bool passesDepthFunc(float srcz, float destz)
{
    if (!ctx.DepthTestEnabled) return true;
    bool ret = passesCompareFunc(srcz, destz, ctx.DepthCompareFunc);
    return ret;
}

inline void drawScanLineGeneric(const Gradients& grads, const Edge* left, const Edge* right)
{
    const int xstart = ceil(left->X);
    const float xprestep = xstart - left->X;

    GLuint* dest = ctx.Buf.C + left->Y * ctx.Buf.Stride + xstart;
    float* zdest = ctx.Buf.Z + left->Y * ctx.Buf.Stride + xstart;
    //printf("zoff: %d %d\n", left->Y, xstart);
    int width = ceil(right->X) - xstart;

    const GLuint* texdata;
    int texWidth, texStride, texHeight;
    if (ctx.Texture2DEnabled)
    {
        texdata = ctx.CurrentTexture->C;
        texWidth = ctx.CurrentTexture->Width;
        texStride = texWidth;
        texHeight = ctx.CurrentTexture->Height;
    }

    float ooz = left->OOZ + xprestep * grads.dOOZdX;
    float uoz = left->U0OZ + xprestep * grads.dU0OZdX;
    float voz = left->V0OZ + xprestep * grads.dV0OZdX;
    float roz = left->ROZ + xprestep * grads.dROZdX;
    float goz = left->GOZ + xprestep * grads.dGOZdX;
    float boz = left->BOZ + xprestep * grads.dBOZdX;
    float aoz = left->AOZ + xprestep * grads.dAOZdX;

    //printf("width: %d, xstart: %d, lX: %f, rX: %f\n", width, xstart, left->X, right->X);
    //printf("ooz: %f, uoz: %f, voz: %f\n", ooz, uoz, voz);
    while (width-- > 0)
    {
        float z = 1 / ooz;
        unsigned int fragColor = 0;
        if (ctx.Texture2DEnabled)
        {
            float fu = (uoz * z + 0.5f);
            float fv = (voz * z + 0.5f);
            int u = (int)(fu);// * texWidth - 0.5f);
            int v = (int)(fv);// * texHeight - 0.5f);
            //printf("u, v: %d %d\n", u, v);
            fragColor = *(texdata + u + (v * texStride));

            uoz += grads.dU0OZdX;
            voz += grads.dV0OZdX;
        }
        else
        {
            float z = 1 / ooz;
            float fr = (roz * z);
            float fg = (goz * z);
            float fb = (boz * z);
            float fa = (aoz * z);
            fragColor = floatColorToUint(fr, fg, fb, fa);

            roz += grads.dROZdX;
            goz += grads.dGOZdX;
            boz += grads.dBOZdX;
            aoz += grads.dAOZdX;
        }
        ooz += grads.dOOZdX;

        if (passesDepthFunc(z, *zdest))
        {
            *dest = fragColor;
            *zdest = z;
        }
        dest += 1;
        zdest += 1;
    }
}

// calculate (2x) the signed area of the triangle (ie. cross product).
// if it's <= 0 then this is back facing, else front facing.
// the FrontFace api function can swap this behaviour (GL_CW means to negate area)
// then, depending on the setting of CullFace, we return whether this face
// should be dropped from rasterization.
// all calculations are done in window space (i.e. what we have here).
inline bool isCulled(const Vec4& v1, const Vec4& v2, const Vec4& v3)
{
    if (!ctx.CullFaceEnabled) return false;
    if (ctx.CullFace == CF_FrontAndBack) return true;

    VecFloat area = (v1.X()*v2.Y() - v2.X()*v1.Y())
        + (v2.X()*v3.Y() - v3.X()*v2.Y())
        + (v3.X()*v1.Y() - v1.X()*v3.Y());

    if (ctx.FrontFace == FF_Clockwise) area = -area;

    bool areaLEZero = area <= 0.f;
    return ((areaLEZero && ctx.CullFace == CF_Back)
            || (!areaLEZero && ctx.CullFace == CF_Front));
}

// does runtime switching on all renderstate (mostly at scanline level). once
// more of it's implemented, pull out into compile-time generated variations.
inline void renderTriangleGeneric(const ProcVert* V1, const ProcVert* V2, const ProcVert* V3)
{
    if (isCulled(V1->pos, V2->pos, V3->pos)) return;

    // v1 top, v2 middle, v3 bottom
    if (V1->pos.Y() > V3->pos.Y()) swap(V1, V3);
    if (V2->pos.Y() > V3->pos.Y()) swap(V2, V3);
    if (V1->pos.Y() > V2->pos.Y()) swap(V1, V2);

    //printf("(%f, %f), (%f, %f), (%f, %f)\n",
            //(float)V1->pos.X(), (float)V1->pos.Y(),
            //(float)V2->pos.X(), (float)V2->pos.Y(),
            //(float)V3->pos.X(), (float)V3->pos.Y());

    ProcVert v1 = *V1;
    ProcVert v2 = *V2;
    ProcVert v3 = *V3;

    if (ctx.Texture2DEnabled)
    {
        int texWidth = ctx.CurrentTexture->Width;
        int texHeight = ctx.CurrentTexture->Height;
        v1.tex[0] = Vec4(v1.tex[0].X() * texWidth - 0.5f, v1.tex[0].Y() * texHeight - 0.5f, 0.f, 0.f); // todo; t, r
        v2.tex[0] = Vec4(v2.tex[0].X() * texWidth - 0.5f, v2.tex[0].Y() * texHeight - 0.5f, 0.f, 0.f);
        v3.tex[0] = Vec4(v3.tex[0].X() * texWidth - 0.5f, v3.tex[0].Y() * texHeight - 0.5f, 0.f, 0.f);
        // todo; tex1
    }
    //v1.pos.SetY(-(float)v1.pos.Y());
    //v2.pos.SetY(-(float)v2.pos.Y());
    //v3.pos.SetY(-(float)v3.pos.Y());

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
        drawScanLineGeneric(grads, left1, right1);
        edge12.Step(); edge13.Step();
        height -= 1;
    }

    height = edge23.Height;
    while (height)
    {
        drawScanLineGeneric(grads, left2, right2);
        edge23.Step(); edge13.Step();
        height -= 1;
    }
    //printf("pixels: %d\n", pixelCount);
}

