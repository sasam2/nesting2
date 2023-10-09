#pragma once
#include <vector>
#include <GL/glew.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_with_holes_2.h>
#include <CGAL/convex_hull_2.h>
#include <CGAL/Small_side_angle_bisector_decomposition_2.h>
#include "BoundingBox.h"

#define M_PI       3.14159265358979323846

#define WIDER 0
#define HIGHER 1
#define LARGER 2
#define MORE_IRREGULAR 3
#define MORE_RECTANGULAR 4
#define RANDOM 5
#define DYNAMIC 6

using namespace std;

typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef Kernel::Point_2                               Point_2;
typedef Kernel::Point_3                               Point_3;
typedef CGAL::Vector_2<Kernel>						  Vector_2;
typedef CGAL::Vector_3<Kernel>						  Vector_3;
typedef CGAL::Polygon_with_holes_2<Kernel>            Polygon_with_holes_2;
typedef CGAL::Polygon_2<Kernel>                       Polygon_2;
typedef CGAL::Aff_transformation_2<Kernel>			  Transformation;
typedef Kernel::Construct_translated_point_2		  Translate_point_2;
typedef Kernel::Construct_vector_2					  Construct_vector_2;

struct Selection {
	int nfpsCalculation = 0;
	int heuristic = 0;
	char problemName[1024] = "shapes_config_stock_sheet.txt";
};


GLdouble* createPosition(double x, double y, double z);
GLfloat* createColor(float r, float g, float b, float a);
BoundingBox getBoundingBox(vector<Polygon_2> poly);
BoundingBox getBoundingBox(Polygon_2 p);

