#include "Math.h"
#include "ClearScene.h"
#include <numbers>

void ClearScene::Initialize() {
	// カメラ初期化
	camera_.Initialize();


	clear_ = new Clear();
	clear_->Initialize(Model::CreateFromOBJ("clear"), &camera_, {-2.0f, -10.0f, 0.0f});
}

void ClearScene::Update() {

	clear_->Update();

	camera_.TransferMatrix();

	if (Input::GetInstance()->PushKey(DIK_SPACE)) {
		finished_ = true;
	}
}

void ClearScene::Draw() {
	DirectXCommon* dxCommon_ = DirectXCommon::GetInstance();
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	Model::PreDraw(commandList);

	clear_->Draw();

	Model::PostDraw();

	
}

ClearScene::~ClearScene() {	
	// タイトルの解放
	if (clear_) {
		delete clear_;
		clear_ = nullptr;
	}
}