#pragma once
#include "Map.h"
#include "include/Angel.h"
#pragma comment(lib, "glew32.lib")

#define WINDOWLEN 600 //窗口长度
#define COE 0.8       //迷宫地图/窗口（系数）
#define BlACK vec3(0,0,0) //黑色
#define WHITE vec3(1,1,1) //白色
#define GREY vec3(105.0/255,105.0/255,105.0/255)//灰色
#define RED vec3(1,0,0)//红色
#define GREEN vec3(0,1,0)//绿色

class Game
{
private:
	int argc;
	char **argv;
	int sidelen = MAPLENGTH;          //边长
	int windowlen = WINDOWLEN;     //窗口长度
	Map themap;
	double coe = COE;              //窗口坐标最大值（标量）
	vec2 signPoint[10];//储存指示箭头
	vec3 signColor[10];//储存指示箭头颜色
	vec2 allPoint[6 * (MAPLENGTH + 2)*(MAPLENGTH + 2)];//储存地图所有的点位置
	vec3 allColor[6 * (MAPLENGTH + 2)*(MAPLENGTH + 2)];//储存地图所有的点颜色,把墙设为灰色
	void initial();                 //初始化
	static void display();                //显示窗口
	static void keyboard(unsigned char key,int x,int y);//键盘控制函数
	static void special(int key, int x, int y);//定义特殊键响应事件
public:
	Game(int _argc, char **_argv);
	void GameStart();
	~Game();
};

 