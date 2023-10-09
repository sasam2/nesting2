#include "Problem.h"

//Scope::Problem
Problem::Problem()
{
}

Problem::~Problem()
{
}

void Problem::addPiece(Piece newPiece, int quant/*, double rot[]*/) {
	pieces.push_back(newPiece);
	quantity.push_back(quant);
}

void Problem::popPiece() {
	pieces.pop_back();
	quantity.pop_back();
}

void Problem::print() {
	for (int i = 0; i < pieces.size(); i++)
	{
		cout << "Piece " << i << endl;
		pieces[i].print();
		cout << "   Quantity " << quantity[i] << endl;
	}

}
/*
int Problem::decPieceQuantity(int piece){
	if(quantity[piece])
		return --quantity[piece];
	else
		return -1;
}
*/
/*
bool Problem::empty(){
	int i;
	for(i=0; i<quantity.size() && !quantity[i]; i++);
	return i==quantity.size();
}*/

vector<Piece> Problem::getPieces() {
	return pieces;
}

vector<int> Problem::getQuantity() {
	return quantity;
}

int Problem::totalOfPieces() {
	int acc = 0;
	for (int i = 0; i < quantity.size(); i++)
		acc += quantity[i];
	return acc;
}

int Problem::maxHeight()
{
	int maxHeight = 0;
	/*
	GLdouble **boundingBox = new GLdouble*[4];
	for(int i=0;i<4;i++)
		boundingBox[i]=new GLdouble[3];
	*/
	for (int i = 0; i < pieces.size(); i++)
	{
		BoundingBox pieceBB = pieces[i].getBoundingBox();
		int maxY = pieceBB.getY() + pieceBB.getHeight();
		if (maxY > maxHeight)
			maxHeight = maxY;
	}
	/*
	for(int i=0;i<4;i++)
		delete[] boundingBox[i];
	delete[] boundingBox;
	*/
	return maxHeight;
}