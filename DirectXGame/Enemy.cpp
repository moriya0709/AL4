#include <cassert>
#include <numbers>
#include "Enemy.h"
#include "Math.h"
#include "Player.h"
#include "GameScene.h"

void Enemy::Initialize(Model* model, Camera* camera, const Vector3& position,GameScene* gameScene) {
	
	// NULLチェック
	assert(model);

	// モデル
	model_ = model;
	// カメラ
	camera_ = camera;
	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	// 角度調整
	worldTransform_.rotation_.y = std::numbers::pi_v<float> * 3.0f / 2.0f;

	// 速度
	velocity_ = {-kWalkSpeed, 0, 0};
	// 経過時間
	walkTimer_ = 0.0f;

	// ゲームシーン
	gameScene_ = gameScene;
};

void Enemy::OnCollision(const Player* player)
{
	(void)player;

	if (behavior_ == Behavior::kDead) {
		// 敵がやられているなら何もしない
		return;
	}

	
	
}

void Enemy::Update() {
	behavior_ = behaviorRequest_;

	switch (behavior_) {
	case Behavior::kRoot:
	default:
		BehaviorRootUpdate();
		break;
	case Behavior::kDead:
		BehaviorDeadUpdate();
		break;
	}

	// ワールド行列更新
	WorldTransformUpdate(worldTransform_);
}

void Enemy::Draw() {
	// モデル描画
	model_->Draw(worldTransform_, *camera_);
}

Vector3 Enemy::GetWorldPosition() {

	Vector3 worldPos;

	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

AABB Enemy::GetAABB() { 
	
	Vector3 worldPos = GetWorldPosition();

	AABB aabb;

	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};

	return aabb;
}

void Enemy::BehaviorRootUpdate() {
	// 移動
	worldTransform_.translation_ += velocity_;

	// タイマーを加算
	walkTimer_ += 1.0f / 60.0f;

	// 回転アニメーション
	worldTransform_.rotation_.x = std::sin(std::numbers::pi_v<float> * 2.0f * walkTimer_ / kWalkMotionTime);
}

void Enemy::BehaviorDeadUpdate() {
	// タイマーを加算
	deadTimer_ += 0.01f;

	// 回転アニメーション
	worldTransform_.rotation_.x = EaseOut(0.0f, -1.0f, deadTimer_);
	worldTransform_.rotation_.y = EaseOut(0.0f, 10.0f, deadTimer_);
	
	if (deadTimer_ >= 1.0f) {
		// 敵の振る舞いをデス演出に変更
		behaviorRequest_ = Behavior::kDead;
		isDead_ = true;
	}
}
