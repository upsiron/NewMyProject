#include "stageTileManager.h"
#include "scene.h"

// 更新処理
void StageTileManager::Update(float elapsedTime)
{
	//更新
	for (StageTile* stageTile : stageTiles)
	{
		stageTile->Update(elapsedTime);
	}

	// ※stagesの範囲for文の中でerase()すると不具合が発生してしまうため、
	// 更新処理が終わった後に破棄リストに積まれたオブジェクトを削除する。
	for (StageTile* stageTile : removes)
	{
		// std::vectorから要素を削除する場合はイテレーターで削除しなければならない
		std::vector<StageTile*>::iterator it = std::find(stageTiles.begin(), stageTiles.end(), stageTile);
		if (it != stageTiles.end())
		{
			stageTiles.erase(it);
		}

		// 削除
		delete stageTile;
	}
	// 破棄リストをクリア
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

// ステージ登録
void StageTileManager::Register(StageTile* stage)
{
	// 登録
	stageTiles.emplace_back(stage);
}

// ステージ削除
void StageTileManager::Remove(StageTile* stage)
{
	// 破棄リストに追加
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
			// エネミー情報表示
			stageTile->DrawDebugGUI();
		}
	}
	ImGui::End();
}
