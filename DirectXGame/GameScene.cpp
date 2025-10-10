#include "GameScene.h"
#include "Math.h"

using namespace KamataEngine;

void GameScene::Initialize() {

	// 自キャラの生成
	player_ = new Player();

	// 3Dモデルの生成
	playerModel_ = Model::CreateFromOBJ("player");
	blockModel_ = Model::CreateFromOBJ("block");
	skydomeModel_ = Model::CreateFromOBJ("skydome", true);
	enemyModel_ = Model::CreateFromOBJ("enemy");
	deathParticlesModel_ = Model::CreateFromOBJ("deathParticle");
	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	// カメラの初期化
	camera_.farZ = 1000;
	camera_.Initialize();

	// デバックカメラの生成
	debugCamera_ = new DebugCamera(WinApp::kWindowWidth, WinApp::kWindowHeight);

	// 背景
	skydome_ = new Skydome;
	// 背景の初期化
	skydome_->Initialize(skydomeModel_, &camera_);

	// マップチップの初期化
	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");
	GenerateBlocks();
	// 座標をマップチップ番号で指定
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(3, 17);

	// 自キャラの初期化
	player_->Initialize(playerModel_, &camera_, playerPosition);
	// マップチップフィールドの生成と初期化
	player_->SetMapChipField(mapChipField_);

	// カメラコントロールの初期化
	cameraController_ = new CameraController(); // 生成
	cameraController_->Initialize(&camera_);    // 初期化
	cameraController_->SetTarget(player_);      // 追従対象セット
	cameraController_->Reset();                 // リセット

	CameraController::Rect cameraArea = {12.0f, 100 - 12.0f, 6.0f, 6.0f};
	cameraController_->SetMovableArea(cameraArea);

	// 敵の初期化
	for (int32_t i = 0; i < 0; ++i) {
		Enemy* newEnemy = new Enemy();
		Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(i * 10, 18);
		newEnemy->Initialize(enemyModel_, &camera_, enemyPosition, this);

		enemyes_.push_back(newEnemy);
	}

	// ゲームプレイフェーズから開始
	phase_ = Phase::kPlay;

	// 弾の初期化
	bulletModel_ = Model::CreateFromOBJ("bullet");
	for (int32_t i = 0; i < 10; i++) {
		Bullet* newBullet = new Bullet();
		newBullet->Initialize(bulletModel_, &camera_, playerPosition);

		bullets_.push_back(newBullet);
	}

	// 動くブロックの初期化
	moveBlockModel_ = Model::CreateFromOBJ("move_block");
	for (int32_t i = 0; i < 1; ++i) {
		MoveBlock* newMoveBlock = new MoveBlock();
		Vector3 moveBlockPosition = mapChipField_->GetMapChipPositionByIndex(40, 16);
		newMoveBlock->Initialize(moveBlockModel_, &camera_, moveBlockPosition);
		moveBlocks_.push_back(newMoveBlock);
	}

	// ゴール
	goalModel_ = Model::CreateFromOBJ("goal");
	goal_ = new Goal();
	Vector3 goalPosition = mapChipField_->GetMapChipPositionByIndex(95, 14);
	goal_->Initialize(goalModel_, &camera_, goalPosition);

	// フェード
	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);

	// 背景
	backGroundModel_ = Model::CreateFromOBJ("backGround", true);
	backGround_ = new BackGround();
	Vector3 backGroundPosition = {50.0f, -20.0f, 50.0f};
	backGround_->Initialize(backGroundModel_, &camera_, backGroundPosition);

}

void GameScene::Update() {

	// フェーズの切り替え
	ChangePhase();

	switch (phase_) {
	case Phase::kPlay:
		// ゲームプレイフェーズの処理
		break;
	case Phase::kDeath:
		// デス演出フェーズの処理
		if (deathParticles_ && deathParticles_->IsFinished()) {
		}
		break;
	}

	// 自キャラの更新
	player_->Update();
	// 背景の更新
	skydome_->Update();
	backGround_->Update();
	// カメラコントロール
	cameraController_->Updata();

	// 敵の更新
	// デスフラグの立った敵を削除
	enemyes_.remove_if([](Enemy* enemy) {
		if (enemy->isDead_) {
			delete enemy;
			return true;
		}

		return false;
	});

	for (Enemy* enemy : enemyes_) {
		enemy->Update();
	}

	// 弾の更新
	bullets_.remove_if([](Bullet* bullet) {
		if (bullet->IsAlive()) {
			delete bullet;
			return true;
		}
		return false;
	});
	for (Bullet* bullet : bullets_) {
		bullet->Update();
	}

	// 弾のクールタイム処理
	if (bulletCoolTime_ > 0) {
		bulletCoolTime_--;
	}

	// 発射処理
	if (Input::GetInstance()->TriggerKey(DIK_SPACE) && bulletCoolTime_ <= 0) {
		isBullet_ = true;
	}

	if (isBullet_) {
		// 予備動作時間
		bulletParameter_++;

		// 前進動作へ移行
		if (bulletParameter_ >= kBulletAnticipationTime) {

			Bullet* newBullet = new Bullet();
			newBullet->Initialize(bulletModel_, &camera_, player_->GetWorldPosition());
			newBullet->isDead_ = false;

			// 弾を飛ばす向き
			if (player_->lrDirection_ == Player::LRDirection::kRight) {
				newBullet->velocity_ = Vector3(0.05f, 0, 0);
			} else {
				newBullet->velocity_ = Vector3(-0.05f, 0, 0);
			}

			bullets_.push_back(newBullet);
			bulletCoolTime_ = 20; // クールタイムリセット
			bulletParameter_ = 0;
			isBullet_ = false;
		}
	}

	// 動くブロックの更新
	for (MoveBlock* moveBlock : moveBlocks_) {
		moveBlock->Update();
	}

	// ゴール
	goal_->Update();


	// デバックカメラの切り替え
#ifdef _DEBUG
	if (Input::GetInstance()->TriggerKey(DIK_1)) {
		isDebugCameraActive_ = true;
	}
#endif

	// カメラの処理
	if (isDebugCameraActive_) {
		// デバックカメラの更新
		debugCamera_->Update();
		camera_.matView = debugCamera_->GetCamera().matView;
		camera_.matProjection = debugCamera_->GetCamera().matProjection;
		// ビュープロジェクション行列の転送
		camera_.TransferMatrix();
	} else {
		// ビュープロジェクション行列の更新と転送
		camera_.UpdateMatrix();
	}

	// ブロックの更新
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;

			// アフィン変換～DirectXに転送
			WorldTransformUpdate(*worldTransformBlock);
		}
	}

	// デバックカメラの更新
	debugCamera_->Update();

	// 全ての当たり判定を行う
	CheckAllCollisions();

	if (deathParticles_) {
		// デスパーティクルの更新
		deathParticles_->Update();
	}
}

void GameScene::Draw() {
	// DirectXCommonインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 3Dモデル描画前処理
	Model::PreDraw(dxCommon->GetCommandList());

	// 敵の描画
	for (Enemy* enemy : enemyes_) {
		enemy->Draw();
	}

	// 弾の描画
	for (Bullet* bullet : bullets_) {
		bullet->Draw();
	}

	// 動くブロックの描画
	for (MoveBlock* moveBlock : moveBlocks_) {
		moveBlock->Draw();
	}


	// ゴールの描画
	goal_->Draw();

	// ブロックの描画
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;
			blockModel_->Draw(*worldTransformBlock, camera_);
		}
	}

	// 背景
	skydome_->Draw();
	backGround_->Draw();

	if (deathParticles_) {
		// デスパーティクルの更新
		deathParticles_->Draw();
	}

	// 自キャラの描画
	if (!player_->IsDead()) {
		player_->Draw();
	}

	// 3Dモデル描画後処理
	Model::PostDraw();

	// スプライト描画前処理
	Sprite::PreDraw(dxCommon->GetCommandList());

	// スプライト描画後処理
	Sprite::PostDraw();

	// フェードの描画
	fade_->Draw();
}

GameScene::~GameScene() {
	// 自キャラの解放
	delete player_;
	delete playerModel_;
	// ブロックの解放
	delete blockModel_;

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();

	// デバックカメラの解放
	delete debugCamera_;

	// 背景の解放
	delete skydome_;
	delete skydomeModel_;

	// マップチップの解放
	delete mapChipField_;

	// 敵の解放
	for (Enemy* enemy : enemyes_) {
		delete enemy;
	}
	delete enemyModel_;

	// デスパーティクルの解放
	delete deathParticles_;
	delete deathParticlesModel_;

	// 弾の解放
	for (Bullet* bullet : bullets_) {
		delete bullet;
	}
	delete bulletModel_;
	// カメラコントロールの解放
	delete cameraController_;

	// 動くブロックの解放
	for (MoveBlock* moveBlock : moveBlocks_) {
		delete moveBlock;
	}
	delete moveBlockModel_;

	// ゴールの解放
	delete goal_;
	delete goalModel_;

	// フェードの解放
	delete fade_;
	// 背景の解放
	delete backGround_;
	delete backGroundModel_;

}

void GameScene::ChangePhase() {
	switch (phase_) {
	case Phase::kFadeIn:
		// フェードインフェーズの処理
		fade_->Update();

		if (fade_->IsFinished()) {
			phase_ = Phase::kPlay;
		}
		break;
	case Phase::kPlay:
		if (player_->IsDead()) {
			// 死亡演出
			phase_ = Phase::kFadeOut;

			const Vector3& deathParticlesPosition = player_->GetWorldPosition();

			deathParticles_ = new DeathParticles;
			deathParticles_->Initialize(deathParticlesModel_, &camera_, deathParticlesPosition);
		}
		break;
	case Phase::kFadeOut:
		fade_->Update();
		// フェードアウトフェーズの処理
		if (player_->IsDead()) {
			phase_ = Phase::kPlay;
			player_->isDead_ = false;
			player_->Initialize(playerModel_, &camera_, mapChipField_->GetMapChipPositionByIndex(3, 17));
			break;
		}
		if (fade_->IsFinished()) {
			finished_ = true;
		}
		break;
	}

	// フェード更新
	fade_->Update();
}

void GameScene::GenerateBlocks() {

	uint32_t numBlockVirtical = mapChipField_->GetNumBlockVirtical();
	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	worldTransformBlocks_.resize(numBlockVirtical);
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		worldTransformBlocks_[i].resize(numBlockHorizontal);
	}

	// ブロックの生成
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {

		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {

			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlocks_[i][j] = worldTransform;
				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			}
		}
	}
}

void GameScene::CheckAllCollisions() {
	// 判定対象１と２の座標
	AABB aabb1, aabb2;

	// 自キャラの座標
	aabb1 = player_->GetAABB();

	// 自キャラと敵弾全ての当たり判定
	for (Enemy* enemy : enemyes_) {
		if (enemy->IsCollisionDisabled())
			continue; // コリジョン無効の敵はスキップ

		// 敵弾の座標
		aabb2 = enemy->GetAABB();

		// AABB同士の交差判定
		if (IsCollision(aabb1, aabb2)) {
			if (player_->behavior_ == Player::Behavior::kRoot) {
				// 自キャラの衝突時コールバックを呼び出す
				player_->OnCollision(enemy);
			} else if (player_->behavior_ == Player::Behavior::kAttack) {
				// 敵弾の衝突時コールバックを呼び出す
				enemy->OnCollision(player_);
			}
		}
	}

	// 自キャラと弾の当たり判定
	for (Bullet* bullet : bullets_) {
		// 敵弾の座標
		aabb2 = bullet->GetAABB();

		// AABB同士の交差判定
		if (IsCollision(aabb1, aabb2)) {
			if (player_->behavior_ == Player::Behavior::kRoot) {
				// 自キャラの衝突時コールバックを呼び出す
				player_->OnCollision(bullet);
			}
		}
	}

	// 自キャラと動くブロックの当たり判定
	for (MoveBlock* moveBlock : moveBlocks_) {
		// 敵弾の座標
		aabb2 = moveBlock->GetAABB();

		// AABB同士の交差判定
		if (IsCollision(aabb1, aabb2)) {
			if (!moveBlock->isStop) {
				// 自キャラの衝突時コールバックを呼び出す
				player_->OnCollision(moveBlock);
			}
		}
	}

	// 弾と動くブロックの当たり判定
	for (Bullet* bullet : bullets_) {
		// 敵弾の座標
		aabb1 = bullet->GetAABB();
		for (MoveBlock* moveBlock : moveBlocks_) {
			// 敵弾の座標
			aabb2 = moveBlock->GetAABB();
			// AABB同士の交差判定
			if (IsCollision(aabb1, aabb2)) {

				// 弾の衝突時コールバックを呼び出す
				bullet->OnCollision(moveBlock);
				// 動くブロックの衝突時コールバックを呼び出す
				moveBlock->OnCollision(bullet);
			}
		}
	}

	// 自キャラとゴールの当たり判定
	aabb2 = goal_->GetAABB();
	if (IsCollision(aabb1, aabb2)) {
		fade_->Start(Fade::Status::FadeOut, 1.0f);
		phase_ = Phase::kFadeOut;
	}
}
