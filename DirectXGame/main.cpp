#include "TitleScene.h"
#include "GameScene.h"
#include "ClearScene.h"
#include "KamataEngine.h"

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

void ChageScene()
{
	switch (scene)
	{ 
	case Scene::kTitle:
		if (titleScene->IsFinished()) {
			// シーン変更
			scene = Scene::kGame;
			// 旧シーンの解放
			delete titleScene;
			titleScene = nullptr;
			// 新シーンの生成と初期化
			gameScene = new GameScene;
			gameScene->Initialize();	
		}
		break;
	case Scene::kGame:
		if(gameScene->IsFinished()) {
			// シーン変更
			scene = Scene::kClear;
			// 旧シーンの解放
			delete gameScene;
			gameScene = nullptr;
			// 新シーンの生成と初期化
			clearScene = new ClearScene;
			clearScene->Initialize();
		}
		break;
	case Scene::kClear:
		if (clearScene->IsFinished()) {
			// シーン変更
			scene = Scene::kTitle;
			// 旧シーンの解放
			delete clearScene;
			clearScene = nullptr;
			// 新シーンの生成と初期化
			titleScene = new TitleScene;
			titleScene->Initialize();
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
	KamataEngine::Initialize(L"LE2C_モリヤ_ユウゴ_大砲君");

	// DirectXCommonインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// ImGuiManagerインスタンスの取得
	ImGuiManager* imguiManager = ImGuiManager::GetInstance();

	// タイトルシーンのインスタンス生成
	titleScene = new TitleScene();
	// ゲームシーン初期化
	titleScene->Initialize();

	// ゲームシーンのインスタンス生成
	gameScene = new GameScene();
	// ゲームシーン初期化
	gameScene->Initialize();

	// クリアシーンのインスタンス生成
	clearScene = new ClearScene();
	// クリアシーン初期化
	clearScene->Initialize();

	// メインループ
	while (true) {
		// エンジンの行進
		if (KamataEngine::Update()) {
			break;
		}

		// ImGui受付開始
		imguiManager->Begin();

		// シーン切り替え
		ChageScene();
		// 現在シーン更新
		UpdateScene();

		// ImGui受付終了
		imguiManager->End();

		// 描画開始
		dxCommon->PreDraw();

		// 現在シーンの描画
		DrawScene();

		// ImGuiの描画
		imguiManager->Draw();

		// 描画終了
		dxCommon->PostDraw();
	}

	// タイトルシーンの解放
	delete titleScene;
	// nullptrの代入
	titleScene = nullptr;

	// ゲームシーンの解放
	delete gameScene;
	// nullptrの代入
	gameScene = nullptr;

	// クリアシーンの解放
	delete clearScene;
	// nullptrの代入
	clearScene = nullptr;

	// エンジンの終了処理
	KamataEngine::Finalize();
}
