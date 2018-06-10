#pragma once
#include <vector>
#include <iostream>
#include <ctime>
using namespace std;
#define MAPLENGTH 50                     //�Թ���ģ
#define wallPerLine MAPLENGTH/20*19          //ÿ��ǽ������


class Point
{
public:
	int x;
	int y;
	Point(int _x, int _y) :x(_x), y(_y) {};
	void show();
};

class Map
{
private:
	vector<Point> route;
	bool iswall[MAPLENGTH+2][MAPLENGTH+2] = {false}; //�Ƿ�Ϊǽ
	void randToRoute();				   //�������һ������·��
	void randToMap();                  //������ɵ�ͼ
public:
	Map();
	void showMap();
	void showRoute();
	bool isWall(int x,int y);           //��ͼ�ӿ�
	void SuitTo3DGame();//Ϊ����Ӧ3D�����ı�
	~Map();
};