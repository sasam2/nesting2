#pragma once
#include "Layout.h"
#include <fstream>

class LayoutLoader
{
private:
	const char *baseFolder;

	void getFirstNonEmptyLine(istream* s, string* line);
	Problem loadFile(char* filename, bool mode);
	vector<string> loadConfigurationFile(char* filename);

public:
	LayoutLoader(const char* baseFolder);
	Layout loadLayout(char* filename);
	void unloadLayout(Layout layout);

};

