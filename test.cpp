#include <stdio.h>
#ifndef WANT_DIST_TEST
    #include "mingl_debug.h"
#else
    #include "dist/mingl.h"
#endif

using namespace mingl;

extern unsigned char testtex_rgba[];

class Main : public MinGL, public EventListener
{
    float mOffset;
    public:

        Main() : mOffset(0.0f) // (0.5f) is a pathological case
        {
        }

        void OnKeyDown(Key k)
        {
            if (k == Key::Up)
            {
                mOffset -= 0.01f;
            }
            if (k == Key::Down)
            {
                mOffset += 0.01f;
            }
        }

        void Run()
        {
            SetListener(this);

            printf("GL_VENDOR: %s\nGL_RENDERER: %s\nGL_VERSION: %s\n", GetString(GL_VENDOR), GetString(GL_RENDERER), GetString(GL_VERSION));
            printf("sizeof(mingl::MinGL): %d\n", sizeof(MinGL));

            EnableClientState(GL_VERTEX_ARRAY);
            EnableClientState(GL_TEXTURE_COORD_ARRAY);

            Enable(GL_TEXTURE_2D);
            GLuint texid; GenTextures(1, &texid);
            BindTexture(GL_TEXTURE_2D, texid);
            TexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, testtex_rgba);

            //float delta = 1.f;
            while (IsOpen())
            {
                MatrixMode(GL_PROJECTION);
                LoadIdentity();
                Ortho(0, 1280, 720, 0, -20, 100);
                MatrixMode(GL_MODELVIEW);
                LoadIdentity();
                MatrixMode(GL_TEXTURE);
                LoadIdentity();
                ClearColor(0.f, 0.f, 0.f, 0.f);
                Clear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
                Color(1.f, 0.f, 0.f, 1.f);

                if (0)
                {
                    float triangle[] =
                    {
                        300.f, 100.f, 0.f,
                        400.f, 200.f, 0.f,
                        250.f, 400.f, 0.f,

                        300.f, 100.f, 0.f,
                        250.f, 400.f, 0.f,
                        50.f, 300.f, 0.f,
                    };

                    float texcoords[] =
                    {
                        1.f, 0.f,
                        1.f, 1.f,
                        0.f, 1.f,

                        1.f, 0.f,
                        0.f, 1.f,
                        0.f, 0.f,
                    };

                    VertexPointer(3, GL_FLOAT, 0, triangle);
                    TexCoordPointer(2, GL_FLOAT, 0, texcoords);
                    DrawArrays(GL_TRIANGLES, 0, 18);
                }

                if (1)
                {
                    float verts[] = 
                    {
                        99.5f, 99.5f,
                        163.5f, 99.5f,
                        99.5f, 163.5f,

                        //163.5f, 99.5f,
                        163.5f, 163.5f,
                        //99.5f, 163.5f,
                    };

                    /*
                    float tcs[] =
                    {
                        -0.5f, -0.5f,
                        63.5f, -0.5f,
                        -0.5f, 63.5f,

                        //63.5f, -0.5f,
                        63.5f, 63.5f,
                        //-0.5f, 63.5f,
                    };
                    */
                    float tcs[] =
                    {
                        0.f, 0.f,
                        1.f, 0.f,
                        0.f, 1.f,

                        //63.5f, -0.5f,
                        1.f, 1.f,
                        //-0.5f, 63.5f,
                    };

                    for (unsigned int i = 0; i < sizeof(verts)/sizeof(verts[0]); ++i)
                    {
                        //verts[i] -= 0.5f;
                    }

                    //mOffset += delta;
                    //if (mOffset > 10) delta = -1.f;
                    //if (mOffset < -10) delta = 1.f;

                    for (unsigned int j = 0; j < sizeof(verts)/sizeof(verts[0]); j += 2)
                    {
                        verts[j] += mOffset;
                        verts[j+1] += mOffset;
                        //verts[j] = (float)(int)verts[j];
                    }

                    VertexPointer(2, GL_FLOAT, 0, verts);
                    TexCoordPointer(2, GL_FLOAT, 0, tcs);
                    DrawArrays(GL_TRIANGLE_STRIP, 0, sizeof(verts)/sizeof(verts[0])/2);
                }

                /*
                if (0)
                {
                    float test[] =
                    {
                        756.f, 100.f,
                        756.f, 356.f,
                        500.f, 356.f,
                    };
                    float tc[] =
                    {
                        1.f, 0.f,
                        1.f, 1.f,
                        0.f, 1.f,
                    };
                    VertexPointer(2, GL_FLOAT, 0, test);
                    TexCoordPointer(2, GL_FLOAT, 0, tc);
                    DrawArrays(GL_TRIANGLES, 0, 6);
                }
                */

                SwapBuffers();
            }

        }
};

int main()
{
    Main* main = new Main();
    main->Run();
    return 0;
}
