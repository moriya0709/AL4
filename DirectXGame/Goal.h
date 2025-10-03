#pragma once
#include "KamataEngine.h"
#include "Math.h"

using namespace KamataEngine;
class Player;

class Goal {
public:
	// 初期化
	void Initialize(Model* model, Camera* camera, const Vector3& position);
	// 更新
	void Update();
	// 描画
	void Draw();
	// ワールド座標を取得const
	Vector3 GetWorldPosition();
	AABB GetAABB();

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* modelGoal_ = nullptr;
	// カメラ
	Camera* camera_ = nullptr;

	// 当たり判定サイズ
	static inline float kWidth = 1.0f;
	static inline float kHeight = 1.0f;
	static inline const float kBlank = 0.04f;

};
