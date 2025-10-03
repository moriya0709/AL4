#include <numbers>
#include "TitleScene.h"
#include "Math.h"

void TitleScene::Initialize() {
	modelPlayer_ = Model::CreateFromOBJ("player");

	// カメラ初期化
	camera_.Initialize();

	const float kPlayerScale = 10.0f;

	worldTransformPlayer_.Initialize();

	worldTransformPlayer_.scale_ = {kPlayerScale, kPlayerScale, kPlayerScale};

	worldTransformPlayer_.rotation_.y = 0.95f * std::numbers::pi_v<float>;

	worldTransformPlayer_.translation_.x = -2.0f;

	worldTransformPlayer_.translation_.y = -10.0f;

	// フェード
	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);

	title_ = new Title();
	title_->Initialize(Model::CreateFromOBJ("title"), &camera_, {-2.0f, -10.0f, 0.0f});

}

void TitleScene::Update() {

	switch (phase_)
	{ 
	case Phase::kFadeIn:
		fade_->Update();

		if (fade_->IsFinished()) {
			phase_ = Phase::kMain;
		}
		break;
	case Phase::kMain:
		if (Input::GetInstance()->PushKey(DIK_SPACE)) 
		{
			fade_->Start(Fade::Status::FadeOut, 1.0f);
			phase_ = Phase::kFadeOut;
		}
		break;
	case Phase::kFadeOut:
		fade_->Update();
		if (fade_->IsFinished()) {
			finished_ = true;
		}
		break;
	}

	counter_ += 1.0f / 60.0f;
	counter_ = std::fmod(counter_, kTimePlayerMove);

	float angle = counter_ / kTimePlayerMove * 2.0f * std::numbers::pi_v<float>;

	worldTransformPlayer_.translation_.y = std::sin(angle) + 10.0f;

	camera_.TransferMatrix();

	// タイトル表示
	title_->Update();

	// フェード更新
	fade_->Update();

	// アフィン変換～DirectXに転送
	WorldTransformUpdate(worldTransformPlayer_);
}

void TitleScene::Draw() {
	DirectXCommon* dxCommon_ = DirectXCommon::GetInstance();
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	Model::PreDraw(commandList);

	modelPlayer_->Draw(worldTransformPlayer_, camera_);

	// タイトル表示
	title_->Draw();


	Model::PostDraw();

	// フェードの描画
	fade_->Draw();
}

TitleScene::~TitleScene() {
	// モデルの解放
	if (modelPlayer_) {
		delete modelPlayer_;
		modelPlayer_ = nullptr;
	}

	// フェードの解放
	if (fade_) {
		delete fade_;
		fade_ = nullptr;
	}

	// タイトルの解放
	if (title_) {
		delete title_;
		title_ = nullptr;
	}
}