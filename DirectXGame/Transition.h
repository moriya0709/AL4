#pragma once
#include "Easing.h"
#include "KamataEngine.h"

using namespace KamataEngine;

class Transition {
public:
	// トランジションを行うかどうか
	bool isTransition_ = false;
	// イージング
	const int kMaxEasing = 2;
	EasingSet transition_[2];

	~Transition();
	// 初期化
	void Initialize();
	// 更新
	void Update();
	// 描画
	void Draw();

	// リセット
	void Play();

private:
	// スプライト
	Sprite* spriteBlack_ = nullptr;
	Sprite* spriteWhite_ = nullptr;
	// テクスチャ
	int blackTex_ = 0;
	int whiteTex_ = 0;

	// イージング
	Easing* easing_;
};
