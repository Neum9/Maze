#pragma once
#include <windows.h>
#include <string>
#include "Angel.h"
#pragma comment(lib, "glew32.lib")
using namespace std;

#define MAX_CHAR 128

class Prompt
{
private:
	char *Words;
	void drawString(const char* str);
	void selectFont(int size, int charset, const char* face);
public:
	Prompt();
	~Prompt();
	void DrawString(const char* str,vec2 pos);
};