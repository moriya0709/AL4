#pragma once
#define _USE_MATH_DEFINES

#include "ImGui.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "Math.h"
#include "KamataEngine.h"

using namespace KamataEngine;

struct UiSet {
	Vector3 pos;
	Vector3 size;
	Vector3 startPos;
	Vector3 endPos;
	Vector3 startSize;
	Vector3 endSize;
	float rotation;
	float startRotation;
	float endRotation;
	float moveTime;
	float sizeTime;
	float rotationTime;
	float moveEasedT;
	float sizeEasedT;
	float rotationEasedT;
	int color;
};

class Easing {
public:
	int time;

	// 初期化
	void Initialize();
	// 更新
	void Update();
	// 描画
	void Draw();

	// 移動
	void Move(UiSet& ui, float timeSpeed, int num);
	// サイズ
	void Size(UiSet& ui, float timeSpeed, int num);
	// 回転
	void Rotation(UiSet& ui, float timeSpeed, int num);

private:
	// 制御点の数
	int kNumControlPoints = 4;

	// 制御点
	std::vector<ImVec2> kControlPoints = {
	    {50,  50 }, // スタート
	    {100, 100}, // ハンドル１
	    {150, 150}, // ハンドル2
	    {200, 200}, // エンド
	};

	// イージング用コントロールポイント
	Vector2 easeP[10][4] = {
	    {{0.0f, 0.0f}, {0.3f, 0.3f}, {0.6f, 0.6f}, {1.0f, 1.0f}},
        {{0.0f, 0.0f}, {0.3f, 0.3f}, {0.6f, 0.6f}, {1.0f, 1.0f}},
        {{0.0f, 0.0f}, {0.3f, 0.3f}, {0.6f, 0.6f}, {1.0f, 1.0f}},
	    {{0.0f, 0.0f}, {0.3f, 0.3f}, {0.6f, 0.6f}, {1.0f, 1.0f}},
        {{0.0f, 0.0f}, {0.3f, 0.3f}, {0.6f, 0.6f}, {1.0f, 1.0f}},
        {{0.0f, 0.0f}, {0.3f, 0.3f}, {0.6f, 0.6f}, {1.0f, 1.0f}},
	    {{0.0f, 0.0f}, {0.3f, 0.3f}, {0.6f, 0.6f}, {1.0f, 1.0f}},
        {{0.0f, 0.0f}, {0.3f, 0.3f}, {0.6f, 0.6f}, {1.0f, 1.0f}},
        {{0.0f, 0.0f}, {0.3f, 0.3f}, {0.6f, 0.6f}, {1.0f, 1.0f}},
	    {{0.0f, 0.0f}, {0.3f, 0.3f}, {0.6f, 0.6f}, {1.0f, 1.0f}},
	};

	// イージングの枠数
	int kEasePNum = 0;

	// 制御点の数
	int ControlPointsNum = 4;

	// ファイル名
	char fileName[20] = "";

	// --- ImGui ---
	// 現在のウィンドウのDrawListを取得
	ImDrawList* draw_list;

	// すべての制御点に原点を加算して、スクリーン座標に変換
	ImVec2 controlPosA;
	ImVec2 controlPosB;
	ImVec2 controlPosC;
	ImVec2 controlPosD;

	// 線形補間
	Vector2 Lerp(const Vector2& p0, const Vector2& p1, float t);
	// float用
	float Lerp(const float& p0, const float& p1, float t);
	// 3次ベジェ曲線
	Vector2 Bezier(Vector2 p0, Vector2 p1, Vector2 p2, Vector2 p3, float t);
	// イージング計算
	float BezierEasing(float t, Vector2 p0, Vector2 p1, Vector2 p2, Vector2 p3);
	// ファイル読み込み
	void LoadEasing(const std::string& filePath, int num);
	// ファイル書き込み
	bool EntryEasing(const std::string& filePath, int num);
};
