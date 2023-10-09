#include "Layout.h"

Layout::Layout() 
{

}

Layout::Layout(vector<Piece> ps, vector<Piece> ss)
{
	pieces = ps;
	stockSheet = ss;
}


Layout::~Layout(void)
{

}


Layout::Layout(Problem p, vector<Piece> ss)
{
	//problem=p;
	pieces = p.getPieces();
	quantity = p.getQuantity();
	stockSheet = ss;
	rotationStep = 360;
	resolution = stockSheet[0].getBoundingBox().getHeight() / 1000;
	margin = 0;
}

Layout::Layout(Problem p, vector<Piece> ss, int rot)
{
	pieces = p.getPieces();
	quantity = p.getQuantity();
	stockSheet = ss;
	rotationStep = rot;
	resolution = stockSheet[0].getBoundingBox().getHeight() / 1000;
	margin = 0;
}

Layout::Layout(Problem p, vector<Piece> ss, int rot, double res)
{
	pieces = p.getPieces();
	quantity = p.getQuantity();
	stockSheet = ss;
	rotationStep = rot;
	resolution = res;
	margin = 0;
}

Layout::Layout(Problem p, vector<Piece> ss, int rot, double res, int marg)
{
	pieces = p.getPieces();
	quantity = p.getQuantity();
	stockSheet = ss;
	rotationStep = rot;
	resolution = res;
	margin = marg;
}

vector<Piece> Layout::getStockSheet()
{
	return stockSheet;
}

GLdouble* Layout::generateRandomPosition(int p)
{
	GLdouble point[3];
	BoundingBox pieceBB = pieces[p].getBoundingBox();
	BoundingBox stockSheetBB = stockSheet[0].getBoundingBox();
	/*
	if(pieceBB.getX() || pieceBB.getY())
	{
		p=p.clone();
		p.normalizeToOrigin();
		pieceBB = p.getBoundingBox();
	}
	*/
	point[0] = ((float)rand() / RAND_MAX) * (stockSheetBB.getWidth() - pieceBB.getWidth()) + (stockSheetBB.getX());
	point[1] = ((float)rand() / RAND_MAX) * (stockSheetBB.getHeight() - pieceBB.getHeight()) + (stockSheetBB.getY());
	point[2] = 0.0;

	return point;
}

int Layout::chooseRandomPiece()
{
	return rand() % pieces.size();
}


void Layout::setStockSheet(vector<Piece> ss)
{
	stockSheet = ss;
}

double Layout::getMaxWidth()
{
	double maxPieceWidth = 0;

	for (int i = 0; i < pieces.size(); i++)
	{
		BoundingBox pieceBB = pieces[i].getBoundingBox();
		maxPieceWidth = max(maxPieceWidth, pieceBB.getWidth());		//get max width
	}

	return maxPieceWidth;
}

double Layout::getMaxHeight()
{
	double maxPieceHeight = 0;

	for (int i = 0; i < pieces.size(); i++)
	{
		BoundingBox pieceBB = pieces[i].getBoundingBox();
		maxPieceHeight = max(maxPieceHeight, pieceBB.getHeight());	//get max height
	}

	return maxPieceHeight;
}

BoundingBox Layout::stockSheetsBoundingBox()
{
	double minX, minY;
	minX = 0;
	minY = 0;
	double w = 0;
	double h = 0;

	for (int p = 0; p < stockSheet.size(); p++)
	{
		BoundingBox spBB = stockSheet[p].getBoundingBox();

		minX = myMin(minX, spBB.getX());
		minY = myMin(minY, spBB.getY());

		h = myMax(h, spBB.getHeight());
		w += spBB.getWidth();
	}

	return BoundingBox(minX, minY, w, h);
}