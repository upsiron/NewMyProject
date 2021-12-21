//#include "Mathf.h"
#include "EnemyBlueSlime.h"
#include "Player.h"

// �R���X�g���N�^
EnemyBlueSlime::EnemyBlueSlime()
{
	Framework& framework = Framework::Instance();
	ID3D11Device* device = framework.GetDevice();

	//slimeMesh = std::make_shared<SkinnedMesh>(device, "Data/Slime/slime.fbx",true);
	slimeMesh = std::make_shared<SkinnedMesh>(device, "Data/001_cube.fbx", true);
	slimeObj = std::make_unique<SkinnedObject>(slimeMesh);
	slimeObj->SetPosition(DirectX::XMFLOAT3(2.0f, 0.0f, 0.0f));
	slimeObj->SetScale(DirectX::XMFLOAT3(1.5f, 1.5f, 1.5f));
	slimeObj->SetAngle(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));

	// ���f�����傫���̂ŃX�P�[�����O
	//scale.x = scale.y = scale.z = 0.01f;

	radius = 0.5f;
	height = 1.0f;
}

// �f�X�g���N�^
EnemyBlueSlime::~EnemyBlueSlime()
{
	
}

void EnemyBlueSlime::Update(float elapsedTime)
{
	//MoveToTarget(elapsedTime, 1.0f);
	// ���͏����X�V
	//UpdateVelocity(elapsedTime);

	slimeObj->SetPosition(position);
	slimeObj->SetAngle(angle);

	slimeObj->Update();


	// ���G���ԍX�V
	//UpdateInvincibleTimer(elapsedTime);

	// �I�u�W�F�N�g�s��X�V
	//UpdateTransform();

	// �e�ۍX�V����
	//projectileManager.Update(elapsedTime);


	// ���f���s��X�V
	//model->UpdateTransform(transform);
}

void EnemyBlueSlime::Render(ID3D11DeviceContext* immediateContext,
	const DirectX::XMFLOAT4X4& view,
	const DirectX::XMFLOAT4X4& projection,
	const DirectX::XMFLOAT4& light,
	const DirectX::XMFLOAT4& materialColor,
	bool wireframe)
{
	//slimeObj->Render(immediateContext, view, projection, light, materialColor, false);
	slimeObj->Render(immediateContext, view, projection, light, materialColor, 0.0f, false);

}

void EnemyBlueSlime::OnDamaged()
{
	damageFlag = true;

}

// ���S�������ɌĂ΂��
void EnemyBlueSlime::OnDead()
{
	deathFlag = true;
}

void EnemyBlueSlime::DrawDebugPrimitive()
{
	// ���N���X�̃f�o�b�O�v���~�e�B�u�`��
	/*Enemy::DrawDebugPrimitive();

	DebugRenderer* debugRenderer = GetDebugRenderer();*/

	//// �꒣��͈͂��f�o�b�O�~���`��
	//debugRenderer->DrawCylinder(territoryOrigin, territoryRange, 1.0f, DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));

	//// �^�[�Q�b�g�ʒu���f�o�b�O���`��
	//debugRenderer->DrawSphere(targetPosition, radius, DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f));

	//// ���G�͈͂��f�o�b�O�~���`��
	//debugRenderer->DrawCylinder(position, searchRange, 1.0f, DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));
}

// �꒣��ݒ�
void EnemyBlueSlime::SetTerritory(const DirectX::XMFLOAT3& origin, float range)
{
	territoryOrigin = origin;
	territoryRange = range;
}
// �^�[�Q�b�g�ʒu�������_���ݒ�
void EnemyBlueSlime::SetRandomTargetPosition()
{
	/*float theta = Mathf::RandomRange(-DirectX::XM_PI, DirectX::XM_PI);
	float range = Mathf::RandomRange(0.0f, territoryRange);
	targetPosition.x = territoryOrigin.x + sinf(theta) * range;
	targetPosition.y = territoryOrigin.y;
	targetPosition.z = territoryOrigin.z + cosf(theta) * range;*/
}

// �ړI�n�_�ֈړ�
void EnemyBlueSlime::MoveToTarget(float elapsedTime, float speedRate)
{
	// �^�[�Q�b�g�����ւ̐i�s�x�N�g�����Z�o
	float vx = targetPosition.x - position.x;
	float vz = targetPosition.z - position.z;
	float dist = sqrtf(vx * vx + vz * vz);
	vx /= dist;
	vz /= dist;

	// �ړ�����
	Move(vx, vz, moveSpeed * speedRate);
	Turn(elapsedTime, vx, vz, turnSpeed * speedRate);
}

// �U��
void EnemyBlueSlime::ShotProjectile()
{
	// �O����
	DirectX::XMFLOAT3 dir;
	dir.x = sinf(angle.y);
	dir.y = 0.0f;
	dir.z = cosf(angle.y);

	// ���ˈʒu
	DirectX::XMFLOAT3 pos;
	pos.x = position.x;
	pos.y = position.y + height * 0.5f;
	pos.z = position.z;

	// ����
	/*ProjectileStraite* projectile = new ProjectileStraite(&projectileManager);
	projectile->Launch(dir, pos);*/

}

//�X�L��
void EnemyBlueSlime::SkillProjectile()
{
	// �O����
	DirectX::XMFLOAT3 dir;
	dir.x = sinf(angle.y);
	dir.y = 0.0f;
	dir.z = cosf(angle.y);

	// ���ˈʒu
	DirectX::XMFLOAT3 pos;
	pos.x = position.x;
	pos.y = position.y + height * 0.5f;
	pos.z = position.z;

	// ����
	/*ProjectileSkill* projectile = new ProjectileSkill(&projectileManager);
	projectile->Launch(dir, pos);*/
}

bool EnemyBlueSlime::SearchPlayer()
{
	// �v���C���[�Ƃ̍��፷���l������3D�ŋ������������
	const DirectX::XMFLOAT3& playerPosition = Player::Instance().GetPosition();
	float vx = playerPosition.x - position.x;
	float vy = playerPosition.y - position.y;
	float vz = playerPosition.z - position.z;
	float dist = sqrtf(vx * vx + vy * vy + vz * vz);

	if (dist < searchRange)
	{
		float distXZ = sqrtf(vx * vx + vz * vz);
		// �P�ʃx�N�g����
		vx /= distXZ;
		vz /= distXZ;

		// �����x�N�g����
		float frontX = sinf(angle.y);
		float frontZ = cosf(angle.y);
		// 2�̃x�N�g���̓��ϒl�őO�㔻��
		float dot = (frontX * vx) + (frontZ * vz);
		if (dot > 0.0f)
		{
			return true;
		}
	}
	return false;
}

// �f�o�b�O�G�l�~�[���\��
void EnemyBlueSlime::DrawDebugGUI()
{
	std::string str = "";
	/*if (activeNode != nullptr)
	{
		str = activeNode->GetName();
	}*/
	//�g�����X�t�H�[��
	if (ImGui::CollapsingHeader("EnemyBlueSlime", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// �ʒu
		ImGui::InputFloat3("Position", &position.x);
		// ��]
		DirectX::XMFLOAT3 a;
		a.x = DirectX::XMConvertToDegrees(angle.x);
		a.y = DirectX::XMConvertToDegrees(angle.y);
		a.z = DirectX::XMConvertToDegrees(angle.z);
		ImGui::InputFloat3("Angle", &a.x);
		angle.x = DirectX::XMConvertToRadians(a.x);
		angle.y = DirectX::XMConvertToRadians(a.y);
		angle.z = DirectX::XMConvertToRadians(a.z);
		// �X�P�[��
		ImGui::InputFloat3("Scale", &scale.x);

		ImGui::Text(u8"Behavior�@%s", str.c_str());
	}
}