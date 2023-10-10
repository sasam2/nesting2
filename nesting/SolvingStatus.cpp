#include "SolvingStatus.h"


SolvingStatus::SolvingStatus() {}

void SolvingStatus::load(Selection& userSelection, Layout& layout) {
	//this->layout = layoutArg;

	//NORMALIZAR peças e detectar largura e altura maximas
	double maxPieceWidth = layout.getMaxWidth();
	double maxPieceHeight = layout.getMaxHeight();
	for (int i = 0; i < layout.getPieces().size(); i++)
	{
		/*BoundingBox pieceBB = layout.getPieces()[i].getBoundingBox();
		maxPieceWidth = max(maxPieceWidth,pieceBB.getWidth());		//get max width
		maxPieceHeight = max(maxPieceHeight,pieceBB.getHeight());	//get max height*/
		layout.getPieces()[i].normalizeToOrigin();					//normalizar peças
		layout.getPieces()[i].print();
	}

	//normalizar superficies
	layout.getStockSheet()[0].normalizeToOrigin();

	GLdouble nextOrigin[3];
	nextOrigin[0] = 0.0;
	nextOrigin[1] = 0.0;
	nextOrigin[2] = 0.0;
	for (int i = 1; i < layout.getStockSheet().size(); i++)
	{
		//cout<<"lastPoint[0] "<<nextOrigin[0]<<endl;

		nextOrigin[0] = nextOrigin[0] - (layout.getStockSheet()[i - 1].getBoundingBox().getWidth() + maxPieceWidth);

		layout.getStockSheet()[i].normalizeToOrigin();
		layout.getStockSheet()[i].normalize(nextOrigin);

	}


	//RODAR peças e calcular estatisticas
	numberOfPolygons = layout.getPieces().size();
	numberOfNFPs = numberOfPolygons + 1;
	nrOfRotations = 360 / layout.getRotationStep();
	double angleRadians = ((double)layout.getRotationStep()) * M_PI / 180;

	polygons = new Polygon_with_holes_2 * [numberOfPolygons];
	
	//vector<int> piecesAvaliability;

	for (int i = 0; i < numberOfPolygons; i++)
	{
		//guardar nr de rotacoes disponivel para ser colocado
		piecesAvaliability.push_back(nrOfRotations);
		polygons[i] = new Polygon_with_holes_2[nrOfRotations];
		polygons[i][0] = piece2PolygonWithHoles(layout.getPieces()[i]);

		//area da peça, area da convex hull, nr de vertices concavos
		int nrOfConcaveVertices = getNumberOfConcaveVertices(polygons[i][0].outer_boundary());
		double a = polygons[i][0].outer_boundary().area();

		Polygon_2 convexHull;
		CGAL::convex_hull_2(polygons[i][0].outer_boundary().vertices_begin(), polygons[i][0].outer_boundary().vertices_end(), back_inserter(convexHull));
		double concavity = (convexHull.area() - a) / convexHull.area();

		Polygon_2 boundingBox;
		boundingBox.push_back(Point_2(polygons[i][0].bbox().xmin(), polygons[i][0].bbox().ymin()));
		boundingBox.push_back(Point_2(polygons[i][0].bbox().xmax(), polygons[i][0].bbox().ymin()));
		boundingBox.push_back(Point_2(polygons[i][0].bbox().xmax(), polygons[i][0].bbox().ymax()));
		boundingBox.push_back(Point_2(polygons[i][0].bbox().xmin(), polygons[i][0].bbox().ymax()));
		double rectangularity = boundingBox.area() - polygons[i][0].outer_boundary().area();
		print_polygon(boundingBox);

		//largura e altura da BB
		CGAL::Bbox_2 pBB = polygons[i][0].bbox();
		double w = pBB.xmax() - pBB.xmin();
		double l = pBB.ymax() - pBB.ymin();
		infos[Point_2(i, 0)] = PolyInfo(w, l, a, rectangularity, concavity, nrOfConcaveVertices);

		//rodar as peças e guardar rotaçoes
		for (int r = 1; r < nrOfRotations; r++)
		{
			polygons[i][r] = generatePieceDiscreteRotation(polygons[i][0], angleRadians * r);
			//cout<<"clkwse "<<i<<" "<<r<<" "<<polygons[i][r].outer_boundary().is_counterclockwise_oriented()<<endl;
						
			pBB = polygons[i][r].bbox();
			w = pBB.xmax() - pBB.xmin();
			l = pBB.ymax() - pBB.ymin();
			infos[Point_2(i, r)] = PolyInfo(w, l, a, rectangularity, concavity, nrOfConcaveVertices);
		}
	}
	//Polygon_with_holes_2 stockSheetPoly = piece2PolygonWithHoles(layout.getStockSheet()[0]);
	stockSheetsPoly = new Polygon_with_holes_2[layout.getStockSheet().size()];

	for (int ss = 0; ss < layout.getStockSheet().size(); ss++) {
		stockSheetsPoly[ss] = piece2PolygonWithHoles(layout.getStockSheet()[ss]);
	}

	//DECOMPOSICAO

	//decompor peças
	polygonsDecompositions = decomposePiecesAngBis(polygons, numberOfPolygons, nrOfRotations);

	//decompor stock sheets
	//inverter
	stockSheetsPolyForInnerFP = new vector<Polygon_2>[layout.getStockSheet().size()];
	for (int ss = 0; ss < layout.getStockSheet().size(); ss++)
	{
		if (!(stockSheetsPoly[ss].outer_boundary()).is_simple())
		{
			cout << "Error stock sheet " << ss << ":\n";
			print_polygon(stockSheetsPoly[ss].outer_boundary());
		}
		stockSheetsPolyForInnerFP[ss] = invertPolygonWithHoles(stockSheetsPoly[ss], maxPieceWidth, maxPieceHeight, layout.getResolution());
	}

	//decompor
	CGAL::Small_side_angle_bisector_decomposition_2<Kernel> decompAB;

	for (int ss = 0; ss < layout.getStockSheet().size(); ss++)
	{
		for (int i = 0; i < stockSheetsPolyForInnerFP[ss].size(); i++)
		{
			if (!(stockSheetsPolyForInnerFP[ss][i]).is_simple())
			{
				cout << "Error inverted stock sheet " << ss << ":\n";
				print_polygon(stockSheetsPolyForInnerFP[ss][i]);
			}
			decompAB(stockSheetsPolyForInnerFP[ss][i], back_inserter(sSPolyVec));
		}
	}

	//CALCULO DOS NFPs e IFPs 

	//generate no fit polygons e inner fit polygons
	nfpsPiecesAndStockSheet = new vector<Polygon_2>***[numberOfPolygons];
	for (int i = 0; i < numberOfPolygons; i++)//para cada poligono movel
	{
		nfpsPiecesAndStockSheet[i] = new vector<Polygon_2>**[nrOfRotations];
		for (int ri = 0; ri < nrOfRotations; ri++)//para cada rotacao do poligono movel
		{
			nfpsPiecesAndStockSheet[i][ri] = new vector<Polygon_2>*[numberOfNFPs];
			for (int j = 0; j < numberOfNFPs - 1; j++)//para cada poligono fixo e sotck sheet
			{
				nfpsPiecesAndStockSheet[i][ri][j] = new vector<Polygon_2>[nrOfRotations];
				for (int rj = 0; rj < nrOfRotations; rj++)//para cada rotacao do poligono fixo
				{
					double time;
					//nfpsPiecesAndStockSheet[i][ri][j][rj]=minkowskiSumsWithDecompositionNoConversion(polygonsDecompositions[j][rj], polygonsDecompositions[i][ri]/*, &time*/);
					//nfpsPiecesAndStockSheet[i][ri][j][rj]=sortEdgesWithDecomposition(polygonsDecompositions[j][rj], polygonsDecompositions[i][ri]);
					if (userSelection.nfpsCalculation == 0) {
						nfpsPiecesAndStockSheet[i][ri][j][rj] = NonFitPolygonsCalculation::sortEdgesWithDecomposition(polygonsDecompositions[j][rj], polygonsDecompositions[i][ri]);
					} else {
						nfpsPiecesAndStockSheet[i][ri][j][rj] = NonFitPolygonsCalculation::minkowskiSumsWithDecompositionNoConversion(polygonsDecompositions[j][rj], polygonsDecompositions[i][ri]);
					}
					
				}
			}
			//ifp para a stock sheet
			nfpsPiecesAndStockSheet[i][ri][numberOfNFPs - 1] = new vector<Polygon_2>[1];
			nfpsPiecesAndStockSheet[i][ri][numberOfNFPs - 1][0] = NonFitPolygonsCalculation::minkowskiSumsWithDecompositionNoConversion(sSPolyVec, polygonsDecompositions[i][ri]);
		}
	}

	
}


Polygon_with_holes_2 SolvingStatus::piece2PolygonWithHoles(Piece a, double inv)
{
	Polygon_with_holes_2   pa;

	//cout<<"before"<<inv*0.0<<endl;
	//a.print();

	for (int i = 0; i < a.getComponents().size(); i++)
	{
		Component c = a.getComponents()[i];
		Polygon_2 newComponent;

		for (int j = 0; j < c.size(); j++)
			newComponent.push_back(Point_2(inv * c[j][0], inv * c[j][1]));

		if (i == 0)
			pa = Polygon_with_holes_2(newComponent);
		else
			pa.add_hole(newComponent);
	}
	//if(inv<0)
	//{
	//cout<<"after"<<endl;
	//print_polygon_with_holes (pa);
	//}
	return pa;
}

int SolvingStatus::getNumberOfConcaveVertices(Polygon_2 p)
{
	int numberOfConcaveVertices = 0;

	vector<Point_2> vertices = p.container();
	for (int v = 0; v < vertices.size(); v++)
	{
		Point_3 p0 = Point_3(vertices[(v - 1 + vertices.size()) % vertices.size()].x(), vertices[(v - 1 + vertices.size()) % vertices.size()].y(), 0);
		Point_3 p1 = Point_3(vertices[v].x(), vertices[v].y(), 0);
		Point_3 p2 = Point_3(vertices[(v + 1) % vertices.size()].x(), vertices[(v + 1) % vertices.size()].y(), 0);

		Vector_3 v0(p0, p1);
		Vector_3 v1(p1, p2);
		Vector_3 crossProduct = CGAL::cross_product(v0, v1);

		if (crossProduct.z() < 0) //se o produto vectorial e positivo, e um vertice concavo
		{
			numberOfConcaveVertices++;
		}
	}
	return numberOfConcaveVertices;
}

Polygon_with_holes_2 SolvingStatus::generatePieceDiscreteRotation(Polygon_with_holes_2 p, double currentAngle)
{
	//print_polygon_with_holes(p);
	Transformation rotate(CGAL::ROTATION, sin(currentAngle), cos(currentAngle));
	//cout<<"Angle="<<currentAngle<<endl;

	//rodar outer boundary
	Polygon_2 rotatedPOuterBoundary;
	for (Polygon_2::Vertex_iterator vit = p.outer_boundary().vertices_begin(); vit != p.outer_boundary().vertices_end(); ++vit)
	{
		Point_2 currentPoint = *vit;
		rotatedPOuterBoundary.push_back(rotate(currentPoint));
	}
	//cout<<"Rotated outer boundary: "<<endl;
	//print_polygon(rotatedPOuterBoundary);

	Vector_2 lowLeft(-rotatedPOuterBoundary.bbox().xmin(), -rotatedPOuterBoundary.bbox().ymin());
	Transformation normalizeToOrigin(CGAL::TRANSLATION, lowLeft);
	//cout<<"Vector: "<<lowLeft.hx()<<" "<<lowLeft.hy()<<endl;
	//Polygon_2 rotatedPOuterBoundaryNormalized;
	for (Polygon_2::Vertex_iterator vit = rotatedPOuterBoundary.vertices_begin(); vit != rotatedPOuterBoundary.vertices_end(); ++vit)
	{
		Point_2 currentPoint = *vit;
		*vit = normalizeToOrigin(currentPoint);
		//cout<<"Before: "<<currentPoint<<" after: "<<*vit<<endl;
	}
	//cout<<"Rotated outer boundary normalized: "<<endl;
	//print_polygon(rotatedPOuterBoundary);

	Polygon_with_holes_2 rotatedP(rotatedPOuterBoundary);
	for (Polygon_with_holes_2::Hole_iterator hit = p.holes_begin(); hit != p.holes_end(); ++hit)
	{
		Polygon_2 currentHole;
		for (Polygon_2::Vertex_iterator vit = hit->vertices_begin(); vit != hit->vertices_end(); ++vit)
		{
			Point_2 currentPoint = *vit;
			currentHole.push_back(normalizeToOrigin(rotate(currentPoint)));
		}
		rotatedP.add_hole(currentHole);
	}

	//print_polygon_with_holes(rotatedP);

	return rotatedP;
}


vector<Polygon_2>** SolvingStatus::decomposePiecesAngBis(Polygon_with_holes_2** pieces, int dim1, int dim2)
{
	clock_t begin, end;
	CGAL::Small_side_angle_bisector_decomposition_2<Kernel> decompAB;
	vector<Polygon_2>** result;

	result = new vector<Polygon_2>*[dim1];
	for (int i = 0; i < dim1; i++)
	{
		//cout<<i<<endl;
		result[i] = new vector<Polygon_2>[dim2];
		if (!(pieces[i][0].outer_boundary()).is_simple())
		{
			cout << "Error polygon " << i << ":\n";
			print_polygon(pieces[i][0].outer_boundary());
		}

		for (int j = 0; j < dim2; j++)
		{
			//cout<<"   "<<j<<endl;
			//Create polygons
			print_polygon(pieces[i][j].outer_boundary());
			vector<Polygon_2> subPolyPa;
			decompAB(pieces[i][j].outer_boundary(), back_inserter(subPolyPa));
			result[i][j] = subPolyPa;
		}
	}
	return result;
}


vector<Polygon_2> SolvingStatus::invertPolygonWithHoles(Polygon_with_holes_2 l, double maxPieceWidth, double maxPieceHeight, double resolution)
{
	vector<Polygon_2> layoutForInnerFitPolygon;
	CGAL::Bbox_2 bbox = l.outer_boundary().bbox();
	Polygon_2::Vertex_circulator vc = l.outer_boundary().vertices_circulator();

	if (l.outer_boundary().is_counterclockwise_oriented())
		l.outer_boundary().reverse_orientation();

	//bboxPolyBL
	Polygon_2 bboxPoly;
	bboxPoly.push_back(Point_2(bbox.xmin() - maxPieceWidth, bbox.ymin() - maxPieceHeight));

	Point_2 firstPoint(numeric_limits<double>::max(), numeric_limits<double>::max());//vertice de baixo a direita
	Polygon_2::Vertex_circulator firstPointCirculator;			//iterator para vertice de baixo a direita
	Polygon_2::Vertex_const_circulator searchBegin = vc;		//inicio da pesquisa do vertice "firstPointCirculator"
	do {
		if (vc->y() == bbox.ymin() && vc->x() < firstPoint.x())	//se vc e um vertice de baixo a direita que "firstPoint"
		{	//gravar esse vertice em "firstPoint" e respectivo iterator em "firstPointCirculator"
			firstPoint = *vc;
			firstPointCirculator = vc;
		}
	} while (++vc != searchBegin);
	bboxPoly.push_back(firstPoint);

	vc = firstPointCirculator;	//colocar o iterator "vc" a apontar para o primeiro ponto ("firstPointCirculator")
	Polygon_2::Vertex_const_circulator begin = vc++;
	for (; vc != begin; vc++)
		bboxPoly.push_back(*vc);

	bboxPoly.push_back(Point_2(vc->x() + resolution / 2, vc->y()));
	bboxPoly.push_back(Point_2(bbox.xmin() - maxPieceWidth + resolution / 2, vc->y() - maxPieceHeight));
	bboxPoly.push_back(Point_2(bbox.xmax() + maxPieceWidth, bbox.ymin() - maxPieceHeight));
	bboxPoly.push_back(Point_2(bbox.xmax() + maxPieceWidth, bbox.ymax() + maxPieceHeight));
	bboxPoly.push_back(Point_2(bbox.xmin() - maxPieceWidth, bbox.ymax() + maxPieceHeight));

	layoutForInnerFitPolygon.push_back(bboxPoly);
	for (Polygon_with_holes_2::Hole_iterator hit = l.holes_begin(); hit != l.holes_end(); ++hit)
	{
		if (l.outer_boundary().is_counterclockwise_oriented())
			hit->reverse_orientation();
		layoutForInnerFitPolygon.push_back(Polygon_2(*hit));
	}
	//print_polygon(bboxPoly);
	return layoutForInnerFitPolygon;
}

vector<Polygon_2>  SolvingStatus::minkowskiSumsWithDecompositionNoConversion(vector<Polygon_2>   subPolyPa, vector<Polygon_2>   subPolyPb)
{
	clock_t begin, end;
	//Transformation rotate(CGAL::ROTATION, sin(M_PI), cos(M_PI));
	Transformation scale(CGAL::SCALING, -1);

	vector<Polygon_2> subPolySymPb;
	for (vector<Polygon_2>::iterator jt = subPolyPb.begin(); jt != subPolyPb.end(); jt++) {
		Polygon_2 p;
		for (int i = 0; i < jt->size(); i++) {
			p.push_back((*jt)[i].transform(scale));
		}
		subPolySymPb.push_back(p);
	}

	//calcular as somas
	vector<Polygon_2> sums;
	for (vector<Polygon_2>::iterator it = subPolyPa.begin(); it != subPolyPa.end(); it++)
		for (vector<Polygon_2>::iterator jt = subPolySymPb.begin(); jt != subPolySymPb.end(); jt++) {
			sums.push_back(NonFitPolygonsCalculation::minkowskiSumConvexPoly2_ConvexHull2(*it, *jt));
		}

	/*
	vector<Polygon_2> sums;
	for(list<Polygon_2>::iterator it=subPolyPa.begin(); it != subPolyPa.end(); it++)
		for(list<Polygon_2>::iterator jt=subPolyPb.begin(); jt != subPolyPb.end(); jt++){
			for(int i=0; i <jt->size(); i++){
				(*jt)[i]=(*jt)[i].transform(scale);
			}
			cout<<"ola"<<endl;
			print_polygon(*jt);
			sums.push_back(minkowskiSumConvexPoly2_ConvexHull2(*it, *jt));
		}
	*/
	return sums;
}



SolvingStatus::~SolvingStatus() {

}

void SolvingStatus::cleanup() {

	for (int i = 0; i < numberOfPolygons; i++) {
		delete[] polygons[i];
	}
	delete[] polygons;
	delete[] stockSheetsPoly;
	for (int i = 0; i < numberOfPolygons; i++) {
		delete[] polygonsDecompositions[i];		
	}
	delete[] polygonsDecompositions;
	delete[] stockSheetsPolyForInnerFP;
	
	for (int i = 0; i < numberOfPolygons; i++)
	{
		for (int ri = 0; ri < nrOfRotations; ri++)
		{
			for (int j = 0; j < numberOfNFPs; j++)
			{
				delete[] nfpsPiecesAndStockSheet[i][ri][j];
			}
			delete[] nfpsPiecesAndStockSheet[i][ri];
		}
		delete[] nfpsPiecesAndStockSheet[i];
	}
	delete[] nfpsPiecesAndStockSheet;


}