#include "3DGame.h"

TriDimGame::TriDimGame(int _argc, char ** _argv)
{
	argc = _argc;
	argv = _argv;
}

//空闲回调函数
void Idle_t()
{
	glutPostRedisplay();
}

void TriDimGame::GameStart()
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(WINDOWLEN, WINDOWLEN);
	glutInitWindowPosition(600, 10);
	glutCreateWindow("2015150163_陈佳聪_期末大作业");
	glewInit();
	initial();
	glutDisplayFunc(display);
	glutIdleFunc(Idle_t);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutMouseWheelFunc(MouseWheel);
	glutMainLoop();
	DelTexture();
}

TriDimGame::TriDimGame()
{
}


mat4 TriDimGame::GetShadow(vec3 l)
{
	//设置阴影矩阵
	float lx = l[0];
	float ly = l[1];
	float lz = l[2];
	float k = -(0.5*TORSO_HEIGHT + LEFT_LEG_HEIGHT) + 0.01;

	// TODO 计算阴影投影矩阵，绘制投影之后的图形，以k为投影面
	mat4 SM = mat4(-(k - ly), 0.0, 0.0, 0.0,
		-lx, -k, -lz, -1.0,
		0.0, 0.0, -(k - ly), 0.0,
		lx*k, ly*k, lz*k, ly);
	return SM;
}

void TriDimGame::AddTexture(vec3 * Pos, vec3 * Index, string Pic,vec3 c)
{
	My_Mesh* my_mesh = new My_Mesh;
	my_mesh->setMat(MrRobot.GetPartModelMatrix(Head), GetView(), ProjMatrix);
	my_mesh->Generate(Pos, Index, c);
	my_mesh->set_texture_file(Pic);
	mp_->add_mesh(my_mesh);
	my_meshs.push_back(my_mesh);
}

void TriDimGame::DelTexture()
{
	for (unsigned int i = 0; i < my_meshs.size(); i++)
	{
		delete my_meshs[i];
	}
	delete mp_;
}

void TriDimGame::initial()
{
	
	//载入着色器
	ProgramID = InitShader("3DGame_vshader.glsl", "3DGame_fshader.glsl");
	ModelMatrixID = glGetUniformLocation(ProgramID, "modelMatrix");
	ViewMatrixID = glGetUniformLocation(ProgramID, "viewMatrix");
	ProjMatrixID = glGetUniformLocation(ProgramID, "projMatrix");
	ShadowMatrixID = glGetUniformLocation(ProgramID, "shadowMatrix");
	isShadowID = glGetUniformLocation(ProgramID, "isShadow");
	LightPosID = glGetUniformLocation(ProgramID, "lightPos");
	
	//光照点初始化
	//AllLight.push_back(vec3(-20, 100, 20));
	AllLight.push_back(vec3(-150, 200, -150));
	//纹理实体分配
	mp_ = new Mesh_Painter;

	//观察参数初始化
	r = 50;
	Theta = M_PI  / 3;
	delta = 0;

	//视点初始化
	at = vec4(0, 0, 0, 1);
	eye = vec4(r*sin(Theta)*sin(delta), r*cos(Theta),
		r*sin(Theta)*cos(delta), 1);
	up = vec4(-cos(Theta)*sin(delta), sin(Theta), -cos(Theta)*cos(delta), 0);
	//机器人方向初始化
	DirectionW = DirectionS = 1;

	//机器人实体初始化
	MrRobot.SetRobotCenter(vec2(0.0, 0.0));
	MrRobot.RobotInit(ProgramID);
	//墙壁实体初始化
	MrWall.SetWallCenter(vec2(0.0, -10));
	MrWall.WallInit(ProgramID);
	//地板实体初始化
	MrFloor.FloorInit();


	//旋转部位初始化
	Angle = Torso;
	//移动矩阵初始化
	MoveMatrix = mat4(1.0);
	
	//设置投影矩阵
	ProjMatrix = Perspective(60, 1.0, 0.1, 100);
	glUniformMatrix4fv(ProjMatrixID, 1, GL_TRUE, ProjMatrix);
	
	//机器人纹理初始化
	InitRobotTexture();

	//墙壁纹理初始化
	InitWallTexture();

	//地板纹理初始化
	InitFloorTexture();

	//录入所有墙壁中心，用于检测碰撞
	vec4 Bri;
	for (int i = 0;i < MAPLENGTH + 2;i++)
	{
		for (int j = 0;j < MAPLENGTH + 2;j++)
		{
			if (MrMap.isWall(i, j))
			{
				Bri = Translate(-10 * j, 0, -10 * i) * vec3(MrWall.GetWallCenter().x, 0, MrWall.GetWallCenter().y);
				AllWallCenter.push_back(vec2(Bri.x, Bri.z));
			}
		}
	}
	//指示实体分配
	MrDenote = new Denote[2];
	MrDenote[0].DenoteInit(ProgramID, vec3(-10, -(0.5*TORSO_HEIGHT + LEFT_LEG_HEIGHT), 0));
	MrDenote[1].DenoteInit(ProgramID, vec3(-10*(MAPLENGTH), -(0.5*TORSO_HEIGHT + LEFT_LEG_HEIGHT), -10*(MAPLENGTH+1)));
	
	//纹理着色器
	mp_->init_shaders("v_texture.glsl", "f_texture.glsl");
	mp_->update_vertex_buffer();
	mp_->update_texture();

	//给所有纹理赋光照
	mp_->setLightPos(AllLight.at(0));

	isStart = false;
	isEnd = false;
	
	glBindVertexArray(0);
	
	//设置背景色
	glClearColor(187.0/255, 174.0/255, 1.0, 1.0);
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_LIGHTING);
}

void TriDimGame::display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glUniform3fv(LightPosID, 1, &(AllLight.at(0)[0]));

	//*************************机器人start**********************

	glUseProgram(ProgramID);
	//将视点和机器人所在赋初值
	at = vec4(0.0, 0.0, 0.0, 1.0);
	eye = vec4(r*sin(Theta)*sin(delta), r*cos(Theta),
		r*sin(Theta)*cos(delta), 1);
	up = vec4(-cos(Theta)*sin(delta), sin(Theta), -cos(Theta)*cos(delta), 0);
	//将模型矩阵换成机器人移动矩阵
	ModelMatrix = MoveMatrix;

	//确定机器人位置
	at.x = ModelMatrix[0][3];
	at.y = ModelMatrix[1][3];
	at.z = ModelMatrix[2][3];

	//更新机器人中心
	MrRobot.SetRobotCenter(vec2(at.x, at.z));

	//确定视点位置
	eye.x += ModelMatrix[0][3];
	eye.y += ModelMatrix[1][3];
	eye.z += ModelMatrix[2][3];

	//更新视点矩阵
	glUniformMatrix4fv(ViewMatrixID, 1, GL_TRUE, GetView());

	//更新阴影矩阵,需要多个阴影时使用
	//画阴影，随时更新模型矩阵供纹理使用
	glUniform1d(isShadowID, true);
	for (int i = 0;i < AllLight.size();i++)
	{
		glUniformMatrix4fv(ShadowMatrixID, 1, GL_TRUE, GetShadow(AllLight.at(i)));

		ModelMatrix = ModelMatrix * RotateY(MrRobot.GetTheta(Torso));//躯干变换矩阵
		MrRobot.torso(ModelMatrix);//躯干绘制

		MrRobot.ModelPush(ModelMatrix);
		ModelMatrix *= (Translate(0.0, 0.5*(TORSO_HEIGHT + HEAD_HEIGHT), 0.0) * RotateY(MrRobot.GetTheta(Head)));
		MrRobot.head(ModelMatrix);
		ModelMatrix = MrRobot.ModelPop();

		MrRobot.ModelPush(ModelMatrix);
		ModelMatrix *= (Translate(-0.5*(TORSO_WIDTH + LEFT_ARM_WIDTH), 0.0, 0.0) * RotateX(MrRobot.GetTheta(LeftArm)));
		MrRobot.left_arm(ModelMatrix);
		ModelMatrix = MrRobot.ModelPop();

		MrRobot.ModelPush(ModelMatrix);
		ModelMatrix *= (Translate(0.5*(TORSO_WIDTH + RIGHT_ARM_WIDTH), 0.0, 0.0) * RotateX(MrRobot.GetTheta(RightArm)));
		MrRobot.right_arm(ModelMatrix);
		ModelMatrix = MrRobot.ModelPop();

		MrRobot.ModelPush(ModelMatrix);
		ModelMatrix *= (Translate(-0.5*(TORSO_WIDTH - LEFT_LEG_WIDTH), -0.5*(TORSO_HEIGHT + LEFT_LEG_HEIGHT), 0.0) * RotateX(MrRobot.GetTheta(LeftLeg)));
		MrRobot.left_leg(ModelMatrix);
		ModelMatrix = MrRobot.ModelPop();

		MrRobot.ModelPush(ModelMatrix);
		ModelMatrix *= (Translate(0.5*(TORSO_WIDTH - RIGHT_LEG_WIDTH), -0.5*(TORSO_HEIGHT + RIGHT_LEG_HEIGHT), 0.0) * RotateX(MrRobot.GetTheta(RightLeg)));
		MrRobot.right_leg(ModelMatrix);
		ModelMatrix = MrRobot.ModelPop();

		//恢复移动矩阵
		ModelMatrix = MoveMatrix;
	}
	//画机器人阴影完毕

	//画机器人纹理
	mp_->setIsRobot(true);
	DrawRobotTexture();

	//*************************机器人end************************

	//*************************墙壁start************************

	glUseProgram(ProgramID);
	glUniformMatrix4fv(ModelMatrixID, 1, true, mat4(1.0));

	//画墙壁阴影和纹理
	mp_->setIsRobot(false);
	glUniform1d(isShadowID, true);

	for (int j = 0;j < MAPLENGTH + 2;j++)
	{
		for (int k = 0;k < MAPLENGTH + 2;k++)
		{
			if (MrMap.isWall(j, k))
			{
				for (int i = 0;i < AllLight.size();i++)
				{
					glUseProgram(ProgramID);
					glUniformMatrix4fv(ShadowMatrixID, 1, GL_TRUE, GetShadow(AllLight.at(i)));
					glUniformMatrix4fv(ModelMatrixID, 1, GL_TRUE, Translate(-10 * k, 0, -10 * j));
					MrWall.DrawWall(Translate(-10 * k, 0, -10 * j));
				}
				DrawWallTexture();
			}
		}
	}

	//*************************墙壁end**************************

	//*************************地板start************************

	glUseProgram(ProgramID);

	//画地板纹理
	mp_->setIsRobot(false);
	DrawFloorTexture();

	//*************************地板end**************************

	//*************************指示start************************

	glUseProgram(ProgramID);
	for (int i = 0;i < 2;i++)
	{
		glUniformMatrix4fv(ModelMatrixID, 1, true, MrDenote[i].GetModelMatrix());
		//画指示
		glUniform1d(isShadowID, false);
		MrDenote[i].DenoteDraw();
		//画指示影子
		glUniform1d(isShadowID, true);
		MrDenote[i].DenoteDraw();
	}
	//*************************指示end**************************

	glutSwapBuffers();
}

void TriDimGame::DrawRobotTexture()
{
	int i;
	//头
	for (i = 0;i < 5;i++)
	{
		mp_->draw_meshes(MrRobot.GetPartModelMatrix(Head), GetView(), i);
	}
	//躯干
	for (;i < 11;i++)
	{
		mp_->draw_meshes(MrRobot.GetPartModelMatrix(Torso), GetView(), i);
	}
	//左臂
	for (;i < 17;i++)
	{
		mp_->draw_meshes(MrRobot.GetPartModelMatrix(LeftArm), GetView(), i);
	}
	//右臂
	for (;i < 23;i++)
	{
		mp_->draw_meshes(MrRobot.GetPartModelMatrix(RightArm), GetView(), i);
	}
	//左脚
	for (;i < 29;i++)
	{
		mp_->draw_meshes(MrRobot.GetPartModelMatrix(LeftLeg), GetView(), i);
	}
	//右脚
	for (;i < 35;i++)
	{
		mp_->draw_meshes(MrRobot.GetPartModelMatrix(RightLeg), GetView(), i);
	}
}

void TriDimGame::DrawWallTexture()
{
	for (int i = 35;i < 40;i++)
	{
		mp_->draw_meshes(MrWall.GetModelMatrix(), GetView(), i);
	}
}

void TriDimGame::keyboard(unsigned char key, int x, int y)
{
	if (isEnd)
	{
		return;
	}
	switch (key) {
	case 'w':
		//先检测是否能够移动
		if (CanMove(1))
		{
			if (MrRobot.GetTheta(RightArm) == 40.0)
			{
				DirectionW = -1;
			}
			else if (MrRobot.GetTheta(RightArm) == -40.0)
			{
				DirectionW = 1;
			}
			MrRobot.SetTheta(RightArm, 5.0*DirectionW);
			MrRobot.SetTheta(LeftLeg, 5.0*DirectionW);
			MrRobot.SetTheta(LeftArm, -5.0*DirectionW);
			MrRobot.SetTheta(RightLeg, -5.0*DirectionW);
			MoveMatrix = Translate(-1 * sin(MrRobot.GetTheta(Torso) * M_PI / 180),
				0.0, -1 * cos(MrRobot.GetTheta(Torso) * M_PI / 180)) * MoveMatrix;
			//检测是否进入迷宫和结束
			if (!isStart)
			{
				CheckStart();
			}
			CheckEnd();
		}
		break;
	case 's':
		if (CanMove(-1))
		{
			if (MrRobot.GetTheta(RightArm) == 40.0)
			{
				DirectionS = 1;
			}
			else if (MrRobot.GetTheta(RightArm) == -40.0)
			{
				DirectionS = -1;
			}
			MrRobot.SetTheta(RightArm, -5.0*DirectionS);
			MrRobot.SetTheta(LeftLeg, -5.0*DirectionS);
			MrRobot.SetTheta(LeftArm, 5.0*DirectionS);
			MrRobot.SetTheta(RightLeg, 5.0*DirectionS);
			MoveMatrix = Translate(sin(MrRobot.GetTheta(Torso) * M_PI / 180),
				0.0, cos(MrRobot.GetTheta(Torso) * M_PI / 180)) * MoveMatrix;
			//检测是否进入迷宫和结束
			if (!isStart)
			{
				CheckStart();
			}
			CheckEnd();
		}
		break;
	case 'a':
		MrRobot.SetTheta(Torso, 3.0);
		delta += 3.0 * M_PI / 180;
		break;
	case 'd':
		MrRobot.SetTheta(Torso, -3.0);
		delta -= 3.0 * M_PI / 180;
		break;
	case 033: // Escape Key
	case 'q': case 'Q':
		exit(EXIT_SUCCESS);
		break;
	}
	glutPostRedisplay();
}

void TriDimGame::special(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_DOWN:
		Theta += M_PI / 180;
		break;
	case GLUT_KEY_UP:
		Theta -= M_PI / 180;
		break;
	case GLUT_KEY_LEFT:
		delta += M_PI / 180;
		break;
	case GLUT_KEY_RIGHT:
		delta -= M_PI / 180;
		break;
	}
}

mat4 TriDimGame::GetView()
{
	//视点矩阵
	return LookAt(eye, at, up);
}

void TriDimGame::MouseWheel(int button, int dir, int x, int y)
{
	if (dir > 0)
	{
		if (r < 5)
		{
			return;
		}
		r -= 1;
	}
	if (dir < 0)
	{
		r += 1;
	}

}

void TriDimGame::InitRobotTexture()
{
	vec3 BriPos[4], BriIndex[2];
	BriIndex[0].x = 0;
	BriIndex[0].y = 1;
	BriIndex[0].z = 2;
	BriIndex[1].x = 2;
	BriIndex[1].y = 3;
	BriIndex[1].z = 0;

	//脸
	BriPos[0] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(8).x);
	BriPos[1] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(8).y);
	BriPos[2] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(8).z);
	BriPos[3] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(9).y);
	AddTexture(BriPos, BriIndex, "Texture/face.png",vec3(MrRobot.GetRobotCenter().x,0.5,MrRobot.GetRobotCenter().y));
	//头发
	BriPos[0] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(0).x);
	BriPos[1] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(0).y);
	BriPos[2] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(0).z);
	BriPos[3] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(1).y);
	AddTexture(BriPos, BriIndex, "Texture/hair.png", vec3(MrRobot.GetRobotCenter().x,0.5,MrRobot.GetRobotCenter().y));
	BriPos[0] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(2).x);
	BriPos[1] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(2).y);
	BriPos[2] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(2).z);
	BriPos[3] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(3).y);
	AddTexture(BriPos, BriIndex, "Texture/hair.png", vec3(MrRobot.GetRobotCenter().x,0.5,MrRobot.GetRobotCenter().y));
	BriPos[0] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(4).x);
	BriPos[1] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(4).y);
	BriPos[2] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(4).z);
	BriPos[3] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(5).y);
	AddTexture(BriPos, BriIndex, "Texture/hair.png", vec3(MrRobot.GetRobotCenter().x,0.5,MrRobot.GetRobotCenter().y));
	BriPos[0] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(6).x);
	BriPos[1] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(6).y);
	BriPos[2] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(6).z);
	BriPos[3] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(7).y);
	AddTexture(BriPos, BriIndex, "Texture/hair.png", vec3(MrRobot.GetRobotCenter().x,0.5,MrRobot.GetRobotCenter().y));
	//身体前面
	BriPos[0] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(8).x);
	BriPos[1] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(8).y);
	BriPos[2] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(8).z);
	BriPos[3] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(9).y);
	AddTexture(BriPos, BriIndex, "Texture/beforebody.png", vec3(MrRobot.GetRobotCenter().x,0.5,MrRobot.GetRobotCenter().y));
	//身体其他
	BriPos[0] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(0).x);
	BriPos[1] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(0).y);
	BriPos[2] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(0).z);
	BriPos[3] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(1).y);
	AddTexture(BriPos, BriIndex, "Texture/body.png", vec3(MrRobot.GetRobotCenter().x,0.5,MrRobot.GetRobotCenter().y));
	BriPos[0] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(2).x);
	BriPos[1] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(2).y);
	BriPos[2] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(2).z);
	BriPos[3] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(3).y);
	AddTexture(BriPos, BriIndex, "Texture/body.png", vec3(MrRobot.GetRobotCenter().x,0.5,MrRobot.GetRobotCenter().y));
	BriPos[0] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(4).x);
	BriPos[1] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(4).y);
	BriPos[2] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(4).z);
	BriPos[3] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(5).y);
	AddTexture(BriPos, BriIndex, "Texture/body.png", vec3(MrRobot.GetRobotCenter().x,0.5,MrRobot.GetRobotCenter().y));
	BriPos[0] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(6).x);
	BriPos[1] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(6).y);
	BriPos[2] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(6).z);
	BriPos[3] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(7).y);
	AddTexture(BriPos, BriIndex, "Texture/body.png", vec3(MrRobot.GetRobotCenter().x,0.5,MrRobot.GetRobotCenter().y));
	BriPos[0] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(10).x);
	BriPos[1] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(10).y);
	BriPos[2] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(10).z);
	BriPos[3] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(11).y);
	AddTexture(BriPos, BriIndex, "Texture/body.png", vec3(MrRobot.GetRobotCenter().x,0.5,MrRobot.GetRobotCenter().y));
	//左右臂
	for (int i = 0;i < 6;i++)
	{
		BriPos[0] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(2*i).x);
		BriPos[1] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(2*i).y);
		BriPos[2] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(2*i).z);
		BriPos[3] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(2 * i + 1).y);
		AddTexture(BriPos, BriIndex, "Texture/arm.png", vec3(MrRobot.GetRobotCenter().x,0.5,MrRobot.GetRobotCenter().y));
	}
	for (int i = 0;i < 6;i++)
	{
		BriPos[0] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(2 * i).x);
		BriPos[1] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(2 * i).y);
		BriPos[2] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(2 * i).z);
		BriPos[3] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(2 * i + 1).y);
		AddTexture(BriPos, BriIndex, "Texture/arm.png", vec3(MrRobot.GetRobotCenter().x,0.5,MrRobot.GetRobotCenter().y));
	}
	//左脚前面
	BriPos[0] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(8).x);
	BriPos[1] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(8).y);
	BriPos[2] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(8).z);
	BriPos[3] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(9).y);
	AddTexture(BriPos, BriIndex, "Texture/beforeleg.png", vec3(MrRobot.GetRobotCenter().x,0.5,MrRobot.GetRobotCenter().y));
	//左脚其他
	BriPos[0] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(0).x);
	BriPos[1] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(0).y);
	BriPos[2] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(0).z);
	BriPos[3] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(1).y);
	AddTexture(BriPos, BriIndex, "Texture/leg.png", vec3(MrRobot.GetRobotCenter().x,0.5,MrRobot.GetRobotCenter().y));
	BriPos[0] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(2).x);
	BriPos[1] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(2).y);
	BriPos[2] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(2).z);
	BriPos[3] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(3).y);
	AddTexture(BriPos, BriIndex, "Texture/leg.png", vec3(MrRobot.GetRobotCenter().x,0.5,MrRobot.GetRobotCenter().y));
	BriPos[0] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(4).x);
	BriPos[1] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(4).y);
	BriPos[2] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(4).z);
	BriPos[3] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(5).y);
	AddTexture(BriPos, BriIndex, "Texture/leg.png", vec3(MrRobot.GetRobotCenter().x,0.5,MrRobot.GetRobotCenter().y));
	BriPos[0] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(6).x);
	BriPos[1] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(6).y);
	BriPos[2] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(6).z);
	BriPos[3] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(7).y);
	AddTexture(BriPos, BriIndex, "Texture/leg.png", vec3(MrRobot.GetRobotCenter().x,0.5,MrRobot.GetRobotCenter().y));
	BriPos[0] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(10).x);
	BriPos[1] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(10).y);
	BriPos[2] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(10).z);
	BriPos[3] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(11).y);
	AddTexture(BriPos, BriIndex, "Texture/leg.png", vec3(MrRobot.GetRobotCenter().x,0.5,MrRobot.GetRobotCenter().y));
	//右脚前面
	BriPos[0] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(8).x);
	BriPos[1] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(8).y);
	BriPos[2] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(8).z);
	BriPos[3] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(9).y);
	AddTexture(BriPos, BriIndex, "Texture/beforeleg.png", vec3(MrRobot.GetRobotCenter().x,0.5,MrRobot.GetRobotCenter().y));
	//右脚其他
	BriPos[0] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(0).x);
	BriPos[1] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(0).y);
	BriPos[2] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(0).z);
	BriPos[3] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(1).y);
	AddTexture(BriPos, BriIndex, "Texture/leg.png", vec3(MrRobot.GetRobotCenter().x,0.5,MrRobot.GetRobotCenter().y));
	BriPos[0] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(2).x);
	BriPos[1] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(2).y);
	BriPos[2] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(2).z);
	BriPos[3] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(3).y);
	AddTexture(BriPos, BriIndex, "Texture/leg.png", vec3(MrRobot.GetRobotCenter().x,0.5,MrRobot.GetRobotCenter().y));
	BriPos[0] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(4).x);
	BriPos[1] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(4).y);
	BriPos[2] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(4).z);
	BriPos[3] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(5).y);
	AddTexture(BriPos, BriIndex, "Texture/leg.png", vec3(MrRobot.GetRobotCenter().x,0.5,MrRobot.GetRobotCenter().y));
	BriPos[0] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(6).x);
	BriPos[1] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(6).y);
	BriPos[2] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(6).z);
	BriPos[3] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(7).y);
	AddTexture(BriPos, BriIndex, "Texture/leg.png", vec3(MrRobot.GetRobotCenter().x,0.5,MrRobot.GetRobotCenter().y));
	BriPos[0] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(10).x);
	BriPos[1] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(10).y);
	BriPos[2] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(10).z);
	BriPos[3] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(11).y);
	AddTexture(BriPos, BriIndex, "Texture/leg.png", vec3(MrRobot.GetRobotCenter().x,0.5,MrRobot.GetRobotCenter().y));
}

void TriDimGame::InitWallTexture()
{
	vec3 BriPos[4], BriIndex[2];
	BriIndex[0].x = 0;
	BriIndex[0].y = 1;
	BriIndex[0].z = 2;
	BriIndex[1].x = 2;
	BriIndex[1].y = 3;
	BriIndex[1].z = 0;
	for (int i = 0;i < 5;i++)
	{
		BriPos[0] = MrWall.GetCubePos((int)MrWall.GetCubeFace(2 * i).x);
		BriPos[1] = MrWall.GetCubePos((int)MrWall.GetCubeFace(2 * i).y);
		BriPos[2] = MrWall.GetCubePos((int)MrWall.GetCubeFace(2 * i).z);
		BriPos[3] = MrWall.GetCubePos((int)MrWall.GetCubeFace(2 * i + 1).y);
		AddTexture(BriPos, BriIndex, "Texture/wall3.jpg", vec3(-250, 0, -250));
	}
}

void TriDimGame::InitFloorTexture()
{
	vec3 BriPos[4], BriIndex[2];
	BriIndex[0].x = 0;
	BriIndex[0].y = 1;
	BriIndex[0].z = 2;
	BriIndex[1].x = 2;
	BriIndex[1].y = 3;
	BriIndex[1].z = 0;

	BriPos[0] = MrFloor.GetSquarePos((int)MrFloor.GetSquareFace(0).x);
	BriPos[1] = MrFloor.GetSquarePos((int)MrFloor.GetSquareFace(0).y);
	BriPos[2] = MrFloor.GetSquarePos((int)MrFloor.GetSquareFace(0).z);
	BriPos[3] = MrFloor.GetSquarePos((int)MrFloor.GetSquareFace(1).y);
	AddTexture(BriPos, BriIndex, "Texture/floor.jpg", vec3(0, -7.5, 0));
}

void TriDimGame::DrawFloorTexture()
{
	mp_->draw_meshes(mat4(1.0), GetView(), my_meshs.size()-1);
}

bool TriDimGame::CanMove(int dir)
{
	mat4 PreModel = Translate(-dir * sin(MrRobot.GetTheta(Torso) * M_PI / 180), 0.0, -dir * cos(MrRobot.GetTheta(Torso) * M_PI / 180)) * MoveMatrix;
	double _x = PreModel[0][3], _y = PreModel[2][3];
	for (int i = 0;i < AllWallCenter.size();i++)
	{
		if (sqrt((_x - AllWallCenter.at(i).x) * (_x - AllWallCenter.at(i).x) + (_y - AllWallCenter.at(i).y) * (_y - AllWallCenter.at(i).y)) < 2 * LEFT_ARM_HEIGHT)
		{
			return false;
		}
	}
	return true;
}

void TriDimGame::CheckStart()
{
	if (sqrt((MrRobot.GetRobotCenter().x - MrDenote[0].GetPos2().x)*(MrRobot.GetRobotCenter().x - MrDenote[0].GetPos2().x) +
		(MrRobot.GetRobotCenter().y - MrDenote[0].GetPos2().y)*(MrRobot.GetRobotCenter().y - MrDenote[0].GetPos2().y)) < 6)
	{
		isStart = true;
		cout << "Game Start!" << endl;
	}
}

void TriDimGame::CheckEnd()
{
	if (sqrt((MrRobot.GetRobotCenter().x - MrDenote[1].GetPos2().x)*(MrRobot.GetRobotCenter().x - MrDenote[1].GetPos2().x) +
		(MrRobot.GetRobotCenter().y - MrDenote[1].GetPos2().y)*(MrRobot.GetRobotCenter().y - MrDenote[1].GetPos2().y)) < 6)
	{
		isEnd = true;
		cout << "You Get It!" << endl;
	}
}

