#include "./Transform.h"
#include "../math/Mymath.h"
#include "../base/CanvasTool.h"

#pragma region WorldTransform

WorldTransform::WorldTransform() {
	Initialize();
}
WorldTransform::WorldTransform(Vector3 s, Vector3 r, Vector3 t) {
	Initialize();
	scale_ = s;
	rotate_ = r;
	translation_ = t;
}
WorldTransform::WorldTransform(Vector3 r, Vector3 t) {
	Initialize();
	rotate_ = r;
	translation_ = t;
}
WorldTransform::WorldTransform(Vector3 t) {
	Initialize();
	translation_ = t;
}

void WorldTransform::Initialize() {
	scale_ = { 1.0f,1.0f,1.0f };
	rotate_ = { 0.0f,0.0f,0.0f };
	translation_ = { 0.0f,0.0f,0.0f };
	matWorld_ = new Matrix4x4(Mymath::MakeIdentity4x4());
	parent_ = nullptr;
}
//
//void WorldTransform::UpdateMatrix() {
//	*matWorld_ = Mymath::MakeAffineMatrix(scale_, rotate_, translation_);
//	// 親があれば親を掛ける
//	if (parent_ != nullptr) {
//		*matWorld_ = Mymath::Multiply(parent_->GetMatrix(), *matWorld_);
//	}
//	TransferMatrix();
//}
//
//void WorldTransform::TransferMatrix() {
//	//*cBuffer_->CBData_->wvpData_ = matWorld_;
//	*cBuffer_->wvpData_ = *matWorld_;
//}

void WorldTransform::CreateResource() {
	resource_ = (CanvasTool::GetInstance()->CreateBufferResource(sizeof(Matrix4x4)));
	resource_->Map(0, nullptr, reinterpret_cast<void**>(&matWorld_));
}


void WorldTransform::SetParent(WorldTransform* parent) {
	parent_ = parent;
}


WorldTransform* WorldTransform::GetParent() const {
	return parent_;
}

Matrix4x4 WorldTransform::GetMatWorld() {
	*matWorld_ = Mymath::MakeAffineMatrix(scale_, rotate_, translation_);

	if (parent_) {
		//*matWorld_ = parent_->GetMatWorld() * *matWorld_;
		*matWorld_ = Mymath::Multiply(parent_->GetMatWorld(), *matWorld_);
	}

	return *matWorld_;
}
D3D12_GPU_VIRTUAL_ADDRESS WorldTransform::GetGPUVirtualAddress()const {
	return resource_->GetGPUVirtualAddress();
}

#pragma endregion