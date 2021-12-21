#pragma once
#include "Enemy.h"

//class BehaviorTree;
//class BehaviorData;
//class NodeBase;

class EnemyBlueSlime :public Enemy
{
public:
	EnemyBlueSlime();
	EnemyBlueSlime(std::shared_ptr<SkinnedMesh>& mesh) :
		Enemy(mesh) {};
	~EnemyBlueSlime()override;

	// �X�V����
	void Update(float elapsedTime)override;

	// �`�揈��
	void Render(ID3D11DeviceContext* immediateContext,
		const DirectX::XMFLOAT4X4& view,
		const DirectX::XMFLOAT4X4& projection,
		const DirectX::XMFLOAT4& light,
		const DirectX::XMFLOAT4& materialColor,
		bool wireframe)override;

	// �f�o�b�O�v���~�e�B�u�`��
	void DrawDebugPrimitive() override;

	// �꒣��ݒ�
	void SetTerritory(const DirectX::XMFLOAT3& origin, float range);

	// �^�[�Q�b�g�ʒu�������_���ݒ�
	void SetRandomTargetPosition();

	// �ڕW�n�_�ֈړ�
	void MoveToTarget(float elapsedTime, float speedRate);

	// �v���C���[���G
	bool SearchPlayer();

	// �^�[�Q�b�g�|�W�V�����ݒ�
	void SetTargetPosition(DirectX::XMFLOAT3 position) { targetPosition = position; }

	// �^�[�Q�b�g�|�W�V�����擾
	DirectX::XMFLOAT3 GetTargetPosition() { return targetPosition; }

	// �|�W�V�����擾
	DirectX::XMFLOAT3 GetPosition() { return position; }

	// �U���͈͎擾
	float GetAttackRange() { return attackRange; }

	// �e�ۓ��͏���
	void ShotProjectile();

	// �X�L�����͏���
	void SkillProjectile();

	// �f�o�b�O�G�l�~�[���\��
	void DrawDebugGUI();

	// ���s�^�C�}�[�ݒ�
	void SetRunTimer(float timer) { runTimer = timer; }

	// ���s�^�C�}�[�擾
	float GetRunTimer() { return runTimer; }

	// �_���[�W�t���O�ݒ�
	void SetDamageFlag(bool flag) { damageFlag = flag; }

	// �_���[�W�t���O�擾
	bool GetDamageFlag() { return damageFlag; }

	// ���S�t���O�ݒ�
	void SetDeathFlag(bool flag) { deathFlag = flag; }

	// ���S�t���O�擾
	bool GetDeathFlag() { return deathFlag; }

protected:
	// �_���[�W���󂯂����ɌĂ΂��
	void OnDamaged() override;
	// ���S�����Ƃ��ɌĂ΂��
	void OnDead() override;

private:


	DirectX::XMFLOAT3	targetPosition = { 0.0f,0.0f,0.0f };
	DirectX::XMFLOAT3	territoryOrigin = { 0.0f,0.0f,0.0f };
	float				territoryRange = 10.0f;
	float				moveSpeed = 3.0f;
	float				turnSpeed = DirectX::XMConvertToRadians(360);
	float				searchRange = 5.0f;
	float				attackRange = 1.5f;
	float				runTimer = 0.0f;

	bool				damageFlag = false;
	bool				deathFlag = false;
};