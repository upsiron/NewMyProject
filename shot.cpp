#include	"shot.h"
#include	"scene.h"
#include	"framework.h"

/*******************************************************************************
	弾丸管理クラスの初期化関数
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
	弾丸クラスの移動関数
*******************************************************************************/
void	Shot::Loop_Move()
{
	//	最大距離の2乗を事前に計算
	const float		DIST_SQUARE = MAX_DISTANCE * MAX_DISTANCE;
	Timer++;
	//	直線移動
	position.x += Speed.x;
	position.z += Speed.z;


	Update();
}

void Shot::Loop_Move(StaticObject* player)
{
	//	最大距離の2乗を事前に計算
	const float		DIST_SQUARE = MAX_DISTANCE * MAX_DISTANCE;
	Timer = FADE_TIMER;
	//	直線移動
	position.x += Speed.x * 5.0f;
	position.z += Speed.z * 5.0f;

	DirectX::XMFLOAT3 pos = player->GetPosition();

	DirectX::XMFLOAT3 vec;
	vec.x = position.x - pos.x;
	vec.y = position.y - pos.y;
	vec.z = position.z - pos.z;
	float	d_sq = vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;

	//	距離の比較
	if (d_sq < DIST_SQUARE)	return;


	//	タイマー管理
	Timer--;
	if (Timer < 0)
	{
		//	消滅処理
		Exist = false;
	}

	Update();
}

//放物線を描いて落下する弾
void	Shot::Parabola_Move() {

	//	上昇スピード限界値
	static float AccelLimit = 100.0f;
	//	上昇限界高度
	static const int HeightLimit = 100;

	//初期角度設定
	if (PosInt == true) {
		angle.x = DirectX::XMConvertToRadians(-45);
		PosInt = false;
	}


	//下降に切り替え
	if (position.y >= HeightLimit) {
		FallFlg = true;
	}

	//下降中
	if (FallFlg == true) {
		accel -= 0.4f;
		angle.x += sinf(DirectX::XMConvertToRadians(2));
		if (accel < -AccelLimit) {	//加速限界
			accel = -AccelLimit;
		}
	}
	//上昇中
	else {
		accel += 0.4f;
		angle.x += sinf(DirectX::XMConvertToRadians(1.2f));
		if (accel > AccelLimit) {	//加速限界
			accel = AccelLimit;
		}
	}
	//着弾・消滅
	if (position.y < -5) {
		Initialize();
		PosInt = true;
		return;
	}

	//位置更新
	position.y += accel;

	//	直線移動
	position.x += Speed.x * 0.5f;
	position.z += Speed.z * 0.5f;

	Update();

}
void Shot::ParabolaGuide()
{
	//	上昇スピード限界値
	static float AccelLimit = 100.0f;
	//	上昇限界高度
	static const int HeightLimit = 100;

	//てすと 初期角度設定
	if (PosInt == true) {
		angle.x = DirectX::XMConvertToRadians(-45);
		PosInt = false;
	}


	//下降に切り替え
	if (position.y >= HeightLimit) {
		FallFlg = true;
	}

	//下降中
	if (FallFlg == true) {
		accel -= 0.4f;
		angle.x += sinf(DirectX::XMConvertToRadians(2));
		if (accel < -AccelLimit) {	//加速限界
			accel = -AccelLimit;
		}
	}
	//上昇中
	else {
		accel += 0.4f;
		angle.x += sinf(DirectX::XMConvertToRadians(1.2f));
		if (accel > AccelLimit) {	//加速限界
			accel = AccelLimit;
		}
	}
	//着弾・消滅
	if (position.y < -5) {
		Initialize();
		PosInt = true;
		return;
	}

	//位置更新
	position.y += accel;

	//	直線移動
	position.x += Speed.x * 0.5f;
	position.z += Speed.z * 0.5f;

	//角度変えてみる
	//Angle.x+=sinf(XMConvertToRadians(1));


	Update();

}

/*******************************************************************************
	弾丸管理クラスの発射関数
*******************************************************************************/
//弾の出現位置,角度,弾の種類,色
void Shot::Set(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 angle, int shotNum, DirectX::XMFLOAT3 color)
{
	const float OFS_FRONT = 0.0f/*12.0f*/;
	const float OFS_HEIGHT = 1.0f/*6.5f*/;

	//砲口から出すための補正
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