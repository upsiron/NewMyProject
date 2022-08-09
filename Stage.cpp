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
	stageTileMesh = std::make_shared<SkinnedMesh>(device, "Data/Stage/StageTile.fbx", true);
	gimmickTileMesh = std::make_shared<SkinnedMesh>(device, "Data/Stage/GimmickTile.fbx", true);
	stageBaseMesh = std::make_shared<SkinnedMesh>(device, "Data/Stage/StageBase.fbx", true);


}

Stage::~Stage()
{
	//delete instance;
}

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