#include "stageManager.h"
#include "scene.h"

// 更新処理
void StageManager::Update(float elapsedTime)
{
	//ステージ無限処理
	for (int i = 0; i < StageMax; i++)
	{
		if (stages.at(i)->GetPosition().z + 10.0f < Player::Instance().GetPosition().z)
		{
			//ステージが更新されるたびに障害物の配置位置を更新
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

			//ステージ更新時に新しいパターンをランダムで決定
			stages.at(i)->Rand = rand() % 5;
			//ステージを一番前のポジションに持っていく
			stages.at(i)->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, (stages.at(i)->GetPosition().z + 10.0f * StageMax)));
		}
	}

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
			// エネミー情報表示
			stageBase->DrawDebugGUI();
		}
	}
	ImGui::End();
}
