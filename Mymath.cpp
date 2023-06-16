#include "Mymath.h"
#include <cassert>
#include <cmath>
#include "MyConst.h"

#pragma region Vector

#pragma region Vector2

#pragma region Operator OverLoad

inline Vector2 operator+(const Vector2& v1, const Vector2& v2) {
	Vector2 temp;
	temp.x = v1.x + v2.x;
	temp.y = v1.y + v2.y;
	return temp;
}
inline Vector2 operator-(const Vector2& v1, const Vector2& v2) {
	Vector2 temp;
	temp.x = v1.x - v2.x;
	temp.y = v1.y - v2.y;
	return temp;
}
inline Vector2 operator*(const Vector2& v, float scalar) {
	Vector2 temp;
	temp.x = v.x * scalar;
	temp.y = v.y * scalar;
	return temp;
}
inline Vector2 operator*(float scalar, const Vector2& v) {
	Vector2 temp;
	temp.x = v.x * scalar;
	temp.y = v.y * scalar;
	return temp;
}

inline Vector2& operator+=(Vector2& v1, const Vector2& v2) {
	v1 = v1 + v2;
	return v1;
}

inline Vector2& operator-=(Vector2& v1, const Vector2& v2) {
	v1 = v1 - v2;
	return v1;
}

inline Vector2& operator*=(Vector2& v, float scalar) {
	v = v * scalar;
	return v;
}

#pragma endregion

// 加算
Vector2 Mymath::Add(const Vector2& v1, const Vector2& v2) { return v1 + v2; }

// 減算
Vector2 Mymath::Subtract(const Vector2& v1, const Vector2& v2) { return v1 - v2; }

// スカラー倍
Vector2 Mymath::Multiply(float scalar, const Vector2& v) { return v * scalar; }

// 内積
float Mymath::Dot(const Vector2& v1, const Vector2& v2) { return v1.x * v2.x + v1.y * v2.y; }

// 長さ
float Mymath::Length(const Vector2& v) { return sqrtf(Dot(v, v)); }

// 正規化
Vector2 Mymath::Normalize(const Vector2& v) {
	float length = Length(v);
	Vector2 temp = v;
	if (length != 0) {
		temp.x /= length;
		temp.y /= length;
	}
	return temp;
}

Vector2 Mymath::Transform(const Vector2& vector, const Matrix3x3& matrix) {
	Vector3 temp = Multiply(Vector3{ vector.x, vector.y, 1.0f }, matrix);
	assert(temp.z != 0.0f);
	temp.x /= temp.z;
	temp.y /= temp.z;
	temp.z /= temp.z;
	return Vector2{ temp.x, temp.y };
}

// End Vector2
#pragma endregion

#pragma region Vector3

#pragma region Operator OverLoad

Vector3 operator+(const Vector3& v1, const Vector3& v2) {
	Vector3 temp{};
	temp.x = v1.x + v2.x;
	temp.y = v1.y + v2.y;
	temp.z = v1.z + v2.z;
	return temp;
}
Vector3 operator-(const Vector3& v1, const Vector3& v2) {
	Vector3 temp{};
	temp.x = v1.x - v2.x;
	temp.y = v1.y - v2.y;
	temp.z = v1.z - v2.z;
	return temp;
}
Vector3 operator*(const Vector3& v, float scalar) {
	Vector3 temp{};
	temp.x = v.x * scalar;
	temp.y = v.y * scalar;
	temp.z = v.z * scalar;
	return temp;
}
Vector3 operator*(float scalar, const Vector3& v) {
	Vector3 temp{};
	temp.x = v.x * scalar;
	temp.y = v.y * scalar;
	temp.z = v.z * scalar;
	return temp;
}

Vector3& operator+=(Vector3& v1, const Vector3& v2) {
	v1 = v1 + v2;
	return v1;
}

Vector3& operator-=(Vector3& v1, const Vector3& v2) {
	v1 = v1 - v2;
	return v1;
}

Vector3& operator*=(Vector3& v, float scalar) {
	v = v * scalar;
	return v;
}

#pragma endregion

// 加算
Vector3 Mymath::Add(const Vector3& v1, const Vector3& v2) { return v1 + v2; }

// 減算
Vector3 Mymath::Subtract(const Vector3& v1, const Vector3& v2) { return v1 - v2; }

// スカラー倍
Vector3 Mymath::Multiply(float scalar, const Vector3& v) { return v * scalar; }

Vector3 Mymath::Multiply(const Vector3& v, const Matrix3x3& matrix) {
	float temp[4]{ 0, 0, 0, 0 };
	float vf[4]{ v.x, v.y, v.z };
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			temp[i] += vf[j] * matrix.m[j][i];
		}
	}
	return Vector3{ temp[0], temp[1], temp[2] };
}
// 内積
float Mymath::Dot(const Vector3& v1, const Vector3& v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

Vector3 Mymath::Cross(const Vector3& v1, const Vector3& v2) {
	Vector3 result;
	result.x = v1.y * v2.z - v2.y * v1.z;
	result.y = v1.z * v2.x - v2.z * v1.x;
	result.z = v1.x * v2.y - v2.x * v1.y;
	return result;
}

// 長さ
float Mymath::Length(const Vector3& v) { return sqrtf(Dot(v, v)); }

// 正規化
Vector3 Mymath::Normalize(const Vector3& v) {
	float length = Length(v);
	Vector3 temp = v;
	if (length != 0) {
		temp.x /= length;
		temp.y /= length;
		temp.z /= length;
	}
	return temp;
}

Vector3 Mymath::Transform(const Vector3& vector, const Matrix4x4& matrix) {
	Vector4 temp = Multiply(Vector4{ vector.x, vector.y, vector.z, 1.0f }, matrix);
	assert(temp.w != 0.0f);
	temp.x /= temp.w;
	temp.y /= temp.w;
	temp.z /= temp.w;
	return Vector3{ temp.x, temp.y, temp.z };
}

Vector3 Mymath::TransformNormal(const Vector3& v, const Matrix4x4& m) {
	Vector3 result{
		v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0],
		v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1],
		v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2],
	};
	return result;
}

Vector3 Mymath::Project(const Vector3& v1, const Vector3& v2) {
	Vector3 normalV2 = Normalize(v2);
	return Dot(v1, normalV2) * normalV2;
}

Vector3 Mymath::CrossPoint(const Vector3& point, const Segment& segment) {
	return segment.origin + Project(point - segment.origin, segment.diff);
}

Vector3 Mymath::Perpendicular(const Vector3& vector) {
	if (vector.x != 0.0f || vector.y != 0.0f) {
		return { -vector.y,vector.x,0.0f };
	}
	return { 0.0f,-vector.z,vector.y };
}

// End Vector3
#pragma endregion

#pragma region Vector4

#pragma region Operator OverLoad

inline Vector4 operator+(const Vector4& v1, const Vector4& v2) {
	Vector4 temp;
	temp.x = v1.x + v2.x;
	temp.y = v1.y + v2.y;
	temp.z = v1.z + v2.z;
	temp.w = v1.w + v2.w;
	return temp;
}
inline Vector4 operator-(const Vector4& v1, const Vector4& v2) {
	Vector4 temp;
	temp.x = v1.x - v2.x;
	temp.y = v1.y - v2.y;
	temp.z = v1.z - v2.z;
	temp.w = v1.w - v2.w;
	return temp;
}
inline Vector4 operator*(const Vector4& v, float scalar) {
	Vector4 temp;
	temp.x = v.x * scalar;
	temp.y = v.y * scalar;
	temp.z = v.z * scalar;
	temp.w = v.w * scalar;
	return temp;
}
inline Vector4 operator*(float scalar, const Vector4& v) {
	Vector4 temp;
	temp.x = v.x * scalar;
	temp.y = v.y * scalar;
	temp.z = v.z * scalar;
	temp.w = v.w * scalar;
	return temp;
}

inline Vector4& operator+=(Vector4& v1, const Vector4& v2) {
	v1 = v1 + v2;
	return v1;
}

inline Vector4& operator-=(Vector4& v1, const Vector4& v2) {
	v1 = v1 - v2;
	return v1;
}

inline Vector4& operator*=(Vector4& v, float scalar) {
	v = v * scalar;
	return v;
}

#pragma endregion

// 加算
Vector4 Mymath::Add(const Vector4& v1, const Vector4& v2) { return v1 + v2; }

// 減算
Vector4 Mymath::Subtract(const Vector4& v1, const Vector4& v2) { return v1 - v2; }

// スカラー倍
Vector4 Mymath::Multiply(float scalar, const Vector4& v) { return v * scalar; }

Vector4 Mymath::Multiply(const Vector4& v, const Matrix4x4& matrix) {
	float temp[4]{ 0, 0, 0, 0 };
	float vf[4]{ v.x, v.y, v.z, v.w };
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			temp[i] += vf[j] * matrix.m[j][i];
		}
	}
	return Vector4{ temp[0], temp[1], temp[2], temp[3] };
}

// 内積
float Mymath::Dot(const Vector4& v1, const Vector4& v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
}

// 長さ
float Mymath::Length(const Vector4& v) { return sqrtf(Dot(v, v)); }

// 正規化
Vector4 Mymath::Normalize(const Vector4& v) {
	float length = Length(v);
	Vector4 temp = v;
	if (length != 0) {
		temp.x /= length;
		temp.y /= length;
		temp.z /= length;
		temp.w /= length;
	}
	return temp;
}
// End Vector4
#pragma endregion

// End Vector
#pragma endregion

#pragma region Sphere

bool Mymath::IsCollision(const Sphere& s1, const Sphere& s2) {
	// 2 つの球の中心点間の距離を求める
	float distance = Length(s1.center - s2.center);
	// 半径の合計よりも短ければ衝突
	if (distance <= s1.radius + s2.radius) {
		return true;
	}
	return false;
}

bool Mymath::IsCollision(const Sphere& sphere, const Plane& plane) {
	// 球の中心と平面の距離を求める
	float distance = sqrtf(powf(Dot(sphere.center, plane.normal) - plane.distance, 2));
	// 半径よりも短ければ衝突
	if (distance <= sphere.radius) {
		return true;
	}
	return false;
}

bool Mymath::IsCollision(const Plane& plane, const Line& line) {
	// 垂直判定を行うために、法線と線の内積を求める
	float dot = Dot(plane.normal, line.diff);
	// 垂直 = 平行であるので、衝突しない
	if (dot == 0.0f) {
		return false;
	}
	return true;
}

bool Mymath::IsCollision(const Plane& plane, const Ray& ray) {
	// 垂直判定を行うために、法線と線の内積を求める
	float dot = Dot(plane.normal, ray.diff);
	// 垂直 = 平行であるので、衝突しない
	if (dot == 0.0f) {
		return false;
	}
	// t を求める
	float t = (plane.distance - Dot(ray.origin, plane.normal)) / dot;
	if (t < 0.0f) {
		return false;
	}
	return true;
}

bool Mymath::IsCollision(const Plane& plane, const Segment& segment) {
	// 垂直判定を行うために、法線と線の内積を求める
	float dot = Dot(plane.normal, segment.diff);
	// 垂直 = 平行であるので、衝突しない
	if (dot == 0.0f) {
		return false;
	}
	// t を求める
	float t = (plane.distance - Dot(segment.origin, plane.normal)) / dot;
	if (t < 0.0f || 1.0f < t) {
		return false;
	}
	return true;
}


#pragma endregion


#pragma region Matrix

#pragma region Matrix4x4

Matrix4x4 Mymath::Add(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 temp = m1;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			temp.m[i][j] += m2.m[i][j];
		}
	}
	return temp;
}

Matrix4x4 Mymath::Subtract(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 temp = m1;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			temp.m[i][j] -= m2.m[i][j];
		}
	}
	return temp;
}

Matrix4x4 Mymath::Multiply(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 temp;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			temp.m[i][j] = m1.m[i][0] * m2.m[0][j] + m1.m[i][1] * m2.m[1][j] +
				m1.m[i][2] * m2.m[2][j] + m1.m[i][3] * m2.m[3][j];
		}
	}
	return temp;
}

Matrix4x4 Mymath::Inverse(const Matrix4x4& matrix) {
	float det;
	det = matrix.m[0][0] * matrix.m[1][1] * matrix.m[2][2] * matrix.m[3][3] +
		matrix.m[0][0] * matrix.m[1][2] * matrix.m[2][3] * matrix.m[3][1] +
		matrix.m[0][0] * matrix.m[1][3] * matrix.m[2][1] * matrix.m[3][2]

		- matrix.m[0][0] * matrix.m[1][3] * matrix.m[2][2] * matrix.m[3][1] -
		matrix.m[0][0] * matrix.m[1][2] * matrix.m[2][1] * matrix.m[3][3] -
		matrix.m[0][0] * matrix.m[1][1] * matrix.m[2][3] * matrix.m[3][2]

		- matrix.m[0][1] * matrix.m[1][0] * matrix.m[2][2] * matrix.m[3][3] -
		matrix.m[0][2] * matrix.m[1][0] * matrix.m[2][3] * matrix.m[3][1] -
		matrix.m[0][3] * matrix.m[1][0] * matrix.m[2][1] * matrix.m[3][2]

		+ matrix.m[0][3] * matrix.m[1][0] * matrix.m[2][2] * matrix.m[3][1] +
		matrix.m[0][2] * matrix.m[1][0] * matrix.m[2][1] * matrix.m[3][3] +
		matrix.m[0][1] * matrix.m[1][0] * matrix.m[2][3] * matrix.m[3][2]

		+ matrix.m[0][1] * matrix.m[1][2] * matrix.m[2][0] * matrix.m[3][3] +
		matrix.m[0][2] * matrix.m[1][3] * matrix.m[2][0] * matrix.m[3][1] +
		matrix.m[0][3] * matrix.m[1][1] * matrix.m[2][0] * matrix.m[3][2]

		- matrix.m[0][3] * matrix.m[1][2] * matrix.m[2][0] * matrix.m[3][1] -
		matrix.m[0][2] * matrix.m[1][1] * matrix.m[2][0] * matrix.m[3][3] -
		matrix.m[0][1] * matrix.m[1][3] * matrix.m[2][0] * matrix.m[3][2]

		- matrix.m[0][1] * matrix.m[1][2] * matrix.m[2][3] * matrix.m[3][0] -
		matrix.m[0][2] * matrix.m[1][3] * matrix.m[2][1] * matrix.m[3][0] -
		matrix.m[0][3] * matrix.m[1][1] * matrix.m[2][2] * matrix.m[3][0]

		+ matrix.m[0][3] * matrix.m[1][2] * matrix.m[2][1] * matrix.m[3][0] +
		matrix.m[0][2] * matrix.m[1][1] * matrix.m[2][3] * matrix.m[3][0] +
		matrix.m[0][1] * matrix.m[1][3] * matrix.m[2][2] * matrix.m[3][0];

	float det11 = 1 / det *
		(+matrix.m[1][1] * matrix.m[2][2] * matrix.m[3][3] +
			matrix.m[1][2] * matrix.m[2][3] * matrix.m[3][1] +
			matrix.m[1][3] * matrix.m[2][1] * matrix.m[3][2] -
			matrix.m[1][3] * matrix.m[2][2] * matrix.m[3][1] -
			matrix.m[1][2] * matrix.m[2][1] * matrix.m[3][3] -
			matrix.m[1][1] * matrix.m[2][3] * matrix.m[3][2]);
	float det12 = 1 / det *
		(-matrix.m[0][1] * matrix.m[2][2] * matrix.m[3][3] -
			matrix.m[0][2] * matrix.m[2][3] * matrix.m[3][1] -
			matrix.m[0][3] * matrix.m[2][1] * matrix.m[3][2] +
			matrix.m[0][3] * matrix.m[2][2] * matrix.m[3][1] +
			matrix.m[0][2] * matrix.m[2][1] * matrix.m[3][3] +
			matrix.m[0][1] * matrix.m[2][3] * matrix.m[3][2]);
	float det13 = 1 / det *
		(+matrix.m[0][1] * matrix.m[1][2] * matrix.m[3][3] +
			matrix.m[0][2] * matrix.m[1][3] * matrix.m[3][1] +
			matrix.m[0][3] * matrix.m[1][1] * matrix.m[3][2] -
			matrix.m[0][3] * matrix.m[1][2] * matrix.m[3][1] -
			matrix.m[0][2] * matrix.m[1][1] * matrix.m[3][3] -
			matrix.m[0][1] * matrix.m[1][3] * matrix.m[3][2]);
	float det14 = 1 / det *
		(-matrix.m[0][1] * matrix.m[1][2] * matrix.m[2][3] -
			matrix.m[0][2] * matrix.m[1][3] * matrix.m[2][1] -
			matrix.m[0][3] * matrix.m[1][1] * matrix.m[2][2] +
			matrix.m[0][3] * matrix.m[1][2] * matrix.m[2][1] +
			matrix.m[0][2] * matrix.m[1][1] * matrix.m[2][3] +
			matrix.m[0][1] * matrix.m[1][3] * matrix.m[2][2]);

	float det21 = 1 / det *
		(-matrix.m[1][0] * matrix.m[2][2] * matrix.m[3][3] -
			matrix.m[1][2] * matrix.m[2][3] * matrix.m[3][0] -
			matrix.m[1][3] * matrix.m[2][0] * matrix.m[3][2] +
			matrix.m[1][3] * matrix.m[2][2] * matrix.m[3][0] +
			matrix.m[1][2] * matrix.m[2][0] * matrix.m[3][3] +
			matrix.m[1][0] * matrix.m[2][3] * matrix.m[3][2]);

	float det22 = 1 / det *
		(+matrix.m[0][0] * matrix.m[2][2] * matrix.m[3][3] +
			matrix.m[0][2] * matrix.m[2][3] * matrix.m[3][0] +
			matrix.m[0][3] * matrix.m[2][0] * matrix.m[3][2] -
			matrix.m[0][3] * matrix.m[2][2] * matrix.m[3][0] -
			matrix.m[0][2] * matrix.m[2][0] * matrix.m[3][3] -
			matrix.m[0][0] * matrix.m[2][3] * matrix.m[3][2]);

	float det23 = 1 / det *
		(-matrix.m[0][0] * matrix.m[1][2] * matrix.m[3][3] -
			matrix.m[0][2] * matrix.m[1][3] * matrix.m[3][0] -
			matrix.m[0][3] * matrix.m[1][0] * matrix.m[3][2] +
			matrix.m[0][3] * matrix.m[1][2] * matrix.m[3][0] +
			matrix.m[0][2] * matrix.m[1][0] * matrix.m[3][3] +
			matrix.m[0][0] * matrix.m[1][3] * matrix.m[3][2]);

	float det24 = 1 / det *
		(+matrix.m[0][0] * matrix.m[1][2] * matrix.m[2][3] +
			matrix.m[0][2] * matrix.m[1][3] * matrix.m[2][0] +
			matrix.m[0][3] * matrix.m[1][0] * matrix.m[2][2] -
			matrix.m[0][3] * matrix.m[1][2] * matrix.m[2][0] -
			matrix.m[0][2] * matrix.m[1][0] * matrix.m[2][3] -
			matrix.m[0][0] * matrix.m[1][3] * matrix.m[2][2]);

	float det31 = 1 / det *
		(+matrix.m[1][0] * matrix.m[2][1] * matrix.m[3][3] +
			matrix.m[1][1] * matrix.m[2][3] * matrix.m[3][0] +
			matrix.m[1][3] * matrix.m[2][0] * matrix.m[3][1] -
			matrix.m[1][3] * matrix.m[2][1] * matrix.m[3][0] -
			matrix.m[1][1] * matrix.m[2][0] * matrix.m[3][3] -
			matrix.m[1][0] * matrix.m[2][3] * matrix.m[3][1]);

	float det32 = 1 / det *
		(-matrix.m[0][0] * matrix.m[2][1] * matrix.m[3][3] -
			matrix.m[0][1] * matrix.m[2][3] * matrix.m[3][0] -
			matrix.m[0][3] * matrix.m[2][0] * matrix.m[3][1] +
			matrix.m[0][3] * matrix.m[2][1] * matrix.m[3][0] +
			matrix.m[0][1] * matrix.m[2][0] * matrix.m[3][3] +
			matrix.m[0][0] * matrix.m[2][3] * matrix.m[3][1]);

	float det33 = 1 / det *
		(+matrix.m[0][0] * matrix.m[1][1] * matrix.m[3][3] +
			matrix.m[0][1] * matrix.m[1][3] * matrix.m[3][0] +
			matrix.m[0][3] * matrix.m[1][0] * matrix.m[3][1] -
			matrix.m[0][3] * matrix.m[1][1] * matrix.m[3][0] -
			matrix.m[0][1] * matrix.m[1][0] * matrix.m[3][3] -
			matrix.m[0][0] * matrix.m[1][3] * matrix.m[3][1]);

	float det34 = 1 / det *
		(-matrix.m[0][0] * matrix.m[1][1] * matrix.m[2][3] -
			matrix.m[0][1] * matrix.m[1][3] * matrix.m[2][0] -
			matrix.m[0][3] * matrix.m[1][0] * matrix.m[2][1] +
			matrix.m[0][3] * matrix.m[1][1] * matrix.m[2][0] +
			matrix.m[0][1] * matrix.m[1][0] * matrix.m[2][3] +
			matrix.m[0][0] * matrix.m[1][3] * matrix.m[2][1]);

	float det41 = 1 / det *
		(-matrix.m[1][0] * matrix.m[2][1] * matrix.m[3][2] -
			matrix.m[1][1] * matrix.m[2][2] * matrix.m[3][0] -
			matrix.m[1][2] * matrix.m[2][0] * matrix.m[3][1] +
			matrix.m[1][2] * matrix.m[2][1] * matrix.m[3][0] +
			matrix.m[1][1] * matrix.m[2][0] * matrix.m[3][2] +
			matrix.m[1][0] * matrix.m[2][2] * matrix.m[3][1]);

	float det42 = 1 / det *
		(+matrix.m[0][0] * matrix.m[2][1] * matrix.m[3][2] +
			matrix.m[0][1] * matrix.m[2][2] * matrix.m[3][0] +
			matrix.m[0][2] * matrix.m[2][0] * matrix.m[3][1] -
			matrix.m[0][2] * matrix.m[2][1] * matrix.m[3][0] -
			matrix.m[0][1] * matrix.m[2][0] * matrix.m[3][2] -
			matrix.m[0][0] * matrix.m[2][2] * matrix.m[3][1]);

	float det43 = 1 / det *
		(-matrix.m[0][0] * matrix.m[1][1] * matrix.m[3][2] -
			matrix.m[0][1] * matrix.m[1][2] * matrix.m[3][0] -
			matrix.m[0][2] * matrix.m[1][0] * matrix.m[3][1] +
			matrix.m[0][2] * matrix.m[1][1] * matrix.m[3][0] +
			matrix.m[0][1] * matrix.m[1][0] * matrix.m[3][2] +
			matrix.m[0][0] * matrix.m[1][2] * matrix.m[3][1]);

	float det44 = 1 / det *
		(+matrix.m[0][0] * matrix.m[1][1] * matrix.m[2][2] +
			matrix.m[0][1] * matrix.m[1][2] * matrix.m[2][0] +
			matrix.m[0][2] * matrix.m[1][0] * matrix.m[2][1] -
			matrix.m[0][2] * matrix.m[1][1] * matrix.m[2][0] -
			matrix.m[0][1] * matrix.m[1][0] * matrix.m[2][2] -
			matrix.m[0][0] * matrix.m[1][2] * matrix.m[2][1]);

	Matrix4x4 result = { det11, det12, det13, det14, det21, det22, det23, det24,
						det31, det32, det33, det34, det41, det42, det43, det44 };
	return result;
}

Matrix4x4 Mymath::Transpose(const Matrix4x4& matrix) {
	Matrix4x4 temp;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			temp.m[i][j] = matrix.m[j][i];
		}
	}
	return temp;
}

Matrix4x4 Mymath::MakeIdentity4x4() {
	return Matrix4x4{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
}

Matrix4x4 Mymath::MakeTranslateMatrix(const Vector3& translate) {
	Matrix4x4 translateMatrix_ = { 1, 0, 0, 0, 0,           1,           0,           0,
								  0, 0, 1, 0, translate.x, translate.y, translate.z, 1 };
	return translateMatrix_;
}

Matrix4x4 Mymath::MakeScaleMatrix(const Vector3& scale) {
	Matrix4x4 scaleMatrix_ = { scale.x, 0, 0, 0, 0, scale.y, 0, 0, 0, 0, scale.z, 0, 0, 0, 0, 1 };
	return scaleMatrix_;
}

Matrix4x4 Mymath::MakeRotateXMatrix(float radian) {
	Matrix4x4 rotX_ = { 1,
					   0,
					   0,
					   0,
					   0,
					   std::cosf(radian),
					   std::sinf(radian),
					   0,
					   0,
					   -std::sinf(radian),
					   std::cosf(radian),
					   0,
					   0,
					   0,
					   0,
					   1 };
	return rotX_;
}

Matrix4x4 Mymath::MakeRotateYMatrix(float radian) {
	Matrix4x4 rotY_ = { std::cosf(radian), 0, -std::sinf(radian), 0, 0, 1, 0, 0,
					   std::sinf(radian), 0, std::cosf(radian),  0, 0, 0, 0, 1 };
	return rotY_;
}

Matrix4x4 Mymath::MakeRotateZMatrix(float radian) {
	Matrix4x4 rotZ_ = {
		std::cosf(radian),
		std::sinf(radian),
		0,
		0,
		-std::sinf(radian),
		std::cosf(radian),
		0,
		0,
		0,
		0,
		1,
		0,
		0,
		0,
		0,
		1 };
	return rotZ_;
}

Matrix4x4 Mymath::MakeRotateXYZMatrix(const Vector3& radian) {
	Matrix4x4 rotX_ = MakeRotateXMatrix(radian.x);
	Matrix4x4 rotY_ = MakeRotateYMatrix(radian.y);
	Matrix4x4 rotZ_ = MakeRotateZMatrix(radian.z);
	return Multiply(Multiply(rotX_, rotY_), rotZ_);
}

Matrix4x4
Mymath::MakeAffineMatrix(const Vector3& scale, const Vector3& rot, const Vector3& translate) {

	Matrix4x4 scaleMatrix_ = MakeScaleMatrix(scale);

	Matrix4x4 rotateMatrix_ = MakeRotateXYZMatrix(rot);

	Matrix4x4 translateMatrix_ = MakeTranslateMatrix(translate);

	return Multiply(Multiply(scaleMatrix_, rotateMatrix_), translateMatrix_);
}

Matrix4x4 Mymath::MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
	Matrix4x4 result{
		1 / aspectRatio * (1 / std::tanf(fovY / 2)),0,0,0,
		0,(1 / std::tanf(fovY / 2)),0,0,
		0,0,farClip / (farClip - nearClip),1,
		0,0,(-nearClip * farClip) / (farClip - nearClip),0.0f
	};
	return result;
}

Matrix4x4 Mymath::MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip) {
	Matrix4x4 result{
		2 / (right - left),0,0,0,
		0,2 / (top - bottom),0,0,
		0,0,1 / (farClip - nearClip),0,
		(left + right) / (left - right),(top + bottom) / (bottom - top),nearClip / (nearClip - farClip),1
	};
	return result;
}

Matrix4x4 Mymath::MakeOrthographicMatrix(const Vector2& leftTop, const Vector2& rightBottom, const Vector2& nearFar) {
	return MakeOrthographicMatrix(leftTop.x, leftTop.y, rightBottom.x, rightBottom.y, nearFar.x, nearFar.y);
}

Matrix4x4 Mymath::MakeViewportMatrix(float left, float top, float width, float height, float minD = 0, float maxD = 1) {
	assert(minD <= maxD);
	Matrix4x4 result{
		width / 2.0f,0,0,0,
		0,-height / 2.0f,0,0,
		0,0,maxD - minD,0,
		left + width / 2.0f,top + height / 2.0f, minD,1
	};
	return result;
}
Matrix4x4 Mymath::MakeViewportMatrix(const Vector2& leftTop, const Vector2& size, const Vector2& depth = { 0,1 }) {
	return MakeViewportMatrix(leftTop.x, leftTop.y, size.x, size.y, depth.x, depth.y);
}
Matrix4x4 Mymath::MakeViewportMatrix(const Vector4& info, const Vector2& depth = { 0,1 }) {
	return MakeViewportMatrix(info.x, info.y, info.z, info.w, depth.x, depth.y);
}

// End Matrix4x4
#pragma endregion

// End Matrix
#pragma endregion