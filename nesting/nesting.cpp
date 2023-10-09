// nesting.cpp : Este arquivo contém a função 'main'. A execução do programa começa e termina ali.
//

#include <iostream>
#include "callbacks.h"

int main(int argc, char** argv)
{
    // init GLUT and GL
    initGLUT(argc, argv);
    if (GLEW_OK != glewInit()) return 1;
    initGL();

    glutMainLoop();

    return 0;
}