#pragma once

#include <vector>
#include "Enemy.h"

// エネミーマネージャー
class EnemyManager
{
private:
	EnemyManager() {}
	~EnemyManager() {}

public:

	// 唯一のインスタンス取得
	static EnemyManager& Instance()
	{
		static EnemyManager instance;
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

	// エネミー登録
	void Register(Enemy* enemy);

	// 初期化
	void Clear();

	// デバッグプリミティブ描画
	void DrawDebugPrimitive();

	// // デバッグ情報表示
	void DrawDebugGUI();

	// エネミー数取得
	int GetEnemyCount()const { return static_cast<int>(enemies.size()); }

	// エネミー取得
	Enemy* GetEnemy(int index) { return enemies.at(index); }

	// エネミー削除
	void Remove(Enemy* enemy);

	// 付与したIDからエネミーを取得
	Enemy* GetEnemyFromId(int id);

private:
	std::vector<Enemy*> enemies;
	std::vector<Enemy*> removes;

	// エネミー同士の衝突処理
	void CollisionEnemyVsEnemies();
public:
	int identity = 0;	// 付与するIDの値(この値にMetaAI::Identity::Enemyを加算して付与する)
}; 
