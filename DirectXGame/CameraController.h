#pragma once
#include "KamataEngine.h"

class Player;
using namespace KamataEngine;

class CameraController {
public:
	// 矩形
	struct Rect {
		float left = 0.0f;
		float right = 1.0f;
		float bottom = 0.0f;
		float top = 1.0f;
	};

	// 初期化
	void Initialize(Camera* camera);
	// 更新
	void Updata();
	// セット
	void SetTarget(Player* target) { target_ = target; }
	// リセット
	void Reset();
	// カメラ移動範囲
	void SetMovableArea(Rect area) { movableArea_ = area; }

private:
	// カメラ
	Camera* camera_ = nullptr;
	// 追従対象
	Player* target_ = nullptr;
	// 追従対象とカメラの座標の差（オフセット）
	Vector3 targetOffset_ = {0, 0, -13.0f};
	// カメラ移動範囲
	Rect movableArea_ = {0, 100, 0, 100};
	// 目標座標
	Vector3 destination_;
	// 座標補間割合
	static inline const float kInterpolationRate = 0.1f;
	// 速度掛け率
	static inline const float kVelocityBias = 30.0f;
	// 追従対象の書く方向へのカメラ移動範囲
	static inline const Rect targetMargin = {-9.0f, 9.0f, -5.0f, 5.0f};
};
