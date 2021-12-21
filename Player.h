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

	// デバッグプリミティブ描画
	void DrawDebugPrimitive();

	// プレイヤーとエネミーとの衝突判定
	void CollisionPlayerVsEnemis();

	void InputShot();

	int GetJumpCount() { return jumpCount; }

	bool GetGameOverFlg() { return GameOverFlg; }

protected:
	//着地した時に呼ばれる
	void OnLanding() override;

private:

	float				moveSpeed = 6.0f;
	float				turnSpeed = DirectX::XMConvertToRadians(720);

	float               StageSideEndPos = 1.6f * 3.0f; //ステージの端っこの当たり判定用

	int					jumpCount = 0;           //ジャンプした回数カウント用
	int					jumpLimit = 1;           //ジャンプの回数制限用

	int					shot_timer = 0;

	float               SaveElapsedTime = 0.0f;  //アニメーションに使うelapsedTime用

	bool                GameOverFlg = false;

	bool                debugflg;

	int                 AnimationState = 0;
	enum CurrentAnime
	{
		RUN = 0,
		JUMP,
		KNOCK,
		KNOCKRIGHT,
		KNOCKLEFT
	};

};