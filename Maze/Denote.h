#pragma once
#include "include/Angel.h"
#include <vector>
#include <cmath>
using namespace std;

#pragma comment(lib,"glew32.lib")

class Denote
{
private:
	vec3 Pos;
	GLuint ProgramID;
	GLuint vPosition;
	GLuint vColor;
	GLuint vNormal;
	GLuint Vao;
	GLuint Vbo[3];
	
	vector<vec3> ConePos;//圆锥点位置
	vector<vec3> CylinderPos;//圆柱点位置
	vector<vec3> AllCol;//所有的颜色
	vector<vec3> AllNor;//所有的法线

	vec3 ConeVertex;//圆锥下顶点
	float ConeHeight;//圆锥高
	float ConeRadius;//圆锥半径

	float CylinerButtomY;//圆柱下表面y坐标
	float CylinerHeight;//圆柱高
	float CylinerRadius;//圆柱半径
	vec2 CylinerCenter;//圆柱圆心

	int count;//计数，负责矩阵浮动变换
	int dir;//浮动方向
	float extent;//幅度
	int speed;//速度（反）
	float y_pos;

	mat4 ModelMatrix;
public:
	Denote();
	void DenoteInit(GLuint pd,vec3 p);
	void GenarateCone();
	void GenarateCylinder();
	mat4 GetModelMatrix();
	~Denote();
	void DenoteDraw();
	vec2 GetPos2();
	void ChangeRed();
};
