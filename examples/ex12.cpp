#define MINGL_STANDARD_OPENGL_NAMES
#include "../dist/mingl.h"

MINGL_DEFINE_IMPLICIT_GLOBAL_CONTEXT()
using namespace mingl;

void drawCube(float size)
{
    #define V(a,b,c) a size, b size, c size,
        float r[] = { V(+,-,+) V(+,-,-) V(+,+,+) V(+,+,-) };
        float t[] = { V(+,+,+) V(+,+,-) V(-,+,+) V(-,+,-) };
        float f[] = { V(+,+,+) V(-,+,+) V(+,-,+) V(-,-,+) };
        float l[] = { V(-,-,+) V(-,+,+) V(-,-,-) V(-,+,-) };
        float b[] = { V(-,-,+) V(-,-,-) V(+,-,+) V(+,-,-) };
        float k[] = { V(-,-,-) V(-,+,-) V(+,-,-) V(+,+,-) };
    #undef V
    #define DRAW(n) glVertexPointer(3, GL_FLOAT, 0, n); glDrawArrays(GL_TRIANGLE_STRIP, 0, 4)
        DRAW(r);
        DRAW(t);
        DRAW(f);
        DRAW(l);
        DRAW(b);
        DRAW(k);
    #undef DRAW
}

int main()
{
    int width = 500;
    int height = 500;
    MinGLGlobalContext = new MinGL("ex12", width, height);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearColor(0.f, 0.f, 0.f, 0.f);
    glClearDepthf(1.f);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.f, (float)width/(float)height, 1.f, 100.f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnableClientState(GL_VERTEX_ARRAY);

    while (MinGLGlobalContext->IsOpen())
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();

        gluLookAt(
                0.f, 0.f, 4.f,
                0.f, 0.f, 0.f,
                0.f, 1.f, 0.f);

        glColor4f(1.f, 0.f, 0.f, 1.f);
        glTranslatef(.25f, 0.f, 0.f);
        drawCube(0.5f);

        glColor4f(0.f, 1.f, 0.f, 1.f);
        glTranslatef(-.25f, 0.f, -1.f);
        drawCube(0.5f);

        glColor4f(0.f, 0.f, 1.f, 1.f);
        glTranslatef(-.25f, 0.f, -1.f);
        drawCube(0.5f);

        MinGLGlobalContext->SwapBuffers();
    }
}
