#include "Floor.h"
#include "Robot.h"

Floor::Floor()
{
}

Floor::~Floor()
{
}

void Floor::FloorInit()
{
	SquarePos[0] = vec3(-1000, -(0.5*TORSO_HEIGHT + LEFT_LEG_HEIGHT), 1000);
	SquarePos[1] = vec3(1000, -(0.5*TORSO_HEIGHT + LEFT_LEG_HEIGHT), 1000);
	SquarePos[2] = vec3(1000, -(0.5*TORSO_HEIGHT + LEFT_LEG_HEIGHT), -1000);
	SquarePos[3] = vec3(-1000, -(0.5*TORSO_HEIGHT + LEFT_LEG_HEIGHT), -1000);

	SquareFace[0] = vec3(0, 1, 2);
	SquareFace[1] = vec3(2, 3, 0);

	for (int i = 0;i < 4;i++)
	{
		SquareNormal[i] = vec3(0, 1, 0);
	}

	for (int i = 0;i < 2;i++)
	{
		AllPos.push_back(SquarePos[(int)SquareFace[i].x]);
		AllPos.push_back(SquarePos[(int)SquareFace[i].y]);
		AllPos.push_back(SquarePos[(int)SquareFace[i].z]);
	}

	for (int i = 0;i < AllPos.size();i++)
	{
		AllNormal.push_back(vec3(0.0, 1.0, 0.0));
	}

}

vec3 Floor::GetSquarePos(int i)
{
	return SquarePos[i];
}

vec3 Floor::GetSquareFace(int i)
{
	return SquareFace[i];
}

