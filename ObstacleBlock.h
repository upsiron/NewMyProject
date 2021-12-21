#pragma once
#include "Obstacle.h"

class ObstacleBlock :public Obstacle
{
public:
	ObstacleBlock();
	ObstacleBlock(std::shared_ptr<SkinnedMesh>& mesh) :
		Obstacle(mesh) {};
	~ObstacleBlock()override;

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
private:

};