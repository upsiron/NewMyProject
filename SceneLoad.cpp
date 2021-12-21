#include <thread>
#include "framework.h"
#include "scene.h" 

void SceneLoad::Initialize()
{  // スレッド開始 
   // ※この関数が抜けても LoadingThead()を実行させるため手放す  
    std::thread thread(LoadingThread, this);
    thread.detach();
} 

void SceneLoad::LoadingThread(SceneLoad* scene)
{  
    //Sleep(1000);
    scene->nextScene->Initialize();  
    scene->nextScene->initialized = true; 
} 

void SceneLoad::Update(float elapsedTime) {  
    // 次のシーンへ 
    if (nextScene->initialized)
    {
        SceneManager::Instance().ChangeScene( 
            nextScene.release());   
        return; 
    } 

 // ローディング演出処理  
    timer -= elapsedTime;
    if (timer < 0)  { 
        timer = 0.1f; 
        strcpy_s(message, "Now Loading");  
        counter = (counter + 1) % 4;  
        for (int i = 0; i < counter; ++i) { 
            strcat_s(message, ".");
            //Sleep(120);
        }
    }
}

void SceneLoad::Render() {  
    // DeviceContext の取得と画面クリア 
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

    // Now Loading…描画 
    Sprite* font = framework.GetFont();
    ID3D11BlendState* bs = framework.GetBlendState(Blender::BS_ALPHA);
    context->OMSetBlendState(bs, nullptr, 0xFFFFFFFF);
    font->TextOut(context, message, 220, 346, 64, 64,1,1,0,1); 
} 
    