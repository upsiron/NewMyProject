#include "stageTile.h"
#include "collision.h"
#include "scene.h"

//�R���X�g���N�^
StageTile::StageTile()
{
	stageTileObj = std::make_unique<SkinnedObject>(stageTileMesh);
	stageTileObj->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	stageTileObj->SetScale(DirectX::XMFLOAT3(1.6f, 0.1f, 1.6f));
	stageTileObj->SetAngle(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	StageScaleGap = 0.7f;
}

StageTile::~StageTile()
{
}

// �X�V����
void StageTile::Update(float elapsedTime)
{
	if (ColorNum == RED || ColorNum == GREEN || ColorNum == BLUE)
	{
		stageTileObj = std::make_unique<SkinnedObject>(gimmickTileMesh);
		stageTileObj->SetScale(DirectX::XMFLOAT3(1.6f, 0.1f, 1.6f));
	}
	else
	{
		stageTileObj = std::make_unique<SkinnedObject>(stageTileMesh);
		stageTileObj->SetScale(DirectX::XMFLOAT3(1.6f, 0.1f, 1.6f));
	}

	stageTileObj->SetPosition(position);
	
	//�^�C����4�_�|�W�V�����擾
	tileLeftTop = DirectX::XMFLOAT3(
		stageTileObj->GetPosition().x - stageTileObj->GetScale().x - StageScaleGap,
		0.0f,
		stageTileObj->GetPosition().z + stageTileObj->GetScale().z + StageScaleGap);
	tileRightTop = DirectX::XMFLOAT3(
		stageTileObj->GetPosition().x + stageTileObj->GetScale().x + StageScaleGap,
		0.0f,
		stageTileObj->GetPosition().z + stageTileObj->GetScale().z + StageScaleGap);
	tileLeftBottom = DirectX::XMFLOAT3(
		stageTileObj->GetPosition().x - stageTileObj->GetScale().x - StageScaleGap,
		0.0f,
		stageTileObj->GetPosition().z - stageTileObj->GetScale().z - StageScaleGap);
	tileRightBottom = DirectX::XMFLOAT3(
		stageTileObj->GetPosition().x + stageTileObj->GetScale().x + StageScaleGap,
		0.0f,
		stageTileObj->GetPosition().z - stageTileObj->GetScale().z - StageScaleGap);

	stageTileObj->Update(elapsedTime);
}

//�`�揈��
void StageTile::Render(ID3D11DeviceContext* immediateContext,
	const DirectX::XMFLOAT4X4& view,
	const DirectX::XMFLOAT4X4& projection,
	const DirectX::XMFLOAT4& light,
	const DirectX::XMFLOAT4& materialColor,
	bool wireframe)
{
	stageTileObj->Render(immediateContext, view, projection, light, TileColor[ColorNum], false);
}

void StageTile::DrawDebugGUI()
{
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("StageTile", nullptr, ImGuiWindowFlags_None))
	{
		//�g�����X�t�H�[��
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			//ImGui::InputInt("value",)
			// �ʒu
			//ImGui::InputFloat3("Position", &position.x);
			// ��]
			DirectX::XMFLOAT3 a;
			a.x = DirectX::XMConvertToDegrees(angle.x);
			a.y = DirectX::XMConvertToDegrees(angle.y);
			a.z = DirectX::XMConvertToDegrees(angle.z);
			//ImGui::InputFloat3("Angle", &a.x);
			angle.x = DirectX::XMConvertToRadians(a.x);
			angle.y = DirectX::XMConvertToRadians(a.y);
			angle.z = DirectX::XMConvertToRadians(a.z);
			// �X�P�[��
			//ImGui::InputFloat3("Scale", &scale.x);
			/*ImGui::InputFloat3("cornerLT", &TriangleCorners[0][0].x);
			ImGui::InputFloat3("cornerLB", &TriangleCorners[1][1].x);
			ImGui::InputFloat3("cornerRT", &TriangleCorners[0][1].x);
			ImGui::InputFloat3("cornerRB", &TriangleCorners[0][2].x);*/
		}
	}
	ImGui::End();
}