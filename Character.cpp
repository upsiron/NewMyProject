
#include "character.h"
#include "collision.h"
#include "stageTileManager.h"
#include "stageTile.h"
#include "player.h"

// �ړ�����
void Character::Move(float vx, float vz, float speed)
{
	// �ړ������x�N�g����ݒ�
	moveVecX = vx;
	moveVecZ = vz;

	// �ő呬�x�ݒ�
	maxMoveSpeed = speed;
}

// ���񏈗�
void Character::Turn(float elapsedTime, float vx, float vz, float speed)
{
	speed *= elapsedTime;

	// �i�s�x�N�g�����[���x�N�g���̏ꍇ�͏�������K�v�Ȃ�
	float length = sqrtf(vx * vx + vz * vz);
	if (length <= 0.001f)
	{
		return;
	}
	// �i�s�x�N�g����P�ʃx�N�g����
	vx /= length;
	vz /= length;

	// ���g�̉�]�l����O���������߂�
	float frontX = sinf(angle.y);
	float frontZ = cosf(angle.y);

	// ��]�p�����߂邽�߁A2�̒P�ʃx�N�g���̓��ς��v�Z����
	float dot = frontX * vx + frontZ * vz;

	// ���ϒl��-1.0�`1.0�ŕ\������Ă���A2�̒P�ʃx�N�g���̊p�x��
	// �������ق�1.0�ɋ߂Â��Ƃ��������𗘗p���ĉ�]���x�𒲐�����
	float rot = 1.0f - dot;

	// ���E������s�����߂ɂQ�̒P�ʃx�N�g���̊O�ς��v�Z����
	float cross = (frontX * vz) - (frontZ * vx);

	// 2D�̊O�ϒl�����̏ꍇ�����̏ꍇ���ɂ���č��E���肪�s����
	// ���E������s�����Ƃɂ���č��E��]��I������
	if (cross < 0.0f)
	{
		angle.y += rot;
	}
	else
	{
		angle.y -= rot;
	}
}

// �W�����v����
void Character::Jump(float speed)
{
	// ������̗͂�ݒ�
	velocity.y = speed;
}

//
void Character::UpdateVelocity(float elapsedTime)
{
	// �o�߃t���[��
	float elapsedFrame = elapsedTime * 60.0f;

	// �������͍X�V����
	UpdateVerticalVelocity(elapsedFrame);

	// �����ړ��X�V����
	UpdateVerticalMove(elapsedTime);

	// �������͍X�V����
	UpdateHorizontalVelocity(elapsedFrame);

	// �����ړ������X�V
	UpdateHorizontalMove(elapsedTime);
}

// �������͍X�V����
void Character::UpdateVerticalVelocity(float elapsedFrame)
{
	velocity.y += gravity * elapsedFrame;
}

// �����ړ��X�V����
void Character::UpdateVerticalMove(float elapsedTime)
{
	float my = velocity.y * elapsedTime;
	
	//���~��(�d�͂������Ă�)
	if (my < 0.0f)
	{
		//�n�ʂɐG��Ă��邩�`�F�b�N
		if (FloorCheck() && position.y > -1.0f)
		{
			position.y = 0.0f;
			// ���n����
			if (!isGround)
			{
				OnLanding();
			}
			isGround = true;
			velocity.y = 0.0f;
		}
		else
		{
			// �󒆂ɕ����Ă���
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
	// �㏸��(�W�����v��)
	else if (my > 0.0f)
	//if (my > 0.0f)
	{
		position.y += my;
		isGround = false;
	}
}

// �������͏����X�V
void Character::UpdateHorizontalVelocity(float elapsedFrame)
{
	// XZ���ʂ̑��͂���������
	float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
	if (length > 0.0f)
	{
		// ���C��
		float friction = this->friction * elapsedFrame;

		// �󒆂ɂ���Ƃ��͖��C�͂����炷
		if (!isGround) friction *= airControl;

		// ���C�ɂ�鉡�����̌�������
		if (length > friction)
		{
			// �P�ʃx�N�g����
			float vx = velocity.x / length;
			float vz = velocity.z / length;

			velocity.x -= vx * friction;
			velocity.z -= vz * friction;
		}
		// �������̑��x�����C�͈ȉ��ɂȂ����̂ő��͂𖳌���
		else
		{
			velocity.x = 0.0f;
			velocity.z = 0.0f;
		}
	}
	// XZ���ʂ̑��͂���������
	if (length <= maxMoveSpeed)
	{
		// �ړ��x�N�g�����[���x�N�g���łȂ��Ȃ��������
		float moveVecLength = sqrtf(moveVecX * moveVecX + moveVecZ * moveVecZ);
		if (moveVecLength > 0.0f)
		{
			// ������
			float acceleration = this->acceleration * elapsedFrame;
			// �󒆂ɂ���Ƃ��͉����͂����炷
			if (!isGround)acceleration *= airControl;

			// �ړ��x�N�g���ɂ���������
			//velocity.x += moveVecX * acceleration;
			velocity.x += moveVecX;
			velocity.z += moveVecZ * acceleration;

			// �ő呬�x����
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
	// �ړ��x�N�g�������Z�b�g
	moveVecX = 0.0f;
	moveVecZ = 0.0f;
}

// �����ړ��X�V����
void Character::UpdateHorizontalMove(float elapsedTime)
{
	// �����ړ��l
	float mx = velocity.x * elapsedTime;
	float mz = velocity.z * elapsedTime;

	// �ړ�
	position.x += mx;
	position.z += mz;
}

//�n�ʂɐG��Ă��邩�`�F�b�N���鏈��
bool Character::FloorCheck()
{
	StageTileManager& stageTileManager = StageTileManager::Instance();
	int stageCount = stageTileManager.GetStageCount();

	for (int i = 0; i < stageCount; i++)
	{
		StageTile* stageTile = stageTileManager.GetStageTile(i);
		//���ɓ������Ă���true
		if (Collision::FloorVsPlayer(
			stageTile->tileLeftTop,
			stageTile->tileRightTop,
			stageTile->tileLeftBottom,
			stageTile->tileRightBottom,
			playerObj->GetPosition()))
		{
			//��//
			//�v���C���[������Ă��鏰�̐F���ԂȂ�Ԃ̃t���Oture
			if (stageTile->GetTileColor() == stageTile->RED)
			{
				Player::Instance().gimmickSE->Play(false);
				Player::Instance().RedGimmickFlg = true;
			}
			else Player::Instance().RedGimmickFlg = false;

			//��//
			//�v���C���[������Ă��鏰�̐F���Ȃ�̃t���Oture
			if (stageTile->GetTileColor() == stageTile->BLUE)
			{
				Player::Instance().gimmickSE->Play(false); 
				Player::Instance().BlueGimmickFlg = true;
			}
			else Player::Instance().BlueGimmickFlg = false;

			//��//
			//�v���C���[������Ă��鏰�̐F���΂Ȃ�΂̃t���Oture
			if (stageTile->GetTileColor() == stageTile->GREEN)
			{
				Player::Instance().gimmickSE->Play(false);
				Player::Instance().GreenGimmickFlg = true;
			}
			//�F���Ȃ��Ȃ�e�t���Ofalse
			else Player::Instance().GreenGimmickFlg = false;

			//��//
			//�v���C���[������Ă��鏰�̐F�����Ȃ物�̃t���Oture
			if (stageTile->GetTileColor() == stageTile->YELLOW)
			{
				Player::Instance().gimmickSE->Play(false);
				Player::Instance().YellowGimmickFlg = true;
			}
			//�F���Ȃ��Ȃ�e�t���Ofalse
			else Player::Instance().YellowGimmickFlg = false;

			//��//
			//�v���C���[������Ă��鏰�̐F�����Ȃ率�̃t���Oture
			if (stageTile->GetTileColor() == stageTile->PURPLE)
			{
				Player::Instance().gimmickSE->Play(false);
				Player::Instance().PurpleGimmickFlg = true;
			}
			//�F���Ȃ��Ȃ�e�t���Ofalse
			else Player::Instance().PurpleGimmickFlg = false;

			//���F//
			//�v���C���[������Ă��鏰�̐F�����F�Ȃ琅�F�̃t���Oture
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
