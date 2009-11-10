#include <stdio.h>
#include "mingl.h"

using namespace mingl;

extern unsigned char testtex_rgba[];

int main()
{
    Display* display = DisplayOpen();

    printf("GL_VENDOR: %s\nGL_RENDERER: %s\nGL_VERSION: %s\n", glGetString(GL_VENDOR), glGetString(GL_RENDERER), glGetString(GL_VERSION));

    glEnableClientState(GL_VERTEX_ARRAY);

    glEnable(GL_TEXTURE_2D);
    GLuint texid; glGenTextures(1, &texid);
    glBindTexture(GL_TEXTURE_2D, texid);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, testtex_rgba);

    float offset = 0.f;
    float delta = 1.f;
    while (DisplayIsOpen(display))
    {
        glLoadIdentity();
        glClearColor(0.f, 0.f, 0.f, 0.f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        glColor4f(1.f, 0.f, 0.f, 1.f);

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

            glVertexPointer(3, GL_FLOAT, 0, triangle);
            glTexCoordPointer(2, GL_FLOAT, 0, texcoords);
            glDrawArrays(GL_TRIANGLES, 0, 18);
        }

        if (1)
        {
            float verts[] = 
            {
                99.5f, -0.5f,
                227.5f, -0.5f,
                99.5f, 127.5f,

                227.5f, -0.5f,
                99.5f, 127.5f,
                227.5f, 127.5f,
            };

            float tcs[] =
            {
                -0.5f, -0.5f,
                63.5f, -0.5f,
                -0.5f, 63.5f,

                63.5f, -0.5f,
                -0.5f, 63.5f,
                63.5f, 63.5f,
            };

            for (unsigned int i = 0; i < sizeof(verts)/sizeof(verts[0]); ++i)
            {
                //verts[i] -= 0.5f;
            }

            offset += delta;
            if (offset > 10) delta = -1.f;
            if (offset < -10) delta = 1.f;

            for (unsigned int j = 0; j < sizeof(verts)/sizeof(verts[0]); j += 2)
            {
                verts[j] += offset;
                //verts[j] = (float)(int)verts[j];
            }

            glVertexPointer(2, GL_FLOAT, 0, verts);
            glTexCoordPointer(2, GL_FLOAT, 0, tcs);
            glDrawArrays(GL_TRIANGLES, 0, sizeof(verts)/sizeof(verts[0]));
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
            glVertexPointer(2, GL_FLOAT, 0, test);
            glTexCoordPointer(2, GL_FLOAT, 0, tc);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        DisplaySwapBuffers(display);
    }

    return 0;
}
