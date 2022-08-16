#pragma once

#include <d3d11.h>
#include "Character.h"


class Player : public Character
{
public:
	// コンストラクタ
	Player(std::shared_ptr<SkinnedMesh>& mesh) :Character(mesh) { saveElapsedTime = 0.0f;};

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

	//ジャンプカウント取得
	int GetJumpCount() { return jumpCount; }

	//ゲームオーバーフラグ取得
	bool GetGameOverFlg() { return gameOverFlg; }

	//デバッグフラグ取得
	bool GetDebugFlg() { return debugFlg; }

	//コインカウント取得
	int GetCoinCount() { return playerCoinCount; }
	
	//ギミック更新処理
	void GimmickUpdate();

	void InvincibleUpdate();

	//プレイヤカラーセット関数
	void SetPlayerColor(DirectX::XMFLOAT4 color) { Color = color; }

	//RGBゲット関数
	float GetR() { return R; }
	float GetG() { return G; }
	float GetB() { return B; }

	//ライトブルーフラグゲット関数
	const bool& GetLightBlueFlg() const { return LightBlueGimmickFlg; }

	//無敵フラグゲット関数
	bool GetInvincibleFlg() { return playerInvincibleFlg; }

	//無敵タイマーゲット関数
	float GetInvincibleTime() { return playerInvincibleTime; }

	//ギミック用フラグ
	bool RedGimmickFlg = false;
	bool GreenGimmickFlg = false;
	bool BlueGimmickFlg = false;
	bool YellowGimmickFlg = false;
	bool PurpleGimmickFlg = false;
	bool LightBlueGimmickFlg = false;

	//SE用
	std::unique_ptr<SoundSource> gimmickSE;
	std::unique_ptr<SoundSource> speedUpSE;

	//スピードカウント用
	int speedLevelCount = 0;
	int renderTimer = 0;
protected:
	//着地した時に呼ばれる
	void OnLanding() override;

private:
	//SE用
	std::unique_ptr<SoundSource> jumpSE;
	std::unique_ptr<SoundSource> jumpEndSE;
	std::unique_ptr<SoundSource> collisionSE;
	std::unique_ptr<SoundSource> coinSE;

	float				moveSpeed = 6.0f;
	float				turnSpeed = DirectX::XMConvertToRadians(720);

	float               stageSideEndPos = 1.4f * 3.0f;	//ステージの端っこの当たり判定用

	int					jumpCount = 0;					//ジャンプした回数カウント用
	int					jumpLimit = 1;					//ジャンプの回数制限用

	float               saveElapsedTime = 0.0f;			//アニメーションに使うelapsedTime用

	bool                gameOverFlg = false;            //ゲームオーバーフラグ

	bool                debugFlg = false;               //デバッグフラグ

	float               blueGimmickTime = 0;			//ギミック用タイム
	float               yellowGimmickTime = 0;
	float               purpleGimmickTime = 0;

	bool                reversalFlg = false;            //反転フラグ

	bool                playerInvincibleFlg = false;	//プレイヤー無敵フラグ
	float               playerInvincibleTime = 0;		//プレイヤ無敵時間

	//コイン関係
	int                 coinState[3] = { 0 };          //コイン状態
	float				coinAngle[3] = { 0.1f };       //コイン回転量
	float				coinPositionY[3] = { 0.0f };   //コイン移動量
    int                 playerCoinCount = 0;           //コイン取得数
	int                 oldPlayerCoinCount = 0;        //コイン取得数
	bool                coinFlg = false;                //コインのフラグ

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
		REDMINUS = 0,
		REDPULS
	};
	int colorState = 0;
	float R = 1.0f;
	float G = 1.0f;
	float B = 1.0f;

};