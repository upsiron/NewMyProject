#include <thread>
//#include "framework.h"
#include "scene.h" 
#include "Camera.h" 

void SceneLoad::Initialize()
{  // �X���b�h�J�n 
   // �����̊֐��������Ă� LoadingThead()�����s�����邽�ߎ����  
    std::thread thread(LoadingThread, this);
    thread.detach();

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

	//load�摜������
	img = std::make_unique<Sprite>(Framework::Instance().GetDevice(), L"Data/Image/Load.png");
	imgDot = std::make_unique<Sprite>(Framework::Instance().GetDevice(), L"Data/Image/Dot.png");

    //�V�F�[�_�[
    NoLightShader = std::make_unique<StaticMeshShader>(device, "Shaders/NoLight_vs.cso", "Shaders/NoLight_ps.cso");
    ParticleShader = std::make_unique<PointSpriteShader>(device, "Shaders/PointSprite_vs.cso", "Shaders/PointSprite_gs.cso", "Shaders/PointSprite_ps.cso");
    BrightShader = std::make_unique<BrightSpriteShader>(device, "Shaders/bright_vs.cso", "Shaders/bright_ps.cso");
    BokehShader = std::make_unique<BokehSpriteShader>(device, "Shaders/bokeh_vs.cso", "Shaders/bokeh_ps.cso");

    //�p�[�e�B�N��
    Particles = std::make_unique<ParticleSystem>(device, 200000);

    //�u���[��������
    FullScreen = std::make_unique<FullScreenQuad>(device);
    FullScreenBright = std::make_unique<FullScreenQuad>(device);
    FullScreenBokeh = std::make_unique<FullScreenQuad>(device);
    OffScreen = std::make_unique<FrameBuffer>(device, 1280 / SCREEN, 720 / SCREEN);
    Bright = std::make_unique<FrameBuffer>(device, 1280 / SCREEN, 720 / SCREEN);
    Bloom1 = std::make_unique<FrameBuffer>(device, 1280 / BLOOM1, 720 / BLOOM1);
    Bloom2 = std::make_unique<FrameBuffer>(device, 1280 / BLOOM2, 720 / BLOOM2);
    Bloom3 = std::make_unique<FrameBuffer>(device, 1280 / BLOOM3, 720 / BLOOM3);
    Bloom4 = std::make_unique<FrameBuffer>(device, 1280 / BLOOM4, 720 / BLOOM4);
} 

void SceneLoad::LoadingThread(SceneLoad* scene)
{  
    //Sleep(1000);
    scene->nextScene->Initialize();  
    scene->nextScene->initialized = true; 
} 

void SceneLoad::Update(float elapsedTime) {

	//�J�����Ƀ^�[�Q�b�g��������̂�ݒ�
	cameraController->SetTarget(DirectX::XMFLOAT3(0.0f, 5.8f, -10.0));
	//�J�����X�V
	cameraController->Update(elapsedTime);
	//�p�[�e�B�N��
	Particles->Star({ 0.0f,0.0f,0.0f }, {0.0f, 0.0f, 0.0f}, 20000);
    Particles->Update(elapsedTime);


    // ���̃V�[���� 
    if (nextScene->initialized)
    {
        SceneManager::Instance().ChangeScene( 
            nextScene.release());   
        return; 
    } 

    // ���[�f�B���O���o����  
    timer -= elapsedTime;
    if (timer < 0)  { 
        timer = 0.1f; 
        //strcpy_s(message, "Now Loading");  
        counter = (counter + 1) % 4;  
        //for (int i = 0; i < counter; ++i) { 
        //    strcat_s(message, ".");
        //    //Sleep(120);
        //}
    }
}

void SceneLoad::Render() {  

    //�u���[��
    RenderBloom();

    // DeviceContext �̎擾�Ɖ�ʃN���A 
    Framework& framework = Framework::Instance();

    float screenWidth = static_cast<FLOAT>(framework.GetScreenWidth());
    float screenHeight = static_cast<FLOAT>(framework.GetScreenHeight());

    D3D11_VIEWPORT viewport = {};
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = screenWidth;
    viewport.Height = screenHeight;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    ID3D11DeviceContext* context = framework.GetImmediateContext();
    context->RSSetViewports(1, &viewport);

    FLOAT color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
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
        OffScreen->getShaderResourceView(0),
        OffScreen->getShaderResourceView(1),
    };
    FullScreen->Blit(context, ShaderResourceViews, 0, 2, PixelShaders[1].Get());

    context->OMSetBlendState(BsAdd, nullptr, 0xFFFFFFFF);

    ID3D11ShaderResourceView* BokehShaderResourceViews1[2]{
        Bloom1->getShaderResourceView(0),
        Bloom1->getShaderResourceView(1),
    };
    FullScreenBokeh->Blit(context, BokehShaderResourceViews1, 0, 2, BokehShader.get());

    ID3D11ShaderResourceView* BokehShaderResourceViews2[2]{
        Bloom2->getShaderResourceView(0),
        Bloom2->getShaderResourceView(1),
    };
    FullScreenBokeh->Blit(context, BokehShaderResourceViews2, 0, 2, BokehShader.get());

    ID3D11ShaderResourceView* BokehShaderResourceViews3[2]{
        Bloom3->getShaderResourceView(0),
        Bloom3->getShaderResourceView(1),
    };
    FullScreenBokeh->Blit(context, BokehShaderResourceViews3, 0, 2, BokehShader.get());

    ID3D11ShaderResourceView* BokehShaderResourceViews4[2]{
        Bloom4->getShaderResourceView(0),
        Bloom4->getShaderResourceView(1),
    };
    FullScreenBokeh->Blit(context, BokehShaderResourceViews4, 0, 2, BokehShader.get());

    context->OMSetBlendState(BsAlpha, nullptr, 0xFFFFFFFF);
}

void SceneLoad::RenderBloom()
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
		OffScreen->Activate(context);
		OffScreen->Clear(context);

		Particles->Render(context, ParticleShader.get(), view, projection, framework.GetBlendState(Blender::BS_ALPHA));
		// Now Loading�c�`�� 
		img->Render(context, 0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 0.0f, 1280.0f, 720.0f);

		for (int i = 0; i < counter; ++i) {
			imgDot->Render(context, 0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 0.0f, 1280.0f-(25*i), 720.0f);
		}
		
		//Sprite* font = framework.GetFont();
		//ID3D11BlendState* bs = framework.GetBlendState(Blender::BS_ALPHA);
		//context->OMSetBlendState(bs, nullptr, 0xFFFFFFFF);
		////font->TextOut(context, message, 220, 346, 64, 64, 0, 1, 1, 1);
		//font->TextOut(context, message, 620, 620, 48, 48, 0, 1, 1, 1);

		//�I�t�X�N���[������
		OffScreen->Deactivate(context);
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
			Bright->Activate(context);
			Bright->Clear(context);

			framework.SetSamplerCramp(0);
			ID3D11ShaderResourceView* ShaderResourceViews[2]{
				OffScreen->getShaderResourceView(0),
				OffScreen->getShaderResourceView(1),
			};
			FullScreenBright->Blit(context, ShaderResourceViews, 0, 2, BrightShader.get());


			//�I�t�X�N���[������
			Bright->Deactivate(context);
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
			Bloom1->Activate(context);
			Bloom1->Clear(context);

			framework.SetSamplerCramp(0);
			ID3D11ShaderResourceView* ShaderResourceViews[2]{
				Bright->getShaderResourceView(0),
				Bright->getShaderResourceView(1),
			};
			FullScreenBright->Blit(context, ShaderResourceViews, 0, 2, BrightShader.get());

			//�I�t�X�N���[������
			Bloom1->Deactivate(context);
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
			Bloom2->Activate(context);
			Bloom2->Clear(context);

			framework.SetSamplerCramp(0);
			ID3D11ShaderResourceView* ShaderResourceViews[2]{
				Bloom1->getShaderResourceView(0),
				Bloom1->getShaderResourceView(1),
			};
			FullScreenBright->Blit(context, ShaderResourceViews, 0, 2, BrightShader.get());

			//�I�t�X�N���[������
			Bloom2->Deactivate(context);
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
			Bloom3->Activate(context);
			Bloom3->Clear(context);

			framework.SetSamplerCramp(0);
			ID3D11ShaderResourceView* ShaderResourceViews[2]{
				Bloom2->getShaderResourceView(0),
				Bloom2->getShaderResourceView(1),
			};
			FullScreenBright->Blit(context, ShaderResourceViews, 0, 2, BrightShader.get());

			//�I�t�X�N���[������
			Bloom3->Deactivate(context);
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
			Bloom4->Activate(context);
			Bloom4->Clear(context);

			framework.SetSamplerCramp(0);
			ID3D11ShaderResourceView* ShaderResourceViews[2]{
				Bloom3->getShaderResourceView(0),
				Bloom3->getShaderResourceView(1),
			};
			FullScreenBright->Blit(context, ShaderResourceViews, 0, 2, BrightShader.get());

			//�I�t�X�N���[������
			Bloom4->Deactivate(context);
		}
	}
}

    