#pragma once
#include "KamataEngine.h"
#include "Math.h"

using namespace KamataEngine;

class Player;
class Bullet;

class MoveBlock {
public:
	// 弾が当たった時
	bool isStop = false;

	//　初期化
	void Initialize(Model* model, Camera* camera, const Vector3& position);
	// 更新
	void Update();
	// 描画
	void Draw();

	// ワールド座標を取得const
	Matrix4x4 GetWorldMatrix() const;
	AABB GetAABB();
	// 衝突応答
	void OnCollision(const Player* player);
	void OnCollision(const Bullet* bullet);

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* modelMoveBlock_ = nullptr;
	// カメラ
	Camera* camera_ = nullptr;
	// 速度
	Vector3 velocity_ = {};
	// 動きが止まる時間
	uint32_t stopTime_ = 180;

	// 当たり判定サイズ
	static inline const float kWidth = 3.8f;
	static inline const float kHeight = 0.8f;
	static inline const float kBlank = 0.04f;

};
