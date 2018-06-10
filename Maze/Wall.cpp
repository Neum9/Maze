#include "Wall.h"

Wall::Wall()
{
}

Wall::~Wall()
{
}

void Wall::WallInit(GLuint pd)
{
	WallCenter = vec2(0.0, -10.0);

	CubePos[0] = vec3(-5, -7.5, -5);
	CubePos[1] = vec3(5, -7.5, -5);
	CubePos[2] = vec3(5, 12.5, -5);
	CubePos[3] = vec3(-5, 12.5, -5);
	CubePos[4] = vec3(-5, -7.5, -15);
	CubePos[5] = vec3(5, -7.5, -15);
	CubePos[6] = vec3(5, 12.5, -15);
	CubePos[7] = vec3(-5, 12.5, -15);

	//初始化方块面片
	CubeFace[0] = vec3(0, 1, 2);
	CubeFace[1] = vec3(2, 3, 0);
	CubeFace[2] = vec3(4, 0, 3);
	CubeFace[3] = vec3(3, 7, 4);
	CubeFace[4] = vec3(3, 2, 6);
	CubeFace[5] = vec3(6, 7, 3);
	CubeFace[6] = vec3(1, 5, 6);
	CubeFace[7] = vec3(6, 2, 1);
	CubeFace[8] = vec3(5, 4, 7);
	CubeFace[9] = vec3(7, 6, 5);
	CubeFace[10] = vec3(4, 5, 1);
	CubeFace[11] = vec3(1, 0, 4);

	//初始化方块点法线
	vec3 Center = vec3(0.5, 0.5, -0.5);
	for (int i = 0;i < 8;i++)
	{
		CubeNormal[i] = CubePos[i] - Center;
	}
	//根据面片生成所有点
	for (int i = 0;i < 12;i++)
	{
		AllPos.push_back(CubePos[(int)CubeFace[i].x]);
		AllPos.push_back(CubePos[(int)CubeFace[i].y]);
		AllPos.push_back(CubePos[(int)CubeFace[i].z]);
	}

	ProgramID = pd;
	GLuint vPosition = glGetAttribLocation(ProgramID, "vPosition");
	GLuint vNormal = glGetAttribLocation(ProgramID, "vNormal");
	ModelMatrixID = glGetUniformLocation(ProgramID, "modelMatrix");

	ModelMatrix = mat4(1.0);

	//开启顶点属性
	glGenVertexArrays(1, &Vao);

	//顶点
	glBindVertexArray(Vao);
	glGenBuffers(2, &Vbo[0]);

	//位置
	glBindBuffer(GL_ARRAY_BUFFER, Vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, AllPos.size() * sizeof(vec3), &AllPos[0], GL_STATIC_DRAW);
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);

	//法线
	glBindBuffer(GL_ARRAY_BUFFER, Vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, AllPos.size() * sizeof(vec3), NULL, GL_STATIC_DRAW);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vNormal);
}

void Wall::DrawWall(mat4 mm)
{
	ModelMatrix = mm;
	//设置法线
	AllNormal.clear();
	vec4 C = vec3(0.0, 7.5, -10);
	for (int i = 0;i < AllPos.size();i++)
	{
		AllNormal.push_back(vec3((ModelMatrix*(vec4(AllPos.at(i) - C, 1.0))).x, (ModelMatrix*(vec4(AllPos.at(i) - C, 1.0))).y, (ModelMatrix*(vec4(AllPos.at(i) - C, 1.0))).z));
	}

	glBindBuffer(GL_ARRAY_BUFFER, Vbo[1]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, AllNormal.size() * sizeof(vec3), &AllNormal[0]);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(Vao);
	glDrawArrays(GL_TRIANGLES, 0, AllPos.size());
}

void Wall::SetWallCenter(vec2 c)
{
	WallCenter = c;
}

vec2 Wall::GetWallCenter()
{
	return WallCenter;
}

vec3 Wall::GetCubePos(int i)
{
	return CubePos[i];
}

vec3 Wall::GetCubeFace(int i)
{
	return CubeFace[i];
}

mat4 Wall::GetModelMatrix()
{
	return ModelMatrix;
}
