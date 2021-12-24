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
#include "stage.h"
#include "stageBase.h"
#include "stageTile.h"
#include "CameraController.h"
#include "GeometricPrimitive.h"
#include "Input/Input.h"


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

		return true;
	}

public:
	Scene(){}
	virtual ~Scene() {}
	virtual void Initialize() = 0;
	virtual void Finalize() = 0;
	virtual void Update(float elapsedTime) = 0;
	virtual void Render()=0;

	struct ConstantBufferForPerFrame
	{
		DirectX::XMFLOAT4	LightColor;		//���C�g�̐F
		//DirectX::XMFLOAT4	LightDir;		//���C�g�̕���
		DirectX::XMFLOAT4	AmbientColor;	//����
		DirectX::XMFLOAT4	EyePos;			//�J����
	};

	//imGui
	//virtual void imGuiUpdate();
	void imGuiRender();

	int Result[5] = { 0 };
	int ThisResult = 0;
};

class SceneLoad :public Scene {
private:
	std::unique_ptr<Scene> nextScene;
	std::unique_ptr<Transition> FadeBlack;
	// Now Loading...�A�j���[�V�����p 
	float timer = 0.0f;
	int counter = 0;
	char message[32] = { 0 };
public:
	SceneLoad(Scene* nextScene) {
		// ���[�f�B���O��ɑJ�ڂ���V�[����ێ�
		this->nextScene.reset(nextScene);
	}
	virtual ~SceneLoad() {}
	void Initialize() override;
	void Finalize()  override {};
	void Update(float elapsedTime) override;
	void Render() override;
private:
	static void LoadingThread(SceneLoad* scene);
};




class SceneMain :public Scene
{
public:
#define StageMax 10
	//static const int StageMax = 5;
	static const int StageTileMax = 9;
	static const int ObstacleMax = 3;
private:
	Player* player = nullptr;
	Stage* stage = nullptr;
	EnemyBlueSlime* enemy[5][2] = {nullptr};
	ObstacleBlock* obstacle[StageMax][ObstacleMax] = { nullptr };
	StageBase* stageBase[StageMax] = { nullptr };
	StageTile* stageTile[StageMax][StageTileMax] = { nullptr };
	CameraController* cameraController = nullptr;


	std::shared_ptr<SkinnedMesh> skyMesh;
	std::unique_ptr<SkinnedObject> skyObj;
	//std::unique_ptr<Sprite> img;
	std::unique_ptr<Transition> FadeBlack;
	//DirectX::XMFLOAT3 pos = { 0.0f,0.0f,0.0f };
	//DirectX::XMFLOAT3 angle = { 0.0f,0.0f,0.0f };

	std::shared_ptr<Texture> tex;

	std::unique_ptr<SoundSource>	BGM;

	//Microsoft::WRL::ComPtr<ID3D11Buffer> ConstantBuffer;
	////�R���X�^���g�o�b�t�@�쐬
	//bool CreateConstantBuffer(ID3D11Buffer** ppCB, u_int size)
	//{
	//	Framework& framework = Framework::Instance();
	//	ID3D11Device* device = framework.GetDevice();
	//	// �萔�o�b�t�@����
	//	D3D11_BUFFER_DESC bd;
	//	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	//	bd.ByteWidth = size;
	//	bd.Usage = D3D11_USAGE_DEFAULT;
	//	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	//	bd.CPUAccessFlags = 0;

	//	HRESULT hr = device->CreateBuffer(&bd, NULL, ppCB);
	//	assert(SUCCEEDED(hr));

	//	return true;
	//}
	int cameraState = 0;
	bool fadeInFlg = false;
	bool fadeOutFlg = false;
public:
	SceneMain() {}
	virtual ~SceneMain() {};
	void Initialize() override;
	void Finalize() override;
	void Update(float elapsedTime) override;
	void Render() override;

	void ResultMeter();

	static void SetDirLight(DirectX::XMFLOAT3 dir, DirectX::XMFLOAT3 color);


	// ���[���h���W(3D)����X�N���[�����W(2D)�֕ϊ�
	void WorldToScreen(DirectX::XMFLOAT3* screenPosition, const DirectX::XMFLOAT3& worldPosition);
	// �X�N���[�����W(2D)���烏�[���h���W(3D)�֕ϊ�
	void ScreenToWorld(DirectX::XMFLOAT3* worldPosition, const DirectX::XMFLOAT3& screenPosition);

	//imGui
	void imGuiUpdate();
	
	//struct ConstantBufferForPerFrame
	//{
	//	DirectX::XMFLOAT4	LightColor;		//���C�g�̐F
	//	//DirectX::XMFLOAT4	LightDir;		//���C�g�̕���
	//	DirectX::XMFLOAT4	AmbientColor;	//����
	//	DirectX::XMFLOAT4	EyePos;			//�J����
	//};
};



class SceneTitle :public Scene
{
private:
	std::unique_ptr<Sprite> img;
	std::unique_ptr<Transition> FadeBlack;

public:
	SceneTitle() {}
	virtual ~SceneTitle() {}
	void Initialize() override;
	void Finalize()  override {};
	void Update(float elapsedTime) override;
	void Render() override;

};

class SceneClear :public Scene
{
private:
	std::unique_ptr<Sprite> img;
	std::unique_ptr<Transition> FadeBlack;
public:
	static SceneClear& Instance()
	{
		static SceneClear inst;
		return inst;
	}

	SceneClear() {}
	virtual ~SceneClear() {}
	void Initialize() override;
	void Finalize()  override {};
	void Update(float elapsedTime) override;
	void Render() override;
};

class SceneOver :public Scene
{
private:
	std::unique_ptr<Sprite> img;

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
