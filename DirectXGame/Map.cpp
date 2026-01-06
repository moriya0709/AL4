#include "Map.h"

void Map::Initialize(Camera* camera) {
	modelGround_ = Model::CreateFromOBJ("ground");
	camera_ = camera;
	worldTransform_.Initialize();
	worldTransform_.translation_ = {0.0f, -1.0f, 0.0f};
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};

	
}

void Map::Update() { 


	WorldTransformUpdate(worldTransform_); 
}

void Map::Draw() { modelGround_->Draw(worldTransform_, *camera_); }

// ワールド座標を取得
Vector3 Map::GetWorldPosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos = worldTransform_.translation_;

	return worldPos;
}

// ワールドスケールを取得
Vector3 Map::GetWorldScale() {
	// ワールド座標を入れる変数
	Vector3 worldScale;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldScale = worldTransform_.scale_;

	return worldScale;
}


// AABBを取得
AABB Map::GetAABB() {
	Vector3 worldPos = GetWorldPosition();

	AABB aabb;

	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};

	return aabb;
}

// 衝突応答
void Map::OnCollision(const Player* player) { (void)player; }



