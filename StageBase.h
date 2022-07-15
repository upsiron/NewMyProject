#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "stage.h"

// ステージ
class StageBase : public Stage
{
public:
	static StageBase* GetInstance()
	{
		static StageBase Instance;
		return &Instance;
	}


public:
	StageBase();
	StageBase(std::shared_ptr<SkinnedMesh>& mesh) :Stage(mesh) {};
	~StageBase();

	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Render(ID3D11DeviceContext* immediateContext,
		const DirectX::XMFLOAT4X4& view,
		const DirectX::XMFLOAT4X4& projection,
		const DirectX::XMFLOAT4& light,
		const DirectX::XMFLOAT4& materialColor,
		bool wireframe);

	// デバッグ用GUI表示
	void DrawDebugGUI();

	//ステージパターンランダム配置用変数
	int stageRand = 0;
	int stageRandWidth = 0;
};
