#include <stdio.h>
#include "mingl.h"

using namespace mingl;

int main()
{
    Display* display = DisplayOpen();

    printf("GL_VENDOR: %s\nGL_RENDERER: %s\nGL_VERSION: %s\n", glGetString(GL_VENDOR), glGetString(GL_RENDERER), glGetString(GL_VERSION));

    glEnableClientState(GL_VERTEX_ARRAY);

    float triangle[] =
    {
        0.f,  1.f, -3.f,
        1.f,  0.f, -3.f,
        -1.f, 0.f, -3.f,
    };

    while (DisplayIsOpen(display))
    {
        glLoadIdentity();
        glClearColor(0.7f, 0.7f, 0.7f, 1.f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        glColor4f(1.f, 0.f, 0.f, 1.f);
        glVertexPointer(3, GL_FLOAT, 0, &triangle);
        glDrawArrays(GL_TRIANGLES, 0, 9);

        DisplaySwapBuffers(display);
    }

    return 0;
}
