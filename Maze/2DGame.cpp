#include "2DGame.h"


GLuint vaoDs[3];               //�ֱ�洢��ͼ�͵�Ķ������
GLuint vboDs[6];               //�ֱ�洢��ͼ�͵��λ����ɫ
Map map;                       //�Թ���ͼ
vec2 nowPoint[3];//�������ڵ��λ��
vec3 nowColor[3] = { GREEN,GREEN,GREEN };//�������ڵ����ɫ��Ϊ��ɫ
vec2 Nowp(1,1);

void showcommand()
{
	cout << "please input the r to show the route!" << endl;
	cout << "please input the m to show the map!" << endl;
}

//���лص�����
void Idle()
{
	glutPostRedisplay();
}

Game::Game(int _argc, char **_argv)
{
	themap = map;
	argc = _argc;
	argv = _argv;
}

void Game::display()
{
	glClear(GL_COLOR_BUFFER_BIT);

	//���Ƶ�ͼ���е�
	glBindVertexArray(vaoDs[0]);
	
	for (int i = 0;i < 6 * (MAPLENGTH + 2)*(MAPLENGTH + 2);i += 3)
	{
		glDrawArrays(GL_TRIANGLES, i, 3);
	}

	//����ָʾ��
	glBindVertexArray(vaoDs[1]);

	glDrawArrays(GL_LINES,0,2);
	glDrawArrays(GL_LINE_STRIP, 2, 3);
	glDrawArrays(GL_LINES, 5, 2);
	glDrawArrays(GL_LINE_STRIP, 7, 3);

	//�������ڵ�
	glBindVertexArray(vaoDs[2]);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glutSwapBuffers();
}

void Game::keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'm':
		map.showMap();
		break;
	case 'r':
		map.showRoute();
		break;
	default:
		break;
	}
}

void Game::special(int key, int x,int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		if (!map.isWall(Nowp.x - 1, Nowp.y))
		{
			Nowp.x -= 1;
			for (int i = 0;i < 3;i++)
			{
				nowPoint[i].y += 2 * COE / (MAPLENGTH + 2);
			}
		}
		break;
	case GLUT_KEY_DOWN:
		if (!map.isWall(Nowp.x + 1, Nowp.y))
		{
			Nowp.x += 1;
			for (int i = 0;i < 3;i++)
			{
				nowPoint[i].y -= 2 * COE / (MAPLENGTH + 2);
			}
		}
		break;
	case GLUT_KEY_LEFT:
		if (!map.isWall(Nowp.x, Nowp.y - 1))
		{
			Nowp.y -= 1;
			for (int i = 0;i < 3;i++)
			{
				nowPoint[i].x -= 2 * COE / (MAPLENGTH + 2);
			}
		}
		break;
	case GLUT_KEY_RIGHT:
		if (!map.isWall(Nowp.x, Nowp.y + 1))
		{
			Nowp.y += 1;
			for (int i = 0;i < 3;i++)
			{
				nowPoint[i].x += 2 * COE / (MAPLENGTH + 2);
			}
		}
		break;
	default:
		break;
	}

	glBindBuffer(GL_ARRAY_BUFFER, vboDs[4]);
	glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(vec2), nowPoint, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Game::initial()
{
	//����ָʾ��ͷ�ĵ�λ��
	signPoint[0] = vec2(-coe+coe/(sidelen+2),coe+2*coe/(sidelen+2));
	signPoint[1] = vec2(-coe + coe / (sidelen + 2), coe);
	signPoint[2] = vec2(-coe,coe+coe/(sidelen+2));
	signPoint[3] = vec2(-coe + coe / (sidelen + 2), coe);
	signPoint[4] = vec2(-coe+2*coe/(sidelen+2), coe + coe / (sidelen + 2));
	for (int i = 0;i < 5;i++)
	{
		signPoint[i + 5] = vec2(signPoint[i].x + 2*coe - 2 * coe / (sidelen + 2), signPoint[i].y - 2*coe - 2*coe / (sidelen + 2));
	}
	//���ü�ͷΪ��ɫ
	for (int i = 0;i < 10;i++)
	{
		signColor[i] = RED;
	}

	//�������е��λ�ã�ǽ����ɫ
	for (int i = 0, orderp = 0, orderc = 0;i < sidelen + 2;i++)  //x��
	{
		for (int j = 0;j < sidelen + 2;j++)  //y��
		{
			allPoint[orderp++] = vec2(-coe + 2 * coe / (sidelen + 2)*i, coe - 2 * coe / (sidelen + 2)*j); //1
			allPoint[orderp++] = vec2(-coe + 2 * coe / (sidelen + 2)*i, coe - 2 * coe / (sidelen + 2)*(j + 1)); //2
			allPoint[orderp++] = vec2(-coe + 2 * coe / (sidelen + 2)*(i + 1), coe - 2 * coe / (sidelen + 2)*(j + 1)); //3
			allPoint[orderp++] = vec2(-coe + 2 * coe / (sidelen + 2)*(i + 1), coe - 2 * coe / (sidelen + 2)*(j + 1)); //4
			allPoint[orderp++] = vec2(-coe + 2 * coe / (sidelen + 2)*(i + 1), coe - 2 * coe / (sidelen + 2)*(j)); //4
			allPoint[orderp++] = vec2(-coe + 2 * coe / (sidelen + 2)*i, coe - 2 * coe / (sidelen + 2)*j); //6

			if (map.isWall(j, i))
			{
				allColor[orderc++] = GREY;
				allColor[orderc++] = GREY;
				allColor[orderc++] = GREY;
				allColor[orderc++] = GREY;
				allColor[orderc++] = GREY;
				allColor[orderc++] = GREY;
			}
			else
			{
				allColor[orderc++] = WHITE;
				allColor[orderc++] = WHITE;
				allColor[orderc++] = WHITE;
				allColor[orderc++] = WHITE;
				allColor[orderc++] = WHITE;
				allColor[orderc++] = WHITE;
			}
		}
	}
	//���ÿ�ʼ������Ϊ��ɫ
	allColor[0] = RED;
	allColor[1] = RED;
	allColor[2] = RED;
	allColor[3] = RED;
	allColor[4] = RED;
	allColor[5] = RED;
	allColor[6 * (MAPLENGTH + 2)*(MAPLENGTH + 2)-6] = RED;
	allColor[6 * (MAPLENGTH + 2)*(MAPLENGTH + 2)-5] = RED;
	allColor[6 * (MAPLENGTH + 2)*(MAPLENGTH + 2)-4] = RED;
	allColor[6 * (MAPLENGTH + 2)*(MAPLENGTH + 2)-3] = RED;
	allColor[6 * (MAPLENGTH + 2)*(MAPLENGTH + 2)-2] = RED;
	allColor[6 * (MAPLENGTH + 2)*(MAPLENGTH + 2)-1] = RED;

	//�������ڵ�Ϊλ��
	nowPoint[0] = vec2(-coe + 3*coe / (sidelen + 2),coe-5*coe/2/(sidelen+2));
	nowPoint[1] = vec2(-coe + 5*coe / 2 / (sidelen + 2), coe - coe * 7 / 2 / (sidelen + 2));
	nowPoint[2] = vec2(-coe + 7*coe/2/(sidelen+2), coe - coe * 7 / 2 / (sidelen + 2));


	//������ɫ��
	GLuint program = InitShader("2DGame_vshader.glsl", "2DGame_fshader.glsl");
	glUseProgram(program);
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	GLuint vColor = glGetAttribLocation(program, "vColor");

	//������������
	glGenVertexArrays(2, &vaoDs[0]);

	//��ͼ���е�
	glBindVertexArray(vaoDs[0]);
	glGenBuffers(2, &vboDs[0]);

	//���е�λ��
	glBindBuffer(GL_ARRAY_BUFFER, vboDs[0]);
	glBufferData(GL_ARRAY_BUFFER, 6 * (MAPLENGTH + 2)*(MAPLENGTH + 2) * sizeof(vec2), allPoint, GL_STATIC_DRAW);
	glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);

	//���е���ɫ
	glBindBuffer(GL_ARRAY_BUFFER, vboDs[1]);
	glBufferData(GL_ARRAY_BUFFER, 6 * (MAPLENGTH + 2)*(MAPLENGTH + 2) * sizeof(vec3), allColor, GL_STATIC_DRAW);
	glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColor);

	//ָʾ��
	glBindVertexArray(vaoDs[1]);
	glGenBuffers(2, &vboDs[2]);

	//ָʾ��λ��
	glBindBuffer(GL_ARRAY_BUFFER, vboDs[2]);
	glBufferData(GL_ARRAY_BUFFER,10 * sizeof(vec2), signPoint, GL_STATIC_DRAW);
	glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);

	//ָʾ����ɫ
	glBindBuffer(GL_ARRAY_BUFFER, vboDs[3]);
	glBufferData(GL_ARRAY_BUFFER, 10 * sizeof(vec3), signColor, GL_STATIC_DRAW);
	glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColor);

	//���ڵ�
	glBindVertexArray(vaoDs[2]);
	glGenBuffers(2, &vboDs[4]);

	//ָʾ��λ��
	glBindBuffer(GL_ARRAY_BUFFER, vboDs[4]);
	glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(vec2), nowPoint, GL_STATIC_DRAW);
	glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);

	//ָʾ����ɫ
	glBindBuffer(GL_ARRAY_BUFFER, vboDs[5]);
	glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(vec3), nowColor, GL_STATIC_DRAW);
	glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColor);

	glBindVertexArray(0);
	//��������Ϊ��ɫ
	glClearColor(1, 1, 1, 1);
}



void Game::GameStart()
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(windowlen, windowlen);
	glutInitWindowPosition(600,70);
	glutCreateWindow("2D-Maze");
	glewInit();
	initial();
	glutDisplayFunc(display);
	glutIdleFunc(Idle);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	showcommand();
	glutMainLoop();
}

Game::~Game()
{}