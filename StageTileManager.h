#pragma once

#include <vector>
#include "stageTile.h"

class StageTileManager
{
private:
	StageTileManager() {};
	~StageTileManager() {};

public:

	//�C���X�^���X�擾
	static StageTileManager& Instance()
	{
		static StageTileManager instance;
		return instance;
	}

	// �X�V����
	void Update(float elapsedTime);

	// �`�揈��
	void Render(ID3D11DeviceContext* immediateContext,
		const DirectX::XMFLOAT4X4& view,
		const DirectX::XMFLOAT4X4& projection,
		const DirectX::XMFLOAT4& light,
		const DirectX::XMFLOAT4& materialColor,
		bool wireframe);

	// �X�e�[�W�o�^
	void Register(StageTile* stage);

	// ������
	void Clear();

	// �f�o�b�O���\��
	void DrawDebugGUI();

	// �X�e�[�W���擾
	int GetStageCount()const { return static_cast<int>(stageTiles.size()); }

	// �X�e�[�W�擾
	StageTile* GetStageTile(int index) { return stageTiles.at(index); }

	// �X�e�[�W�폜
	void Remove(StageTile* stage);
private:
	std::vector<StageTile*> stageTiles;
	std::vector<StageTile*> removes;
};