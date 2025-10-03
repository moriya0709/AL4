#pragma once
#include "KamataEngine.h"

using namespace KamataEngine;

class Fade {
public:
	// フェードの状態
	enum class Status {
		None,    // フェードなし
		FadeIn,  // フェードイン中
		FadeOut, // フェードアウト中
	};

	// 初期化
	void Initialize();
	// 更新	
	void Update();
	// 描画
	void Draw();
	// フェードの開始
	void Start(Status status, float duration);
	// フェードの停止
	void Stop();
	// フェードの終了判定
	bool IsFinished() const;

private:
	
	// 現在のフェードの状態
	Status status_ = Status::None;

	// スプライト
	Sprite* sprite_ = nullptr;

	// フェードの継続時間	
	float duration_ = 0.0f;
	// 経過時間カウンター
	float counter_ = 0.0f;

	
};
