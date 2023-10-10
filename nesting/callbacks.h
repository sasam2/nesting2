#pragma once
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/glui.h>
#include <iostream>
#include "NonFitPolygonsCalculation.h"
#include "LayoutBuilder.h"

//#define DEBUG

using namespace std;

int initGLUT(int argc, char** argv);
void initGL();

//callbacks
void displayCB();
void timerCB(int millisec);
void idleCB();
void reshapeCB(int w, int h);
void solveCB(int dummy);

