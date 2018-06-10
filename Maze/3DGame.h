#pragma once
#include "Map.h"
#include "Robot.h"
#include "Wall.h"
#include "Floor.h"
#include "mesh.h"
#include "Mesh_Painter.h"
#include "include/Angel.h"
#include "Denote.h"
#include <iostream>
#include <cmath>
#include <vector>
using namespace std;

#pragma comment(lib, "glew32.lib")
#pragma comment(lib,"FreeImage.lib")

#define GREY vec3(105.0/255,105.0/255,105.0/255)//灰色
#define WINDOWLEN 700 //窗口长度

static vector<vec3> AllLight;//所有的光源

static mat4 ModelMatrix;
static mat4 ViewMatrix;
static mat4 ProjMatrix;
static mat4 ShadowMatrix;

static GLuint ProgramID;
static GLuint ModelMatrixID;
static GLuint ViewMatrixID;
static GLuint ProjMatrixID;
static GLuint ShadowMatrixID;
static GLuint isShadowID;
static GLuint LightPosID;

static int DirectionW;//机器人方向 正负
static int DirectionS;

static GLuint Angle;//旋转部位
static mat4 MoveMatrix;//机器人移动矩阵

//观察参数
static double r;
static double Theta;
static double delta;

//机器人位置
static vec4 at;
//视角
static vec4 eye;
//向上方向
static vec4 up;

static Robot MrRobot;//机器人实体
static Wall MrWall;//墙壁实体
static Floor MrFloor;//地板实体
static Map MrMap;//地图实体
static Denote *MrDenote;//指示实体指针

static vector<My_Mesh*> my_meshs;//纹理
static Mesh_Painter*			mp_;

static vector<vec2> AllWallCenter;//所有墙壁中心

static bool isStart;
static bool isEnd;

class TriDimGame
{
private:
	int argc;
	char **argv;
	static void initial();
	static void display();
	static void keyboard(unsigned char key, int x, int y);
	static void special(int key, int x, int y);
	static mat4 GetView();
	static void TriDimGame::MouseWheel(int button, int dir, int x, int y);
	static mat4 GetShadow(vec3 l);
	static void AddTexture(vec3 *Pos, vec3 *Index, string Pic,vec3 c);
	static void InitRobotTexture();//初始化机器人纹理
	static void DrawRobotTexture();//绘制机器人纹理
	static void InitWallTexture();//初始化墙壁纹理
	static void DrawWallTexture();//绘制墙壁纹理
	static void InitFloorTexture();//初始化地板纹理
	static void DrawFloorTexture();//绘制地板纹理
	static void DelTexture();//删除所有纹理
	static bool CanMove(int dir);//检测是否能够移动
	static void CheckStart();//检测游戏是否开始
	static void CheckEnd();//检测游戏是否结束
public:
	TriDimGame(int _argc, char **_argv);
	void GameStart();
	TriDimGame();
};
