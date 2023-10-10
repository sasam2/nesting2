#include "DrawingStatus.h"

DrawingStatus::DrawingStatus() {}

DrawingStatus::~DrawingStatus() {}

void DrawingStatus::load(Selection& userSelection, Layout& layout, SolvingStatus& solvingStatus) {
	
	int numberOfPolygons = solvingStatus.numberOfPolygons;
	int nrOfRotations = solvingStatus.nrOfRotations;
	int numberOfNFPs = solvingStatus.numberOfNFPs;
	Polygon_with_holes_2** polygons = solvingStatus.polygons;
	vector<Polygon_2>**** nfpsPiecesAndStockSheet = solvingStatus.nfpsPiecesAndStockSheet;
	vector<Polygon_2> sSPolyVec = solvingStatus.sSPolyVec;

	blue = createColor(0.0, 0.0, 1.0, 0.25);

	//CALCULATE DISPLAY LISTS
	//stock sheet
	stockList = generatePolygonDisplayList(sSPolyVec);

	viewport = BoundingBox(layout.stockSheetsBoundingBox().getX(),
		layout.stockSheetsBoundingBox().getY(),
		layout.stockSheetsBoundingBox().getWidth() + (layout.getStockSheet().size() - 1) * layout.getMaxWidth(),
		layout.stockSheetsBoundingBox().getHeight());
	//polys
	currentDrawingPolys = DrawingWithRotations(numberOfPolygons, nrOfRotations);
	currentDrawingPolys.addDisplayListsFromPolygonVector(polygons, numberOfPolygons, nrOfRotations, generatePolygonWHolesDisplayList);
	currentDrawingPolys.setViewPort(viewport);
	//currentDrawingPolys.setViewPort(layout.getStockSheet()[0].getBoundingBox());
	currentDrawingPolys.resetPlacements();

	//NFPs e IFPs
	int hV = viewport.getHeight();
	h = hV / layout.getResolution();
	int wV = viewport.getWidth();
	w = wV / layout.getResolution();
	origin = createPosition(0.0, 0.0, 0.0);

	for (int p = 0; p < numberOfPolygons; p++)
	{
		for (int r = 0; r < nrOfRotations; r++)
		{
			Point_2 pointPR(p, r);
			currentDrawingNFPs[pointPR] = DrawingWithRotations(numberOfNFPs, nrOfRotations);
			currentDrawingNFPs[pointPR].addNFPsDisplayListsFromPolygonVector(nfpsPiecesAndStockSheet[p][r], numberOfNFPs, nrOfRotations, generatePolygonDisplayList);
			currentDrawingNFPs[pointPR].setViewPort(viewport);
			//currentDrawingNFPs[pointPR].setViewPort(layout.getStockSheet()[0].getBoundingBox());
			currentDrawingNFPs[pointPR].resetPlacements();
			currentDrawingNFPs[pointPR].addListPlacement(numberOfNFPs - 1, 0, origin, blue);

			glGenBuffers(1, &currentDrawingNFPs[pointPR].bufferObject);
			glBindBuffer(GL_PIXEL_PACK_BUFFER, currentDrawingNFPs[pointPR].bufferObject);
			glBufferData(GL_PIXEL_PACK_BUFFER,	//(layout.getStockSheet()[0].getBoundingBox().getWidth()/layout.getResolution())*
				//(layout.getStockSheet()[0].getBoundingBox().getHeight()/layout.getResolution()),
				(h * (w + 3)),
				0,
				GL_STREAM_READ);
			glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
		
		}
	}

	switch (userSelection.heuristic) {
	
		case WIDER:
		case HIGHER:
		case LARGER:
		case MORE_IRREGULAR:
		case MORE_RECTANGULAR:
		case RANDOM:
			preProcessingStatic(userSelection.heuristic, layout, numberOfPolygons, nrOfRotations, solvingStatus.infos);
			break;
		case DYNAMIC:
			preProcessingDynamic(layout, numberOfPolygons, nrOfRotations, numberOfNFPs, solvingStatus.infos, solvingStatus.polygonsDecompositions, nfpsPiecesAndStockSheet);
			break;
		default:
			throw exception();
		
	}
		
}

void DrawingStatus::cleanup() {
	currentDrawingPolys.clear();
	//clear currentDrawingNFPs
	for (map<Point_2, DrawingWithRotations>::iterator it = currentDrawingNFPs.begin(); it != currentDrawingNFPs.end(); it++) {
		it->second.clear();
	}
	currentDrawingNFPs.clear();
	delete[] blue;
	delete[] origin;
	delete[] colorStatic;

	piecesOrdered.clear();
	//clear drawingPolysTest
	for (map<Point_2, DrawingWithRotations>::iterator it = drawingPolysTest.begin(); it != drawingPolysTest.end(); it++) {
		it->second.clear();
	}
	//clear layoutNFPsTest
	for (map<Point_2, GLuint>::iterator it = layoutNFPsTest.begin(); it != layoutNFPsTest.end(); it++) {
		glDeleteBuffers(1, &it->second);
	}
	viewport = BoundingBox();
	//h = w = tx = ty = tw = th = 0;
	//clear layoutNFPsTest
	for (map<Point_2, GLuint>::iterator it = layoutNFPsTest.begin(); it != layoutNFPsTest.end(); it++) {
		glDeleteBuffers(1, &it->second);
	}
	glDeleteLists(stockList, 1);
	
}


void DrawingStatus::preProcessingStatic(int staticHeuristic, Layout layout, int numberOfPolygons, int nrOfRotations, map<Point_2, PolyInfo> infos)
{
	int greaterDimension;
	if (layout.getStockSheet()[0].getBoundingBox().getWidth() > layout.getStockSheet()[0].getBoundingBox().getHeight()) {
		greaterDimension = 1;
	}
	else {
		greaterDimension = 2;
	}

	map<int, int> piecesToPlace;
	for (int p = 0; p < numberOfPolygons; p++)
		piecesToPlace[p] = layout.getQuantity()[p];

	switch (staticHeuristic)
	{
	case WIDER:
		//cout<<infos.size();
		for (int p = 0; p < numberOfPolygons; p++)
		{
			Point_2 best(p, 0);
			for (int r = 1; r < nrOfRotations; r++)
			{
				Point_2 currentP(p, r);
				if (infos[best].width > infos[currentP].width)
					best = currentP;
			}
			list<Point_2>::iterator j = piecesOrdered.begin();
			for (; j != piecesOrdered.end() && infos[*j].width > infos[best].width; j++);
			for (int c = 0; c < layout.getQuantity()[p]; c++)
				j = piecesOrdered.insert(j, best);

		}
		break;

	case HIGHER:
		//cout<<infos.size();
		for (int p = 0; p < numberOfPolygons; p++)
		{
			Point_2 best(p, 0);
			for (int r = 1; r < nrOfRotations; r++)
			{
				Point_2 currentP(p, r);
				if (infos[best].height > infos[currentP].height)
					best = currentP;
			}
			list<Point_2>::iterator j = piecesOrdered.begin();
			for (; j != piecesOrdered.end() && infos[*j].height > infos[best].height; j++);
			for (int c = 0; c < layout.getQuantity()[p]; c++)
				j = piecesOrdered.insert(j, best);
		}
		break;

	case LARGER:
		for (int p = 0; p < numberOfPolygons; p++)
		{
			Point_2 best(p, 0);	//ver qual das rotaçoes e a melhor (a que tiver a dimensao mais estreita na direccao da dimensao mais larga da stock sheet)

			for (int r = 1; r < nrOfRotations; r++)
			{
				Point_2 currentP(p, r);
				//cout<<best<<" "<<infos[best].width<<" "<<currentP<<" "<<infos[currentP].width<<endl;

				if (greaterDimension == 1)
				{
					if (infos[best].width > infos[currentP].width)
						best = currentP;
				}
				else
				{
					if (infos[best].height > infos[currentP].height)
						best = currentP;
				}
			}
			list<Point_2>::iterator j = piecesOrdered.begin();	//introduzir n vezes essa peça com essa rotacao na lista, ordenada por areas
			for (; j != piecesOrdered.end() && infos[*j].area > infos[best].area; j++);
			for (int c = 0; c < layout.getQuantity()[p]; c++)
				j = piecesOrdered.insert(j, best);
		}
		break;

	case MORE_IRREGULAR:
		for (int p = 0; p < numberOfPolygons; p++)
		{
			Point_2 best(p, 0);	//ver qual das rotaçoes e a melhor (a que tiver a dimensao mais larga na direccao da dimensao mais larga da stock sheet)
			for (int r = 1; r < nrOfRotations; r++)
			{
				Point_2 currentP(p, r);
				if (greaterDimension == 1)
				{
					if (infos[best].width > infos[currentP].width)
						best = currentP;
				}
				else
				{
					if (infos[best].height > infos[currentP].height)
						best = currentP;
				}
			}
			list<Point_2>::iterator j = piecesOrdered.begin();	//introduzir n vezes essa peça com essa rotacao na lista, ordenada por areas
			for (; j != piecesOrdered.end() && infos[*j].concavity > infos[best].concavity; j++);
			for (int c = 0; c < layout.getQuantity()[p]; c++)
				j = piecesOrdered.insert(j, best);
		}
		break;

	case MORE_RECTANGULAR:
		for (int p = 0; p < numberOfPolygons; p++)
		{
			Point_2 best(p, 0);	//ver qual das rotaçoes e a melhor (a que tiver a dimensao mais larga na direccao da dimensao mais larga da stock sheet)
			for (int r = 1; r < nrOfRotations; r++)
			{
				Point_2 currentP(p, r);
				if (greaterDimension == 1)
				{
					if (infos[best].width > infos[currentP].width)
						best = currentP;
				}
				else
				{
					if (infos[best].height > infos[currentP].height)
						best = currentP;
				}
			}
			list<Point_2>::iterator j = piecesOrdered.begin();	//introduzir n vezes essa peça com essa rotacao na lista, ordenada por areas
			for (; j != piecesOrdered.end() && infos[*j].rectangularity > infos[best].rectangularity; j++);
			for (int c = 0; c < layout.getQuantity()[p]; c++)
				j = piecesOrdered.insert(j, best);
		}
		break;

	case RANDOM:
		while (!piecesToPlace.empty())
		{
			//escolher peça
			int pieceIndex = rand() % piecesToPlace.size();
			int piece;
			map<int, int>::iterator it = piecesToPlace.begin();
			for (int i = 0;
				it != piecesToPlace.end() && i < pieceIndex;
				it++, i++);
			piece = it->first;

			//escolher rotacao
			int rotation = rand() % nrOfRotations;
			piecesOrdered.push_back(Point_2(piece, rotation));

			//decrementar quantidade
			if (--it->second == 0)
				piecesToPlace.erase(it);	//se ja nao houver peças desse tipo apagar peça do mapa
		}
		break;

	default:
		break;
	}
}

void DrawingStatus::preProcessingDynamic(Layout layout, int numberOfPolygons, int nrOfRotations, int numberOfNFPs, map<Point_2, PolyInfo> infos, vector<Polygon_2>** polygonsDecompositions, vector<Polygon_2>**** nfpsPiecesAndStockSheet)
{
	GLdouble* origin = createPosition(0.0, 0.0, 0.0);
	colorStatic = createColor(0.0, 0.0, 1.0, 1.0);

	BoundingBox viewport(layout.stockSheetsBoundingBox().getX(),
		layout.stockSheetsBoundingBox().getY(),
		layout.stockSheetsBoundingBox().getWidth() + (layout.getStockSheet().size() - 1) * layout.getMaxWidth(),
		layout.stockSheetsBoundingBox().getHeight());

	int hV = viewport.getHeight();
	h = hV / layout.getResolution();
	int wV = viewport.getWidth();
	w = wV / layout.getResolution();

	for (int p = 0; p < numberOfPolygons; p++)
	{
		for (int r = 0; r < nrOfRotations; r++)
		{
			Point_2 pointPR(p, r);

			drawingPolysTest[pointPR] = DrawingWithRotations(numberOfPolygons, nrOfRotations);
			drawingPolysTest[pointPR].addDisplayListsFromPolygonVector(polygonsDecompositions, numberOfPolygons, nrOfRotations, generatePolygonDisplayList);
			drawingPolysTest[pointPR].setViewPort(viewport);
			//drawingPolysTest[pointPR].setViewPort(layout.getStockSheet()[0].getBoundingBox());
			drawingPolysTest[pointPR].resetPlacements();


			glGenBuffers(1, &layoutNFPsTest[pointPR]);
			glBindBuffer(GL_PIXEL_PACK_BUFFER, layoutNFPsTest[pointPR]);
			glBufferData(GL_PIXEL_PACK_BUFFER,	//(layout.getStockSheet()[0].getBoundingBox().getWidth()/layout.getResolution())*
				//(layout.getStockSheet()[0].getBoundingBox().getHeight()/layout.getResolution()), 
				(h * (w + 3)),
				0,
				GL_STREAM_READ);
			glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

			for (int p2 = 0; p2 < numberOfPolygons; p2++)
			{
				for (int r2 = 0; r2 < nrOfRotations; r2++)
				{
					Point_2 pointPR2(p2, r2);

					drawingNFPsTest[pointPR][pointPR2] = DrawingWithRotations(numberOfNFPs, nrOfRotations);
					drawingNFPsTest[pointPR][pointPR2].addNFPsDisplayListsFromPolygonVector(nfpsPiecesAndStockSheet[p2][r2], numberOfNFPs, nrOfRotations, generatePolygonDisplayList);
					drawingNFPsTest[pointPR][pointPR2].setViewPort(viewport);
					//drawingNFPsTest[pointPR][pointPR2].setViewPort(layout.getStockSheet()[0].getBoundingBox());
					drawingNFPsTest[pointPR][pointPR2].resetPlacements();
					drawingNFPsTest[pointPR][pointPR2].addListPlacement(numberOfNFPs - 1, 0, origin, colorStatic);

					//glBufferDataARB with NULL pointer reserves only memory space.
					glGenBuffers(1, &drawingNFPsTest[pointPR][pointPR2].bufferObject);
					glBindBuffer(GL_PIXEL_PACK_BUFFER, drawingNFPsTest[pointPR][pointPR2].bufferObject);
					glBufferData(GL_PIXEL_PACK_BUFFER,	//(drawingNFPsTest[pointPR][pointPR2].getViewPort().getWidth()/layout.getResolution())*
						//(drawingNFPsTest[pointPR][pointPR2].getViewPort().getHeight()/layout.getResolution()), 
						(h * (w + 3)),
						0,
						GL_STREAM_READ);
					glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
				}
			}
		}
	}
	//delete[] origin;
	//delete[] colorStatic;
}