#include "Prompt.h"

void Prompt::drawString(const char * str)
{
	static int isFirstCall = 1;
	static GLuint lists;
	if (isFirstCall) { // ����ǵ�һ�ε��ã�ִ�г�ʼ��
					   // Ϊÿһ��ASCII�ַ�����һ����ʾ�б�
		isFirstCall = 0;
		// ����MAX_CHAR����������ʾ�б���
		lists = glGenLists(MAX_CHAR);
		// ��ÿ���ַ��Ļ������װ����Ӧ����ʾ�б���
		wglUseFontBitmaps(wglGetCurrentDC(), 0, MAX_CHAR, lists);
	}
	// ����ÿ���ַ���Ӧ����ʾ�б�����ÿ���ַ�
	for (; *str != '\0'; ++str)
		glCallList(lists + *str);
}

void Prompt::selectFont(int size, int charset, const char * face)
{
	HFONT hFont = CreateFontA(size, 0, 0, 0, FW_MEDIUM, 0, 0, 0,
		charset, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, face);
	HFONT hOldFont = (HFONT)SelectObject(wglGetCurrentDC(), hFont);
	DeleteObject(hOldFont);
}

Prompt::Prompt()
{
	Words = new char;
}

Prompt::~Prompt()
{
	delete[]Words;
}

void Prompt::DrawString(const char* str,vec2 pos)
{
	/*
	delete[]Words;
	Words = new char[s.size() + 1];
	for (int i = 0;i < s.size();i++)
	{
		Words[i] = s.at(i);
	}
	Words[s.size()] = 0;
	*/
	//glUseProgram(NULL);
	selectFont(48, ANSI_CHARSET, "Comic Sans MS");	
	glColor3f(1, 0, 0);
	//glRasterPos2f(pos.x,pos.y);
	//glRasterPos2f(-0.25, 0.9);
	glRasterPos3f(0.0, 0.0, 0.0);
	drawString(str);
}
