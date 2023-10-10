#include "callbacks.h"

Selection globalUserSelection;
Layout globalLayout;
SolvingStatus globalSolvingStatus;
DrawingStatus globalDrawingStatus;

int handle;

ofstream testfile;

void solveCB(int dummy)
{
	//delete data from previous problem
	globalDrawingStatus.cleanup();
	globalSolvingStatus.cleanup();
	//FIXME add layout cleanup here
	LayoutBuilder::iteration = 0;

	//getData
	cout << "Selected nfp algorithm: " << globalUserSelection.nfpsCalculation << endl;
	cout << "Selected heuristic: " << globalUserSelection.heuristic << endl;
	cout << "Selected problem name: " << globalUserSelection.problemName << endl;

	//loadProblem
	globalLayout.load("problems/", globalUserSelection.problemName);

	//pre process geometry
	globalSolvingStatus.load(globalUserSelection, globalLayout);

	globalDrawingStatus.load(globalUserSelection, globalLayout, globalSolvingStatus);

	int resCorrectionWidth = globalDrawingStatus.viewport.getWidth() / globalLayout.getResolution();

	glutReshapeWindow(globalDrawingStatus.w + globalDrawingStatus.tx, globalDrawingStatus.h + globalDrawingStatus.ty);
	cout << "Viewport: " << globalDrawingStatus.viewport.getWidth() << "    " << globalDrawingStatus.viewport.getHeight() << endl;
	cout << "Window Dimensions: " << globalDrawingStatus.w << " " << globalDrawingStatus.h << endl;
	cout << "Resolution: " << globalLayout.getResolution() << endl;

	//inicialize piece placement
	if (globalUserSelection.heuristic == DYNAMIC)
		LayoutBuilder::putPieceDynamic(globalLayout, globalSolvingStatus, globalDrawingStatus, true);


}



void displayCB()
{
	// clear buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// save the initial ModelView matrix before modifying ModelView matrix
	glPushMatrix();
	glLoadIdentity();
	glDisable(GL_BLEND);

	if (globalDrawingStatus.currentDrawingNFPs.size() > 0)
	{
		if (globalUserSelection.heuristic==DYNAMIC) {//Programaçao Dinamica
#ifdef DEBUG
			LayoutBuilder::displayDynamic(globalUserSelection, globalLayout, globalSolvingStatus, globalDrawingStatus ,&testfile);
#else
			LayoutBuilder::buildDynamicLayout(globalUserSelection, globalLayout, globalSolvingStatus, globalDrawingStatus);
#endif		
		}
		else {		//Criterios Estáticos
#ifdef DEBUG
			if (!testfile.is_open()) {
				char filename[100];
				sprintf_s(filename, "staticTests %s - %d.csv", globalUserSelection.problemName, globalUserSelection.heuristic);
				testfile.open(filename);
				testfile << "Iteration; Placement time ; Updating time" << endl;
			}
			LayoutBuilder::displayStatic(globalUserSelection, globalLayout, globalSolvingStatus, globalDrawingStatus, &testfile);
#else
			LayoutBuilder::buildStaticLayout(globalUserSelection, globalLayout, globalSolvingStatus, globalDrawingStatus);

#endif
		}
	}
#ifdef DEBUG
	else
	{
		if (testfile.is_open())
			testfile.close();
	}
#endif	

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	BoundingBox layoutBB = globalDrawingStatus.currentDrawingPolys.getViewPort();
	gluOrtho2D(layoutBB.getX(), layoutBB.getWidth(), layoutBB.getY(), layoutBB.getHeight());	// set to orthogonal projection
	glMatrixMode(GL_MODELVIEW);																	// switch to modelview matrix

	glColor3f(1.0, 1.0, 0.0);
	for (int i = 0; i < globalDrawingStatus.currentDrawingPolys.getPositions().size(); i++)
	{
		glPushMatrix();
		//glColor4f(currentDrawingPolys[i].getColors()[i][0], currentDrawingPolys[i].getColors()[i][1], currentDrawingPolys[i].getColors()[i][2], currentDrawingPolys[i].getColors()[i][3]);
		glTranslatef(globalDrawingStatus.currentDrawingPolys.getPositions()[i][0], globalDrawingStatus.currentDrawingPolys.getPositions()[i][1], globalDrawingStatus.currentDrawingPolys.getPositions()[i][2]);
		glCallList(globalDrawingStatus.currentDrawingPolys.getLists()[globalDrawingStatus.currentDrawingPolys.getOrder()[i].first][globalDrawingStatus.currentDrawingPolys.getOrder()[i].second]);
		glPopMatrix();
	}
	glColor3f(1.0, 0.55, 0.0);
	glCallList(globalDrawingStatus.stockList);

	glPopMatrix();

	glutSwapBuffers();
}

void timerCB(int millisec)
{
	glutTimerFunc(millisec, timerCB, millisec);
	glutPostRedisplay();
}

void idleCB()
{
	if (glutGetWindow() != handle)
		glutSetWindow(handle);
	glutPostRedisplay();
}

void reshapeCB(int w, int h)
{
	// set viewport to be the entire window
	//glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	GLUI_Master.auto_set_viewport();
	glutPostRedisplay();
}

int initGLUT(int argc, char** argv)
{
	// GLUT stuff for windowing
	// initialization openGL window.
	// it is called before any other GLUT routine
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);   // display mode

	glutInitWindowSize(500, 200);               // window size

	glutInitWindowPosition(100, 100);           // window location

	// finally, create a window with openGL context
	// Window will not displayed until glutMainLoop() is called
	// it returns a unique ID
	handle = glutCreateWindow("Layout");     // param is the title of window

	// register GLUT callback functions
	glutDisplayFunc(displayCB);
	glutTimerFunc(33, timerCB, 33);             // redraw only every given millisec

	//glutIdleFunc(idleCB);                       // redraw only every given millisec
	//glutReshapeFunc(reshapeCB);

	GLUI_Master.set_glutIdleFunc(idleCB);
	GLUI_Master.set_glutReshapeFunc(reshapeCB);

	GLUI* glui = GLUI_Master.create_glui_subwindow(handle, GLUI_SUBWINDOW_BOTTOM);
	/*	GLUI_SUBWINDOW_RIGHT
		GLUI_SUBWINDOW_BOTTOM */
	glui->set_main_gfx_window(handle);

	GLUI_Panel* panel_nfps = glui->add_panel("NFPs calculation:");
	GLUI_RadioGroup* radiogroup_nfps = glui->add_radiogroup_to_panel(panel_nfps, &globalUserSelection.nfpsCalculation);
	GLUI_RadioButton* radiobutton_slp = glui->add_radiobutton_to_group(radiogroup_nfps, "Slope diagram");
	GLUI_RadioButton* radiobutton_mk = glui->add_radiobutton_to_group(radiogroup_nfps, "Minkowski sums");


	//WIDER 0
	//HIGHER 1
	//LARGER 2
	//MORE_IRREGULAR 3
	//MORE_RECTANGULAR 4
	//RANDOM 5
	//DYNAMIC 6

	glui->add_column(true);
	GLUI_Panel* panel_heuristic = glui->add_panel("Heuristic:");
	GLUI_RadioGroup* radiogroup_heuristic = glui->add_radiogroup_to_panel(panel_heuristic, &globalUserSelection.heuristic);
	GLUI_RadioButton* radiobutton_wider = glui->add_radiobutton_to_group(radiogroup_heuristic, "Length");
	GLUI_RadioButton* radiobutton_higher = glui->add_radiobutton_to_group(radiogroup_heuristic, "Height");
	GLUI_RadioButton* radiobutton_larger = glui->add_radiobutton_to_group(radiogroup_heuristic, "Size");
	GLUI_RadioButton* radiobutton_irregular = glui->add_radiobutton_to_group(radiogroup_heuristic, "Irregularity");
	GLUI_RadioButton* radiobutton_rectangular = glui->add_radiobutton_to_group(radiogroup_heuristic, "Rectangularity");
	GLUI_RadioButton* radiobutton_random = glui->add_radiobutton_to_group(radiogroup_heuristic, "Random");
	GLUI_RadioButton* radiobutton_dynamic = glui->add_radiobutton_to_group(radiogroup_heuristic, "Dynamic");

	glui->add_column(true);
	GLUI_Panel* panel_file = glui->add_panel("File:");
	GLUI_EditText* edittext_file = glui->add_edittext_to_panel(panel_file, "Name", GLUI_EDITTEXT_TEXT, &globalUserSelection.problemName);

	GLUI_Button* botaoUndo = glui->add_button("Solve", 1, solveCB);
	
	GLUI_Master.get_viewport_area(&globalDrawingStatus.tx, &globalDrawingStatus.ty, &globalDrawingStatus.tw, &globalDrawingStatus.th);

	return handle;
}


void initGL()
{
	glShadeModel(GL_SMOOTH);                    // shading mathod: GL_SMOOTH or GL_FLAT
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);      // 4-byte pixel alignment

	// enable /disable features
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	//glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	//glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);

	// track material ambient and diffuse from surface color, call it before glEnable(GL_COLOR_MATERIAL)
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	glClearColor(0, 0, 0, 0);                   // background color
	glClearStencil(0);                          // clear stencil buffer
	glClearDepth(1.0f);                         // 0 is near, 1 is far
	glDepthFunc(GL_LEQUAL);

	//initLights();
	//setCamera(0, 0, 5, 0, 0, 0);
}
