#pragma once
#include <GL/glut.h>
#include <opencv2/opencv.hpp>

class BoundingBox
{
	GLdouble x, y, width, height;
public:
	BoundingBox(void);
	BoundingBox(GLdouble minX, GLdouble minY, GLdouble w, GLdouble h);
	~BoundingBox(void);

	GLdouble getX();
	GLdouble getY();
	GLdouble getWidth();
	GLdouble getHeight();
	void normalizeToOrigin();
	bool contains(cv::Point point);
	bool contains(GLdouble* point);

};
