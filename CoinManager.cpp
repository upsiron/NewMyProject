#include <imgui.h>
#include "CoinManager.h"
#include "Collision.h"


// 更新処理
void CoinManager::Update(float elapsedTime)
{
	for (Object* coin : coins)
	{
		//if (enemy->GetPosition().y >= 0)
		{
			coin->Update(elapsedTime);
		}
	}

	// ※coinsの範囲for文の中でerase()すると不具合が発生してしまうため、
	// 更新処理が終わった後に破棄リストに積まれたオブジェクトを削除する。
	for (Object* coin : removes)
	{
		// std::vectorから要素を削除する場合はイテレーターで削除しなければならない
		std::vector<Object*>::iterator it = std::find(coins.begin(), coins.end(), coin);
		if (it != coins.end())
		{
			coins.erase(it);
		}

		// 削除
		delete coin;
	}
	// 破棄リストをクリア
	removes.clear();
}

//描画処理
void CoinManager::Render(ID3D11DeviceContext* immediateContext,
	const DirectX::XMFLOAT4X4& view,
	const DirectX::XMFLOAT4X4& projection,
	const DirectX::XMFLOAT4& light,
	const DirectX::XMFLOAT4& materialColor,
	bool wireframe)
{
	for (Object* coin : coins)
	{
		//障害物のY座標が0より大きいなら描画
		if (coin->GetPosition().y >= -1.0f)
		{
			coin->Render(immediateContext, view, projection, light, materialColor, wireframe);
		}
	}
}

// コイン登録
void CoinManager::Register(Object* coin)
{
	// 登録
	coins.emplace_back(coin);
}
// コイン削除
void CoinManager::Remove(Object* coin)
{
	// 破棄リストに追加
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
			// エネミー情報表示
			coin->DrawDebugGUI();
		}
	}
	ImGui::End();
}

// IDからコインを取得する
Object* CoinManager::GetCoinFromId(int id)
{
	for (Object* coin : coins)
	{
		if (coin->GetId() == id)
			return coin;
	}
	return nullptr;
}