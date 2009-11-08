#include <stdio.h>
#include "mingl.h"

using namespace mingl;

extern unsigned char testtex_rgba[];

int main()
{
    Display* display = DisplayOpen();

    printf("GL_VENDOR: %s\nGL_RENDERER: %s\nGL_VERSION: %s\n", glGetString(GL_VENDOR), glGetString(GL_RENDERER), glGetString(GL_VERSION));

    glEnableClientState(GL_VERTEX_ARRAY);

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

    /*
    float triangle[] = 
    {
        100.f, 100.f, -3.f,
        300.f, 300.f, -3.f,
        100.f, 300.f, -3.f,
    };

    float texcoords[] =
    {
        0.f, 0.f,
        1.f, 1.f,
        0.f, 1.f,
    };
    */

    glEnable(GL_TEXTURE_2D);
    GLuint texid; glGenTextures(1, &texid);
    glBindTexture(GL_TEXTURE_2D, texid);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, testtex_rgba);

    while (DisplayIsOpen(display))
    {
        glLoadIdentity();
        glClearColor(0.7f, 0.7f, 0.7f, 1.f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        glColor4f(1.f, 0.f, 0.f, 1.f);
        glVertexPointer(3, GL_FLOAT, 0, triangle);
        glTexCoordPointer(2, GL_FLOAT, 0, texcoords);
        glDrawArrays(GL_TRIANGLES, 0, 18);

        DisplaySwapBuffers(display);
    }

    return 0;
}
