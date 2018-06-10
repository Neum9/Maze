#pragma once
#include <vector>
#include <iostream>
#include <ctime>
using namespace std;
#define MAPLENGTH 50                     //迷宫规模
#define wallPerLine MAPLENGTH/20*19          //每行墙的数量


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
	bool iswall[MAPLENGTH+2][MAPLENGTH+2] = {false}; //是否为墙
	void randToRoute();				   //随机生成一条可行路线
	void randToMap();                  //随机生成地图
public:
	Map();
	void showMap();
	void showRoute();
	bool isWall(int x,int y);           //地图接口
	void SuitTo3DGame();//为了适应3D做出改变
	~Map();
};