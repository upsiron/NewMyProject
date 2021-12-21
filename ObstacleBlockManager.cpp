#include <imgui.h>
#include "ObstacleBlockManager.h"
#include "Collision.h"


// �X�V����
void ObstacleBlockManager::Update(float elapsedTime)
{
	for (Obstacle* obstacle : obstacles)
	{
		//if (enemy->GetPosition().y >= 0)
		{
			obstacle->Update(elapsedTime);
		}
	}

	// ��enemies�͈̔�for���̒���erase()����ƕs����������Ă��܂����߁A
	// �X�V�������I�������ɔj�����X�g�ɐς܂ꂽ�I�u�W�F�N�g���폜����B
	for (Obstacle* obstacle : removes)
	{
		// std::vector����v�f���폜����ꍇ�̓C�e���[�^�[�ō폜���Ȃ���΂Ȃ�Ȃ�
		std::vector<Obstacle*>::iterator it = std::find(obstacles.begin(), obstacles.end(), obstacle);
		if (it != obstacles.end())
		{
			obstacles.erase(it);
		}

		// �폜
		delete obstacle;
	}
	// �j�����X�g���N���A
	removes.clear();
}

void ObstacleBlockManager::Render(ID3D11DeviceContext* immediateContext,
	const DirectX::XMFLOAT4X4& view,
	const DirectX::XMFLOAT4X4& projection,
	const DirectX::XMFLOAT4& light,
	const DirectX::XMFLOAT4& materialColor,
	bool wireframe)
{
	for (Obstacle* obstacle : obstacles)
	{
		//��Q����Y���W��0���傫���Ȃ�`��
		if (obstacle->GetPosition().y >= 0)
		{
			obstacle->Render(immediateContext, view, projection, light, materialColor, wireframe);
		}
	}
}

// �G�l�~�[�o�^
void ObstacleBlockManager::Register(Obstacle* enemy)
{
	//���^AI�p�Ȃ̂ł킩��Ȃ�
	// ID��ݒ�
	//enemy->SetId(identity + static_cast<int>(Meta::Identity::Enemy));
	//identity++;//�ݒ肵����C���N�������g����


	// �o�^
	obstacles.emplace_back(enemy);
}
// �G�l�~�[�폜
void ObstacleBlockManager::Remove(Obstacle* enemy)
{
	// �j�����X�g�ɒǉ�
	removes.emplace_back(enemy);
}
void ObstacleBlockManager::Clear()
{
	for (Obstacle* obstacle : obstacles)
	{
		delete obstacle;
	}
	obstacles.clear();
}

void ObstacleBlockManager::DrawDebugPrimitive()
{
	for (Obstacle* obstacle : obstacles) {
		// �Փ˔���p�f�o�b�O����`��
		//enemy->DrawDebugPrimitive();
	}
}

void ObstacleBlockManager::DrawDebugGUI()
{
	ImGui::SetNextWindowPos(ImVec2(980, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Obstacle", nullptr, ImGuiWindowFlags_None))
	{
		for (Obstacle* obstacle : obstacles) {
			// �G�l�~�[���\��
			obstacle->DrawDebugGUI();
		}
	}
	ImGui::End();
}

// ID����G�l�~�[���擾����
Obstacle* ObstacleBlockManager::GetObstacleFromId(int id)
{
	for (Obstacle* obstacle : obstacles)
	{
		if (obstacle->GetId() == id)
			return obstacle;
	}
	return nullptr;
}