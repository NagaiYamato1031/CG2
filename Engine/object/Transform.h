#pragma once
#include "../math/Vector3.h"
#include "../math/Matrix4x4.h"

#include <d3d12.h>
#include <wrl.h>
#include <memory>

/// <summary>
/// ワールド空間までの変換を含めた Transform クラス
/// </summary>
class Transform {
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	Transform();

public:

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize();

	/// <summary>
	/// 行列更新
	/// </summary>
	virtual void UpdateMatrix();

#pragma region ゲッターセッター

	void SetScale(const Vector3& scale) { scale_ = scale; };
	void SetRotate(const Vector3& rotate) { rotate_ = rotate; };
	void SetTranslation(const Vector3& translation) { translation_ = translation; };
	/// <summary>
	/// 親の Transform を決める
	/// </summary>
	/// <param name="parent"></param>
	void SetParent(Transform* parent) { parent_ = parent; };

	const Vector3& GetScale() const { return scale_; };
	const Vector3& GetTranslation() const { return translation_; };
	const Vector3& GetRotate() const { return rotate_; };
	const Matrix4x4& GetMatrix() const { return matWorld_; };
	Transform* GetParent()const { return parent_; };

#pragma endregion




protected:

	// スケール
	Vector3 scale_ = { 1,1,1 };
	// 回転角
	Vector3 rotate_ = { 0,0,0 };
	// ローカル座標
	Vector3 translation_ = { 0,0,0 };
	// ローカル → ワールド変換行列
	Matrix4x4 matWorld_;
	// 親の Transform
	Transform* parent_ = nullptr;

};

/// <summary>
/// ConstantBuffer への転送を含めた Transform クラス
/// </summary>
class TransformEx : public Transform {
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// 行列更新
	/// </summary>
	void UpdateMatrix() override;

	/// <summary>
	/// 行列の転送
	/// </summary>
	void TransferMatrix();

private:

	/// <summary>
	/// リソースの作成
	/// </summary>
	void CreateConstBuffer();

private:

	struct ConstantBufferData
	{
		Matrix4x4* wvpData_;
	};
	struct CBuffer
	{
		Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
		Matrix4x4* wvpData_;
	};


private:
	// コンスタントバッファ
	std::unique_ptr<CBuffer> cBuffer_;

};
