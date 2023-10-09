#include "Piece.h"

//Piece
Piece::Piece(void)
{
}

Piece::Piece(Component c)
{
	addComponent(c);
}


Piece::Piece(vector<Component> c)
{
	components = c;
}


Piece::~Piece(void)
{
	/*for (int i = 0; i < components.size(); i++) {
		for (int j = 0; j < components[i].size(); j++) {
			delete components[i][j];
		}
	}*/
}

vector<Component> Piece::getComponents()
{
	return components;
}

void Piece::eraseComponent(int i)
{
	vector<Component>::iterator it = components.begin();
	it += i;
	components.erase(it);
}

void Piece::addComponent(Component c)
{
	components.push_back(c);
}

BoundingBox Piece::getBoundingBox()
{

	GLdouble maxXY[2];
	GLdouble minXY[2];

	maxXY[0] = 0.0;
	maxXY[1] = 0.0;

	minXY[0] = numeric_limits<double>::infinity();
	minXY[1] = numeric_limits<double>::infinity();


	//escolhe os pontos dos cantos inferior esquerdo e superior direito
	for (int i = 0; i < components.size(); i++)
	{
		for (int j = 0; j < components[i].size(); j++)
		{
			//cout<<"component "<<i<<" "<<components[i][j][0]<<" "<<components[i][j][1]<<endl;
			if (components[i][j][0] < minXY[0]) minXY[0] = components[i][j][0];
			if (components[i][j][1] < minXY[1]) minXY[1] = components[i][j][1];

			if (components[i][j][0] > maxXY[0]) maxXY[0] = components[i][j][0];
			if (components[i][j][1] > maxXY[1]) maxXY[1] = components[i][j][1];
		}
	}

	BoundingBox bb(minXY[0], minXY[1], maxXY[0] - minXY[0], maxXY[1] - minXY[1]);
	return bb;
}

void Piece::normalize(GLdouble point[3]) {
	//cout<<"Normalize to point "<<point[0]<<" "<<point[1]<<" "<<point[2]<<endl;
	for (int i = 0; i < components.size(); i++)
	{
		for (int j = 0; j < components[i].size(); j++)
		{
			components[i][j][0] -= point[0];
			components[i][j][1] -= point[1];
			components[i][j][2] -= point[2];
			//cout<<"A"<<j+i<<" "<<components[i][j][0]<<" "<<components[i][j][1]<<" "<<components[i][j][2]<<endl;
		}
	}
}

void Piece::normalizeToOrigin()
{
	BoundingBox pieceBB = getBoundingBox();
	GLdouble referencePoint[3] = { 0.0, 0.0, 0.0 };

	referencePoint[0] = pieceBB.getX();
	referencePoint[1] = pieceBB.getY();
	normalize(referencePoint);
}


void Piece::print() {
	cout << *this;
}

ostream& operator<<(ostream& c, Piece p) {
	for (int i = 0; i < p.components.size(); i++) {
		c << "   Component" << endl;
		for (int j = 0; j < p.components[i].size(); j++)
			c << "      " << p.components[i][j][0] << ' ' << p.components[i][j][1] << ' ' << p.components[i][j][2] << endl;
	}
	return c;
}