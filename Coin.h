#pragma once
#include "Object.h"

class Coin :public Object
{
public:
	Coin();
	Coin(std::shared_ptr<SkinnedMesh>& mesh) :
		Object(mesh) {};
	~Coin()override;

	// �X�V����
	void Update(float elapsedTime)override;

	// �`�揈��
	void Render(ID3D11DeviceContext* immediateContext,
		const DirectX::XMFLOAT4X4& view,
		const DirectX::XMFLOAT4X4& projection,
		const DirectX::XMFLOAT4& light,
		const DirectX::XMFLOAT4& materialColor,
		bool wireframe)override;

	// �f�o�b�O�G�l�~�[���\��
	void DrawDebugGUI();

	//�J���[ getter/setter
	void SetObstacleColor(int c) { ColorNum = c; }
	int GetObstacleColor() { return ColorNum; }
private:
	int ColorNum = 0;
	DirectX::XMFLOAT4 ObstacleColor[11] = {
		{1.0f, 1.0f, 1.0f, 1.0f},//NONE
		{1.0f, 1.0f, 1.0f, 1.0f},//NONE
		{1.0f, 1.0f, 1.0f, 1.0f},//NONE
		{1.0f, 1.0f, 1.0f, 1.0f},//NONE
		{1.0f, 1.0f, 1.0f, 1.0f},//NONE
		{0.0f, 0.0f, 1.0f, 1.0f},//MOVEBLUE
		{0.0f, 0.0f, 1.0f, 1.0f},//MOVEBLUE
		{0.0f, 0.0f, 1.0f, 1.0f},//MOVEBLUE
		{1.0f, 1.0f, 1.0f, 1.0f},//NONE
		{1.0f, 1.0f, 1.0f, 1.0f},//NONE
		{1.0f, 1.0f, 1.0f, 1.0f},//NONE
	};
};
