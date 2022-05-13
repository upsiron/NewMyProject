#include <imgui.h>
#include "EnemyManager.h"
//#include "MetaAI.h"
#include "Collision.h"


// 更新処理
void EnemyManager::Update(float elapsedTime)
{
	for (Enemy* enemy : enemies)
	{
		//if (enemy->GetPosition().y >= 0)
		{
			enemy->Update(elapsedTime);
		}
	}

	// ※enemiesの範囲for文の中でerase()すると不具合が発生してしまうため、
	// 更新処理が終わった後に破棄リストに積まれたオブジェクトを削除する。
	for (Enemy* enemy : removes)
	{
		// std::vectorから要素を削除する場合はイテレーターで削除しなければならない
		std::vector<Enemy*>::iterator it = std::find(enemies.begin(), enemies.end(), enemy);
		if (it != enemies.end())
		{
			enemies.erase(it);
		}

		// 削除
		delete enemy;
	}
	// 破棄リストをクリア
	removes.clear();
	// 敵同士の衝突判定
	CollisionEnemyVsEnemies();
}

void EnemyManager::Render(ID3D11DeviceContext* immediateContext,
	const DirectX::XMFLOAT4X4& view,
	const DirectX::XMFLOAT4X4& projection,
	const DirectX::XMFLOAT4& light,
	const DirectX::XMFLOAT4& materialColor,
	bool wireframe)
{
	for (Enemy* enemy : enemies)
	{
		//障害物のY座標が0より大きいなら描画
		if (enemy->GetPosition().y >= 0)
		{
			enemy->Render(immediateContext, view, projection, light, materialColor, wireframe);
		}
	}
}

// エネミー登録
void EnemyManager::Register(Enemy* enemy)
{
	//メタAI用なのでわからない
	// IDを設定
	//enemy->SetId(identity + static_cast<int>(Meta::Identity::Enemy));
	//identity++;//設定したらインクリメントする
	

	// 登録
	enemies.emplace_back(enemy);
}
// エネミー削除
void EnemyManager::Remove(Enemy* enemy)
{
	// 破棄リストに追加
	removes.emplace_back(enemy);
}
void EnemyManager::Clear()
{
	for (Enemy* enemy : enemies)
	{
		delete enemy;
		enemy = nullptr;
	}
	enemies.clear();
	enemies.shrink_to_fit();
}

void EnemyManager::DrawDebugPrimitive()
{
	for (Enemy* enemy : enemies) {
		// 衝突判定用デバッグ球を描画
		//enemy->DrawDebugPrimitive();
	}
}

void EnemyManager::DrawDebugGUI()
{
	ImGui::SetNextWindowPos(ImVec2(980, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Enemy", nullptr, ImGuiWindowFlags_None))
	{
		for (Enemy* enemy : enemies) {
			// エネミー情報表示
			enemy->DrawDebugGUI();
		}
	}
	ImGui::End();
}
void EnemyManager::CollisionEnemyVsEnemies()
{
	// 全ての敵と総当たりで衝突処理
	//for (int i = 0; i < enemies.size(); ++i)
	//{
	//	Enemy* enemyA = enemies.at(i);
	//	for (int j = i + 1; j < enemies.size(); ++j)
	//	{
	//		Enemy* enemyB = enemies.at(j);
	//		// 衝突処理
	//		DirectX::XMFLOAT3 outPosition;
	//		if (Collision::IntersectSphereVsSphere(
	//			enemyA->GetPosition(),
	//			enemyA->GetRadius(),
	//			enemyB->GetPosition(),
	//			enemyB->GetRadius(),
	//			outPosition)
	//			)
	//		{
	//			// 押し出し後の位置設定
	//			enemyB->SetPosition(outPosition);
	//		}
	//	}
	//}
}
// IDからエネミーを取得する
Enemy* EnemyManager::GetEnemyFromId(int id)
{
	for (Enemy* enemy : enemies)
	{
		if (enemy->GetId() == id)
			return enemy;
	}
	return nullptr;
}