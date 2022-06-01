#pragma once

#include <d3d11.h>
#include "Character.h"


class Player : public Character
{
public:
	// コンストラクタ
	Player(std::shared_ptr<SkinnedMesh>& mesh) :Character(mesh) { SaveElapsedTime = 0.0f;};

	Player();

	// デストラクタ
	~Player() override;

	static Player& Instance();

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

	// スティック入力値から移動ベクトルを取得
	DirectX::XMFLOAT3 GetMoveVec() const;

	// 移動入力処理
	void InputMove(float elapsedTime);

	// ジャンプ入力処理
	void InputJump();

	// プレイヤーとコインとの衝突判定
	void CollisionPlayerVsCoin();

	// プレイヤーと障害物との衝突判定
	void CollisionPlayerVsObstacle();

	int GetJumpCount() { return jumpCount; }

	bool GetGameOverFlg() { return GameOverFlg; }

	bool GetDebugFlg() { return debugflg; }


	
	//ギミック更新処理
	void GimmickUpdate();

protected:
	//着地した時に呼ばれる
	void OnLanding() override;

private:

	float				moveSpeed = 6.0f;
	float				turnSpeed = DirectX::XMConvertToRadians(720);

	float               StageSideEndPos = 1.4f * 3.0f;	 //ステージの端っこの当たり判定用

	int					jumpCount = 0;					//ジャンプした回数カウント用
	int					jumpLimit = 1;					//ジャンプの回数制限用

	float               SaveElapsedTime = 0.0f;			//アニメーションに使うelapsedTime用

	bool                GameOverFlg = false;           //ゲームオーバーフラグ

	bool                debugflg = false;              //デバッグフラグ

	float               GimmickTime = 0;			   //ギミック用タイム

	int                 AnimationState = 0;            //アニメーション状態

	int                 CoinState[3] = { 0 };          //コイン状態
	float				CoinAngle[3] = { 0.1f };       //コイン回転量
	float				CoinPositionY[3] = { 0.0f };   //コイン移動量
    int                 PlayerCoinCount = 0;           //コイン取得数
	int                 OldPlayerCoinCount = 0;        //コイン取得数
	bool                CoinFlg = false;                //コインのフラグ

	//アニメーション用タグ
	enum CurrentAnime
	{
		RUN = 0,
		JUMP,
		FLIP,
		KNOCK,
		KNOCKRIGHT,
		KNOCKLEFT,
		KNOCKFRONT
	};

	//プレイヤーの色変更用変数
	DirectX::XMFLOAT4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
	enum ColorChange
	{
		RedMinus = 0,
		RedPuls
	};
	int ColorState = 0;
	float R = 1.0f;
	float G = 1.0f;
	float B = 1.0f;

};