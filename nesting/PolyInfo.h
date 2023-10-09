#pragma once
class PolyInfo
{
public:
	double width, height, area, concavity, rectangularity;
	int numberOfConcaveVertices;
	PolyInfo() {}
	PolyInfo(double w, double l, double a, double r, double c, int nCV) {
		width = w;		height = l;		area = a;		rectangularity = r;	concavity = c;	numberOfConcaveVertices = nCV;
	}
	~PolyInfo() {}

};
