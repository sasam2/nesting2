#include "display_lists_generation.h"


int generatePolygonDisplayList(vector<Polygon_2> p) {

	GLuint id = glGenLists(1);  // create a display list
	if (!id) return id;          // failed to create a list, return 0

	glNewList(id, GL_COMPILE);
	for (int i = 0; i < p.size(); i++)
	{
		glBegin(GL_POLYGON);
		for (Polygon_2::Vertex_iterator hit = p[i].vertices_begin(); hit != p[i].vertices_end(); ++hit)
		{
			glVertex2d(CGAL::to_double(hit->x()),
				CGAL::to_double(hit->y()));
		}
		glEnd();
	}
	glEndList();

	return id;					// return handle ID of a display list
}

int  generatePolygonWHolesDisplayList(CGAL::Polygon_with_holes_2<Kernel> p) {

	GLuint id = glGenLists(1);  // create a display list
	if (!id) return id;          // failed to create a list, return 0

	GLUtesselator* tess = gluNewTess(); // create a tessellator
	if (!tess) return 0;  // failed to create tessellation object, return 0

	// register callback functions
	// This polygon is self-intersecting, so GLU_TESS_COMBINE callback function
	// must be registered. The combine callback will process the intersecting vertices.
	gluTessCallback(tess, GLU_TESS_BEGIN, (void(__stdcall*)(void))tessBeginCB);
	gluTessCallback(tess, GLU_TESS_END, (void(__stdcall*)(void))tessEndCB);
	gluTessCallback(tess, GLU_TESS_ERROR, (void(__stdcall*)(void))tessErrorCB);
	gluTessCallback(tess, GLU_TESS_VERTEX, (void(__stdcall*)(void))tessVertexCB);

	// tessellate and compile a concave quad into display list
	// Pay attention to winding rules if multiple contours are overlapped.
	// The winding rules determine which parts of polygon will be filled(interior)
	// or not filled(exterior). For each enclosed region partitioned by multiple
	// contours, tessellator assigns a winding number to the region by using
	// given winding rule. The default winding rule is GLU_TESS_WINDING_ODD,
	// but, here we are using non-zero winding rule to fill the middle area.
	// BTW, the middle region will not be filled with the odd winding rule.
	// gluTessProperty(tess, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_NONZERO);
	glNewList(id, GL_COMPILE);
	// glColor4f(color[0],color[1],color[2],color[3]);
	gluTessBeginPolygon(tess, 0);

	gluTessBeginContour(tess);
	for (CGAL::Polygon_2<Kernel>::Vertex_iterator vit = p.outer_boundary().vertices_begin(); vit != p.outer_boundary().vertices_end(); ++vit)
	{
		double* v = createPosition(CGAL::to_double(vit->x()),
			CGAL::to_double(vit->y()),
			0.0);
		gluTessVertex(tess, v, v);
	}

	gluTessEndContour(tess);

	for (CGAL::Polygon_with_holes_2<Kernel>::Hole_iterator hit = p.holes_begin(); hit != p.holes_end(); ++hit)
	{

		gluTessBeginContour(tess);
		for (CGAL::Polygon_2<Kernel>::Vertex_iterator vit = hit->vertices_begin(); vit != hit->vertices_end(); ++vit)
		{
			double* v = createPosition(CGAL::to_double(vit->x()),
				CGAL::to_double(vit->y()),
				0.0);
			gluTessVertex(tess, v, v);
		}
		gluTessEndContour(tess);
	}

	gluTessEndPolygon(tess);
	glEndList();
	gluDeleteTess(tess);        // safe to delete after tessellation
	return id;					// return handle ID of a display list
}


///////////////////////////////////////////////////////////////////////////////
// GLU_TESS CALLBACKS
///////////////////////////////////////////////////////////////////////////////
void CALLBACK tessBeginCB(GLenum which)
{
	glBegin(which);

	// DEBUG //
	//ss << "glBegin(" << getPrimitiveType(which) << ");\n";
}

void CALLBACK tessEndCB()
{
	glEnd();

	// DEBUG //
	//ss << "glEnd();\n";
}

void CALLBACK tessVertexCB(const GLvoid* data)
{
	// cast back to double type
	const GLdouble* ptr = (const GLdouble*)data;

	glVertex3dv(ptr);

	// DEBUG //
	//ss << "  glVertex3d(" << *ptr << ", " << *(ptr+1) << ", " << *(ptr+2) << ");\n";
}

void CALLBACK tessErrorCB(GLenum errorCode)
{
	const GLubyte* errorStr;

	errorStr = gluErrorString(errorCode);
	cerr << "[ERROR]: " << errorStr << endl;
}