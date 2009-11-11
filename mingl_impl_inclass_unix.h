#ifdef MINGL_PLATFORM_UNIX

// NOTE! This file is include _into_the_body_ of the MinGL class.

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
            const float dx23 = v2.pos.x - v3.pos.x;
            const float dy13 = v1.pos.y - v3.pos.y;
            const float dx13 = v1.pos.x - v3.pos.x;
            const float dy23 = v2.pos.y - v3.pos.y;
            const float oodX = 1.f / ((dx23 * dy13) - (dx13 * dy23));

            float oodY = -oodX;

            OOZ1 = 1.f / v1.pos.z;
            OOZ2 = 1.f / v2.pos.z;
            OOZ3 = 1.f / v3.pos.z;
            UOZ1 = v1.tex.x * OOZ1;
            UOZ2 = v2.tex.x * OOZ2;
            UOZ3 = v3.tex.x * OOZ3;
            VOZ1 = v1.tex.y * OOZ1;
            VOZ2 = v2.tex.y * OOZ2;
            VOZ3 = v3.tex.y * OOZ3;

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
            Y = ceil(v1.pos.y);
            int yend = ceil(v2.pos.y);
            Height = yend - Y;

            const float yprestep = Y - v1.pos.y;
            const float realWidth = v2.pos.x - v1.pos.x;
            const float realHeight = v2.pos.y - v1.pos.y;
            //printf("realwidth/height: %f, %f\n", realWidth, realHeight);

            X = (realWidth * yprestep) / realHeight + v1.pos.x;
            XStep = realWidth / realHeight;
            //printf("x, xstep: %f, %f\n", X, XStep);

            float xprestep = X - v1.pos.x;

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
        : Execing(true)
        , Printing(true)
    {
        for (int i = 0; i < NUM_MATRIX_MODES; ++i)
            CurMatrix[i] = &MatrixStack[i][0];
    }

    bool Execing;
    bool Printing;

    MatrixModeE MatrixMode;
    Mat44 MatrixStack[NUM_MATRIX_MODES][MAX_MATRIX_STACK_DEPTH] __attribute__((aligned(16)));
    Mat44* CurMatrix[NUM_MATRIX_MODES];

    float ClearDepth;
    GLuint ClearColor;

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
        GLuint ColorInt;
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
    ArrayState TexCoordArray;

    bool Texture2DEnabled;
    Texture* CurrentTexture;
    std::map<GLuint, Texture*> AllTextures;
};

inline void drawScanLine(const Gradients& grads, const Edge* left, const Edge* right)
{
    int xstart = ceil(left->X);
    float xprestep = xstart - left->X;

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
    if (V1->pos.y > V3->pos.y) swap(V1, V3);
    if (V2->pos.y > V3->pos.y) swap(V2, V3);
    if (V1->pos.y > V2->pos.y) swap(V1, V2);

    TriVert v1 = *V1;
    TriVert v2 = *V2;
    TriVert v3 = *V3;
/*    printf("input pos: (%f,%f) (%f,%f) (%f,%f)\n",
            v1.pos.x, v1.pos.y,
            v2.pos.x, v2.pos.y,
            v3.pos.x, v3.pos.y);
    printf("input tex: (%f,%f) (%f,%f) (%f,%f)\n",
            v1.tex.x, v1.tex.y,
            v2.tex.x, v2.tex.y,
            v3.tex.x, v3.tex.y);
            */

    /*v1.pos.x -= 0.5f;
    v2.pos.x -= 0.5f;
    v3.pos.x -= 0.5f;
    v1.pos.y -= 0.5f;
    v2.pos.y -= 0.5f;
    v3.pos.y -= 0.5f;*/

    Gradients grads(v1, v2, v3);
    Edge edge12(grads, v1, v2, grads.OOZ1, grads.UOZ1, grads.VOZ1);
    Edge edge13(grads, v1, v3, grads.OOZ1, grads.UOZ1, grads.VOZ1);
    Edge edge23(grads, v2, v3, grads.OOZ2, grads.UOZ2, grads.VOZ2);

    // figure out where v2.x is on long edge
    const float xOnLong = (((v2.pos.y - v1.pos.y) * (v3.pos.x - v1.pos.x)) / (v3.pos.y - v1.pos.y)) + v1.pos.x;
    //printf("xOnLong: %f\n", xOnLong);

    //printf("scanning in %c %c %c\n", v1.debug, v2.debug, v3.debug);
    const Edge* left1;
    const Edge* right1;
    const Edge* left2;
    const Edge* right2;
    if (v2.pos.x > xOnLong)
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
inline GLuint floatColorToUint(Vec4 v) { return floatColorToUint(v.x, v.y, v.z, v.w); }

void pumpEvents()
{
    ::XEvent event;
    while (true)
    {
        if (::XCheckWindowEvent(mDisplay, mWindow, -1, &event))
        {
            handleEvent(event);
        }
        else if (::XCheckTypedEvent(mDisplay, ClientMessage, &event))
        {
            handleEvent(event);
        }
        else
        {
            break;
        }
    }

    // send key press and up events

    for (int i = 0; i < KeyMapSize; ++i)
    {
        if (mKeyIsReleased[i] && mKeyIsPressed[i])
        {
            if (mListener) mListener->OnKeyUp(static_cast<Key::Code>(i));
            mKeyIsPressed[i] = false;
            mKeyIsReleased[i] = false;
        }
        else if (mKeyIsPressed[i])
        {
            if (mListener) mListener->OnKeyPressed(static_cast<Key::Code>(i));
        }
    }		
}

void handleEvent(const ::XEvent& event)
{
    switch (event.type)
    {
        case ClientMessage:
        {
            if (event.xclient.message_type == mWMProtocols &&
                event.xclient.format == 32 &&
                event.xclient.data.l[0] == (long)mWMDeleteWindow)
            {
                mIsShuttingDown = true;
            }
            break;
        }

        case KeyPress:
        case KeyRelease:
        {
            const KeySym keySym = ::XKeycodeToKeysym(mDisplay, event.xkey.keycode, 0);
            const int hiSym = (keySym & 0xff00) >> 8;
            const int loSym = keySym & 0xff;

            Key key = Key::Undefined;
            switch (hiSym)
            {
                case 0x00:
                    key = mNormalKeys[loSym];
                    break;
                case 0xff:
                    key = mFunctionKeys[loSym];
                    break;
            }

            if (event.type == KeyPress)
            {
                if (!mKeyIsPressed[key])
                {
                    bool defaultKeyHandlers = true;

                    if (mListener)
                    {
                        mListener->OnKeyDown(key);
                        defaultKeyHandlers = mListener->DoDefaultKeyHandlers();
                    }

                    if (defaultKeyHandlers && key == Key::Escape)
                    {
                        mIsShuttingDown = true;
                    }
                
                }
                mKeyIsPressed[key] = true;
                mKeyIsReleased[key] = false;
            }
            else
            {
                mKeyIsReleased[key] = true;
            }
            break;
        }
    }
}

void initializeKeyMaps()
{
    for (int i = 0; i < KeyMapSize; ++i)
    {
        mNormalKeys[i] = Key::Undefined;
        mFunctionKeys[i] = Key::Undefined;
        mKeyIsPressed[i] = false;
        mKeyIsReleased[i] = false;
    }

    mNormalKeys[XK_space] = Key::Space;
    mNormalKeys[XK_comma] = Key::Comma;
    mNormalKeys[XK_period] = Key::Period;
    mNormalKeys[XK_slash] = Key::Slash;
    mNormalKeys[XK_0] = Key::Zero;
    mNormalKeys[XK_1] = Key::One;
    mNormalKeys[XK_2] = Key::Two;
    mNormalKeys[XK_3] = Key::Three;
    mNormalKeys[XK_4] = Key::Four;
    mNormalKeys[XK_5] = Key::Five;
    mNormalKeys[XK_6] = Key::Six;
    mNormalKeys[XK_7] = Key::Seven;
    mNormalKeys[XK_8] = Key::Eight;
    mNormalKeys[XK_9] = Key::Nine;
    mNormalKeys[XK_semicolon] = Key::SemiColon;
    mNormalKeys[XK_equal] = Key::Equals;
    mNormalKeys[XK_a] = Key::A;
    mNormalKeys[XK_b] = Key::B;
    mNormalKeys[XK_c] = Key::C;
    mNormalKeys[XK_d] = Key::D;
    mNormalKeys[XK_e] = Key::E;
    mNormalKeys[XK_f] = Key::F;
    mNormalKeys[XK_g] = Key::G;
    mNormalKeys[XK_h] = Key::H;
    mNormalKeys[XK_i] = Key::I;
    mNormalKeys[XK_j] = Key::J;
    mNormalKeys[XK_k] = Key::K;
    mNormalKeys[XK_l] = Key::L;
    mNormalKeys[XK_m] = Key::M;
    mNormalKeys[XK_n] = Key::N;
    mNormalKeys[XK_o] = Key::O;
    mNormalKeys[XK_p] = Key::P;
    mNormalKeys[XK_q] = Key::Q;
    mNormalKeys[XK_r] = Key::R;
    mNormalKeys[XK_s] = Key::S;
    mNormalKeys[XK_t] = Key::T;
    mNormalKeys[XK_u] = Key::U;
    mNormalKeys[XK_v] = Key::V;
    mNormalKeys[XK_w] = Key::W;
    mNormalKeys[XK_x] = Key::X;
    mNormalKeys[XK_y] = Key::Y;
    mNormalKeys[XK_z] = Key::Z;
    mNormalKeys[XK_bracketleft] = Key::OpenBracket;
    mNormalKeys[XK_backslash] = Key::BackSlash;
    mNormalKeys[XK_bracketright] = Key::CloseBracket;

    mFunctionKeys[0xff & XK_BackSpace] = Key::Backspace;
    mFunctionKeys[0xff & XK_Tab] = Key::Tab;
    mFunctionKeys[0xff & XK_Linefeed] = Key::Undefined;
    mFunctionKeys[0xff & XK_Clear] = Key::Clear;
    mFunctionKeys[0xff & XK_Return] = Key::Enter;
    mFunctionKeys[0xff & XK_Pause] = Key::Pause;
    mFunctionKeys[0xff & XK_Scroll_Lock] = Key::ScrollLock;
    mFunctionKeys[0xff & XK_Sys_Req] = Key::PrintScreen;
    mFunctionKeys[0xff & XK_Escape] = Key::Escape;
    mFunctionKeys[0xff & XK_Delete] = Key::Delete;
    mFunctionKeys[0xff & XK_Kanji] = Key::Kanji;
    mFunctionKeys[0xff & XK_Kana_Shift] = Key::Kana;
    mFunctionKeys[0xff & XK_Home] = Key::Home;
    mFunctionKeys[0xff & XK_Left] = Key::Left;
    mFunctionKeys[0xff & XK_Up] = Key::Up;
    mFunctionKeys[0xff & XK_Right] = Key::Right;
    mFunctionKeys[0xff & XK_Down] = Key::Down;
    mFunctionKeys[0xff & XK_Prior] = Key::Undefined;
    mFunctionKeys[0xff & XK_Page_Up] = Key::PageUp;
    mFunctionKeys[0xff & XK_Next] = Key::Undefined;
    mFunctionKeys[0xff & XK_Page_Down] = Key::PageDown;
    mFunctionKeys[0xff & XK_End] = Key::End;
    mFunctionKeys[0xff & XK_Begin] = Key::Undefined;
    mFunctionKeys[0xff & XK_Select] = Key::Undefined;
    mFunctionKeys[0xff & XK_Print] = Key::Undefined;
    mFunctionKeys[0xff & XK_Execute] = Key::Undefined;
    mFunctionKeys[0xff & XK_Insert] = Key::Insert;
    mFunctionKeys[0xff & XK_Undo] = Key::Undefined;
    mFunctionKeys[0xff & XK_Redo] = Key::Undefined;
    mFunctionKeys[0xff & XK_Menu] = Key::Undefined;
    mFunctionKeys[0xff & XK_Find] = Key::Undefined;
    mFunctionKeys[0xff & XK_Cancel] = Key::Cancel;
    mFunctionKeys[0xff & XK_Help] = Key::Help;
    mFunctionKeys[0xff & XK_Break] = Key::Undefined;
    mFunctionKeys[0xff & XK_Mode_switch] = Key::ModeChange;
    mFunctionKeys[0xff & XK_Num_Lock] = Key::NumLock;
    mFunctionKeys[0xff & XK_KP_Space] = Key::Space;
    mFunctionKeys[0xff & XK_KP_Tab] = Key::Tab;
    mFunctionKeys[0xff & XK_KP_Enter] = Key::Enter;
    mFunctionKeys[0xff & XK_KP_F1] = Key::F1;
    mFunctionKeys[0xff & XK_KP_F2] = Key::F2;
    mFunctionKeys[0xff & XK_KP_F3] = Key::F3;
    mFunctionKeys[0xff & XK_KP_F4] = Key::F4;
    mFunctionKeys[0xff & XK_KP_Home] = Key::Home;
    mFunctionKeys[0xff & XK_KP_Left] = Key::Left;
    mFunctionKeys[0xff & XK_KP_Right] = Key::Right;
    mFunctionKeys[0xff & XK_KP_Down] = Key::Down;
    mFunctionKeys[0xff & XK_KP_Prior] = Key::Undefined;
    mFunctionKeys[0xff & XK_KP_Page_Up] = Key::PageUp;
    mFunctionKeys[0xff & XK_KP_Next] = Key::Undefined;
    mFunctionKeys[0xff & XK_KP_Page_Down] = Key::PageDown;
    mFunctionKeys[0xff & XK_KP_End] = Key::End;
    mFunctionKeys[0xff & XK_KP_Begin] = Key::Undefined;
    mFunctionKeys[0xff & XK_KP_Insert] = Key::Insert;
    mFunctionKeys[0xff & XK_KP_Delete] = Key::Delete;
    mFunctionKeys[0xff & XK_KP_Equal] = Key::Equals;
    mFunctionKeys[0xff & XK_KP_Multiply] = Key::Multiply;
    mFunctionKeys[0xff & XK_KP_Add] = Key::Add;
    mFunctionKeys[0xff & XK_KP_Separator] = Key::Separator;
    mFunctionKeys[0xff & XK_KP_Subtract] = Key::Subtract;
    mFunctionKeys[0xff & XK_KP_Decimal] = Key::Decimal;
    mFunctionKeys[0xff & XK_KP_Divide] = Key::Divide;
    mFunctionKeys[0xff & XK_KP_0] = Key::NumPad0;
    mFunctionKeys[0xff & XK_KP_1] = Key::NumPad1;
    mFunctionKeys[0xff & XK_KP_2] = Key::NumPad2;
    mFunctionKeys[0xff & XK_KP_3] = Key::NumPad3;
    mFunctionKeys[0xff & XK_KP_4] = Key::NumPad4;
    mFunctionKeys[0xff & XK_KP_5] = Key::NumPad5;
    mFunctionKeys[0xff & XK_KP_6] = Key::NumPad6;
    mFunctionKeys[0xff & XK_KP_7] = Key::NumPad7;
    mFunctionKeys[0xff & XK_KP_8] = Key::NumPad8;
    mFunctionKeys[0xff & XK_KP_9] = Key::NumPad9;
    mFunctionKeys[0xff & XK_F1] = Key::F1;
    mFunctionKeys[0xff & XK_F2] = Key::F2;
    mFunctionKeys[0xff & XK_F3] = Key::F3;
    mFunctionKeys[0xff & XK_F4] = Key::F4;
    mFunctionKeys[0xff & XK_F5] = Key::F5;
    mFunctionKeys[0xff & XK_F6] = Key::F6;
    mFunctionKeys[0xff & XK_F7] = Key::F7;
    mFunctionKeys[0xff & XK_F8] = Key::F8;
    mFunctionKeys[0xff & XK_F9] = Key::F9;
    mFunctionKeys[0xff & XK_F10] = Key::F10;
    mFunctionKeys[0xff & XK_F11] = Key::F11;
    mFunctionKeys[0xff & XK_F12] = Key::F12;
    mFunctionKeys[0xff & XK_Shift_L] = Key::Shift;
    mFunctionKeys[0xff & XK_Shift_R] = Key::Shift;
    mFunctionKeys[0xff & XK_Control_L] = Key::Control;
    mFunctionKeys[0xff & XK_Control_R] = Key::Control;
    mFunctionKeys[0xff & XK_Caps_Lock] = Key::CapsLock;
    mFunctionKeys[0xff & XK_Shift_Lock] = Key::CapsLock;
    mFunctionKeys[0xff & XK_Meta_L] = Key::Meta;
    mFunctionKeys[0xff & XK_Meta_R] = Key::Meta;
    mFunctionKeys[0xff & XK_Alt_L] = Key::Alt;
    mFunctionKeys[0xff & XK_Alt_R] = Key::Alt;
}

// these are nicer implemented near their api functions
void enableOrDisable(GLenum cap, bool val);


// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
//
// Actual private member variables declared here.
//
// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
// ---------------------------------------------------------------------

::Display* mDisplay;
::Window mWindow;
::GC mGC;
::XImage* mImage;
bool mIsOpen;
bool mIsShuttingDown;
Atom mWMProtocols;
Atom mWMDeleteWindow;
int mWidth;
int mHeight;

enum 
{ 
    EventMask = KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | ButtonMotionMask,
    KeyMapSize = 256
};
typedef Key::Code TKeyMap[KeyMapSize];
typedef bool TKeyFlags[KeyMapSize];
TKeyMap mNormalKeys;
TKeyMap mFunctionKeys;
TKeyFlags mKeyIsPressed;
TKeyFlags mKeyIsReleased;

DisplayImplContext ctx __attribute__((aligned(16)));

#endif
