#include <imgui.h>
#include "ObstacleBlockManager.h"
#include "Collision.h"


// �X�V����
void ObstacleBlockManager::Update(float elapsedTime)
{
	for (Object* obstacle : obstacles)
	{
		//if (enemy->GetPosition().y >= 0)
		{
			obstacle->Update(elapsedTime);
		}
	}

	// ��obstacles�͈̔�for���̒���erase()����ƕs����������Ă��܂����߁A
	// �X�V�������I�������ɔj�����X�g�ɐς܂ꂽ�I�u�W�F�N�g���폜����B
	for (Object* obstacle : removes)
	{
		// std::vector����v�f���폜����ꍇ�̓C�e���[�^�[�ō폜���Ȃ���΂Ȃ�Ȃ�
		std::vector<Object*>::iterator it = std::find(obstacles.begin(), obstacles.end(), obstacle);
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

//�`�揈��
void ObstacleBlockManager::Render(ID3D11DeviceContext* immediateContext,
	const DirectX::XMFLOAT4X4& view,
	const DirectX::XMFLOAT4X4& projection,
	const DirectX::XMFLOAT4& light,
	const DirectX::XMFLOAT4& materialColor,
	bool wireframe)
{
	for (Object* obstacle : obstacles)
	{
		//��Q����Y���W��0���傫���Ȃ�`��
		if (obstacle->GetPosition().y >= -1.0f)
		{
			obstacle->Render(immediateContext, view, projection, light, materialColor, wireframe);
		}
	}
}

// ��Q���o�^
void ObstacleBlockManager::Register(Object* enemy)
{
	// �o�^
	obstacles.emplace_back(enemy);
}
// ��Q���폜
void ObstacleBlockManager::Remove(Object* obstacle)
{
	// �j�����X�g�ɒǉ�
	removes.emplace_back(obstacle);
}
void ObstacleBlockManager::Clear()
{
	for (Object* obstacle : obstacles)
	{
		delete obstacle;
		obstacle = nullptr;
	}
	obstacles.clear();
	obstacles.shrink_to_fit();
}

void ObstacleBlockManager::DrawDebugGUI()
{
	ImGui::SetNextWindowPos(ImVec2(980, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Obstacle", nullptr, ImGuiWindowFlags_None))
	{
		for (Object* obstacle : obstacles) {
			// ��Q�����\��
			obstacle->DrawDebugGUI();
		}
	}
	ImGui::End();
}

// ID�����Q�����擾����
Object* ObstacleBlockManager::GetObstacleFromId(int id)
{
	for (Object* obstacle : obstacles)
	{
		if (obstacle->GetId() == id)
			return obstacle;
	}
	return nullptr;
}