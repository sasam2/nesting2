#include "global.h"

GLdouble* createPosition(double x, double y, double z)
{
	GLdouble* position = new GLdouble[4];
	position[0] = x;	position[1] = y;		position[2] = z;		position[3] = 0.0;
	return position;
}

GLfloat* createColor(float r, float g, float b, float a)
{
	GLfloat* color = new GLfloat[4];
	color[0] = r;	color[1] = g;	color[2] = b; color[3] = a;
	return color;
}

BoundingBox getBoundingBox(vector<Polygon_2> poly)
{
	CGAL::Bbox_2 nfpBBpart(0, 0, 0, 0);
	for (int p = 0; p < poly.size(); p++)
	{
		//print_polygon (poly[p]);
		nfpBBpart = nfpBBpart + poly[p].bbox();
		//cout<<nfpBBpart.xmin()<<" "<<nfpBBpart.ymin()<<"; "<<nfpBBpart.xmax()<<" "<<nfpBBpart.ymax()<<endl;
	}
	BoundingBox nfpBB(nfpBBpart.xmin(), nfpBBpart.ymin(), nfpBBpart.xmax() - nfpBBpart.xmin(), nfpBBpart.ymax() - nfpBBpart.ymin());
	return nfpBB;
}


BoundingBox getBoundingBox(Polygon_2 p)
{
	GLdouble maxXY[2];
	GLdouble minXY[2];

	maxXY[0] = 0.0;
	maxXY[1] = 0.0;

	minXY[0] = numeric_limits<double>::infinity();
	minXY[1] = numeric_limits<double>::infinity();

	//escolhe os pontos dos cantos inferior esquerdo e superior direito
	for (Polygon_2::Vertex_iterator vit = p.vertices_begin(); vit != p.vertices_end(); ++vit)
	{
		//cout<<"component "<<i<<" "<<components[i][j][0]<<" "<<components[i][j][1]<<endl;
		double vitX = CGAL::to_double(vit->x()/*.exact()*/);
		double vitY = CGAL::to_double(vit->y()/*.exact()*/);

		minXY[0] = min(vitX, minXY[0]);
		minXY[1] = min(vitX, minXY[0]);

		maxXY[0] = max(vitX, minXY[0]);
		maxXY[1] = max(vitX, minXY[0]);
	}

	BoundingBox bb(minXY[0], minXY[1], maxXY[0] - minXY[0], maxXY[1] - minXY[1]);
	return bb;
}