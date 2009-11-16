#define MINGL_STANDARD_OPENGL_NAMES
#include "../dist/mingl.h"

MINGL_DEFINE_IMPLICIT_GLOBAL_CONTEXT()
using namespace mingl;

float xrot = 0.0f;
float yrot = 0.0f;

bool acw = true;

GLfloat box[] = {
	// FRONT
	-0.5f, -0.5f,  0.5f,
	 0.5f, -0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	// BACK
	-0.5f, -0.5f, -0.5f,
	-0.5f,  0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	// LEFT
	-0.5f, -0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f,  0.5f, -0.5f,
	// RIGHT
	 0.5f, -0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f, -0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	// BOTTOM
	-0.5f, -0.5f,  0.5f,
	-0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f,  0.5f,
	 0.5f, -0.5f, -0.5f,
};

class Settings : public EventListener
{
    virtual void OnKeyDown(Key key)
    {
        if (key == Key::F)
        {
            acw = !acw;
            glFrontFace(acw ? GL_CCW : GL_CW);
        }
        else if (key == Key::C)
        {
            if (glIsEnabled(GL_CULL_FACE))
            {
                glDisable(GL_CULL_FACE);
            }
            else
            {
                glEnable(GL_CULL_FACE);
            }
        }
    }
};

int main()
{
    int width = 500;
    int height = 500;
    Settings settings;
    MinGLGlobalContext = new MinGL("ex14 (c toggle cull, f toggle winding)", width, height);
    MinGLGlobalContext->SetListener(&settings);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepthf(1.0f);

	glVertexPointer(3, GL_FLOAT, 0, box);
	glEnableClientState(GL_VERTEX_ARRAY);

	glShadeModel(GL_FLAT);

	glEnable(GL_CULL_FACE);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glViewport(0, 0, width, height);
	gluPerspective(45.0f, 1.0f * width / height, 1.0f, 100.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

    while (MinGLGlobalContext->IsOpen())
    {
        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();

        gluLookAt(
                0.0f, 0.0f, 3.0f,
                0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f);

        glRotatef(xrot, 1.0f, 0.0f, 0.0f);
        glRotatef(yrot, 0.0f, 1.0f, 0.0f);

        glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);

        glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
        glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);

        glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
        glDrawArrays(GL_TRIANGLE_STRIP, 16, 4);

        MinGLGlobalContext->SwapBuffers();

        xrot += 2.f;
        yrot += 3.f;
    }
}
