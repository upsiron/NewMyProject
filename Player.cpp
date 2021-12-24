#include <imgui.h>
#include "Player.h"
#include "Input/Input.h"
#include "Camera.h"
#include "ObstacleBlockManager.h"
#include "collision.h"
#include "KeyInput.h"
#include "Scene.h"


static Player* instance = nullptr;
static DirectX::XMFLOAT3 savePos;

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
		"Data/Banana/runAnime.fbx",
		"Data/Banana/RunningForwardFlip.fbx",
		"Data/Banana/KnockedOut.fbx",
		"Data/Banana/KnockOutRight.fbx",
		"Data/Banana/KnockOutLeft.fbx",
	}; 
	//������
	playerMesh = std::make_shared<SkinnedMesh>(device, "Data/Banana/banana.fbx", Animationfilenames, true);

	playerObj = std::make_unique<SkinnedObject>(playerMesh);
	SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	SetScale(DirectX::XMFLOAT3(0.013f, 0.013f, 0.013f));
	SetAngle(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));

	//�X�N���[���X�s�[�h������
	scrollSpeed = 0.2f;

	playerObj->SetMotion(RUN, 0, 0.08f);

	// ���f�����傫���̂ŃX�P�[�����O
	//scale.x = scale.y = scale.z = 0.01f;
}

// �f�X�g���N�^
Player::~Player()
{
	//delete model;
}

// �v���C���[�X�V����
void Player::Update(float elapsedTime)
{
	//���݂�elapsedTime�ێ�
	SaveElapsedTime = elapsedTime;

	//�ʒu���Z�b�g
	playerObj->SetPosition(position);
	playerObj->SetAngle(angle);
	playerObj->SetScale(scale);

	//�����X�N���[��
	if(debugflg)position.z += scrollSpeed;

	//�X�N���[����������
	if (RedGimmickFlg)scrollSpeed = oldScrollSpeed + 0.05f;
	else oldScrollSpeed = scrollSpeed;

	if (GreenGimmickFlg)
	{
		if (jumpCount < jumpLimit)
		{
			//�W�����v�A�j���[�V�����Z�b�g
			playerObj->SetMotion(JUMP, 0, 0.08f);
			jumpCount++;
			Jump(30.0f);
		}
	}
	
	// ���͍X�V����
	UpdateVelocity(elapsedTime);
	
	// �ړ�����
	InputMove(elapsedTime);

	// �W�����v����
	InputJump();

	// �v���C���[�ƓG�Ƃ̏Փ˔���
	CollisionPlayerVsEnemis();

	//�v���C���[�̍X�V
	playerObj->Update();

	//�����Q�[���I�[�o�[
	if (position.y < -1.5f)
	{
		scrollSpeed = 0.0f;
		GameOverFlg = true;
	}

	//���݂̃v���C���[�̃|�W�V������ێ�
	savePos = position;
}

void Player::Render(ID3D11DeviceContext* immediateContext,
	const DirectX::XMFLOAT4X4& view,
	const DirectX::XMFLOAT4X4& projection,
	const DirectX::XMFLOAT4& light,
	const DirectX::XMFLOAT4& materialColor,
	bool wireframe)
{
	//�v���C���[�̕`��
	//playerObj->Render(immediateContext, view, projection, light, materialColor, &keyframe, wireframe);
	playerObj->Render(immediateContext, view, projection, light, materialColor, SaveElapsedTime, wireframe);
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
			if (gamePad.GetButtonDown() & GamePad::BTN_Y)debugflg = !debugflg;
			if (ImGui::Button("Debug"))debugflg = true;
			if (ImGui::Button("NoDebug"))debugflg = false;
			ImGui::SliderFloat("scroll", &scrollSpeed, 0.0f, 1.0f);
			ImGui::SliderFloat("jump", &jumpSpeed, 0.0f, 100.0f);
		}
	}
	ImGui::End();
}

// �X�e�B�b�N���͒l����ړ��x�N�g�����擾
DirectX::XMFLOAT3 Player::GetMoveVec()const
{
	// ���͏����擾
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ax = gamePad.GetAxisLX();
	float ay = gamePad.GetAxisLY();

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
	//vec.x = (cameraRightX * ax) + (cameraFrontX * ay);
	vec.x = ax;
	//vec.z = (cameraRightZ * ax) + (cameraFrontZ * ay);
	if (debugflg)
	{
		vec.z = 0.0f;
	}
	else
	{
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
	if (position.x > StageSideEndPos)position.x = savePos.x;
	if (position.x < -1.0f * StageSideEndPos)position.x = savePos.x;

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

// �v���C���[�ƓG�̏Փ˔���
void Player::CollisionPlayerVsEnemis()
{
	ObstacleBlockManager& obstacleBlockManager = ObstacleBlockManager::Instance();

	// �S�Ă̏�Q�������ďՓ˂��Ă��邩�`�F�b�N
	int obstacleCount = obstacleBlockManager.GetObstacleCount();
	for (int i = 0; i < obstacleCount; ++i)
	{
		Obstacle* obstacle = obstacleBlockManager.GetObstacle(i);

		//�`�悳��ĂȂ��Ȃ�Փˏ������΂�
		if (!obstacle->GetExistFlg())continue;

		// �Փˏ���
		//side�@�����蔻��
		//�v���C���[����Q�����E�ɂ���Ȃ獶�ǂ̓����蔻�菈��
		if (position.x > obstacle->GetPosition().x && position.z > obstacle->GetPosition().z - obstacle->GetScale().z)
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
				position.x = savePos.x;
				if (position.x > obstacle->GetPosition().x)
				{
					//�X�N���[���X�g�b�v
					scrollSpeed = 0.0f;
					//���|��A�j���[�V�����Z�b�g
					playerObj->SetMotion(KNOCKLEFT, 0, 0.0f);
					//playerObj->SetAnime(KNOCKLEFT);
				}
			}
		}
		//�v���C���[����Q�����E�ɂ��Ȃ��Ȃ��Q����荶�ɂ���̂ŉE�Ǔ����蔻��
		else if (position.x < obstacle->GetPosition().x && position.z > obstacle->GetPosition().z - obstacle->GetScale().z)
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
				position.x = savePos.x;
				if (position.x < obstacle->GetPosition().x)
				{
					//�X�N���[���X�g�b�v
					scrollSpeed = 0.0f;
					//�E�|��A�j���[�V�����Z�b�g
					playerObj->SetMotion(KNOCKRIGHT, 0, 0.0f);
					//playerObj->SetAnime(KNOCKRIGHT);
				}
			}
		}
		//front�@�����蔻��
		if (position.z < obstacle->GetPosition().z- obstacle->GetScale().z)
		{
			if (Collision::VerticalFrontWallVsPlayer(
				obstacle->ObstacleUpLeftBottom,
				obstacle->ObstacleUpRightBottom,
				obstacle->ObstacleDownLeftBottom,
				obstacle->ObstacleDownRightBottom,
				position)
				)
			{
				position.z = savePos.z;
				if (position.z < obstacle->GetPosition().z)
				{
					//���|��A�j���[�V�����Z�b�g
					playerObj->SetMotion(KNOCK, 0, 0.0f);
					GameOverFlg = true;
					//playerObj->SetAnime(KNOCK);
				}
			}
		}
	}
}

void Player::InputShot()
{
	if (KeyInput::KeyTrigger() & KEY_C)
	{
		shot_timer = 0;
		//�ʏ�e
		for (auto& s : shotObj)
		{
			if (s->Exist)continue;	//���݂��Ă���Εʂ�����
			s->Set(DirectX::XMFLOAT3(position.x, position.y, position.z), angle, SHOT::LOOP);
			break;
		}
	}
	//�V���b�g�^�C�}�[�Ŏ˒�
	if (shot_timer > 20)
	{
		for (auto& s : shotObj) {
			s->Exist = false;
		}
	}

	for (auto& s : shotObj) {
		if (!s->Exist)continue;		//���݂��Ȃ���Εʂ�����
		switch (s->ShotNum)
		{
		case SHOT::NONE:
			break;
		case SHOT::LOOP:
			s->Loop_Move();
			break;
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
			//scrollSpeed = scrollSpeed - (saveScrollSpeed - 0.15f);
			//�W�����v�A�j���[�V�����Z�b�g
			playerObj->SetMotion(JUMP, 0, 0.08f);
			//playerObj->SetAnime(JUMP);
			jumpCount++;
			Jump(jumpSpeed);
		}
	}
}

// �f�o�b�O�v���~�e�B�u�`��
//void Player::DrawDebugPrimitive()
//{
//	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();
//
//	// �Փ˔���p�f�o�b�O����`��
//	debugRenderer->DrawSphere(position, radius, DirectX::XMFLOAT4(0, 0, 0, 1));
//
//	// �e�ۃf�o�b�O�v���~�e�B�u�`�揈��
//	//projectileManager.DrawDebugPrimitive();
//}

//���n���Ă��鎞�ɂ��鏈��
void Player::OnLanding()
{
	//scrollSpeed = saveScrollSpeed;
	//����A�j���[�V�����Z�b�g
	playerObj->SetMotion(RUN, 0, 0.0f);
	//playerObj->SetAnime(RUN);
	jumpCount = 0;
}

