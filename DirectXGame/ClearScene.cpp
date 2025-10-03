#include "Math.h"
#include "ClearScene.h"
#include <numbers>

void ClearScene::Initialize() {
	// カメラ初期化
	camera_.Initialize();

	// フェード
	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);

	clear_ = new Clear();
	clear_->Initialize(Model::CreateFromOBJ("clear"), &camera_, {-2.0f, -10.0f, 0.0f});
}

void ClearScene::Update() {

	switch (phase_) {
	case Phase::kFadeIn:
		fade_->Update();

		if (fade_->IsFinished()) {
			phase_ = Phase::kMain;
		}
		break;
	case Phase::kMain:
		if (Input::GetInstance()->PushKey(DIK_SPACE)) {
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

	clear_->Update();

	camera_.TransferMatrix();

	// フェード更新
	fade_->Update();

}

void ClearScene::Draw() {
	DirectXCommon* dxCommon_ = DirectXCommon::GetInstance();
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	Model::PreDraw(commandList);

	clear_->Draw();

	Model::PostDraw();

	// フェードの描画
	fade_->Draw();
}

ClearScene::~ClearScene() {


	// フェードの解放
	if (fade_) {
		delete fade_;
		fade_ = nullptr;
	}
	// タイトルの解放
	if (clear_) {
		delete clear_;
		clear_ = nullptr;
	}
}