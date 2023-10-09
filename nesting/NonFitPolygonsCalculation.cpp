#include "NonFitPolygonsCalculation.h"

vector<Polygon_2> NonFitPolygonsCalculation::sortEdgesWithDecomposition(vector<Polygon_2>   subPolyPa, vector<Polygon_2>   subPolyPb)
{
	vector<Polygon_2> sums;

	for (vector<Polygon_2>::iterator it = subPolyPa.begin(); it != subPolyPa.end(); it++)
	{
		for (vector<Polygon_2>::iterator jt = subPolyPb.begin(); jt != subPolyPb.end(); jt++)
		{
			//sort
			sums.push_back(sortEdgesConvexPoly(*it, *jt));
		}
	}
	return sums;

}

Polygon_2  NonFitPolygonsCalculation::sortEdgesConvexPoly(Polygon_2 a, Polygon_2 b)
{
	Construct_vector_2 f_vector;
	list<Vector_2> slopes;

	CGAL::Bbox_2 bboxA = a.bbox();
	Point_2 lastPoint(numeric_limits<double>::max(), numeric_limits<double>::max());//vertice da direita mais abaixo
	list<Vector_2>::iterator lastPointIterator;										//iterator para vertice da direita mais abaixo
	list<Vector_2>::iterator itLastInsertedElement;

	a = removeRedundantVertices(a);
	b = removeRedundantVertices(b);

	//Ordenar e adicionar vectores do poligono A
	Polygon_2::Vertex_const_circulator a_it_begin = a.vertices_circulator();
	Polygon_2::Vertex_circulator a_it = a_it_begin;
	do {

		Point_2 p0 = *a_it;
		Point_2 p1 = *(++a_it);
		Vector_2 slope(p0, p1);

		list<Vector_2>::iterator it;
		for (it = slopes.begin(); it != slopes.end() && it->direction() < slope.direction(); it++);
		itLastInsertedElement = slopes.insert(it, slope);

		if (p0.x() == bboxA.xmax() && p0.y() < lastPoint.y()) //se vc e um vertice a esquerda e esta mais abaixo que "firstPoint"
		{
			//gravar esse vertice em "firstPoint" e respectivo iterator em "firstPointCirculator"
			lastPoint = p0;
			lastPointIterator = itLastInsertedElement;
		}

	} while (a_it != a_it_begin);

	//Ordenar e adicionar vectores do poligono B
	Polygon_2::Vertex_const_circulator b_it_begin = b.vertices_circulator();
	Polygon_2::Vertex_circulator b_it = b_it_begin;

	CGAL::Bbox_2 bboxB = b.bbox();
	Point_2 leftBottomPoint(numeric_limits<double>::max(), numeric_limits<double>::max());//vertice da direita mais abaixo

	list<Vector_2>::iterator itCurrentlyInsertedVector = slopes.end();
	list<Vector_2>::iterator itLastInsertedVector = slopes.end();
	list<Vector_2>::iterator itLeftBottomPoint = slopes.end();
	itLastInsertedElement = slopes.end();
	do {
		Point_2 p0 = *b_it;
		Point_2 p1 = *(--b_it);
		Vector_2 slope(p0, p1);

		list<Vector_2>::iterator it;
		for (it = slopes.begin(); it != slopes.end() && it->direction() < slope.direction(); it++);

		itLastInsertedVector = itCurrentlyInsertedVector;
		itCurrentlyInsertedVector = slopes.insert(it, slope);

		if (p0.x() == bboxB.xmin() && p0.y() < leftBottomPoint.y()) //se vc e um vertice a esquerda e esta mais abaixo que "firstPoint"
		{
			leftBottomPoint = p0;	//gravar esse vertice em "firstPoint" e respectivo iterator em "firstPointCirculator"
			itLeftBottomPoint = itLastInsertedVector;	//itLeftBottomPointAfter=itCurrentlyInsertedVector;
		}
	} while (b_it != b_it_begin);

	//se leftBottomPoint nao foi inicializado e porque se comecou por esse ponto e o vector que lhe corresponde e o ultimo adicionado (itCurrentlyInsertedVector)
	if (itLeftBottomPoint == slopes.end())		itLeftBottomPoint = itCurrentlyInsertedVector;
	//se leftBottomPoint tiver menor slope que lastPointIterator, defini-lo como o vector inicial
	if (itLeftBottomPoint->direction() <= lastPointIterator->direction())	lastPointIterator = itLeftBottomPoint;

	lastPoint = lastPoint + Vector_2(leftBottomPoint, CGAL::ORIGIN);

	//Somar os vectores por ordem partindo de lastPoint
	Translate_point_2 f_add;
	Polygon_2 sum;

	Polygon_2::Vertex_iterator lastInserted;
	Vector_2 lastV = CGAL::NULL_VECTOR;
	for (list<Vector_2>::iterator it = lastPointIterator; it != slopes.end(); it++)
	{
		Vector_2 v = *it;
		Point_2 nextPoint = f_add(lastPoint, v);

		if (v.direction() == lastV.direction())
		{
			sum.erase(lastInserted);
		}
		lastInserted = sum.insert(sum.vertices_end(), nextPoint);
		lastPoint = *lastInserted;
		lastV = v;
	}
	for (list<Vector_2>::iterator it = slopes.begin(); it != lastPointIterator; it++)
	{
		Vector_2 v = *it;
		Point_2 nextPoint = f_add(lastPoint, v);

		if (v.direction() == lastV.direction())
		{
			sum.erase(lastInserted);
		}
		lastInserted = sum.insert(sum.vertices_end(), nextPoint);
		lastPoint = *lastInserted;
		lastV = v;
	}

	//Eliminar o vertice inicial se este estiver entre dois vectores com a mesma direcçao
	Polygon_2::Vertex_circulator sumIt = sum.vertices_circulator();

	while (*(--sumIt) != lastPoint);

	Point_2 after = *(++sumIt);
	Point_2 before = *(--(--sumIt));

	if (Vector_2(before, after).direction() == Vector_2(lastPoint, after).direction())
		sum.erase(lastInserted);

	return sum;
}

Polygon_2 NonFitPolygonsCalculation::removeRedundantVertices(Polygon_2 a)
{

	Polygon_2 a2;
	a2.push_back(a[0]);
	int a2Index = 0;
	//print_polygon(a);
	for (int i = 1; i < a.size(); i++)
	{
		//cout<<"i: "<<i<<"/"<<a.size()<<endl;
		//cout<<(i+1)%a.size()<<endl<<(i+2)%a.size()<<endl;

		int i2 = i % a.size();
		int i3 = (i + 1) % a.size();

		Point_2 p1 = a2[a2Index];
		Point_2 p2 = a[i2];
		Point_2 p3 = a[i3]; //p3 = a2[0] se i == a.size()-2 (penultimo elem)

		Vector_2 edge1(p1, p2);
		Vector_2 edge2(p2, p3);

		if (edge1.direction() != edge2.direction())
		{
			if (i2 < a.size())
			{
				a2.push_back(p2);
				a2Index++;
			}
		}
		else
		{
			if (i2 >= a.size())
			{
				a2.erase(a2.vertices_begin());
			}
		}
	}
	return a2;

}

vector<Polygon_2> NonFitPolygonsCalculation::minkowskiSumsWithDecompositionNoConversion(vector<Polygon_2>   subPolyPa, vector<Polygon_2>   subPolyPb)
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
			sums.push_back(minkowskiSumConvexPoly2_ConvexHull2(*it, *jt));
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

Polygon_2  NonFitPolygonsCalculation::minkowskiSumConvexPoly2_ConvexHull2(Polygon_2 a, Polygon_2 b)
{
	//return minkowskiSumConvexPoly(a, b, CGAL::ch_graham_andrew );

	Translate_point_2       f_add;
	Construct_vector_2      f_vector;
	Polygon_2 sum;
	//print_polygon(a);
	//print_polygon(b);
	for (Polygon_2::Vertex_iterator vit = a.vertices_begin(); vit != a.vertices_end(); ++vit)
		for (Polygon_2::Vertex_iterator vjt = b.vertices_begin(); vjt != b.vertices_end(); ++vjt)
			sum.push_back(f_add(*vit, f_vector(CGAL::ORIGIN, *vjt)));

	Polygon_2 hull;
	//CGAL::ch_graham_andrew (sum.vertices_begin(), sum.vertices_end(), back_inserter(hull));
	CGAL::convex_hull_2(sum.vertices_begin(), sum.vertices_end(), back_inserter(hull));
	//print_polygon(hull);
	return hull;
}