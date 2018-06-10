#include "Robot.h"

Robot::Robot()
{

}

void Robot::RobotInit(GLuint pd)
{
	for (int i = 0;i < NumJointAngles;i++)
	{
		theta[i] = 0;
	}

	//初始化方块位置
	CubePos[0] = vec3(0.0, 0.0, 0.0);
	CubePos[1] = vec3(1.0, 0.0, 0.0);
	CubePos[2] = vec3(1.0, 1.0, 0.0);
	CubePos[3] = vec3(0.0, 1.0, 0.0);
	CubePos[4] = vec3(0.0, 0.0, -1.0);
	CubePos[5] = vec3(1.0, 0.0, -1.0);
	CubePos[6] = vec3(1.0, 1.0, -1.0);
	CubePos[7] = vec3(0.0, 1.0, -1);

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

	//开启顶点属性
	glGenVertexArrays(1, &Vao);

	//顶点
	glBindVertexArray(Vao);
	glGenBuffers(2,&Vbo[0]);

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

Robot::~Robot()
{
}

void Robot::torso(mat4 ModelMatrix)
{
	mat4 instance = (Scale(TORSO_WIDTH, TORSO_HEIGHT, TORSO_WIDTH) * Translate(-0.5, -0.5, 0.5));//本节点局部变换矩阵
	PartModelMatrix[Torso] = ModelMatrix * instance;
	glUniformMatrix4fv(ModelMatrixID, 1, GL_TRUE, ModelMatrix * instance);//父节点矩阵*本节点局部变换矩阵
	AllNormal.clear();
	vec4 C = vec3(0.5, 0.5, -0.5);
	for (int i = 0;i < AllPos.size();i++)
	{
		AllNormal.push_back(vec3((ModelMatrix*instance*(vec4(AllPos.at(i) - C, 1.0))).x, (ModelMatrix*instance*(vec4(AllPos.at(i) - C, 1.0))).y, (ModelMatrix*instance*(vec4(AllPos.at(i) - C, 1.0))).z));
	}

	glBindBuffer(GL_ARRAY_BUFFER, Vbo[1]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, AllNormal.size() * sizeof(vec3), &AllNormal[0]);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(Vao);
	glDrawArrays(GL_TRIANGLES, 0, AllPos.size());
}

void Robot::head(mat4 ModelMatrix)
{
	mat4 instance = (Scale(HEAD_WIDTH, HEAD_HEIGHT, HEAD_WIDTH) * Translate(-0.5, -0.5, 0.5));//本节点局部变换矩阵
	PartModelMatrix[Head] = ModelMatrix * instance;
	glUniformMatrix4fv(ModelMatrixID, 1, GL_TRUE, ModelMatrix * instance);//父节点矩阵*本节点局部变换矩阵

	AllNormal.clear();
	vec4 C = vec3(0.5, 0.5, -0.5);
	for (int i = 0;i < AllPos.size();i++)
	{
		AllNormal.push_back(vec3((ModelMatrix*instance*(vec4(AllPos.at(i) - C, 1.0))).x, (ModelMatrix*instance*(vec4(AllPos.at(i) - C, 1.0))).y, (ModelMatrix*instance*(vec4(AllPos.at(i) - C, 1.0))).z));
	}

	glBindBuffer(GL_ARRAY_BUFFER, Vbo[1]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, AllNormal.size() * sizeof(vec3), &AllNormal[0]);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(Vao);
	glDrawArrays(GL_TRIANGLES, 0, AllPos.size());
}

void Robot::left_arm(mat4 ModelMatrix)
{
	mat4 instance = (Translate(0.0, -0.5*LEFT_ARM_HEIGHT, 0.0) * Scale(LEFT_ARM_WIDTH, LEFT_ARM_HEIGHT, LEFT_ARM_WIDTH) * Translate(-0.5, -0.5, 0.5));//本节点局部变换矩阵
	ModelMatrix = Translate(0.0, 0.5*LEFT_ARM_HEIGHT, 0.0) * ModelMatrix;
	PartModelMatrix[LeftArm] = ModelMatrix * instance;
	glUniformMatrix4fv(ModelMatrixID, 1, GL_TRUE, ModelMatrix * instance);//父节点矩阵*本节点局部变换矩阵

	AllNormal.clear();
	vec4 C = vec3(0.5, 0.5, -0.5);
	for (int i = 0;i < AllPos.size();i++)
	{
		AllNormal.push_back(vec3((ModelMatrix*instance*(vec4(AllPos.at(i) - C, 1.0))).x, (ModelMatrix*instance*(vec4(AllPos.at(i) - C, 1.0))).y, (ModelMatrix*instance*(vec4(AllPos.at(i) - C, 1.0))).z));
	}

	glBindBuffer(GL_ARRAY_BUFFER, Vbo[1]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, AllNormal.size() * sizeof(vec3), &AllNormal[0]);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(Vao);
	glDrawArrays(GL_TRIANGLES, 0, AllPos.size());
}

void Robot::right_arm(mat4 ModelMatrix)
{
	mat4 instance = (Translate(0.0, -0.5*RIGHT_ARM_HEIGHT, 0.0) * Scale(RIGHT_ARM_WIDTH, RIGHT_ARM_HEIGHT, RIGHT_ARM_WIDTH) * Translate(-0.5, -0.5, 0.5));//本节点局部变换矩阵
	ModelMatrix = Translate(0.0, 0.5*RIGHT_ARM_HEIGHT, 0.0) * ModelMatrix;
	PartModelMatrix[RightArm] = ModelMatrix * instance;
	glUniformMatrix4fv(ModelMatrixID, 1, GL_TRUE, ModelMatrix * instance);//父节点矩阵*本节点局部变换矩阵

	AllNormal.clear();
	vec4 C = vec3(0.5, 0.5, -0.5);
	for (int i = 0;i < AllPos.size();i++)
	{
		AllNormal.push_back(vec3((ModelMatrix*instance*(vec4(AllPos.at(i) - C, 1.0))).x, (ModelMatrix*instance*(vec4(AllPos.at(i) - C, 1.0))).y, (ModelMatrix*instance*(vec4(AllPos.at(i) - C, 1.0))).z));
	}

	glBindBuffer(GL_ARRAY_BUFFER, Vbo[1]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, AllNormal.size() * sizeof(vec3), &AllNormal[0]);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(Vao);
	glDrawArrays(GL_TRIANGLES, 0, AllPos.size());
}

void Robot::left_leg(mat4 ModelMatrix)
{
	mat4 instance = (Translate(0.0, -0.5*LEFT_LEG_HEIGHT, 0.0) * Scale(LEFT_LEG_WIDTH, LEFT_LEG_HEIGHT, LEFT_LEG_WIDTH) * Translate(-0.5, -0.5, 0.5));//本节点局部变换矩阵
	ModelMatrix = Translate(0.0, 0.5*LEFT_LEG_HEIGHT, 0.0) * ModelMatrix;
	PartModelMatrix[LeftLeg] = ModelMatrix * instance;
	glUniformMatrix4fv(ModelMatrixID, 1, GL_TRUE, ModelMatrix * instance);//父节点矩阵*本节点局部变换矩阵

	AllNormal.clear();
	vec4 C = vec3(0.5, 0.5, -0.5);
	for (int i = 0;i < AllPos.size();i++)
	{
		AllNormal.push_back(vec3((ModelMatrix*instance*(vec4(AllPos.at(i) - C, 1.0))).x, (ModelMatrix*instance*(vec4(AllPos.at(i) - C, 1.0))).y, (ModelMatrix*instance*(vec4(AllPos.at(i) - C, 1.0))).z));
	}

	glBindBuffer(GL_ARRAY_BUFFER, Vbo[1]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, AllNormal.size() * sizeof(vec3), &AllNormal[0]);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(Vao);
	glDrawArrays(GL_TRIANGLES, 0, AllPos.size());
}

void Robot::right_leg(mat4 ModelMatrix)
{
	mat4 instance = (Translate(0.0, -0.5*RIGHT_LEG_HEIGHT, 0.0) * Scale(RIGHT_LEG_WIDTH, RIGHT_LEG_HEIGHT, RIGHT_LEG_WIDTH) * Translate(-0.5, -0.5, 0.5));//本节点局部变换矩阵
	ModelMatrix = Translate(0.0, 0.5*RIGHT_LEG_HEIGHT, 0.0) * ModelMatrix;
	PartModelMatrix[RightLeg] = ModelMatrix * instance;
	glUniformMatrix4fv(ModelMatrixID, 1, GL_TRUE, ModelMatrix * instance);//父节点矩阵*本节点局部变换矩阵

	AllNormal.clear();
	vec4 C = vec3(0.5, 0.5, -0.5);
	for (int i = 0;i < AllPos.size();i++)
	{
		AllNormal.push_back(vec3((ModelMatrix*instance*(vec4(AllPos.at(i) - C, 1.0))).x, (ModelMatrix*instance*(vec4(AllPos.at(i) - C, 1.0))).y, (ModelMatrix*instance*(vec4(AllPos.at(i) - C, 1.0))).z));
	}

	glBindBuffer(GL_ARRAY_BUFFER, Vbo[1]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, AllNormal.size() * sizeof(vec3), &AllNormal[0]);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(Vao);
	glDrawArrays(GL_TRIANGLES, 0, AllPos.size());
}

void Robot::ModelPush(mat4 m)
{
	MatStore.push(m);
}

mat4 Robot::ModelPop()
{
	return MatStore.pop();
}

GLfloat Robot::GetTheta(int part)
{
	return theta[part];
}

void Robot::SetTheta(int part, GLfloat addtheta)
{
	theta[part] += addtheta;
}

void Robot::SetRobotCenter(vec2 c)
{
	RobotCenter = c;
}

vec2 Robot::GetRobotCenter()
{
	return RobotCenter;
}

vec3 Robot::GetCubePos(int i)
{
	return CubePos[i];
}

vec3 Robot::GetCubeFace(int i)
{
	return CubeFace[i];
}

mat4 Robot::GetPartModelMatrix(int Part)
{
	return PartModelMatrix[Part];
}

