#include <imgui.h>
#include "CoinManager.h"
#include "Collision.h"


// �X�V����
void CoinManager::Update(float elapsedTime)
{
	for (Object* coin : coins)
	{
		//if (enemy->GetPosition().y >= 0)
		{
			coin->Update(elapsedTime);
		}
	}

	// ��coins�͈̔�for���̒���erase()����ƕs����������Ă��܂����߁A
	// �X�V�������I�������ɔj�����X�g�ɐς܂ꂽ�I�u�W�F�N�g���폜����B
	for (Object* coin : removes)
	{
		// std::vector����v�f���폜����ꍇ�̓C�e���[�^�[�ō폜���Ȃ���΂Ȃ�Ȃ�
		std::vector<Object*>::iterator it = std::find(coins.begin(), coins.end(), coin);
		if (it != coins.end())
		{
			coins.erase(it);
		}

		// �폜
		delete coin;
	}
	// �j�����X�g���N���A
	removes.clear();
}

//�`�揈��
void CoinManager::Render(ID3D11DeviceContext* immediateContext,
	const DirectX::XMFLOAT4X4& view,
	const DirectX::XMFLOAT4X4& projection,
	const DirectX::XMFLOAT4& light,
	const DirectX::XMFLOAT4& materialColor,
	bool wireframe)
{
	for (Object* coin : coins)
	{
		//��Q����Y���W��0���傫���Ȃ�`��
		if (coin->GetPosition().y >= -1.0f)
		{
			coin->Render(immediateContext, view, projection, light, materialColor, wireframe);
		}
	}
}

// �R�C���o�^
void CoinManager::Register(Object* coin)
{
	// �o�^
	coins.emplace_back(coin);
}
// �R�C���폜
void CoinManager::Remove(Object* coin)
{
	// �j�����X�g�ɒǉ�
	removes.emplace_back(coin);
}
void CoinManager::Clear()
{
	for (Object* coin : coins)
	{
		delete coin;
		coin = nullptr;
	}
	coins.clear();
	coins.shrink_to_fit();
}

void CoinManager::DrawDebugGUI()
{
	ImGui::SetNextWindowPos(ImVec2(980, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Coin", nullptr, ImGuiWindowFlags_None))
	{
		for (Object* coin : coins) {
			// �G�l�~�[���\��
			coin->DrawDebugGUI();
		}
	}
	ImGui::End();
}

// ID����R�C�����擾����
Object* CoinManager::GetCoinFromId(int id)
{
	for (Object* coin : coins)
	{
		if (coin->GetId() == id)
			return coin;
	}
	return nullptr;
}