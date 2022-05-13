#include "stage.h"


static Stage* instance = nullptr;

// �C���X�^���X�擾
Stage& Stage::Instance()
{
	return *instance;
}

//�R���X�g���N�^
Stage::Stage()
{
	instance = this;
	
	Framework& framework = Framework::Instance();
	ID3D11Device* device = framework.GetDevice();

	//�X�e�[�W������
	stageBaseMesh = std::make_shared<SkinnedMesh>(device, "Data/Stage/StageTile.fbx", true);
	stageBaseMesh2 = std::make_shared<SkinnedMesh>(device, "Data/Stage/untitled.fbx", true);

	//device->Release();
}

Stage::~Stage()
{
	// �X�e�[�W���f����j��
	//delete model;
	//delete instance;
}

// �X�V����
//void Stage::Update(float elapsedTime)
//{
//	
//}

//�`�揈��
//void Stage::Render(ID3D11DeviceContext* immediateContext,
//	const DirectX::XMFLOAT4X4& view,
//	const DirectX::XMFLOAT4X4& projection,
//	const DirectX::XMFLOAT4& light,
//	const DirectX::XMFLOAT4& materialColor,
//	bool wireframe)
//{
//	//�X�e�[�W�`��
//	//stageBaseObj->Render(immediateContext, view, projection, light, materialColor, false);
//
//}

void Stage::DrawDebugGUI()
{
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("StageBase", nullptr, ImGuiWindowFlags_None))
	{
		//�g�����X�t�H�[��
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			// �ʒu
			ImGui::InputFloat3("Position", &position.x);
			// ��]
			DirectX::XMFLOAT3 a;
			a.x = DirectX::XMConvertToDegrees(angle.x);
			a.y = DirectX::XMConvertToDegrees(angle.y);
			a.z = DirectX::XMConvertToDegrees(angle.z);
			ImGui::InputFloat3("Angle", &a.x);
			angle.x = DirectX::XMConvertToRadians(a.x);
			angle.y = DirectX::XMConvertToRadians(a.y);
			angle.z = DirectX::XMConvertToRadians(a.z);
			// �X�P�[��
			ImGui::InputFloat3("Scale", &scale.x);
		}
	}
	ImGui::End();
}