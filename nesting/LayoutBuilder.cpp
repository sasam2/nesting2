#include "LayoutBuilder.h"


int LayoutBuilder::iteration=0;
int* LayoutBuilder::placedPieces = NULL;
GLfloat* LayoutBuilder::blue = NULL;
GLfloat* LayoutBuilder::green = NULL;
vector<int> LayoutBuilder::piecesToPlace;

void LayoutBuilder::buildStaticLayout(Selection& userSelection, Layout& layout, SolvingStatus& solvingStatus, DrawingStatus& drawingStatus)
{
	ofstream myfile;
	char filename[100];
	sprintf_s(filename, "staticTests %s - %d.csv", userSelection.problemName, userSelection.heuristic);
	myfile.open(filename);
	myfile << "Iteration; Placement time ; Updating time" << endl;

	clock_t begin = clock();
	bool lastIterationSuccessful = false;
	while (drawingStatus.currentDrawingNFPs.size() > 0)
		lastIterationSuccessful = displayStatic(userSelection, layout, solvingStatus, drawingStatus, &myfile);
	if (!lastIterationSuccessful)
		cout << "ERROR: not all pieces could be placed! Terminating." << endl;
	else
		cout << "SUCCESS: all pieces placed!" << endl;
	clock_t end = clock();

	myfile << "TOTAL;" << diffclock(end, begin) << endl;
	double maxLength = calculateMaxLength(drawingStatus.currentDrawingPolys, solvingStatus.polygons);
	myfile << "LENGTH;" << calculateMaxLength(drawingStatus.currentDrawingPolys, solvingStatus.polygons) << endl;
	myfile << "USED AREA;" << calculateUsedArea(layout, solvingStatus.numberOfPolygons, solvingStatus.polygons, maxLength) << endl;
	myfile << "WIDTH;" << layout.stockSheetsBoundingBox().getHeight();
	myfile.close();
}



bool LayoutBuilder::displayStatic(Selection& userSelection, Layout& layout, SolvingStatus& solvingStatus, DrawingStatus& drawingStatus, ofstream* myfile)
{
	int previousBind = 0;
	static vector<int> piecesToPlace;

	if (iteration == 0)
	{
		glDisable(GL_BLEND);
		GLUI_Master.get_viewport_area(&drawingStatus.tx, &drawingStatus.ty, &drawingStatus.tw, &drawingStatus.th);
		//blue=createColor(0.0,0.0,1.0,1.0);
		piecesToPlace = layout.getQuantity();
		/*
		for(map<Point_2, DrawingWithRotations>::iterator itNFPsOfPiecesToPlaceIndex=currentDrawingNFPs.begin();
		itNFPsOfPiecesToPlaceIndex != currentDrawingNFPs.end();
		itNFPsOfPiecesToPlaceIndex++)
		{
			DrawingWithRotations *d=&itNFPsOfPiecesToPlaceIndex->second;
			glClear(GL_COLOR_BUFFER_BIT);
			glColor3f(0.0,0.0,1.0);
			for(int i=0; i < d->getPositions().size(); i++)
			{
				glPushMatrix();
				//glColor4f	(d->getColors()[i][0], d->getColors()[i][1], d->getColors()[i][2], d->getColors()[i][3]);
				glTranslatef(d->getPositions()[i][0], d->getPositions()[i][1], d->getPositions()[i][2]);
				glCallList	(d->getLists()[d->getOrder()[i].first][d->getOrder()[i].second]);
				glPopMatrix	();
			}
			//d->bufferObject=drawingNFPsTest[Point_2(0,0)][itNFPsOfPiecesToPlaceIndex->first].bufferObject;
			//IplImage *imgTest = cvCreateImage(cvSize(wWidth, wHeight), IPL_DEPTH_8U,1);

			int previousBind=0;
			glReadBuffer(GL_BACK_LEFT);
			glGetIntegerv(GL_PIXEL_PACK_BUFFER_BINDING, &previousBind);
			glBindBuffer(GL_PIXEL_PACK_BUFFER, d->bufferObject);
			cout<<glGetError()<<endl;
			glReadPixels(tx, ty, tw, th, GL_BLUE, GL_UNSIGNED_BYTE, 0);
			cout<<glGetError()<<endl;
			glBindBuffer(GL_PIXEL_PACK_BUFFER, previousBind);

			int p=(int)itNFPsOfPiecesToPlaceIndex->first.x();
			int r=(int)itNFPsOfPiecesToPlaceIndex->first.y();

	//d->bufferObject=drawingNFPsTest[Point_2(0,0)][itNFPsOfPiecesToPlaceIndex->first].bufferObject;
	//#ifdef DEBUG
			IplImage *imgTest = cvCreateImage(cvSize(tw, th), IPL_DEPTH_8U,1);
			char* initPtr = imgTest->imageData;
			glGetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING, &previousBind);
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, d->bufferObject);
			imgTest->imageData = (char*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_READ_ONLY);
			char imageName[100];
			sprintf(imageName, "INIT %d %d.jpg", p, r);
			cvSaveImage(imageName, imgTest);
			glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, previousBind);
			imgTest->imageData=initPtr;
			cvReleaseImage(&imgTest);
	//#endif
		}
		*/
	}

	//definir dimensoes do viewport
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	BoundingBox layoutBB = drawingStatus.currentDrawingNFPs.begin()->second.getViewPort();
	gluOrtho2D(layoutBB.getX(), layoutBB.getWidth(), layoutBB.getY(), layoutBB.getHeight());
	glMatrixMode(GL_MODELVIEW);

	//escolher a proxima peça
	Point_2 nextPiece;
	int nextPolygonIndex;
	GLdouble* nextPlacement = NULL;

	//escolher uma peça e uma rotacao
	nextPiece = *drawingStatus.piecesOrdered.begin();
	nextPolygonIndex = (int)nextPiece.x();

	//procurar posição para essa peça
	clock_t begin = clock();
	/*
	//desenhar nfps dessa peça com o layout
	glClear(GL_COLOR_BUFFER_BIT);
	DrawingWithRotations *d=&currentDrawingNFPs[nextPiece];
	glColor3f(0,0.0,1.0);
	for(int i=0; i < d->getPositions().size(); i++)
	{
		glPushMatrix();
		//glColor4f	(d->getColors()[i][0], d->getColors()[i][1], d->getColors()[i][2], d->getColors()[i][3]);
		glTranslatef(d->getPositions()[i][0], d->getPositions()[i][1], d->getPositions()[i][2]);
		glCallList	(d->getLists()[d->getOrder()[i].first][d->getOrder()[i].second]);
		glPopMatrix	();
	}
	*/

	//desenhar os nfps dos outros poligonos para o layout com a nova peça adicionada
	for (map<Point_2, DrawingWithRotations>::iterator itNFPsOfPiecesToPlaceIndex = drawingStatus.currentDrawingNFPs.begin();
		itNFPsOfPiecesToPlaceIndex != drawingStatus.currentDrawingNFPs.end();
		itNFPsOfPiecesToPlaceIndex++)
	{
		DrawingWithRotations* d = &itNFPsOfPiecesToPlaceIndex->second;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		//desenhar nfps da iteracao anterior
		glGetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING, &previousBind);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, d->bufferObject);
		glRasterPos3d(0.0, 0.0, 0.0);
		glDrawPixels(drawingStatus.tw, drawingStatus.th, GL_BLUE, GL_UNSIGNED_BYTE, 0);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, previousBind);
		glGetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING, &previousBind);

		//desenhar nfp da iteracao actual
		glColor3f(0.0, 0.0, 1.0);
		glPushMatrix();
		glTranslatef(d->getPositions().back()[0], d->getPositions().back()[1], d->getPositions().back()[2]);
		glCallList(d->getLists()[d->getOrder().back().first][d->getOrder().back().second]);
		glPopMatrix();

		//le imagem dos nfps para o PBO
		glReadBuffer(GL_BACK_LEFT);
		glGetIntegerv(GL_PIXEL_PACK_BUFFER_BINDING, &previousBind);
		glBindBuffer(GL_PIXEL_PACK_BUFFER, d->bufferObject);
#ifdef DEBUG
		cout << glGetError() << endl;
#endif
		glReadPixels(drawingStatus.tx, drawingStatus.ty, drawingStatus.tw, drawingStatus.th, GL_BLUE, GL_UNSIGNED_BYTE, 0);
#ifdef DEBUG
		cout << glGetError() << endl;
#endif
		glBindBuffer(GL_PIXEL_PACK_BUFFER, previousBind);

		//se for o pbo da peça que foi colocada, ler imagem para memoria
		if (itNFPsOfPiecesToPlaceIndex->first.x() == nextPiece.x() && itNFPsOfPiecesToPlaceIndex->first.y() == nextPiece.y())
		{
			//le PBO para openCV
			glGetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING, &previousBind);
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, d->bufferObject);

			//determinar a posicao da peça
			IplImage* imgTest = cvCreateImage(cvSize(drawingStatus.tw, drawingStatus.th), IPL_DEPTH_8U, 1);
			imgTest->imageData = (char*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_READ_ONLY);

			//DEBUG guardar imagem
#ifdef DEBUG
			cv::Mat imgTestMat = cv::cvarrToMat(imgTest);
			char imageName[100];
			sprintf_s(imageName, "iteration %d piece %d %d piecesPlaced %d .jpg", iteration, (int)nextPiece.x(), (int)nextPiece.y(), drawingStatus.currentDrawingNFPs.begin()->second.getPositions().size());
			cv::imwrite(imageName, imgTestMat);
#endif

			nextPlacement = getPiecePosition(imgTest, layout);

			//se nao for possivel colocar essa peça terminar
			if (nextPlacement == NULL)
			{
				return false;
			}

			glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, previousBind);
			cvReleaseImage(&imgTest);
		}
	}


	clock_t end = clock();
	double timePlacement = diffclock(end, begin);
	begin = clock();




	end = clock();
	double timeUpdating = diffclock(end, begin);
	//colocar a peça nos desenhos dos poligonos e dos nfps
	drawingStatus.currentDrawingPolys.addListPlacement(nextPiece.x(), nextPiece.y(), nextPlacement, drawingStatus.blue);

#ifdef DEBUG
	cout << "Piece: " << drawingStatus.currentDrawingPolys.getOrder().back().first << " " << drawingStatus.currentDrawingPolys.getOrder().back().second << endl;
	cout << "Position: " << drawingStatus.currentDrawingPolys.getPositions().back()[0] << " " << drawingStatus.currentDrawingPolys.getPositions().back()[1] << endl;
	cout << "List: " << drawingStatus.currentDrawingPolys.getLists()[drawingStatus.currentDrawingPolys.getOrder().back().first][drawingStatus.currentDrawingPolys.getOrder().back().second] << endl;
#endif

	//currentDrawingNFPs
	for (map<Point_2, DrawingWithRotations>::iterator itDrawing = drawingStatus.currentDrawingNFPs.begin();
		itDrawing != drawingStatus.currentDrawingNFPs.end();
		itDrawing++)
	{
		itDrawing->second.addListPlacement(nextPiece.x(), nextPiece.y(), nextPlacement, drawingStatus.blue);
	}

	//apagar a peça da lista de peças a colocar
	drawingStatus.piecesOrdered.erase(drawingStatus.piecesOrdered.begin());
	if (drawingStatus.piecesOrdered.empty()) {
		drawingStatus.currentDrawingNFPs.clear();
		//glDeleteBuffers(1, &pboNFPs);
	}

	*myfile << iteration << "; " << timePlacement << ";" << timeUpdating << endl;

	iteration++;
	return true;
}

GLdouble* LayoutBuilder::getPiecePosition(IplImage* layoutImg, Layout layout)
{
	vector<vector<cv::Point>> feasiblePositions = getFeasiblePositions(layoutImg);
	vector<vector<GLdouble*>> feasiblePositionsConverted(feasiblePositions.size());

	for (int i = 0; i < feasiblePositions.size(); i++)
	{
		feasiblePositionsConverted[i] = vector<GLdouble*>(feasiblePositions[i].size());
		for (int j = 0; j < feasiblePositions[i].size(); j++) {
			if (feasiblePositions[i][j].x == 1) feasiblePositions[i][j].x = 0;
			if (feasiblePositions[i][j].y == 1) feasiblePositions[i][j].y = 0;

			feasiblePositionsConverted[i][j] = createPosition(((double)feasiblePositions[i][j].x + layout.getMargin()) * layout.getResolution(), ((double)feasiblePositions[i][j].y + layout.getMargin()) * layout.getResolution(), 0.0);
		}
	}

	//aplicar bottom left
	GLdouble* position = NULL;
	if (feasiblePositionsConverted.size() > 0) {
		position = bottomLeft(feasiblePositionsConverted);
	}
	//deleting other positions to avoid memory leaks
	for (int i = 0; i < feasiblePositionsConverted.size(); i++)
	{
		for (int j = 0; j < feasiblePositionsConverted[i].size(); j++)
		{
			if (feasiblePositionsConverted[i][j] != position) {
				delete[] feasiblePositionsConverted[i][j];
			}
		}
	}
	//returning bottom left position or NULL as default
	return position;
		
}

vector<vector<cv::Point>> LayoutBuilder::getFeasiblePositions(IplImage* image)
{
	cv::Mat imageMat = cv::cvarrToMat(image);
	return showContours(imageMat);
}


pair<vector<vector<cv::Point>>, vector<cv::Vec4i>> LayoutBuilder::getContours(cv::Mat mat, double maxDist)
{
	vector<vector<cv::Point>> contours;
	vector<cv::Vec4i> hierarchy;
	cv::findContours(mat, contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE);

	vector<Piece> pieces = contourVecPieceVec(contours);
	ofstream f;
	/*
	f.open("pieces.txt");
	f<<"Antes: "<<endl;
	for(int i=0; i < pieces.size(); i++)
	{
		f<<"Piece "<<i<<endl;
		f<<pieces[i];
	}
	*/
	vector<vector<cv::Point>> contoursSimplification;
	for (int i = 0; i < contours.size(); i++)
	{
		contoursSimplification.push_back(vector<cv::Point>());
		cv::approxPolyDP(contours[i], contoursSimplification[i], maxDist, true);
	}


	//cout<<"Depois: "<<endl;
	pieces = contourVecPieceVec(contoursSimplification);
	/*
	for(int i=0; i < pieces.size(); i++)
	{
		cout<<"Piece "<<i<<endl;
		pieces[i].print();
	}
	*/
	return pair<vector<vector<cv::Point>>, vector<cv::Vec4i>>(contoursSimplification, hierarchy);
}

vector<vector<cv::Point>> LayoutBuilder::showContours(cv::Mat thrImgMat)
{
	cv::Mat cntrImgMat(thrImgMat.rows, thrImgMat.cols, CV_8UC1, cv::Scalar(0));
	cv::Mat invCntrImgMat(thrImgMat.rows, thrImgMat.cols, CV_8UC1, cv::Scalar(0));
	cv::Mat displayImgMat(thrImgMat.rows * 2/*3*/, thrImgMat.cols, CV_8UC1, cv::Scalar(0));
	cv::Mat roi;

	vector<vector<cv::Point>> contours;
	vector<vector<cv::Point>> invContours;
	vector<cv::Vec4i> hierarchy;
	vector<cv::Vec4i> invHierarchy;

	//desenhar imagem inicial
	roi = cv::Mat(displayImgMat, cv::Rect(cv::Point(0, 0), thrImgMat.size()));
	thrImgMat.copyTo(roi);

	//desenhar contornos invertidos
	cv::threshold(thrImgMat, invCntrImgMat, 0, 255, cv::ThresholdTypes::THRESH_BINARY_INV);
	pair<vector<vector<cv::Point>>, vector<cv::Vec4i>> contoursPair = getContours(invCntrImgMat, 1.0);
	contours = contoursPair.first;
	cv::drawContours(invCntrImgMat, contours, -1, cv::Scalar(255));
	roi = cv::Mat(displayImgMat, cv::Rect(cv::Point(0, invCntrImgMat.rows/*2*/), invCntrImgMat.size()));
	invCntrImgMat.copyTo(roi);

	//mostrar imagem
	//cvConvertImage(displayImgMat, displayImgMat, CV_CVTIMG_FLIP);
	const char* windowName = "Threshold Evaluation";
	cv::imshow(windowName, displayImgMat);

#ifdef DEBUG
	//cv::imwrite(windowName, displayImgMat);
#endif
	cntrImgMat.release();
	invCntrImgMat.release();
	displayImgMat.release();
	return contours;
}

GLdouble* LayoutBuilder::bottomLeft(vector<vector<GLdouble*>> feasiblePositions)
{
	//escolher o ponto mais abaixo e mais à esquerda dos contornos
	GLdouble* position = createPosition(numeric_limits<int>::max(), numeric_limits<int>::max(), 0.0);
	for (int i = 0; i < feasiblePositions.size(); i++)
	{
		for (int j = 0; j < feasiblePositions[i].size(); j++)
		{
			if (feasiblePositions[i][j][0] < position[0] ||
				feasiblePositions[i][j][0] == position[0] && feasiblePositions[i][j][1] < position[1])
			{
				//delete position;
				position = feasiblePositions[i][j];
			}
		}
	}

	return position;
}

vector<Piece> LayoutBuilder::contourVecPieceVec(vector<vector<cv::Point>> contours)
{
	vector<Piece> pieces;

	for (int i = 0; i < contours.size(); i++)
	{
		Piece p;
		p.addComponent(pointVec2Component(contours[i]));
		pieces.push_back(p);
	}
	return pieces;
}

Component LayoutBuilder::pointVec2Component(vector<cv::Point> poly, bool rev)
{
	Component c;

	if (rev)
		for (int i = poly.size() - 1; i >= 0; i--)
			c.push_back(createPosition(poly[i].x, poly[i].y, 0.0));
	else
		for (int i = 0; i < poly.size(); i++)
			c.push_back(createPosition(poly[i].x, poly[i].y, 0.0));

	return c;
}


bool LayoutBuilder::displayDynamic(Selection& userSelection, Layout& layout, SolvingStatus& solvingStatus, DrawingStatus& drawingStatus, ofstream* myfile)
{
	GLUI_Master.get_viewport_area(&drawingStatus.tx, &drawingStatus.ty, &drawingStatus.tw, &drawingStatus.th);
	//cout<<"tw th "<<tw<<" "<<th<<endl;

	int previousBind = 0;
	GLfloat blue[4] = { 0.0, 0.0, 1.0, 1.0 };
	set<Point_2> pointsToDelete;
	if (iteration == 0)	displayDynamic_drawCurrentLayout(drawingStatus);

	//definir dimensoes do viewport
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	BoundingBox layoutBB = drawingStatus.drawingNFPsTest.begin()->second.begin()->second.getViewPort();
	gluOrtho2D(layoutBB.getX(), layoutBB.getWidth(), layoutBB.getY(), layoutBB.getHeight());
	glMatrixMode(GL_MODELVIEW);

	double findPositioningTime, addPlacementTime, drawLayoutsTime, savePBOTime;

	findPositioningTime = 0;
	addPlacementTime = 0;
	drawLayoutsTime = 0;
	savePBOTime = 0;

	//experimentar colocar todas as peças
	clock_t begin = clock();


	for (map<Point_2, map<Point_2, DrawingWithRotations>>::iterator itPolygonBeingTestedIndex = drawingStatus.drawingNFPsTest.begin();
		itPolygonBeingTestedIndex != drawingStatus.drawingNFPsTest.end();
		itPolygonBeingTestedIndex++)
	{

#ifdef DEBUG
		cout << "Sizes iteration " << iteration << ": " << drawingStatus.drawingNFPsTest.size() << " " << drawingStatus.layoutNFPsTest.size() << endl;
#endif

		Point_2 polygonRotationBeingTested = itPolygonBeingTestedIndex->first;
		int polygonBeingTestedPiecesIndex = polygonRotationBeingTested.x();
		int rotationBeingTestedPiecesIndex = polygonRotationBeingTested.y();

		//determinar a posiçao da peça a experimentar
		clock_t lala = clock();
		GLdouble* position;
		IplImage* imgTest = cvCreateImage(cvSize(drawingStatus.tw, drawingStatus.th), IPL_DEPTH_8U, 1);
		char* initPtr = imgTest->imageData;
		glGetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING, &previousBind);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, drawingStatus.currentDrawingNFPs[polygonRotationBeingTested].bufferObject);
		imgTest->imageData = (char*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_READ_ONLY);


#ifdef DEBUG
		cv::Mat imgTestMat = cv::cvarrToMat(imgTest);
		char imageName[100];
		sprintf_s(imageName, "iteration %d currentNFPs beforePositionCalc %d %d.jpg", iteration, polygonBeingTestedPiecesIndex, rotationBeingTestedPiecesIndex);
		cv::imwrite(imageName, imgTestMat);
#endif

		position = getPiecePosition(imgTest, layout);
		glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, previousBind);
		imgTest->imageData = initPtr;
		cvReleaseImage(&imgTest);

		clock_t lalala = clock();
		findPositioningTime += diffclock(lalala, lala);

		//se nao for possivel posicionar a peça, marca-la para nao a testar nas iteraçoes seguintes
		if (position == 0)
		{
			//cout<<"LE FU\n";
			//nao e possivel colocar 
			if (--solvingStatus.piecesAvaliability[polygonBeingTestedPiecesIndex] == 0)
			{
				//cout<<"ERROR: not all pieces could be placed! Terminating."<<endl;
				drawingStatus.layoutNFPsTest.clear();
				drawingStatus.currentDrawingNFPs.clear();
				drawingStatus.drawingNFPsTest.clear();
				drawingStatus.drawingPolysTest.clear();
				return false;
			}
			else
			{
				pointsToDelete.insert(itPolygonBeingTestedIndex->first);
			}
		}
		else
		{
			//se for possivel posicionar a peça, desenhar as imagens dos nfps para todas as peças possiveis de colocar a seguir
			//posicionar a peça no layout de teste das peças
#ifdef DEBUG
			cout << "Position " << position[0] << " " << position[1] << endl;
#endif
			DrawingWithRotations* d = &drawingStatus.drawingPolysTest[polygonRotationBeingTested];

			//adiciona posicao ao layout
			d->addListPlacement(polygonBeingTestedPiecesIndex, rotationBeingTestedPiecesIndex, position, blue);

			//desenhar os nfps dos outros poligonos para o layout com a nova peça adicionada
			for (map<Point_2, DrawingWithRotations>::iterator itDrawing = itPolygonBeingTestedIndex->second.begin();
				itDrawing != itPolygonBeingTestedIndex->second.end();
				itDrawing++)
			{
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

				//se este poligono nao tiver sido marcado para apagar
				if (pointsToDelete.find(itDrawing->first) == pointsToDelete.end())
				{
					lala = clock();
					//adicionar nfp do poligono
					DrawingWithRotations* dnfps = &itDrawing->second;
					dnfps->addListPlacement(polygonBeingTestedPiecesIndex, rotationBeingTestedPiecesIndex, position, blue);

					//desenhar nfps da iteracao anterior
					glGetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING, &previousBind);
					glBindBuffer(GL_PIXEL_UNPACK_BUFFER, drawingStatus.currentDrawingNFPs[itDrawing->first].bufferObject/*dnfps->bufferObject*/);
					glRasterPos3d(0.0, 0.0, 0.0);
					glDrawPixels(drawingStatus.tw, drawingStatus.th, GL_BLUE, GL_UNSIGNED_BYTE, 0);
					glBindBuffer(GL_PIXEL_UNPACK_BUFFER, previousBind);
					glGetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING, &previousBind);

					//desenhar nfp da iteracao actual
					glColor3f(0.0, 0.0, 1.0);
					glPushMatrix();
					glTranslatef(dnfps->getPositions().back()[0], dnfps->getPositions().back()[1], dnfps->getPositions().back()[2]);
					glCallList(dnfps->getLists()[dnfps->getOrder().back().first][dnfps->getOrder().back().second]);
					glPopMatrix();

#ifdef DEBUG
					cv::Mat imgTestMat = getOpenCVImage(drawingStatus.tx, drawingStatus.ty, drawingStatus.tw, drawingStatus.th, GL_BLUE);
					char imageName[100];
					sprintf_s(imageName, "iteration %d currentNFPs beforePositionCalc %d %d - %d%d.jpg", iteration, polygonBeingTestedPiecesIndex, rotationBeingTestedPiecesIndex, (int)itDrawing->first.x(), (int)itDrawing->first.y());
					cv::imwrite(imageName, imgTestMat);
					cvReleaseImage(&imgTest);
#endif

					//renderizar nfps do poligono
					lalala = clock();
					addPlacementTime += diffclock(lalala, lala);

					//adicionar todas as imagem dos nfps dos poligonos no buffer de acumulacao
					GLfloat alpha = 1.0 / itPolygonBeingTestedIndex->second.size();
					glAccum(GL_ACCUM, alpha);
					lalala = clock();
					drawLayoutsTime += diffclock(lalala, lala);

					//guardar a imagem dos nfps do poligonos
					lala = clock();
					glReadBuffer(GL_BACK_LEFT);
					glGetIntegerv(GL_PIXEL_PACK_BUFFER_BINDING, &previousBind);
					glBindBuffer(GL_PIXEL_PACK_BUFFER, dnfps->bufferObject);
					glReadPixels(drawingStatus.tx, drawingStatus.ty, drawingStatus.tw, drawingStatus.th, GL_BLUE, GL_UNSIGNED_BYTE, 0);
					glBindBuffer(GL_PIXEL_PACK_BUFFER, previousBind);
					lalala = clock();
					savePBOTime += diffclock(lalala, lala);
				}
			}

			//ler a imagem dos nfps de todos os poligonos acumulados
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glAccum(GL_RETURN, 1.0);
			glClear(GL_ACCUM_BUFFER_BIT);

#ifdef DEBUG
			cv::Mat imgTest = getOpenCVImage(drawingStatus.tx, drawingStatus.ty, drawingStatus.tw, drawingStatus.th, GL_BLUE);
			char imageName[100];
			sprintf_s(imageName, "iteration %d NFPs pieces %d %d overlaping.jpg", iteration, polygonBeingTestedPiecesIndex, rotationBeingTestedPiecesIndex);
			cv::imwrite(imageName, imgTest);
			imgTest.release();
#endif

			//draw in black layout pieces <--- WRONG! layout and nfps images cannot be combined as they represent differrent things
			//glClear(GL_DEPTH_BUFFER_BIT);
			/*glColor3f(0.0, 0.0, 0.0);
			for (int i = 0; i < d->getPositions().size(); i++)
			{
				glPushMatrix();
				glTranslatef(d->getPositions()[i][0], d->getPositions()[i][1], d->getPositions()[i][2]);
				glCallList(d->getLists()[d->getOrder()[i].first][d->getOrder()[i].second]);
				glPopMatrix();
			}

#ifdef DEBUG
			imgTest = getOpenCVImage(drawingStatus.tx, drawingStatus.ty, drawingStatus.tw, drawingStatus.th, GL_BLUE);
			sprintf_s(imageName, "iteration %d NFPs pieces %d %d overlaping difference.jpg", iteration, polygonBeingTestedPiecesIndex, rotationBeingTestedPiecesIndex);
			cv::imwrite(imageName, imgTest);
			imgTest.release();
#endif*/

			//ler imagem da soma dos nfps com a diferença do espaço ocupado pelas peças 
			glReadBuffer(GL_BACK_LEFT);
			glGetIntegerv(GL_PIXEL_PACK_BUFFER_BINDING, &previousBind);
			glBindBuffer(GL_PIXEL_PACK_BUFFER, drawingStatus.layoutNFPsTest[polygonRotationBeingTested]);
			glReadPixels(drawingStatus.tx, drawingStatus.ty, drawingStatus.tw, drawingStatus.th, GL_BLUE, GL_UNSIGNED_BYTE, 0);
			glBindBuffer(GL_PIXEL_PACK_BUFFER, previousBind);
		}
	}
	clock_t end = clock();
	double placementTime = diffclock(end, begin);

	begin = clock();
	//apagar as peças que ja nao podem ser colocadas
	for (set<Point_2>::iterator it = pointsToDelete.begin(); it != pointsToDelete.end(); it++)
	{
		drawingStatus.layoutNFPsTest.erase(*it);		//map<int, IplImage*>
		drawingStatus.currentDrawingNFPs.erase(*it);	//map<int, Drawing> 
		drawingStatus.drawingNFPsTest.erase(*it);		//map<int, map<int, Drawing>>
		drawingStatus.drawingPolysTest.erase(*it);	//map<int, Drawing>
	}

	//apagar peças que ja nao podem ser colocadas das hipoteses dos layouts a ser testados
	for (map<Point_2, map<Point_2, DrawingWithRotations>>::iterator it = drawingStatus.drawingNFPsTest.begin(); it != drawingStatus.drawingNFPsTest.end(); it++)
	{
		for (set<Point_2>::iterator itd = pointsToDelete.begin(); itd != pointsToDelete.end(); itd++)
		{
			it->second.erase(*itd);
		}
	}
	Point_2 placedPiece = putPieceDynamic(layout, solvingStatus,  drawingStatus, false);

	end = clock();

	double updatingTime = diffclock(end, begin);
	*myfile << iteration << ";" << placementTime << ";" << updatingTime << ";" << drawingStatus.currentDrawingPolys.getPositions().size() << ";" << drawingStatus.drawingNFPsTest.size() << ";" << placedPiece << ";" << findPositioningTime << ";" << addPlacementTime << "; " << drawLayoutsTime << "; " << savePBOTime << ";" << endl;
	iteration++;
	return true;
}


void LayoutBuilder::buildDynamicLayout(Selection& userSelection, Layout& layout, SolvingStatus& solvingStatus, DrawingStatus& drawingStatus)
{
	ofstream myfile;
	char filename[100];
	sprintf_s(filename, "dynamicTests %s.csv", userSelection.problemName);
	myfile.open(filename);
	myfile << "Iteration; Placement time ; Updating time; Placed pieces; Avaliable types; Placed piece; Time positioning1;Time positioning2; Time drawing1; Time drawing2" << endl;
	//*myfile<<iteration<<";"<<placementTime<<";"<<updatingTime<<";"<<currentDrawingPolys.getPositions().size()<<";"<<drawingNFPsTest.size()<<";"<<placedPiece<<";"<<findPositioningTime<<";"<<addPlacementTime<<"; "<<drawLayoutsTime<<"; "<<savePBOTime<<";"<<endl;

	clock_t begin = clock();
	bool lastIterationSuccessful = false;
	while (drawingStatus.currentDrawingNFPs.size() > 0)
		lastIterationSuccessful = displayDynamic(userSelection, layout, solvingStatus, drawingStatus, &myfile);
	if (!lastIterationSuccessful)
		cout << "ERROR: not all pieces could be placed! Terminating." << endl;
	else
		cout << "SUCCESS: all pieces placed!" << endl;
	clock_t end = clock();

	myfile << "TOTAL;" << diffclock(end, begin) << endl;
	double maxLength = calculateMaxLength(drawingStatus.currentDrawingPolys, solvingStatus.polygons);
	myfile << "LENGTH;" << maxLength << endl;
	myfile << "USED AREA;" << calculateUsedArea(layout, solvingStatus.numberOfPolygons, solvingStatus.polygons, maxLength) << endl;
	myfile << "WIDTH;" << layout.stockSheetsBoundingBox().getHeight();
	myfile.close();
}

void LayoutBuilder::displayDynamic_drawCurrentLayout(DrawingStatus &drawingStatus)
{
	glDisable(GL_BLEND);

	//definir dimensoes do viewport
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	BoundingBox layoutBB = drawingStatus.drawingNFPsTest.begin()->second.begin()->second.getViewPort();
	gluOrtho2D(layoutBB.getX(), layoutBB.getWidth(), layoutBB.getY(), layoutBB.getHeight());	// set to orthogonal projection
	glMatrixMode(GL_MODELVIEW);			// switch to modelview matrix

	GLUI_Master.get_viewport_area(&drawingStatus.tx, &drawingStatus.ty, &drawingStatus.tw, &drawingStatus.th);

	for (map<Point_2, DrawingWithRotations>::iterator itNFPsOfPiecesToPlaceIndex = drawingStatus.currentDrawingNFPs.begin();
		itNFPsOfPiecesToPlaceIndex != drawingStatus.currentDrawingNFPs.end();
		itNFPsOfPiecesToPlaceIndex++)
	{
		DrawingWithRotations* d = &itNFPsOfPiecesToPlaceIndex->second;
		glClear(GL_COLOR_BUFFER_BIT);
		glColor3f(0.0, 0.0, 1.0);
		for (int i = 0; i < d->getPositions().size(); i++)
		{
			glPushMatrix();
			//glColor4f	(d->getColors()[i][0], d->getColors()[i][1], d->getColors()[i][2], d->getColors()[i][3]);
			glTranslatef(d->getPositions()[i][0], d->getPositions()[i][1], d->getPositions()[i][2]);
			glCallList(d->getLists()[d->getOrder()[i].first][d->getOrder()[i].second]);
			glPopMatrix();
		}
		//d->bufferObject=drawingNFPsTest[Point_2(0,0)][itNFPsOfPiecesToPlaceIndex->first].bufferObject;
		//IplImage *imgTest = cvCreateImage(cvSize(wWidth, wHeight), IPL_DEPTH_8U,1);

		int previousBind = 0;
		glReadBuffer(GL_BACK_LEFT);
		glGetIntegerv(GL_PIXEL_PACK_BUFFER_BINDING, &previousBind);
		glBindBuffer(GL_PIXEL_PACK_BUFFER, d->bufferObject);
		cout << glGetError() << endl;
		glReadPixels(drawingStatus.tx, drawingStatus.ty, drawingStatus.tw, drawingStatus.th, GL_BLUE, GL_UNSIGNED_BYTE, 0);
		cout << glGetError() << endl;
		glBindBuffer(GL_PIXEL_PACK_BUFFER, previousBind);
		//glutSwapBuffers();
		for (map<Point_2, map<Point_2, DrawingWithRotations>>::iterator itTest = drawingStatus.drawingNFPsTest.begin();
			itTest != drawingStatus.drawingNFPsTest.end();
			itTest++)
		{
			glReadBuffer(GL_BACK_LEFT);
			glGetIntegerv(GL_PIXEL_PACK_BUFFER_BINDING, &previousBind);
			glBindBuffer(GL_PIXEL_PACK_BUFFER, itTest->second[itNFPsOfPiecesToPlaceIndex->first].bufferObject);
			glReadPixels(drawingStatus.tx, drawingStatus.ty, drawingStatus.tw, drawingStatus.th, GL_BLUE, GL_UNSIGNED_BYTE, 0);
			glBindBuffer(GL_PIXEL_PACK_BUFFER, previousBind);
		}

		glClear(GL_COLOR_BUFFER_BIT);

		//cout<<"d->bufferObject: "<<d->bufferObject<<endl;

		int p = (int)itNFPsOfPiecesToPlaceIndex->first.x();
		int r = (int)itNFPsOfPiecesToPlaceIndex->first.y();

		//d->bufferObject=drawingNFPsTest[Point_2(0,0)][itNFPsOfPiecesToPlaceIndex->first].bufferObject;
#ifdef DEBUG
		IplImage* imgTest = cvCreateImage(cvSize(drawingStatus.tw, drawingStatus.th), IPL_DEPTH_8U, 1);
		cv::Mat imgTestMat = cv::cvarrToMat(imgTest);
		char* initPtr = imgTest->imageData;
		glGetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING, &previousBind);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, d->bufferObject);
		imgTest->imageData = (char*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_READ_ONLY);
		char imageName[100];
		sprintf_s(imageName, "INIT %d %d.jpg", p, r);
		cv::imwrite(imageName, imgTestMat);
		glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, previousBind);
		imgTest->imageData = initPtr;
		cvReleaseImage(&imgTest);
		imgTestMat.release();
#endif
	}
}


Point_2 LayoutBuilder::putPieceDynamic(Layout& layout, SolvingStatus& solvingStatus, DrawingStatus& drawingStatus, bool reset)
{

	if (reset)
	{
		delete[] blue;
		blue = createColor(0.0, 0.0, 1.0, 1.0 / solvingStatus.numberOfPolygons);
		delete[] green;
		green = createColor(0.0, 1.0, 0.0, 0.1);
		delete[] placedPieces;
		placedPieces = new int[layout.getPieces().size()];

		for (int i = 0; i < layout.getPieces().size(); i++)
		{
			placedPieces[i] = 0;
			piecesToPlace.push_back(layout.getQuantity()[i]);
		}
		return Point_2();
	}
	else
	{
		Point_2 nextPiece = dynamicPieceSelection(drawingStatus);

		int nextPolygonIndex = nextPiece.x();
		placedPieces[nextPolygonIndex]++;
		piecesToPlace[nextPolygonIndex]--;
#ifdef DEBUG
		cout << "Next piece: " << nextPiece.x() << " " << nextPiece.y() << endl;
		cout << "Pieces to place:" << endl;
		for (int i = 0; i < solvingStatus.numberOfPolygons; i++) {
			cout << "Peca " << i << ": " << piecesToPlace[i] << "/" << layout.getQuantity()[i] << "; ";
		}
		cout << endl << endl;
#endif
		//save next placement pieces drawings
		GLdouble* nextPlacement = drawingStatus.drawingPolysTest[nextPiece].getPositions().back();
#ifdef DEBUG
		cout << "Next piece position: " << nextPlacement[0] << " " << nextPlacement[1] << endl;
#endif
		for (map<Point_2, DrawingWithRotations>::iterator itImage = drawingStatus.currentDrawingNFPs.begin();
			itImage != drawingStatus.currentDrawingNFPs.end();
			itImage++)
		{
			GLuint bufferObjectBackup = itImage->second.bufferObject;
			itImage->second.bufferObject = drawingStatus.drawingNFPsTest[nextPiece][itImage->first].bufferObject;
			drawingStatus.drawingNFPsTest[nextPiece][itImage->first].bufferObject = bufferObjectBackup;

			/*
			int previousBind;
			IplImage *imgTest = cvCreateImage(cvSize(wWidth, wHeight), IPL_DEPTH_8U,1);
			glGetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING, &previousBind);
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, itImage->second.bufferObject);

			imgTest->imageData = (char*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_READ_ONLY);

			char imageName[100];
			sprintf(imageName, "iteration %d currentNFPs %d %d.jpg", iteration, (int)itImage->first.x(), (int)itImage->first.y());
			cvSaveImage(imageName, imgTest);

			glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, previousBind);
			cvReleaseImage(&imgTest);
			*/
		}

		//if no more pieces nextPiece, erase from map those pictures! 
		if (piecesToPlace[nextPolygonIndex] == 0)
		{
#ifdef DEBUG
			cout << "Deleting piece " << nextPolygonIndex << endl;
#endif
			solvingStatus.piecesAvaliability[nextPolygonIndex] = -1;
			vector<Point_2> keysToDelete;
			for (map<Point_2, DrawingWithRotations>::iterator iterase = drawingStatus.currentDrawingNFPs.begin();
				iterase != drawingStatus.currentDrawingNFPs.end();
				iterase++)
			{
				if ((int)iterase->first.x() == nextPolygonIndex) {
					keysToDelete.push_back(iterase->first);
				}
			}

			for (int i = 0; i < keysToDelete.size(); i++)
			{
				//cvReleaseImage(&currentLayoutNFPs[keysToDelete[i]]);
				//currentLayoutNFPs.erase(keysToDelete[i]);	//map<int, IplImage*> 
				drawingStatus.layoutNFPsTest.erase(keysToDelete[i]);		//map<int, IplImage*>
				drawingStatus.currentDrawingNFPs.erase(keysToDelete[i]);	//map<int, Drawing> 
				drawingStatus.drawingNFPsTest.erase(keysToDelete[i]);		//map<int, map<int, Drawing>>
				drawingStatus.drawingPolysTest.erase(keysToDelete[i]);	//map<int, Drawing>
			}

			for (map<Point_2, map<Point_2, DrawingWithRotations>>::iterator it = drawingStatus.drawingNFPsTest.begin(); it != drawingStatus.drawingNFPsTest.end(); it++)
			{
				for (int i = 0; i < keysToDelete.size(); i++)
				{
					it->second.erase(keysToDelete[i]);
				}
			}
		}

		//add new placement to drawings
		drawingStatus.currentDrawingPolys.addListPlacement(nextPiece.x(), nextPiece.y(), nextPlacement, blue);

		//currentDrawingNFPs
		for (map<Point_2, DrawingWithRotations>::iterator itDrawing = drawingStatus.currentDrawingNFPs.begin();
			itDrawing != drawingStatus.currentDrawingNFPs.end();
			itDrawing++)
		{
			itDrawing->second.addListPlacement(nextPiece.x(), nextPiece.y(), nextPlacement, blue);
		}

		//drawingPolysTest
		for (map<Point_2, DrawingWithRotations>::iterator itDrawing = drawingStatus.drawingPolysTest.begin();
			itDrawing != drawingStatus.drawingPolysTest.end();
			itDrawing++)
		{
			if (itDrawing->first != nextPiece)
			{
				itDrawing->second.popLastPlacement();
				itDrawing->second.addListPlacement(nextPiece.x(), nextPiece.y(), nextPlacement, blue);
			}
		}

		//drawingNFPsTest
		for (map<Point_2, map<Point_2, DrawingWithRotations>>::iterator itDrawingNFPsTest = drawingStatus.drawingNFPsTest.begin();
			itDrawingNFPsTest != drawingStatus.drawingNFPsTest.end();
			itDrawingNFPsTest++)
		{
			for (map<Point_2, DrawingWithRotations>::iterator itDrawing = itDrawingNFPsTest->second.begin();
				itDrawing != itDrawingNFPsTest->second.end();
				itDrawing++)
			{
				//if(itDrawing->first != nextPiece)
				//{
				itDrawing->second.popLastPlacement();
				itDrawing->second.addListPlacement(nextPiece.x(), nextPiece.y(), nextPlacement, blue);
				//}
			}
		}
		return nextPiece;
	}
	//cout<<"Sizes after after "<<currentLayoutNFPs.size()<<" "<<currentDrawingNFPs.size()<<" "<<layoutNFPsTest.size()<<" "<<drawingNFPsTest.size()<<" "<<drawingPolysTest.size()<<endl;
}


Point_2 LayoutBuilder::dynamicPieceSelection(DrawingStatus drawingStatus)
{
	//cout<<"Iteration: "<<iteration<<endl;
	Point_2 pieceToplace(-1, -1);
	int previousWastePixels = numeric_limits<int>::max();

	IplImage* imgTest = cvCreateImage(cvSize(drawingStatus.tw, drawingStatus.th), IPL_DEPTH_8U, 1);
	void* initPtr = imgTest->imageData;
	double intMinThreshold, intMaxThreshold;
	for (map<Point_2, GLuint>::iterator itImg = drawingStatus.layoutNFPsTest.begin();
		itImg != drawingStatus.layoutNFPsTest.end();
		++itImg)
	{

		int previousBind;
		glGetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING, &previousBind);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, itImg->second);

		//cout<<"PBO: "<<(int)itImg->second<<endl;
		imgTest->imageData = (char*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_READ_WRITE);
		//cout<<"ptr: "<<(int)imgTest->imageData<<endl;
		cvMinMaxLoc(imgTest, &intMinThreshold, &intMaxThreshold);
		intMaxThreshold--;

		cv::Mat imgTestMat = cv::cvarrToMat(imgTest);

		cv::threshold(imgTestMat, imgTestMat, intMaxThreshold, 255, cv::THRESH_BINARY);
#ifdef DEBUG

		char imageName[100];
		sprintf_s(imageName, "iteration %d NFPs PBO %d %d.jpg", iteration, (int)itImg->first.x(), (int)itImg->first.y());
		cv::imwrite(imageName, imgTestMat);
#endif
		int wastePixels = cvCountNonZero(imgTest);

		if (wastePixels < previousWastePixels) {
#ifdef DEBUG
			cout << itImg->first << " previousWastePixels: " << previousWastePixels << endl;
#endif
			pieceToplace = itImg->first;
			previousWastePixels = wastePixels;
		}
#ifdef DEBUG
		if (intMaxThreshold < 254) cout << "MaxThreshold in dynamicPieceSelection WTF?\n";
#endif
		glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, previousBind);
		double intMaxThreshold, intMinThreshold;

	}
	imgTest->imageData = (char*)initPtr;
	cvReleaseImage(&imgTest);

	return pieceToplace;
}

cv::Mat LayoutBuilder::getOpenCVImage(int xb, int yb, int width, int height, int channel)
{
	IplImage* img;
	int format;

	if (channel != GL_BLUE && channel != GL_RED && channel != GL_GREEN)
	{
		format = GL_BGR_EXT;
		img = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
	}
	else {
		format = channel;
		img = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
	}

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadBuffer(GL_BACK_LEFT);
	glReadPixels(xb, yb, width, height, format, GL_UNSIGNED_BYTE, img->imageData);
	return cv::cvarrToMat(img);
}

void LayoutBuilder::cleanup() {
	
	iteration = 0;
	delete[] blue;
	delete[] green;
	delete[] placedPieces;
}