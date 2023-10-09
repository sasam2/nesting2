#pragma once
#include <vector>
#include "global.h"

using namespace std;

int generatePolygonDisplayList(vector<Polygon_2> p);
int generatePolygonWHolesDisplayList(CGAL::Polygon_with_holes_2<Kernel> p);
void CALLBACK tessBeginCB(GLenum which);
void CALLBACK tessEndCB();
void CALLBACK tessVertexCB(const GLvoid* data);
void CALLBACK tessErrorCB(GLenum errorCode);