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

#define GREY vec3(105.0/255,105.0/255,105.0/255)//��ɫ
#define WINDOWLEN 700 //���ڳ���

static vector<vec3> AllLight;//���еĹ�Դ

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

static int DirectionW;//�����˷��� ����
static int DirectionS;

static GLuint Angle;//��ת��λ
static mat4 MoveMatrix;//�������ƶ�����

//�۲����
static double r;
static double Theta;
static double delta;

//������λ��
static vec4 at;
//�ӽ�
static vec4 eye;
//���Ϸ���
static vec4 up;

static Robot MrRobot;//������ʵ��
static Wall MrWall;//ǽ��ʵ��
static Floor MrFloor;//�ذ�ʵ��
static Map MrMap;//��ͼʵ��
static Denote *MrDenote;//ָʾʵ��ָ��

static vector<My_Mesh*> my_meshs;//����
static Mesh_Painter*			mp_;

static vector<vec2> AllWallCenter;//����ǽ������

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
	static void InitRobotTexture();//��ʼ������������
	static void DrawRobotTexture();//���ƻ���������
	static void InitWallTexture();//��ʼ��ǽ������
	static void DrawWallTexture();//����ǽ������
	static void InitFloorTexture();//��ʼ���ذ�����
	static void DrawFloorTexture();//���Ƶذ�����
	static void DelTexture();//ɾ����������
	static bool CanMove(int dir);//����Ƿ��ܹ��ƶ�
	static void CheckStart();//�����Ϸ�Ƿ�ʼ
	static void CheckEnd();//�����Ϸ�Ƿ����
public:
	TriDimGame(int _argc, char **_argv);
	void GameStart();
	TriDimGame();
};
