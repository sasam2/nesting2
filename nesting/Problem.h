#pragma once
#include "piece.h"

using namespace std;

class Problem
{
protected:
	vector<Piece> pieces;
	vector<int> quantity;

public:
	Problem();
	virtual ~Problem();
	void addPiece(Piece newPiece, int quant/*, double rot[]*/);
	void popPiece();
	void print();
	//int decPieceQuantity(int piece);
	//bool empty();
	vector<Piece> getPieces();
	vector<int> getQuantity();
	int totalOfPieces();
	int maxHeight();
};