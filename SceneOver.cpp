
#include "scene.h"
#include "KeyInput.h"
#include "framework.h"

//**********************************************
//
//		終了シーン
//
//**********************************************

//--------------------------------------------------------
//		初期化
//--------------------------------------------------------

void SceneOver::Initialize()
{
	img = std::make_unique<Sprite>(Framework::Instance().GetDevice(), L"Data/Over.png");
}

//--------------------------------------------------------
//		更新
//--------------------------------------------------------

void SceneOver::Update(float elapsedTime)
{
	if (KeyInput::KeyTrigger() & KEY_START)
	{
		SceneManager::Instance().ChangeScene(new SceneClear());
		return;
	}
}
//--------------------------------------------------------
//		描画
//--------------------------------------------------------

void SceneOver::Render()
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
}