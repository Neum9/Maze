#include "Map.h"

void Point::show()
{
	cout << " (" << x << "," << y << ") ";
}

Map::Map()
{
	randToRoute();
	randToMap();
	SuitTo3DGame();//适应3D
}

bool Map::isWall(int x,int y)
{
	return iswall[x][y];
}

void Map::SuitTo3DGame()
{
	//将（0,1）设为入口
	iswall[0][1] = false;
}

void Map::randToRoute()
{
	bool cannotarrive[MAPLENGTH + 2][MAPLENGTH + 2] = {false};
	srand(time(NULL));
	int dir;//方向
	//先将最外围生成墙
	for (int i = 0;i < MAPLENGTH + 2;i++)
	{
		cannotarrive[i][0] = true;
		cannotarrive[i][MAPLENGTH + 1] = true;
		cannotarrive[0][i] = true;
		cannotarrive[MAPLENGTH + 1][i] = true;
		//顺便生成map里的墙
		iswall[i][0] = true;
		iswall[i][MAPLENGTH + 1] = true;
		iswall[0][i] = true;
		iswall[MAPLENGTH + 1][i] = true;
	}
	//随机生成路线,起点为（1,1），终点为（MAPLENGTH,MAPLENGTH）
	route.push_back(Point(1, 1));
	cannotarrive[1][1] = true;
	Point nowPoint(1, 1);
	//方便map生成
	//iswall[nowPoint.x][nowPoint.y] = true;
	//随机生成路线
	while (true)
	{
		//若是最后一点到达终点则结束
		if (nowPoint.x == MAPLENGTH && nowPoint.y == MAPLENGTH)
		{
			break;
		}
		//用一个4长度数组记录可以到达的方向
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
		//若是这一点无法继续走，就将这一点设为墙并舍弃
		if (orderofdir.size() == 0)
		{
			//方便map的生成
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
		//从可行方向找出一个
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
			//将此点纳入路线，并设为墙
			nowPoint.x += xd;
			nowPoint.y += yd;
			route.push_back(nowPoint);
			//这里将此设为墙，只是为了map的生成
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
	//随机设置墙
	srand(time(NULL));
	for (int i = 1;i < MAPLENGTH + 2;i++)
	{
		for (int j = 0;j < wallPerLine;j++)
		{
			iswall[i][(int)rand() % (MAPLENGTH)+1] = true;
		}
	}
	//将可行路线去墙
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