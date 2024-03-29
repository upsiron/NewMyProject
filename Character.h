#pragma once

#include <DirectXMath.h>
#include "StaticMesh.h"
#include "StaticObject.h"
#include "SKinnedMesh.h"
#include "SkinnedObject.h"
#include "stage.h"
#include "shot.h"

class Character:public SkinnedObject
{
public:
	Character() {};
	Character(std::shared_ptr<SkinnedMesh>& mesh) :
	SkinnedObject(mesh) {};
	virtual ~Character() {};

	//Velocityセット関数
	void SetVelocity(const DirectX::XMFLOAT3& velocity) { this->velocity = velocity; }

	//スクロールスピードセット関数
	void SetPlayerSpeed(float playerSpeed) { this->oldPlayerSpeed = playerSpeed; }

	//スクロールスピードアップ関数
	void PlayerSpeedUp(float playerSpeed) { this->oldPlayerSpeed = this->oldPlayerSpeed + playerSpeed; }

	//スクロールスピードダウン関数
	void PlayerSpeedDown(float playerSpeedDown) { this->oldPlayerSpeed = this->oldPlayerSpeed - playerSpeedDown; }

	// 半径取得
	float GetRadius() const { return radius; }

	// 高さ取得
	float GetHeight()const { return height; };

	// 地面に接地しているか
	bool IsGround()const { return isGround; }

	// 健康状態を取得
	int GetHealth() const { return health; };

	// 健康状態を取得
	void SetHealth(int setHealth) { health = setHealth; };

	// 最大健康状態を取得
	int GetMaxHealth() const { return maxHealth; }

	//自機
	std::shared_ptr<SkinnedMesh> playerMesh;
	std::unique_ptr<SkinnedObject> playerObj;

#define   MAX  100
	//弾
	std::shared_ptr<StaticMesh> shotMesh = nullptr;
	std::unique_ptr<Shot> shotObj[MAX] = { nullptr };

	//敵
	std::shared_ptr<SkinnedMesh> slimeMesh;
	std::unique_ptr<SkinnedObject> slimeObj;

	//障害物
	std::shared_ptr<SkinnedMesh> obstacleMesh;
	std::unique_ptr<SkinnedObject> obstacleObj;

	//コイン
	std::shared_ptr<SkinnedMesh> coinMesh;
	std::unique_ptr<SkinnedObject> coinObj;
protected:
	// 移動処理
	void Move(float vx, float vz, float speed);

	// 旋回処理
	void Turn(float elapsedTime, float vx, float vz, float speed);

	// ジャンプ処理
	void Jump(float speed);

	// ダメージを受けた時に呼ばれる
	virtual void OnDamaged() {}

	// 死亡した時に呼ばれる
	virtual void OnDead() {}

	// 速力処理更新
	void UpdateVelocity(float elapsedTime);

	// 着地した時に呼ばれる
	virtual void OnLanding() {}


	DirectX::XMFLOAT4X4	transform = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};

	float	radius = 0.5f;
	float	height = 2.0f;
	int		health = 5;
	int		maxHealth = 5;
	float	gravity = -1.0f;
	float   oldGravity = 0.0f;
	DirectX::XMFLOAT3 velocity = { 0,0,0 };
	

	bool	isGround = false;
	float	slopeRate = 1.0f;
	float	friction = 0.5f;
	float	acceleration = 1.0f;
	float	maxMoveSpeed = 5.0f;
	float	moveVecX = 0.0f;
	float	moveVecZ = 0.0f;
	float	airControl = 0.3f;	
	float	invincibleTimer = 1.0f;
	float	stepOffset = 1.0f;
	float	jumpSpeed = 17.0f;
	float   playerSpeed = 0.15f;
	float   oldPlayerSpeed = 0.0f;
	int     flgCount = 0;
	int     oldFlgCount = 0;


private:

	// 垂直速力処理更新
	void UpdateVerticalVelocity(float elapsedFrame);

	// 垂直移動更新処理
	void UpdateVerticalMove(float elapsedTime);

	// 水平速力処理更新
	void UpdateHorizontalVelocity(float elapsedFrame);

	// 水平移動更新処理
	void UpdateHorizontalMove(float elapsedTime);

	//床に当たってるかチェック
	bool FloorCheck();


};