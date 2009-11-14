#define MINGL_STANDARD_OPENGL_NAMES
#include "../dist/mingl.h"

MINGL_DEFINE_IMPLICIT_GLOBAL_CONTEXT()

using namespace mingl;

int main()
{
    MinGLGlobalContext = new MinGL("ex10");

    float xrot = 0.f;
    float yrot = 0.f;

    GLfloat triangle[] = {
        -0.25f, -0.25f, 0.0f,
        0.25f, -0.25f, 0.0f,
        -0.25f, 0.25f, 0.0f
    };

    GLfloat colors[] = {
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f
    };

    GLfloat square[] = {
        -0.25f, -0.25f,
        0.25f, -0.25f,
        -0.25f, 0.25f,
        0.25f, 0.25f
    };

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrthof(0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnableClientState(GL_VERTEX_ARRAY);

    while (MinGLGlobalContext->IsOpen())
    {
        glClear(GL_COLOR_BUFFER_BIT);

        // Triangle
        {
            glShadeModel(GL_SMOOTH);

            glVertexPointer(3, GL_FLOAT, 0, triangle);
            glColorPointer(4, GL_FLOAT, 0, colors);

            glEnableClientState(GL_COLOR_ARRAY);
        }

        glPushMatrix();
        {
            glTranslatef(0.25f, 0.5f, 0.f);
            glScalef(0.5f, 0.5f, 0.5f);
            glRotatef(xrot, 1.0f, 0.0f, 0.0f);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }
        glPopMatrix();

        // Square
        glDisableClientState(GL_COLOR_ARRAY);
        glShadeModel(GL_FLAT);
        glVertexPointer(2, GL_FLOAT, 0, square);
        glColor4f(0.25f, 0.25f, 0.75f, 1.0f);

        glPushMatrix();
        {
            glTranslatef(0.75f, 0.5f, 0.0f);
            glScalef(0.5f, 0.5f, 0.5f);
            glRotatef(yrot, 0.0f, 1.0f, 0.0f);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }
        glPopMatrix();

        xrot += 2.f;
        yrot += 3.f;

        MinGLGlobalContext->SwapBuffers();
    }
}
