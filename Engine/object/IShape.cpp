#include "./IShape.h"

IShape::IShape() {
}
IShape::~IShape() {
}

IShape::IShape(const IShape& obj) {
	*this = obj;
}

const IShape& IShape::operator=(const IShape& obj) {
	//*this = obj;
	this->transform_.scale_ = obj.transform_.scale_;
	this->transform_.rotate_ = obj.transform_.rotate_;
	this->transform_.translation_ = obj.transform_.translation_;
	//this->transform_.matWorld_ = obj.transform_.matWorld_;
	this->transform_.SetParent(obj.transform_.GetParent());
	//memcpy_s(this->vertices, sizeof(this->vertices), obj.vertices, sizeof(obj.vertices));
	return *this;
}