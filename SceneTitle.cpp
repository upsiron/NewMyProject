
#include "scene.h"
#include "Camera.h"
#include "KeyInput.h"
//#include "framework.h"

//**********************************************
//
//		タイトルシーン
//
//**********************************************
//--------------------------------------------------------
//		初期化
//--------------------------------------------------------

void SceneTitle::Initialize()
{
	Framework& framework = Framework::Instance();
	ID3D11Device* device = framework.GetDevice();

	//スクリーン縦横サイズ変数
	float screenWidth = static_cast<float>(framework.GetScreenWidth());
	float screenHeight = static_cast<float>(framework.GetScreenHeight());

	//コンスタントバッファ
	CreateConstantBuffer(&ConstantBuffer, sizeof(ConstantBufferForPerFrame));
	CreateConstantBuffer(&ConstantBufferBloom, sizeof(ConstantBufferForBloom));

	//カメラ初期化
	Camera& camera = Camera::Instance();

	//カメラの設定
	camera.SetLookAt(
		DirectX::XMFLOAT3(0, 20, -10),  //カメラの位置
		DirectX::XMFLOAT3(0, 0, 1),      //カメラの焦点
		DirectX::XMFLOAT3(1, 0, 0)		 //カメラの上方向を決める
	);

	//カメラの視野角
	camera.SetPerspecticeFov(
		DirectX::XMConvertToRadians(45),
		screenWidth / screenHeight,
		0.1f,
		1000.0f
	);

	//カメラコントローラー初期化
	cameraController = new CameraController();

	imgTitle = std::make_unique<Sprite>(Framework::Instance().GetDevice(), L"Data//Image/title2.png");
	imgPushSpace = std::make_unique<Sprite>(Framework::Instance().GetDevice(), L"Data//Image/PushSpace.png");
	FadeBlack = std::make_unique<Transition>();
	FadeBlack->init(device, L"Data/Image/unnamed.png", { 0,0 }, { 1920,1080 }, { 0,0 }, { 1920,1080 }, { 1.0f,1.0f,1.0f,1.0f });

	//シェーダー
	NoLightShader = std::make_unique<StaticMeshShader>(device, "Shaders/NoLight_vs.cso", "Shaders/NoLight_ps.cso");
	ParticleShader = std::make_unique<PointSpriteShader>(device, "Shaders/PointSprite_vs.cso", "Shaders/PointSprite_gs.cso", "Shaders/PointSprite_ps.cso");
	BrightShader = std::make_unique<BrightSpriteShader>(device, "Shaders/bright_vs.cso", "Shaders/bright_ps.cso");
	BokehShader = std::make_unique<BokehSpriteShader>(device, "Shaders/bokeh_vs.cso", "Shaders/bokeh_ps.cso");

	//パーティクル
	Particles = std::make_unique<ParticleSystem>(device, 200000);

	//ブルーム初期化
	fullScreen = std::make_unique<FullScreenQuad>(device);
	fullScreenBright = std::make_unique<FullScreenQuad>(device);
	fullScreenBokeh = std::make_unique<FullScreenQuad>(device);
	offScreen = std::make_unique<FrameBuffer>(device, 1280 / SCREEN, 720 / SCREEN);
	bright = std::make_unique<FrameBuffer>(device, 1280 / SCREEN, 720 / SCREEN);
	bloom1 = std::make_unique<FrameBuffer>(device, 1280 / BLOOM1, 720 / BLOOM1);
	bloom2 = std::make_unique<FrameBuffer>(device, 1280 / BLOOM2, 720 / BLOOM2);
	bloom3 = std::make_unique<FrameBuffer>(device, 1280 / BLOOM3, 720 / BLOOM3);
	bloom4 = std::make_unique<FrameBuffer>(device, 1280 / BLOOM4, 720 / BLOOM4);

	state = 0;
	timer = 0;
}

//--------------------------------------------------------
//		更新
//--------------------------------------------------------

void SceneTitle::Update(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();


	//カメラにターゲットさせるものを設定
	cameraController->SetTarget(DirectX::XMFLOAT3(0.0f, 5.8f, -10.0));
	//カメラ更新
	cameraController->Update(elapsedTime);

	Particles->Star({ 0.0f,0.0f,0.0f }, {0.0f, 0.0f, -(rand() % 10001) * 0.002f - 0.001f}, 200000);
	Particles->Update(elapsedTime);

	timer++;
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
		SceneManager::Instance().ChangeScene(
			new SceneLoad(new SceneMain()));
		//SceneManager::Instance().ChangeScene(new SceneMain);
		return;
	}
}

//--------------------------------------------------------
//		描画
//--------------------------------------------------------

void SceneTitle::Render()
{
	//ブルーム
	RenderBloom();

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

	//ブルーム関係レンダリング
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
	//img->Render(context, 0, 0, 1280, 720, 0, 0, 640, 480);
	//img->Render(context, 0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 0.0f, 669.0f, 373.0f);

	FadeBlack->render(context);
}


void SceneTitle::RenderBloom()
{
	// カメラパラメータ設定
	Camera& camera = Camera::Instance();

	Framework& framework = Framework::Instance();
	ID3D11DeviceContext* context = framework.GetImmediateContext();

	//ラスタライザ―設定
	context->RSSetState(framework.GetRasterizerState0());
	//デプスステンシルステート設定
	context->OMSetDepthStencilState(framework.GetNoneDepthStencilState(), 1);
	// ビューポートの設定
	framework.SetViewPort(1280.0f / SCREEN, 720.0f / SCREEN);
	{
		DirectX::XMFLOAT4 lightDirection(0.0f, -1.0f, 0.0f, 0.0f);
		DirectX::XMFLOAT4X4 view = camera.GetView();
		DirectX::XMFLOAT4X4 projection = camera.GetProjection();
		DirectX::XMFLOAT4 materialColor(1.0f, 1.0f, 1.0f, 1.0f);
		{
			// コンスタントバッファ設定
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
		//オフスクリーン有効
		offScreen->Activate(context);
		offScreen->Clear(context);

		Particles->Render(context, ParticleShader.get(), view, projection, framework.GetBlendState(Blender::BS_ALPHA));
		//img->Render(context, 0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 0.0f, 669.0f, 373.0f);
		imgTitle->Render(context, 0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 0.0f, 1280.0f, 720.0f);
		if (timer / 32 % 2) imgPushSpace->Render(context, 0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 0.0f, 1280.0f, 720.0f);

		//オフスクリーン無効
		offScreen->Deactivate(context);
	}
	//Bright
	{
		//ラスタライザ―設定
		context->RSSetState(framework.GetRasterizerState0());
		//デプスステンシルステート設定
		context->OMSetDepthStencilState(framework.GetDepthStencilState(), 1);
		// ビューポートの設定
		framework.SetViewPort(1280.0f / SCREEN, 720.0f / SCREEN);
		{
			DirectX::XMFLOAT4 lightDirection(0.0f, -1.0f, 0.0f, 0.0f);
			DirectX::XMFLOAT4X4 view = camera.GetView();
			DirectX::XMFLOAT4X4 projection = camera.GetProjection();
			DirectX::XMFLOAT4 materialColor(1.0f, 1.0f, 1.0f, 1.0f);
			{
				// コンスタントバッファ設定
				ConstantBufferForBloom cb{};
				cb.threshold = threshold;
				context->UpdateSubresource(ConstantBufferBloom.Get(), 0, NULL, &cb, 0, 0);
				context->PSSetConstantBuffers(3, 1, ConstantBufferBloom.GetAddressOf());
			}

			//オフスクリーン有効
			bright->Activate(context);
			bright->Clear(context);

			framework.SetSamplerCramp(0);
			ID3D11ShaderResourceView* ShaderResourceViews[2]{
				offScreen->getShaderResourceView(0),
				offScreen->getShaderResourceView(1),
			};
			fullScreenBright->Blit(context, ShaderResourceViews, 0, 2, BrightShader.get());


			//オフスクリーン無効
			bright->Deactivate(context);
		}
	}
	//Bloom1
	{

		//ラスタライザ―設定
		context->RSSetState(framework.GetRasterizerState0());
		//デプスステンシルステート設定
		context->OMSetDepthStencilState(framework.GetDepthStencilState(), 1);
		// ビューポートの設定
		framework.SetViewPort(1280.0f / BLOOM1, 720.0f / BLOOM1);
		{
			DirectX::XMFLOAT4 lightDirection(0.0f, -1.0f, 0.0f, 0.0f);
			DirectX::XMFLOAT4X4 view = camera.GetView();
			DirectX::XMFLOAT4X4 projection = camera.GetProjection();
			DirectX::XMFLOAT4 materialColor(1.0f, 1.0f, 1.0f, 1.0f);
			{
				// コンスタントバッファ設定
				ConstantBufferForBloom cb{};
				cb.threshold = threshold;
				context->UpdateSubresource(ConstantBufferBloom.Get(), 0, NULL, &cb, 0, 0);
				context->PSSetConstantBuffers(3, 1, ConstantBufferBloom.GetAddressOf());
			}

			//オフスクリーン有効
			bloom1->Activate(context);
			bloom1->Clear(context);

			framework.SetSamplerCramp(0);
			ID3D11ShaderResourceView* ShaderResourceViews[2]{
				bright->getShaderResourceView(0),
				bright->getShaderResourceView(1),
			};
			fullScreenBright->Blit(context, ShaderResourceViews, 0, 2, BrightShader.get());

			//オフスクリーン無効
			bloom1->Deactivate(context);
		}
	}
	//Bloom2
	{
		//ラスタライザ―設定
		context->RSSetState(framework.GetRasterizerState0());
		//デプスステンシルステート設定
		context->OMSetDepthStencilState(framework.GetDepthStencilState(), 1);
		// ビューポートの設定
		framework.SetViewPort(1280.0f / BLOOM2, 720.0f / BLOOM2);
		{
			DirectX::XMFLOAT4 lightDirection(0.0f, -1.0f, 0.0f, 0.0f);
			DirectX::XMFLOAT4X4 view = camera.GetView();
			DirectX::XMFLOAT4X4 projection = camera.GetProjection();
			DirectX::XMFLOAT4 materialColor(1.0f, 1.0f, 1.0f, 1.0f);
			{
				// コンスタントバッファ設定
				ConstantBufferForBloom cb{};
				cb.threshold = threshold;
				context->UpdateSubresource(ConstantBufferBloom.Get(), 0, NULL, &cb, 0, 0);
				context->PSSetConstantBuffers(3, 1, ConstantBufferBloom.GetAddressOf());
			}

			//オフスクリーン有効
			bloom2->Activate(context);
			bloom2->Clear(context);

			framework.SetSamplerCramp(0);
			ID3D11ShaderResourceView* ShaderResourceViews[2]{
				bloom1->getShaderResourceView(0),
				bloom1->getShaderResourceView(1),
			};
			fullScreenBright->Blit(context, ShaderResourceViews, 0, 2, BrightShader.get());

			//オフスクリーン無効
			bloom2->Deactivate(context);
		}
	}
	//Bloom3
	{
		//ラスタライザ―設定
		context->RSSetState(framework.GetRasterizerState0());
		//デプスステンシルステート設定
		context->OMSetDepthStencilState(framework.GetDepthStencilState(), 1);
		// ビューポートの設定
		framework.SetViewPort(1280.0f / BLOOM3, 720.0f / BLOOM3);
		{
			DirectX::XMFLOAT4 lightDirection(0.0f, -1.0f, 0.0f, 0.0f);
			DirectX::XMFLOAT4X4 view = camera.GetView();
			DirectX::XMFLOAT4X4 projection = camera.GetProjection();
			DirectX::XMFLOAT4 materialColor(1.0f, 1.0f, 1.0f, 1.0f);
			{
				// コンスタントバッファ設定
				ConstantBufferForBloom cb{};
				cb.threshold = threshold;
				context->UpdateSubresource(ConstantBufferBloom.Get(), 0, NULL, &cb, 0, 0);
				context->PSSetConstantBuffers(3, 1, ConstantBufferBloom.GetAddressOf());
			}

			//オフスクリーン有効
			bloom3->Activate(context);
			bloom3->Clear(context);

			framework.SetSamplerCramp(0);
			ID3D11ShaderResourceView* ShaderResourceViews[2]{
				bloom2->getShaderResourceView(0),
				bloom2->getShaderResourceView(1),
			};
			fullScreenBright->Blit(context, ShaderResourceViews, 0, 2, BrightShader.get());

			//オフスクリーン無効
			bloom3->Deactivate(context);
		}
	}
	//Bloom4
	{

		//ラスタライザ―設定
		context->RSSetState(framework.GetRasterizerState0());
		//デプスステンシルステート設定
		context->OMSetDepthStencilState(framework.GetDepthStencilState(), 1);
		// ビューポートの設定
		framework.SetViewPort(1280.0f / BLOOM4, 720.0f / BLOOM4);
		{
			DirectX::XMFLOAT4 lightDirection(0.0f, -1.0f, 0.0f, 0.0f);
			DirectX::XMFLOAT4X4 view = camera.GetView();
			DirectX::XMFLOAT4X4 projection = camera.GetProjection();
			DirectX::XMFLOAT4 materialColor(1.0f, 1.0f, 1.0f, 1.0f);
			{
				// コンスタントバッファ設定
				ConstantBufferForBloom cb{};
				cb.threshold = threshold;
				context->UpdateSubresource(ConstantBufferBloom.Get(), 0, NULL, &cb, 0, 0);
				context->PSSetConstantBuffers(3, 1, ConstantBufferBloom.GetAddressOf());
			}


			//オフスクリーン有効
			bloom4->Activate(context);
			bloom4->Clear(context);

			framework.SetSamplerCramp(0);
			ID3D11ShaderResourceView* ShaderResourceViews[2]{
				bloom3->getShaderResourceView(0),
				bloom3->getShaderResourceView(1),
			};
			fullScreenBright->Blit(context, ShaderResourceViews, 0, 2, BrightShader.get());

			//オフスクリーン無効
			bloom4->Deactivate(context);
		}
	}
}