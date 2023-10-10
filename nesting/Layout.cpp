#include "Layout.h"

Layout::Layout() 
{

}

Layout::Layout(vector<Piece> ps, vector<Piece> ss)
{
	pieces = ps;
	stockSheet = ss;
}


Layout::~Layout(void)
{

}


Layout::Layout(Problem p, vector<Piece> ss)
{
	//problem=p;
	pieces = p.getPieces();
	quantity = p.getQuantity();
	stockSheet = ss;
	rotationStep = 360;
	resolution = stockSheet[0].getBoundingBox().getHeight() / 1000;
	margin = 0;
}

Layout::Layout(Problem p, vector<Piece> ss, int rot)
{
	pieces = p.getPieces();
	quantity = p.getQuantity();
	stockSheet = ss;
	rotationStep = rot;
	resolution = stockSheet[0].getBoundingBox().getHeight() / 1000;
	margin = 0;
}

Layout::Layout(Problem p, vector<Piece> ss, int rot, double res)
{
	pieces = p.getPieces();
	quantity = p.getQuantity();
	stockSheet = ss;
	rotationStep = rot;
	resolution = res;
	margin = 0;
}

Layout::Layout(Problem p, vector<Piece> ss, int rot, double res, int marg)
{
	pieces = p.getPieces();
	quantity = p.getQuantity();
	stockSheet = ss;
	rotationStep = rot;
	resolution = res;
	margin = marg;
}

vector<Piece> Layout::getStockSheet()
{
	return stockSheet;
}

GLdouble* Layout::generateRandomPosition(int p)
{
	GLdouble point[3];
	BoundingBox pieceBB = pieces[p].getBoundingBox();
	BoundingBox stockSheetBB = stockSheet[0].getBoundingBox();
	/*
	if(pieceBB.getX() || pieceBB.getY())
	{
		p=p.clone();
		p.normalizeToOrigin();
		pieceBB = p.getBoundingBox();
	}
	*/
	point[0] = ((float)rand() / RAND_MAX) * (stockSheetBB.getWidth() - pieceBB.getWidth()) + (stockSheetBB.getX());
	point[1] = ((float)rand() / RAND_MAX) * (stockSheetBB.getHeight() - pieceBB.getHeight()) + (stockSheetBB.getY());
	point[2] = 0.0;

	return point;
}

int Layout::chooseRandomPiece()
{
	return rand() % pieces.size();
}


void Layout::setStockSheet(vector<Piece> ss)
{
	stockSheet = ss;
}

double Layout::getMaxWidth()
{
	double maxPieceWidth = 0;

	for (int i = 0; i < pieces.size(); i++)
	{
		BoundingBox pieceBB = pieces[i].getBoundingBox();
		maxPieceWidth = max(maxPieceWidth, pieceBB.getWidth());		//get max width
	}

	return maxPieceWidth;
}

double Layout::getMaxHeight()
{
	double maxPieceHeight = 0;

	for (int i = 0; i < pieces.size(); i++)
	{
		BoundingBox pieceBB = pieces[i].getBoundingBox();
		maxPieceHeight = max(maxPieceHeight, pieceBB.getHeight());	//get max height
	}

	return maxPieceHeight;
}

BoundingBox Layout::stockSheetsBoundingBox()
{
	double minX, minY;
	minX = 0;
	minY = 0;
	double w = 0;
	double h = 0;

	for (int p = 0; p < stockSheet.size(); p++)
	{
		BoundingBox spBB = stockSheet[p].getBoundingBox();

		minX = myMin(minX, spBB.getX());
		minY = myMin(minY, spBB.getY());

		h = myMax(h, spBB.getHeight());
		w += spBB.getWidth();
	}

	return BoundingBox(minX, minY, w, h);
}

void Layout::load(const char* folder, char* problemConfigFile) {
	vector<string> parameters = loadConfigurationFile(folder, problemConfigFile);
	int piecesFileFormat = atoi(parameters[1].c_str());


	Problem problem = loadFile(folder, (char*)parameters[0].c_str(), piecesFileFormat);
	int stockSheetFormat = atoi(parameters[3].c_str());
	Piece stockSheet = loadFile(folder, (char*)parameters[2].c_str(), stockSheetFormat).getPieces()[0];
	vector<Piece> ss;	ss.push_back(stockSheet); //REMENDO
	int rotation = atoi(parameters[4].c_str());
	if (rotation == 0) rotation = 360;
	double resolution = atof(parameters[5].c_str());
	int margin = atoi(parameters[6].c_str());
	if (360 % rotation != 0)
		throw exception();

	this->pieces = problem.getPieces();
	this->quantity = problem.getQuantity();
	this->stockSheet = ss;
	this->rotationStep = rotation;
	this->resolution = resolution;
	this->margin = margin;	
}


vector<string> Layout::loadConfigurationFile(const char* baseFolder, char* filename)
{
	string piecesFile, piecesFormat, stockSheetFile, stockSheetFormat, rotationStep, resolution, margin;

	vector<string> parameters;
	char filePath[1024];
	strcpy_s(filePath, baseFolder);
	strcat_s(filePath, filename);

	ifstream file(filePath);
	//get pieces file
	getFirstNonEmptyLine(&file, &piecesFile);
	getFirstNonEmptyLine(&file, &piecesFile);
	getFirstNonEmptyLine(&file, &piecesFormat);
	parameters.push_back(piecesFile);
	parameters.push_back(piecesFormat);
	//get stock sheet file
	getFirstNonEmptyLine(&file, &stockSheetFile);
	getFirstNonEmptyLine(&file, &stockSheetFile);
	getFirstNonEmptyLine(&file, &stockSheetFormat);
	parameters.push_back(stockSheetFile);
	parameters.push_back(stockSheetFormat);
	//get rotations
	getFirstNonEmptyLine(&file, &rotationStep);
	getFirstNonEmptyLine(&file, &rotationStep);
	parameters.push_back(rotationStep);
	//get resolution
	getFirstNonEmptyLine(&file, &resolution);
	getFirstNonEmptyLine(&file, &resolution);
	parameters.push_back(resolution);
	//get margin
	getFirstNonEmptyLine(&file, &margin);
	getFirstNonEmptyLine(&file, &margin);
	parameters.push_back(margin);

	file.close();
	return parameters;
}

Problem Layout::loadFile(const char* baseFolder, char* filename, bool mode)
{
	string line;

	char filePath[1024];
	strcpy_s(filePath, baseFolder);
	strcat_s(filePath, filename);

	ifstream file(filePath);
	Problem problem;

	if (file.is_open())
	{
		int nr_polygons = 0;
		int quantity = 0;
		int nr_components = 1;
		int nr_vertices = 0;

		if (mode)
		{
			getFirstNonEmptyLine(&file, &line); //le cabecalho nr de poligonos
			getFirstNonEmptyLine(&file, &line); //le nr poligonos
			nr_polygons = atoi(line.c_str());
		}

		while (file.good()) //le pecas
		{
			getFirstNonEmptyLine(&file, &line);	//elimina cabecalho poligono
			getFirstNonEmptyLine(&file, &line);	//elimina cabecalho quantidade
			getFirstNonEmptyLine(&file, &line);	//le quantidade
			quantity = atoi(line.c_str());

			if (mode)
			{
				//elimina cabecalho nr componentes
				getFirstNonEmptyLine(&file, &line);
				//le nr componenetes
				getFirstNonEmptyLine(&file, &line);
				nr_components = atoi(line.c_str());
			}
			Piece current_piece;

			//current_piece.clear(); //elimina conteudo da peca anterior
			for (int i_components = 0; i_components < nr_components; i_components++) //le componentes
			{
				if (mode)
					getFirstNonEmptyLine(&file, &line);	//le componenete

				getFirstNonEmptyLine(&file, &line);	//le cabecalho nr vertices
				getFirstNonEmptyLine(&file, &line);	//le nr vertices
				nr_vertices = atoi(line.c_str());
				getFirstNonEmptyLine(&file, &line);	//elimina cabecalho vertices

				//cria novo componente
				Component current_component;
				/*
				current_component = new double*[nr_vertices];
				for(int i = 0; i < nr_vertices; ++i)
					current_component[i] = new double[3];
				*/

				//adiciona vertices ao componente
				for (int i_vertices = 0; i_vertices < nr_vertices; i_vertices++) //le vertices
				{
					GLdouble* current_vertex = new GLdouble[3];
					string coord;
					file >> coord;
					current_vertex[0] = atoi(coord.c_str());
					file >> coord;
					current_vertex[1] = atoi(coord.c_str());
					current_vertex[2] = 0;
					current_component.push_back(current_vertex);
				}
				//adiciona componente a peca
				current_piece.addComponent(current_component);
			}
			problem.addPiece(current_piece, quantity); //adiciona peça ao problema
			current_piece.print();
		}
		file.close();
		if (quantity == 0)	problem.popPiece();
	}
	else cout << "Unable to open file " << filename << endl;
	return problem;
}

void Layout::getFirstNonEmptyLine(istream* s, string* line)
{
	do {
		getline(*s, *line);
	} while (line->size() == 0 && s->good());
}
