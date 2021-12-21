//#include "Mathf.h"
#include "EnemyBlueSlime.h"
#include "Player.h"

// コンストラクタ
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

	// モデルが大きいのでスケーリング
	//scale.x = scale.y = scale.z = 0.01f;

	radius = 0.5f;
	height = 1.0f;
}

// デストラクタ
EnemyBlueSlime::~EnemyBlueSlime()
{
	
}

void EnemyBlueSlime::Update(float elapsedTime)
{
	//MoveToTarget(elapsedTime, 1.0f);
	// 速力処理更新
	//UpdateVelocity(elapsedTime);

	slimeObj->SetPosition(position);
	slimeObj->SetAngle(angle);

	slimeObj->Update();


	// 無敵時間更新
	//UpdateInvincibleTimer(elapsedTime);

	// オブジェクト行列更新
	//UpdateTransform();

	// 弾丸更新処理
	//projectileManager.Update(elapsedTime);


	// モデル行列更新
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

// 死亡した時に呼ばれる
void EnemyBlueSlime::OnDead()
{
	deathFlag = true;
}

void EnemyBlueSlime::DrawDebugPrimitive()
{
	// 基底クラスのデバッグプリミティブ描画
	/*Enemy::DrawDebugPrimitive();

	DebugRenderer* debugRenderer = GetDebugRenderer();*/

	//// 縄張り範囲をデバッグ円柱描画
	//debugRenderer->DrawCylinder(territoryOrigin, territoryRange, 1.0f, DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));

	//// ターゲット位置をデバッグ球描画
	//debugRenderer->DrawSphere(targetPosition, radius, DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f));

	//// 索敵範囲をデバッグ円柱描画
	//debugRenderer->DrawCylinder(position, searchRange, 1.0f, DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));
}

// 縄張り設定
void EnemyBlueSlime::SetTerritory(const DirectX::XMFLOAT3& origin, float range)
{
	territoryOrigin = origin;
	territoryRange = range;
}
// ターゲット位置をランダム設定
void EnemyBlueSlime::SetRandomTargetPosition()
{
	/*float theta = Mathf::RandomRange(-DirectX::XM_PI, DirectX::XM_PI);
	float range = Mathf::RandomRange(0.0f, territoryRange);
	targetPosition.x = territoryOrigin.x + sinf(theta) * range;
	targetPosition.y = territoryOrigin.y;
	targetPosition.z = territoryOrigin.z + cosf(theta) * range;*/
}

// 目的地点へ移動
void EnemyBlueSlime::MoveToTarget(float elapsedTime, float speedRate)
{
	// ターゲット方向への進行ベクトルを算出
	float vx = targetPosition.x - position.x;
	float vz = targetPosition.z - position.z;
	float dist = sqrtf(vx * vx + vz * vz);
	vx /= dist;
	vz /= dist;

	// 移動処理
	Move(vx, vz, moveSpeed * speedRate);
	Turn(elapsedTime, vx, vz, turnSpeed * speedRate);
}

// 攻撃
void EnemyBlueSlime::ShotProjectile()
{
	// 前方向
	DirectX::XMFLOAT3 dir;
	dir.x = sinf(angle.y);
	dir.y = 0.0f;
	dir.z = cosf(angle.y);

	// 発射位置
	DirectX::XMFLOAT3 pos;
	pos.x = position.x;
	pos.y = position.y + height * 0.5f;
	pos.z = position.z;

	// 発射
	/*ProjectileStraite* projectile = new ProjectileStraite(&projectileManager);
	projectile->Launch(dir, pos);*/

}

//スキル
void EnemyBlueSlime::SkillProjectile()
{
	// 前方向
	DirectX::XMFLOAT3 dir;
	dir.x = sinf(angle.y);
	dir.y = 0.0f;
	dir.z = cosf(angle.y);

	// 発射位置
	DirectX::XMFLOAT3 pos;
	pos.x = position.x;
	pos.y = position.y + height * 0.5f;
	pos.z = position.z;

	// 発射
	/*ProjectileSkill* projectile = new ProjectileSkill(&projectileManager);
	projectile->Launch(dir, pos);*/
}

bool EnemyBlueSlime::SearchPlayer()
{
	// プレイヤーとの高低差を考慮して3Dで距離判定をする
	const DirectX::XMFLOAT3& playerPosition = Player::Instance().GetPosition();
	float vx = playerPosition.x - position.x;
	float vy = playerPosition.y - position.y;
	float vz = playerPosition.z - position.z;
	float dist = sqrtf(vx * vx + vy * vy + vz * vz);

	if (dist < searchRange)
	{
		float distXZ = sqrtf(vx * vx + vz * vz);
		// 単位ベクトル化
		vx /= distXZ;
		vz /= distXZ;

		// 方向ベクトル化
		float frontX = sinf(angle.y);
		float frontZ = cosf(angle.y);
		// 2つのベクトルの内積値で前後判定
		float dot = (frontX * vx) + (frontZ * vz);
		if (dot > 0.0f)
		{
			return true;
		}
	}
	return false;
}

// デバッグエネミー情報表示
void EnemyBlueSlime::DrawDebugGUI()
{
	std::string str = "";
	/*if (activeNode != nullptr)
	{
		str = activeNode->GetName();
	}*/
	//トランスフォーム
	if (ImGui::CollapsingHeader("EnemyBlueSlime", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// 位置
		ImGui::InputFloat3("Position", &position.x);
		// 回転
		DirectX::XMFLOAT3 a;
		a.x = DirectX::XMConvertToDegrees(angle.x);
		a.y = DirectX::XMConvertToDegrees(angle.y);
		a.z = DirectX::XMConvertToDegrees(angle.z);
		ImGui::InputFloat3("Angle", &a.x);
		angle.x = DirectX::XMConvertToRadians(a.x);
		angle.y = DirectX::XMConvertToRadians(a.y);
		angle.z = DirectX::XMConvertToRadians(a.z);
		// スケール
		ImGui::InputFloat3("Scale", &scale.x);

		ImGui::Text(u8"Behavior　%s", str.c_str());
	}
}