#include <imgui.h>
#include "Player.h"
#include "Input/Input.h"
#include "Camera.h"
#include "ObstacleBlockManager.h"
#include "CoinManager.h"
#include "EnemyManager.h"
#include "Collision.h"
#include "KeyInput.h"


static Player* instance = nullptr;
static DirectX::XMFLOAT3 SavePos;

// �C���X�^���X�擾
Player& Player::Instance()
{
	return *instance;
}

// �R���X�g���N�^
Player::Player()
{
	// �C���X�^���X�|�C���g�ݒ�
	instance = this;

	Framework& framework = Framework::Instance();
	ID3D11Device* device = framework.GetDevice();

	//�A�j���[�V����������
	std::vector<std::string> Animationfilenames{
		"Data/Banana/Animations/runAnime.fbx",
		"Data/Banana/Animations/Jump.fbx",
		"Data/Banana/Animations/RunningForwardFlip.fbx",
		"Data/Banana/Animations/KnockedOut.fbx",
		"Data/Banana/Animations/KnockOutRight.fbx",
		"Data/Banana/Animations/KnockOutLeft.fbx",
		"Data/Banana/Animations/KnockOutFront.fbx",
	}; 

	//�v���C���[������
	playerMesh = std::make_shared<SkinnedMesh>(device, "Data/Banana/banana.fbx", Animationfilenames, true);
	playerObj = std::make_unique<SkinnedObject>(playerMesh);
	SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	SetScale(DirectX::XMFLOAT3(0.013f, 0.013f, 0.013f));
	SetAngle(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));

	//�v���C���[�J���[�ݒ�
	Color = { 0.7f, 0.7f, 0.7f, 1.0f };

	//�X�N���[���X�s�[�h������
	playerSpeed = 0.25f;
	oldPlayerSpeed = 0.25f;

	//�R�C��������
	for (int i = 0; i < 3; i++)
	{
		coinState[i] = 0;
		coinAngle[i] = 0.1f;
		coinPositionY[i] = 0.0f;
	}

	//�Q�[���I�[�o�[�t���O������
	gameOverFlg = false;

	//�X�s�[�h�A�b�vUI�\���p
	speedLevelCount = 0;
	renderTimer = 0;

	//�v���C���[SE������
	jumpSE = framework.GetSoundManager()->CreateSoundSource("Data/Sounds/Jump.wav");
	jumpEndSE = framework.GetSoundManager()->CreateSoundSource("Data/Sounds/JumpEnd.wav");
	collisionSE = framework.GetSoundManager()->CreateSoundSource("Data/Sounds/Collision.wav");
	coinSE = framework.GetSoundManager()->CreateSoundSource("Data/Sounds/Coin.wav");
	gimmickSE = framework.GetSoundManager()->CreateSoundSource("Data/Sounds/Gimmick.wav");
	speedUpSE = framework.GetSoundManager()->CreateSoundSource("Data/Sounds/SpeedUp.wav");
}

// �f�X�g���N�^
Player::~Player()
{
}

// �v���C���[�X�V����
void Player::Update(float elapsedTime)
{
	//�����[�X�p
	GamePad& gamePad = Input::Instance().GetGamePad();
	if (gamePad.GetButtonDown() & GamePad::BTN_B || KeyInput::KeyRelease() & KEY_START)debugFlg = true;

	//�f�o�b�O�p
	//if (gamePad.GetButtonDown() & GamePad::BTN_Y || KeyInput::KeyRelease() & KEY_START)debugFlg = !debugFlg;

	//���݂�elapsedTime�ێ�
	saveElapsedTime = elapsedTime;

	//�ʒu���Z�b�g
	playerObj->SetPosition(position);
	playerObj->SetAngle(angle);
	playerObj->SetScale(scale);

	//�����X�N���[��
	if(debugFlg)position.z += playerSpeed;

	//�M�~�b�N�X�V����
	if (!playerInvincibleFlg)GimmickUpdate();
	
	// ���͍X�V����
	UpdateVelocity(elapsedTime);
	
	// �ړ�����
	if(!gameOverFlg)InputMove(elapsedTime);

	// �W�����v����
	if (!gameOverFlg)InputJump();

	// �v���C���[�Ə�Q���Ƃ̏Փ˔���
	if(!playerInvincibleFlg)CollisionPlayerVsObstacle();

	// �v���C���[�ƃR�C���Ƃ̏Փ˔���
	CollisionPlayerVsCoin();

	//�v���C���[�̖��G����
	InvincibleUpdate();

	/*if (KeyInput::KeyRelease() & KEY_X && playerCoinCount == 3 && playerSpeed > 0.25f)
	{
		PlayerSpeedDown(0.1f);
		playerCoinCount = 0;
	}*/

	//�v���C���[�̍X�V
	playerObj->Update(elapsedTime);

	//�����Q�[���I�[�o�[
	if (position.y < -1.5f)
	{
		playerSpeed = 0.0f;
		gameOverFlg = true;
	}

	//���݂̃v���C���[�̃|�W�V������ێ�
	SavePos = position;
}

void Player::Render(ID3D11DeviceContext* immediateContext,
	const DirectX::XMFLOAT4X4& view,
	const DirectX::XMFLOAT4X4& projection,
	const DirectX::XMFLOAT4& light,
	const DirectX::XMFLOAT4& materialColor,
	bool wireframe)
{
	//�v���C���[�̕`��
	playerObj->Render(immediateContext, view, projection, light, Color, wireframe);
}

//�f�o�b�N�pGUI
void Player::DrawDebugGUI()
{
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None))
	{
		//�g�����X�t�H�[��
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
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
			//�n�ʂɂ��Ă��邩�ǂ���
			ImGui::Text("true(1)/false(0):%d", IsGround());
			//�f�o�b�O
			GamePad& gamePad = Input::Instance().GetGamePad();
			if (gamePad.GetButtonDown() & GamePad::BTN_Y)debugFlg = !debugFlg;
			if (ImGui::Button("Debug"))debugFlg = true;
			if (ImGui::Button("NoDebug"))debugFlg = false;
			ImGui::SliderFloat("scroll", &oldPlayerSpeed, 0.0f, 1.0f);
			ImGui::SliderFloat("jump", &jumpSpeed, 0.0f, 100.0f);
			ImGui::SliderFloat("gravity", &gravity, -100.0f, 0.0f);
			ImGui::InputInt("playerCoinCount", &playerCoinCount, 0.0f, 100.0f);
		}
	}
	ImGui::End();
}

//�v���C���[�֌W�M�~�b�N�X�V����
void Player::GimmickUpdate()
{
	//�v���C���[�F�ƃX�s�[�h�̏����l
	SetPlayerColor({ 1.0f,1.0f,1.0f,1.0f });
	playerSpeed = oldPlayerSpeed;

	//////////////////
	//�ԃp�l��
	//////////////////
	if (RedGimmickFlg)
	{
		playerCoinCount = 0;
		SetPlayerColor({ 1.0f,0.0f,0.0f,1.0f });
	}

	//////////////////
	//�p�l��
	//////////////////
	//�X�N���[����������
	if (BlueGimmickFlg)blueGimmickTime = 300.0f;
	//�M�~�b�N�^�C���̎��Ԃ����X�N���[���X�s�[�h���グ��
	if (blueGimmickTime >= 0.0f)
	{
		blueGimmickTime--;
		playerSpeed = oldPlayerSpeed + 0.15f;
		SetPlayerColor({ 0.0f,1.0f,2.0f,1.0f });
	}

	//////////////////
	//���p�l��
	//////////////////
	if (YellowGimmickFlg)yellowGimmickTime = 300.0f;
	//�M�~�b�N�^�C���̎��Ԃ������E����𔽓]
	if (yellowGimmickTime >= 0.0f)
	{
		yellowGimmickTime--;
		reversalFlg = true;
		SetPlayerColor({ 2.0f,1.0f,0.0f,1.0f });
	}
	else
	{
		reversalFlg = false;
	}

	//////////////////
	//���p�l��
	//////////////////
	if (PurpleGimmickFlg)purpleGimmickTime = 300.0f;
	//�M�~�b�N�^�C���̎��Ԃ����X�N���[���X�s�[�h��������
	if (purpleGimmickTime >= 0.0f)
	{
		purpleGimmickTime--;
		playerSpeed = oldPlayerSpeed - 0.15f;
		SetPlayerColor({ 1.0f,0.0f,1.0f,1.0f });
	}

	//�Ɖ��F�𓥂񂾎�
	if (blueGimmickTime >= 0.0f && yellowGimmickTime >= 0.0f)
	{
		SetPlayerColor({ 0.0f,1.0f,0.0f,1.0f });
	}

	
	///////////////
	//�΃p�l��
	///////////////
	//��W�����v����
	if (GreenGimmickFlg)
	{
		if (jumpCount < jumpLimit)
		{
			//�W�����v�A�j���[�V�����Z�b�g
			playerObj->SetMotion(FLIP, 0, 0.02f);
			jumpCount++;
			Jump(28.5f);
		}
	}
}

//�v���C���[���G�X�V����
void Player::InvincibleUpdate()
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	//���G�̏���������m�点��v���C���[�_�ŏ���
	if (playerCoinCount >= 5)
	{
		SetPlayerColor({ R,R,R,1.0f });
		switch (colorState)
		{
		case 0:
			R -= 0.1f;
			if (R < 0.5f)colorState = 1;
			break;
		case 1:
			R += 0.1f;
			if (R > 1.0f)colorState = 0;
			break;
		}

	}

	//5�R�C�������߂�B�{�^���Ŗ��G���[�h�ɓ���
	if (playerCoinCount >= 5 && gamePad.GetButtonDown() & GamePad::BTN_B)
	{
		playerInvincibleTime = 600;
		playerCoinCount -= 5;
	}
	if (playerInvincibleTime >= 0)
	{
		//���Ԃ����炵�Ă�
		playerInvincibleTime--;

		playerInvincibleFlg = true;

		//�v���C���[�̃X�s�[�h�𖳓G�������A�b�v
		playerSpeed = oldPlayerSpeed + 0.40f;

		//���G�����킩��₷���悤�ɐF��ς��Ă���
		SetPlayerColor({ R,G,B,1.0f });
		switch (colorState)
		{
		case 0:
			R -= 0.1f;
			if (R < 0.0f)colorState = 1;
			break;
		case 1:
			R += 0.1f;
			if (R > 1.0f)colorState = 2;
			break;
		case 2:
			G -= 0.1f;
			if (G < 0.0f)colorState = 3;
			break;
		case 3:
			G += 0.1f;
			if (G > 1.0f)colorState = 4;
			break;
		case 4:
			B -= 0.1f;
			if (B < 0.0f)colorState = 5;
			break;
		case 5:
			B += 0.1f;
			if (B > 1.0f)colorState = 0;
			break;
		}
	}
	else playerInvincibleFlg = false;
}

// �X�e�B�b�N���͒l����ړ��x�N�g�����擾
DirectX::XMFLOAT3 Player::GetMoveVec()const
{
	// ���͏����擾
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ax = gamePad.GetAxisLX();
	float ay = gamePad.GetAxisLY();
	//float ay = 0.0f;

	// �J���������ƃX�e�B�b�N�̓��͒l�ɂ���Đi�s�������v�Z����
	Camera& camera = Camera::Instance();
	const DirectX::XMFLOAT3& cameraRight = camera.GetRight();
	const DirectX::XMFLOAT3& cameraFront = camera.GetFront();

	// �ړ��x�N�g����XZ���ʂɐ����ȃx�N�g���ɂȂ�悤�ɂ���

	// �J�����E�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�
	float cameraRightX = cameraRight.x;
	float cameraRightZ = cameraRight.z;
	float cameraRightLength = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);
	if (cameraRightLength > 0.0f)
	{
		// �P�ʃx�N�g����
		cameraRightX /= cameraRightLength;
		cameraRightZ /= cameraRightLength;
	}

	// �J�����O�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�
	float cameraFrontX = cameraFront.x;
	float cameraFrontZ = cameraFront.z;
	float cameraFrontLength = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);
	if (cameraFrontLength > 0.0)
	{
		// �P�ʃx�N�g����
		cameraFrontX /= cameraFrontLength;
		cameraFrontZ /= cameraFrontLength;
	}

	// �X�e�B�b�N�̐������͒l���J�����E�����ɔ��f���A
	// �X�e�B�b�N�̐������͒l���J�����O�����ɔ��f���A
	// �i�s�x�N�g�����v�Z
	DirectX::XMFLOAT3 vec;
	//���F�̏��𓥂񂾎��̍��E���]����
	if (reversalFlg)vec.x = -ax;
	else vec.x = ax;
	//vec.x = (cameraRightX * ax) + (cameraFrontX * ay);
	//vec.z = (cameraRightZ * ax) + (cameraFrontZ * ay);
	if (debugFlg)
	{
		vec.z = 0.0f;
	}
	else
	{
		//vec.z = 0.0f;
		vec.z = ay;
	}
	// Y�������ɂ͈ړ����Ȃ�
	vec.y = 0.0f;

	return vec;
}

// �ړ����͏���
void Player::InputMove(float elapsedTime)
{
	//���E�̈ړ�����
	if (position.x > stageSideEndPos)position.x = SavePos.x;
	if (position.x < -1.0f * stageSideEndPos)position.x = SavePos.x;

	// �i�s�x�N�g�����擾
	DirectX::XMFLOAT3 moveVec = GetMoveVec();

	// �ړ�����
	Move(moveVec.x, moveVec.z, moveSpeed);

	// ���񏈗�
	//Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);

	//�ړ������̌���������
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ax = gamePad.GetAxisLX();

	if (ax > 0.0f)
	{
		if(angle.y < 0.30f)angle.y += 0.02f;
		playerObj->SetAngle(angle);
	}
	else if (ax < 0.0f)
	{
		if (angle.y > -0.30f)angle.y -= 0.02f;
		playerObj->SetAngle(angle);
	}
	else
	{
		angle.y = 0.0f;
		playerObj->SetAngle(angle);
	}
}

//�v���C���[�ƃR�C���̓����蔻��
void Player::CollisionPlayerVsCoin()
{
	CoinManager& coinManager = CoinManager::Instance();
	int coinCount = coinManager.GetCoinCount();
	for (int i = 0; i < coinCount; i++)
	{
		Object* object = coinManager.GetCoin(i);
		if (!object->GetExistFlg())continue;

		//�R�C�����������Ă��鎞�Ƃ��Ȃ����ŏ�Ԃ�ς���
		if (debugFlg && Collision::HitSphere(position, 0.4f, { object->GetPosition().x,  object->GetPosition().y - 1.5f, object->GetPosition().z }, 0.4f))
		{
			//�R�C��SE�炷
			coinSE->Play(false);

			coinState[i] = 1;
		}
		else
		{
			coinState[i] = 0;
		}

		//�R�C���̉�]�ʂ�ݒ�
		if (coinState[i] == 1 && coinAngle[i] > 0.0f)
		{
			coinAngle[i] += 0.05f;
			coinPositionY[i] += 2.5f;
		}
		else
		{
			if (coinAngle[i] > 0.1f)coinAngle[i] -= 0.01f;
			if (coinPositionY[i] > 0.1f)coinPositionY[i] -= 0.1f;
		}

		//�R�C�������J�E���g
		if (coinPositionY[i] > 0 && coinState[i] == 1)
		{
			playerCoinCount = oldPlayerCoinCount + 1;
		}
		else
		{
			oldPlayerCoinCount = playerCoinCount;
		}

		//�ړ��ʃZ�b�g
		object->SetMovePosition(coinPositionY[i]);
		//��]�ʃZ�b�g
		object->SetMoveAngle(coinAngle[i]);
	}
}

// �v���C���[�Ə�Q���̏Փ˔���
void Player::CollisionPlayerVsObstacle()
{
	ObstacleBlockManager& obstacleBlockManager = ObstacleBlockManager::Instance();

	// �S�Ă̏�Q�������ďՓ˂��Ă��邩�`�F�b�N
	int obstacleCount = obstacleBlockManager.GetObstacleCount();
	for (int i = 0; i < obstacleCount; ++i)
	{
		Object* obstacle = obstacleBlockManager.GetObstacle(i);

		//�`�悳��ĂȂ��Ȃ�Փˏ������΂�
		if (!obstacle->GetExistFlg())continue;

		// �Փˏ���
		/////////////////////
		//LeftSide�@�����蔻��
		/////////////////////
		//�v���C���[����Q�����E�ɂ���Ȃ獶�ǂ̓����蔻�菈��
		if (position.x > obstacle->GetPosition().x + obstacle->GetScale().x &&
			position.z > obstacle->GetPosition().z - obstacle->GetScale().z && 
			position.y <= obstacle->GetPosition().y + obstacle->GetScale().y)
		{
			//���ǂɓ������Ă邩�`�F�b�N
			if (Collision::SideLeftWallVsPlayer(
				obstacle->ObstacleUpRightTop,
				obstacle->ObstacleUpRightBottom,
				obstacle->ObstacleDownRightTop,
				obstacle->ObstacleDownRightBottom,
				position)
				)
			{
				position.x = SavePos.x;
				if (position.x > obstacle->GetPosition().x)
				{
					//�Փ�SE�炷
					collisionSE->Play(false);
					
					//�X�N���[���X�g�b�v
					playerSpeed = 0.0f;
					oldPlayerSpeed = 0.0f;
					
					//���|��A�j���[�V�����Z�b�g
					playerObj->SetMotion(KNOCKLEFT, 0, 0.0f);
					
					//�Q�[���I�[�o�[�t���O�𗧂Ă�
					gameOverFlg = true;
				}
			}
		}
		///////////////////////
		//RightSide�@�����蔻��
		///////////////////////
		//�v���C���[����Q�����E�ɂ��Ȃ��Ȃ��Q����荶�ɂ���̂ŉE�Ǔ����蔻��
		else if (position.x < obstacle->GetPosition().x - obstacle->GetScale().x &&
				 position.z > obstacle->GetPosition().z - obstacle->GetScale().z && 
				 position.y <= obstacle->GetPosition().y + obstacle->GetScale().y)
		{
			//�E�ǂɓ������Ă邩�`�F�b�N
			if (Collision::SideRightWallVsPlayer(
				obstacle->ObstacleUpLeftTop,
				obstacle->ObstacleUpLeftBottom,
				obstacle->ObstacleDownLeftTop,
				obstacle->ObstacleDownLeftBottom,
				position)
				)
			{
				position.x = SavePos.x;
				if (position.x < obstacle->GetPosition().x)
				{
					//�Փ�SE�炷
					collisionSE->Play(false);
					
					//�X�N���[���X�g�b�v
					playerSpeed = 0.0f;
					oldPlayerSpeed = 0.0f;
					
					//�E�|��A�j���[�V�����Z�b�g
					playerObj->SetMotion(KNOCKRIGHT, 0, 0.0f);
					
					//�Q�[���I�[�o�[�t���O�𗧂Ă�
					gameOverFlg = true;
				}
			}
		}
		///////////////////
		//front�@�����蔻��
		///////////////////
		//�v���C���[����Q���S�ʂ��O�ɂ��邩�ǂ���
		else if (position.z < obstacle->GetPosition().z- obstacle->GetScale().z && 
				 position.y <= obstacle->GetPosition().y + obstacle->GetScale().y)
		{
			if (Collision::FrontWallVsPlayer(
				obstacle->ObstacleUpLeftBottom,
				obstacle->ObstacleUpRightBottom,
				obstacle->ObstacleDownLeftBottom,
				obstacle->ObstacleDownRightBottom,
				position)
				)
			{
				position.z = SavePos.z;
				position.x = SavePos.x;
				if (position.z < obstacle->GetPosition().z)
				{
					//�Փ�SE�炷
					collisionSE->Play(false);
					
					//�X�N���[���X�g�b�v
					playerSpeed = 0.0f;
					oldPlayerSpeed = 0.0f;
					
					//���|��A�j���[�V�����Z�b�g
					playerObj->SetMotion(KNOCK, 0, 0.0f);
					
					//�Q�[���I�[�o�[�t���O�𗧂Ă�
					gameOverFlg = true;
					
				}
			}
		}
		/////////////////////
		//�u���b�N�� �����蔻��
		/////////////////////
		if (Collision::FloorVsPlayer(
			obstacle->ObstacleUpLeftTop,
			obstacle->ObstacleUpRightTop,
			obstacle->ObstacleUpLeftBottom,
			obstacle->ObstacleUpRightBottom,
			position)
			)
		{	
			//�����钼�O�ɕێ����Ă����ʒu����	
			position = SavePos;
			SetVelocity({ 0,0,0 });
			if (position.y >= obstacle->GetPosition().y)
			{
				//�Փ�SE�炷
				collisionSE->Play(false);
				
				//�X�N���[���X�g�b�v
				playerSpeed = 0.0f;
				oldPlayerSpeed = 0.0f;
				
				//�O�|��A�j���[�V�����Z�b�g
				playerObj->SetMotion(KNOCKFRONT, 0, 0.0f);
				
				//�Q�[���I�[�o�[�t���O�𗧂Ă�
				gameOverFlg = true;
				
			}
		}

	}
}

// �W�����v���͏���
void Player::InputJump()
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	if (gamePad.GetButtonDown() & GamePad::BTN_A /*&& IsGround()*/ && position.y > -1.0f)
	{
		// �W�����v�񐔐���
		if (jumpCount < jumpLimit)
		{
			//�W�����v��SE�炷
			jumpSE->Play(false);
			//�W�����v�A�j���[�V�����Z�b�g
			playerObj->SetMotion(JUMP, 0, 0.03f);
			jumpCount++;
			Jump(jumpSpeed);
		}
	}
}

//���n���Ă��鎞�ɂ��鏈��
void Player::OnLanding()
{
	//����A�j���[�V�����Z�b�g
	if (!gameOverFlg)playerObj->SetMotion(RUN, 0, 0.0f);
	jumpCount = 0;
	jumpEndSE->Play(false);
}

