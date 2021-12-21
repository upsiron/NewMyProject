#pragma once

#include "StaticObject.h"
#include "GeometricPrimitive.h"
#include <DirectXMath.h>

/*******************************************************************************
	�e�ۃN���X�̒�`
*******************************************************************************/
class Shot :public StaticObject
{
public:
	//static �ϐ�
	static constexpr float	MAX_DISTANCE = 0.05f;	//�ő�˒�
	static const int		FADE_TIMER = 1;


	//��{�ϐ�
	DirectX::XMFLOAT3	Speed;
	bool				Exist;
	int					Timer;
	float				Alpha;

	int					ShotNum;

	//���[�v�e�p
	int					LoopKounter;	//���[�v������

	//�����e�p
	bool				FallFlg;		//�����J�n�t���O
	bool				PosInt;
	float				accel;			//�����x

	//�֐�
	Shot(std::shared_ptr<StaticMesh>& obj) :StaticObject(obj) {}

	void	Initialize();	//�e�ϐ��̍ď�����
	//void	Render(const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& proj, const DirectX::XMFLOAT4& light_dir);

	void	Set(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 angle, int ShotNum, DirectX::XMFLOAT3 color = DirectX::XMFLOAT3(1, 1, 1));

	void	Loop_Move();
	void	Loop_Move(StaticObject* player);
	void	Parabola_Move();
	void	ParabolaGuide();

};

//���������ϐ�
enum SHOT {
	NONE,
	LOOP = 1,
	PARABOLA,
};
#pragma once
