#pragma once

#include "KamataEngine.h"
#include "Math.h"

class GameScene;
class Player;

class Enemy {
public:
	
	// 振る舞い
	enum class Behavior {
		kUnknown = -1,
		kRoot,   // 通常状態
		kDead, // デス状態
	};

	// デスフラグ
	bool isDead_ = false;

	// 初期化
	void Initialize(Model* model, Camera* camera, const Vector3& position, GameScene* gameScene);
	// 更新
	void Update();
	// 描画
	void Draw();
	// ワールド座標を取得
	Vector3 GetWorldPosition();
	// AABBを取得
	AABB GetAABB();
	// 衝突応答
	void OnCollision(const Player* player);
	// 通常行動更新
	void BehaviorRootUpdate();
	// デス更新
	void BehaviorDeadUpdate();
	// 当たり判定無効フラグ
	bool IsCollisionDisabled() const { return isCollisionDisabled_; }

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
	// カメラ
	Camera* camera_ = nullptr;
	// 歩行の速さ
	static inline const float kWalkSpeed = 0.02f;
	// 速度
	Vector3 velocity_ = {};

	// 最初の角度[度]
	static inline const float kWalkMotionAngleStart = 0.0f;
	// 最後の角度[度]
	static inline const float kWalkMotionAngleEnd = 30.0f;
	// アニメーションの同期となる時間[秒]
	static inline const float kWalkMotionTime = 1.0f;
	// 経過時間
	float walkTimer_ = 0.0f;

	// 当たり判定サイズ
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;

	// 振るまい
	Behavior behavior_ = Behavior::kRoot;
	// 次の振るまいリクエスト
	Behavior behaviorRequest_ = Behavior::kUnknown;

	// デス演出のアニメーションの経過時間
	float deadTimer_ = 0.0f;

	// 当たり判定無効フラグ
	bool isCollisionDisabled_ = false;

	// ゲームシーン
	GameScene* gameScene_ = nullptr;
};
