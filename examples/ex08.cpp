#include "../dist/mingl.h"

using namespace mingl;

int main()
{
    MinGL gl;

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

    gl.ClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    gl.MatrixMode(GL_PROJECTION);
    gl.LoadIdentity();
    gl.Ortho(0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f);
    gl.VertexPointer(3, GL_FLOAT, 0, triangle);
    gl.ColorPointer(4, GL_FLOAT, 0, colors);
    gl.EnableClientState(GL_VERTEX_ARRAY);
    gl.EnableClientState(GL_COLOR_ARRAY);

    while (gl.IsOpen())
    {
        gl.Clear(GL_COLOR_BUFFER_BIT);
        gl.DrawArrays(GL_TRIANGLES, 0, 3);
        gl.SwapBuffers();
    }
}
