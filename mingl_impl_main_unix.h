#ifdef MINGL_PLATFORM_UNIX

namespace mingl
{

inline MinGL::MinGL(const char* title, int width, int height)
    : mIsOpen(false)
    , mIsShuttingDown(false)
    , mWidth(width)
    , mHeight(height)
    , mListener(0)
{
    initializeKeyMaps();


    mDisplay = ::XOpenDisplay(0);
    if (!mDisplay)
    {
        Close();
        return;
    }

    int screen = XDefaultScreen(mDisplay);
    ::Visual* visual = XDefaultVisual(mDisplay, screen);
    if (!visual)
    {
        Close();
        return;
    }

    Window root = XDefaultRootWindow(mDisplay);
    int screenWidth = XDisplayWidth(mDisplay, screen);
    int screenHeight = XDisplayHeight(mDisplay, screen);
    int left = (screenWidth - width) / 2;
    int top = (screenHeight - height) / 2;

    ::XSetWindowAttributes attributes;
    attributes.border_pixel = attributes.background_pixel = XBlackPixel(mDisplay, screen);
    attributes.backing_store = NotUseful;

    int displayDepth = DefaultDepth(mDisplay, screen);

    mWindow = ::XCreateWindow(mDisplay, root, left, top, width, height, 0,
            displayDepth, InputOutput, visual,
            CWBackPixel | CWBorderPixel | CWBackingStore, &attributes);

    ::XStoreName(mDisplay, mWindow, title);

    mWMProtocols = XInternAtom(mDisplay, "WM_PROTOCOLS", True);
    mWMDeleteWindow = XInternAtom(mDisplay, "WM_DELETE_WINDOW", True);
    if (mWMProtocols == 0 || mWMDeleteWindow == 0)
    {
        Close();
        return;
    }
    if (::XSetWMProtocols(mDisplay, mWindow, &mWMDeleteWindow, 1) == 0)
    {
        Close();
        return;
    }

    ::XSizeHints sizeHints;
    sizeHints.flags = PPosition | PMinSize | PMaxSize;
    sizeHints.x = sizeHints.y = 0;
    sizeHints.min_width = sizeHints.max_width = width;
    sizeHints.min_height = sizeHints.max_height = height;
    ::XSetNormalHints(mDisplay, mWindow, &sizeHints);
    ::XClearWindow(mDisplay, mWindow);
    ::XSelectInput(mDisplay, mWindow, EventMask);

    int bufferDepth = 32;
    int bytesPerPixel = (bufferDepth + 7) / 8;
    int bitsPerPixel = 8 * bytesPerPixel;
    mGC = DefaultGC(mDisplay, screen);
    mImage = ::XCreateImage(mDisplay, CopyFromParent, displayDepth, ZPixmap, 0, 0,
            width, height, bitsPerPixel, width * bytesPerPixel);
#if defined(MINGL_LITTLE_ENDIAN)
    mImage->byte_order = LSBFirst;
#else
    mImage->byte_order = MSBFirst;
#endif  
    if (!mImage)
    {
        Close();
        return;
    }

    ::XMapRaised(mDisplay, mWindow);
    ::XFlush(mDisplay);

    ctx.Init(this, width, height);

    mIsOpen = true;
}

inline MinGL::~MinGL()
{
    Close();
}

inline void MinGL::Close()
{
    if (mImage)
    {
        XDestroyImage(mImage);
        mImage = 0;
    }

    if (mDisplay && mWindow)
    {
        XDestroyWindow(mDisplay, mWindow);
        mWindow = 0;
    }

    if (mDisplay)
    {
        XCloseDisplay(mDisplay);
        mDisplay = 0;
    }

    mIsOpen = false;
}

inline void MinGL::SwapBuffers()
{
    if (mIsShuttingDown)
    {
        Close();
        return;
    }

    if (!mDisplay || !mWindow || !mImage)
        return;

    mImage->data = (char*)ctx.Buf.C;

    ::XPutImage(mDisplay, mWindow, mGC, mImage, 0, 0, 0, 0, mWidth, mHeight);
    ::XFlush(mDisplay);

    mImage->data = 0;

    pumpEvents();
}

inline bool MinGL::IsOpen() const
{
    return mIsOpen;
}

inline void* MinGL::operator new(size_t size)
{
    const int alignSize = 16;
    void* p = MINGL_ALLOCATE_MEMORY(size + alignSize - 1);
    uintptr_t addr = reinterpret_cast<uintptr_t>(p);
    addr += (alignSize - 1);
    addr &= ~(alignSize - 1);
    return reinterpret_cast<void*>(addr);
}

inline void MinGL::operator delete(void* p)
{
    MINGL_FREE_MEMORY(p);
}

}

#endif
