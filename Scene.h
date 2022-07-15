#pragma once


#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#include <memory>
#include "sprite.h"

#include "transition.h"
#include "framework.h"
#include "texture.h"
#include "player.h"
#include "enemyBlueSlime.h"
#include "ObstacleBlock.h"
#include "Coin.h"
#include "stage.h"
#include "stageBase.h"
#include "stageTile.h"
#include "CameraController.h"
#include "GeometricPrimitive.h"
#include "Input/Input.h"
#include "ParticleSystem.h"
#include "Shader.h"
#include "FullScreenQuad.h"
#include "FrameBuffer.h"


class Scene
{
private:
	// SceneLoad と SceneManager のみで設定できるフラグを定義 
	friend class SceneLoad;
	friend class SceneManager;
	bool initialized = false;
protected:
	int timer = 0;
	int state = 0;


	Microsoft::WRL::ComPtr<ID3D11Buffer> ConstantBuffer;
	//コンスタントバッファ作成
	bool CreateConstantBuffer(ID3D11Buffer** ppCB, u_int size)
	{
		Framework& framework = Framework::Instance();
		ID3D11Device* device = framework.GetDevice();
		// 定数バッファ生成
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
		bd.ByteWidth = size;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;

		HRESULT hr = device->CreateBuffer(&bd, NULL, ppCB);
		assert(SUCCEEDED(hr));

		//delete device;
		return true;
	}

public:
	Scene(){}
	virtual ~Scene() 
	{
		//ConstantBufferBloom->Release();
	}
	virtual void Initialize() = 0;
	virtual void Finalize() = 0;
	virtual void Update(float elapsedTime) = 0;
	virtual void Render()=0;

	//コンスタントバッファ構造体
	struct ConstantBufferForPerFrame
	{
		DirectX::XMFLOAT4	LightColor;		//ライトの色
		//DirectX::XMFLOAT4	LightDir;		//ライトの方向
		DirectX::XMFLOAT4	AmbientColor;	//環境光
		DirectX::XMFLOAT4	EyePos;			//カメラ
	};

	//コンスタントバッファ構造体(ブルーム用)
	struct ConstantBufferForBloom
	{
		float threshold;
		float dummy1;
		float dummy2;
		float dummy3;
	};
	ComPtr<ID3D11Buffer> ConstantBufferBloom;


	//imGui
	//virtual void imGuiUpdate();
	void imGuiRender();

	int Result[5] = { 0 };
	int ThisResult = 0;
};

class SceneLoad :public Scene {
private:
	std::unique_ptr<Sprite> img;
	std::unique_ptr<Sprite> imgDot;
	std::unique_ptr<Scene> nextScene;
	std::unique_ptr<Transition> FadeBlack;
	// Now Loading...アニメーション用 
	float timer = 0.0f;
	int counter = 0;
	//char message[32] = { 0 };

	//
	const DirectX::XMFLOAT4 ambientColor = { 0.564f,0.564f,0.564f,1 };
	DirectX::XMFLOAT4 DirLightColor = { 1, 1, 1, 1 };

	//カメラコントローラー実体
	CameraController* cameraController = nullptr;

	//シェーダー
	std::unique_ptr<Shader> ParticleShader{};
	std::unique_ptr<Shader> NoLightShader{};
	std::unique_ptr<Shader> BrightShader{};
	std::unique_ptr<Shader> BokehShader{};
	Microsoft::WRL::ComPtr<ID3D11PixelShader> PixelShaders[8];

	//パーティクル
	std::unique_ptr<ParticleSystem> Particles{};

	//ブルーム関連
	float threshold = 0.170f; //輝度閾値

	//スクリーンぼかしのために割る数値
	const float SCREEN = 1;
	const float BLOOM1 = 2;
	const float BLOOM2 = 4;
	const float BLOOM3 = 16;
	const float BLOOM4 = 64;

	//オフスクリーンで描画用
	std::unique_ptr<FullScreenQuad> fullScreen{};         //ノーマル
	std::unique_ptr<FullScreenQuad> fullScreenBright{};   //ブライトシェーダー用
	std::unique_ptr<FullScreenQuad> fullScreenBokeh{};    //ボケシェーダー用
	std::unique_ptr<FrameBuffer> offScreen{};
	std::unique_ptr<FrameBuffer> bright{};
	std::unique_ptr<FrameBuffer> bloom1{};
	std::unique_ptr<FrameBuffer> bloom2{};
	std::unique_ptr<FrameBuffer> bloom3{};
	std::unique_ptr<FrameBuffer> bloom4{};
public:
	SceneLoad(Scene* nextScene) {
		// ローディング後に遷移するシーンを保持
		this->nextScene.reset(nextScene);
	}
	virtual ~SceneLoad() 
	{
		delete cameraController;
		cameraController = nullptr;
	}
	void Initialize() override;
	void Finalize()  override {};
	void Update(float elapsedTime) override;
	void Render() override;
	//ブルームレンダー関数
	void RenderBloom();
private:
	static void LoadingThread(SceneLoad* scene);
};

class SceneMain :public Scene
{
public:
	//ステージ・障害物のMAX
#define StageMax 10
	//static const int StageMax = 5;
	static const int StageTileMax = 9;
	static const int ObstacleMax = 6;
	static const int CoinMax = 3;

	//動く障害物の動く値の変数
	float ObstacleMove[2] = { 0.0f };
private:
	//フレームワーク関連
	Framework& framework = Framework::Instance();
	ID3D11Device* device = framework.GetDevice();
	ID3D11DeviceContext* context = framework.GetImmediateContext();
	HWND hwnd = framework.GetWindowHandle();

	//実体
	Player* player = nullptr;
	//EnemyBlueSlime* enemy = {nullptr};
	ObstacleBlock* obstacle[StageMax][ObstacleMax] = { nullptr };
	Coin* coin[CoinMax] = { nullptr };
	StageBase* stageBase[StageMax] = { nullptr };
	StageTile* stageTile[StageMax][StageTileMax] = { nullptr };
	CameraController* cameraController = nullptr;

	std::shared_ptr<SkinnedMesh> skyMesh;
	std::unique_ptr<SkinnedObject> skyObj;

	//シェーダー
	std::unique_ptr<Shader> SkinnedShader{};
	std::unique_ptr<Shader> ParticleShader{};
	std::unique_ptr<Shader> NoLightShader{};
	std::unique_ptr<Shader> BrightShader{};
	std::unique_ptr<Shader> BokehShader{};
	Microsoft::WRL::ComPtr<ID3D11PixelShader> PixelShaders[8];

	//パーティクル
	std::unique_ptr<ParticleSystem> particles{};

	//ブルーム関連
	float threshold = 0.17f; //輝度閾値

	//スクリーンぼかしのために割る数値
	const float SCREEN = 1;
	const float BLOOM1 = 2;
	const float BLOOM2 = 4;
	const float BLOOM3 = 16;
	const float BLOOM4 = 64;

	//オフスクリーンで描画用
	std::unique_ptr<FullScreenQuad> fullScreen{};         //ノーマル
	std::unique_ptr<FullScreenQuad> fullScreenBright{};   //ブライトシェーダー用
	std::unique_ptr<FullScreenQuad> fullScreenBokeh{};    //ボケシェーダー用
	std::unique_ptr<FrameBuffer> offScreen{};
	std::unique_ptr<FrameBuffer> bright{};
	std::unique_ptr<FrameBuffer> bloom1{};
	std::unique_ptr<FrameBuffer> bloom2{};
	std::unique_ptr<FrameBuffer> bloom3{};
	std::unique_ptr<FrameBuffer> bloom4{};

	//std::unique_ptr<Sprite> img;
	//フェードインアウト用
	std::unique_ptr<Transition> FadeBlack;

	//テクスチャ用
	std::shared_ptr<Texture> tex;

	//BGM用
	std::unique_ptr<SoundSource> BGM;

	// フェードインアウトフラグ
	bool fadeInFlg = false;
	bool fadeOutFlg = false;
public:
	SceneMain() {}
	virtual ~SceneMain() 
	{
		delete cameraController;
		cameraController = nullptr;
	};
	void Initialize() override;
	void Finalize() override;
	void Update(float elapsedTime) override;
	void Render() override;

	//ステージの更新
	void StageUpdate();

	//ブルームレンダー関数
	void RenderBloom();

	//リザルトランキング計算関数
	void ResultMeter();

	//ライトセット
	static void SetDirLight(DirectX::XMFLOAT3 dir, DirectX::XMFLOAT3 color);

	// ワールド座標(3D)からスクリーン座標(2D)へ変換
	void WorldToScreen(DirectX::XMFLOAT3* screenPosition, const DirectX::XMFLOAT3& worldPosition);

	// スクリーン座標(2D)からワールド座標(3D)へ変換
	void ScreenToWorld(DirectX::XMFLOAT3* worldPosition, const DirectX::XMFLOAT3& screenPosition);

	//imGui
	void imGuiUpdate();
};



class SceneTitle :public Scene
{
private:
	std::unique_ptr<Sprite> imgTitle;
	std::unique_ptr<Sprite> imgPushSpace;
	std::unique_ptr<Transition> FadeBlack;

	const DirectX::XMFLOAT4 ambientColor = { 0.564f,0.564f,0.564f,1 };
	DirectX::XMFLOAT4 DirLightColor = { 1, 1, 1, 1 };

	//カメラコントローラー実体
	CameraController* cameraController = nullptr;

	//シェーダー
	std::unique_ptr<Shader> ParticleShader{};
	std::unique_ptr<Shader> NoLightShader{};
	std::unique_ptr<Shader> BrightShader{};
	std::unique_ptr<Shader> BokehShader{};
	Microsoft::WRL::ComPtr<ID3D11PixelShader> PixelShaders[8];

	//パーティクル
	std::unique_ptr<ParticleSystem> Particles{};

	//ブルーム関連
	float threshold = 0.470f; //輝度閾値

	//スクリーンぼかしのために割る数値
	const float SCREEN = 1;
	const float BLOOM1 = 2;
	const float BLOOM2 = 4;
	const float BLOOM3 = 16;
	const float BLOOM4 = 64;

	//オフスクリーンで描画用
	std::unique_ptr<FullScreenQuad> fullScreen{};         //ノーマル
	std::unique_ptr<FullScreenQuad> fullScreenBright{};   //ブライトシェーダー用
	std::unique_ptr<FullScreenQuad> fullScreenBokeh{};    //ボケシェーダー用
	std::unique_ptr<FrameBuffer> offScreen{};
	std::unique_ptr<FrameBuffer> bright{};
	std::unique_ptr<FrameBuffer> bloom1{};
	std::unique_ptr<FrameBuffer> bloom2{};
	std::unique_ptr<FrameBuffer> bloom3{};
	std::unique_ptr<FrameBuffer> bloom4{};

public:
	SceneTitle() {}
	virtual ~SceneTitle() 
	{
		delete cameraController;
		cameraController = nullptr;
	}
	void Initialize() override;
	void Finalize()  override {};
	void Update(float elapsedTime) override;
	void Render() override;
	//ブルームレンダー関数
	void RenderBloom();

};

class SceneClear :public Scene
{
private:
	std::unique_ptr<Sprite> img;
	std::unique_ptr<Sprite> imgButtonTitle;
	std::unique_ptr<Sprite> imgButtonReplay;
	std::unique_ptr<Transition> FadeBlack;
	DirectX::XMFLOAT4 ButtonTitleColor = { 0.0f, 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT4 ButtonReplayColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	int ChangeSceneState = 0;
	float R[5] = { 1.0f };
	float G[5] = { 1.0f };
	float B[5] = { 1.0f };
	const DirectX::XMFLOAT4 ambientColor = { 0.564f,0.564f,0.564f,1 };
	DirectX::XMFLOAT4 DirLightColor = { 1, 1, 1, 1 };

	//カメラコントローラー実体
	CameraController* cameraController = nullptr;

	//シェーダー
	std::unique_ptr<Shader> ParticleShader{};
	std::unique_ptr<Shader> NoLightShader{};
	std::unique_ptr<Shader> BrightShader{};
	std::unique_ptr<Shader> BokehShader{};
	Microsoft::WRL::ComPtr<ID3D11PixelShader> PixelShaders[8];

	//パーティクル
	std::unique_ptr<ParticleSystem> Particles{};

	//ブルーム関連
	float threshold = 0.470f; //輝度閾値

	//スクリーンぼかしのために割る数値
	const float SCREEN = 1;
	const float BLOOM1 = 2;
	const float BLOOM2 = 4;
	const float BLOOM3 = 16;
	const float BLOOM4 = 64;

	//オフスクリーンで描画用
	std::unique_ptr<FullScreenQuad> fullScreen{};         //ノーマル
	std::unique_ptr<FullScreenQuad> fullScreenBright{};   //ブライトシェーダー用
	std::unique_ptr<FullScreenQuad> fullScreenBokeh{};    //ボケシェーダー用
	std::unique_ptr<FrameBuffer> offScreen{};
	std::unique_ptr<FrameBuffer> bright{};
	std::unique_ptr<FrameBuffer> bloom1{};
	std::unique_ptr<FrameBuffer> bloom2{};
	std::unique_ptr<FrameBuffer> bloom3{};
	std::unique_ptr<FrameBuffer> bloom4{};


public:
	static SceneClear& Instance()
	{
		static SceneClear inst;
		return inst;
	}

	SceneClear() {}
	virtual ~SceneClear() 
	{
		free(cameraController);
		/*delete cameraController;
		cameraController = nullptr;*/
	}
	void Initialize() override;
	void Finalize()  override {};
	void Update(float elapsedTime) override;
	void Render() override;
	//ブルームレンダー関数
	void RenderBloom();
};

class SceneOver :public Scene
{
private:
	std::unique_ptr<Sprite> img;
	std::unique_ptr<Transition> FadeBlack;
public:
	SceneOver() {}
	virtual ~SceneOver() {}
	void Initialize() override;
	void Finalize()  override {};
	void Update(float elapsedTime) override;
	void Render() override;

};

//----------------------------------------------------
//		SceneManager(管理)クラス
//----------------------------------------------------
class SceneManager
{
private:
	std::unique_ptr<Scene> currentScene;

private:
	SceneManager() {}
	~SceneManager() {}
	
public:
	static SceneManager& Instance()
	{
		static SceneManager inst;
		return inst;
	}

	void Finalize();
	void Update(float elapsedTime);
	void Render();
	void ChangeScene(Scene* new_scene);

};
