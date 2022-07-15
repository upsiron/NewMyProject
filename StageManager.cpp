#include "stageManager.h"
//#include "scene.h"
#include "Player.h"

// �X�V����
void StageManager::Update(float elapsedTime)
{
	//�X�e�[�W��������
	for (int i = 0; i < stageMax; i++)
	{
		if (stages.at(i)->GetPosition().z + 10.0f < Player::Instance().GetPosition().z)
		{

			/*if (Player::Instance().GetPosition().z >= 500.0f && Player::Instance().GetPosition().z <= 501.0f)
			{
				Player::Instance().PlayerSpeedUp(0.1f);
			}
			if (Player::Instance().GetPosition().z >= 1000.0f && Player::Instance().GetPosition().z <= 1001.0f)
			{
				Player::Instance().PlayerSpeedUp(0.1f);
			}
			if (Player::Instance().GetPosition().z >= 1500.0f && Player::Instance().GetPosition().z <= 1501.0f)
			{
				Player::Instance().PlayerSpeedUp(0.1f);
			}*/

			if (Player::Instance().GetPosition().z >= 500.0f && Player::Instance().GetPosition().z <= 501.0f)
			{
				//Player::Instance().PlayerSpeedUp(0.1f);
				stages.at(0)->stageRandWidth = 6;
			}

			//�X�e�[�W�X�V���Ƀ����_���Ō��肳�ꂽ�p�^�[�����X�V
			stages.at(i)->stageRand = r;
			//�X�e�[�W����ԑO�̃|�W�V�����Ɏ����Ă���
			stages.at(i)->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, (stages.at(i)->GetPosition().z + 10.0f * stageMax)));
		}
	}

	//���ɕ`�悳���X�e�[�W�̃p�^�[���������_���Ō���
	if (stages.at(0)->GetPosition().z < Player::Instance().GetPosition().z)r = rand() % stages.at(0)->stageRandWidth;

	//�X�V
	for (StageBase* stageBase : stages)
	{
		stageBase->Update(elapsedTime);
	}

	
	// ��stages�͈̔�for���̒���erase()����ƕs����������Ă��܂����߁A
	// �X�V�������I�������ɔj�����X�g�ɐς܂ꂽ�I�u�W�F�N�g���폜����B
	for (StageBase* stageBase : removes)
	{
		// std::vector����v�f���폜����ꍇ�̓C�e���[�^�[�ō폜���Ȃ���΂Ȃ�Ȃ�
		std::vector<StageBase*>::iterator it = std::find(stages.begin(), stages.end(), stageBase);
		if (it != stages.end())
		{
			stages.erase(it);
		}

		// �폜
		delete stageBase;
	}
	// �j�����X�g���N���A
	removes.clear();
}

void StageManager::Render(ID3D11DeviceContext* immediateContext,
	const DirectX::XMFLOAT4X4& view,
	const DirectX::XMFLOAT4X4& projection,
	const DirectX::XMFLOAT4& light,
	const DirectX::XMFLOAT4& materialColor,
	bool wireframe)
{
	for (StageBase* stageBase : stages)
	{
		stageBase->Render(immediateContext, view, projection, light, materialColor, wireframe);
	}
}

// �X�e�[�W�o�^
void StageManager::Register(StageBase* stage)
{
	// �o�^
	stages.emplace_back(stage);
}

// �X�e�[�W�폜
void StageManager::Remove(StageBase* stage)
{
	// �j�����X�g�ɒǉ�
	removes.emplace_back(stage);
}

void StageManager::Clear()
{
	for (StageBase* stageBase : stages)
	{
		delete stageBase;
		stageBase = nullptr;
	}
	stages.clear();
	stages.shrink_to_fit();
}

void StageManager::DrawDebugGUI()
{
	ImGui::SetNextWindowPos(ImVec2(980, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("StageBase", nullptr, ImGuiWindowFlags_None))
	{
		for (StageBase* stageBase : stages) {
			// �G�l�~�[���\��
			stageBase->DrawDebugGUI();
		}
	}
	ImGui::End();
}
