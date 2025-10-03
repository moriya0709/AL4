#define NOMINMAX


#include<algorithm>
#include <assert.h>

#include "Bullet.h"
#include "MapChipField.h"


void Bullet::Initialize(Model* model,Camera* camera, const Vector3& position) {
	// NULLポインタチェック
	assert(model);
	// 引数の内容をメンバ変数に記録
	modelBullet_ = model;
	camera_ = camera;

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;

	// デスフラグの初期化
	isDead_ = true;

	lifeTime_ = 120;
}

void Bullet::Update() {

	if (isDead_ == false) {
		Move();
	}

	worldTransform_.scale_.x += 0.005f;
	worldTransform_.scale_.y += 0.005f;
	worldTransform_.scale_.z += 0.005f;
	kWidth += 0.005f;
	kHeight += 0.005f;

	// 寿命を減らす
	lifeTime_--;
	if (lifeTime_ <= 0.0f) {
		isDead_ = true;
	}

	// アフィン変換~Directxに転送
	WorldTransformUpdate(worldTransform_);

}

void Bullet::Draw() { modelBullet_->Draw(worldTransform_, *camera_); }

void Bullet::Move() { 
	// 移動
	worldTransform_.translation_ += velocity_;
}

Vector3 Bullet::GetWorldPosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos = worldTransform_.translation_;

	return worldPos;
}

AABB Bullet::GetAABB() {
	Vector3 worldPos = GetWorldPosition();

	AABB aabb;

	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};

	return aabb;
}

void Bullet::OnCollision(const Player* player) {
	(void)player; 
	isDead_ = true;
}

void Bullet::OnCollision(const MoveBlock* moveBlock) { 
	(void)moveBlock;
	isDead_ = true;
}

