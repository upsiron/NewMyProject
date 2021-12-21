#include	"shot.h"
#include	"scene.h"
#include	"framework.h"

/*******************************************************************************
	�e�ۊǗ��N���X�̏������֐�
*******************************************************************************/
void	Shot::Initialize()
{
	SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	SetScale(DirectX::XMFLOAT3(0.003f, 0.003f, 0.003f));
	SetAngle(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	Exist = false;
	LoopKounter = 0;
	FallFlg = false;
	accel = 0;
	ShotNum = NONE;
	PosInt = true;
	Update();
}




/*******************************************************************************
	�e�ۃN���X�̈ړ��֐�
*******************************************************************************/
void	Shot::Loop_Move()
{
	//	�ő勗����2������O�Ɍv�Z
	const float		DIST_SQUARE = MAX_DISTANCE * MAX_DISTANCE;
	Timer++;
	//	�����ړ�
	position.x += Speed.x;
	position.z += Speed.z;


	Update();
}

void Shot::Loop_Move(StaticObject* player)
{
	//	�ő勗����2������O�Ɍv�Z
	const float		DIST_SQUARE = MAX_DISTANCE * MAX_DISTANCE;
	Timer = FADE_TIMER;
	//	�����ړ�
	position.x += Speed.x * 5.0f;
	position.z += Speed.z * 5.0f;

	DirectX::XMFLOAT3 pos = player->GetPosition();

	DirectX::XMFLOAT3 vec;
	vec.x = position.x - pos.x;
	vec.y = position.y - pos.y;
	vec.z = position.z - pos.z;
	float	d_sq = vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;

	//	�����̔�r
	if (d_sq < DIST_SQUARE)	return;


	//	�^�C�}�[�Ǘ�
	Timer--;
	if (Timer < 0)
	{
		//	���ŏ���
		Exist = false;
	}

	Update();
}

//��������`���ė�������e
void	Shot::Parabola_Move() {

	//	�㏸�X�s�[�h���E�l
	static float AccelLimit = 100.0f;
	//	�㏸���E���x
	static const int HeightLimit = 100;

	//�����p�x�ݒ�
	if (PosInt == true) {
		angle.x = DirectX::XMConvertToRadians(-45);
		PosInt = false;
	}


	//���~�ɐ؂�ւ�
	if (position.y >= HeightLimit) {
		FallFlg = true;
	}

	//���~��
	if (FallFlg == true) {
		accel -= 0.4f;
		angle.x += sinf(DirectX::XMConvertToRadians(2));
		if (accel < -AccelLimit) {	//�������E
			accel = -AccelLimit;
		}
	}
	//�㏸��
	else {
		accel += 0.4f;
		angle.x += sinf(DirectX::XMConvertToRadians(1.2f));
		if (accel > AccelLimit) {	//�������E
			accel = AccelLimit;
		}
	}
	//���e�E����
	if (position.y < -5) {
		Initialize();
		PosInt = true;
		return;
	}

	//�ʒu�X�V
	position.y += accel;

	//	�����ړ�
	position.x += Speed.x * 0.5f;
	position.z += Speed.z * 0.5f;

	Update();

}
void Shot::ParabolaGuide()
{
	//	�㏸�X�s�[�h���E�l
	static float AccelLimit = 100.0f;
	//	�㏸���E���x
	static const int HeightLimit = 100;

	//�Ă��� �����p�x�ݒ�
	if (PosInt == true) {
		angle.x = DirectX::XMConvertToRadians(-45);
		PosInt = false;
	}


	//���~�ɐ؂�ւ�
	if (position.y >= HeightLimit) {
		FallFlg = true;
	}

	//���~��
	if (FallFlg == true) {
		accel -= 0.4f;
		angle.x += sinf(DirectX::XMConvertToRadians(2));
		if (accel < -AccelLimit) {	//�������E
			accel = -AccelLimit;
		}
	}
	//�㏸��
	else {
		accel += 0.4f;
		angle.x += sinf(DirectX::XMConvertToRadians(1.2f));
		if (accel > AccelLimit) {	//�������E
			accel = AccelLimit;
		}
	}
	//���e�E����
	if (position.y < -5) {
		Initialize();
		PosInt = true;
		return;
	}

	//�ʒu�X�V
	position.y += accel;

	//	�����ړ�
	position.x += Speed.x * 0.5f;
	position.z += Speed.z * 0.5f;

	//�p�x�ς��Ă݂�
	//Angle.x+=sinf(XMConvertToRadians(1));


	Update();

}

/*******************************************************************************
	�e�ۊǗ��N���X�̔��ˊ֐�
*******************************************************************************/
//�e�̏o���ʒu,�p�x,�e�̎��,�F
void Shot::Set(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 angle, int shotNum, DirectX::XMFLOAT3 color)
{
	const float OFS_FRONT = 0.0f/*12.0f*/;
	const float OFS_HEIGHT = 1.0f/*6.5f*/;

	//�C������o�����߂̕␳
	DirectX::XMFLOAT3 shot_p = pos;
	shot_p.x += sinf(angle.y) * OFS_FRONT;
	shot_p.z += cosf(angle.y) * OFS_FRONT;
	shot_p.y += OFS_HEIGHT;

	SetPosition(shot_p);
	SetAngle(angle);
	Speed.x = sinf(angle.y);
	Speed.z = cosf(angle.y);
	Timer = Shot::FADE_TIMER;
	Alpha = 1.0f;
	ShotNum = shotNum;
	Exist = true;
}