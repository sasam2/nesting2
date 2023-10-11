#pragma once
#include <fstream>
#include "Problem.h"


#define myMax(x,y) (x)>(y)?(x):(y)
#define myMin(x,y) (x)<(y)?(x):(y)

class Layout : public Problem
{
private:
	//Piece stockSheet;
	vector<Piece> stockSheet;
	int rotationStep, margin;
	double resolution;
	vector<int> order;

	Problem loadFile(const char* baseFolder, char* filename, bool mode);
	vector<string> loadConfigurationFile(const char* baseFolder, char* filename);
	void getFirstNonEmptyLine(istream* s, string* line);

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

	void load(const char* folder, char* problemName);

	void cleanup();
};

