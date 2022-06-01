#pragma once
#include "Object.h"

class Coin :public Object
{
public:
	Coin();
	Coin(std::shared_ptr<SkinnedMesh>& mesh) :
		Object(mesh) {};
	~Coin()override;

	// 更新処理
	void Update(float elapsedTime)override;

	// 描画処理
	void Render(ID3D11DeviceContext* immediateContext,
		const DirectX::XMFLOAT4X4& view,
		const DirectX::XMFLOAT4X4& projection,
		const DirectX::XMFLOAT4& light,
		const DirectX::XMFLOAT4& materialColor,
		bool wireframe)override;

	// デバッグエネミー情報表示
	void DrawDebugGUI();
};
