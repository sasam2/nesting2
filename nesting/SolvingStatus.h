#pragma once

#include "global.h"
#include "Layout.h"
#include "PolyInfo.h"
#include "print_utils.h"
#include "NonFitPolygonsCalculation.h"

class SolvingStatus
{	

private:
	Layout layout;
	static Polygon_with_holes_2 piece2PolygonWithHoles(Piece a, double inv=1.0);
	static int getNumberOfConcaveVertices(Polygon_2 p);
	static Polygon_with_holes_2 generatePieceDiscreteRotation(Polygon_with_holes_2 p, double currentAngle);
	static vector<Polygon_2>** decomposePiecesAngBis(Polygon_with_holes_2** pieces, int dim1, int dim2);
	static vector<Polygon_2> invertPolygonWithHoles(Polygon_with_holes_2 l, double maxPieceWidth, double maxPieceHeight, double resolution);
	static vector<Polygon_2> minkowskiSumsWithDecompositionNoConversion(vector<Polygon_2>   subPolyPa, vector<Polygon_2>   subPolyPb);

public:
	Polygon_with_holes_2** polygons;
	vector<int> piecesAvaliability;
	int numberOfPolygons;
	int nrOfRotations;
	int numberOfNFPs;
	vector<Polygon_2>**** nfpsPiecesAndStockSheet;
	vector<Polygon_2> sSPolyVec;
	map<Point_2, PolyInfo> infos;
	vector<Polygon_2>** polygonsDecompositions;

	SolvingStatus();
	~SolvingStatus();
	void load(Selection& userSelection, Layout& layout);


};

