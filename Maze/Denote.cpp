#include "Denote.h"

Denote::Denote()
{
}

void Denote::DenoteInit(GLuint pd,vec3 p)
{
	Pos = p;
	ConeVertex = vec3(Pos.x, Pos.y + 1,Pos.z);
	ConeHeight = 4.0;
	ConeRadius = 4.0;

	CylinerButtomY = Pos.y + 1 + ConeHeight;
	CylinerHeight = 4.0;
	CylinerRadius = 2.5;
	CylinerCenter = vec2(ConeVertex.x,ConeVertex.z);

	count = 0;
	dir = 1;
	extent = 10;
	speed = 1;
	y_pos = 0;

	GenarateCone();
	GenarateCylinder();
	//生成所有的法线，中心为
	vec3 C(ConeVertex.x, CylinerButtomY, ConeVertex.z);
	for (int i = 0;i < ConePos.size();i++)
	{
		AllNor.push_back(ConePos.at(i) - C);
	}
	for (int j = 0;j < CylinderPos.size();j++)
	{
		AllNor.push_back(CylinderPos.at(j) - C);
	}

	ProgramID = pd;
	vPosition = glGetAttribLocation(ProgramID, "vPosition");
	vColor = glGetAttribLocation(ProgramID, "vColor");
	vNormal = glGetAttribLocation(ProgramID, "vNormal");

	glGenVertexArrays(1, &Vao);

	glBindVertexArray(Vao);
	glGenBuffers(3, &Vbo[0]);

	//位置
	glBindBuffer(GL_ARRAY_BUFFER, Vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, (ConePos.size() + CylinderPos.size()) * sizeof(vec3), NULL, GL_STATIC_DRAW);
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);

	glBindBuffer(GL_ARRAY_BUFFER, Vbo[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, ConePos.size() * sizeof(vec3), &ConePos[0]);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, Vbo[0]);
	glBufferSubData(GL_ARRAY_BUFFER, ConePos.size() * sizeof(vec3), CylinderPos.size() * sizeof(vec3), &CylinderPos[0]);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//颜色
	glBindBuffer(GL_ARRAY_BUFFER, Vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, AllCol.size() * sizeof(vec3), &AllCol[0], GL_STATIC_DRAW);
	glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColor);
	
	//法线
	glBindBuffer(GL_ARRAY_BUFFER, Vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, AllNor.size() * sizeof(vec3), &AllNor[0], GL_STATIC_DRAW);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vNormal);
}

void Denote::GenarateCone()
{
	vec3 p1(0,ConeVertex.y+ConeHeight,0), p2(0, ConeVertex.y + ConeHeight, 0);
	for (int i = 0;i < 360;i++)
	{
		p1.x = ConeVertex.x + ConeRadius * cos(i * M_PI / 180.0);
		p1.z = ConeVertex.z + ConeRadius * sin(i * M_PI / 180.0);
		p2.x = ConeVertex.x + ConeRadius * cos((i+1) * M_PI / 180.0);
		p2.z = ConeVertex.z + ConeRadius * sin((i+1) * M_PI / 180.0);
		ConePos.push_back(ConeVertex);
		ConePos.push_back(p1);
		ConePos.push_back(p2);
		AllCol.push_back(vec3(0.0, 1.0, 0.0));
		AllCol.push_back(vec3(0.0, 1.0, 0.0));
		AllCol.push_back(vec3(0.0, 1.0, 0.0));
	}
}

void Denote::GenarateCylinder()
{
	vec3 p1(0, CylinerButtomY, 0), p2(0, CylinerButtomY, 0),
		p3(0, CylinerButtomY + CylinerHeight, 0), p4(0, CylinerButtomY + CylinerHeight, 0);
	vec3 aboveCenter(CylinerCenter.x, CylinerButtomY + CylinerHeight, CylinerCenter.y);

	for (int i = 0;i < 360;i++)
	{
		//周围
		p1.x = p4.x = CylinerCenter.x + CylinerRadius * cos(i * M_PI / 180.0);
		p1.z = p4.z = CylinerCenter.y + CylinerRadius * sin(i * M_PI / 180.0);
		p2.x = p3.x = CylinerCenter.x + CylinerRadius * cos((i+1) * M_PI / 180.0);
		p2.z = p3.z = CylinerCenter.y + CylinerRadius * sin((i+1) * M_PI / 180.0);

		CylinderPos.push_back(p1);
		CylinderPos.push_back(p2);
		CylinderPos.push_back(p3);

		CylinderPos.push_back(p3);
		CylinderPos.push_back(p4);
		CylinderPos.push_back(p1);

		AllCol.push_back(vec3(0.0, 1.0, 0.0));
		AllCol.push_back(vec3(0.0, 1.0, 0.0));
		AllCol.push_back(vec3(0.0, 1.0, 0.0));
		AllCol.push_back(vec3(0.0, 1.0, 0.0));
		AllCol.push_back(vec3(0.0, 1.0, 0.0));
		AllCol.push_back(vec3(0.0, 1.0, 0.0));

		//上表面
		CylinderPos.push_back(p3);
		CylinderPos.push_back(p4);
		CylinderPos.push_back(aboveCenter);
		AllCol.push_back(vec3(0.0, 1.0, 0.0));
		AllCol.push_back(vec3(0.0, 1.0, 0.0));
		AllCol.push_back(vec3(0.0, 1.0, 0.0));
	}
}

mat4 Denote::GetModelMatrix()
{
	count += dir;
	if (count % speed == 0)
	{
		if (count == speed * 20 || count == 0)
		{
			dir *= -1;
		}
		y_pos += dir / extent;
		ModelMatrix = Translate(0, y_pos, 0);
	}
	return ModelMatrix;
}

Denote::~Denote()
{
}

void Denote::DenoteDraw()
{
	glBindVertexArray(Vao);
	glDrawArrays(GL_TRIANGLES, 0, ConePos.size());
	glDrawArrays(GL_TRIANGLES, ConePos.size(), CylinderPos.size());
}

vec2 Denote::GetPos2()
{
	return vec2(Pos.x,Pos.z);
}

void Denote::ChangeRed()
{

}