#pragma once
#include "./Transform.h"

class IShape {
public:

	IShape();
	IShape(const IShape& obj);
	const IShape& operator=(const IShape& obj);
	~IShape();

	virtual void Initialize() = 0;

public:

	struct VectorPosColor {
		Vector3 position;	// 座標
		Vector4 color;		// 色
	};

public:

	// ワールド座標データ
	WorldTransform transform_;

	// 座標と色
	//VectorPosColor vertices[3];

};

