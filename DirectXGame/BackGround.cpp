#include "BackGround.h"

void BackGround::Initialize(Model* model, Camera* camera, const Vector3& position) {
	modelBackGround_ = model;
	camera_ = camera;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
}

void BackGround::Update() {
	// アフィン変換～DirectXに転送
	WorldTransformUpdate(worldTransform_);
}

void BackGround::Draw() { modelBackGround_->Draw(worldTransform_, *camera_); }
