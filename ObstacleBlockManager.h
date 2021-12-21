#pragma once

#include <vector>
#include "Obstacle.h"

// エネミーマネージャー
class ObstacleBlockManager
{
private:
	ObstacleBlockManager() {}
	~ObstacleBlockManager() {}

public:

	// 唯一のインスタンス取得
	static ObstacleBlockManager& Instance()
	{
		static ObstacleBlockManager instance;
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
	void Register(Obstacle* obstacle);

	// 初期化
	void Clear();

	// デバッグプリミティブ描画
	void DrawDebugPrimitive();

	// // デバッグ情報表示
	void DrawDebugGUI();

	// エネミー数取得
	int GetObstacleCount()const { return static_cast<int>(obstacles.size()); }

	// エネミー取得
	Obstacle* GetObstacle(int index) { return obstacles.at(index); }

	// エネミー削除
	void Remove(Obstacle* obstacle);

	// 付与したIDからエネミーを取得
	Obstacle* GetObstacleFromId(int id);

private:
	std::vector<Obstacle*> obstacles;
	std::vector<Obstacle*> removes;

	// エネミー同士の衝突処理
	//void CollisionEnemyVsEnemies();
public:
	int identity = 0;	// 付与するIDの値(この値にMetaAI::Identity::Enemyを加算して付与する)
};