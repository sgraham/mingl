// See mingl.h for copyright and documentation.

#include "mingl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef unix
    #define MINGL_PLATFORM_UNIX
#endif

#ifdef MINGL_PLATFORM_UNIX
    #include <X11/Xlib.h>
    #include <X11/Xutil.h>
    #include <X11/keysymdef.h>
#endif

namespace mingl
{

#ifdef MINGL_PLATFORM_UNIX

    class Display
    {
        public:
            Display(const char* title, int width, int height)
                : mIsOpen(false)
                , mIsShuttingDown(false)
                , mWidth(width)
                , mHeight(height)
            {
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

                // create (image) buffer

                /*
                buffer_.reset(width * height * bytesPerPixel);
                if (buffer_.isEmpty())
                {
                    close();
                    return false;
                }
                */

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

                mIsOpen = true;
            }

            ~Display()
            {
                Close();
            }

            void Close()
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

            void SwapBuffers()
            {
                if (mIsShuttingDown)
                {
                    Close();
                    return;
                }

                if (!mDisplay || !mWindow || !mImage)
                    return;

                char* testdata = (char*)malloc(1280*720*4);
                char j = 0;
                for (int i = 0; i < 1280*720*4; i += 4)
                {
                    testdata[i+1] = j;
                    testdata[i+2] = 0;
                    testdata[i+3] = 0;
                    j += 1;
                }

                mImage->data = testdata;

                ::XPutImage(mDisplay, mWindow, mGC, mImage, 0, 0, 0, 0, mWidth, mHeight);
                ::XFlush(mDisplay);

                mImage->data = NULL;
                free(testdata);

                pumpEvents();
            }

            bool IsOpen()
            {
                return mIsOpen;
            }

        private:

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
                }
            }


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
    };

#endif




Display* DisplayOpen(const char* title, int width, int height)
{
    return new Display(title, width, height);
}

bool DisplayIsOpen(Display* display)
{
    return display->IsOpen();
}

void DisplayClose(Display* display)
{
    display->Close();
}

void DisplaySwapBuffers(Display* display)
{
    display->SwapBuffers();
}

}
