#pragma once

struct Vector2;
struct Vector3;
struct Vector4;

struct Matrix3x3 {
	float m[3][3];
};
struct Matrix4x4;

#pragma region Oprator OverLoad

inline extern Vector2 operator+(const Vector2& v1, const Vector2& v2);
inline extern Vector2 operator-(const Vector2& v1, const Vector2& v2);
inline extern Vector2 operator*(const Vector2& v, float scalar);
inline extern Vector2 operator*(float scalar, const Vector2& v);
inline extern Vector2& operator+=(Vector2& v1, const Vector2& v2);
inline extern Vector2& operator-=(Vector2& v1, const Vector2& v2);
inline extern Vector2& operator*=(Vector2& v, float scalar);

inline extern Vector3 operator+(const Vector3& v1, const Vector3& v2);
inline extern Vector3 operator-(const Vector3& v1, const Vector3& v2);
inline extern Vector3 operator*(const Vector3& v, float scalar);
inline extern Vector3 operator*(float scalar, const Vector3& v);
inline extern Vector3& operator+=(Vector3& v1, const Vector3& v2);
inline extern Vector3& operator-=(Vector3& v1, const Vector3& v2);
inline extern Vector3& operator*=(Vector3& v, float scalar);

inline extern Vector4 operator+(const Vector4& v1, const Vector4& v2);
inline extern Vector4 operator-(const Vector4& v1, const Vector4& v2);
inline extern Vector4 operator*(const Vector4& v, float scalar);
inline extern Vector4 operator*(float scalar, const Vector4& v);
inline extern Vector4& operator+=(Vector4& v1, const Vector4& v2);
inline extern Vector4& operator-=(Vector4& v1, const Vector4& v2);
inline extern Vector4& operator*=(Vector4& v, float scalar);

#pragma endregion

namespace Mymath {

#pragma region Vector

#pragma region Vector2

/// <summary>
/// ベクトルの加算
/// </summary>
/// <param name="v1">ベクトル１</param>
/// <param name="v2">ベクトル２</param>
/// <returns>加算されたベクトル</returns>
Vector2 Add(const Vector2& v1, const Vector2& v2);

/// <summary>
/// ベクトルの減算
/// </summary>
/// <param name="v1">ベクトル１</param>
/// <param name="v2">ベクトル２</param>
/// <returns>減算されたベクトル</returns>
Vector2 Subtract(const Vector2& v1, const Vector2& v2);

/// <summary>
/// ベクトルの乗算
/// </summary>
/// <param name="scalar">スカラー</param>
/// <param name="v">ベクトル</param>
/// <returns>乗算されたベクトル</returns>
Vector2 Multiply(float scalar, const Vector2& v);

/// <summary>
/// ベクトルの内積
/// </summary>
/// <param name="v1">ベクトル１</param>
/// <param name="v2">ベクトル２</param>
/// <returns>内積</returns>
float Dot(const Vector2& v1, const Vector2& v2);

/// <summary>
/// ベクトルの長さ
/// </summary>
/// <param name="v">ベクトル</param>
/// <returns>ベクトルの長さ</returns>
float Length(const Vector2& v);

/// <summary>
/// ベクトルの正規化
/// </summary>
/// <param name="v">ベクトル</param>
/// <returns>正規化されたベクトル</returns>
Vector2 Normalize(const Vector2& v);

/// <summary>
/// 座標変換
/// </summary>
/// <param name="vector">座標</param>
/// <param name="matrix">行列</param>
/// <returns>変換座標</returns>
Vector2 Transform(const Vector2& vector, const Matrix3x3& matrix);

// End Vector2
#pragma endregion

#pragma region Vector3

/// <summary>
/// ベクトルの加算
/// </summary>
/// <param name="v1">ベクトル１</param>
/// <param name="v2">ベクトル２</param>
/// <returns>加算されたベクトル</returns>
Vector3 Add(const Vector3& v1, const Vector3& v2);

/// <summary>
/// ベクトルの減算
/// </summary>
/// <param name="v1">ベクトル１</param>
/// <param name="v2">ベクトル２</param>
/// <returns>減算されたベクトル</returns>
Vector3 Subtract(const Vector3& v1, const Vector3& v2);

/// <summary>
/// ベクトルの乗算
/// </summary>
/// <param name="scalar">スカラー</param>
/// <param name="v">ベクトル</param>
/// <returns>乗算されたベクトル</returns>
Vector3 Multiply(float scalar, const Vector3& v);

/// <summary>
/// ベクトルと行列の乗算
/// </summary>
/// <param name="v">ベクトル</param>
/// <param name="matrix">行列</param>
/// <returns>乗算されたベクトル</returns>
Vector3 Multiply(const Vector3& v, const Matrix3x3& matrix);

/// <summary>
/// ベクトルの内積
/// </summary>
/// <param name="v1">ベクトル１</param>
/// <param name="v2">ベクトル２</param>
/// <returns>内積</returns>
float Dot(const Vector3& v1, const Vector3& v2);

/// <summary>
/// ベクトルの長さ
/// </summary>
/// <param name="v">ベクトル</param>
/// <returns>ベクトルの長さ</returns>
float Length(const Vector3& v);

/// <summary>
/// ベクトルの正規化
/// </summary>
/// <param name="v">ベクトル</param>
/// <returns>正規化されたベクトル</returns>
Vector3 Normalize(const Vector3& v);

/// <summary>
/// 座標変換
/// </summary>
/// <param name="vector">座標</param>
/// <param name="matrix">行列</param>
/// <returns>変換座標</returns>
Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);

/// <summary>
///	ベクトル変換
/// </summary>
/// <param name="v">ベクトル</param>
/// <param name="m">行列</param>
/// <returns>変換ベクトル</returns>
Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m);

// End Vector3
#pragma endregion

#pragma region Vecotor4

/// <summary>
/// ベクトルの加算
/// </summary>
/// <param name="v1">ベクトル１</param>
/// <param name="v2">ベクトル２</param>
/// <returns>加算されたベクトル</returns>
Vector4 Add(const Vector4& v1, const Vector4& v2);

/// <summary>
/// ベクトルの減算
/// </summary>
/// <param name="v1">ベクトル１</param>
/// <param name="v2">ベクトル２</param>
/// <returns>減算されたベクトル</returns>
Vector4 Subtract(const Vector4& v1, const Vector4& v2);

/// <summary>
/// ベクトルの乗算
/// </summary>
/// <param name="scalar">スカラー</param>
/// <param name="v">ベクトル</param>
/// <returns>乗算されたベクトル</returns>
Vector4 Multiply(float scalar, const Vector4& v);

/// <summary>
/// ベクトルの乗算
/// </summary>
/// <param name="v">ベクトル</param>
/// <param name="matrix">行列</param>
/// <returns>乗算されたベクトル</returns>
Vector4 Multiply(const Vector4& v, const Matrix4x4& matrix);

/// <summary>
/// ベクトルの内積
/// </summary>
/// <param name="v1">ベクトル１</param>
/// <param name="v2">ベクトル２</param>
/// <returns>内積</returns>
float Dot(const Vector4& v1, const Vector4& v2);

/// <summary>
/// ベクトルの長さ
/// </summary>
/// <param name="v">ベクトル</param>
/// <returns>ベクトルの長さ</returns>
float Length(const Vector4& v);

/// <summary>
/// ベクトルの正規化
/// </summary>
/// <param name="v">ベクトル</param>
/// <returns>正規化されたベクトル</returns>
Vector4 Normalize(const Vector4& v);

// End Vecotr4
#pragma endregion

// End Vector
#pragma endregion

#pragma region Matrix

#pragma region Matrix3x3

// End Matrix3x3
#pragma endregion

#pragma region Matrix4x4

/// <summary>
/// 行列の加算
/// </summary>
/// <param name="m1">行列１</param>
/// <param name="m2">行列２</param>
/// <returns>加算された行列</returns>
Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2);

/// <summary>
/// 行列の減算
/// </summary>
/// <param name="m1">行列１</param>
/// <param name="m2">行列２</param>
/// <returns>減算された行列</returns>
Matrix4x4 Subtract(const Matrix4x4& m1, const Matrix4x4& m2);

/// <summary>
/// 行列の乗算
/// </summary>
/// <param name="m1">行列１</param>
/// <param name="m2">行列２</param>
/// <returns><乗算された行列/returns>
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);

/// <summary>
/// 逆行列
/// </summary>
/// <param name="matrix">行列</param>
/// <returns>逆行列</returns>
Matrix4x4 Inverse(const Matrix4x4& matrix);

/// <summary>
/// 転置行列
/// </summary>
/// <param name="matrix">行列</param>
/// <returns>転置行列</returns>
Matrix4x4 Transpose(const Matrix4x4& matrix);

/// <summary>
/// 4x4 単位行列
/// </summary>
/// <returns>4x4 単位行列</returns>
Matrix4x4 MakeIdentity4x4();

/// <summary>
/// 4x4 平行移動行列
/// </summary>
/// <param name="translate">移動ベクトル</param>
/// <returns>4x4 平行移動行列</returns>
Matrix4x4 MakeTranslateMatrix(const Vector3& translate);

/// <summary>
/// 4x4 拡縮行列
/// </summary>
/// <param name="scale">拡縮</param>
/// <returns>4x4 拡縮行列</returns>
Matrix4x4 MakeScaleMatrix(const Vector3& scale);

/// <summary>
/// X 軸回転行列
/// </summary>
/// <param name="radian">X の回転角</param>
/// <returns>X 軸回転行列</returns>
Matrix4x4 MakeRotateXMatrix(float radian);

/// <summary>
/// Y 軸回転行列
/// </summary>
/// <param name="radian">Y の回転角</param>
/// <returns>Y 軸回転行列</returns>
Matrix4x4 MakeRotateYMatrix(float radian);

/// <summary>
/// Z 軸回転行列
/// </summary>
/// <param name="radian">Z の回転角</param>
/// <returns>Z 軸回転行列</returns>
Matrix4x4 MakeRotateZMatrix(float radian);

/// <summary>
/// 回転行列
/// </summary>
/// <param name="radian">XYZ の回転角</param>
/// <returns>回転行列</returns>
Matrix4x4 MakeRotateXYZMatrix(const Vector3& radian);

/// <summary>
/// 4x4 アフィン行列
/// </summary>
/// <param name="scale">スケール</param>
/// <param name="rotate">回転</param>
/// <param name="translate">移動</param>
/// <returns>4x4 アフィン行列</returns>
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

// End Matrix4x4
#pragma endregion

// End Matrix
#pragma endregion

} // namespace Mymath