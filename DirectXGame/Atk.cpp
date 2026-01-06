#include "Atk.h"
#include "Player.h"

void Atk::Initialize(Camera* camera) {
	// カメラ
	camera_ = camera;

	// ワールド変換の初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = {0.0f, 0.0f, -1.0f};
	worldTransform_.scale_ = {3.0f, 3.0f, 1.0f};

	// モデルの生成
	model_ = Model::CreateFromOBJ("atk");



}

void Atk::Update(Player* player) {
	// ローカル変数に代入
	player_ = player;

	if (player_->behavior_ == Player::Behavior::kAttack) {
		worldTransform_.translation_ = player_->GetWorldPosition();

		// 攻撃の座標調整
		if (player_->attackTypes_ == Player::Normal) {
			if (player_->lrDirection_ == Player::LRDirection::kRight) {
				worldTransform_.translation_.x += kOffsetX;
				worldTransform_.translation_.y += kOffsetY;
			} else {
				worldTransform_.translation_.x -= kOffsetX;
				worldTransform_.translation_.y += kOffsetY;
			}

			// 範囲の表示サイズ
			worldTransform_.scale_ = {3.0f, 3.0f, 1.0f};
		} else {
			worldTransform_.scale_ = {5.0f, 5.0f, 1.0f};
		}
	}


	// ワールド変換の更新
	WorldTransformUpdate(worldTransform_); 
}

void Atk::Draw() { 
	//if (player_->behavior_ == Player::Behavior::kAttack) {
	//	model_->Draw(worldTransform_, *camera_);
	//}
}

// ワールド座標を取得
Vector3 Atk::GetWorldPosition() {
	Vector3 worldPos;

	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

// AABBを取得
AABB Atk::GetAABB_Normal() {
	Vector3 worldPos = GetWorldPosition();

	AABB aabb;

	aabb.min = {worldPos.x - kNormalWidth / 2.0f, worldPos.y - kNormalHeight / 2.0f, worldPos.z - kNormalWidth / 2.0f};
	aabb.max = {worldPos.x + kNormalWidth / 2.0f, worldPos.y + kNormalHeight / 2.0f, worldPos.z + kNormalWidth / 2.0f};

	return aabb;
}
AABB Atk::GetAABB_Air() {
	Vector3 worldPos = GetWorldPosition();

	AABB aabb;

	aabb.min = {worldPos.x - kAirWidth / 2.0f, worldPos.y - kAirHeight / 2.0f, worldPos.z - kAirWidth / 2.0f};
	aabb.max = {worldPos.x + kAirWidth / 2.0f, worldPos.y + kAirHeight / 2.0f, worldPos.z + kAirWidth / 2.0f};

	return aabb;
}

// 衝突応答
void Atk::OnCollision(const Enemy* enemy) {
	(void)enemy;
}
