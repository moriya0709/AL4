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

	// ステージセレクトUI
	for (int i = 0; i < kSelect; i++) {
		selectUi[i].worldTransform.Initialize();
		selectUi[i].worldTransform.translation_ = {5.0f, float(i - 1), -50.0f};
		selectUi[i].worldTransform.rotation_ = {0.0f, 0.5f, 0.0f};
		selectUi[i].worldTransform.scale_ = {1.0f, 1.0f, 1.0f};
		selectUi[i].startPos = selectUi[i].worldTransform.translation_;
		selectUi[i].endPos = {1.5f, float(-i + 1), -45.0f};
		selectUi[i].startSize = selectUi[i].worldTransform.scale_;
		selectUi[i].endSize = selectUi[i].worldTransform.scale_;
		selectUi[i].moveTime = 0.0f;
		selectUi[i].moveEasedT = 0.0f;
		selectUi[i].sizeEasedT = 0.0f;
		selectUi[i].sizeTime = 0.0f;
		selectUiModel_[i] = Model::CreateFromOBJ("selectUi");
	}
	selectUi[0].endSize = selectUi[0].worldTransform.scale_ + Vector3(0.3f, 0.3f, 0.0f);
	// 選択中のステージ
	selectStage = 1;

	// テクスチャ
	selectUiTex[0] = TextureManager::Load("selectUi/stage1.png");
	selectUiTex[1] = TextureManager::Load("selectUi/stage2.png");

	// フェード
	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);

	// イージング
	easing_ = new Easing();
	easing_->Initialize();

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
		if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
			// セレクト演出
			if (isSelect) {
				if (selectUi[1].moveTime >= 1.0f) {
					for (int i = 0; i < kSelect; i++) {
						selectUi[i].moveEasedT = 0.0f;
						selectUi[i].moveTime = 0.0f;
						selectUi[i].startPos = selectUi[i].worldTransform.translation_;
						selectUi[i].endPos = {5.0f, float(i - 1), -50.0f};
					}
				}
			} else {
				isSelect = true;
			}
		}

		// シーン切り換え演出
		if (isSelect) {
			if (selectUi[0].worldTransform.translation_.x != 1.5f) {
				if (selectUi[1].moveTime >= 1.0f) {
					finished_ = true;
				}
			}
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

		// ステージセレクト
	if (isSelect) {
		if (selectUi[1].moveTime >= 1.0f) {
			if (Input::GetInstance()->PushKey(DIK_W)) {
				if (selectUi[0].worldTransform.translation_.y != 1.0f) {
					for (int i = 0; i < kSelect; i++) {
						selectUi[i].moveEasedT = 0.0f;
						selectUi[i].moveTime = 0.0f;
						selectUi[i].startPos = selectUi[i].worldTransform.translation_;
						selectUi[i].endPos = {1.5f, selectUi[i].worldTransform.translation_.y - 1.0f, -45.0f};

						// 選択ステージのUI強調化
						if (selectUi[i].worldTransform.translation_.y == 1.0f) {
							selectUi[i].startSize = selectUi[i].worldTransform.scale_;
							selectUi[i].endSize = selectUi[i].worldTransform.scale_ + Vector3(-0.3f, -0.3f, 0.0f);
							selectUi[i].sizeEasedT = 0.0f;
							selectUi[i].sizeTime = 0.0f;
						}

						if (selectUi[i].worldTransform.translation_.y == 2.0f) {
							selectUi[i].startSize = selectUi[i].worldTransform.scale_;
							selectUi[i].endSize = selectUi[i].worldTransform.scale_ + Vector3(0.3f, 0.3f, 0.0f);
							selectUi[i].sizeEasedT = 0.0f;
							selectUi[i].sizeTime = 0.0f;
						}
					}
					selectStage--;
				}
			}
			if (Input::GetInstance()->PushKey(DIK_S)) {
				if (selectUi[1].worldTransform.translation_.y != 1.0f) {
					for (int i = 0; i < kSelect; i++) {
						selectUi[i].moveEasedT = 0.0f;
						selectUi[i].moveTime = 0.0f;
						selectUi[i].startPos = selectUi[i].worldTransform.translation_;
						selectUi[i].endPos = {1.5f, selectUi[i].worldTransform.translation_.y + 1.0f, -45.0f};

						// 選択ステージのUI強調化
						if (selectUi[i].worldTransform.translation_.y == 1.0f) {
							selectUi[i].startSize = selectUi[i].worldTransform.scale_;
							selectUi[i].endSize = selectUi[i].worldTransform.scale_ + Vector3(-0.3f, -0.3f, 0.0f);
							selectUi[i].sizeEasedT = 0.0f;
							selectUi[i].sizeTime = 0.0f;
						}

						if (selectUi[i].worldTransform.translation_.y == 0.0f) {
							selectUi[i].startSize = selectUi[i].worldTransform.scale_;
							selectUi[i].endSize = selectUi[i].worldTransform.scale_ + Vector3(0.3f, 0.3f, 0.0f);
							selectUi[i].sizeEasedT = 0.0f;
							selectUi[i].sizeTime = 0.0f;
						}
					}
					selectStage++;
				}
			}
		}

		for (int i = 0; i < kSelect; i++) {
			// イージング
			if (selectUi[i - 1].moveTime >= 0.2f || i == 0) {
				easing_->Move(selectUi[i], 0.03f, 2);
			}
			easing_->Size(selectUi[i], 0.03f, 2);
		}
	}

	// タイトル表示
	title_->Update();

	// フェード更新
	fade_->Update();

	// アフィン変換～DirectXに転送
	WorldTransformUpdate(worldTransformPlayer_);
	for (int i = 0; i < kSelect; i++)
		WorldTransformUpdate(selectUi[i].worldTransform);
}

void TitleScene::Draw() {
	DirectXCommon* dxCommon_ = DirectXCommon::GetInstance();
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	Model::PreDraw(commandList);

	modelPlayer_->Draw(worldTransformPlayer_, camera_);

	// タイトル表示
	title_->Draw();

	// ステージセレクトUI
	if (isSelect) {
		for (int i = 0; i < kSelect; i++) {
			selectUiModel_[i]->Draw(selectUi[i].worldTransform, camera_, selectUiTex[i]);
		}
	}


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