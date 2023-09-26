#pragma once
#include "../math/Vector2.h"
#include "../math/Vector3.h"
#include "../math/Vector4.h"
#include "../math/Matrix3x3.h"
#include "../math/Matrix4x4.h"

#include <d3d12.h>
#include <wrl.h>
#include <memory>

/// <summary>
/// WorldTransform クラス
/// </summary>
class WorldTransform {
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	WorldTransform();
	WorldTransform(Vector3 s, Vector3 r, Vector3 t);
	WorldTransform(Vector3 r, Vector3 t);
	WorldTransform(Vector3 t);

	~WorldTransform() = default;

public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// リソースを作成する
	/// </summary>
	void CreateResource();


	///// <summary>
	///// 行列更新
	///// </summary>
	//void UpdateMatrix();

	///// <summary>
	///// 行列の転送
	///// </summary>
	//void TransferMatrix();

public:

	void SetParent(WorldTransform* parent);

	WorldTransform* GetParent() const ;
	Matrix4x4 GetMatWorld();
	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress()const;
	ID3D12Resource* GetResource()const { return resource_.Get(); };

public:

	// スケール
	Vector3 scale_ = { 1,1,1 };
	// 回転角
	Vector3 rotate_ = { 0,0,0 };
	// ローカル座標
	Vector3 translation_ = { 0,0,0 };


private:
	// コンスタントバッファ
	//std::unique_ptr<CBuffer> cBuffer_;

	// ローカル → ワールド変換行列
	Matrix4x4* matWorld_ = nullptr;
	// 親の Transform
	WorldTransform* parent_ = nullptr;

	// リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> resource_;


};
