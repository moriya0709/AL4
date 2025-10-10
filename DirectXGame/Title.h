#pragma once
#include "KamataEngine.h"
#include "Math.h"

using namespace KamataEngine;

class Title {
public:
	void Initialize(Model* model, Camera* camera, const Vector3& position);
	void Update();
	void Draw();

private:
	WorldTransform worldTransform_;
	Model* modelTitle_ = nullptr;
	Camera* camera_ = nullptr;
};
