#pragma once
#include "KamataEngine.h"

using namespace KamataEngine;

enum class MapChipType {
	kBlank, // 空白
	kBlock, // ブロック
};

struct MapChipData {
	std::vector<std::vector<MapChipType>> data;
};

class MapChipField {
public:
	struct IndexSet {
		uint32_t xIndex; // X座標のインデックス
		uint32_t yIndex; // Y座標のインデックス
	};

	// 範囲矩形
	struct Rect {
		float left;   // 左端
		float right;  // 右端
		float top;    // 上端
		float bottom; // 下端
	};

	// 1ブロックのサイズ
	static inline const float kBlockWidth = 1.0f;
	static inline const float kBlockHeight = 1.0f;
	// ブロックの個数
	static inline const uint32_t kNumBlockVirtical = 20;
	static inline const uint32_t kNumBlockHorizontal = 100;
	// マップチップデータ
	MapChipData mapChipData_;
	// マップチップフィールド
	MapChipField* mapChipField_;

	// リセット
	void ResetMapChipData();
	// 読み込み
	void LoadMapChipCsv(const std::string& filePath);
	// 種類別の取得
	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);
	// 座標取得
	Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex);
	// 縦方向のブロック数を取得
	uint32_t GetNumBlockVirtical();
	// 横方向のブロック数を取得
	uint32_t GetNumBlockHorizontal();
	// 座標からマップチップ番号を計算
	IndexSet GetMapChipIndexSetByPosition(const Vector3& position);
	// ブロックの範囲取得
	Rect GetRectByIndex(uint32_t xIndex, uint32_t yINdex);

	void SetMapChipTypeByIndex(int xIndex, int yIndex, MapChipType type);
};
