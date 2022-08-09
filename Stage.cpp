#include "stage.h"


static Stage* instance = nullptr;

// インスタンス取得
Stage& Stage::Instance()
{
	return *instance;
}

//コンストラクタ
Stage::Stage()
{
	instance = this;
	
	Framework& framework = Framework::Instance();
	ID3D11Device* device = framework.GetDevice();

	//ステージ初期化
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
		//トランスフォーム
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			// 位置
			ImGui::InputFloat3("Position", &position.x);
			// 回転
			DirectX::XMFLOAT3 a;
			a.x = DirectX::XMConvertToDegrees(angle.x);
			a.y = DirectX::XMConvertToDegrees(angle.y);
			a.z = DirectX::XMConvertToDegrees(angle.z);
			ImGui::InputFloat3("Angle", &a.x);
			angle.x = DirectX::XMConvertToRadians(a.x);
			angle.y = DirectX::XMConvertToRadians(a.y);
			angle.z = DirectX::XMConvertToRadians(a.z);
			// スケール
			ImGui::InputFloat3("Scale", &scale.x);
		}
	}
	ImGui::End();
}