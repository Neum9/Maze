#include "2DGame.h"
#include "3DGame.h"
#include <iostream>
using namespace std;

int main(int argc, char **argv)
{
	
	TriDimGame game(argc,argv);
	//Game game(argc, argv);
	game.GameStart();

	system("pause");
	return 0;
}