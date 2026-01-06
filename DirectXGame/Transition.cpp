#include "Transition.h"

Transition::~Transition() {
	delete spriteBlack_;
	delete spriteWhite_;
	delete easing_;
}

void Transition::Initialize() {

	// transition_初期化
	for (int i = 0; i < kMaxEasing; i++) {
		transition_[i].pos = {0.0f, 360.0f};
		transition_[i].size = {1280.0f, 0.0f};
		transition_[i].startPosV2 = {transition_[i].pos};
		transition_[i].endPosV2 = {transition_[i].endPosV2.x - transition_[i].endSizeV2.x / 2.0f, transition_[i].endPosV2.y - transition_[i].endSizeV2.y / 2.0f};
		transition_[i].startSizeV2 = {transition_[i].size};
		transition_[i].endSizeV2 = {1280.0f, 720.0f};
		transition_[i].moveTime = 0.0f;
		transition_[i].sizeTime = 0.0f;
		transition_[i].moveEasedT = 0.0f;
		transition_[i].sizeEasedT = 0.0f;
	}

	// テクスチャ読み込み
	blackTex_ = TextureManager::Load("transitionBlack/transitionBlack.png"); // 黒い幕
	whiteTex_ = TextureManager::Load("transitionWhite/transitionWhite.png"); // 白い幕
	// スプライト生成
	spriteBlack_ = Sprite::Create(blackTex_, transition_[0].pos); // 黒い幕
	spriteWhite_ = Sprite::Create(whiteTex_, transition_[1].pos); // 白い幕

	// トランジションフラグ初期化
	isTransition_ = false;

	// イージング初期化
	easing_ = new Easing();
	easing_->Initialize();
}

void Transition::Update() {
	// イージング
	if (isTransition_) {
		easing_->SizeV2(transition_[0], 0.015f, 4);
		easing_->MoveV2(transition_[0], 0.015f, 4);

		if (transition_[0].sizeTime >= 0.2f) {
			easing_->SizeV2(transition_[1], 0.015f, 4);
			easing_->MoveV2(transition_[1], 0.015f, 4);
		}
	}

	easing_->Update();

	// トランジション終了判定
	if (transition_[1].sizeTime >= 1.0f) {
		isTransition_ = false;
	}

	// 座標更新
	spriteBlack_->SetPosition(transition_[0].pos);
	spriteWhite_->SetPosition(transition_[1].pos);
	// サイズ更新
	spriteBlack_->SetSize(transition_[0].size);
	spriteWhite_->SetSize(transition_[1].size);
}

void Transition::Draw() {
	DirectXCommon* dxCommon_ = DirectXCommon::GetInstance();

	// スプライト描画前処理
	Sprite::PreDraw(dxCommon_->GetCommandList());

	// トランジション描画
	spriteBlack_->Draw();
	spriteWhite_->Draw();

	// スプライト描画後処理
	Sprite::PostDraw();
}

void Transition::Play() {
	// トランジションリセット
	for (int i = 0; i < kMaxEasing; i++) {
		transition_[i].moveTime = 0.0f;
		transition_[i].moveEasedT = 0.0f;
		transition_[i].sizeTime = 0.0f;
		transition_[i].sizeEasedT = 0.0f;
	}
	isTransition_ = true;
}
