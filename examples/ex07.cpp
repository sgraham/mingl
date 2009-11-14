#include "../dist/mingl.h"

using namespace mingl;

int main()
{
    MinGL gl;

    GLfloat square[] = {
        0.25, 0.25, 0.f,
        0.75, 0.25, 0.f,
        0.25, 0.75, 0.f,
        0.75, 0.75, 0.f,
    };

    gl.ClearColor(0.f, 0.f, 0.f, 0.f);
    gl.MatrixMode(GL_PROJECTION);
    gl.LoadIdentity();
    gl.Ortho(0.f, 1.f, 0.f, 1.f, -1.f, 1.f);
    gl.VertexPointer(3, GL_FLOAT, 0, square);
    gl.EnableClientState(GL_VERTEX_ARRAY);

    while (gl.IsOpen())
    {
        gl.Clear(GL_COLOR_BUFFER_BIT);
        gl.DrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        gl.Flush();
        gl.SwapBuffers();
    }
}
