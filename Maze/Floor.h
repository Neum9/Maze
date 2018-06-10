#pragma once
#include "include/Angel.h"
#include <vector>
using namespace std;

#pragma comment(lib,"glew32.lib")

class Floor
{
private:
	vec3 SquarePos[4];
	vec3 SquareFace[2];
	vec3 SquareNormal[4];

	vector<vec3> AllPos;
	vector<vec3> AllNormal;
public:
	Floor();
	~Floor();
	void FloorInit();
	vec3 GetSquarePos(int i);
	vec3 GetSquareFace(int i);
};