#pragma once

#include <d3d11.h>
#include "Character.h"


class Player : public Character
{
public:
	// �R���X�g���N�^
	Player(std::shared_ptr<SkinnedMesh>& mesh) :Character(mesh) { saveElapsedTime = 0.0f;};

	Player();

	// �f�X�g���N�^
	~Player() override;

	static Player& Instance();

	// �X�V����
	void Update(float elapsedTime);

	// �`�揈��
	void Render(ID3D11DeviceContext* immediateContext,
					const DirectX::XMFLOAT4X4& view,
					const DirectX::XMFLOAT4X4& projection,
					const DirectX::XMFLOAT4& light,
					const DirectX::XMFLOAT4& materialColor,
					bool wireframe);

	// �f�o�b�O�pGUI�\��
	void DrawDebugGUI();

	// �X�e�B�b�N���͒l����ړ��x�N�g�����擾
	DirectX::XMFLOAT3 GetMoveVec() const;

	// �ړ����͏���
	void InputMove(float elapsedTime);

	// �W�����v���͏���
	void InputJump();

	// �v���C���[�ƃR�C���Ƃ̏Փ˔���
	void CollisionPlayerVsCoin();

	// �v���C���[�Ə�Q���Ƃ̏Փ˔���
	void CollisionPlayerVsObstacle();

	//�W�����v�J�E���g�擾
	int GetJumpCount() { return jumpCount; }

	//�Q�[���I�[�o�[�t���O�擾
	bool GetGameOverFlg() { return gameOverFlg; }

	//�f�o�b�O�t���O�擾
	bool GetDebugFlg() { return debugFlg; }

	//�R�C���J�E���g�擾
	int GetCoinCount() { return playerCoinCount; }
	
	//�M�~�b�N�X�V����
	void GimmickUpdate();

	void InvincibleUpdate();

	//�v���C���J���[�Z�b�g�֐�
	void SetPlayerColor(DirectX::XMFLOAT4 color) { Color = color; }

	//RGB�Q�b�g�֐�
	float GetR() { return R; }
	float GetG() { return G; }
	float GetB() { return B; }

	//���C�g�u���[�t���O�Q�b�g�֐�
	const bool& GetLightBlueFlg() const { return LightBlueGimmickFlg; }

	//���G�t���O�Q�b�g�֐�
	bool GetInvincibleFlg() { return playerInvincibleFlg; }

	//���G�^�C�}�[�Q�b�g�֐�
	float GetInvincibleTime() { return playerInvincibleTime; }

	//�M�~�b�N�p�t���O
	bool RedGimmickFlg = false;
	bool GreenGimmickFlg = false;
	bool BlueGimmickFlg = false;
	bool YellowGimmickFlg = false;
	bool PurpleGimmickFlg = false;
	bool LightBlueGimmickFlg = false;

	//SE�p
	std::unique_ptr<SoundSource> gimmickSE;
	std::unique_ptr<SoundSource> speedUpSE;

	//�X�s�[�h�J�E���g�p
	int speedLevelCount = 0;
	int renderTimer = 0;
protected:
	//���n�������ɌĂ΂��
	void OnLanding() override;

private:
	//SE�p
	std::unique_ptr<SoundSource> jumpSE;
	std::unique_ptr<SoundSource> jumpEndSE;
	std::unique_ptr<SoundSource> collisionSE;
	std::unique_ptr<SoundSource> coinSE;

	float				moveSpeed = 6.0f;
	float				turnSpeed = DirectX::XMConvertToRadians(720);

	float               stageSideEndPos = 1.4f * 3.0f;	//�X�e�[�W�̒[�����̓����蔻��p

	int					jumpCount = 0;					//�W�����v�����񐔃J�E���g�p
	int					jumpLimit = 1;					//�W�����v�̉񐔐����p

	float               saveElapsedTime = 0.0f;			//�A�j���[�V�����Ɏg��elapsedTime�p

	bool                gameOverFlg = false;            //�Q�[���I�[�o�[�t���O

	bool                debugFlg = false;               //�f�o�b�O�t���O

	float               blueGimmickTime = 0;			//�M�~�b�N�p�^�C��
	float               yellowGimmickTime = 0;
	float               purpleGimmickTime = 0;

	bool                reversalFlg = false;            //���]�t���O

	bool                playerInvincibleFlg = false;	//�v���C���[���G�t���O
	float               playerInvincibleTime = 0;		//�v���C�����G����

	//�R�C���֌W
	int                 coinState[3] = { 0 };          //�R�C�����
	float				coinAngle[3] = { 0.1f };       //�R�C����]��
	float				coinPositionY[3] = { 0.0f };   //�R�C���ړ���
    int                 playerCoinCount = 0;           //�R�C���擾��
	int                 oldPlayerCoinCount = 0;        //�R�C���擾��
	bool                coinFlg = false;                //�R�C���̃t���O

	//�A�j���[�V�����p�^�O
	enum CurrentAnime
	{
		RUN = 0,
		JUMP,
		FLIP,
		KNOCK,
		KNOCKRIGHT,
		KNOCKLEFT,
		KNOCKFRONT
	};

	//�v���C���[�̐F�ύX�p�ϐ�
	DirectX::XMFLOAT4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
	enum ColorChange
	{
		REDMINUS = 0,
		REDPULS
	};
	int colorState = 0;
	float R = 1.0f;
	float G = 1.0f;
	float B = 1.0f;

};