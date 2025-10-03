#pragma once
#include "KamataEngine.h"
#include "Fade.h"
#include "Clear.h"

class ClearScene {
public:
	// シーンのフェーズ
	enum class Phase {
		kFadeIn,  // フェードイン
		kMain,    // メイン部
		kFadeOut, // フェードアウト
	};

	// 初期化
	void Initialize();
	// 更新
	void Update();
	// 描画
	void Draw();
	// デストラクタ
	~ClearScene();
	// デスフラグのgetter
	bool IsFinished() const { return finished_; }

private:
	static inline const float kTimePlayerMove = 2.0f;

	Camera camera_;

	Clear* clear_ = nullptr;

	float counter_ = 0.0f;
	// 終了フラグ
	bool finished_ = false;

	Fade* fade_ = nullptr;
	// 現在のフェーズ
	Phase phase_ = Phase::kFadeIn;
};
