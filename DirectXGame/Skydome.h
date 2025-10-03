#pragma once
#include "KamataEngine.h"

using namespace KamataEngine;

class Skydome {
private:
	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
	// カメラ
	Camera* camera_ = nullptr;

public:
	// 初期化
	void Initialize(Model* model,Camera* camera);
	// 更新
	void Update();
	// 描画
	void Draw();
};