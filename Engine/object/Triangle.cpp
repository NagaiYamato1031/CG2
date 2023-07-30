#include "./Triangle.h"

Triangle::Triangle() {
	SuccessorInitialize();
}
Triangle::~Triangle() {

}

Triangle::Triangle(const Triangle& obj) {
	*this = obj;
}

const Triangle& Triangle::operator=(const Triangle& obj) {
	*this = obj;
}

void Triangle::SuccessorInitialize() {
	for (size_t i = 0; i < 3; i++)
	{
		vertices[i].position = { 0.0f,0.0f,0.0f };
		vertices[i].color = { 1.0f,1.0f,1.0f,1.0f };
	}
}