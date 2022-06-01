#pragma once

#include <d3d11.h>
#include "Character.h"


class Player : public Character
{
public:
	// �R���X�g���N�^
	Player(std::shared_ptr<SkinnedMesh>& mesh) :Character(mesh) { SaveElapsedTime = 0.0f;};

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

	int GetJumpCount() { return jumpCount; }

	bool GetGameOverFlg() { return GameOverFlg; }

	bool GetDebugFlg() { return debugflg; }


	
	//�M�~�b�N�X�V����
	void GimmickUpdate();

protected:
	//���n�������ɌĂ΂��
	void OnLanding() override;

private:

	float				moveSpeed = 6.0f;
	float				turnSpeed = DirectX::XMConvertToRadians(720);

	float               StageSideEndPos = 1.4f * 3.0f;	 //�X�e�[�W�̒[�����̓����蔻��p

	int					jumpCount = 0;					//�W�����v�����񐔃J�E���g�p
	int					jumpLimit = 1;					//�W�����v�̉񐔐����p

	float               SaveElapsedTime = 0.0f;			//�A�j���[�V�����Ɏg��elapsedTime�p

	bool                GameOverFlg = false;           //�Q�[���I�[�o�[�t���O

	bool                debugflg = false;              //�f�o�b�O�t���O

	float               GimmickTime = 0;			   //�M�~�b�N�p�^�C��

	int                 AnimationState = 0;            //�A�j���[�V�������

	int                 CoinState[3] = { 0 };          //�R�C�����
	float				CoinAngle[3] = { 0.1f };       //�R�C����]��
	float				CoinPositionY[3] = { 0.0f };   //�R�C���ړ���
    int                 PlayerCoinCount = 0;           //�R�C���擾��
	int                 OldPlayerCoinCount = 0;        //�R�C���擾��
	bool                CoinFlg = false;                //�R�C���̃t���O

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
		RedMinus = 0,
		RedPuls
	};
	int ColorState = 0;
	float R = 1.0f;
	float G = 1.0f;
	float B = 1.0f;

};