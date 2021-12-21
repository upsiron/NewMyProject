#pragma once

#include "StaticObject.h"
#include "GeometricPrimitive.h"
#include <DirectXMath.h>

/*******************************************************************************
	弾丸クラスの定義
*******************************************************************************/
class Shot :public StaticObject
{
public:
	//static 変数
	static constexpr float	MAX_DISTANCE = 0.05f;	//最大射程
	static const int		FADE_TIMER = 1;


	//基本変数
	DirectX::XMFLOAT3	Speed;
	bool				Exist;
	int					Timer;
	float				Alpha;

	int					ShotNum;

	//ループ弾用
	int					LoopKounter;	//ループした回数

	//放物弾用
	bool				FallFlg;		//落下開始フラグ
	bool				PosInt;
	float				accel;			//加速度

	//関数
	Shot(std::shared_ptr<StaticMesh>& obj) :StaticObject(obj) {}

	void	Initialize();	//各変数の再初期化
	//void	Render(const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& proj, const DirectX::XMFLOAT4& light_dir);

	void	Set(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 angle, int ShotNum, DirectX::XMFLOAT3 color = DirectX::XMFLOAT3(1, 1, 1));

	void	Loop_Move();
	void	Loop_Move(StaticObject* player);
	void	Parabola_Move();
	void	ParabolaGuide();

};

//お試し中変数
enum SHOT {
	NONE,
	LOOP = 1,
	PARABOLA,
};
#pragma once
