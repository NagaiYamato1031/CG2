#include "./Triangle.h"

Triangle::Triangle() {
}
Triangle::~Triangle() {
}

Triangle::Triangle(const Triangle& obj) {
	*this = obj;
}

const Triangle& Triangle::operator=(const Triangle& obj) {
	//*this = obj;
	this->transform_.scale_ = obj.transform_.scale_;
	this->transform_.rotate_ = obj.transform_.rotate_;
	this->transform_.translation_ = obj.transform_.translation_;
	this->transform_.matWorld_ = obj.transform_.matWorld_;
	this->transform_.parent_ = obj.transform_.parent_;
	memcpy_s(this->vertices, sizeof(this->vertices), obj.vertices, sizeof(obj.vertices));
	return *this;
}

void Triangle::Initialize() {
	transform_.Initialize();
	for (size_t i = 0; i < 3; i++)
	{
		vertices[i].position = { 0.0f,0.0f,0.0f };
		vertices[i].color = { 1.0f,1.0f,1.0f,1.0f };
	}
}