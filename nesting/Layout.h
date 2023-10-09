#pragma once
#include "Problem.h"

#define myMax(x,y) (x)>(y)?(x):(y)
#define myMin(x,y) (x)<(y)?(x):(y)

class Layout : public Problem
{
	//Piece stockSheet;
	vector<Piece> stockSheet;
	int rotationStep, margin;
	double resolution;
	vector<GLfloat*> positions;
	vector<int> order;

public:
	Layout();
	Layout(Problem p, vector<Piece> ss, int rot);
	Layout(Problem p, vector<Piece> ss, int rot, double res);
	Layout(Problem p, vector<Piece> ss, int rot, double res, int marg);
	Layout(Problem p, vector<Piece> ss);
	Layout(vector<Piece> ps, vector<Piece> ss);
	virtual ~Layout(void);

	vector<Piece> getStockSheet();
	void setStockSheet(vector<Piece> ss);

	GLdouble* generateRandomPosition(int p);
	int chooseRandomPiece();

	int getRotationStep() { return rotationStep; }
	double getResolution() { return resolution; }
	int getMargin() { return margin; }

	double getMaxWidth();
	double getMaxHeight();
	BoundingBox stockSheetsBoundingBox();
};

