#pragma once
#include <vector>
#include "Coin.h"
#include "Object.h"

// コインマネージャー
class CoinManager
{
private:
	CoinManager() {}
	~CoinManager() {}

public:

	// 唯一のインスタンス取得
	static CoinManager& Instance()
	{
		static CoinManager instance;
		return instance;
	}

	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Render(ID3D11DeviceContext* immediateContext,
		const DirectX::XMFLOAT4X4& view,
		const DirectX::XMFLOAT4X4& projection,
		const DirectX::XMFLOAT4& light,
		const DirectX::XMFLOAT4& materialColor,
		bool wireframe);

	// コイン登録
	void Register(Object* coin);

	// 初期化
	void Clear();

	// デバッグ情報表示
	void DrawDebugGUI();

	// コイン数取得
	int GetCoinCount()const { return static_cast<int>(coins.size()); }

	// コイン取得
	Object* GetCoin(int index) { return coins.at(index); }

	// コイン削除
	void Remove(Object* coin);

	// 付与したIDからコインを取得
	Object* GetCoinFromId(int id);

private:
	std::vector<Object*> coins;
	std::vector<Object*> removes;
};