
#include "scene.h"
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

	img = std::make_unique<Sprite>(Framework::Instance().GetDevice(), L"Data/Title.png");
	FadeBlack = std::make_unique<Transition>();
	FadeBlack->init(device, L"Data/unnamed.png", { 0,0 }, { 1920,1080 }, { 0,0 }, { 1920,1080 }, { 1.0f,1.0f,1.0f,1.0f });

	state = 0;
	timer = 0;
}

//--------------------------------------------------------
//		更新
//--------------------------------------------------------

void SceneTitle::Update(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();

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
		return;
	}
}

//--------------------------------------------------------
//		描画
//--------------------------------------------------------

void SceneTitle::Render()
{
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

	ID3D11BlendState* bs = framework.GetBlendState(Blender::BS_ALPHA);
	context->OMSetBlendState(bs, nullptr, 0xFFFFFFFF);

	img->Render(context, 0, 0, 1280, 720, 0, 0, 640, 480);
	FadeBlack->render(context);
}
