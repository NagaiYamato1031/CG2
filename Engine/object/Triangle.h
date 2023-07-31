#pragma once
#include "./Transform.h"

class Triangle : public TransformEx {
public:

	Triangle();
	Triangle(const Triangle& obj);
	const Triangle& operator=(const Triangle& obj);
	~Triangle();

	void SuccessorInitialize() override;


public:

	struct VectorPosColor {
		Vector3 position;	// 座標
		Vector4 color;		// 色
	};

public:

	// 座標と色
	VectorPosColor vertices[3];

};

