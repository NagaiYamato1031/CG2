#pragma once

#include<cstdint>

#include "math/Vector2.h"
#include "math/Vector3.h"
#include "math/Vector4.h"
#include "math/Matrix3x3.h"
#include "math/Matrix4x4.h"

/// <summary>
/// 球体
/// </summary>
struct Sphere
{
	Vector3 center;	// 中心
	float radius;	// 半径
};

/// <summary>
/// 直線
/// </summary>
struct Line
{
	Vector3 origin;	// 始点
	Vector3 diff;	// 終点への差分ベクトル
};

/// <summary>
/// 半直線
/// </summary>
struct Ray
{
	Vector3 origin;	// 始点
	Vector3 diff;	// 終点への差分ベクトル
};

/// <summary>
/// 線分
/// </summary>
struct Segment
{
	Vector3 origin;	// 始点
	Vector3 diff;	// 終点への差分ベクトル
};

/// <summary>
/// 平面
/// </summary>
struct Plane
{
	Vector3 normal;	// 法線
	float distance;	// 距離
};

/// <summary>
/// 三角形
/// </summary>
struct Triangle
{
	Vector3 vertices[3];	// 頂点
};

/// <summary>
/// 軸平行境界箱
/// </summary>
struct AABB
{
	Vector2 min;	// 最小点
	Vector2 max;	// 最大点
};

/// <summary>
/// トランスフォーム
/// </summary>
struct Transform {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};

/// <summary>
/// 頂点データ
/// </summary>
struct VertexData
{
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};

/// <summary>
/// マテリアル
/// </summary>
struct Material
{
	Vector4 color;
	int32_t enableLighting;
};

/// <summary>
/// トランスフォーム行列
/// </summary>
struct TransformationMatrix {
	Matrix4x4 WVP;		// スクリーン座標変換行列
	Matrix4x4 World;	// ワールド行列
};

/// <summary>
/// 平行光源
/// </summary>
struct DirectionalLight
{
	Vector4 color;		// ライトの色
	Vector3 direction;	// ライトの向き
	float intensity;	// 輝度
};
