#pragma once

#include <vector>
#include "stageTile.h"

class StageTileManager
{
private:
	StageTileManager() {};
	~StageTileManager() {};

public:

	//インスタンス取得
	static StageTileManager& Instance()
	{
		static StageTileManager instance;
		return instance;
	}

	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Render(ID3D11DeviceContext* immediateContext,
		const DirectX::XMFLOAT4X4& view,
		const DirectX::XMFLOAT4X4& projection,
		const DirectX::XMFLOAT4& light,
		const DirectX::XMFLOAT4& materialColor,
		bool wireframe);

	// ステージ登録
	void Register(StageTile* stage);

	// 初期化
	void Clear();

	// デバッグ情報表示
	void DrawDebugGUI();

	// ステージ数取得
	int GetStageCount()const { return static_cast<int>(stageTiles.size()); }

	// ステージ取得
	StageTile* GetStageTile(int index) { return stageTiles.at(index); }

	// ステージ削除
	void Remove(StageTile* stage);
private:
	std::vector<StageTile*> stageTiles;
	std::vector<StageTile*> removes;
};