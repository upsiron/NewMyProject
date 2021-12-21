#pragma once

#include <vector>
#include "stageBase.h"

class StageManager
{
private:
	StageManager() {};
	~StageManager() {};

public:

	//インスタンス取得
	static StageManager& Instance()
	{
		static StageManager instance;
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
	void Register(StageBase* stage);

	// 初期化
	void Clear();

	// デバッグ情報表示
	void DrawDebugGUI();

	// ステージ数取得
	int GetStageCount()const { return static_cast<int>(stages.size()); }

	// ステージ取得
	StageBase* GetStage(int index) { return stages.at(index); }

	// ステージ削除
	void Remove(StageBase* stage);
private:
	std::vector<StageBase*> stages;
	std::vector<StageBase*> removes;
};