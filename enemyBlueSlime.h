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

	// 更新処理
	void Update(float elapsedTime)override;

	// 描画処理
	void Render(ID3D11DeviceContext* immediateContext,
		const DirectX::XMFLOAT4X4& view,
		const DirectX::XMFLOAT4X4& projection,
		const DirectX::XMFLOAT4& light,
		const DirectX::XMFLOAT4& materialColor,
		bool wireframe)override;

	// デバッグプリミティブ描画
	void DrawDebugPrimitive() override;

	// 縄張り設定
	void SetTerritory(const DirectX::XMFLOAT3& origin, float range);

	// ターゲット位置をランダム設定
	void SetRandomTargetPosition();

	// 目標地点へ移動
	void MoveToTarget(float elapsedTime, float speedRate);

	// プレイヤー索敵
	bool SearchPlayer();

	// ターゲットポジション設定
	void SetTargetPosition(DirectX::XMFLOAT3 position) { targetPosition = position; }

	// ターゲットポジション取得
	DirectX::XMFLOAT3 GetTargetPosition() { return targetPosition; }

	// ポジション取得
	DirectX::XMFLOAT3 GetPosition() { return position; }

	// 攻撃範囲取得
	float GetAttackRange() { return attackRange; }

	// 弾丸入力処理
	void ShotProjectile();

	// スキル入力処理
	void SkillProjectile();

	// デバッグエネミー情報表示
	void DrawDebugGUI();

	// 実行タイマー設定
	void SetRunTimer(float timer) { runTimer = timer; }

	// 実行タイマー取得
	float GetRunTimer() { return runTimer; }

	// ダメージフラグ設定
	void SetDamageFlag(bool flag) { damageFlag = flag; }

	// ダメージフラグ取得
	bool GetDamageFlag() { return damageFlag; }

	// 死亡フラグ設定
	void SetDeathFlag(bool flag) { deathFlag = flag; }

	// 死亡フラグ取得
	bool GetDeathFlag() { return deathFlag; }

protected:
	// ダメージを受けた時に呼ばれる
	void OnDamaged() override;
	// 死亡したときに呼ばれる
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