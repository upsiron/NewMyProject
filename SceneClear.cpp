
#include "scene.h"
#include "Camera.h"
#include "KeyInput.h"
//#include "framework.h"

#include <iostream>
#include <string>    
#include <fstream>   

//**********************************************
//
//		�N���A�V�[��
//
//**********************************************
//--------------------------------------------------------
//		������
//--------------------------------------------------------

void SceneClear::Initialize()
{
	Framework& framework = Framework::Instance();
	ID3D11Device* device = framework.GetDevice();

	//�X�N���[���c���T�C�Y�ϐ�
	float screenWidth = static_cast<float>(framework.GetScreenWidth());
	float screenHeight = static_cast<float>(framework.GetScreenHeight());

	//�R���X�^���g�o�b�t�@
	CreateConstantBuffer(&ConstantBuffer, sizeof(ConstantBufferForPerFrame));
	CreateConstantBuffer(&ConstantBufferBloom, sizeof(ConstantBufferForBloom));

	//�J����������
	Camera& camera = Camera::Instance();

	//�J�����̐ݒ�
	camera.SetLookAt(
		DirectX::XMFLOAT3(0, 20, -10),  //�J�����̈ʒu
		DirectX::XMFLOAT3(0, 0, 1),      //�J�����̏œ_
		DirectX::XMFLOAT3(1, 0, 0)		 //�J�����̏���������߂�
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

	img = std::make_unique<Sprite>(Framework::Instance().GetDevice(), L"Data/Image/result2.png");
	imgButtonTitle = std::make_unique<Sprite>(Framework::Instance().GetDevice(), L"Data/Image/ResultTitle.png");
	imgButtonReplay = std::make_unique<Sprite>(Framework::Instance().GetDevice(), L"Data/Image/ResultReplay.png");
	FadeBlack = std::make_unique<Transition>();
	FadeBlack->init(device, L"Data/Image/unnamed.png", { 0,0 }, { 1920,1080 }, { 0,0 }, { 1920,1080 }, { 1.0f,1.0f,1.0f,1.0f });

	//�V�F�[�_�[
	NoLightShader = std::make_unique<StaticMeshShader>(device, "Shaders/NoLight_vs.cso", "Shaders/NoLight_ps.cso");
	ParticleShader = std::make_unique<PointSpriteShader>(device, "Shaders/PointSprite_vs.cso", "Shaders/PointSprite_gs.cso", "Shaders/PointSprite_ps.cso");
	BrightShader = std::make_unique<BrightSpriteShader>(device, "Shaders/bright_vs.cso", "Shaders/bright_ps.cso");
	BokehShader = std::make_unique<BokehSpriteShader>(device, "Shaders/bokeh_vs.cso", "Shaders/bokeh_ps.cso");

	//�p�[�e�B�N��
	Particles = std::make_unique<ParticleSystem>(device, 200000);

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

	ChangeSceneState = 1;

	state = 0;
	timer = 0;

	//�t�@�C���ǂݎ��
	std::string Filename = "Text/score.txt";
	std::ifstream ReadingFile(Filename, std::ios::in);
	for (int i = 0; i < 5; i++)
	{
		ReadingFile >> Result[i];
	}
	ReadingFile >> ThisResult;

	//����̃X�R�A�������N�ɓ�������F��ς���
	for (int i = 0; i < 5; i++)
	{
		if (ThisResult == Result[i])
		{
			R[i] = 1.0f;
			G[i] = 1.0f;
			B[i] = 0.0f;
		}
		else
		{
			R[i] = 1.0f;
			G[i] = 1.0f;
			B[i] = 1.0f;
		}
	}
}

//--------------------------------------------------------
//		�X�V
//--------------------------------------------------------

void SceneClear::Update(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	//�e�L�X�g�T�C�Y�ϓ�����
	float ax = gamePad.GetAxisLX();
	if (ax > 0.0f)
	{
		ButtonTitleColor = { 1.0f,1.0f,1.0f,1.0f };
		ButtonReplayColor = { 0.0f,1.0f,1.0f,1.0f };
		ChangeSceneState = 0;
	}
	else if (ax < 0.0f)
	{
		ButtonTitleColor = { 0.0f,1.0f,1.0f,1.0f };
		ButtonReplayColor = { 1.0f,1.0f,1.0f,1.0f };
		ChangeSceneState = 1;
	}


	//�J�����Ƀ^�[�Q�b�g��������̂�ݒ�
	cameraController->SetTarget(DirectX::XMFLOAT3(0.0f, 5.8f, -10.0));
	//�J�����X�V
	cameraController->Update(elapsedTime);

	Particles->hanabi({0.0f,0.0f,0.0f }, 200000);
	Particles->Update(elapsedTime);

	timer++;

	//���̃V�[���J��
	switch (state)
	{
	case 0:
		FadeBlack->fadeIn(0.03f);
		if (FadeBlack->GetAlpha() <= 0.1f) state++;
		break;
	case 1:
		FadeBlack->fadeIn(0.03f);
		if (KeyInput::KeyRelease() & KEY_START || gamePad.GetButtonDown() & GamePad::BTN_B)
		{
			state++;
		}
		break;
	case 2:
		FadeBlack->fadeOut(0.04f);
		if (FadeBlack->GetAlpha() >= 1.0f)
		{
			state++;
		}
		break;
	case 3:
		//�J�����R���g���[���[delete
		if (cameraController != nullptr)
		{
			delete cameraController;
			cameraController = nullptr;
		}
		if (ChangeSceneState == 1)
		{
			SceneManager::Instance().ChangeScene(new SceneTitle());
		}
		else
		{
		    SceneManager::Instance().ChangeScene(new SceneLoad(new SceneMain()));
		}
		return;
	}
}
//--------------------------------------------------------
//		�`��
//--------------------------------------------------------

void SceneClear::Render()
{
	//�u���[��
	RenderBloom();

	Framework& framework = Framework::Instance();
	float screen_width = static_cast<FLOAT>(framework.GetScreenWidth());
	float screen_height = static_cast<FLOAT>(framework.GetScreenHeight());

	Camera& camera = Camera::Instance();

	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = screen_width;
	viewport.Height = screen_height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	ID3D11DeviceContext* context = framework.GetImmediateContext();

	context->RSSetViewports(1, &viewport);

	FLOAT color[] = { 0.4f, 0.4f, 0.9f, 1.0f };
	ID3D11RenderTargetView* rtv = framework.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = framework.GetDepthStencilView();
	context->ClearRenderTargetView(rtv, color);
	context->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->OMSetRenderTargets(1, &rtv, dsv);
	context->OMSetDepthStencilState(framework.GetNoneDepthStencilState(), 1);

	ID3D11BlendState* BsAlpha = framework.GetBlendState(Blender::BS_ALPHA);
	ID3D11BlendState* BsAdd = framework.GetBlendState(Blender::BS_ADD);
	context->OMSetBlendState(BsAlpha, nullptr, 0xFFFFFFFF);
	
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

	FadeBlack->render(context);
}

void SceneClear::RenderBloom()
{
	// �J�����p�����[�^�ݒ�
	Camera& camera = Camera::Instance();

	Framework& framework = Framework::Instance();
	ID3D11DeviceContext* context = framework.GetImmediateContext();

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

		Particles->Render(context, ParticleShader.get(), view, projection, framework.GetBlendState(Blender::BS_ALPHA));
		//img->Render(context, 0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 0.0f, 669.0f, 373.0f);
		img->Render(context, 0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 0.0f, 1280.0f, 720.0f);
		Sprite* Font = framework.GetFont();
		const char* Meter = "M";
		//�����N
		for (int i = 0; i < 5; i++)
		{
			std::string result = to_string(Result[i]);
			result += Meter;
			Font->TextOut(context, result, 850+(i*30), 237 + 78 * (i + 1), 48, 48, R[i], G[i], B[i], 1.0f);
		}
		//����̃X�R�A
		std::string thisResult = to_string(ThisResult);
		thisResult += Meter;
		Font->TextOut(context, thisResult, 100, 300, 48, 48, 1.0f, 1.0f, 0.0f, 1.0f);

		//�l�N�X�g�V�[��
		imgButtonTitle->Render(context, 0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, ButtonTitleColor.x, ButtonTitleColor.y, ButtonTitleColor.z);
		imgButtonReplay->Render(context, 0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, ButtonReplayColor.x, ButtonReplayColor.y, ButtonReplayColor.z);
		/*const char* Title = "Title";
		const char* Replay = "Replay";
		Font->TextOut(context, Title, 50, 600, TextSize[0], TextSize[0]);
		Font->TextOut(context, Replay, 400, 600, TextSize[1], TextSize[1]);*/
		context->OMSetDepthStencilState(framework.GetDepthStencilState(), 1);

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
}
