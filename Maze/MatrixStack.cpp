#include "MatrixStack.h"

MatrixStack::MatrixStack(int numMatrices) :_index(0), _size(numMatrices)
{
	_matrices = new mat4[numMatrices];
}

MatrixStack::~MatrixStack()
{
	delete[]_matrices;
}

void MatrixStack::push(const mat4& m) {
	assert(_index + 1 < _size);
	_matrices[_index++] = m;

}

mat4& MatrixStack::pop(void) {
	assert(_index - 1 >= 0);
	_index--;
	return _matrices[_index];
}