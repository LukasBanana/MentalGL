// Simple test for MentalGL
// Created on 26/03/2018 by Lukas Hermanns

#include <stdio.h>
#include <glad/glad.h>

#ifdef __APPLE__
#   include <GLUT/glut.h>
#else
#   include <GL/glut.h>
#endif

#define MENTAL_GL_GETINTEGERI_V
#define MENTAL_GL_GETINTEGER64I_V

#ifndef MENTAL_GL_IMPLEMENTATION
#define MENTAL_GL_IMPLEMENTATION
#endif

#include "mental_gl.h"

void drawScene(void)
{
    float s = 0.5f;

    glBegin(GL_TRIANGLES);
    {
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex2f(0.0f, s);

        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex2f(s, -s);

        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex2f(-s, -s);
    }
    glEnd();

    static int renderStateShowen;
    if (!renderStateShowen)
    {
        renderStateShowen = 1;

        MGLQueryFormatting fmt = { ' ', 3, 200, MGLFormattingOrderDefault, 1, NULL };
        MGLString renderState = mglQueryRenderState(&fmt);

        printf("%s\n", mglGetString(renderState));
        mglFreeString(renderState);
    }
}

void displayCallback(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawScene();
    glutSwapBuffers();
}

void initGL(void)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    #ifdef __APPLE__
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_3_2_CORE_PROFILE);
    #else
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    #endif

    glutCreateWindow("MentalGL Test 1");

    gladLoadGL();

    glutDisplayFunc(displayCallback);
    
    initGL();
    glutMainLoop();

    return 0;
}


