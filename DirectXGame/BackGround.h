#pragma once
#include "KamataEngine.h"
#include "Math.h"

using namespace KamataEngine;

class BackGround {
public:
	// 初期化
	void Initialize(Model* model, Camera* camera, const Vector3& position);
	// 更新
	void Update();
	// 描画
	void Draw();

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* modelBackGround_ = nullptr;
	// カメラ
	Camera* camera_ = nullptr;
};
