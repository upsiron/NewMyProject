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

	// �f�o�b�O�v���~�e�B�u�`��
	void DrawDebugPrimitive();

	// �v���C���[�ƃG�l�~�[�Ƃ̏Փ˔���
	void CollisionPlayerVsEnemis();

	void InputShot();

	int GetJumpCount() { return jumpCount; }

	bool GetGameOverFlg() { return GameOverFlg; }

protected:
	//���n�������ɌĂ΂��
	void OnLanding() override;

private:

	float				moveSpeed = 6.0f;
	float				turnSpeed = DirectX::XMConvertToRadians(720);

	float               StageSideEndPos = 1.6f * 3.0f; //�X�e�[�W�̒[�����̓����蔻��p

	int					jumpCount = 0;           //�W�����v�����񐔃J�E���g�p
	int					jumpLimit = 1;           //�W�����v�̉񐔐����p

	int					shot_timer = 0;

	float               SaveElapsedTime = 0.0f;  //�A�j���[�V�����Ɏg��elapsedTime�p

	bool                GameOverFlg = false;

	bool                debugflg;

	int                 AnimationState = 0;
	enum CurrentAnime
	{
		RUN = 0,
		JUMP,
		KNOCK,
		KNOCKRIGHT,
		KNOCKLEFT
	};

};