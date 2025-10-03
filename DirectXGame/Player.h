#pragma once
#include "KamataEngine.h"
#include "Math.h"

using namespace KamataEngine;

class MapChipField;
class Enemy;
class Bullet;
class MoveBlock;

class Player {
public:
	// 左右
	enum class LRDirection {
		kRight,
		kLeft,
	};

	// 角
	enum Corner {
		kRightBottom, // 右下
		kLeftBottom,  // 左下
		kRightTop,    // 右上
		kLeftTop,     // 左上

		kNumCorner // 要素数
	};

	// 振る舞い
	enum class Behavior {
		kUnknown = -1,
		kRoot,   // 通常状態
		kAttack, // 攻撃中
	};

	enum class AttackPhase {
		kUnknown = -1, // 無効な状態

		kAnticipation, // 予備動作
		kAction,       // 前進動作
		kRecovery,     // 余韻動作
	};

	// 顔の向き
	LRDirection lrDirection_ = LRDirection::kRight;

	// 振るまい
	Behavior behavior_ = Behavior::kRoot;

	// デスフラグ
	bool isDead_ = false;

	// 初期化
	void Initialize(Model* model, Camera * camera, const Vector3& position);
	// 更新
	void Update();
	// 描画
	void Draw();
	// 座標を渡す
	const WorldTransform& GetWorldTransform() const { return worldTransform_; }
	// 速度加算
	const Vector3& GetVelocity() const { return velocity_; }
	// ポインタをセット
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }
	// ワールド座標を取得
	Vector3 GetWorldPosition();
	// AABBを取得
	AABB GetAABB();
	// 衝突応答
	void OnCollision(const Enemy* enemy);
	void OnCollision(const Bullet* bullet);
	void OnCollision(const MoveBlock* moveBlock);
	// デスフラグのgetter
	bool IsDead() const { return isDead_; }
	bool IsAttack() const { return isAttack_; }
	// 通常行動更新
	void BehaviorRootUpdate();
	// 攻撃行動更新
	void BehaviorAttackUpdate();
	// 通常行動初期化
	void BehaviorRootInitialize();
	// 攻撃行動初期化
	void BehaviorAttackInitialize();

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	WorldTransform worldTransformAttack_;
	// モデル
	Model* modelPlayer_ = nullptr;
	Model* modelAttack_ = nullptr;
	// カメラ
	Camera* camera_ = nullptr;
	// 速度
	Vector3 velocity_ = {};
	// 加速度
	static inline const float kAcceleration = 0.2f;
	// 減速度
	static inline const float kAttenuation = 0.1f;
	// 最大速度制限
	static inline const float kLimitRunSpeed = 0.2f;
	// 旋回開始時の角度
	float turnFirstRotationY_ = 0.0f;
	// 旋回タイマー
	float turnTimer_ = 0.0f;
	// 旋回時間<秒>
	static inline const float kTimeTurn = 0.3f;
	// 設置状態フラグ
	bool onGround_ = true;
	// 重力加速度（下方向）
	static inline const float kGravityAcceleration = 0.98f;
	// 最大落下速度（下方向）
	static inline const float kLimitFallSpeed = 0.5f;
	// ジャンプ初速（上方向）
	static inline const float kJumpAcceleration = 20.0f;
	// マップチップによるフィールド
	MapChipField* mapChipField_ = nullptr;
	// キャラクターの当たり判定サイズ
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;
	static inline const float kBlank = 0.04f;
	// 着地時の速度減衰率
	static inline const float kAttenuationLanding = 0.0f;
	// 微笑な数値
	static inline const float kGroundSearchHeight = 0.06f;
	// 着地時の速度減衰率
	static inline const float kAttenuationWall = 0.2f;
	bool isAttack_ = false;

	// 02_14 14枚目 次の振るまいリクエスト
	Behavior behaviorRequest_ = Behavior::kUnknown;

	// 攻撃ギミックの経過時間カウンター
	uint32_t attackParameter_ = 0;

	// 02_14 24枚目 攻撃フェーズ
	AttackPhase attackPhase_ = AttackPhase::kUnknown;

	// 02_14 26枚目 予備動作の時間
	static inline const uint32_t kAnticipationTime = 8;
	// 02_14 26枚目 前進動作の時間
	static inline const uint32_t kActionTime = 5;
	// 02_14 26枚目 余韻動作の時間
	static inline const uint32_t kRecoveryTime = 12;

	// マップとの当たり判定情報
	struct CollisionMapInfo {
		bool ceiling = false; // 天井衝突フラグ
		bool landing = false; // 着地フラグ
		bool hitWall = false; // 壁接地フラグ
		Vector3 move;         // 移動量
	};

	// 移動入力
	void InputMove();
	// マップ衝突判定
	void CheckMapCollision(CollisionMapInfo& info);
	void CheckMapCollisionUp(CollisionMapInfo& info);
	void CheckMapCollisionDown(CollisionMapInfo& info);
	void CheckMapCollisionRight(CollisionMapInfo& info);
	void CheckMapCollisionLeft(CollisionMapInfo& info);
	// 角の座標計算
	Vector3 CornerPosition(const Vector3& center, Corner corner);
	// 接地状態の切り替え処理
	void UpdateOnGround(const CollisionMapInfo& info);
	// 壁接触している場合の処理
	void UpdateOnWall(const CollisionMapInfo& info);
};