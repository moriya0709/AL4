#define NOMINMAX

#include <algorithm>

#include "MoveBlock.h"

void MoveBlock::Initialize(Model* model, Camera* camera, const Vector3& position) {
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	modelMoveBlock_ = model;
	camera_ = camera;
}

void MoveBlock::Update() { 
	if (isStop) {
		// 弾が当たった時
		stopTime_--;
		if (stopTime_ <= 0) {
			isStop = false;
			stopTime_ = 180;
		}
	}
	else
	{
		// 回転
		worldTransform_.rotation_.z += 0.05f;
	}

	WorldTransformUpdate(worldTransform_);
}

void MoveBlock::Draw() { modelMoveBlock_->Draw(worldTransform_, *camera_); }

Matrix4x4 MoveBlock::GetWorldMatrix() const {
	// 単位行列からスタート
	Matrix4x4 world = MakeIdentityMatrix();

	// スケーリング行列
	Matrix4x4 scaleMat = MakeScaleMatrix(worldTransform_.scale_);

	// 回転行列（XYZ回転を合成）
	Matrix4x4 rotX = MakeRotateXMatrix(worldTransform_.rotation_.x);
	Matrix4x4 rotY = MakeRotateYMatrix(worldTransform_.rotation_.y);
	Matrix4x4 rotZ = MakeRotateZMatrix(worldTransform_.rotation_.z);
	Matrix4x4 rotMat = rotZ * rotX * rotY; // ← 回転の順序はエンジンに合わせて調整

	// 平行移動行列
	Matrix4x4 transMat = MakeTranslateMatrix(worldTransform_.translation_);

	// ワールド行列 = S * R * T
	world = scaleMat * rotMat * transMat;

	return world;
}


AABB MoveBlock::GetAABB() {

	// ローカルの半径（中心からのオフセット）
	float halfW = kWidth / 2.0f;
	float halfH = kHeight / 2.0f;
	float halfD = kWidth / 2.0f;

	// ローカル座標での8頂点
	std::vector<Vector3> localVertices = {
	    {-halfW, -halfH, -halfD},
        {halfW,  -halfH, -halfD},
        {halfW,  halfH,  -halfD},
        {-halfW, halfH,  -halfD},
	    {-halfW, -halfH, halfD },
        {halfW,  -halfH, halfD },
        {halfW,  halfH,  halfD },
        {-halfW, halfH,  halfD }
    };

	// ワールド行列（回転も含まれているものを取得する想定）
	Matrix4x4 worldMat = GetWorldMatrix();

	// min/max の初期化
	Vector3 minPos = {FLT_MAX, FLT_MAX, FLT_MAX};
	Vector3 maxPos = {-FLT_MAX, -FLT_MAX, -FLT_MAX};

	for (const auto& local : localVertices) {
		// ワールド座標に変換
		Vector3 world = Transform(local, worldMat);

		minPos.x = std::min(minPos.x, world.x);
		minPos.y = std::min(minPos.y, world.y);
		minPos.z = std::min(minPos.z, world.z);

		maxPos.x = std::max(maxPos.x, world.x);
		maxPos.y = std::max(maxPos.y, world.y);
		maxPos.z = std::max(maxPos.z, world.z);
	}

	AABB aabb;
	aabb.min = minPos;
	aabb.max = maxPos;

	return aabb;
}


void MoveBlock::OnCollision(const Player* player) { (void)player; }

void MoveBlock::OnCollision(const Bullet* bullet) { 
	(void)bullet;

	isStop = true;
}
