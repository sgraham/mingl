#define MINGL_STANDARD_OPENGL_NAMES
#include "../dist/mingl.h"

MINGL_DEFINE_IMPLICIT_GLOBAL_CONTEXT()

using namespace mingl;

GLfloat triangle[] = {
    0.5f,-0.5f, 0.0f,
    0.0f, 0.5f, 0.0f,
    -0.5f,-0.5f, 0.0f
};

GLfloat colors[] = {
    1.0f, 0.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f
};

int main()
{
    MinGLGlobalContext = new MinGL("ex11");

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepthf(1.f);

    glVertexPointer(3, GL_FLOAT, 0, triangle);
    glColorPointer(4, GL_FLOAT, 0, colors);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrthof(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 20.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    while (MinGLGlobalContext->IsOpen())
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glPushMatrix();
        {
            glTranslatef(-0.2f, 0.f, -1.f);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            glRotatef(45.f, 0.f, 0.f, 1.f);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }
        glPopMatrix();

        glPushMatrix();
        {
            glTranslatef(0.5f, 0.f, 0.f);
            glScalef(0.5f, 0.5f, 0.5f);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }
        glPopMatrix();

        MinGLGlobalContext->SwapBuffers();
    }
}
