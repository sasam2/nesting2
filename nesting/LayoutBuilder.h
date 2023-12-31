#pragma once
//#define DEBUG
#include <GL/glew.h>
#include <GL/glui.h>
#include <opencv2/core/core_c.h>
#include "measurements.h"
#include "DrawingStatus.h"

using namespace std;

class LayoutBuilder
{
private:
	static int iteration;

	static int* placedPieces;
	static GLfloat* blue;
	static GLfloat* green;
	static vector<int> piecesToPlace;


	static GLdouble* getPiecePosition(cv::Mat layoutImg, Layout layout);
	static vector<vector<cv::Point>> getFeasiblePositions(cv::Mat image);
	static vector<vector<cv::Point>> showContours(cv::Mat thrImgMat);
	static cv::Point bottomLeft(vector<vector<cv::Point>> feasiblePositions);
	static pair<vector<vector<cv::Point>>, vector<cv::Vec4i>> getContours(cv::Mat mat, double maxDist);
	static GLdouble* bottomLeft(vector<vector<GLdouble*>> feasiblePositions);
	static vector<Piece> contourVecPieceVec(vector<vector<cv::Point>> contours);
	static Component pointVec2Component(vector<cv::Point> poly, bool rev = false);
	static void displayDynamic_drawCurrentLayout(DrawingStatus& drawingStatus);
	static Point_2 dynamicPieceSelection(DrawingStatus drawingStatus);
	static cv::Mat getOpenCVImage(int xb, int yb, int width, int height, int channel=GL_BLUE);

public:
	static void buildStaticLayout(Selection& userSelection, Layout& layout, SolvingStatus& solvingStatus, DrawingStatus& drawingStatus);
	static bool displayStatic(Selection& userSelection, Layout& layout, SolvingStatus& solvingStatus, DrawingStatus& drawingStatus, ofstream* myfile);
	static bool displayDynamic(Selection& userSelection, Layout& layout, SolvingStatus& solvingStatus, DrawingStatus& drawingStatus, ofstream* myfile);
	static Point_2 putPieceDynamic(Layout& layout, SolvingStatus& solvingStatus, DrawingStatus& drawingStatus, bool reset);
	static void buildDynamicLayout(Selection& userSelection, Layout& layout, SolvingStatus& solvingStatus, DrawingStatus& drawingStatus);
	static void cleanup();
};

