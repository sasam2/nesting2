#pragma once
#include <time.h>
#include "global.h"
#include "DrawingWithRotations.h"
#include "Problem.h"
#include "Layout.h"


#define myMax(x,y) (x)>(y)?(x):(y)
#define myMin(x,y) (x)<(y)?(x):(y)

using namespace std;

double calculateThreshold(Problem p);
double diffclock(clock_t clock1, clock_t clock2);
double calculateMaxLength(DrawingWithRotations currentDrawingPolys, Polygon_with_holes_2** polygons);
double calculateUsedArea(Layout layout, int numberOfPolygons, Polygon_with_holes_2** polygons, double maxLength);