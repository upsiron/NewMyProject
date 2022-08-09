#pragma once
#include "stage.h"

// �X�e�[�W
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
		MOB1,
		MOB2,
		MOB3,
		RED,
		GREEN,
		BLUE,
		COIN1,
		COIN2,
		COIN3,
	};

	//�ǂݍ��񂾃X�e�[�W�}�b�v�̐��l������
	int StageTileMapTxt[6][10][9] = { 0 };

	//���ۂɃv���O������Ŏg���X�e�[�W�}�b�v�̕ϐ�
	int StageTileMap[6][10][9] = { 0 };


	//�X�e�[�W�}�b�v
	//int StageTileMap[6][10][9] =
	//{
	//	//0
	//	{
	//		//00
	//		{
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE
	//		 },
	//		//01
	//		{
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE
	//		 },
	//		//02
	//		{
	//		 OB1,  OB2, NONE,
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE
	//		},
	//		//03
	//		{
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE
	//		},
	//		//04
	//		{
	//		 NONE, NONE, NONE,
	//		 NONE, HOLE, RED,
	//		 NONE, NONE, NONE
	//		},
	//		//05
	//		{
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE
	//		},
	//		//06
	//		{
	//		 OB1,  NONE, OB2,
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE
	//		 },
	//		//07
	//		{
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE,
	//		 NONE, HOLE, NONE
	//		 },
	//		//08
	//		{
	//		 RED,  RED,  RED,
	//		 NONE, NONE, COIN1,
	//		 NONE, NONE, NONE
	//		},
	//		//09
	//		{
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE
	//		},
	//	},

	//	//1
	//	{
	//		//00
	//		{
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE
	//		 },
	//		//01
	//		{
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE
	//		 },
	//		//02
	//		{
	//		 OB1, OB2, OB3,
	//		 NONE, NONE, NONE,
	//		 GREEN, GREEN, GREEN
	//		},
	//		//03
	//		{
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE
	//		},
	//		//04
	//		{
	//		 GREEN, NONE, NONE,
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE
	//		},
	//		//05
	//		{
	//		 NONE, NONE, NONE,
	//		 NONE, OB3, HOLE,
	//		 OB1,  OB2, NONE
	//		},
	//		//06
	//		{
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE,
	//		 COIN2, NONE, NONE
	//		 },
	//		//07
	//		{
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE
	//		 },
	//		//08
	//		{
	//		 HOLE, OB1, OB2,
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE
	//		},
	//		//09
	//		{
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE
	//		},
	//	},

	//	//2
	//	{
	//		//00
	//		{
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE
	//		 },
	//		//01
	//		{
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, BLUE
	//		 },
	//		//02
	//		{
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE
	//		},
	//		//03
	//		{
	//		 NONE, NONE, NONE,
	//		 MOB3, OB1, MOB1,
	//		 NONE, NONE, NONE
	//		},
	//		//04
	//		{
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE
	//		},
	//		//05
	//		{
	//		 NONE, NONE, NONE,
	//		 HOLE, BLUE, HOLE,
	//		 NONE, NONE, NONE
	//		},
	//		//06
	//		{
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE
	//		 },
	//		//07
	//		{
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE
	//		 },
	//		//08
	//		{
	//		 NONE, OB1, COIN3,
	//		 NONE, OB2, NONE,
	//		 MOB1, OB3, MOB3
	//		},
	//		//09
	//		{
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE
	//		},
	//	},

	//	//3
	//	{
	//		//00
	//		{
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE
	//		 },
	//		//01
	//		{
	//		 OB1,  OB2,  NONE,
	//		 NONE, NONE, COIN1,
	//		 NONE, NONE, NONE
	//		 },
	//		//02
	//		{
	//		 HOLE, HOLE, NONE,
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE
	//		},
	//		//03
	//		{
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE,
	//		 NONE, OB1,  NONE
	//		},
	//		//04
	//		{
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE,
	//		 OB1,  NONE, OB2
	//		},
	//		//05
	//		{
	//		 COIN2, NONE, NONE,
	//		 RED,  HOLE, RED,
	//		 NONE, NONE, NONE
	//		},
	//		//06
	//		{
	//		 NONE, OB1,  NONE,
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE
	//		 },
	//		//07
	//		{
	//		 NONE, NONE, NONE,
	//		 HOLE, NONE, NONE,
	//		 NONE, NONE, COIN3
	//		 },
	//		//08
	//		{
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE,
	//		 NONE, OB1,  OB2
	//		},
	//		//09
	//		{
	//		 OB1,  OB2,  OB3,
	//		 NONE, NONE, NONE,
	//		 GREEN, GREEN, GREEN
	//		},
	//	},

	//	//4
	//	{
	//		//00
	//		{
	//		 NONE, NONE, NONE,
	//		 NONE, COIN1, NONE,
	//		 NONE, NONE, NONE
	//		 },
	//		//01
	//		{
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE,
	//		 OB1,  NONE, OB2
	//		 },
	//		//02
	//		{
	//		 NONE, HOLE, HOLE,
	//		 NONE, HOLE, HOLE,
	//		 NONE, HOLE, HOLE
	//		},
	//		//03
	//		{
	//		 NONE, NONE, NONE,
	//		 NONE, RED,  NONE,
	//		 NONE, NONE, NONE
	//		},
	//		//04
	//		{
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE,
	//		 OB1,  OB2,  NONE
	//		},
	//		//05
	//		{
	//		 NONE, OB1,  OB2,
	//		 NONE, NONE, NONE,
	//		 NONE, GREEN, GREEN
	//		},
	//		//06
	//		{
	//		 OB1,  RED,  NONE,
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE
	//		 },
	//		//07
	//		{
	//		 NONE, OB1,  NONE,
	//		 NONE, NONE, HOLE,
	//		 NONE, NONE, NONE
	//		 },
	//		//08
	//		{
	//		 NONE, NONE,  NONE,
	//		 HOLE, HOLE, HOLE,
	//		 NONE, NONE, NONE
	//		},
	//		//09
	//		{
	//		 NONE, HOLE, NONE,
	//		 NONE, HOLE, NONE,
	//		 NONE, NONE, NONE
	//		},
	//	},

	//	//5
	//	{
	//		//00
	//		{
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE
	//		 },
	//		//01
	//		{
	//		 NONE, NONE, NONE,
	//		 NONE, BLUE, NONE,
	//		 NONE, NONE, NONE
	//		 },
	//		//02
	//		{
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE
	//		},
	//		//03
	//		{
	//		 MOB3,  OB1, MOB1,
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE
	//		},
	//		//04
	//		{
	//		 NONE, NONE, NONE,
	//		 NONE, BLUE, NONE,
	//		 NONE, NONE, NONE
	//		},
	//		//05
	//		{
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE
	//		},
	//		//06
	//		{
	//		 MOB1, OB2,  MOB3,
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE
	//		 },
	//		//07
	//		{
	//		 NONE, OB1, HOLE,
	//		 RED,  OB2, NONE,
	//		 HOLE, OB3, NONE
	//		 },
	//		//08
	//		{
	//		 NONE, OB1, NONE,
	//		 HOLE, OB2, NONE,
	//		 NONE, OB3, NONE
	//		},
	//		//09
	//		{
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE,
	//		 NONE, NONE, NONE
	//		},
	//	},
	//};

	//�J���[ getter/setter
	void SetTileColor(int c) { ColorNum = c; }
	int GetTileColor() { return ColorNum; }

private:

	//�J���[�ԍ�
	int ColorNum = 0;

	DirectX::XMFLOAT4 TileColor[14] = {
		{1.0f, 1.0f, 1.0f, 1.0f},//NONE
		{1.0f, 1.0f, 1.0f, 1.0f},//NONE
		{1.0f, 1.0f, 1.0f, 1.0f},//NONE
		{1.0f, 1.0f, 1.0f, 1.0f},//NONE
		{1.0f, 1.0f, 1.0f, 1.0f},//NONE
		{1.0f, 1.0f, 1.0f, 1.0f},//NONE
		{1.0f, 1.0f, 1.0f, 1.0f},//NONE
		{1.0f, 1.0f, 1.0f, 1.0f},//NONE
		{2.5f, 0.0f, 0.0f, 1.0f},//RED
		{0.0f, 2.5f, 0.0f, 1.0f},//GREEN
		{0.0f, 0.0f, 5.0f, 1.0f},//BLUE
		{1.0f, 1.0f, 1.0f, 1.0f},//COIN
		{1.0f, 1.0f, 1.0f, 1.0f},//COIN
		{1.0f, 1.0f, 1.0f, 1.0f},//COIN
	};

	//�X�e�[�W�̌��Ԗ��ߍ��킹�X�P�[��
	float StageScaleGap = 0.7f;
};