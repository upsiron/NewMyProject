#include "stageTileManager.h"
#include "scene.h"

// �X�V����
void StageTileManager::Update(float elapsedTime)
{
	//�X�V
	for (StageTile* stageTile : stageTiles)
	{
		stageTile->Update(elapsedTime);
	}

	// ��stages�͈̔�for���̒���erase()����ƕs����������Ă��܂����߁A
	// �X�V�������I�������ɔj�����X�g�ɐς܂ꂽ�I�u�W�F�N�g���폜����B
	for (StageTile* stageTile : removes)
	{
		// std::vector����v�f���폜����ꍇ�̓C�e���[�^�[�ō폜���Ȃ���΂Ȃ�Ȃ�
		std::vector<StageTile*>::iterator it = std::find(stageTiles.begin(), stageTiles.end(), stageTile);
		if (it != stageTiles.end())
		{
			stageTiles.erase(it);
		}

		// �폜
		delete stageTile;
	}
	// �j�����X�g���N���A
	removes.clear();
}

void StageTileManager::Render(ID3D11DeviceContext* immediateContext,
	const DirectX::XMFLOAT4X4& view,
	const DirectX::XMFLOAT4X4& projection,
	const DirectX::XMFLOAT4& light,
	const DirectX::XMFLOAT4& materialColor,
	bool wireframe)
{
	for (StageTile* stageTile : stageTiles)
	{
		stageTile->Render(immediateContext, view, projection, light, materialColor, wireframe);
	}
}

// �X�e�[�W�o�^
void StageTileManager::Register(StageTile* stage)
{
	// �o�^
	stageTiles.emplace_back(stage);
}

// �X�e�[�W�폜
void StageTileManager::Remove(StageTile* stage)
{
	// �j�����X�g�ɒǉ�
	removes.emplace_back(stage);
}

void StageTileManager::Clear()
{
	for (StageTile* stageTile : stageTiles)
	{
		delete stageTile;
	}
	stageTiles.clear();
}

void StageTileManager::DrawDebugGUI()
{
	ImGui::SetNextWindowPos(ImVec2(980, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("StageBase", nullptr, ImGuiWindowFlags_None))
	{
		for (StageTile* stageTile : stageTiles) {
			// �G�l�~�[���\��
			stageTile->DrawDebugGUI();
		}
	}
	ImGui::End();
}
