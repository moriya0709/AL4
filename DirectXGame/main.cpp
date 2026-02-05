#include "KamataEngine.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "ClearScene.h"
#include "Transition.h"


using namespace KamataEngine;

// シーン
enum class Scene {
	kUnknown = 0,
	kTitle, // タイトルシーン
	kGame,  // ゲームシーン
	kClear, // クリアシーン
};
// 現在のシーン
Scene scene = Scene::kTitle;

// タイトルシーンのインスタンス生成
TitleScene* titleScene = nullptr;
// ゲームシーンのインスタンス生成
GameScene* gameScene = nullptr;
// クリアシーンのインスタンス生成
ClearScene* clearScene = nullptr;
// トランジション
Transition* transition = nullptr;

// シーン切り換えまでの時間
int changeTime = 0;

void ChangeScene()
{
	if (changeTime > 0) {
		changeTime--;
	} else {
		switch (scene) {
		case Scene::kTitle:
			if (titleScene->IsFinished()) {
				if (!transition->isTransition_) {
					transition->Play();
				}

				if (transition->transition_[1].sizeTime >= 0.5f) {
					// シーン変更
					scene = Scene::kGame;
					// 新シーンの生成と初期化
					gameScene = new GameScene;
					gameScene->Initialize(titleScene->selectStage);
					// 旧シーンの解放
					delete titleScene;
					titleScene = nullptr;
					changeTime = 60;
				}
			}
			break;
		case Scene::kGame:
			if (gameScene->IsFinished()) {
				if (!transition->isTransition_) {
					transition->Play();
				}

				if (transition->transition_[1].sizeTime >= 0.5f) {
					// シーン変更
					scene = Scene::kClear;
					// 旧シーンの解放
					delete gameScene;
					gameScene = nullptr;
					// 新シーンの生成と初期化
					clearScene = new ClearScene;
					clearScene->Initialize();
					changeTime = 60;
				}
			}
			break;
		case Scene::kClear:
			if (clearScene->IsFinished()) {
				if (!transition->isTransition_) {
					transition->Play();
				}

				if (transition->transition_[1].sizeTime >= 0.5f) {
					// シーン変更
					scene = Scene::kTitle;
					// 旧シーンの解放
					delete clearScene;
					clearScene = nullptr;
					// 新シーンの生成と初期化
					titleScene = new TitleScene;
					titleScene->Initialize();
					changeTime = 60;
				}
			}
		}
	}

}

void UpdateScene() {
	switch (scene)
	{ 
	case Scene::kTitle:
		titleScene->Update();
		break;
	case Scene::kGame:
		gameScene->Update();
		break;
	case Scene::kClear:
		clearScene->Update();
		break;
	}
}

void DrawScene() {
	switch (scene)
	{ 
	case Scene::kTitle:
		titleScene->Draw();
		break;
	case Scene::kGame:
		gameScene->Draw();
		break;
	case Scene::kClear:
		clearScene->Draw();
		break;
	}
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	// エンジンの初期化
	KamataEngine::Initialize(L"LE2D_22_モリヤ_ユウゴ_大砲君");

	// DirectXCommonインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// ImGuiManagerインスタンスの取得
	ImGuiManager* imguiManager = ImGuiManager::GetInstance();

	// タイトルシーン
	titleScene = new TitleScene();
	titleScene->Initialize();
	// ゲームシーン
	gameScene = new GameScene();
	gameScene->Initialize(titleScene->selectStage);
	// クリアシーン
	clearScene = new ClearScene();
	clearScene->Initialize();
	// トランジション
	transition = new Transition();
	transition->Initialize();


	// BGM
	// サウンドデータの読み込み
	uint32_t bgmData_ = Audio::GetInstance()->LoadWave("bgm/title.wav");
	// BGM音量設定
	float bgmVolume_ = 0.3f;
	// BGM再生
	uint32_t bgmHandle_;
	bgmHandle_ = Audio::GetInstance()->PlayWave(bgmData_, true, bgmVolume_);


	// メインループ
	while (true) {
		// エンジンの行進
		if (KamataEngine::Update()) {
			break;
		}

		// ImGui受付開始
		imguiManager->Begin();

		// シーン切り替え
		ChangeScene();
		// 現在シーン更新
		UpdateScene();
	
		// トランジション
		transition->Update();

		// ImGui受付終了
		imguiManager->End();

		// 描画開始
		dxCommon->PreDraw();

		// 現在シーンの描画
		DrawScene();

		// トランジション
		transition->Draw();

		// ImGuiの描画
		imguiManager->Draw();

		// 描画終了
		dxCommon->PostDraw();
	}

	// タイトルシーンの解放
	delete titleScene;
	titleScene = nullptr;

	// ゲームシーンの解放
	delete gameScene;
	gameScene = nullptr;

	// クリアシーンの解放
	delete clearScene;
	clearScene = nullptr;

	// トランジションの解放
	delete transition;
	transition = nullptr;

	// エンジンの終了処理
	KamataEngine::Finalize();
}
