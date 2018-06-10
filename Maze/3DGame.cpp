#include "3DGame.h"

TriDimGame::TriDimGame(int _argc, char ** _argv)
{
	argc = _argc;
	argv = _argv;
}

//���лص�����
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
	glutCreateWindow("2015150163_�¼Ѵ�_��ĩ����ҵ");
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
	//������Ӱ����
	float lx = l[0];
	float ly = l[1];
	float lz = l[2];
	float k = -(0.5*TORSO_HEIGHT + LEFT_LEG_HEIGHT) + 0.01;

	// TODO ������ӰͶӰ���󣬻���ͶӰ֮���ͼ�Σ���kΪͶӰ��
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
	
	//������ɫ��
	ProgramID = InitShader("3DGame_vshader.glsl", "3DGame_fshader.glsl");
	ModelMatrixID = glGetUniformLocation(ProgramID, "modelMatrix");
	ViewMatrixID = glGetUniformLocation(ProgramID, "viewMatrix");
	ProjMatrixID = glGetUniformLocation(ProgramID, "projMatrix");
	ShadowMatrixID = glGetUniformLocation(ProgramID, "shadowMatrix");
	isShadowID = glGetUniformLocation(ProgramID, "isShadow");
	LightPosID = glGetUniformLocation(ProgramID, "lightPos");
	
	//���յ��ʼ��
	//AllLight.push_back(vec3(-20, 100, 20));
	AllLight.push_back(vec3(-150, 200, -150));
	//����ʵ�����
	mp_ = new Mesh_Painter;

	//�۲������ʼ��
	r = 50;
	Theta = M_PI  / 3;
	delta = 0;

	//�ӵ��ʼ��
	at = vec4(0, 0, 0, 1);
	eye = vec4(r*sin(Theta)*sin(delta), r*cos(Theta),
		r*sin(Theta)*cos(delta), 1);
	up = vec4(-cos(Theta)*sin(delta), sin(Theta), -cos(Theta)*cos(delta), 0);
	//�����˷����ʼ��
	DirectionW = DirectionS = 1;

	//������ʵ���ʼ��
	MrRobot.SetRobotCenter(vec2(0.0, 0.0));
	MrRobot.RobotInit(ProgramID);
	//ǽ��ʵ���ʼ��
	MrWall.SetWallCenter(vec2(0.0, -10));
	MrWall.WallInit(ProgramID);
	//�ذ�ʵ���ʼ��
	MrFloor.FloorInit();


	//��ת��λ��ʼ��
	Angle = Torso;
	//�ƶ������ʼ��
	MoveMatrix = mat4(1.0);
	
	//����ͶӰ����
	ProjMatrix = Perspective(60, 1.0, 0.1, 100);
	glUniformMatrix4fv(ProjMatrixID, 1, GL_TRUE, ProjMatrix);
	
	//�����������ʼ��
	InitRobotTexture();

	//ǽ�������ʼ��
	InitWallTexture();

	//�ذ������ʼ��
	InitFloorTexture();

	//¼������ǽ�����ģ����ڼ����ײ
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
	//ָʾʵ�����
	MrDenote = new Denote[2];
	MrDenote[0].DenoteInit(ProgramID, vec3(-10, -(0.5*TORSO_HEIGHT + LEFT_LEG_HEIGHT), 0));
	MrDenote[1].DenoteInit(ProgramID, vec3(-10*(MAPLENGTH), -(0.5*TORSO_HEIGHT + LEFT_LEG_HEIGHT), -10*(MAPLENGTH+1)));
	
	//������ɫ��
	mp_->init_shaders("v_texture.glsl", "f_texture.glsl");
	mp_->update_vertex_buffer();
	mp_->update_texture();

	//��������������
	mp_->setLightPos(AllLight.at(0));

	isStart = false;
	isEnd = false;
	
	glBindVertexArray(0);
	
	//���ñ���ɫ
	glClearColor(187.0/255, 174.0/255, 1.0, 1.0);
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_LIGHTING);
}

void TriDimGame::display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glUniform3fv(LightPosID, 1, &(AllLight.at(0)[0]));

	//*************************������start**********************

	glUseProgram(ProgramID);
	//���ӵ�ͻ��������ڸ���ֵ
	at = vec4(0.0, 0.0, 0.0, 1.0);
	eye = vec4(r*sin(Theta)*sin(delta), r*cos(Theta),
		r*sin(Theta)*cos(delta), 1);
	up = vec4(-cos(Theta)*sin(delta), sin(Theta), -cos(Theta)*cos(delta), 0);
	//��ģ�;��󻻳ɻ������ƶ�����
	ModelMatrix = MoveMatrix;

	//ȷ��������λ��
	at.x = ModelMatrix[0][3];
	at.y = ModelMatrix[1][3];
	at.z = ModelMatrix[2][3];

	//���»���������
	MrRobot.SetRobotCenter(vec2(at.x, at.z));

	//ȷ���ӵ�λ��
	eye.x += ModelMatrix[0][3];
	eye.y += ModelMatrix[1][3];
	eye.z += ModelMatrix[2][3];

	//�����ӵ����
	glUniformMatrix4fv(ViewMatrixID, 1, GL_TRUE, GetView());

	//������Ӱ����,��Ҫ�����Ӱʱʹ��
	//����Ӱ����ʱ����ģ�;�������ʹ��
	glUniform1d(isShadowID, true);
	for (int i = 0;i < AllLight.size();i++)
	{
		glUniformMatrix4fv(ShadowMatrixID, 1, GL_TRUE, GetShadow(AllLight.at(i)));

		ModelMatrix = ModelMatrix * RotateY(MrRobot.GetTheta(Torso));//���ɱ任����
		MrRobot.torso(ModelMatrix);//���ɻ���

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

		//�ָ��ƶ�����
		ModelMatrix = MoveMatrix;
	}
	//����������Ӱ���

	//������������
	mp_->setIsRobot(true);
	DrawRobotTexture();

	//*************************������end************************

	//*************************ǽ��start************************

	glUseProgram(ProgramID);
	glUniformMatrix4fv(ModelMatrixID, 1, true, mat4(1.0));

	//��ǽ����Ӱ������
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

	//*************************ǽ��end**************************

	//*************************�ذ�start************************

	glUseProgram(ProgramID);

	//���ذ�����
	mp_->setIsRobot(false);
	DrawFloorTexture();

	//*************************�ذ�end**************************

	//*************************ָʾstart************************

	glUseProgram(ProgramID);
	for (int i = 0;i < 2;i++)
	{
		glUniformMatrix4fv(ModelMatrixID, 1, true, MrDenote[i].GetModelMatrix());
		//��ָʾ
		glUniform1d(isShadowID, false);
		MrDenote[i].DenoteDraw();
		//��ָʾӰ��
		glUniform1d(isShadowID, true);
		MrDenote[i].DenoteDraw();
	}
	//*************************ָʾend**************************

	glutSwapBuffers();
}

void TriDimGame::DrawRobotTexture()
{
	int i;
	//ͷ
	for (i = 0;i < 5;i++)
	{
		mp_->draw_meshes(MrRobot.GetPartModelMatrix(Head), GetView(), i);
	}
	//����
	for (;i < 11;i++)
	{
		mp_->draw_meshes(MrRobot.GetPartModelMatrix(Torso), GetView(), i);
	}
	//���
	for (;i < 17;i++)
	{
		mp_->draw_meshes(MrRobot.GetPartModelMatrix(LeftArm), GetView(), i);
	}
	//�ұ�
	for (;i < 23;i++)
	{
		mp_->draw_meshes(MrRobot.GetPartModelMatrix(RightArm), GetView(), i);
	}
	//���
	for (;i < 29;i++)
	{
		mp_->draw_meshes(MrRobot.GetPartModelMatrix(LeftLeg), GetView(), i);
	}
	//�ҽ�
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
		//�ȼ���Ƿ��ܹ��ƶ�
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
			//����Ƿ�����Թ��ͽ���
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
			//����Ƿ�����Թ��ͽ���
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
	//�ӵ����
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

	//��
	BriPos[0] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(8).x);
	BriPos[1] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(8).y);
	BriPos[2] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(8).z);
	BriPos[3] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(9).y);
	AddTexture(BriPos, BriIndex, "Texture/face.png",vec3(MrRobot.GetRobotCenter().x,0.5,MrRobot.GetRobotCenter().y));
	//ͷ��
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
	//����ǰ��
	BriPos[0] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(8).x);
	BriPos[1] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(8).y);
	BriPos[2] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(8).z);
	BriPos[3] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(9).y);
	AddTexture(BriPos, BriIndex, "Texture/beforebody.png", vec3(MrRobot.GetRobotCenter().x,0.5,MrRobot.GetRobotCenter().y));
	//��������
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
	//���ұ�
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
	//���ǰ��
	BriPos[0] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(8).x);
	BriPos[1] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(8).y);
	BriPos[2] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(8).z);
	BriPos[3] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(9).y);
	AddTexture(BriPos, BriIndex, "Texture/beforeleg.png", vec3(MrRobot.GetRobotCenter().x,0.5,MrRobot.GetRobotCenter().y));
	//�������
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
	//�ҽ�ǰ��
	BriPos[0] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(8).x);
	BriPos[1] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(8).y);
	BriPos[2] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(8).z);
	BriPos[3] = MrRobot.GetCubePos((int)MrRobot.GetCubeFace(9).y);
	AddTexture(BriPos, BriIndex, "Texture/beforeleg.png", vec3(MrRobot.GetRobotCenter().x,0.5,MrRobot.GetRobotCenter().y));
	//�ҽ�����
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

