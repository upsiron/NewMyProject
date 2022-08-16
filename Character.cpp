
#include "character.h"
#include "collision.h"
#include "stageTileManager.h"
#include "stageTile.h"
#include "player.h"

// 移動処理
void Character::Move(float vx, float vz, float speed)
{
	// 移動方向ベクトルを設定
	moveVecX = vx;
	moveVecZ = vz;

	// 最大速度設定
	maxMoveSpeed = speed;
}

// 旋回処理
void Character::Turn(float elapsedTime, float vx, float vz, float speed)
{
	speed *= elapsedTime;

	// 進行ベクトルがゼロベクトルの場合は処理する必要なし
	float length = sqrtf(vx * vx + vz * vz);
	if (length <= 0.001f)
	{
		return;
	}
	// 進行ベクトルを単位ベクトル化
	vx /= length;
	vz /= length;

	// 自身の回転値から前方向を求める
	float frontX = sinf(angle.y);
	float frontZ = cosf(angle.y);

	// 回転角を求めるため、2つの単位ベクトルの内積を計算する
	float dot = frontX * vx + frontZ * vz;

	// 内積値は-1.0～1.0で表現されており、2つの単位ベクトルの角度が
	// 小さいほど1.0に近づくという性質を利用して回転速度を調整する
	float rot = 1.0f - dot;

	// 左右判定を行うために２つの単位ベクトルの外積を計算する
	float cross = (frontX * vz) - (frontZ * vx);

	// 2Dの外積値が生の場合か負の場合かによって左右判定が行える
	// 左右判定を行うことによって左右回転を選択する
	if (cross < 0.0f)
	{
		angle.y += rot;
	}
	else
	{
		angle.y -= rot;
	}
}

// ジャンプ処理
void Character::Jump(float speed)
{
	// 上方向の力を設定
	velocity.y = speed;
}

//
void Character::UpdateVelocity(float elapsedTime)
{
	// 経過フレーム
	float elapsedFrame = elapsedTime * 60.0f;

	// 垂直速力更新処理
	UpdateVerticalVelocity(elapsedFrame);

	// 垂直移動更新処理
	UpdateVerticalMove(elapsedTime);

	// 水平速力更新処理
	UpdateHorizontalVelocity(elapsedFrame);

	// 水平移動処理更新
	UpdateHorizontalMove(elapsedTime);
}

// 垂直速力更新処理
void Character::UpdateVerticalVelocity(float elapsedFrame)
{
	velocity.y += gravity * elapsedFrame;
}

// 垂直移動更新処理
void Character::UpdateVerticalMove(float elapsedTime)
{
	float my = velocity.y * elapsedTime;
	
	//下降中(重力かかってる)
	if (my < 0.0f)
	{
		//地面に触れているかチェック
		if (FloorCheck() && position.y > -1.0f)
		{
			position.y = 0.0f;
			// 着地した
			if (!isGround)
			{
				OnLanding();
			}
			isGround = true;
			velocity.y = 0.0f;
		}
		else
		{
			// 空中に浮いている
			if (position.y < -0.1f)
			{
				playerSpeed = 0.0f;
				isGround = false;
				position.y += my;
			}
			else
			{
				isGround = false;
				position.y += my;
			}
		}

	}
	// 上昇中(ジャンプ中)
	else if (my > 0.0f)
	//if (my > 0.0f)
	{
		position.y += my;
		isGround = false;
	}
}

// 水平速力処理更新
void Character::UpdateHorizontalVelocity(float elapsedFrame)
{
	// XZ平面の速力を減速する
	float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
	if (length > 0.0f)
	{
		// 摩擦力
		float friction = this->friction * elapsedFrame;

		// 空中にいるときは摩擦力を減らす
		if (!isGround) friction *= airControl;

		// 摩擦による横方向の減速処理
		if (length > friction)
		{
			// 単位ベクトル化
			float vx = velocity.x / length;
			float vz = velocity.z / length;

			velocity.x -= vx * friction;
			velocity.z -= vz * friction;
		}
		// 横方向の速度が摩擦力以下になったので速力を無効化
		else
		{
			velocity.x = 0.0f;
			velocity.z = 0.0f;
		}
	}
	// XZ平面の速力を加速する
	if (length <= maxMoveSpeed)
	{
		// 移動ベクトルがゼロベクトルでないなら加速する
		float moveVecLength = sqrtf(moveVecX * moveVecX + moveVecZ * moveVecZ);
		if (moveVecLength > 0.0f)
		{
			// 加速力
			float acceleration = this->acceleration * elapsedFrame;
			// 空中にいるときは加速力を減らす
			if (!isGround)acceleration *= airControl;

			// 移動ベクトルによる加速処理
			//velocity.x += moveVecX * acceleration;
			velocity.x += moveVecX;
			velocity.z += moveVecZ * acceleration;

			// 最大速度制限
			float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
			if (length > maxMoveSpeed)
			{
				float vx = velocity.x / length;
				float vz = velocity.z / length;

				velocity.x = vx * maxMoveSpeed;
				velocity.z = vz * maxMoveSpeed;
			}
			if (isGround && slopeRate > 0.0f)
			{
				velocity.y -= length * slopeRate * elapsedFrame;
			}
		}
	}
	// 移動ベクトルをリセット
	moveVecX = 0.0f;
	moveVecZ = 0.0f;
}

// 水平移動更新処理
void Character::UpdateHorizontalMove(float elapsedTime)
{
	// 水平移動値
	float mx = velocity.x * elapsedTime;
	float mz = velocity.z * elapsedTime;

	// 移動
	position.x += mx;
	position.z += mz;
}

//地面に触れているかチェックする処理
bool Character::FloorCheck()
{
	StageTileManager& stageTileManager = StageTileManager::Instance();
	int stageCount = stageTileManager.GetStageCount();

	for (int i = 0; i < stageCount; i++)
	{
		StageTile* stageTile = stageTileManager.GetStageTile(i);
		//床に当たってたらtrue
		if (Collision::FloorVsPlayer(
			stageTile->tileLeftTop,
			stageTile->tileRightTop,
			stageTile->tileLeftBottom,
			stageTile->tileRightBottom,
			playerObj->GetPosition()))
		{
			//赤//
			//プレイヤーが乗っている床の色が赤なら赤のフラグture
			if (stageTile->GetTileColor() == stageTile->RED)
			{
				Player::Instance().gimmickSE->Play(false);
				Player::Instance().RedGimmickFlg = true;
			}
			else Player::Instance().RedGimmickFlg = false;

			//青//
			//プレイヤーが乗っている床の色が青なら青のフラグture
			if (stageTile->GetTileColor() == stageTile->BLUE)
			{
				Player::Instance().gimmickSE->Play(false); 
				Player::Instance().BlueGimmickFlg = true;
			}
			else Player::Instance().BlueGimmickFlg = false;

			//緑//
			//プレイヤーが乗っている床の色が緑なら緑のフラグture
			if (stageTile->GetTileColor() == stageTile->GREEN)
			{
				Player::Instance().gimmickSE->Play(false);
				Player::Instance().GreenGimmickFlg = true;
			}
			//色がないなら各フラグfalse
			else Player::Instance().GreenGimmickFlg = false;

			//黄//
			//プレイヤーが乗っている床の色が黄なら黄のフラグture
			if (stageTile->GetTileColor() == stageTile->YELLOW)
			{
				Player::Instance().gimmickSE->Play(false);
				Player::Instance().YellowGimmickFlg = true;
			}
			//色がないなら各フラグfalse
			else Player::Instance().YellowGimmickFlg = false;

			//紫//
			//プレイヤーが乗っている床の色が紫なら紫のフラグture
			if (stageTile->GetTileColor() == stageTile->PURPLE)
			{
				Player::Instance().gimmickSE->Play(false);
				Player::Instance().PurpleGimmickFlg = true;
			}
			//色がないなら各フラグfalse
			else Player::Instance().PurpleGimmickFlg = false;

			//水色//
			//プレイヤーが乗っている床の色が水色なら水色のフラグture
			if (stageTile->GetTileColor() == stageTile->LIGHTBLUE)
			{
				Player::Instance().gimmickSE->Play(false);
				flgCount = oldFlgCount + 1;
				switch (flgCount)
				{
				case 0:
					flgCount++;
					break;
				case 1:
					Player::Instance().LightBlueGimmickFlg = true;
					break;
				case 2:
					Player::Instance().LightBlueGimmickFlg = false;
					flgCount = 0;
					break;
				}
			}
			else
			{
				oldFlgCount = flgCount;
			}

			return true;
		}
	}
	return false;
}
