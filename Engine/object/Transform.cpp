#include "./Transform.h"
#include "../math/Mymath.h"
#include "../base/CanvasTool.h"

#pragma region Transform

Transform::Transform() {
	scale_ = { 1.0f,1.0f,1.0f };
	rotate_ = { 0.0f,0.0f,0.0f };
	translation_ = { 0.0f,0.0f,0.0f };
	matWorld_ = Mymath::MakeIdentity4x4();
	parent_ = nullptr;
}

void Transform::Initialize() {
	scale_ = { 1.0f,1.0f,1.0f };
	rotate_ = { 0.0f,0.0f,0.0f };
	translation_ = { 0.0f,0.0f,0.0f };
	matWorld_ = Mymath::MakeIdentity4x4();
	parent_ = nullptr;
}
void Transform::UpdateMatrix() {
	matWorld_ = Mymath::MakeAffineMatrix(scale_, rotate_, translation_);
	// 親があれば親を掛ける
	if (parent_ != nullptr) {
		matWorld_ = Mymath::Multiply(parent_->matWorld_, matWorld_);
	}
}

#pragma endregion

#pragma region Transform 派生クラス


#pragma region TransformEx

TransformEx::TransformEx() {
	scale_ = { 1.0f,1.0f,1.0f };
	rotate_ = { 0.0f,0.0f,0.0f };
	translation_ = { 0.0f,0.0f,0.0f };
	matWorld_ = Mymath::MakeIdentity4x4();
	parent_ = nullptr;
}

void TransformEx::Initialize() {
	scale_ = { 1.0f,1.0f,1.0f };
	rotate_ = { 0.0f,0.0f,0.0f };
	translation_ = { 0.0f,0.0f,0.0f };
	matWorld_ = Mymath::MakeIdentity4x4();
	parent_ = nullptr;
	CreateConstBuffer();
}

void TransformEx::UpdateMatrix() {
	matWorld_ = Mymath::MakeAffineMatrix(scale_, rotate_, translation_);
	// 親があれば親を掛ける
	if (parent_ != nullptr) {
		matWorld_ = Mymath::Multiply(parent_->GetMatrix(), matWorld_);
	}
	TransferMatrix();
}

void TransformEx::TransferMatrix() {
	//*cBuffer_->CBData_->wvpData_ = matWorld_;
	*cBuffer_->wvpData_ = matWorld_;
}

void TransformEx::CreateConstBuffer() {
	CanvasTool* tool = CanvasTool::GetInstance();

	cBuffer_ = std::make_unique<CBuffer>();

	cBuffer_->resource_ = tool->CreateBufferResource(sizeof(Matrix4x4));
	//cBuffer_->CBData_->wvpData_ = nullptr;
	cBuffer_->wvpData_ = nullptr;
	//cBuffer_->resource_->Map(0, nullptr, reinterpret_cast<void**>(&cBuffer_->CBData_));
	cBuffer_->resource_->Map(0, nullptr, reinterpret_cast<void**>(&cBuffer_->wvpData_));

	TransferMatrix();
}

#pragma endregion


#pragma endregion