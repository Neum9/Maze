#pragma once
#include "Map.h"
#include "include/Angel.h"
#pragma comment(lib, "glew32.lib")

#define WINDOWLEN 600 //���ڳ���
#define COE 0.8       //�Թ���ͼ/���ڣ�ϵ����
#define BlACK vec3(0,0,0) //��ɫ
#define WHITE vec3(1,1,1) //��ɫ
#define GREY vec3(105.0/255,105.0/255,105.0/255)//��ɫ
#define RED vec3(1,0,0)//��ɫ
#define GREEN vec3(0,1,0)//��ɫ

class Game
{
private:
	int argc;
	char **argv;
	int sidelen = MAPLENGTH;          //�߳�
	int windowlen = WINDOWLEN;     //���ڳ���
	Map themap;
	double coe = COE;              //�����������ֵ��������
	vec2 signPoint[10];//����ָʾ��ͷ
	vec3 signColor[10];//����ָʾ��ͷ��ɫ
	vec2 allPoint[6 * (MAPLENGTH + 2)*(MAPLENGTH + 2)];//�����ͼ���еĵ�λ��
	vec3 allColor[6 * (MAPLENGTH + 2)*(MAPLENGTH + 2)];//�����ͼ���еĵ���ɫ,��ǽ��Ϊ��ɫ
	void initial();                 //��ʼ��
	static void display();                //��ʾ����
	static void keyboard(unsigned char key,int x,int y);//���̿��ƺ���
	static void special(int key, int x, int y);//�����������Ӧ�¼�
public:
	Game(int _argc, char **_argv);
	void GameStart();
	~Game();
};

 