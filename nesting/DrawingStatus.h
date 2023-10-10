#pragma once

#include <vector>
#include "global.h"
#include "SolvingStatus.h"
#include "DrawingWithRotations.h"
#include "display_lists_generation.h"

using namespace std;


class DrawingStatus
{
private:
	void preProcessingStatic(int staticHeuristic, Layout layout, int numberOfPolygons, int nrOfRotations, map<Point_2, PolyInfo> infos);
	void preProcessingDynamic(Layout layout, int numberOfPolygons, int nrOfRotations, int numberOfNFPs, map<Point_2, PolyInfo> infos, vector<Polygon_2>** polygonsDecompositions, vector<Polygon_2>**** nfpsPiecesAndStockSheet);
public:
	DrawingWithRotations currentDrawingPolys;
	map<Point_2, DrawingWithRotations> currentDrawingNFPs;
	GLfloat *blue;
	GLfloat* colorStatic;
	GLdouble* origin;
	list<Point_2> piecesOrdered;
	map<Point_2, DrawingWithRotations> drawingPolysTest;
	map<Point_2, GLuint> layoutNFPsTest;
	map<Point_2, map<Point_2, DrawingWithRotations>> drawingNFPsTest;
	BoundingBox viewport;
	int h, w;
	int tx, ty, tw, th;
	int stockList;
	
	DrawingStatus();
	~DrawingStatus();
	void load(Selection& userSelection, Layout& layout, SolvingStatus& solvingStatus);
	void cleanup();
};

