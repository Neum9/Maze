#pragma once
#include "include/Angel.h"
#include <vector>
using namespace std;
#pragma comment(lib,"glew32.lib")

class Wall
{
private:
	vec3 CubePos[8];
	vec3 CubeFace[12];
	vec3 CubeNormal[8];
	vec2 WallCenter;
	GLuint ProgramID;
	GLuint ModelMatrixID;
	GLuint Vao;
	GLuint Vbo[2];

	vector<vec3> AllPos;
	vector<vec3> AllNormal;

	mat4 ModelMatrix;
public:
	Wall();
	~Wall();
	void WallInit(GLuint pd);
	void DrawWall(mat4 mm);
	void SetWallCenter(vec2 c);
	vec2 GetWallCenter();
	vec3 GetCubePos(int i);
	vec3 GetCubeFace(int i);
	mat4 GetModelMatrix();
};