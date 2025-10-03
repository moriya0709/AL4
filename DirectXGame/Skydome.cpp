#include "Skydome.h"
#include "assert.h"

using namespace KamataEngine;

void Skydome::Initialize(Model* model, Camera* camera) {
	// NULLポインタチェック
	assert(model);
	// 引数の内容をメンバ変数に記録
	model_ = model;
	camera_ = camera;

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
}

void Skydome::Update() {
	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();
}

void Skydome::Draw() {
	// 3Dモデル描画
	model_->Draw(worldTransform_, *camera_);
}
