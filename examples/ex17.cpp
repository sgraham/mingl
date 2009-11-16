#define MINGL_STANDARD_OPENGL_NAMES
#include "../dist/mingl.h"

MINGL_DEFINE_IMPLICIT_GLOBAL_CONTEXT()
using namespace mingl;

extern unsigned char testtex_rgba[];

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
    // TOP
    -0.5f,  0.5f,  0.5f,
    0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    0.5f,  0.5f, -0.5f,
    // BOTTOM
    -0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,
    0.5f, -0.5f,  0.5f,
    0.5f, -0.5f, -0.5f,
};

GLfloat texCoords[] = {
    // FRONT
    0.0f, 0.0f,
    1.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f,
    // BACK
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 0.0f,
    0.0f, 1.0f,
    // LEFT
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 0.0f,
    0.0f, 1.0f,
    // RIGHT
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 0.0f,
    0.0f, 1.0f,
    // TOP
    0.0f, 0.0f,
    1.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f,
    // BOTTOM
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 0.0f,
    0.0f, 1.0f
};

GLuint texture;
float matAmbient[] = { 0.8f, 0.8f, 0.8f, 1.0f };
float lightAmbient[] = { 0.8f, 0.8f, 0.8f, 1.0f };
float xrot = 0.0f;
float yrot = 0.0f;

int main()
{
    int width = 1280;
    int height = 720;
    MinGLGlobalContext = new MinGL("ex17", width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(45.0f, (float)width / (float)height, 1.0f, 100.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, testtex_rgba);

    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glEnable(GL_TEXTURE_2D);
    //glEnable(GL_LIGHTING);
    //glEnable(GL_LIGHT0);

    //glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmbient);

    //glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepthf(1.0f);

    glVertexPointer(3, GL_FLOAT, 0, box);
    glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    //glEnable(GL_CULL_FACE);
    //glFrontFace(GL_CW);
    //glShadeModel(GL_SMOOTH);

    while (MinGLGlobalContext->IsOpen())
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glLoadIdentity();
        gluLookAt(0.f, 0.f, 3.f,
                0.f, 0.f, 0.f,
                0.f, 1.f, 0.f);

        glRotatef(xrot, 1.f, 0.f, 0.f);
        glRotatef(yrot, 0.f, 1.f, 0.f);

        // FRONT AND BACK
        glColor4f(1.f, 0.f, 0.f, 1.f);
        glNormal3f(0.f, 0.f, 1.f);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glNormal3f(0.0f, 0.0f, -1.0f);
        glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);

        // LEFT AND RIGHT
        glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
        glNormal3f(-1.0f, 0.0f, 0.0f);
        glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);
        glNormal3f(1.0f, 0.0f, 0.0f);
        glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);

        // TOP AND BOTTOM
        glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glDrawArrays(GL_TRIANGLE_STRIP, 16, 4);
        glNormal3f(0.0f, -1.0f, 0.0f);
        glDrawArrays(GL_TRIANGLE_STRIP, 20, 4);

        xrot += .2f;
        yrot += .3f;
        MinGLGlobalContext->SwapBuffers();
    }
}
