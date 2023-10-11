#pragma once
#include "BoundingBox.h"

using namespace std;

typedef vector<GLdouble*> Component;

class Piece
{
protected:
	vector<Component> components;
public:
	Piece(void);
	Piece(Component c);
	Piece(vector<Component> c);
	~Piece(void);

	vector<Component> getComponents();
	void addComponent(Component c);
	void eraseComponent(int i);
	//void getBoundingBox(GLdouble** boundingBox);
	void normalize(GLdouble point[3]);
	void normalizeToOrigin();
	void print();
	BoundingBox getBoundingBox();
	friend ostream& operator<<(ostream& c, Piece p);
	void clear();
};