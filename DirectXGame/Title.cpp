#include "Title.h"

void Title::Initialize(Model* model, Camera* camera, const Vector3& position) {
	modelTitle_ = model;
	camera_ = camera;

	logoUi_.worldTransform.Initialize();
	logoUi_.worldTransform.translation_ = position;
	logoUi_.worldTransform.scale_ = {30.0f,30.0f,30.0f};
	logoUi_.worldTransform.rotation_.z = 10.0f;

	logoUi_.startPos = {logoUi_.worldTransform.translation_};
	logoUi_.endPos = {
		logoUi_.worldTransform.translation_.x, 
		logoUi_.worldTransform.translation_.y + 10.0f,
		logoUi_.worldTransform.translation_.z
	};

	logoUi_.moveTime = 0.0f;
	logoUi_.moveEasedT = 0.0f;

	// イージング初期化
	easing_ = new Easing();
	easing_->Initialize();
}

void Title::Update() {

	easing_->Update();

	if (logoUi_.moveTime >= 1.0f) {
		logoUi_.moveTime = 0.0f;
	}

	// イージング処理
	easing_->Move(logoUi_, 0.01f, 3);

	// アフィン変換～DirectXに転送
	WorldTransformUpdate(logoUi_.worldTransform);
}

void Title::Draw() {
	easing_->Draw();

	modelTitle_->Draw(logoUi_.worldTransform, *camera_); 
}
