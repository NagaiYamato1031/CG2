#pragma once
#include "./Transform.h"

class Triangle {
public:

	Triangle();
	Triangle(const Triangle& obj);
	const Triangle& operator=(const Triangle& obj);
	~Triangle();

	void Initialize();

public:

	struct VectorPosColor {
		Vector3 position;	// 座標
		Vector4 color;		// 色
	};

public:

	// ワールド座標データ
	WorldTransform transform_;

	// 座標と色
	VectorPosColor vertices[3];

};

