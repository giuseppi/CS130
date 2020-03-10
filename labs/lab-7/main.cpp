// Name: Giuseppi Pelayo
// Quarter, Year: Winter, 2020
// Lab: 07
//
// This file is to be modified by the student.
// main.cpp
////////////////////////////////////////////////////////////
#ifndef __APPLE__
#include <GL/glut.h>
#else
#include <GLUT/glut.h>
#endif

#include <vector>
#include <cstdio>
#include <math.h>
#include "vec.h"
#include <iostream>

using namespace std;
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;
vector<vec2> control_points;

float factorial(int n) {
    int ret = 1;
    for (int i = n; i > 0; i--) {
      ret = ret * i;
    }
    return ret;
}

float combination(int n, int k) {
    return factorial(n) / (factorial(k) * factorial(n-k));
}

float binomial(int n, int k, float t) {
    return combination(n, k) * pow(t, k) * pow(1-t, n-k);
}

void GL_render()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glutSwapBuffers();

    //glBegin(GL_LINES);
    glBegin(GL_LINE_STRIP); // use GL_LINE_STRIP to draw line segments between consecutive points
    glColor3f(1.0f,0.0f,0.0f);

    /* just for example, remove if desired
    glVertex2f(-.5f,-.5f);
    glVertex2f(.5f,-.5f);
    glVertex2f(.5f,.5f);
    glVertex2f(-.5f,.5f); */

    if (control_points.size() > 1) {
      for(float t = 0; t <= 1; t = t + 0.01) { // iterate t between 0 and 1 in steps of 0.01
        vec2 pt;
        pt[0] = 0;
        pt[1] = 0;
        for (int k = 0; k < (int)control_points.size(); k++) {
          float mult = binomial(control_points.size() - 1, k, t);
          pt[0] += (mult * control_points[k][0]);
          pt[1] += (mult * control_points[k][1]);
        }
        glVertex2f(pt[0], pt[1]);
      }
    }
    
    glEnd();
    glBegin(GL_POINTS);
    glColor3f(0.0f,1.0f,0.0f);
    
    for (int k = 0; k < (int)control_points.size(); k++) {
      glVertex2f(control_points[k][0], control_points[k][1]);
    }
        
    glEnd();
    glFlush();
}

void GL_mouse(int button,int state,int x,int y)
{
    y=WINDOW_HEIGHT-y;
    GLdouble mv_mat[16];
    GLdouble proj_mat[16];
    GLint vp_mat[4];
    glGetDoublev(GL_MODELVIEW_MATRIX,mv_mat);
    glGetDoublev(GL_PROJECTION_MATRIX,proj_mat);
    glGetIntegerv(GL_VIEWPORT,vp_mat);

    if(button==GLUT_LEFT_BUTTON && state==GLUT_DOWN){
        double px,py,dummy_z; // we don't care about the z-value but need something to pass in
        gluUnProject(x,y,0,mv_mat,proj_mat,vp_mat,&px,&py,&dummy_z);
        vec2 pt;
        pt[0] = px;
        pt[1] = py;
        control_points.push_back(pt); // push back the mouse click coordinates into the vector
        glutPostRedisplay();
    }
}

//Initializes OpenGL attributes
void GLInit(int* argc, char** argv)
{
    glutInit(argc, argv);
    //glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

    //glMatrixMode(GL_PROJECTION_MATRIX);
    //glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, -1, 1);
    glutCreateWindow("CS 130 - window");
    glutDisplayFunc(GL_render);
    glutMouseFunc(GL_mouse);
}

int main(int argc, char** argv)
{
    GLInit(&argc, argv);
    glutMainLoop();
    return 0;
}
