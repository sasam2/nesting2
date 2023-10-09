#pragma once
#include <vector>
#include "global.h"
#include <GL/glut.h>
#include "BoundingBox.h"

using namespace std;

class DrawingWithRotations
{
	vector<vector<int>> displayLists;
	vector<vector<BoundingBox>> boundingBoxes;

	vector<GLdouble*> positions;
	vector<GLfloat*> colors;
	vector<pair<int, int>> order;

	BoundingBox viewPort;

public:
	GLuint bufferObject;

	DrawingWithRotations();
	DrawingWithRotations(int nrOfPolygons, int nrOfRotations);
	~DrawingWithRotations();

	int addDisplayList(int polygon, int rotation, int dl, BoundingBox bb);
	void addListPlacement(int polygon, int rotation, GLdouble* position, GLfloat* color);
	void popLastPlacement();
	//int countPlacedList(int list);
	void resetPlacements();
	void clear();
	vector<GLdouble*> getPositions();
	vector<pair<int, int>> getOrder();
	vector<vector<int>> getLists();
	vector<GLfloat*> getColors();
	BoundingBox getViewPort();
	void setPiecesPositions(vector<GLdouble*> pos, vector<pair<int, int>> ord);
	void addDisplayListsFromPolygonVector(vector<Polygon_2>** nfps, int size1, int size2, int (*generateDisplayList) (vector<Polygon_2>));
	void addDisplayListsFromPolygonVector(Polygon_with_holes_2** polys, int size1, int size2, int (*generateDisplayList) (Polygon_with_holes_2));
	void addNFPsDisplayListsFromPolygonVector(vector<Polygon_2>** polys, int size1, int size2, int (*generateDisplayList) (vector<Polygon_2>));

	void setViewPort(BoundingBox v);

	BoundingBox generateGridPositions();
};