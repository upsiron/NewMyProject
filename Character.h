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

	//����Ȃ����ǂƂ肠�����R�����g�A�E�g
		//// �ʒu�擾
		//const DirectX::XMFLOAT3& GetPosition() const { return position; }
		//// �ʒu�ݒ�
		//void SetPosition(const DirectX::XMFLOAT3& position) { this->position = position; }
		//// ��]�擾
		//const DirectX::XMFLOAT3& GetAngle()const { return angle; }
		//// ��]�ݒ�
		//void SetAngle(const DirectX::XMFLOAT3& angle) { this->angle = angle; }
		//// �X�P�[���擾
		//const DirectX::XMFLOAT3& GetScale()const { return scale; }
		//// �X�P�[���ݒ�
		//void SetScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }
	
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

	// �_���[�W��^����
	//bool ApplyDamage(int damage, float invincibleTime);

	// �Ռ���^����
	//void AddImpulse(const DirectX::XMFLOAT3& impluse);

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

	// ���G���ԍX�V
	//void UpdateInvincibleTimer(float elapsedTime);

	// ���n�������ɌĂ΂��
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
	
	//���̂Ƃ���g�p���Ȃ�
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

	// �������͏����X�V
	void UpdateVerticalVelocity(float elapsedFrame);

	// �����ړ��X�V����
	void UpdateVerticalMove(float elapsedTime);

	// �������͏����X�V
	void UpdateHorizontalVelocity(float elapsedFrame);

	// �����ړ��X�V����
	void UpdateHorizontalMove(float elapsedTime);

	bool FloorCheck();

	//bool onSquare;
};