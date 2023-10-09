#include "measurements.h"


double calculateThreshold(Problem p)
{
	return  1.0 / (double)((p.totalOfPieces()/*/nColorComponents*/) + 1);
}


double diffclock(clock_t clock1, clock_t clock2)
{
	double diffticks = clock1 - clock2;
	double diffms = (diffticks * 1000) / CLOCKS_PER_SEC;
	return diffms;
}

double calculateMaxLength(DrawingWithRotations currentDrawingPolys, Polygon_with_holes_2** polygons)
{
	GLdouble max = 0;
	for (int i = 0; i < currentDrawingPolys.getOrder().size(); i++)
	{
		int piece = currentDrawingPolys.getOrder()[i].first;
		int rotation = currentDrawingPolys.getOrder()[i].second;
		GLdouble* position = currentDrawingPolys.getPositions()[i];
		GLdouble maxy = position[0] + (polygons[piece][rotation].bbox().xmax() - polygons[piece][rotation].bbox().xmin());
		max = myMax(maxy, max);
	}
	return max;
}

double calculateUsedArea(Layout layout, int numberOfPolygons, Polygon_with_holes_2** polygons, double maxLength)
{
	double totalarea = maxLength * layout.stockSheetsBoundingBox().getHeight();
	cout << maxLength << " x " << layout.stockSheetsBoundingBox().getHeight() << " = " << totalarea << endl;
	double ocupiedarea = 0;
	//double wastearea=totalarea;
	cout << numberOfPolygons << endl;
	cout << layout.getPieces().size() << endl;
	for (int i = 0; i < numberOfPolygons; i++)
	{
		double pieceArea = polygons[i][0].outer_boundary().area();
		for (Polygon_with_holes_2::Hole_iterator h = polygons[i][0].holes_begin();
			h != polygons[i][0].holes_end();	h++)
			pieceArea += h->area();
		ocupiedarea += pieceArea * layout.getQuantity()[i];
		cout << pieceArea << " x " << layout.getQuantity()[i] << endl;
		//wastearea-=pieceArea*layout.getQuantity()[i];
	}
	cout << ocupiedarea << endl;
	//cout<<wastearea/totalarea<<endl;
	cout << ocupiedarea / totalarea << endl << endl;
	return ocupiedarea / totalarea;
}