#include "CameraController.h"
#include "Math.h"
#include "Player.h"
#include <algorithm>

void CameraController::Initialize(Camera* camera) {
	// カメラの初期化
	camera_ = camera;
}

void CameraController::Updata() {
	// 行列を更新
	camera_->UpdateMatrix();

	// 追従対象のワールドトランスフォームを参照
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	// 追従対象の速度
	const Vector3& targetVelocity = target_->GetVelocity();

	// 追従対象とオフセットと追従対象の速度カメラの目標座標を計算
	destination_ = targetWorldTransform.translation_ + targetOffset_ + targetVelocity * kVelocityBias;

	// 座標補間によりゆったり追従(数学関数追加)
	camera_->translation_ = LerpV(camera_->translation_, destination_, kInterpolationRate);

	// 追従対象が画面外に出ないように補正
	camera_->translation_.x = max(camera_->translation_.x, destination_.x + targetMargin.left);
	camera_->translation_.x = min(camera_->translation_.x, destination_.x + targetMargin.right);
	camera_->translation_.y = max(camera_->translation_.y, destination_.y + targetMargin.bottom);
	camera_->translation_.y = min(camera_->translation_.y, destination_.y + targetMargin.top);

	// 移動範囲制限
	camera_->translation_.x = max(camera_->translation_.x, movableArea_.left);
	camera_->translation_.x = min(camera_->translation_.x, movableArea_.right);
	camera_->translation_.y = max(camera_->translation_.y, movableArea_.bottom);
	camera_->translation_.y = min(camera_->translation_.y, movableArea_.top);
}

void CameraController::Reset() {
	// 追従対象のワールドトランスフォームを参照
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	// 追従対象とオフセットからカメラの座標を計算
	camera_->translation_ = targetWorldTransform.translation_ + targetOffset_;
}
