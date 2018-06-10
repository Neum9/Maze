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
	
	vector<vec3> ConePos;//Բ׶��λ��
	vector<vec3> CylinderPos;//Բ����λ��
	vector<vec3> AllCol;//���е���ɫ
	vector<vec3> AllNor;//���еķ���

	vec3 ConeVertex;//Բ׶�¶���
	float ConeHeight;//Բ׶��
	float ConeRadius;//Բ׶�뾶

	float CylinerButtomY;//Բ���±���y����
	float CylinerHeight;//Բ����
	float CylinerRadius;//Բ���뾶
	vec2 CylinerCenter;//Բ��Բ��

	int count;//������������󸡶��任
	int dir;//��������
	float extent;//����
	int speed;//�ٶȣ�����
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
