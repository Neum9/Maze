#pragma once
#include "include/Angel.h"
#include "MatrixStack.h"
#include <vector>
using namespace std;
#pragma comment(lib,"glew32.lib")

#define TORSO_HEIGHT 5.0
#define TORSO_WIDTH 3.0
#define HEAD_HEIGHT 2.0
#define HEAD_WIDTH  2.0
#define LEFT_ARM_HEIGHT 4.0
#define LEFT_ARM_WIDTH 1.0
#define RIGHT_ARM_HEIGHT 4.0
#define RIGHT_ARM_WIDTH 1.0
#define LEFT_LEG_HEIGHT 5.0
#define LEFT_LEG_WIDTH 1.0
#define RIGHT_LEG_HEIGHT 5.0
#define RIGHT_LEG_WIDTH 1.0

enum {
	Torso,
	Head,
	RightArm,
	LeftArm,
	RightLeg,
	LeftLeg,
	NumJointAngles,
	Quit
};

class Robot
{
private:
	vec3 CubePos[8];//�����λ��
	vec3 CubeFace[12];//������Ƭ
	vec3 CubeNormal[8];//����㷨��

	vec2 RobotCenter;//����������

	MatrixStack MatStore;//ģ�;���洢

	GLuint ProgramID;
	GLuint ModelMatrixID;
	GLuint Vao;
	GLuint Vbo[2];//��ͷ���

	vector<vec3> AllPos;//������Ƭ�����ĵ�
	vector<vec3> AllNormal;//������Ƭ�����ķ���

	GLfloat theta[NumJointAngles];

	mat4 PartModelMatrix[6];
public:
	Robot();
	void RobotInit(GLuint pd);
	~Robot();

	void torso(mat4 mm);
	void head(mat4 mm);
	void left_arm(mat4 mm);
	void right_arm(mat4 mm);
	void left_leg(mat4 mm);
	void right_leg(mat4 mm);

	void ModelPush(mat4 m);
	mat4 ModelPop();

	GLfloat GetTheta(int part);
	void SetTheta(int part, GLfloat addtheta);

	void SetRobotCenter(vec2 c);
	vec2 GetRobotCenter();

	vec3 GetCubePos(int i);
	vec3 GetCubeFace(int i);

	mat4 GetPartModelMatrix(int Part);
};