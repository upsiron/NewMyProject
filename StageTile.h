#pragma once
#include "stage.h"

// ステージ
class StageTile : public Stage
{
public:
	StageTile();
	StageTile(std::shared_ptr<SkinnedMesh>& mesh) :Stage(mesh) {};
	~StageTile();

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

	//マップ用のタグ
	enum MapTag
	{
		NONE = 0,
		HOLE,
		OB1,
		OB2,
		OB3,
		MOB1,
		MOB2,
		MOB3,
		RED,
		GREEN,
		BLUE,
		YELLOW,
		PURPLE,
		LIGHTBLUE,
		COIN1,
		COIN2,
		COIN3,
	};

	//読み込んだステージマップの数値を入れる
	int StageTileMapTxt[6][10][9] = { 0 };

	//実際にプログラム上で使うステージマップの変数
	//int StageTileMap[6][10][9] = { 0 };


	//ステージマップ
	int StageTileMap[10][10][9] =
	{
		//00
		{
			//00
			{
			 NONE, NONE, NONE,
			 NONE, NONE, NONE,
			 NONE, NONE, NONE
			 },
			//01
			{
			 NONE, NONE, NONE,
			 NONE, NONE, NONE,
			 NONE, NONE, NONE
			 },
			//02
			{
			 OB1,  OB2, NONE,
			 NONE, NONE, NONE,
			 NONE, NONE, NONE
			},
			//03
			{
			 NONE, NONE, NONE,
			 NONE, NONE, NONE,
			 NONE, NONE, NONE
			},
			//04
			{
			 NONE, NONE, NONE,
			 NONE, HOLE, BLUE,
			 NONE, NONE, NONE
			},
			//05
			{
			 NONE, NONE, NONE,
			 NONE, NONE, NONE,
			 NONE, NONE, NONE
			},
			//06
			{
			 OB1,  NONE, OB2,
			 NONE, NONE, NONE,
			 NONE, NONE, NONE
			 },
			//07
			{
			 NONE, NONE, NONE,
			 NONE, NONE, NONE,
			 NONE, HOLE, NONE
			 },
			//08
			{
			 BLUE, BLUE, BLUE,
			 NONE, NONE, COIN1,
			 NONE, NONE, NONE
			},
			//09
			{
			 NONE, NONE, NONE,
			 NONE, NONE, NONE,
			 NONE, NONE, NONE
			},
		},

		//01
		{
			//00
			{
			 NONE, NONE, NONE,
			 NONE, NONE, NONE,
			 NONE, NONE, NONE
			 },
			//01
			{
			 NONE, NONE, NONE,
			 NONE, NONE, NONE,
			 NONE, NONE, NONE
			 },
			//02
			{
			 OB1, OB2, OB3,
			 NONE, NONE, NONE,
			 GREEN, GREEN, GREEN
			},
			//03
			{
			 NONE, NONE, NONE,
			 NONE, NONE, NONE,
			 NONE, NONE, NONE
			},
			//04
			{
			 GREEN, NONE, NONE,
			 NONE, NONE, NONE,
			 NONE, NONE, NONE
			},
			//05
			{
			 NONE, NONE, NONE,
			 NONE, OB3, HOLE,
			 OB1,  OB2, NONE
			},
			//06
			{
			 COIN2, NONE, NONE,
			 NONE, NONE, NONE,
			 NONE, NONE, NONE
			 },
			//07
			{
			 NONE, NONE, NONE,
			 NONE, NONE, NONE,
			 NONE, NONE, NONE
			 },
			//08
			{
			 HOLE, OB1, OB2,
			 NONE, NONE, NONE,
			 NONE, NONE, NONE
			},
			//09
			{
			 NONE, NONE, NONE,
			 NONE, NONE, NONE,
			 NONE, NONE, NONE
			},
		},

		//02
		{
			//00
			{
			 NONE, NONE, NONE,
			 NONE, NONE, NONE,
			 NONE, NONE, NONE
			 },
			//01
			{
			 NONE, NONE, NONE,
			 NONE, NONE, NONE,
			 NONE, NONE, LIGHTBLUE
			 },
			//02
			{
			 NONE, NONE, NONE,
			 NONE, NONE, NONE,
			 NONE, NONE, NONE
			},
			//03
			{
			 NONE, NONE, NONE,
			 MOB3, OB1, MOB1,
			 NONE, NONE, NONE
			},
			//04
			{
			 NONE, NONE, NONE,
			 NONE, NONE, NONE,
			 NONE, NONE, NONE
			},
			//05
			{
			 NONE, NONE, NONE,
			 HOLE, LIGHTBLUE, HOLE,
			 NONE, NONE, NONE
			},
			//06
			{
			 NONE, NONE, NONE,
			 NONE, NONE, NONE,
			 NONE, NONE, NONE
			 },
			//07
			{
			 NONE, NONE, NONE,
			 NONE, NONE, NONE,
			 NONE, NONE, NONE
			 },
			//08
			{
			 NONE, OB1, COIN3,
			 NONE, OB2, NONE,
			 MOB1, OB3, MOB3
			},
			//09
			{
			 NONE, NONE, NONE,
			 NONE, NONE, NONE,
			 NONE, NONE, NONE
			},
		},

		//03
		{
			//00
			{
			NONE, NONE, NONE,
			NONE, NONE, NONE,
			NONE, NONE, NONE
			},
			//01
			{
			 NONE, NONE, NONE,
			 NONE, NONE, NONE,
			 NONE, NONE, NONE
			 },
			//02
			{
			 NONE, NONE, NONE,
			 YELLOW, HOLE, YELLOW,
			 NONE, NONE, NONE
			},
			//03
			{
			 NONE, NONE, NONE,
			 NONE, NONE, NONE,
			 NONE, NONE, NONE
			},
			//04
			{
			 NONE, NONE, NONE,
			 NONE, OB1, NONE,
			 NONE, NONE, NONE
			},
			//05
			{
			 YELLOW, YELLOW, YELLOW,
			 NONE, NONE, NONE,
			 NONE, NONE, NONE
			},
			//06
			{
			 NONE, NONE, NONE,
			 NONE, NONE, NONE,
			 NONE, NONE, NONE
			 },
			//07
			{
			 NONE, NONE, NONE,
			 OB1, NONE, OB2,
			 NONE, NONE, NONE
			 },
			//08
			{
			 NONE, NONE, NONE,
			 COIN1, NONE, NONE,
			 NONE, NONE, NONE
			},
			//09
			{
			 NONE, NONE, NONE,
			 NONE, NONE, NONE,
			 NONE, NONE, NONE
			},
		},

		//04
		{
			//00
			{
			NONE, NONE, NONE,
			NONE, NONE, NONE,
			NONE, NONE, NONE
			},
			//01
			{
			 NONE, NONE, NONE,
			 NONE, NONE, NONE,
			 NONE, NONE, NONE
			 },
			//02
			{
			 PURPLE, PURPLE, PURPLE,
			 NONE, NONE, NONE,
			 NONE, NONE, NONE
			},
			//03
			{
			 NONE, HOLE, NONE,
			 NONE, NONE, NONE,
			 NONE, NONE, NONE
			},
			//04
			{
			COIN1, NONE, NONE,
			HOLE, NONE, HOLE,
			NONE, NONE, NONE
			},
			//05
			{
			 NONE, NONE, NONE,
			 OB1, NONE, PURPLE,
			 NONE, HOLE, NONE
			},
			//06
			{
			 NONE, COIN2, NONE,
			 NONE, NONE, NONE,
			 NONE, NONE, NONE
			 },
			//07
			{
			 NONE, NONE, NONE,
			 NONE, OB1, OB2,
			 NONE, NONE, NONE
			 },
			//08
			{
			 NONE, NONE, NONE,
			 NONE, NONE, NONE,
			 NONE, NONE, NONE
			},
			//09
			{
			 NONE, NONE, NONE,
			 NONE, NONE, NONE,
			 NONE, NONE, NONE
			},
		},

		//05
		{
			//00
			{
			 NONE, NONE, NONE,
			 NONE, NONE, NONE,
			 NONE, NONE, NONE
			},
			//01
			{
			 NONE, NONE, NONE,
			 NONE, NONE, NONE,
			 NONE, NONE, NONE
			 },
			//02
			{
			 NONE, NONE, NONE,
			 RED, RED, RED,
			 NONE, NONE, NONE
			},
			//03
			{
			 NONE, NONE, NONE,
			 OB1, NONE, OB2,
			 NONE, NONE, NONE
			},
			//04
			{
			NONE, NONE, NONE,
			NONE, NONE, COIN1,
			NONE, NONE, NONE
			},
			//05
			{
			 NONE, NONE, NONE,
			 HOLE, NONE, RED,
			 NONE, NONE, NONE
			},
			//06
			{
			 NONE, NONE, NONE,
			 NONE, RED, NONE,
			 NONE, NONE, NONE
			 },
			//07
			{
			 NONE, NONE, NONE,
			 COIN2, OB1, RED,
			 NONE, NONE, NONE
			 },
			//08
			{
			 NONE, NONE, NONE,
			 NONE, NONE, NONE,
			 NONE, NONE, NONE
			},
			//09
			{
			 NONE, NONE, NONE,
			 NONE, NONE, NONE,
			 NONE, NONE, NONE
			},
		},

		//06
		{
			//00
			{
			 NONE, NONE, NONE,
			 NONE, YELLOW, NONE,
			 NONE, NONE, NONE
			 },
			//01
			{
			 OB1,  OB2,  NONE,
			 NONE, NONE, COIN1,
			 NONE, NONE, NONE
			 },
			//02
			{
			 HOLE, HOLE, NONE,
			 NONE, NONE, NONE,
			 NONE, NONE, NONE
			},
			//03
			{
			 NONE, NONE, NONE,
			 NONE, NONE, NONE,
			 NONE, OB1,  NONE
			},
			//04
			{
			 NONE, NONE, NONE,
			 NONE, NONE, NONE,
			 OB1,  NONE, OB2
			},
			//05
			{
			 COIN2, NONE, NONE,
			 BLUE,  HOLE, RED,
			 NONE, NONE, NONE
			},
			//06
			{
			 NONE, OB1,  NONE,
			 NONE, NONE, NONE,
			 NONE, NONE, NONE
			 },
			//07
			{
			 NONE, NONE, NONE,
			 HOLE, NONE, NONE,
			 NONE, NONE, COIN3
			 },
			//08
			{
			 NONE, NONE, NONE,
			 PURPLE, NONE, NONE,
			 NONE, OB1,  OB2
			},
			//09
			{
			 OB1,  OB2,  OB3,
			 NONE, NONE, NONE,
			 GREEN, GREEN, GREEN
			},
		},

		//07
		{
			//00
			{
			 NONE, NONE, NONE,
			 NONE, COIN1, NONE,
			 NONE, NONE, NONE
			 },
			//01
			{
			 NONE, NONE, NONE,
			 NONE, NONE, NONE,
			 OB1,  NONE, OB2
			 },
			//02
			{
			 NONE, HOLE, HOLE,
			 NONE, HOLE, HOLE,
			 NONE, HOLE, HOLE
			},
			//03
			{
			 NONE, NONE, NONE,
			 NONE, BLUE, RED,
			 NONE, NONE, NONE
			},
			//04
			{
			 NONE, NONE, NONE,
			 NONE, NONE, NONE,
			 OB1,  OB2,  NONE
			},
			//05
			{
			 NONE, OB1,  OB2,
			 NONE, NONE, NONE,
			 YELLOW, GREEN, GREEN
			},
			//06
			{
			 OB1,  RED,  NONE,
			 NONE, NONE, NONE,
			 NONE, NONE, BLUE
			 },
			//07
			{
			 NONE, OB1,  NONE,
			 NONE, NONE, HOLE,
			 NONE, NONE, NONE
			 },
			//08
			{
			 NONE, NONE,  NONE,
			 HOLE, HOLE, HOLE,
			 NONE, NONE, NONE
			},
			//09
			{
			 NONE, HOLE, NONE,
			 NONE, HOLE, NONE,
			 NONE, NONE, NONE
			},
		},

		//08
		{
			//00
			{
			 NONE, NONE, NONE,
			 NONE, NONE, NONE,
			 NONE, NONE, NONE
			 },
			//01
			{
			 NONE, NONE, NONE,
			 NONE, BLUE, NONE,
			 NONE, NONE, NONE
			 },
			//02
			{
			 NONE, NONE, NONE,
			 PURPLE, PURPLE, PURPLE,
			 NONE, NONE, NONE
			},
			//03
			{
			 MOB3,  OB1, MOB1,
			 NONE, NONE, NONE,
			 NONE, NONE, NONE
			},
			//04
			{
			 NONE, NONE, NONE,
			 NONE, BLUE, NONE,
			 NONE, NONE, NONE
			},
			//05
			{
			 YELLOW, NONE, YELLOW,
			 NONE, NONE, NONE,
			 NONE, NONE, NONE
			},
			//06
			{
			 MOB1, OB2,  MOB3,
			 NONE, NONE, NONE,
			 NONE, NONE, NONE
			 },
			//07
			{
			 NONE, OB1, HOLE,
			 RED,  OB2, NONE,
			 HOLE, OB3, NONE
			 },
			//08
			{
			 NONE, OB1, NONE,
			 HOLE, OB2, NONE,
			 NONE, OB3, NONE
			},
			//09
			{
			 NONE, NONE, NONE,
			 NONE, NONE, NONE,
			 NONE, NONE, NONE
			},
		},

	    //09
		{
		    //00
		    {
		    NONE, NONE, NONE,
		    NONE, NONE, NONE,
		    NONE, NONE, NONE
		    },
		    //01
		    {
		     NONE, NONE, NONE,
		     NONE, NONE, NONE,
		     NONE, NONE, NONE
		     },
		    //02
		    {
			 NONE, NONE, NONE,
			 NONE, NONE, NONE,
			 NONE, NONE, NONE
		    },
		    //03
		    {
		     NONE, NONE, NONE,
		     NONE, NONE, NONE,
		     NONE, NONE, NONE
		    },
		    //04
		    {
			NONE, NONE, NONE,
			NONE, NONE, NONE,
			NONE, NONE, NONE
		    },
		    //05
		    {
		     NONE, NONE, NONE,
		     NONE, NONE, NONE,
		     NONE, NONE, NONE
		    },
		    //06
		    {
			 NONE, NONE, NONE,
			 NONE, NONE, NONE,
			 NONE, NONE, NONE
		     },
		    //07
		    {
			 NONE, NONE, NONE,
			 NONE, NONE, NONE,
			 NONE, NONE, NONE
		     },
		    //08
		    {
			 NONE, NONE, NONE,
			 NONE, NONE, NONE,
			 NONE, NONE, NONE
		    },
		    //09
		    {
		     NONE, NONE, NONE,
		     NONE, NONE, NONE,
		     NONE, NONE, NONE
		    },
		},
	};

	//カラー getter/setter
	void SetTileColor(int c) { ColorNum = c; }
	int GetTileColor() { return ColorNum; }

private:

	//カラー番号
	int ColorNum = 0;

	DirectX::XMFLOAT4 TileColor[17] = {
		{1.0f, 1.0f, 1.0f, 1.0f},//0:NONE
		{1.0f, 1.0f, 1.0f, 1.0f},//1:NONE
		{1.0f, 1.0f, 1.0f, 1.0f},//2:NONE
		{1.0f, 1.0f, 1.0f, 1.0f},//3:NONE
		{1.0f, 1.0f, 1.0f, 1.0f},//4:NONE
		{1.0f, 1.0f, 1.0f, 1.0f},//5:NONE
		{1.0f, 1.0f, 1.0f, 1.0f},//6:NONE
		{1.0f, 1.0f, 1.0f, 1.0f},//7:NONE
		{3.5f, 0.0f, 0.0f, 1.0f},//8:RED
		{0.0f, 2.5f, 0.0f, 1.0f},//9:GREEN
		{0.0f, 0.0f, 4.0f, 1.0f},//10:BLUE
		{2.0f, 2.0f, 0.0f, 1.0f},//11:YELLOW
		{2.0f, 0.0f, 2.0f, 1.0f},//12:PURPLE
		{0.0f, 3.0f, 3.0f, 1.0f},//13:LIGHTBLUE
		{1.0f, 1.0f, 1.0f, 1.0f},//14:COIN
		{1.0f, 1.0f, 1.0f, 1.0f},//15:COIN
		{1.0f, 1.0f, 1.0f, 1.0f},//16:COIN
	};

	//ステージの隙間埋め合わせスケール
	float StageScaleGap = 0.7f;
};