#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "stage.h"

// �X�e�[�W
class StageBase : public Stage
{
public:
	static StageBase* GetInstance()
	{
		static StageBase Instance;
		return &Instance;
	}


public:
	StageBase();
	StageBase(std::shared_ptr<SkinnedMesh>& mesh) :Stage(mesh) {};
	~StageBase();

	// �C���X�^���X�擾
	//static StageBase& Instance();

	// �X�V����
	void Update(float elapsedTime);

	// �`�揈��
	void Render(ID3D11DeviceContext* immediateContext,
		const DirectX::XMFLOAT4X4& view,
		const DirectX::XMFLOAT4X4& projection,
		const DirectX::XMFLOAT4& light,
		const DirectX::XMFLOAT4& materialColor,
		bool wireframe);

	// �f�o�b�O�pGUI�\��
	void DrawDebugGUI();

	//�X�e�[�W�p�^�[�������_���z�u�p�ϐ�
	//�X�e�[�W�p
	int Rand = 0;
	int EasyRand = 0;
	int Pattern = 0;
	// ���C�L���X�g
	//bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);

private:
	//Model* model = nullptr;
};
