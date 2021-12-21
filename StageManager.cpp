#include "stageManager.h"
#include "scene.h"

// �X�V����
void StageManager::Update(float elapsedTime)
{
	//�X�e�[�W��������
	for (int i = 0; i < StageMax; i++)
	{
		if (stages.at(i)->GetPosition().z + 10.0f < Player::Instance().GetPosition().z)
		{
			//�X�e�[�W���X�V����邽�тɏ�Q���̔z�u�ʒu���X�V
			for (int j = 0; j < 5; j++)
			{
				stages.at(i)->SquareRand[j] = rand() % 9;
			}

			if (Player::Instance().GetPosition().z > 500.0f &&
				(i == 1 || i == 3))
			{
				stages.at(i)->Pattern = 1;
			}
			else if(Player::Instance().GetPosition().z > 1000.0f &&
				(i == 2 || i == 4))
			{
				stages.at(i)->Pattern = 2;
			}

			//�X�e�[�W�X�V���ɐV�����p�^�[���������_���Ō���
			stages.at(i)->Rand = rand() % 5;
			//�X�e�[�W����ԑO�̃|�W�V�����Ɏ����Ă���
			stages.at(i)->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, (stages.at(i)->GetPosition().z + 10.0f * StageMax)));
		}
	}

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
	}
	stages.clear();
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