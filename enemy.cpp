#include "Enemy.h"
//#include "EnemyManager.h"

// デバッグプリミティブ描画
void Enemy::DrawDebugPrimitive()
{
	//DebugRenderer* debugRenderer = GetDebugRenderer();

	// 衝突判定用デバッグ球を描画
	//debugRenderer->DrawSphere(position, radius, DirectX::XMFLOAT4(0, 0, 0, 1));
}

// デバッグプリミティブ描画
void Enemy::DrawDebugGUI()
{
}

// 破棄
void Enemy::Destroy()
{
	//EnemyManager::Instance().Remove(this);
}