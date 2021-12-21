#pragma once

#include <vector>
#include "stageBase.h"

class StageManager
{
private:
	StageManager() {};
	~StageManager() {};

public:

	//�C���X�^���X�擾
	static StageManager& Instance()
	{
		static StageManager instance;
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
	void Register(StageBase* stage);

	// ������
	void Clear();

	// �f�o�b�O���\��
	void DrawDebugGUI();

	// �X�e�[�W���擾
	int GetStageCount()const { return static_cast<int>(stages.size()); }

	// �X�e�[�W�擾
	StageBase* GetStage(int index) { return stages.at(index); }

	// �X�e�[�W�폜
	void Remove(StageBase* stage);
private:
	std::vector<StageBase*> stages;
	std::vector<StageBase*> removes;
};