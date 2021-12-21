#include <imgui.h>
#include "ObstacleBlockManager.h"
#include "Collision.h"


// 更新処理
void ObstacleBlockManager::Update(float elapsedTime)
{
	for (Obstacle* obstacle : obstacles)
	{
		//if (enemy->GetPosition().y >= 0)
		{
			obstacle->Update(elapsedTime);
		}
	}

	// ※enemiesの範囲for文の中でerase()すると不具合が発生してしまうため、
	// 更新処理が終わった後に破棄リストに積まれたオブジェクトを削除する。
	for (Obstacle* obstacle : removes)
	{
		// std::vectorから要素を削除する場合はイテレーターで削除しなければならない
		std::vector<Obstacle*>::iterator it = std::find(obstacles.begin(), obstacles.end(), obstacle);
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

void ObstacleBlockManager::Render(ID3D11DeviceContext* immediateContext,
	const DirectX::XMFLOAT4X4& view,
	const DirectX::XMFLOAT4X4& projection,
	const DirectX::XMFLOAT4& light,
	const DirectX::XMFLOAT4& materialColor,
	bool wireframe)
{
	for (Obstacle* obstacle : obstacles)
	{
		//障害物のY座標が0より大きいなら描画
		if (obstacle->GetPosition().y >= 0)
		{
			obstacle->Render(immediateContext, view, projection, light, materialColor, wireframe);
		}
	}
}

// エネミー登録
void ObstacleBlockManager::Register(Obstacle* enemy)
{
	//メタAI用なのでわからない
	// IDを設定
	//enemy->SetId(identity + static_cast<int>(Meta::Identity::Enemy));
	//identity++;//設定したらインクリメントする


	// 登録
	obstacles.emplace_back(enemy);
}
// エネミー削除
void ObstacleBlockManager::Remove(Obstacle* enemy)
{
	// 破棄リストに追加
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
		// 衝突判定用デバッグ球を描画
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
			// エネミー情報表示
			obstacle->DrawDebugGUI();
		}
	}
	ImGui::End();
}

// IDからエネミーを取得する
Obstacle* ObstacleBlockManager::GetObstacleFromId(int id)
{
	for (Obstacle* obstacle : obstacles)
	{
		if (obstacle->GetId() == id)
			return obstacle;
	}
	return nullptr;
}