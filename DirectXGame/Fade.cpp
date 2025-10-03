#include <algorithm>
#include "Fade.h"

void Fade::Initialize() {
	// スプライトの生成
	sprite_ = Sprite::Create(0, Vector2{});

	sprite_->SetSize(Vector2(WinApp::kWindowWidth, WinApp::kWindowHeight));
	sprite_->SetColor(Vector4(0, 0, 0, 1.0f));

}

void Fade::Update() {
	// フェード状態による分岐
	switch (status_) { 
	case Status::None:
	// 何もない
		break;
	case Status::FadeIn:
	    // 1フラーム分の秒数をカウントアップ
		counter_ += 1.0f / 60.0f;
		// フェード継続時間に達したら打ち止め
		if (counter_ >= duration_) {
			counter_ = duration_;
		}
		// 0.0fから1.0fの間で、経過時間がフェード継続時間に近づくほどアルファ値を大きくする
		sprite_->SetColor(Vector4(0, 0, 0, std::clamp(1.0f - counter_ / duration_, 0.0f, 1.0f)));

		break;
	case Status::FadeOut:
	    // １フレーム分に秒数をカウントアップ
		counter_ += 1.0f / 60.0f;
	    // フェード継続時間に達したら打ち止め
		if (counter_ >= duration_) {
			counter_ = duration_;
		}
		// 0.0fから1.0fの間で、経過時間がフェード継続時間に近づくほどアルファ値を大きくする
		sprite_->SetColor(Vector4(0, 0, 0, std::clamp(counter_ / duration_, 0.0f, 1.0f)));

		break;
	}
}

void Fade::Draw() {
	// フェード状態がNoneなら描画しない
	if (status_ == Status::None) {
		return;
	}

	// スプライトの描画
	Sprite::PreDraw(DirectXCommon::GetInstance()->GetCommandList());

	sprite_->Draw();

	Sprite::PostDraw();
}

void Fade::Start(Status status, float duration)
{ 
	status_ = status;
	duration_ = duration;
	counter_ = 0.0f;
}

void Fade::Stop() {
	status_ = Status::None;
	counter_ = 0.0f;
	sprite_->SetColor(Vector4(0, 0, 0, 1.0f));
}

bool Fade::IsFinished() const {
	// フェード状態による分岐
	switch (status_)
	{ 
	case Status::FadeIn:
	case Status::FadeOut:
		if (counter_ >= duration_) {
			return true;
		} else {
			return false;
		}
	}

	return true;
}