#ifdef MINGL_PLATFORM_UNIX

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
