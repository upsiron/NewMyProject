#include "scene.h"
#include "KeyInput.h"
#include "framework.h"
#include "collision.h"
#include "Input/Input.h"
#include "Camera.h"
#include "EnemyManager.h"
#include "ObstacleBlockManager.h"
#include "CoinManager.h"
#include "StageManager.h"
#include "StageTileManager.h"

#include <iostream>
#include <string>    
#include <fstream>   
//**********************************************
//
//		���C���V�[��
//
//**********************************************

static bool ShowDemoWindow = false;
static bool ShowAnotherWindow = false;

static DirectX::XMFLOAT4 ambientColor = { 0.564f,0.564f,0.564f,1 };
static float diffuseColor[3] = { 0.3f, 0.3f, 0.3f };
DirectX::XMFLOAT4 LightDir(1, 1, 1, 1);
DirectX::XMFLOAT4 DirLightColor(1, 1, 1, 1);


//--------------------------------------------------------
//		������
//--------------------------------------------------------

void SceneMain::Initialize()
{
	//�R���X�^���g�o�b�t�@
	CreateConstantBuffer(&ConstantBuffer, sizeof(ConstantBufferForPerFrame));
	CreateConstantBuffer(&ConstantBufferBloom, sizeof(ConstantBufferForBloom));

	//�t�F�[�h������
	FadeBlack = std::make_unique<Transition>();
	FadeBlack->init(device, L"Data/Image/unnamed.png", { 0.0f,0.0f }, { 1920.0f,1080.0f }, { 0.0f,0.0f }, { 1920.0f,1080.0f }, { 1.0f,1.0f,1.0f,1.0f });

	//�摜������
	speedUp = std::make_unique<Sprite>(Framework::Instance().GetDevice(), L"Data//Image/SpeedUp.png");
	speedMax = std::make_unique<Sprite>(Framework::Instance().GetDevice(), L"Data//Image/SpeedMax.png");

	//�X�N���[���c���T�C�Y�ϐ�
	float screenWidth = static_cast<float>(framework.GetScreenWidth());
	float screenHeight = static_cast<float>(framework.GetScreenHeight());

	//------------
	// �V�F�[�_�[
	//------------
	//�V�F�[�_�[������
	SkinnedShader = std::make_unique<SkinnedMeshShader>(device, "Shaders/skinned_mesh_vs.cso", "Shaders/skinned_mesh_ps.cso");
	NoLightShader = std::make_unique<StaticMeshShader>(device, "Shaders/NoLight_vs.cso", "Shaders/NoLight_ps.cso");
	ParticleShader = std::make_unique<PointSpriteShader>(device, "Shaders/PointSprite_vs.cso", "Shaders/PointSprite_gs.cso", "Shaders/PointSprite_ps.cso");
	BrightShader = std::make_unique<BrightSpriteShader>(device, "Shaders/bright_vs.cso", "Shaders/bright_ps.cso");
	BokehShader = std::make_unique<BokehSpriteShader>(device, "Shaders/bokeh_vs.cso", "Shaders/bokeh_ps.cso");

	//------------
	// �J����
	//------------
	//�J����������
	Camera& camera = Camera::Instance();

	//�J�����̐ݒ�
	camera.SetLookAt(
		DirectX::XMFLOAT3(0.0f, 100.0f, -20.0f),	//�J�����̈ʒu
		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),		//�J�����̏œ_
		DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f)			//�J�����̏���������߂�
	);

	//�J�����̎���p
	camera.SetPerspecticeFov(
		DirectX::XMConvertToRadians(45),
		screenWidth / screenHeight,
		0.1f,
		1000.0f
	);
	
	//�J�����R���g���[���[������
	cameraController = new CameraController();


	//---------------
	// �p�[�e�B�N��
	//---------------
	//�p�[�e�B�N��������
	particles = std::make_unique<ParticleSystem>(device, 200000);


	//------------
	// �u���[��
	//------------
	//�u���[��������
	fullScreen = std::make_unique<FullScreenQuad>(device);
	fullScreenBright = std::make_unique<FullScreenQuad>(device);
	fullScreenBokeh = std::make_unique<FullScreenQuad>(device);
	offScreen = std::make_unique<FrameBuffer>(device, 1280 / SCREEN, 720 / SCREEN);
	bright = std::make_unique<FrameBuffer>(device, 1280 / SCREEN, 720 / SCREEN);
	bloom1 = std::make_unique<FrameBuffer>(device, 1280 / BLOOM1, 720 / BLOOM1);
	bloom2 = std::make_unique<FrameBuffer>(device, 1280 / BLOOM2, 720 / BLOOM2);
	bloom3 = std::make_unique<FrameBuffer>(device, 1280 / BLOOM3, 720 / BLOOM3);
	bloom4 = std::make_unique<FrameBuffer>(device, 1280 / BLOOM4, 720 / BLOOM4);

	//------------
	// �v���C���[
	//------------
	//�v���C���[������
	player = new Player();

	//------------
	// �X�e�[�W
	//------------
	//�X�e�[�W�}�l�[�W���[������
	StageManager& stageManager = StageManager::Instance();
	//�X�e�[�W������
	for (int i = 0; i < StageMax; i++)
	{
		stageBase[i] = new StageBase();

		//���W�w��
		stageBase[i]->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 10.0f * i));

		//�X�e�[�W�}�l�[�W���[�ɒǉ�
		stageManager.Register(stageBase[i]);
	}

	//�X�e�[�W�^�C���}�l�[�W���[������
	StageTileManager& stageTileManager = StageTileManager::Instance();
	//�X�e�[�W�^�C��������
	for (int i = 0; i < StageTileMax; i++)
	{
		for (int j = 0; j < StageMax; j++)
		{
			stageTile[j][i] = new StageTile();

			//���W�w��
			stageTile[j][i]->SetPosition(stageBase[j]->squea.spritPosition[i]);

			//�X�e�[�W�^�C���}�l�[�W���[�ɒǉ�
			stageTileManager.Register(stageTile[j][i]);
		}
	}

	//------------
	// �R�C�� 
	//------------
	//�R�C���}�l�[�W���[������
	CoinManager& coinManager = CoinManager::Instance();
	//�R�C��������
	for (int i = 0; i < CoinMax; i++)
	{
		coin[i] = new Coin();

		//���W�w��
		coin[i]->SetPosition(DirectX::XMFLOAT3(
			 0.0f,
			-10.0f,
			-100.0f));

		//�R�C���}�l�[�W���[�ɒǉ�
		coinManager.Register(coin[i]);
	}
	
	//------------
	// ��Q�� 
	//------------
	// ��Q���}�l�[�W���[������
	ObstacleBlockManager& obstacleBlockManager = ObstacleBlockManager::Instance();
	// ��Q��������
	for (int j = 0; j < StageMax; j++)
	{
		for (int i = 0; i < ObstacleMax; i++)
		{
			obstacle[j][i] = new ObstacleBlock();
			obstacle[j][i]->SetPosition(DirectX::XMFLOAT3(
				stageBase[0]->squea.spritPosition[0].x,
				-10.0f,
				stageBase[0]->squea.spritPosition[0].z));
			//�G�l�~�[�}�l�[�W���[�ɒǉ�
			obstacleBlockManager.Register(obstacle[j][i]);
		}
	}


	fadeInFlg = true;
	fadeOutFlg = false;
	ObstacleMove[0] = 0.0f;
	ObstacleMove[1] = -3.0f;

	//���y������
	BGM = framework.GetSoundManager()->CreateSoundSource("Data/Sounds/GameMainBGM.wav");

	//���y�Đ�
	BGM->Play(true);

	//imgui
	/*IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& IO = ImGui::GetIO();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(device, context);
	ImGui::StyleColorsClassic();*/
}

//--------------------------------------------------------
//		�I��
//--------------------------------------------------------
void SceneMain::Finalize()
{
	ObstacleBlockManager::Instance().Clear();
	EnemyManager::Instance().Clear();
	StageManager::Instance().Clear();
	StageTileManager::Instance().Clear();
	CoinManager::Instance().Clear();

	//���̉��
	if (player != nullptr)
	{
		delete player;
		player = nullptr;
	}

	if (cameraController != nullptr)
	{
		delete cameraController;
		cameraController = nullptr;
	}
}


//--------------------------------------------------------
//		�X�V
//--------------------------------------------------------
void SceneMain::Update(float elapsedTime)
{
	timer++;
	
	//���C�g�ݒ�
	DirectX::XMFLOAT3 dir;
	dir.x = LightDir.x;
	dir.y = LightDir.y;
	dir.z = LightDir.z;
	SetDirLight(dir, DirectX::XMFLOAT3(diffuseColor));

	
	FadeBlack->fadeIn(0.03f);

	//�^�[�Q�b�g���镨��pos�����ϐ�
	DirectX::XMFLOAT3 target = player->GetPosition();
	//�J�����̍�������
	target.z -= 2.0f;

	//�J�����Ƀ^�[�Q�b�g��������̂�ݒ�
	cameraController->SetTarget(DirectX::XMFLOAT3(0.0f, 5.8f, target.z));
	//�J�����X�V
	cameraController->Update(elapsedTime);

	//���@�X�V
	player->Update(elapsedTime);
	
	//�X�e�[�W�S�̂̍X�V����
	StageUpdate();

	//��Q���}�l�[�W���[�X�V
	ObstacleBlockManager::Instance().Update(elapsedTime);

	//�R�C���X�V
	CoinManager::Instance().Update(elapsedTime);

	//�X�e�[�W�X�V
	StageManager::Instance().Update(elapsedTime);
	StageTileManager::Instance().Update(elapsedTime);

	//�p�[�e�B�N���X�V
	particles->Update(elapsedTime);
	particles->Star({ player->GetPosition().x,player->GetPosition().y + 5.0f,player->GetPosition().z }, 
					{ 0.0f, 0.0f, -(rand() % 10001) * 0.002f - 0.001f }, 200000);

	//�Q�[���I�[�o�[����
	if (player->GetGameOverFlg())
	{
		fadeOutFlg = true;
	}

	if (fadeOutFlg)
	{
		FadeBlack->fadeOut(0.04f);
		if (FadeBlack->GetAlpha() >= 1.0f)
		{
			BGM->Stop();
			//���U���g�̏��ʌv�Z
			ResultMeter();
			//�I������
			Finalize();
			SceneManager::Instance().ChangeScene((new SceneClear()));
			return;
		}
	}
}

//�X�e�[�W�S�̂̍X�V����
void SceneMain::StageUpdate()
{
	//�̃M�~�b�N����
	if (player->GetBlueFlg() == true && ObstacleMove[0] > -3.5f) ObstacleMove[0] -= 0.1f;
	else if (player->GetBlueFlg() == false && ObstacleMove[0] < 0.0f) ObstacleMove[0] += 0.1f;
	if (player->GetBlueFlg() == false && ObstacleMove[1] > -3.5f) ObstacleMove[1] -= 0.1f;
	else if (player->GetBlueFlg() == true && ObstacleMove[1] < 0.0f) ObstacleMove[1] += 0.1f;

	//�X�e�[�W�^�C���������_���őI�΂ꂽ�p�^�[�����Ƃɔz�u
	for (int j = 0; j < StageMax; j++)
	{
		for (int i = 0; i < StageTileMax; i++)
		{
			//�X�e�[�W��HOLE�̕����ȊO�Ŕz�u
			if (stageTile[j][i]->StageTileMap[stageBase[j]->stageRand][j][i] != stageTile[0][0]->HOLE)
			{
				//------------
				// �^�C���z�u 
				//------------
				stageTile[j][i]->SetPosition(stageBase[j]->squea.spritPosition[i]);

				//------------
				// ��Q���z�u 
				//------------
				//�����Ă���X�e�[�W�^�C���ƃ����_���Ō��܂�����Q���z�u�̔ԍ����ꏏ�Ȃ�z�u
				//if (i == stageBase[j]->SquareRand[0] && stageBase[j]->Rand != 5)

				//�����Ȃ�
				//��Q���P
				if (stageTile[j][i]->StageTileMap[stageBase[j]->stageRand][j][i] == stageTile[0][0]->OB1)
				{
					//�G�l�~�[���X�e�[�W��9�������Ďw�肵���ʒu�ɃZ�b�g
					obstacle[j][0]->SetPosition(DirectX::XMFLOAT3(
						stageBase[j]->squea.spritPosition[i].x,
						1.5f,
						stageBase[j]->squea.spritPosition[i].z));
					//���� ��Ŗ��G�Ƃ��Ŏg������
					obstacle[j][0]->SetExistFlg(true);
				}
				//��Q���Q
				if (stageTile[j][i]->StageTileMap[stageBase[j]->stageRand][j][i] == stageTile[0][0]->OB2)
				{
					//�G�l�~�[���X�e�[�W��9�������Ďw�肵���ʒu�ɃZ�b�g
					obstacle[j][1]->SetPosition(DirectX::XMFLOAT3(
						stageBase[j]->squea.spritPosition[i].x,
						1.5f,
						stageBase[j]->squea.spritPosition[i].z));
					//����
					obstacle[j][1]->SetExistFlg(true);
				}
				//��Q��3
				if (stageTile[j][i]->StageTileMap[stageBase[j]->stageRand][j][i] == stageTile[0][0]->OB3)
				{
					//�G�l�~�[���X�e�[�W��9�������Ďw�肵���ʒu�ɃZ�b�g
					obstacle[j][2]->SetPosition(DirectX::XMFLOAT3(
						stageBase[j]->squea.spritPosition[i].x,
						1.5f,
						stageBase[j]->squea.spritPosition[i].z));
					//����
					obstacle[j][2]->SetExistFlg(true);
				}

				//����
				//��Q��4
				if (stageTile[j][i]->StageTileMap[stageBase[j]->stageRand][j][i] == stageTile[0][0]->MOB1)
				{
					if (stageBase[j]->stageRand == 2 || 5)
					{
						obstacle[j][3]->SetObstacleColor(stageTile[j][i]->StageTileMap[stageBase[j]->stageRand][j][i]);
						//�G�l�~�[���X�e�[�W��9�������Ďw�肵���ʒu�ɃZ�b�g
						obstacle[j][3]->SetPosition(DirectX::XMFLOAT3(
							stageBase[j]->squea.spritPosition[i].x,
							1.5f + ObstacleMove[0],
							stageBase[j]->squea.spritPosition[i].z));
					}
					else
					{
						//�G�l�~�[���X�e�[�W��9�������Ďw�肵���ʒu�ɃZ�b�g
						obstacle[j][3]->SetPosition(DirectX::XMFLOAT3(
							stageBase[j]->squea.spritPosition[i].x,
							1.5f,
							stageBase[j]->squea.spritPosition[i].z));
					}
					//����
					obstacle[j][3]->SetExistFlg(true);
				}

				//��Q��5
				if (stageTile[j][i]->StageTileMap[stageBase[j]->stageRand][j][i] == stageTile[0][0]->MOB2)
				{
					if (stageBase[j]->stageRand == 2 || 5)
					{
						obstacle[j][4]->SetObstacleColor(stageTile[j][i]->StageTileMap[stageBase[j]->stageRand][j][i]);
						//�G�l�~�[���X�e�[�W��9�������Ďw�肵���ʒu�ɃZ�b�g
						obstacle[j][4]->SetPosition(DirectX::XMFLOAT3(
							stageBase[j]->squea.spritPosition[i].x,
							1.5f + ObstacleMove[0],
							stageBase[j]->squea.spritPosition[i].z));
					}
					else
					{
						//�G�l�~�[���X�e�[�W��9�������Ďw�肵���ʒu�ɃZ�b�g
						obstacle[j][4]->SetPosition(DirectX::XMFLOAT3(
							stageBase[j]->squea.spritPosition[i].x,
							1.5f,
							stageBase[j]->squea.spritPosition[i].z));
					}
					//����
					obstacle[j][4]->SetExistFlg(true);
				}

				//��Q��6
				if (stageTile[j][i]->StageTileMap[stageBase[j]->stageRand][j][i] == stageTile[0][0]->MOB3)
				{
					if (stageBase[j]->stageRand == 2||5)
					{
						obstacle[j][5]->SetObstacleColor(stageTile[j][i]->StageTileMap[stageBase[j]->stageRand][j][i]);
						//�G�l�~�[���X�e�[�W��9�������Ďw�肵���ʒu�ɃZ�b�g
						obstacle[j][5]->SetPosition(DirectX::XMFLOAT3(
							stageBase[j]->squea.spritPosition[i].x,
							1.5f + ObstacleMove[1],
							stageBase[j]->squea.spritPosition[i].z));
					}
					else
					{
						//�G�l�~�[���X�e�[�W��9�������Ďw�肵���ʒu�ɃZ�b�g
						obstacle[j][5]->SetPosition(DirectX::XMFLOAT3(
							stageBase[j]->squea.spritPosition[i].x,
							1.5f,
							stageBase[j]->squea.spritPosition[i].z));
					}
					//����
					obstacle[j][5]->SetExistFlg(true);
				}

				//�R�C���z�u
				//�R�C���P
				if (stageTile[j][i]->StageTileMap[stageBase[j]->stageRand][j][i] == stageTile[0][0]->COIN1)
				{
					coin[0]->SetPosition(DirectX::XMFLOAT3(
						stageBase[j]->squea.spritPosition[i].x,
						1.5f,
						stageBase[j]->squea.spritPosition[i].z));

					coin[0]->SetExistFlg(true);
				}

				//�R�C���Q
				if (stageTile[j][i]->StageTileMap[stageBase[j]->stageRand][j][i] == stageTile[0][0]->COIN2)
				{
					coin[1]->SetPosition(DirectX::XMFLOAT3(
						stageBase[j]->squea.spritPosition[i].x,
						1.5f,
						stageBase[j]->squea.spritPosition[i].z));

					coin[1]->SetExistFlg(true);
				}

				//�R�C���R
				if (stageTile[j][i]->StageTileMap[stageBase[j]->stageRand][j][i] == stageTile[0][0]->COIN3)
				{
					coin[2]->SetPosition(DirectX::XMFLOAT3(
						stageBase[j]->squea.spritPosition[i].x,
						1.5f,
						stageBase[j]->squea.spritPosition[i].z));

					coin[2]->SetExistFlg(true);
				}

				//-------------------
				// �^�C���J���[�Z�b�g 
				//-------------------
				//�����Ă���X�e�[�W�^�C���������_���Ō��܂����X�e�[�W�x�[�X��stageTileMap�ԍ����J���[���Z�b�g
				stageTile[j][i]->SetTileColor(stageTile[j][i]->StageTileMap[stageBase[j]->stageRand][j][i]);
			}
			else
			{
				//------------
				// �^�C���z�u 
				//------------
				stageTile[j][i]->SetPosition(DirectX::XMFLOAT3(stageBase[j]->squea.spritPosition[6].x, -20.0f, stageBase[j]->squea.spritPosition[6].z - 100.0f));
			}
		}
	}
}

void SceneMain::imGuiUpdate()
{
	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (ShowDemoWindow)
		ImGui::ShowDemoWindow(&ShowDemoWindow);

	// �v���C���[�f�o�b�O�`��
	player->DrawDebugGUI();
	//EnemyManager::Instance().DrawDebugGUI();
	StageManager::Instance().DrawDebugGUI();
	CoinManager::Instance().DrawDebugGUI();
	//StageTileManager::Instance().DrawDebugGUI();

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	{

		ImGui::Begin(u8"Lightinig");      // Create a window called "Hello, world!" and append into it.
		ImGui::TextColored(ImVec4(1, 1, 0, 1), u8"---------Light---------");

		ImGui::TextColored(ImVec4(1, 1, 0, 1), u8"---------Ambient---------");
		ImGui::ColorEdit3(u8"����", (float*)&ambientColor); // Edit 3 floats representing a color
		ImGui::TextColored(ImVec4(1, 1, 0, 1), u8"---------Diffuse---------");
		ImGui::ColorEdit3(u8"���s��", (float*)&diffuseColor); // Edit 3 floats representing a color
		/*for (int i = 0; i < StageMax; i++)
		{
			ImGui::InputInt("Rand", &stageBase[i]->stageRand);
		}*/
		ImGui::TextColored(ImVec4(1, 1, 0, 1), u8"---------StagePatternNum---------");
		ImGui::InputInt("Rand", &stageBase[0]->stageRand);
		ImGui::Text("true(1)/false(0):%d", player->GetBlueFlg());
		ImGui::InputFloat("ObstacleMove[0]", &ObstacleMove[0]);
		ImGui::InputFloat("ObstacleMove[1]", &ObstacleMove[1]);
		ImGui::SliderFloat(u8"bloom", &threshold, 10.0f, 0.0f);
		ImGui::Separator();

		ImGui::End();
	}

	// 3. Show another simple window.
	if (ShowAnotherWindow)
	{
		ImGui::Begin("Another Window", &ShowAnotherWindow);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			ShowAnotherWindow = false;
		ImGui::End();
	}
}


//--------------------------------------------------------
//		�`��
//--------------------------------------------------------
void SceneMain::RenderBloom()
{
	// �J�����p�����[�^�ݒ�
	Camera& camera = Camera::Instance();

	//���X�^���C�U�\�ݒ�
	context->RSSetState(framework.GetRasterizerState0());
	//�f�v�X�X�e���V���X�e�[�g�ݒ�
	context->OMSetDepthStencilState(framework.GetNoneDepthStencilState(), 1);
	// �r���[�|�[�g�̐ݒ�
	framework.SetViewPort(1280.0f / SCREEN, 720.0f / SCREEN);
	{
		DirectX::XMFLOAT4 lightDirection(0.0f, -1.0f, 0.0f, 0.0f);
		DirectX::XMFLOAT4X4 view = camera.GetView();
		DirectX::XMFLOAT4X4 projection = camera.GetProjection();
		DirectX::XMFLOAT4 materialColor(1.0f, 1.0f, 1.0f, 1.0f);
		{
			// �R���X�^���g�o�b�t�@�ݒ�
			ConstantBufferForPerFrame cb{};
			cb.AmbientColor = ambientColor;
			cb.LightColor = DirLightColor;
			cb.EyePos.x = camera.GetEye().x;
			cb.EyePos.y = camera.GetEye().y;
			cb.EyePos.z = camera.GetEye().z;
			cb.EyePos.w = 1.0f;

			context->UpdateSubresource(ConstantBuffer.Get(), 0, NULL, &cb, 0, 0);
			context->PSSetConstantBuffers(2, 1, ConstantBuffer.GetAddressOf());
		}

		framework.SetSampler(0);
		//�I�t�X�N���[���L��
		offScreen->Activate(context);
		offScreen->Clear(context);

		//�p�[�e�B�N��
		particles->Render(context, ParticleShader.get(), view, projection, framework.GetBlendState(Blender::BS_ALPHA));

		//�[�x�l���r����
		context->OMSetDepthStencilState(framework.GetDepthStencilState(), 1);

		//�v���C���[�`��
		player->Render(context, view, projection, lightDirection, materialColor, false);

		//�R�C��
		CoinManager::Instance().Render(context, view, projection, lightDirection, materialColor, false);

		//�X�e�[�W
		StageTileManager::Instance().Render(context, view, projection, lightDirection, materialColor, false);
		ObstacleBlockManager::Instance().Render(context, view, projection, lightDirection, materialColor, false);

		//�X�s�[�h�A�b�vUI
		if (player->speedLevelCount == 1 || player->speedLevelCount == 2)
		{
			//player->renderTimer == 0;
			if (timer / 32 % 2 && player->renderTimer < 100)
			{
				player->renderTimer++;
				speedUp->Render(context, 0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f);
			}
		}
		if (player->speedLevelCount == 2)
		{
			//player->renderTimer == 0;
			if (timer / 32 % 2 && player->renderTimer < 200)
			{
				player->renderTimer++;
				speedUp->Render(context, 0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f);
			}
		}
		//�X�s�[�h�}�b�N�XUI	
		if (player->speedLevelCount == 3)
		{
			if (timer / 32 % 2 && player->renderTimer < 300)
			{
				player->renderTimer++;
				speedMax->Render(context, 0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f);
			}
		}

		//�i�񂾋����\��
		float fontSize = 16;
		const char* name = "M";
		Sprite* Font = framework.GetFont();
		std::string pMeter = to_string((int)player->GetPosition().z);
		pMeter += name;
		Font->TextOut(context, pMeter, 0, 0, 64, 64, 0, 1, 1, 1);

		//�R�C����
		const char* coin = "Coin";
		Sprite* CoinFont = framework.GetFont();
		std::string cMeter = to_string(player->GetCoinCount());
		CoinFont->TextOut(context, coin, 0, 80, 32, 32, 1, 1, 0, 1);
		CoinFont->TextOut(context, cMeter, 140, 80, 32, 32, 1, 1, 0, 1);

		//�I�t�X�N���[������
		offScreen->Deactivate(context);
	}
	//Bright
	{
		//���X�^���C�U�\�ݒ�
		context->RSSetState(framework.GetRasterizerState0());
		//�f�v�X�X�e���V���X�e�[�g�ݒ�
		context->OMSetDepthStencilState(framework.GetDepthStencilState(), 1);
		// �r���[�|�[�g�̐ݒ�
		framework.SetViewPort(1280.0f / SCREEN, 720.0f / SCREEN);
		{
			DirectX::XMFLOAT4 lightDirection(0.0f, -1.0f, 0.0f, 0.0f);
			DirectX::XMFLOAT4X4 view = camera.GetView();
			DirectX::XMFLOAT4X4 projection = camera.GetProjection();
			DirectX::XMFLOAT4 materialColor(1.0f, 1.0f, 1.0f, 1.0f);
			{
				// �R���X�^���g�o�b�t�@�ݒ�
				ConstantBufferForBloom cb{};
				cb.threshold = threshold;
				context->UpdateSubresource(ConstantBufferBloom.Get(), 0, NULL, &cb, 0, 0);
				context->PSSetConstantBuffers(3, 1, ConstantBufferBloom.GetAddressOf());
			}

			//�I�t�X�N���[���L��
			bright->Activate(context);
			bright->Clear(context);

			framework.SetSamplerCramp(0);
			ID3D11ShaderResourceView* ShaderResourceViews[2]{
				offScreen->getShaderResourceView(0),
				offScreen->getShaderResourceView(1),
			};
			fullScreenBright->Blit(context, ShaderResourceViews, 0, 2, BrightShader.get());


			//�I�t�X�N���[������
			bright->Deactivate(context);
		}
	}
	//Bloom1
	{
		
		//���X�^���C�U�\�ݒ�
		context->RSSetState(framework.GetRasterizerState0());
		//�f�v�X�X�e���V���X�e�[�g�ݒ�
		context->OMSetDepthStencilState(framework.GetDepthStencilState(), 1);
		// �r���[�|�[�g�̐ݒ�
		framework.SetViewPort(1280.0f / BLOOM1, 720.0f / BLOOM1);
		{
			DirectX::XMFLOAT4 lightDirection(0.0f, -1.0f, 0.0f, 0.0f);
			DirectX::XMFLOAT4X4 view = camera.GetView();
			DirectX::XMFLOAT4X4 projection = camera.GetProjection();
			DirectX::XMFLOAT4 materialColor(1.0f, 1.0f, 1.0f, 1.0f);
			{
				// �R���X�^���g�o�b�t�@�ݒ�
				ConstantBufferForBloom cb{};
				cb.threshold = threshold;
				context->UpdateSubresource(ConstantBufferBloom.Get(), 0, NULL, &cb, 0, 0);
				context->PSSetConstantBuffers(3, 1, ConstantBufferBloom.GetAddressOf());
			}

			//�I�t�X�N���[���L��
			bloom1->Activate(context);
			bloom1->Clear(context);

			framework.SetSamplerCramp(0);
			ID3D11ShaderResourceView* ShaderResourceViews[2]{
				bright->getShaderResourceView(0),
				bright->getShaderResourceView(1),
			};
			fullScreenBright->Blit(context, ShaderResourceViews, 0, 2, BrightShader.get());

			//�I�t�X�N���[������
			bloom1->Deactivate(context);
		}
	}
	//Bloom2
	{
		//���X�^���C�U�\�ݒ�
		context->RSSetState(framework.GetRasterizerState0());
		//�f�v�X�X�e���V���X�e�[�g�ݒ�
		context->OMSetDepthStencilState(framework.GetDepthStencilState(), 1);
		// �r���[�|�[�g�̐ݒ�
		framework.SetViewPort(1280.0f / BLOOM2, 720.0f / BLOOM2);
		{
			DirectX::XMFLOAT4 lightDirection(0, -1, 0, 0);
			DirectX::XMFLOAT4X4 view = camera.GetView();
			DirectX::XMFLOAT4X4 projection = camera.GetProjection();
			DirectX::XMFLOAT4 materialColor(1.0f, 1.0f, 1.0f, 1.0f);
			{
				// �R���X�^���g�o�b�t�@�ݒ�
				ConstantBufferForBloom cb{};
				cb.threshold = threshold;
				context->UpdateSubresource(ConstantBufferBloom.Get(), 0, NULL, &cb, 0, 0);
				context->PSSetConstantBuffers(3, 1, ConstantBufferBloom.GetAddressOf());
			}

			//�I�t�X�N���[���L��
			bloom2->Activate(context);
			bloom2->Clear(context);

			framework.SetSamplerCramp(0);
			ID3D11ShaderResourceView* ShaderResourceViews[2]{
				bloom1->getShaderResourceView(0),
				bloom1->getShaderResourceView(1),
			};
			fullScreenBright->Blit(context, ShaderResourceViews, 0, 2, BrightShader.get());

			//�I�t�X�N���[������
			bloom2->Deactivate(context);
		}
	}
	//Bloom3
	{
		//���X�^���C�U�\�ݒ�
		context->RSSetState(framework.GetRasterizerState0());
		//�f�v�X�X�e���V���X�e�[�g�ݒ�
		context->OMSetDepthStencilState(framework.GetDepthStencilState(), 1);
		// �r���[�|�[�g�̐ݒ�
		framework.SetViewPort(1280.0f / BLOOM3, 720.0f / BLOOM3);
		{
			DirectX::XMFLOAT4 lightDirection(0.0f, -1.0f, 0.0f, 0.0f);
			DirectX::XMFLOAT4X4 view = camera.GetView();
			DirectX::XMFLOAT4X4 projection = camera.GetProjection();
			DirectX::XMFLOAT4 materialColor(1.0f, 1.0f, 1.0f, 1.0f);
			{
				// �R���X�^���g�o�b�t�@�ݒ�
				ConstantBufferForBloom cb{};
				cb.threshold = threshold;
				context->UpdateSubresource(ConstantBufferBloom.Get(), 0, NULL, &cb, 0, 0);
				context->PSSetConstantBuffers(3, 1, ConstantBufferBloom.GetAddressOf());
			}
		    
			//�I�t�X�N���[���L��
			bloom3->Activate(context);
			bloom3->Clear(context);

			framework.SetSamplerCramp(0);
			ID3D11ShaderResourceView* ShaderResourceViews[2]{
				bloom2->getShaderResourceView(0),
				bloom2->getShaderResourceView(1),
			};
			fullScreenBright->Blit(context, ShaderResourceViews, 0, 2, BrightShader.get());

			//�I�t�X�N���[������
			bloom3->Deactivate(context);
		}
	}
	//Bloom4
	{
		
		//���X�^���C�U�\�ݒ�
		context->RSSetState(framework.GetRasterizerState0());
		//�f�v�X�X�e���V���X�e�[�g�ݒ�
		context->OMSetDepthStencilState(framework.GetDepthStencilState(), 1);
		// �r���[�|�[�g�̐ݒ�
		framework.SetViewPort(1280.0f / BLOOM4, 720.0f / BLOOM4);
		{
			DirectX::XMFLOAT4 lightDirection(0.0f, -1.0f, 0.0f, 0.0f);
			DirectX::XMFLOAT4X4 view = camera.GetView();
			DirectX::XMFLOAT4X4 projection = camera.GetProjection();
			DirectX::XMFLOAT4 materialColor(1.0f, 1.0f, 1.0f, 1.0f);
			{
				// �R���X�^���g�o�b�t�@�ݒ�
				ConstantBufferForBloom cb{};
				cb.threshold = threshold;
				context->UpdateSubresource(ConstantBufferBloom.Get(), 0, NULL, &cb, 0, 0);
				context->PSSetConstantBuffers(3, 1, ConstantBufferBloom.GetAddressOf());
			}

			
			//�I�t�X�N���[���L��
			bloom4->Activate(context);
			bloom4->Clear(context);

			framework.SetSamplerCramp(0);
			ID3D11ShaderResourceView* ShaderResourceViews[2]{
				bloom3->getShaderResourceView(0),
				bloom3->getShaderResourceView(1),
			};
			fullScreenBright->Blit(context, ShaderResourceViews, 0, 2, BrightShader.get());

			//�I�t�X�N���[������
			bloom4->Deactivate(context);
		}
	}

	//�����_�[�^�[�Q�b�g�̉�
	//ID3D11RenderTargetView* backbuffer = framework.GetRenderTargetView();
	//context->OMSetRenderTargets(1, &backbuffer, framework.GetDepthStencilView());
}

void SceneMain::Render()
{
	//imGuiUpdate();

	//�u���[��
	RenderBloom();

	// ��ʃN���A
	FLOAT color[] = { 0.0f, 0.0f, 0.0f, 1.0f };

	ID3D11RenderTargetView* rtv = framework.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = framework.GetDepthStencilView();

	// �����_�[�^�[�Q�b�g��ݒ�
	context->OMSetRenderTargets(1, &rtv, dsv);
	context->ClearRenderTargetView(rtv, color);
	context->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->OMSetDepthStencilState(framework.GetNoneDepthStencilState(), 1);

	// �r���[�|�[�g�̐ݒ�
	framework.SetViewPort(static_cast<float>(framework.GetScreenWidth()), static_cast<float>(framework.GetScreenHeight()));

	// �J�����p�����[�^�ݒ�
	Camera& camera = Camera::Instance();

	ID3D11BlendState* BsAlpha = framework.GetBlendState(Blender::BS_ALPHA);
	ID3D11BlendState* BsAdd = framework.GetBlendState(Blender::BS_ADD);
	context->OMSetBlendState(BsAlpha, nullptr, 0xFFFFFFFF);

	DirectX::XMFLOAT4 lightDirection(0.0f, -1.0f, 0.0f, 0.0f);

	DirectX::XMFLOAT4X4 view = camera.GetView();

	DirectX::XMFLOAT4X4 projection = camera.GetProjection();

	DirectX::XMFLOAT4 materialColor(1.0f, 1.0f, 1.0f, 1.0f);

	{
		ConstantBufferForPerFrame cb;
		cb.AmbientColor = ambientColor;
		cb.LightColor = DirLightColor;
		cb.EyePos.x = camera.GetEye().x;
		cb.EyePos.y = camera.GetEye().y;
		cb.EyePos.z = camera.GetEye().z;
		cb.EyePos.w = 1.0f;

		context->UpdateSubresource(ConstantBuffer.Get(), 0, NULL, &cb, 0, 0);
		context->VSSetConstantBuffers(2, 1, ConstantBuffer.GetAddressOf());
		context->PSSetConstantBuffers(2, 1, ConstantBuffer.GetAddressOf());
	}

	//�u���[���֌W�����_�����O
	framework.SetSamplerCramp(0);
	ID3D11ShaderResourceView* ShaderResourceViews[2]{
		offScreen->getShaderResourceView(0),
		offScreen->getShaderResourceView(1),
	};
	fullScreen->Blit(context, ShaderResourceViews, 0, 2, PixelShaders[1].Get());

	context->OMSetBlendState(BsAdd, nullptr, 0xFFFFFFFF);

	ID3D11ShaderResourceView* BokehShaderResourceViews1[2]{
		bloom1->getShaderResourceView(0),
		bloom1->getShaderResourceView(1),
	};
	fullScreenBokeh->Blit(context, BokehShaderResourceViews1, 0, 2, BokehShader.get());

	ID3D11ShaderResourceView* BokehShaderResourceViews2[2]{
		bloom2->getShaderResourceView(0),
		bloom2->getShaderResourceView(1),
	};
	fullScreenBokeh->Blit(context, BokehShaderResourceViews2, 0, 2, BokehShader.get());

	ID3D11ShaderResourceView* BokehShaderResourceViews3[2]{
		bloom3->getShaderResourceView(0),
		bloom3->getShaderResourceView(1),
	};
	fullScreenBokeh->Blit(context, BokehShaderResourceViews3, 0, 2, BokehShader.get());

	ID3D11ShaderResourceView* BokehShaderResourceViews4[2]{
		bloom4->getShaderResourceView(0),
		bloom4->getShaderResourceView(1),
	};
	fullScreenBokeh->Blit(context, BokehShaderResourceViews4, 0, 2, BokehShader.get());

	context->OMSetBlendState(BsAlpha, nullptr, 0xFFFFFFFF);

	// ���[���h���W����X�N���[�����W��
	DirectX::XMFLOAT3 worldPosition = player->GetPosition();
	DirectX::XMFLOAT3 screenPosition;
	worldPosition.y += 1.5f;
	WorldToScreen(&screenPosition, worldPosition);

	FadeBlack->render(context);
	
	//imGui
	//Scene::imGuiRender();
}

void Scene::imGuiRender()
{
	// Rendering
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void SceneMain::ResultMeter()
{
	//�t�@�C���ǂݎ��
	std::string Filename = "Text/score.txt";
	std::ifstream ReadingFile(Filename, std::ios::in);
	for (int i = 0; i < 5; i++)
	{
		ReadingFile >> Result[i];
	}

	//�X�R�A���ёւ�
	for (int i = 0; i < 5; i++)
	{
		if ((int)player->GetPosition().z > Result[i])
		{
			for (int j = 4; j >= (i + 1); j--)
			{
				Result[j] = Result[j - 1];
			}
			Result[i] = (int)player->GetPosition().z;
			break;
		}
	}

	//�t�@�C����������
	Filename = "Text/score.txt";
	std::ofstream WritingFile;
	WritingFile.open(Filename, std::ios::out);
	for (int i = 0; i < 5; i++)
	{
		WritingFile << Result[i] << std::endl;
	}
	//����̃��U���g��������
	WritingFile << (int)player->GetPosition().z << std::endl;
}


//--------------------------------------------------------
//�@�@���[���h�E�X�N���[���ϊ��֐�	
//--------------------------------------------------------
void SceneMain::WorldToScreen(DirectX::XMFLOAT3* screenPosition, const DirectX::XMFLOAT3& worldPosition)
{
	Framework& framework = Framework::Instance();

	Camera& camera = Camera::Instance();
	// �r���[�|�[�g
	float viewportX = 0.0f;
	float viewportY = 0.0f;
	float viewportW = static_cast<float>(framework.GetScreenWidth());
	float viewportH = static_cast<float>(framework.GetScreenHeight());
	float viewportMinZ = 0.0f;
	float viewportMaxZ = 1.0f;

	// �r���[�s��
	DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&camera.GetView());

	// �v���W�F�N�V�����s��
	DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&camera.GetProjection());

	// ���[���h�s��
	DirectX::XMMATRIX W = DirectX::XMMatrixIdentity();

	DirectX::XMVECTOR wp = DirectX::XMLoadFloat3(&worldPosition);

#if 0
	DirectX::XMVECTOR SPos = DirectX::XMVector3Project(wp, viewportX, viewportY, viewportW, viewportH, viewportMinZ, viewportMaxZ, P, V, W);
	DirectX::XMStoreFloat3(screenPosition, SPos);
#else 1
	// ���[���h���W���� NDC ���W�֕ϊ�
	DirectX::XMMATRIX WVP = W * V * P;
	DirectX::XMVECTOR NDCPosition = DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&worldPosition), WVP);
	DirectX::XMFLOAT3 ndcPosition;
	DirectX::XMStoreFloat3(&ndcPosition, NDCPosition);


	//viewportX = viewportY = viewportMinZ = 0.0f
	//ndc���W����X�N���[�����W�ւ̕ϊ�
	screenPosition->x = viewportX + (1.0f + ndcPosition.x) * viewportW * 0.5;
	screenPosition->y = viewportY + (1.0f - ndcPosition.y) * viewportH * 0.5;
	screenPosition->z = viewportMinZ + ndcPosition.z * (viewportMaxZ - viewportMinZ);

#endif // 1

}

void SceneMain::ScreenToWorld(DirectX::XMFLOAT3* worldPosition, const DirectX::XMFLOAT3& screenPosition)
{
	Framework& framework = Framework::Instance();

	Camera& camera = Camera::Instance();
	// �r���[�|�[�g
	float viewportX = 0.0f;
	float viewportY = 0.0f;
	float viewportW = static_cast<float>(framework.GetScreenWidth());
	float viewportH = static_cast<float>(framework.GetScreenHeight());
	float viewportMinZ = 0.0f;
	float viewportMaxZ = 1.0f;

	// �r���[�s��
	DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&camera.GetView());

	// �v���W�F�N�V�����s��
	DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&camera.GetProjection());

	// ���[���h�s��
	DirectX::XMMATRIX W = DirectX::XMMatrixIdentity();

	DirectX::XMVECTOR sp = DirectX::XMLoadFloat3(&screenPosition);

#if 0
	DirectX::XMVECTOR WPos = DirectX::XMVector3Unproject(sp, viewportX, viewportY, viewportW, viewportH, viewportMinZ, viewportMaxZ, P, V, W);
	DirectX::XMStoreFloat3(worldPosition, WPos);
#else 1
	DirectX::XMVECTOR NDCpos = DirectX::XMVectorSet(2.0f * (screenPosition.x - viewportX) / viewportW - 1.0f,
		1.0f - 2.0f * (screenPosition.y - viewportY) / viewportH,
		(screenPosition.z - viewportMinZ) / (viewportMaxZ - viewportMinZ), 1.0f);

	DirectX::XMMATRIX WVP = W * V * P;
	DirectX::XMMATRIX IWVP = DirectX::XMMatrixInverse(nullptr, WVP);
	DirectX::XMVECTOR WPos = DirectX::XMVector3TransformCoord(NDCpos, IWVP);
	DirectX::XMStoreFloat3(worldPosition, WPos);

#endif // 1

}


//--------------------------------------------------------
//		���C�g�Z�b�g
//--------------------------------------------------------
void SceneMain::SetDirLight(DirectX::XMFLOAT3 dir, DirectX::XMFLOAT3 color)
{
	float d = sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);

	if (d > 0) { dir.x /= d; dir.y /= d; dir.z /= d; }
	LightDir = DirectX::XMFLOAT4(dir.x, dir.y, dir.z, 0);
	LightDir = DirectX::XMFLOAT4(dir.x, dir.y, dir.z, 0);
	DirLightColor = DirectX::XMFLOAT4(color.x, color.y, color.z, 1);
}