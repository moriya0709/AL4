#include "Goal.h"

void Goal::Initialize(Model* model, Camera* camera, const Vector3& position) { 
	modelGoal_ = model; 
	camera_ = camera;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
}

void Goal::Update() { 
	// 回転
	worldTransform_.rotation_.y += 0.05f;

	// アフィン変換～DirectXに転送
	WorldTransformUpdate(worldTransform_);
}

void Goal::Draw() { modelGoal_->Draw(worldTransform_, *camera_); }

Vector3 Goal::GetWorldPosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos = worldTransform_.translation_;

	return worldPos;
}

AABB Goal::GetAABB() {
	Vector3 worldPos = GetWorldPosition();

	AABB aabb;

	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};

	return aabb;
}
