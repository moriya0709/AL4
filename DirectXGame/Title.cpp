#include "Title.h"

void Title::Initialize(Model* model, Camera* camera, const Vector3& position) {
	modelTitle_ = model;
	camera_ = camera;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.scale_ = {30.0f, 30.0f, 30.0f};
	worldTransform_.rotation_.z = 0.95f * 3.14f;
}

void Title::Update() {
	// アフィン変換～DirectXに転送
	WorldTransformUpdate(worldTransform_);
}

void Title::Draw() { modelTitle_->Draw(worldTransform_, *camera_); }
