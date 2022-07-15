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
	// SceneLoad �� SceneManager �݂̂Őݒ�ł���t���O���` 
	friend class SceneLoad;
	friend class SceneManager;
	bool initialized = false;
protected:
	int timer = 0;
	int state = 0;


	Microsoft::WRL::ComPtr<ID3D11Buffer> ConstantBuffer;
	//�R���X�^���g�o�b�t�@�쐬
	bool CreateConstantBuffer(ID3D11Buffer** ppCB, u_int size)
	{
		Framework& framework = Framework::Instance();
		ID3D11Device* device = framework.GetDevice();
		// �萔�o�b�t�@����
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

	//�R���X�^���g�o�b�t�@�\����
	struct ConstantBufferForPerFrame
	{
		DirectX::XMFLOAT4	LightColor;		//���C�g�̐F
		//DirectX::XMFLOAT4	LightDir;		//���C�g�̕���
		DirectX::XMFLOAT4	AmbientColor;	//����
		DirectX::XMFLOAT4	EyePos;			//�J����
	};

	//�R���X�^���g�o�b�t�@�\����(�u���[���p)
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
	// Now Loading...�A�j���[�V�����p 
	float timer = 0.0f;
	int counter = 0;
	//char message[32] = { 0 };

	//
	const DirectX::XMFLOAT4 ambientColor = { 0.564f,0.564f,0.564f,1 };
	DirectX::XMFLOAT4 DirLightColor = { 1, 1, 1, 1 };

	//�J�����R���g���[���[����
	CameraController* cameraController = nullptr;

	//�V�F�[�_�[
	std::unique_ptr<Shader> ParticleShader{};
	std::unique_ptr<Shader> NoLightShader{};
	std::unique_ptr<Shader> BrightShader{};
	std::unique_ptr<Shader> BokehShader{};
	Microsoft::WRL::ComPtr<ID3D11PixelShader> PixelShaders[8];

	//�p�[�e�B�N��
	std::unique_ptr<ParticleSystem> Particles{};

	//�u���[���֘A
	float threshold = 0.170f; //�P�x臒l

	//�X�N���[���ڂ����̂��߂Ɋ��鐔�l
	const float SCREEN = 1;
	const float BLOOM1 = 2;
	const float BLOOM2 = 4;
	const float BLOOM3 = 16;
	const float BLOOM4 = 64;

	//�I�t�X�N���[���ŕ`��p
	std::unique_ptr<FullScreenQuad> fullScreen{};         //�m�[�}��
	std::unique_ptr<FullScreenQuad> fullScreenBright{};   //�u���C�g�V�F�[�_�[�p
	std::unique_ptr<FullScreenQuad> fullScreenBokeh{};    //�{�P�V�F�[�_�[�p
	std::unique_ptr<FrameBuffer> offScreen{};
	std::unique_ptr<FrameBuffer> bright{};
	std::unique_ptr<FrameBuffer> bloom1{};
	std::unique_ptr<FrameBuffer> bloom2{};
	std::unique_ptr<FrameBuffer> bloom3{};
	std::unique_ptr<FrameBuffer> bloom4{};
public:
	SceneLoad(Scene* nextScene) {
		// ���[�f�B���O��ɑJ�ڂ���V�[����ێ�
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
	//�u���[�������_�[�֐�
	void RenderBloom();
private:
	static void LoadingThread(SceneLoad* scene);
};

class SceneMain :public Scene
{
public:
	//�X�e�[�W�E��Q����MAX
#define StageMax 10
	//static const int StageMax = 5;
	static const int StageTileMax = 9;
	static const int ObstacleMax = 6;
	static const int CoinMax = 3;

	//������Q���̓����l�̕ϐ�
	float ObstacleMove[2] = { 0.0f };
private:
	//�t���[�����[�N�֘A
	Framework& framework = Framework::Instance();
	ID3D11Device* device = framework.GetDevice();
	ID3D11DeviceContext* context = framework.GetImmediateContext();
	HWND hwnd = framework.GetWindowHandle();

	//����
	Player* player = nullptr;
	//EnemyBlueSlime* enemy = {nullptr};
	ObstacleBlock* obstacle[StageMax][ObstacleMax] = { nullptr };
	Coin* coin[CoinMax] = { nullptr };
	StageBase* stageBase[StageMax] = { nullptr };
	StageTile* stageTile[StageMax][StageTileMax] = { nullptr };
	CameraController* cameraController = nullptr;

	std::shared_ptr<SkinnedMesh> skyMesh;
	std::unique_ptr<SkinnedObject> skyObj;

	//�V�F�[�_�[
	std::unique_ptr<Shader> SkinnedShader{};
	std::unique_ptr<Shader> ParticleShader{};
	std::unique_ptr<Shader> NoLightShader{};
	std::unique_ptr<Shader> BrightShader{};
	std::unique_ptr<Shader> BokehShader{};
	Microsoft::WRL::ComPtr<ID3D11PixelShader> PixelShaders[8];

	//�p�[�e�B�N��
	std::unique_ptr<ParticleSystem> particles{};

	//�u���[���֘A
	float threshold = 0.17f; //�P�x臒l

	//�X�N���[���ڂ����̂��߂Ɋ��鐔�l
	const float SCREEN = 1;
	const float BLOOM1 = 2;
	const float BLOOM2 = 4;
	const float BLOOM3 = 16;
	const float BLOOM4 = 64;

	//�I�t�X�N���[���ŕ`��p
	std::unique_ptr<FullScreenQuad> fullScreen{};         //�m�[�}��
	std::unique_ptr<FullScreenQuad> fullScreenBright{};   //�u���C�g�V�F�[�_�[�p
	std::unique_ptr<FullScreenQuad> fullScreenBokeh{};    //�{�P�V�F�[�_�[�p
	std::unique_ptr<FrameBuffer> offScreen{};
	std::unique_ptr<FrameBuffer> bright{};
	std::unique_ptr<FrameBuffer> bloom1{};
	std::unique_ptr<FrameBuffer> bloom2{};
	std::unique_ptr<FrameBuffer> bloom3{};
	std::unique_ptr<FrameBuffer> bloom4{};

	//std::unique_ptr<Sprite> img;
	//�t�F�[�h�C���A�E�g�p
	std::unique_ptr<Transition> FadeBlack;

	//�e�N�X�`���p
	std::shared_ptr<Texture> tex;

	//BGM�p
	std::unique_ptr<SoundSource> BGM;

	// �t�F�[�h�C���A�E�g�t���O
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

	//�X�e�[�W�̍X�V
	void StageUpdate();

	//�u���[�������_�[�֐�
	void RenderBloom();

	//���U���g�����L���O�v�Z�֐�
	void ResultMeter();

	//���C�g�Z�b�g
	static void SetDirLight(DirectX::XMFLOAT3 dir, DirectX::XMFLOAT3 color);

	// ���[���h���W(3D)����X�N���[�����W(2D)�֕ϊ�
	void WorldToScreen(DirectX::XMFLOAT3* screenPosition, const DirectX::XMFLOAT3& worldPosition);

	// �X�N���[�����W(2D)���烏�[���h���W(3D)�֕ϊ�
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

	//�J�����R���g���[���[����
	CameraController* cameraController = nullptr;

	//�V�F�[�_�[
	std::unique_ptr<Shader> ParticleShader{};
	std::unique_ptr<Shader> NoLightShader{};
	std::unique_ptr<Shader> BrightShader{};
	std::unique_ptr<Shader> BokehShader{};
	Microsoft::WRL::ComPtr<ID3D11PixelShader> PixelShaders[8];

	//�p�[�e�B�N��
	std::unique_ptr<ParticleSystem> Particles{};

	//�u���[���֘A
	float threshold = 0.470f; //�P�x臒l

	//�X�N���[���ڂ����̂��߂Ɋ��鐔�l
	const float SCREEN = 1;
	const float BLOOM1 = 2;
	const float BLOOM2 = 4;
	const float BLOOM3 = 16;
	const float BLOOM4 = 64;

	//�I�t�X�N���[���ŕ`��p
	std::unique_ptr<FullScreenQuad> fullScreen{};         //�m�[�}��
	std::unique_ptr<FullScreenQuad> fullScreenBright{};   //�u���C�g�V�F�[�_�[�p
	std::unique_ptr<FullScreenQuad> fullScreenBokeh{};    //�{�P�V�F�[�_�[�p
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
	//�u���[�������_�[�֐�
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

	//�J�����R���g���[���[����
	CameraController* cameraController = nullptr;

	//�V�F�[�_�[
	std::unique_ptr<Shader> ParticleShader{};
	std::unique_ptr<Shader> NoLightShader{};
	std::unique_ptr<Shader> BrightShader{};
	std::unique_ptr<Shader> BokehShader{};
	Microsoft::WRL::ComPtr<ID3D11PixelShader> PixelShaders[8];

	//�p�[�e�B�N��
	std::unique_ptr<ParticleSystem> Particles{};

	//�u���[���֘A
	float threshold = 0.470f; //�P�x臒l

	//�X�N���[���ڂ����̂��߂Ɋ��鐔�l
	const float SCREEN = 1;
	const float BLOOM1 = 2;
	const float BLOOM2 = 4;
	const float BLOOM3 = 16;
	const float BLOOM4 = 64;

	//�I�t�X�N���[���ŕ`��p
	std::unique_ptr<FullScreenQuad> fullScreen{};         //�m�[�}��
	std::unique_ptr<FullScreenQuad> fullScreenBright{};   //�u���C�g�V�F�[�_�[�p
	std::unique_ptr<FullScreenQuad> fullScreenBokeh{};    //�{�P�V�F�[�_�[�p
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
	//�u���[�������_�[�֐�
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
//		SceneManager(�Ǘ�)�N���X
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
