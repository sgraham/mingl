#include <stdio.h>
#include "mingl_debug.h"

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
            printf("sizeof(MinGL): %d\n", sizeof(MinGL));

            EnableClientState(GL_VERTEX_ARRAY);

            Enable(GL_TEXTURE_2D);
            GLuint texid; GenTextures(1, &texid);
            BindTexture(GL_TEXTURE_2D, texid);
            TexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, testtex_rgba);

            //float delta = 1.f;
            while (IsOpen())
            {
                LoadIdentity();
                ClearColor(0.f, 0.f, 0.f, 0.f);
                Clear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
                Color4f(1.f, 0.f, 0.f, 1.f);

                if (0)
                {
                    float triangle[] =
                    {
                        300.f, 100.f, -3.f,
                        400.f, 200.f, -3.f,
                        250.f, 400.f, -3.f,

                        300.f, 100.f, -3.f,
                        250.f, 400.f, -3.f,
                        50.f, 300.f, -3.f,
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

                        163.5f, 99.5f,
                        163.5f, 163.5f,
                        99.5f, 163.5f,
                    };

                    float tcs[] =
                    {
                        -0.5f, -0.5f,
                        63.5f, -0.5f,
                        -0.5f, 63.5f,

                        63.5f, -0.5f,
                        63.5f, 63.5f,
                        -0.5f, 63.5f,
                    };

                    for (unsigned int i = 0; i < sizeof(verts)/sizeof(verts[0]); ++i)
                    {
                        //verts[i] -= 0.5f;
                    }

                    //offset += delta;
                    //if (offset > 10) delta = -0.01f;
                    //if (offset < -10) delta = .01f;

                    for (unsigned int j = 0; j < sizeof(verts)/sizeof(verts[0]); j += 2)
                    {
                        verts[j] += mOffset;
                        verts[j+1] += mOffset;
                        //verts[j] = (float)(int)verts[j];
                    }

                    VertexPointer(2, GL_FLOAT, 0, verts);
                    TexCoordPointer(2, GL_FLOAT, 0, tcs);
                    DrawArrays(GL_TRIANGLES, 0, sizeof(verts)/sizeof(verts[0]));
                }

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

                SwapBuffers();
            }

        }
};

int main()
{
    Main* main = new Main();
    printf("%p\n", main);
    main->Run();
    return 0;
}
