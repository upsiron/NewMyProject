#pragma once
#include "Object.h"

class ObstacleBlock :public Object
{
public:
	ObstacleBlock();
	ObstacleBlock(std::shared_ptr<SkinnedMesh>& mesh) :
		Object(mesh) {};
	~ObstacleBlock()override;

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

	//カラー getter/setter
	void SetObstacleColor(int c) { ColorNum = c; }
	int GetObstacleColor() { return ColorNum; }
private:
	int ColorNum = 0;
	DirectX::XMFLOAT4 ObstacleColor[14] = {
		{1.5f, 1.5f, 1.5f, 1.0f},//NONE
		{1.5f, 1.5f, 1.5f, 1.0f},//NONE
		{1.5f, 1.5f, 1.5f, 1.0f},//NONE
		{1.5f, 1.5f, 1.5f, 1.0f},//NONE
		{1.5f, 1.5f, 1.5f, 1.0f},//NONE
		{0.0f, 3.0f, 3.0f, 1.0f},//MOVEBLUE
		{0.0f, 3.0f, 3.0f, 1.0f},//MOVEBLUE
		{0.0f, 3.0f, 3.0f, 1.0f},//MOVEBLUE
		{1.0f, 1.0f, 1.0f, 1.0f},//NONE
		{1.0f, 1.0f, 1.0f, 1.0f},//NONE
		{1.0f, 1.0f, 1.0f, 1.0f},//NONE
		{1.0f, 1.0f, 1.0f, 1.0f},//NONE
		{1.0f, 1.0f, 1.0f, 1.0f},//NONE
		{1.0f, 1.0f, 1.0f, 1.0f},//NONE
	};
};