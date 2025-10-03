#pragma once
#include "KamataEngine.h"
#include "Math.h"

using namespace KamataEngine;

class MapChipField;
class Player;
class MoveBlock;

class Bullet {
public:
	

	uint32_t coolTime_ = 0;

	// 速度
	Vector3 velocity_ = {};
	// デスフラグ
	bool isDead_ = true;

	// 初期化
	void Initialize(Model* model, Camera* camera, const Vector3& position);
	// 更新
	void Update();
	// 描画
	void Draw();
	// 座標を渡す
	const WorldTransform& GetWorldTransform() const { return worldTransform_; }
	// 速度加算
	const Vector3& GetVelocity()  { return velocity_; }
	// ワールド座標を取得const
	Vector3 GetWorldPosition();
	AABB GetAABB();
	// 衝突応答
	void OnCollision(const Player* player);
	void OnCollision(const MoveBlock* moveBlock);
	// 生存フラグのgetter
	bool IsAlive() const { return isDead_; }

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* modelBullet_ = nullptr;
	// カメラ
	Camera* camera_ = nullptr;
	// 設置状態フラグ
	bool onGround_ = true;
	// キャラクターの当たり判定サイズ
	static inline float kWidth = 1.0f;
	static inline float kHeight = 1.0f;
	static inline const float kBlank = 0.04f;

	// 寿命
	uint32_t lifeTime_ = 120;

	// 攻撃ギミックの経過時間カウンター
	uint32_t attackParameter_ = 0;
	// 予備動作の時間
	static inline const uint32_t kAnticipationTime = 8;

	// 移動
	void Move();


};
