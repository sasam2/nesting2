#include "BoundingBox.h"

BoundingBox::BoundingBox(void) {}

BoundingBox::BoundingBox(GLdouble minX, GLdouble minY, GLdouble w, GLdouble h)
{
	x = minX;
	y = minY;
	width = w;
	height = h;
}

BoundingBox::~BoundingBox(void)
{
}

GLdouble BoundingBox::getX()
{
	return x;
}

GLdouble BoundingBox::getY()
{
	return y;
}

GLdouble BoundingBox::getWidth()
{
	return width;
}

GLdouble BoundingBox::getHeight()
{
	return height;
}

void BoundingBox::normalizeToOrigin()
{
	width -= x;
	height -= y;
	x = 0;
	y = 0;
}
bool BoundingBox::contains(cv::Point point)
{
	return point.x >= x && point.x <= (x + width) && point.y >= y && point.y <= (y + height);
}
bool BoundingBox::contains(GLdouble* point)
{
	return point[0] >= x && point[0] <= (x + width) && point[1] >= y && point[1] <= (y + height);
}