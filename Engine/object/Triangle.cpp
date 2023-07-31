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
	//*this = obj;
	this->scale_ = obj.scale_;
	this->rotate_ = obj.rotate_;
	this->translation_ = obj.translation_;
	this->matWorld_ = obj.matWorld_;
	this->parent_ = obj.parent_;
	memcpy_s(this->vertices, sizeof(this->vertices), obj.vertices, sizeof(obj.vertices));
	return *this;
}

void Triangle::SuccessorInitialize() {
	for (size_t i = 0; i < 3; i++)
	{
		vertices[i].position = { 0.0f,0.0f,0.0f };
		vertices[i].color = { 1.0f,1.0f,1.0f,1.0f };
	}
}