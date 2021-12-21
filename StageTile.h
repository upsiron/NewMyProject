#pragma once
#include "stage.h"

// ステージ
class StageTile : public Stage
{
public:
	StageTile();
	StageTile(std::shared_ptr<SkinnedMesh>& mesh) :Stage(mesh) {};
	~StageTile();

	// インスタンス取得
	//static StageTile& Instance();

	static Stage* GetInstance()
	{
		static Stage Instance;
		return &Instance;
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

	// デバッグ用GUI表示
	void DrawDebugGUI();

	enum MapTag
	{
		NONE = 0,
		HOLE,
		OBST,
		RED,
		GREEN,
		BLUE,
	};

	int stageTileMap[3][6][9] =
	{
		//0
		{
			//00
			{
			 NONE, OBST, NONE,
			 NONE, HOLE, NONE,
			 NONE, NONE, NONE
			 },
			//01
			{
			 NONE, HOLE, NONE,
			 NONE, HOLE, NONE,
			 NONE, NONE, NONE
			 },
			//02
			{
			 NONE, NONE, NONE,
			 HOLE, HOLE, HOLE,
			 NONE, NONE, NONE
			},
			//03
			{
			 NONE, NONE, NONE,
			 NONE, HOLE, HOLE,
			 NONE, NONE, NONE
			},
			//04
			{
			 NONE, NONE, NONE,
			 HOLE, HOLE, NONE,
			 NONE, NONE, NONE
			},
			//05
			{
			 NONE, NONE, NONE,
			 NONE, NONE, NONE,
			 NONE, NONE, NONE
			},
		},

		//1
		{
			//00
			{
			 NONE, NONE, RED,
			 NONE, HOLE, NONE,
			 NONE, NONE, NONE
			 },
			//01
			{
			 OBST, HOLE, NONE,
			 NONE, HOLE, NONE,
			 NONE, NONE, NONE
			 },
			//02
			{
			 NONE, RED, NONE,
			 HOLE, HOLE, HOLE,
			 NONE, NONE, NONE
			},
			//03
			{
			 NONE, NONE, OBST,
			 NONE, HOLE, HOLE,
			 NONE, NONE, NONE
			},
			//04
			{
			 RED, NONE, NONE,
			 HOLE, HOLE, NONE,
			 NONE, NONE, NONE
			},
			//05
			{
			 NONE, NONE, NONE,
			 NONE, NONE, NONE,
			 NONE, NONE, NONE
			},
		},

		//2
		{
			//2_0
			{
			 NONE, NONE, GREEN,
			 NONE, HOLE, NONE,
			 NONE, NONE, NONE
			 },
			//2_1
			{
			 NONE, HOLE, OBST,
			 NONE, HOLE, NONE,
			 NONE, NONE, NONE
			 },
			//2_2
			{
			 NONE, GREEN, NONE,
			 HOLE, HOLE, HOLE,
			 NONE, NONE, NONE
			},
			//2_3
			{
			 OBST, NONE, NONE,
			 NONE, HOLE, HOLE,
			 NONE, NONE, NONE
			},
			//2_4
			{
			 GREEN, NONE, NONE,
			 HOLE, HOLE, NONE,
			 NONE, NONE, NONE
			},
			//2_5
			{
			 NONE, NONE, NONE,
			 NONE, NONE, NONE,
			 NONE, NONE, NONE
			},
		},
	};

	int RayCast(const DirectX::XMFLOAT3& startPosition, const DirectX::XMFLOAT3& endPosition, DirectX::XMFLOAT3* outPosition, DirectX::XMFLOAT3* outNormal, float* outLength);

	//カラー getter/setter
	void SetTileColor(int c) { ColorNum = c; }
	int GetTileColor() { return ColorNum; }
private:

	int ColorNum = 0;

	DirectX::XMFLOAT4 TileColor[6] = {
		{1.0f, 1.0f, 1.0f, 1.0f},//NONE
		{1.0f, 1.0f, 1.0f, 1.0f},//NONE
		{1.0f, 1.0f, 1.0f, 1.0f},//NONE
		{1.0f, 0.0f, 0.0f, 1.0f},//RED
		{0.0f, 1.0f, 0.0f, 1.0f},//GREEN
		{0.0f, 0.0f, 1.0f, 1.0f},//BLUE
	};
};