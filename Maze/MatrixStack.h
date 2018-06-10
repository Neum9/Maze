#pragma once
#include <assert.h>
#include "include/Angel.h"
using namespace std;

#pragma comment(lib,"glew32.lib")

class MatrixStack {
	int    _index;
	int    _size;
	mat4*  _matrices;

public:
	MatrixStack(int numMatrices = 100);
	~MatrixStack();
	void push(const mat4& m);
	mat4& pop(void);
};