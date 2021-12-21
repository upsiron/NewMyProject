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

	//いらないけどとりあえずコメントアウト
		//// 位置取得
		//const DirectX::XMFLOAT3& GetPosition() const { return position; }
		//// 位置設定
		//void SetPosition(const DirectX::XMFLOAT3& position) { this->position = position; }
		//// 回転取得
		//const DirectX::XMFLOAT3& GetAngle()const { return angle; }
		//// 回転設定
		//void SetAngle(const DirectX::XMFLOAT3& angle) { this->angle = angle; }
		//// スケール取得
		//const DirectX::XMFLOAT3& GetScale()const { return scale; }
		//// スケール設定
		//void SetScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }
	
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

	// ダメージを与える
	//bool ApplyDamage(int damage, float invincibleTime);

	// 衝撃を与える
	//void AddImpulse(const DirectX::XMFLOAT3& impluse);

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

	// 無敵時間更新
	//void UpdateInvincibleTimer(float elapsedTime);

	// 着地した時に呼ばれる
	virtual void OnLanding() {}

protected:

	/*DirectX::XMFLOAT3	position = { 0,0,0 };
	DirectX::XMFLOAT3	angle = { 0,0,0, };
	DirectX::XMFLOAT3	scale = { 1,1,1 };*/

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
	DirectX::XMFLOAT3 velocity = { 0,0,0 };
	
	//今のところ使用しない
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
	float	jumpSpeed = 20.0f;
	float   scrollSpeed = 0.15f;
	float   oldScrollSpeed = 0.0f;
	float   saveScrollSpeed = 0.15f;
private:

	// 垂直速力処理更新
	void UpdateVerticalVelocity(float elapsedFrame);

	// 垂直移動更新処理
	void UpdateVerticalMove(float elapsedTime);

	// 水平速力処理更新
	void UpdateHorizontalVelocity(float elapsedFrame);

	// 水平移動更新処理
	void UpdateHorizontalMove(float elapsedTime);

	bool FloorCheck();

	//bool onSquare;
};