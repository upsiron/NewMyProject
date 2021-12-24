#pragma once
#include "stage.h"

// �X�e�[�W
class StageTile : public Stage
{
public:
	StageTile();
	StageTile(std::shared_ptr<SkinnedMesh>& mesh) :Stage(mesh) {};
	~StageTile();

	// �C���X�^���X�擾
	//static StageTile& Instance();

	static Stage* GetInstance()
	{
		static Stage Instance;
		return &Instance;
	}


	// �X�V����
	void Update(float elapsedTime);

	// �`�揈��
	void Render(ID3D11DeviceContext* immediateContext,
		const DirectX::XMFLOAT4X4& view,
		const DirectX::XMFLOAT4X4& projection,
		const DirectX::XMFLOAT4& light,
		const DirectX::XMFLOAT4& materialColor,
		bool wireframe);

	// �f�o�b�O�pGUI�\��
	void DrawDebugGUI();

	//�}�b�v�p�̃^�O
	enum MapTag
	{
		NONE = 0,
		HOLE,
		OB1,
		OB2,
		OB3,
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
			 OB1,  OB2,  NONE,
			 NONE, NONE, NONE,
			 NONE, NONE, NONE
			 },
			//01
			{
			 NONE, NONE, NONE,
			 HOLE, HOLE, HOLE,
			 NONE, NONE, NONE
			 },
			//02
			{
			 OB1,  NONE, OB2,
			 NONE, NONE, NONE,
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
			 NONE, NONE, NONE,
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
			 NONE, NONE, NONE,
			 NONE, NONE, NONE
			 },
			//01
			{
			 OB1,  HOLE, NONE,
			 NONE, HOLE, NONE,
			 NONE, NONE, NONE
			 },
			//02
			{
			 NONE, RED,  NONE,
			 HOLE, HOLE, HOLE,
			 NONE, NONE, NONE
			},
			//03
			{
			 NONE, NONE, NONE,
			 HOLE, HOLE, NONE,
			 NONE, NONE, NONE
			},
			//04
			{
			 RED,  NONE, OB1,
			 NONE, NONE, NONE,
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
			 NONE, HOLE, OB1,
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
			 OB1, NONE, NONE,
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

	//easy
	int stageTileMapEasy[3][10][9] =
	{
		//0
		{
			//00
			{
			 NONE, NONE, NONE,
			 NONE, NONE, NONE,
			 NONE, NONE, NONE
			 },
			//01
			{
			 OB1,  OB2,  NONE,
			 NONE, NONE, NONE,
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
			 NONE, NONE, NONE,
			 RED,  HOLE, RED,
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
			 NONE, NONE, NONE
			 },
			//08
			{
			 NONE, NONE, NONE,
			 NONE, NONE, NONE,
			 NONE, OB1,  OB2
			},
			//09
			{
			 OB1,  OB2,  OB3,
			 NONE, NONE, NONE,
			 GREEN, GREEN, GREEN
			},
		},

		//1
		{
			//00
			{
			 OB1,  OB2,  NONE,
			 NONE, NONE, NONE,
			 NONE, NONE, NONE
			 },
			//01
			{
			 NONE, NONE, NONE,
			 HOLE, HOLE, HOLE,
			 NONE, NONE, NONE
			 },
			//02
			{
			 OB1,  NONE, OB2,
			 NONE, NONE, NONE,
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
			 NONE, NONE, RED,
			 NONE, NONE, NONE,
			 NONE, NONE, NONE
			 },
			//07
			{
			 OB1,  HOLE, NONE,
			 NONE, HOLE, NONE,
			 NONE, NONE, NONE
			 },
			//08
			{
			 NONE, RED,  NONE,
			 HOLE, HOLE, HOLE,
			 NONE, NONE, NONE
			},
			//09
			{
			 NONE, NONE, NONE,
			 HOLE, HOLE, NONE,
			 NONE, NONE, NONE
			},
		},

		//2
		{
			//00
			{
			 OB1,  OB2,  NONE,
			 NONE, NONE, NONE,
			 NONE, NONE, NONE
			 },
			//01
			{
			 NONE, NONE, NONE,
			 HOLE, HOLE, HOLE,
			 NONE, NONE, NONE
			 },
			//02
			{
			 OB1,  NONE, OB2,
			 NONE, NONE, NONE,
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
			 NONE, NONE, RED,
			 NONE, NONE, NONE,
			 NONE, NONE, NONE
			 },
			//07
			{
			 OB1,  HOLE, NONE,
			 NONE, HOLE, NONE,
			 NONE, NONE, NONE
			 },
			//08
			{
			 NONE, RED,  NONE,
			 HOLE, HOLE, HOLE,
			 NONE, NONE, NONE
			},
			//09
			{
			 NONE, NONE, NONE,
			 HOLE, HOLE, NONE,
			 NONE, NONE, NONE
			},
		},
	};

	int RayCast(const DirectX::XMFLOAT3& startPosition, const DirectX::XMFLOAT3& endPosition, DirectX::XMFLOAT3* outPosition, DirectX::XMFLOAT3* outNormal, float* outLength);

	//�J���[ getter/setter
	void SetTileColor(int c) { ColorNum = c; }
	int GetTileColor() { return ColorNum; }
private:

	int ColorNum = 0;

	DirectX::XMFLOAT4 TileColor[8] = {
		{1.0f, 1.0f, 1.0f, 1.0f},//NONE
		{1.0f, 1.0f, 1.0f, 1.0f},//NONE
		{1.0f, 1.0f, 1.0f, 1.0f},//NONE
		{1.0f, 1.0f, 1.0f, 1.0f},//NONE
		{1.0f, 1.0f, 1.0f, 1.0f},//NONE
		{1.0f, 0.0f, 0.0f, 1.0f},//RED
		{0.0f, 1.0f, 0.0f, 1.0f},//GREEN
		{0.0f, 0.0f, 1.0f, 1.0f},//BLUE
	};
};