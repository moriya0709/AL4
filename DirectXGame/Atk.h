#pragma once
#include "KamataEngine.h"
#include "Math.h"

using namespace KamataEngine;

class Enemy;
class Player;

class Atk {
public:

	void Initialize(Camera* camera);
	void Update(Player* player);
	void Draw();
	
	// ワールド座標を取得
	Vector3 GetWorldPosition();
	// AABBを取得
	AABB GetAABB_Normal();
	AABB GetAABB_Air();
	// 衝突応答
	void OnCollision(const Enemy* enemy);

private:
	// モデル
	Model* model_ = nullptr;
	// カメラ
	Camera* camera_ = nullptr;
	// ワールド変換データ
	WorldTransform worldTransform_;

	Player* player_ = nullptr;


	// 攻撃の座標調整値(Normal時)
	static inline const float kOffsetX = 0.5f;
	static inline const float kOffsetY = 1.0f;

	// 当たり判定サイズ(Normal時)
	static inline const float kNormalWidth = 3.0f;
	static inline const float kNormalHeight = 3.0f;
	// 当たり判定サイズ(Air時)
	static inline const float kAirWidth = 3.0f;
	static inline const float kAirHeight = 3.0f;
};
