#define MINGL_STANDARD_OPENGL_NAMES
#include "../dist/mingl.h"

MINGL_DEFINE_IMPLICIT_GLOBAL_CONTEXT()

using namespace mingl;

int main()
{
    MinGLGlobalContext = new MinGL();

    GLfloat triangle[] = {
        0.25f, 0.25f, 0.0f,
        0.75f, 0.25f, 0.0f,
        0.25f, 0.75f, 0.0f
    };
    GLfloat colors[] = {
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f
    };

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrthof(0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f);
    glVertexPointer(3, GL_FLOAT, 0, triangle);
    glColorPointer(4, GL_FLOAT, 0, colors);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    while (MinGLGlobalContext->IsOpen())
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        MinGLGlobalContext->SwapBuffers();
    }
}
