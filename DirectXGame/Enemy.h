#pragma once
#include "KamataEngine.h"
#include "Math.h"

class MapChipField;
class Player;
class Atk;

class Enemy {
public:
	// 左右(キャラクター)
	enum class LRDirection {
		kRight,
		kLeft,
	};

	// 振る舞い
	enum class Behavior {
		kUnknown = -1,
		kRoot,   // 通常状態
		kDead, // デス状態
	};

	// 角
	enum Corner {
		kRightBottom, // 右下
		kLeftBottom,  // 左下
		kRightTop,    // 右上
		kLeftTop,     // 左上

		kNumCorner // 要素数
	};

	// デスフラグ
	bool isDead_ = false;
	// 顔の向き
	LRDirection lrDirection_ = LRDirection::kLeft;
	// 振るまい
	Behavior behavior_ = Behavior::kRoot;

	// 初期化
	void Initialize(Model* model, uint32_t hitEnemyTex, Camera* camera,Player* player, const Vector3& position);
	// 更新
	void Update();
	// 描画
	void Draw();
	// ワールド座標を取得
	Vector3 GetWorldPosition();
	// AABBを取得
	AABB GetAABB();
	// 衝突応答
	void OnCollision(const Player* player);
	void OnCollision(const Atk* atk);
	// 通常行動更新
	void BehaviorRootUpdate();
	// デス更新
	void BehaviorDeadUpdate();
	// 当たり判定無効フラグ
	bool IsCollisionDisabled() const { return isCollisionDisabled_; }
	// ポインタをセット
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }



private:
	// マップとの当たり判定情報
	struct CollisionMapInfo {
		bool ceiling = false; // 天井衝突フラグ
		bool landing = false; // 着地フラグ
		bool hitWall = false; // 壁接地フラグ
		Vector3 move;         // 移動量
	};

	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
	// ヒットテクスチャ
	uint32_t hitEnemyTex_ = 0;
	// カメラ
	Camera* camera_ = nullptr;
	// 歩行の速さ
	float kWalkSpeed = -0.02f;
	// 速度
	Vector3 velocity_ = {};

	// 最初の角度[度]
	static inline const float kWalkMotionAngleStart = 0.0f;
	// 最後の角度[度]
	static inline const float kWalkMotionAngleEnd = 30.0f;
	// アニメーションの同期となる時間[秒]
	static inline const float kWalkMotionTime = 1.0f;
	// 経過時間
	float walkTimer_ = 0.0f;

	// 当たり判定サイズ
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;
	static inline const float kBlank = 0.04f;

	// 次の振るまいリクエスト
	Behavior behaviorRequest_ = Behavior::kUnknown;

	// デス演出のアニメーションの経過時間
	float deadTimer_ = 0.0f;
	// 当たり判定無効フラグ
	bool isCollisionDisabled_ = false;

	// hp
	uint32_t hp_ = 3;
	// 無敵
	bool isInvincible;
	int invincibleTime;

	// 設置状態フラグ
	bool onGround_ = true;

	// 重力加速度（下方向）
	static inline const float kGravityAcceleration = 0.98f;
	// 最大落下速度（下方向）
	static inline const float kLimitFallSpeed = 0.5f;

	// 微小な数値
	static inline const float kGroundSearchHeight = 0.06f;

	// 旋回開始時の角度
	float turnFirstRotationY_ = 0.0f;
	// 旋回タイマー
	float turnTimer_ = 0.0f;
	// 旋回時間<秒>
	static inline const float kTimeTurn = 0.3f;

	// マップチップによるフィールド
	MapChipField* mapChipField_ = nullptr;
	// プレイヤー
	Player* player_ = nullptr;

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
};
