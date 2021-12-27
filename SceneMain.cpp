#include "scene.h"
#include "KeyInput.h"
#include "framework.h"
#include "collision.h"
#include "Input/Input.h"
#include "Camera.h"
#include "EnemyManager.h"
#include "ObstacleBlockManager.h"
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
	Framework& framework = Framework::Instance();
	ID3D11Device* device = framework.GetDevice();
	ID3D11DeviceContext* Context = framework.GetImmediateContext();
	HWND hwnd = framework.GetWindowHandle();

	//�R���X�^���g�o�b�t�@
	CreateConstantBuffer(&ConstantBuffer, sizeof(ConstantBufferForPerFrame));

	FadeBlack = std::make_unique<Transition>();
	FadeBlack->init(device, L"Data/unnamed.png", { 0,0 }, { 1920,1080 }, { 0,0 }, { 1920,1080 }, { 1.0f,1.0f,1.0f,1.0f });

	float screenWidth = static_cast<float>(framework.GetScreenWidth());
	float screenHeight = static_cast<float>(framework.GetScreenHeight());

	//�J����������
	Camera& camera = Camera::Instance();

	//�J�����̐ݒ�
	camera.SetLookAt(
		DirectX::XMFLOAT3(0, 100, -20),  //�J�����̈ʒu
		DirectX::XMFLOAT3(0, 0, 0),      //�J�����̏œ_
		DirectX::XMFLOAT3(0, 1, 0)		 //�J�����̏���������߂�
	);

	//�J�����̎���p
	camera.SetPerspecticeFov(
		DirectX::XMConvertToRadians(45),
		screenWidth / screenHeight,
		0.1f,
		1000.0f
	);


	skyMesh = std::make_shared<SkinnedMesh>(device, "Data/Sky/sky.obj",true);
	skyObj = std::make_unique<SkinnedObject>(skyMesh);
	skyObj->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	skyObj->SetScale(DirectX::XMFLOAT3(7.0f, 7.0f, 7.0f));
	skyObj->SetAngle(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));


	// �J�����R���g���[���[������
	cameraController = new CameraController();

	//�v���C���[������
	player = new Player();

	//���X�e�[�W������
	stage = new Stage();

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

		//�����ʒu���������Ȃ��p�^�[���K�p
		stageBase[0]->Rand = 5;

		//�X�e�[�W�}�l�[�W���[�ɒǉ�
		stageManager.Register(stageBase[i]);
	}


	//--------------
	// �X�e�[�W�^�C��
	//--------------
	//�X�e�[�W�^�C���}�l�[�W���[������
	StageTileManager& stageTileManager = StageTileManager::Instance();
	//�X�e�[�W�^�C��������
	for (int i = 0; i < StageTileMax; i++)
	{
		for (int j = 0; j < StageMax; j++)
		{
			stageTile[j][i] = new StageTile();

			//���W�w��
			stageTile[j][i]->SetPosition(stageBase[j]->squea.SpritPosition[i]);

			//�X�e�[�W�^�C���}�l�[�W���[�ɒǉ�
			stageTileManager.Register(stageTile[j][i]);
		}
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
				stageBase[0]->squea.SpritPosition[0].x,
				-10.0f,
				stageBase[0]->squea.SpritPosition[0].z));
			//�G�l�~�[�}�l�[�W���[�ɒǉ�
			obstacleBlockManager.Register(obstacle[j][i]);
		}
	}

	// �G�l�~�[�}�l�[�W���[������
	//EnemyManager& enemyManager = EnemyManager::Instance();
	//for (int j = 0; j < StageMax; j++)
	//{
	//	for (int i = 0; i < 2; i++)
	//	{
	//		enemy[j][i] = new EnemyBlueSlime();
	//		enemy[j][i]->SetPosition(DirectX::XMFLOAT3(
	//			stageBase[0]->squea.SpritPosition[0].x,
	//			-10.0f,
	//			stageBase[0]->squea.SpritPosition[0].z));
	//		//�G�l�~�[�}�l�[�W���[�ɒǉ�
	//		enemyManager.Register(enemy[j][i]);
	//	}
	//}

	

	//for (int i = 0; i < StageMax; i++)
	//{
	//	//���W�w��
	//	stageBase[i]->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 10.0f * i));

	//	//�����ʒu���������Ȃ��p�^�[���K�p
	//	stageBase[0]->Rand = 5;

	//	//�X�e�[�W�}�l�[�W���[�ɒǉ�
	//	stageManager.Register(stageBase[i]);
	//}

	//

	//for (int i = 0; i < 9; i++)
	//{
	//	for (int j = 0; j < StageMax; j++)
	//	{
	//		//���W�w��
	//		stageTile[j][i]->SetPosition(stageBase[j]->squea.SpritPosition[i]);
	//		//�X�e�[�W�^�C���}�l�[�W���[�ɒǉ�
	//		stageTileManager.Register(stageTile[j][i]);
	//	}
	//}


	state = 0;
	timer = 0;
	fadeInFlg = true;
	fadeOutFlg = false;

	//���y������
	BGM = framework.GetSoundManager()->CreateSoundSource("Data/Sounds/stage.wav");

	//���y�Đ�
	//BGM->Play(true);

	//imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& IO = ImGui::GetIO();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(device, Context);
	ImGui::StyleColorsClassic();

}

//--------------------------------------------------------
//		�I��
//--------------------------------------------------------
void SceneMain::Finalize()
{
	//���̉��
	if (player != nullptr)
	{
		delete player;
		player = nullptr;
	}
	ObstacleBlockManager::Instance().Clear();
	EnemyManager::Instance().Clear();
	StageManager::Instance().Clear();
	StageTileManager::Instance().Clear();
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


	//if (fadeInFlg)
	//{
	//	FadeBlack->fadeIn(0.03f);

    //  //�t�F�[�h�C������O�̃X�e�[�W�X�V	
	//	StageManager::Instance().Update(elapsedTime);
	//	StageTileManager::Instance().Update(elapsedTime);
	//	EnemyManager::Instance().Update(elapsedTime);

	//	//�t�F�[�h�C������O�̃J�����̈ʒu�A�^�[�Q�b�g�ݒ�
	//	DirectX::XMFLOAT3 target = player->GetPosition();
	//	target.y += 3.5f;
	//	cameraController->SetTarget(DirectX::XMFLOAT3(0.0f, target.y, target.z));

	//	//�t�F�[�h�C������O�̃J�����X�V
	//	cameraController->Update(elapsedTime);

	//	if (FadeBlack->GetAlpha() <= 0.0f) fadeInFlg = false;
	//}

	//if (!fadeInFlg)
	{
		FadeBlack->fadeIn(0.03f);

		//�^�[�Q�b�g���镨��pos�����ϐ�
		DirectX::XMFLOAT3 target = player->GetPosition();
		//�J�����̍�������
		target.z -= 2.0f;

		//�J�����Ƀ^�[�Q�b�g��������̂�ݒ�
		cameraController->SetTarget(DirectX::XMFLOAT3(0.0f, 5.8f, target.z));
		//�J�����X�V
		cameraController->Update(elapsedTime);

		skyObj->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, player->GetPosition().z));
		skyObj->Update();

		//���@�X�V
		player->Update(elapsedTime);

		
		//�X�e�[�W�^�C���������_���őI�΂ꂽ�p�^�[�����Ƃɔz�u
		for (int j = 0; j < StageMax; j++)
		{
			for (int i = 0; i < StageTileMax; i++)
			{
				//�X�e�[�W��HOLE�̕����ȊO�Ŕz�u
				//if (stageTile[j][i]->stageTileMap[stageBase[j]->Pattern][stageBase[j]->Rand][i] != HOLE)
				if (stageTile[j][i]->stageTileMapEasy[stageBase[j]->EasyRand][j][i] != 1)
				{
					//------------
					// �^�C���z�u 
					//------------
					stageTile[j][i]->SetPosition(stageBase[j]->squea.SpritPosition[i]);

					//------------
					// ��Q���z�u 
					//------------
					//�����Ă���X�e�[�W�^�C���ƃ����_���Ō��܂�����Q���z�u�̔ԍ����ꏏ�Ȃ�z�u
					//if (i == stageBase[j]->SquareRand[0] && stageBase[j]->Rand != 5)
					//��Q���P
					//if (stageTile[j][i]->stageTileMap[stageBase[j]->Pattern][stageBase[j]->Rand][i] == OB1)
					if (stageTile[j][i]->stageTileMapEasy[stageBase[j]->EasyRand][j][i] == 2)
					{
						//�G�l�~�[���X�e�[�W��9�������Ďw�肵���ʒu�ɃZ�b�g
						obstacle[j][0]->SetPosition(DirectX::XMFLOAT3(
							stageBase[j]->squea.SpritPosition[i].x,
							1.5f, 
							stageBase[j]->squea.SpritPosition[i].z));
						//����
						obstacle[j][0]->SetExistFlg(true);
					}
					//��Q���Q
					//if (stageTile[j][i]->stageTileMap[stageBase[j]->Pattern][stageBase[j]->Rand][i] == OB2)
					if (stageTile[j][i]->stageTileMapEasy[stageBase[j]->EasyRand][j][i] == 3)
					{
						//�G�l�~�[���X�e�[�W��9�������Ďw�肵���ʒu�ɃZ�b�g
						obstacle[j][1]->SetPosition(DirectX::XMFLOAT3(
							stageBase[j]->squea.SpritPosition[i].x,
							1.5f,
							stageBase[j]->squea.SpritPosition[i].z));
						//����
						obstacle[j][1]->SetExistFlg(true);
					}
					//��Q��3
					//if (stageTile[j][i]->stageTileMap[stageBase[j]->Pattern][stageBase[j]->Rand][i] == OB3)
					if (stageTile[j][i]->stageTileMapEasy[stageBase[j]->EasyRand][j][i] == 4)
					{
						//�G�l�~�[���X�e�[�W��9�������Ďw�肵���ʒu�ɃZ�b�g
						obstacle[j][2]->SetPosition(DirectX::XMFLOAT3(
							stageBase[j]->squea.SpritPosition[i].x,
							1.5f,
							stageBase[j]->squea.SpritPosition[i].z));
						//����
						obstacle[j][2]->SetExistFlg(true);
					}

					//-------------------
					// �^�C���J���[�Z�b�g 
					//-------------------
					//�����Ă���X�e�[�W�^�C���ƃ����_���Ō��܂����X�e�[�W�x�[�X��stageTileMap�ԍ����J���[���Z�b�g
					//stageTile[j][i]->SetTileColor(stageTile[j][i]->stageTileMap[stageBase[j]->Pattern][stageBase[j]->Rand][i]);
					stageTile[j][i]->SetTileColor(stageTile[j][i]->stageTileMapEasy[stageBase[j]->EasyRand][j][i]);
					/*if (stageTile[j][i]->stageTileMap[stageBase[j]->Pattern][stageBase[j]->Rand][i] == 5)
					{
						stageTile[j][i]->SetTileColor(stageTile[j][i]->stageTileMap[stageBase[j]->Pattern][stageBase[j]->Rand][i]);
					}
					else
					{
						stageTile[j][i]->SetTileColor(stageTile[j][i]->stageTileMap[stageBase[j]->Pattern][stageBase[j]->Rand][i]);
					}*/
				}
				else
				{
					//------------
					// �^�C���z�u 
					//------------
					//�����J�������̃^�C�����d�˂ĉB��
					stageTile[j][i]->SetPosition(DirectX::XMFLOAT3(stageBase[j]->squea.SpritPosition[6].x, -20.0f, stageBase[j]->squea.SpritPosition[6].z-100.0f));

					//------------
					// ��Q���z�u 
					//------------
					//�g��Ȃ���Q�������ɔz�u���Ă���
					//obstacle[j][0]->SetPosition(DirectX::XMFLOAT3(
					//	stageBase[0]->squea.SpritPosition[0].x,
					//	-10.0f,
					//	stageBase[0]->squea.SpritPosition[0].z));
					//obstacle[j][1]->SetPosition(DirectX::XMFLOAT3(
					//	stageBase[0]->squea.SpritPosition[0].x,
					//	-10.0f,
					//	stageBase[0]->squea.SpritPosition[0].z));

					////���ɍs������Q���̑��݂������i�����蔻���`������Ȃ����Ă���j
					//obstacle[j][0]->SetExistFlg(false);
					//obstacle[j][1]->SetExistFlg(false);
				}
			}
		}

		//�G�l�~�[�}�l�[�W���[�X�V
		//EnemyManager::Instance().Update(elapsedTime);

		//��Q���}�l�[�W���[�X�V
		ObstacleBlockManager::Instance().Update(elapsedTime);

		//�X�e�[�W�X�V
		StageManager::Instance().Update(elapsedTime);
		StageTileManager::Instance().Update(elapsedTime);
		//�Q�[���I�[�o�[����
		if (player->GetGameOverFlg())
		{
			fadeOutFlg = true;
		}
	}

	if (fadeOutFlg)
	{
		FadeBlack->fadeOut(0.04f);
		if (FadeBlack->GetAlpha() >= 1.0f)
		{
			ResultMeter();
			Finalize();
			SceneManager::Instance().ChangeScene((new SceneOver()));
			return;
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

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	{

		ImGui::Begin(u8"Lightinig");      // Create a window called "Hello, world!" and append into it.
		ImGui::TextColored(ImVec4(1, 1, 0, 1), u8"---------Light---------");

		ImGui::TextColored(ImVec4(1, 1, 0, 1), u8"---------Ambient---------");
		ImGui::ColorEdit3(u8"����", (float*)&ambientColor); // Edit 3 floats representing a color
		ImGui::TextColored(ImVec4(1, 1, 0, 1), u8"---------Diffuse---------");
		ImGui::ColorEdit3(u8"���s��", (float*)&diffuseColor); // Edit 3 floats representing a color
		for (int i = 0; i < StageMax; i++)
		{
			ImGui::InputInt("Rand", &stageBase[i]->EasyRand);
		}
		//ImGui::TextColored(ImVec4(1, 1, 0, 1), u8"---------�������C�g---------");
		//ImGui::ColorEdit3(u8"SkyColor", (float*)&SkyColor); // Edit 3 floats representing a color
		//ImGui::ColorEdit3(u8"GroundColor", (float*)&GroundColor); // Edit 3 floats representing a color
		//ImGui::ColorEdit3(u8"FogColor", (float*)&FogColor);
		//ImGui::SliderFloat(u8"Near", &FogNear, 0.0f, 100.0f);
		//ImGui::SliderFloat(u8"far", &FogFar, 0.0f, 100.0f);
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

	// �v���C���[�f�o�b�O�`��
	player->DrawDebugGUI();
	//EnemyManager::Instance().DrawDebugGUI();
	StageManager::Instance().DrawDebugGUI();
	//StageTileManager::Instance().DrawDebugGUI();
}



//--------------------------------------------------------
//		�`��
//--------------------------------------------------------

void SceneMain::Render()
{
	imGuiUpdate();

	Framework& framework = Framework::Instance();

	ID3D11DeviceContext* context = framework.GetImmediateContext();

	FLOAT color[] = { 0.4f, 0.4f, 0.9f, 1.0f };
	//FLOAT color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	ID3D11RenderTargetView* rtv = framework.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = framework.GetDepthStencilView();
	context->ClearRenderTargetView(rtv, color);
	context->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->OMSetRenderTargets(1, &rtv, dsv);

	// �J�����p�����[�^�ݒ�
	Camera& camera = Camera::Instance();

	ID3D11BlendState* bs = framework.GetBlendState(Blender::BS_ALPHA);
	context->OMSetBlendState(bs, nullptr, 0xFFFFFFFF);

	DirectX::XMFLOAT4 lightDirection(0, -1, 0, 0);

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

	//�v���C���[�`��
	player->Render(context, view, projection, lightDirection, materialColor, false);

	//�X�e�[�W�`��
	//StageManager::Instance().Render(context, view, projection, lightDirection, materialColor, false);
	StageTileManager::Instance().Render(context, view, projection, lightDirection, materialColor, false);

	//��Q���`��
	ObstacleBlockManager::Instance().Render(context, view, projection, lightDirection, materialColor, false);

	// �G�l�~�[�`��
	//EnemyManager::Instance().Render(context, view, projection, lightDirection, materialColor, false);

	//skyObj->Render(context, view, projection, lightDirection, DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), 0.0f, false);

	// ���[���h���W����X�N���[�����W��
	DirectX::XMFLOAT3 worldPosition = player->GetPosition();
	DirectX::XMFLOAT3 screenPosition;
	worldPosition.y += 1.5f;
	WorldToScreen(&screenPosition, worldPosition);

	// ��Ԃ̓���ɖ��O��\��
	float fontSize = 16;
	const char* name = "TANK";
	float width = fontSize * strlen(name);
	Sprite* Font = framework.GetFont();
	/*font->TextOut(context, name,
		screenPosition.x - width / 2,
		screenPosition.y,
		fontSize, fontSize);*/

	name = "M";
	std::string pMeter = to_string((int)player->GetPosition().z);
	pMeter += name;
	//font->TextOut(context, name, 0, 0, 32, 32);
	Font->TextOut(context, pMeter, framework.GetScreenWidth()/2 - 100, 0, 64, 64);

	FadeBlack->render(context);

	//imGui
	Scene::imGuiRender();
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


void Scene::imGuiRender()
{
	// Rendering
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
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