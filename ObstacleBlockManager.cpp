#include <imgui.h>
#include "ObstacleBlockManager.h"
#include "Collision.h"


// 更新処理
void ObstacleBlockManager::Update(float elapsedTime)
{
	for (Object* obstacle : obstacles)
	{
		//if (enemy->GetPosition().y >= 0)
		{
			obstacle->Update(elapsedTime);
		}
	}

	// ※obstaclesの範囲for文の中でerase()すると不具合が発生してしまうため、
	// 更新処理が終わった後に破棄リストに積まれたオブジェクトを削除する。
	for (Object* obstacle : removes)
	{
		// std::vectorから要素を削除する場合はイテレーターで削除しなければならない
		std::vector<Object*>::iterator it = std::find(obstacles.begin(), obstacles.end(), obstacle);
		if (it != obstacles.end())
		{
			obstacles.erase(it);
		}

		// 削除
		delete obstacle;
	}
	// 破棄リストをクリア
	removes.clear();
}

//描画処理
void ObstacleBlockManager::Render(ID3D11DeviceContext* immediateContext,
	const DirectX::XMFLOAT4X4& view,
	const DirectX::XMFLOAT4X4& projection,
	const DirectX::XMFLOAT4& light,
	const DirectX::XMFLOAT4& materialColor,
	bool wireframe)
{
	for (Object* obstacle : obstacles)
	{
		//障害物のY座標が0より大きいなら描画
		if (obstacle->GetPosition().y >= -1.0f)
		{
			obstacle->Render(immediateContext, view, projection, light, materialColor, wireframe);
		}
	}
}

// 障害物登録
void ObstacleBlockManager::Register(Object* enemy)
{
	// 登録
	obstacles.emplace_back(enemy);
}
// 障害物削除
void ObstacleBlockManager::Remove(Object* obstacle)
{
	// 破棄リストに追加
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
			// 障害物情報表示
			obstacle->DrawDebugGUI();
		}
	}
	ImGui::End();
}

// IDから障害物を取得する
Object* ObstacleBlockManager::GetObstacleFromId(int id)
{
	for (Object* obstacle : obstacles)
	{
		if (obstacle->GetId() == id)
			return obstacle;
	}
	return nullptr;
}