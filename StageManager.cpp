#include "stageManager.h"
//#include "scene.h"
#include "Player.h"

// 更新処理
void StageManager::Update(float elapsedTime)
{
	//ステージ無限処理
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

			//ステージ更新時にランダムで決定されたパターンを更新
			stages.at(i)->stageRand = r;
			//ステージを一番前のポジションに持っていく
			stages.at(i)->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, (stages.at(i)->GetPosition().z + 10.0f * stageMax)));
		}
	}

	//次に描画されるステージのパターンをランダムで決定
	if (stages.at(0)->GetPosition().z < Player::Instance().GetPosition().z)r = rand() % stages.at(0)->stageRandWidth;

	//更新
	for (StageBase* stageBase : stages)
	{
		stageBase->Update(elapsedTime);
	}

	
	// ※stagesの範囲for文の中でerase()すると不具合が発生してしまうため、
	// 更新処理が終わった後に破棄リストに積まれたオブジェクトを削除する。
	for (StageBase* stageBase : removes)
	{
		// std::vectorから要素を削除する場合はイテレーターで削除しなければならない
		std::vector<StageBase*>::iterator it = std::find(stages.begin(), stages.end(), stageBase);
		if (it != stages.end())
		{
			stages.erase(it);
		}

		// 削除
		delete stageBase;
	}
	// 破棄リストをクリア
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

// ステージ登録
void StageManager::Register(StageBase* stage)
{
	// 登録
	stages.emplace_back(stage);
}

// ステージ削除
void StageManager::Remove(StageBase* stage)
{
	// 破棄リストに追加
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
			// エネミー情報表示
			stageBase->DrawDebugGUI();
		}
	}
	ImGui::End();
}
