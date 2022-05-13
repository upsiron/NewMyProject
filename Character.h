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

	//Velocity�Z�b�g�֐�
	void SetVelocity(const DirectX::XMFLOAT3& velocity) { this->velocity = velocity; }

	//�X�N���[���X�s�[�h�Z�b�g�֐�
	void SetScrollSpeed(float scrollSpeed) { this->oldScrollSpeed = scrollSpeed; }

	// ���a�擾
	float GetRadius() const { return radius; }

	// �����擾
	float GetHeight()const { return height; };

	// �n�ʂɐڒn���Ă��邩
	bool IsGround()const { return isGround; }

	// ���N��Ԃ��擾
	int GetHealth() const { return health; };

	// ���N��Ԃ��擾
	void SetHealth(int setHealth) { health = setHealth; };

	// �ő匒�N��Ԃ��擾
	int GetMaxHealth() const { return maxHealth; }

	//���@
	std::shared_ptr<SkinnedMesh> playerMesh;
	std::unique_ptr<SkinnedObject> playerObj;

#define   MAX  100
	//�e
	std::shared_ptr<StaticMesh> shotMesh = nullptr;
	std::unique_ptr<Shot> shotObj[MAX] = { nullptr };

	//�G
	std::shared_ptr<SkinnedMesh> slimeMesh;
	std::unique_ptr<SkinnedObject> slimeObj;

	//��Q��
	std::shared_ptr<SkinnedMesh> obstacleMesh;
	std::unique_ptr<SkinnedObject> obstacleObj;

	//�R�C��
	std::shared_ptr<SkinnedMesh> coinMesh;
	std::unique_ptr<SkinnedObject> coinObj;
protected:
	// �ړ�����
	void Move(float vx, float vz, float speed);

	// ���񏈗�
	void Turn(float elapsedTime, float vx, float vz, float speed);

	// �W�����v����
	void Jump(float speed);

	// �_���[�W���󂯂����ɌĂ΂��
	virtual void OnDamaged() {}

	// ���S�������ɌĂ΂��
	virtual void OnDead() {}

	// ���͏����X�V
	void UpdateVelocity(float elapsedTime);

	// ���n�������ɌĂ΂��
	virtual void OnLanding() {}

protected:
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
	float   scrollSpeed = 0.15f;
	float   oldScrollSpeed = 0.0f;
	int     flgCount = 0;
	int     oldFlgCount = 0;

private:

	// �������͏����X�V
	void UpdateVerticalVelocity(float elapsedFrame);

	// �����ړ��X�V����
	void UpdateVerticalMove(float elapsedTime);

	// �������͏����X�V
	void UpdateHorizontalVelocity(float elapsedFrame);

	// �����ړ��X�V����
	void UpdateHorizontalMove(float elapsedTime);

	//���ɓ������Ă邩�`�F�b�N
	bool FloorCheck();
};