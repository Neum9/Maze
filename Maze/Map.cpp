#include "Map.h"

void Point::show()
{
	cout << " (" << x << "," << y << ") ";
}

Map::Map()
{
	randToRoute();
	randToMap();
	SuitTo3DGame();//��Ӧ3D
}

bool Map::isWall(int x,int y)
{
	return iswall[x][y];
}

void Map::SuitTo3DGame()
{
	//����0,1����Ϊ���
	iswall[0][1] = false;
}

void Map::randToRoute()
{
	bool cannotarrive[MAPLENGTH + 2][MAPLENGTH + 2] = {false};
	srand(time(NULL));
	int dir;//����
	//�Ƚ�����Χ����ǽ
	for (int i = 0;i < MAPLENGTH + 2;i++)
	{
		cannotarrive[i][0] = true;
		cannotarrive[i][MAPLENGTH + 1] = true;
		cannotarrive[0][i] = true;
		cannotarrive[MAPLENGTH + 1][i] = true;
		//˳������map���ǽ
		iswall[i][0] = true;
		iswall[i][MAPLENGTH + 1] = true;
		iswall[0][i] = true;
		iswall[MAPLENGTH + 1][i] = true;
	}
	//�������·��,���Ϊ��1,1�����յ�Ϊ��MAPLENGTH,MAPLENGTH��
	route.push_back(Point(1, 1));
	cannotarrive[1][1] = true;
	Point nowPoint(1, 1);
	//����map����
	//iswall[nowPoint.x][nowPoint.y] = true;
	//�������·��
	while (true)
	{
		//�������һ�㵽���յ������
		if (nowPoint.x == MAPLENGTH && nowPoint.y == MAPLENGTH)
		{
			break;
		}
		//��һ��4���������¼���Ե���ķ���
		bool nowdir[4] = { cannotarrive[nowPoint.x + 1][nowPoint.y], cannotarrive[nowPoint.x - 1][nowPoint.y],
			 cannotarrive[nowPoint.x][nowPoint.y + 1] , cannotarrive[nowPoint.x][nowPoint.y - 1] };
		vector<int> orderofdir;
		for (int i = 0;i < 4;i++)
		{
			if (!nowdir[i])
			{
				orderofdir.push_back(i);
			}
		}
		//������һ���޷������ߣ��ͽ���һ����Ϊǽ������
		if (orderofdir.size() == 0)
		{
			//����map������
			//iswall[nowPoint.x][nowPoint.y] = false;
			route.pop_back();
			//test
			//cout << "pop:";
			//nowPoint.show();
			//cout << endl;
			nowPoint.x = route.at(route.size()-1).x;
			nowPoint.y = route.at(route.size()-1).y;
			continue;
		}
		//�ӿ��з����ҳ�һ��
			dir = (int)rand() % orderofdir.size();
			dir = orderofdir.at(dir);
			int xd, yd;
			if (dir == 0)
			{
				xd = 1;
				yd = 0;
			}
			else if (dir == 1)
			{
				xd = -1;
				yd = 0;
			}
			else if (dir == 2)
			{
				xd = 0;
				yd = 1;
			}
			else if (dir == 3)
			{
				xd = 0;
				yd = -1;
			}
			//���˵�����·�ߣ�����Ϊǽ
			nowPoint.x += xd;
			nowPoint.y += yd;
			route.push_back(nowPoint);
			//���ｫ����Ϊǽ��ֻ��Ϊ��map������
			//iswall[nowPoint.x][nowPoint.y] = true;
			//test
			//cout << "push:";
			//nowPoint.show();
			//cout << endl;
			cannotarrive[nowPoint.x][nowPoint.y] = true;
	}
}

void Map::randToMap()
{
	//�������ǽ
	srand(time(NULL));
	for (int i = 1;i < MAPLENGTH + 2;i++)
	{
		for (int j = 0;j < wallPerLine;j++)
		{
			iswall[i][(int)rand() % (MAPLENGTH)+1] = true;
		}
	}
	//������·��ȥǽ
	for (int i = 0;i < route.size();i++)
	{
		iswall[route.at(i).x][route.at(i).y] = false;
	}
}

void Map::showMap() 
{
	cout << "the map is:" << endl;
	for (int i = 0;i < MAPLENGTH + 2;i++)
	{
		for (int j = 0;j < MAPLENGTH + 2;j++)
		{
			cout << iswall[i][j] << " ";
		}
		cout << endl;
	}
}

void Map::showRoute()
{
	cout << "the MAPLENGTH of route is " << route.size() << endl;
	cout << "the route is:" << endl;
	for (int i = 0;i < route.size();i++)
	{
		route.at(i).show();
		if (i % 10 == 9)
		{
			cout << endl;
		}
    }
	cout << endl;
}

Map::~Map()
{}