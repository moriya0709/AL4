#define NOMINMAX

#include "assert.h"
#include <algorithm>
#include <cassert>
#include <numbers>

#include "Player.h"
#include "MapChipField.h"
#include "Math.h"

void Player::Initialize(Model* model, Camera* camera, const Vector3& position) {
	// NULLポインタチェック
	assert(model);
	// 引数の内容をメンバ変数に記録
	modelPlayer_ = model;
	camera_ = camera;

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;

	// イージング用
	player_.worldTransform.Initialize();
	player_.worldTransform.translation_ = position;
	player_.worldTransform.rotation_.y = std::numbers::pi_v<float> / 2.0f;
	player_.startRotation = player_.worldTransform.rotation_;
	player_.endRotation = player_.worldTransform.rotation_ + Vector3(6.3f,0.0f,0.0f);
	player_.rotationTime = 1.0f;

	// ワールドトランスフォームの初期化(攻撃)
	worldTransformAttack_.Initialize();
	worldTransformAttack_.translation_ = position;
	worldTransformAttack_.rotation_.y = std::numbers::pi_v<float> / 2.0f;

	// デスフラグの初期化
	isDead_ = false;

	// ジャンプカウント
	jumpCount = 0;

	// 手モデルの生成
	handModel_ = Model::CreateFromOBJ("player_hand");
	hand_.worldTransform.Initialize();

	// 合計後の速度
	finalVelocity = {};

	// イージング
	easing_ = new Easing();
	easing_->Initialize();
}

void Player::Update() {
	if (behaviorRequest_ != Behavior::kUnknown) {
		// 振る舞いを変更する
		behavior_ = behaviorRequest_;

		// 各振る舞いごとの初期化を実行
		switch (behavior_) { 
		case Behavior::kRoot:
		default:
			BehaviorRootInitialize();
			break;
		case Behavior::kAttack:
			BehaviorAttackInitialize();
			break;
		}

		// 振る舞いリクエストをリセット
		behaviorRequest_ = Behavior::kUnknown;
	
	}

	switch (behavior_)
	{
	case Behavior::kRoot:
	default:
		BehaviorRootUpdate();
		break;
	case Behavior::kAttack:
		BehaviorAttackUpdate();
		break;
	}

	// アフィン変換～DirectXに転送
	WorldTransformUpdate(worldTransform_);
	WorldTransformUpdate(worldTransformAttack_);
}

void Player::Draw() {
	// 3Dモデル描画
	modelPlayer_->Draw(worldTransform_, *camera_);
	if (isAttack_) {
		handModel_->Draw(hand_.worldTransform, *camera_);
	}
}

void Player::InputMove() {
		// 左右移動操作
		if (Input::GetInstance()->PushKey(DIK_D) || Input::GetInstance()->PushKey(DIK_A) || state.Gamepad.sThumbLX) {

			// 左右加速
			Vector3 acceleration = {};
			if (Input::GetInstance()->PushKey(DIK_D) || state.Gamepad.sThumbLX > DEAD_ZONE) {

				if (velocity_.x < 0.0f) {
					// 旋回の最初は移動減衰をかける
					velocity_.x *= (1.0f - kAttenuation);
				}
				acceleration.x += kAcceleration / 30.0f;
				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}
			} else if (Input::GetInstance()->PushKey(DIK_A) || state.Gamepad.sThumbLX < -DEAD_ZONE) {
				if (velocity_.x > 0.0f) {
					// 旋回の最初は移動減衰をかける
					velocity_.x *= (1.0f - kAttenuation);
				}
				acceleration.x -= kAcceleration / 30.0f;
				if (lrDirection_ != LRDirection::kLeft) {
					lrDirection_ = LRDirection::kLeft;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}
			}
			velocity_ += acceleration;
			//velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
		} else {
			// 非入力時は移動減衰をかける
			velocity_.x *= (1.0f - kAttenuation);
		}

		// ほぼ0の場合に0にする
		if (std::abs(velocity_.x) <= 0.0001f) {
			velocity_.x = 0.0f;
		}
	    
	if (onGround_) {
		if (Input::GetInstance()->TriggerKey(DIK_SPACE) || (state.Gamepad.wButtons & XINPUT_GAMEPAD_A) && !(preState.Gamepad.wButtons & XINPUT_GAMEPAD_A)) {
			// ジャンプ初速
			velocity_ += Vector3(0, kJumpAcceleration / 60.0f, 0);
		}
	} else {
		// 落下速度
		velocity_ += Vector3(0, -kGravityAcceleration / 60.0f, 0);
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);

		if (jumpCount == 1) {
			if (Input::GetInstance()->TriggerKey(DIK_SPACE) || (state.Gamepad.wButtons & XINPUT_GAMEPAD_A) && !(preState.Gamepad.wButtons & XINPUT_GAMEPAD_A)) {
				// ジャンプ初速
				velocity_.y = 0.0f;
				velocity_ += Vector3(0, kJumpAcceleration / 60.0f, 0);
				jumpCount++;
			}
		}
	}

	// ローリング
	rollingCoolTime -= 1.0f / 60.0f;
	if (invincibility) {
		if (invincibilityFrame > 0) {
			invincibilityFrame--;
		} else {
			invincibility = false;
		}
	}

	if (Input::GetInstance()->TriggerKey(DIK_LSHIFT)) {
		if (rollingCoolTime <= 0.0f) {

			isRolling_ = true;

			if (lrDirection_ == LRDirection::kRight) {
				rollVelocityX_ = 0.5f;
			} else {
				rollVelocityX_ = -0.5f;
			}

			invincibilityFrame = 30;
			invincibility = true;
			rollingCoolTime = 0.5f;

			// イージング用
			player_.rotationEasedT = 0.0f;
			player_.rotationTime = 0.0f;
		}
	}

	if (isRolling_) {
		rollVelocityX_ *= 0.9f; // ← ローリング専用の減衰

		if (std::abs(rollVelocityX_) < 0.01f) {
			rollVelocityX_ = 0.0f;
			isRolling_ = false;
		}

		easing_->Rotation(player_, 0.05f, 0);
		worldTransform_.rotation_.x = player_.worldTransform.rotation_.x;
	}

	finalVelocity = velocity_;
	finalVelocity.x += rollVelocityX_;

}

void Player::CheckMapCollision(CollisionMapInfo& info) {
	CheckMapCollisionUp(info);
	CheckMapCollisionDown(info);
	CheckMapCollisionRight(info);
	CheckMapCollisionLeft(info);
}

void Player::CheckMapCollisionUp(CollisionMapInfo& info) {
	// 上昇ありか
	if (info.move.y <= 0) {
		return;
	}

	// 移動後の４つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;

	// 真上の当たり判定を行う
	bool hit = false;

	// 左上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);

	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	// 右上点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);

	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒットか
	if (hit) {
		// 現在座標が壁の外か判定
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(0, +kHeight / 2.0f, 0));
		if (indexSetNow.yIndex != indexSet.yIndex) {
			// めり込みを排除する方向に移動量を設定する
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(0, +kHeight / 2.0f, 0));
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.move.y = std::max(0.0f, rect.bottom - worldTransform_.translation_.y - (kHeight / 2.0f + kBlank));
			info.ceiling = true;
		}
	}
}

void Player::CheckMapCollisionDown(CollisionMapInfo& info) {

	info;

	// 02_08 スライド7枚目 下降あり？
	if (info.move.y >= 0) {
		return;
	}

	// 02_08 スライド7枚目（下のfor文も）
	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;

	// フラグ初期化
	bool hit = false;

	// 左下の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);

	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);

	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	// 02_08スライド11枚目 ブロックにヒット？
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(0, -kHeight / 2.0f, 0));
		// 現在座標が壁の外か判定
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(0, -kHeight / 2.0f, 0));
		if (indexSetNow.yIndex != indexSet.yIndex) {
			// めり込み先ブロックの範囲矩形
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.move.y = std::min(0.0f, rect.top - worldTransform_.translation_.y + (kHeight / 2.0f + kBlank));
			// 地面に当たったことを記録する
			info.landing = true;
		}
	}
}

void Player::CheckMapCollisionRight(CollisionMapInfo& info) {

	if (info.move.x <= 0) {
		return;
	}

	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;

	// 右側の当たり判定
	bool hit = false;

	// 右上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex - 1, indexSet.yIndex);

	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex - 1, indexSet.yIndex);

	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒット？
	if (hit) {
		// 現在座標が壁の外か判定
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(+kWidth / 2.0f, 0, 0));
		if (indexSetNow.xIndex != indexSet.xIndex) {
			// めり込みを排除する方向に移動量を設定する
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(+kWidth / 2.0f, 0, 0));
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.move.x = std::max(0.0f, rect.left - worldTransform_.translation_.x - (kWidth / 2.0f + kBlank));
			info.hitWall = true;
		}
	}
}

void Player::CheckMapCollisionLeft(CollisionMapInfo& info) {

	if (info.move.x >= 0) {
		return;
	}

	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;

	// 右側の当たり判定
	bool hit = false;

	// 左上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex + 1, indexSet.yIndex);

	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	// 左下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex + 1, indexSet.yIndex);

	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒット？
	if (hit) {
		// 現在座標が壁の外か判定
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(-kWidth / 2.0f, 0, 0));

		if (indexSetNow.xIndex != indexSet.xIndex) {
			// めり込みを排除する方向に移動量を設定する
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(-kWidth / 2.0f, 0, 0));
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.move.x = std::max(0.0f, rect.right - worldTransform_.translation_.x - (kWidth / 2.0f + kBlank));
			info.hitWall = true;
		}
	}
}

Vector3 Player::CornerPosition(const Vector3& center, Corner corner) {

	Vector3 offsetTable[] = {
	    {+kWidth / 2.0f, -kHeight / 2.0f, 0}, //  kRightBottom
	    {-kWidth / 2.0f, -kHeight / 2.0f, 0}, //  kLeftBottom
	    {+kWidth / 2.0f, +kHeight / 2.0f, 0}, //  kRightTop
	    {-kWidth / 2.0f, +kHeight / 2.0f, 0}  //  kLeftTop
	};

	return center + offsetTable[static_cast<uint32_t>(corner)];
}

// 設置状態の切り替え処理
void Player::UpdateOnGround(const CollisionMapInfo& info) {

	info;

	if (onGround_) {
		// ジャンプ開始
		if (velocity_.y > 0.0f) {
			onGround_ = false;
			jumpCount++;
		} else {
			// 落下判定
			// 落下なら空中状態に切り替え

			// 02_08スライド19枚目(このelseブロック全部)
			std::array<Vector3, kNumCorner> positionsNew;

			for (uint32_t i = 0; i < positionsNew.size(); ++i) {
				positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
			}

			bool hit = false;

			MapChipType mapChipType;

			// 左下点の判定
			MapChipField::IndexSet indexSet;
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom] + Vector3(0, -kGroundSearchHeight, 0));
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}

			// 右下点の判定
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom] + Vector3(0, -kGroundSearchHeight, 0));
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}

			// 落下開始
			if (!hit) {
				//DebugText::GetInstance()->ConsolePrintf("jump");
				onGround_ = false;
				jumpCount++;
			}
		}
	} else {
		// 02_08スライド16枚目 地面に接触している場合の処理
		if (info.landing) {
			// 着地状態に切り替える（落下を止める）
			onGround_ = true;
			// 着地時にX速度を減衰
			velocity_.x *= (1.0f - kAttenuationLanding);
			// Y速度をゼロに
			velocity_.y = 0.0f;

			// ジャンプカウントリセット
			jumpCount = 0;
		}
	}
}

// 壁接地中の処理
void Player::UpdateOnWall(const CollisionMapInfo& info) {

	if (info.hitWall) {
		finalVelocity.x *= (1.0f - kAttenuationWall);

		onGround_ = true;
		jumpCount = 0;

		// 壁ジャンプ
		if (Input::GetInstance()->TriggerKey(DIK_SPACE) || (state.Gamepad.wButtons & XINPUT_GAMEPAD_A) && !(preState.Gamepad.wButtons & XINPUT_GAMEPAD_A)) {
			if (lrDirection_ == LRDirection::kRight) {
				velocity_.x -= 0.5f;
			} else if (lrDirection_ == LRDirection::kLeft) {
				velocity_.x += 0.5f;
			}
		}
	}
}

Vector3 Player::GetWorldPosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos = worldTransform_.translation_;
	
	return worldPos;
}

AABB Player::GetAABB() { 
	Vector3 worldPos = GetWorldPosition();

	AABB aabb;

	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};


	return aabb;
}

void Player::OnCollision(const Enemy* enemy) {
	// 不使用
	(void)enemy;

	// 書き換え
	if (!invincibility)
	isDead_ = true;
}

void Player::OnCollision(const Bullet* bullet) {
	(void)bullet;

	// プレイヤーが下方向に移動している（落下中）
	if (velocity_.y < 0.0f) {
		// ジャンプ初速
		velocity_ += Vector3(0, kJumpAcceleration / 60.0f, 0);
	}
}

void Player::OnCollision(const MoveBlock* moveBlock) {
	(void)moveBlock;
	// 書き換え
	if (!invincibility)
	isDead_ = true;
}

// 通常行動初期化
void Player::BehaviorRootInitialize() {}

// 通常行動更新
void Player::BehaviorRootUpdate() {
	
	// 現在のジョイスティックを取得
	Input::GetInstance()->GetJoystickState(0, state);
	// 前回のジョイスティックを取得
	Input::GetInstance()->GetJoystickStatePrevious(0, preState);

	// 移動入力
	InputMove();

	// 衝突情報を初期化
	CollisionMapInfo collisionMapInfo = {};
	collisionMapInfo.move = finalVelocity;
	collisionMapInfo.landing = false;
	collisionMapInfo.hitWall = false;

	// マップ衝突チェック
	CheckMapCollision(collisionMapInfo);

	// 移動
	worldTransform_.translation_ += collisionMapInfo.move;

	// 天井接触による落下開始
	if (collisionMapInfo.ceiling) {
		finalVelocity.y = 0;
	}

	// 壁接触している場合の処理
	UpdateOnWall(collisionMapInfo);

	// 接地判定
	UpdateOnGround(collisionMapInfo);

	// 旋回制御
	if (turnTimer_ > 0.0f) {
		// タイマーを進める
		turnTimer_ = std::max(turnTimer_ - (1.0f / 60.0f), 0.0f);

		float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};

		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];

		worldTransform_.rotation_.y = EaseInOut(destinationRotationY, turnFirstRotationY_, turnTimer_ / kTimeTurn);
	}

	// 攻撃キーを押したら
	if (Input::GetInstance()->IsTriggerMouse(0)) {
		// 攻撃ビヘイビアをリクエスト
		behaviorRequest_ = Behavior::kAttack;
		// 攻撃の種類
		if (onGround_) {
			attackTypes_ = Normal;
		} else {
			attackTypes_ = Air;
		}
		isAttack_ = true;
		// 攻撃行動初期化
		AttackInitialize();
	}

	// 落下判定
	if (worldTransform_.translation_.y < -10.0f) {
		isDead_ = true;
	}
}

// 攻撃行動初期化
void Player::BehaviorAttackInitialize() {

	// 02_14 19枚目 カウンター初期化
	attackParameter_ = 0;

	// 溜めフェーズから始める
	attackPhase_ = AttackPhase::kAnticipation;
}

// 攻撃行動更新
void Player::BehaviorAttackUpdate()
{
	
	// 移動
	if (attackTypes_ == Air) {
		InputMove();
	} else {
		finalVelocity = {};
	}

	// 攻撃動作(手)
	if (attackTypes_ == Normal) {
		easing_->Move(hand_, 0.05f, 0);
	} else if (attackTypes_ == Air) {
		easing_->Rotation(hand_, 0.01f, 0);
		hand_.worldTransform.translation_ = worldTransform_.translation_;
	}

	// 予備動作
	attackParameter_++;
	
	switch (attackPhase_) {
	case AttackPhase::kAnticipation: // 溜め動作
	default: {
		float t = static_cast<float>(attackParameter_) / kAnticipationTime;
		worldTransform_.scale_.z = EaseOut(1.0f, 0.3f, t);
		worldTransform_.scale_.y = EaseOut(1.0f, 1.6f, t);

		// 前進動作へ移行
		if (attackParameter_ >= kAnticipationTime) {
			attackPhase_ = AttackPhase::kAction;
			attackParameter_ = 0; // カウンターをリセット
		}
		break;
	}
	case AttackPhase::kAction: { // 突進動作
		float t = static_cast<float>(attackParameter_) / kActionTime;
		worldTransform_.scale_.z = EaseOut(0.3f, 1.3f, t);
		worldTransform_.scale_.y = EaseIn(1.6f, 0.7f, t);

		// 移動
		if (onGround_) {
			if (lrDirection_ == LRDirection::kRight) {
				finalVelocity.x = 0.1f;
			} else {
				finalVelocity.x = -0.1f;
			}
		}

		// 余韻動作へ移行
		if (attackParameter_ >= kActionTime) {
			attackPhase_ = AttackPhase::kRecovery;
			attackParameter_ = 0; // パラメータをリセット
		}
	} break;
	case AttackPhase::kRecovery: { // 余韻動作
		float t = static_cast<float>(attackParameter_) / kRecoveryTime;
		worldTransform_.scale_.z = EaseOut(1.3f, 1.0f, t);
		worldTransform_.scale_.y = EaseOut(0.7f, 1.0f, t);

		// 通常行動に戻る
		if (attackParameter_ >= kRecoveryTime) {
			behaviorRequest_ = Behavior::kRoot;
			isAttack_ = false;
		}
		break;
	}
	}
	
	// 衝突情報を初期化
	CollisionMapInfo collisionMapInfo = {};
	collisionMapInfo.move = finalVelocity;
	collisionMapInfo.landing = false;
	collisionMapInfo.hitWall = false;
	
	// マップ衝突チェック
	CheckMapCollision(collisionMapInfo);
	
	// 移動
	worldTransform_.translation_ += collisionMapInfo.move;
	
	if (turnTimer_ > 0.0f) {
		// タイマーを進める
		turnTimer_ = std::max(turnTimer_ - (1.0f / 60.0f), 0.0f);
	
		float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};
	
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
	
		worldTransform_.rotation_.y = EaseInOut(destinationRotationY, turnFirstRotationY_, turnTimer_ / kTimeTurn);
	}
	
	worldTransformAttack_.translation_ = worldTransform_.translation_;
	worldTransformAttack_.rotation_ = worldTransform_.rotation_;

	WorldTransformUpdate(hand_.worldTransform);
}

// 攻撃動作初期化
void Player::AttackInitialize() {
	if (attackTypes_ == Normal) {
		hand_.worldTransform.Initialize();
		hand_.worldTransform.translation_ = {worldTransform_.translation_ + Vector3(0.0f, 1.0f, 0.0f)};
		hand_.worldTransform.scale_ = {1.0f, 1.0f, 1.0f};
		hand_.startPos = hand_.worldTransform.translation_;
		hand_.moveTime = 0.0f;
		hand_.moveEasedT = 0.0f;

		if (lrDirection_ == LRDirection::kRight) {
			hand_.worldTransform.rotation_ = {worldTransform_.rotation_ + Vector3(1.0f, 0.0f, 0.0f)};
			hand_.endPos = hand_.worldTransform.translation_ + Vector3(2.0f, -1.0f, 0.0f);
			
		} else if (lrDirection_ == LRDirection::kLeft) {
			hand_.worldTransform.rotation_ = {worldTransform_.rotation_ + Vector3(7.0f, 0.0f, 0.0f)};
			hand_.endPos = hand_.worldTransform.translation_ + Vector3(-2.0f, -1.0f, 0.0f);
			
		}

	} else if (attackTypes_ == Air) {
		hand_.worldTransform.Initialize();
		hand_.worldTransform.translation_ = {worldTransform_.translation_};
		hand_.worldTransform.rotation_ = {0.0f, 0.0f, 0.0f};
		hand_.worldTransform.scale_ = {2.0f, 2.0f, 2.0f};
		hand_.startRotation = hand_.worldTransform.rotation_;
		hand_.endRotation = hand_.worldTransform.rotation_ + Vector3(0.0f, 0.0f, 21.0f);
		hand_.rotationTime = 0.0f;
		hand_.rotationEasedT = 0.0f;
	}
}
