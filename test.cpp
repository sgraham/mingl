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
    float delta = 0.1f;
    while (DisplayIsOpen(display))
    {
        glLoadIdentity();
        glClearColor(0.7f, 0.7f, 0.7f, 1.f);
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

        {
            float tristrip[] = 
            {
                500.f, 100.f,
                756.f, 100.f,
                500.f, 356.f,
                756.f, 356.f,
            };

            float tristriptc[] =
            {
                0.f, 0.f,
                1.f, 0.f,
                0.f, 1.f,
                1.f, 1.f,
            };

            offset += delta;
            /*if (offset > 10) delta = -0.1f;
            if (offset < -10) delta = 0.1f;

            tristrip[0] = 500.f + offset;
            tristrip[2] = 756.f + offset;
            tristrip[4] = 500.f + offset;
            tristrip[6] = 756.f + offset;
            */

            glVertexPointer(2, GL_FLOAT, 0, tristrip);
            glTexCoordPointer(2, GL_FLOAT, 0, tristriptc);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }

        /*
        float test[] =
        {
            564.f, 100.f,
            564.f, 164.f,
            500.f, 164.f,
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
        */

        DisplaySwapBuffers(display);
    }

    return 0;
}
