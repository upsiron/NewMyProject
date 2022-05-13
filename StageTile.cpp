#include "stageTile.h"
#include "collision.h"
#include "scene.h"

//コンストラクタ
StageTile::StageTile()
{
	stageTileObj = std::make_unique<SkinnedObject>(stageBaseMesh);
	stageTileObj->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	stageTileObj->SetScale(DirectX::XMFLOAT3(1.6f, 0.1f, 1.6f));
	stageTileObj->SetAngle(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	StageScaleGap = 0.7f;
}

StageTile::~StageTile()
{
}

// 更新処理
void StageTile::Update(float elapsedTime)
{
	stageTileObj->SetPosition(position);

	//タイルの4点ポジション取得
	TileLeftTop = DirectX::XMFLOAT3(
		stageTileObj->GetPosition().x - stageTileObj->GetScale().x - StageScaleGap,
		0.0f,
		stageTileObj->GetPosition().z + stageTileObj->GetScale().z + StageScaleGap);
	TileRightTop = DirectX::XMFLOAT3(
		stageTileObj->GetPosition().x + stageTileObj->GetScale().x + StageScaleGap,
		0.0f,
		stageTileObj->GetPosition().z + stageTileObj->GetScale().z + StageScaleGap);
	TileLeftBottom = DirectX::XMFLOAT3(
		stageTileObj->GetPosition().x - stageTileObj->GetScale().x - StageScaleGap,
		0.0f,
		stageTileObj->GetPosition().z - stageTileObj->GetScale().z - StageScaleGap);
	TileRightBottom = DirectX::XMFLOAT3(
		stageTileObj->GetPosition().x + stageTileObj->GetScale().x + StageScaleGap,
		0.0f,
		stageTileObj->GetPosition().z - stageTileObj->GetScale().z - StageScaleGap);

	stageTileObj->Update(elapsedTime);
}

//描画処理
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
		//トランスフォーム
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			//ImGui::InputInt("value",)
			// 位置
			//ImGui::InputFloat3("Position", &position.x);
			// 回転
			DirectX::XMFLOAT3 a;
			a.x = DirectX::XMConvertToDegrees(angle.x);
			a.y = DirectX::XMConvertToDegrees(angle.y);
			a.z = DirectX::XMConvertToDegrees(angle.z);
			//ImGui::InputFloat3("Angle", &a.x);
			angle.x = DirectX::XMConvertToRadians(a.x);
			angle.y = DirectX::XMConvertToRadians(a.y);
			angle.z = DirectX::XMConvertToRadians(a.z);
			// スケール
			//ImGui::InputFloat3("Scale", &scale.x);
			/*ImGui::InputFloat3("cornerLT", &TriangleCorners[0][0].x);
			ImGui::InputFloat3("cornerLB", &TriangleCorners[1][1].x);
			ImGui::InputFloat3("cornerRT", &TriangleCorners[0][1].x);
			ImGui::InputFloat3("cornerRB", &TriangleCorners[0][2].x);*/
		}
	}
	ImGui::End();
}