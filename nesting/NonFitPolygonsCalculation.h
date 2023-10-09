#pragma once

#include <vector>
#include "global.h"

using namespace std;

class NonFitPolygonsCalculation {

private:
	static Polygon_2  sortEdgesConvexPoly(Polygon_2 a, Polygon_2 b);
	static Polygon_2 removeRedundantVertices(Polygon_2 a);
public:
	static vector<Polygon_2> sortEdgesWithDecomposition(vector<Polygon_2>   subPolyPa, vector<Polygon_2>   subPolyPb);
	static vector<Polygon_2> minkowskiSumsWithDecompositionNoConversion(vector<Polygon_2>   subPolyPa, vector<Polygon_2>   subPolyPb);

	static Polygon_2  minkowskiSumConvexPoly2_ConvexHull2(Polygon_2 a, Polygon_2 b);
};

