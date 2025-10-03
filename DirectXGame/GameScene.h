#pragma once
#include "KamataEngine.h"
#include "Player.h"
#include "Skydome.h"
#include "MapChipField.h"
#include "CameraController.h"
#include "Enemy.h"
#include "DeathParticles.h"
#include "Math.h"
#include "Bullet.h"
#include "MoveBlock.h"
#include "Goal.h"
#include "Fade.h"
#include "BackGround.h"

using namespace KamataEngine;
// ゲームシーン
class GameScene {
public:

	// 初期化
	void Initialize();
	// 更新
	void Update();
	// 描画
	void Draw();
	// デストラクタ
	~GameScene();

	// 表示ブロックの生成
	void GenerateBlocks();
	// 全ての当たり判定を行う
	void CheckAllCollisions();
	// フェーズの切り替え
	void ChangePhase();
	// デスフラグのgetter
	bool IsFinished() const { return finished_; }

private:
	// シーンのフェーズ
	enum class Phase
	{
		kFadeIn, // フェードイン
		kPlay,  // ゲームプレイ
		kDeath, // デス演出
		kFadeOut, // フェードアウト
	};
	// ゲームの現在フェーズ
	Phase phase_;
	// フェード
	Fade* fade_ = nullptr;

	// 3Dモデルデータ
	Model* playerModel_ = nullptr;
	Model* blockModel_ = nullptr;
	Model* skydomeModel_ = nullptr;
	Model* enemyModel_ = nullptr;
	Model* deathParticlesModel_ = nullptr;
	Model* hitModel_ = nullptr;
	Model* bulletModel_ = nullptr;
	Model* moveBlockModel_ = nullptr;
	Model* goalModel_ = nullptr;
	Model* backGroundModel_ = nullptr;
	// ワールドトランスフォーム
	WorldTransform worldTransform_;
	// カメラ
	Camera camera_;
	// 自キャラ
	Player* player_ = nullptr;
	// ブロック用ワールドトランスフォーム
	std::vector<std::vector<WorldTransform*>> worldTransformBlocks_;
	// デバックカメラ有効
	bool isDebugCameraActive_ = false;
	// デバックカメラ
	DebugCamera* debugCamera_ = nullptr;
	// 背景
	Skydome* skydome_;
	// マップチップ
	MapChipField* mapChipField_;
	// カメラコントロール
	CameraController* cameraController_;
	// 敵
	std::list<Enemy*> enemyes_;
	// デスパーティクル
	DeathParticles* deathParticles_ = nullptr;
	// 終了フラグ
	bool finished_ = false;
	// 弾
	std::list<Bullet*> bullets_;
	uint32_t bulletCoolTime_ = 0;

	bool isBullet_ = false;

	// 攻撃ギミックの経過時間カウンター
	uint32_t bulletParameter_ = 0;
	// 予備動作の時間
	static inline const uint32_t kBulletAnticipationTime = 8;

	// 動くブロック
	std::list<MoveBlock*> moveBlocks_;

	// ゴール
	Goal* goal_ = nullptr;

	// 背景
	BackGround* backGround_ = nullptr;
};
