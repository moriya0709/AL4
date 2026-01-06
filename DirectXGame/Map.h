#pragma once
#include "KamataEngine.h"
#include "Math.h"

using namespace KamataEngine;

class Player;


class Map {
public:

	std::vector<AABB> tiles; // 各タイルのAABB
	
	// 初期化
	void Initialize(Camera* camera);
	// 更新
	void Update();
	// 描画
	void Draw();

	// ワールド座標を取得
	Vector3 GetWorldPosition();
	// ワールドスケールを取得
	Vector3 GetWorldScale();
	// AABBを取得
	AABB GetAABB();
	// 衝突応答
	void OnCollision(const Player* player);

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* modelGround_ = nullptr;
	// カメラ
	Camera* camera_ = nullptr;

	// キャラクターの当たり判定サイズ
	static inline const float kWidth = 24.5f;
	static inline const float kHeight = 3.0f;

};
