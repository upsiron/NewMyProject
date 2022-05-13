#pragma once

#include <vector>
#include "Obstacle.h"
#include "Object.h"

// 障害物マネージャー
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

	// 障害物登録
	void Register(Object* obstacle);

	// 初期化
	void Clear();

	// // デバッグ情報表示
	void DrawDebugGUI();

	// 障害物数取得
	int GetObstacleCount()const { return static_cast<int>(obstacles.size()); }

	// 障害物取得
	Object* GetObstacle(int index) { return obstacles.at(index); }

	// 障害物削除
	void Remove(Object* obstacle);

	// 付与したIDから障害物を取得
	Object* GetObstacleFromId(int id);

private:
	std::vector<Object*> obstacles;
	std::vector<Object*> removes;
};