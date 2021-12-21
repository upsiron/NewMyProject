#include "ObstacleBlock.h"

// コンストラクタ
ObstacleBlock::ObstacleBlock()
{
	Framework& framework = Framework::Instance();
	ID3D11Device* device = framework.GetDevice();

	//slimeMesh = std::make_shared<SkinnedMesh>(device, "Data/Slime/slime.fbx",true);
	obstacleMesh = std::make_shared<SkinnedMesh>(device, "Data/001_cube.fbx", true);
	obstacleObj = std::make_unique<SkinnedObject>(obstacleMesh);
	obstacleObj->SetPosition(DirectX::XMFLOAT3(2.0f, 0.0f, 0.0f));
	obstacleObj->SetScale(DirectX::XMFLOAT3(1.4f, 1.4f, 1.4f));
	//obstacleObj->SetScale(DirectX::XMFLOAT3(4.2f, 1.4f, 1.4f));
	obstacleObj->SetAngle(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));

	// モデルが大きいのでスケーリング
	//scale.x = scale.y = scale.z = 0.01f;

	radius = 0.5f;
	height = 1.0f;
}

// デストラクタ
ObstacleBlock::~ObstacleBlock()
{

}

//更新処理
void ObstacleBlock::Update(float elapsedTime)
{
	obstacleObj->SetPosition(position);
	obstacleObj->SetAngle(angle);

	//ブロック当たる上側4点のポジション取得
	ObstacleUpLeftTop = DirectX::XMFLOAT3(
		obstacleObj->GetPosition().x - obstacleObj->GetScale().x,
		obstacleObj->GetPosition().y + obstacleObj->GetScale().y,
		obstacleObj->GetPosition().z + obstacleObj->GetScale().z);
	ObstacleUpRightTop = DirectX::XMFLOAT3(
		obstacleObj->GetPosition().x + obstacleObj->GetScale().x,
		obstacleObj->GetPosition().y + obstacleObj->GetScale().y,
		obstacleObj->GetPosition().z + obstacleObj->GetScale().z);
	ObstacleUpLeftBottom = DirectX::XMFLOAT3(
		obstacleObj->GetPosition().x - obstacleObj->GetScale().x,
		obstacleObj->GetPosition().y + obstacleObj->GetScale().y,
		obstacleObj->GetPosition().z - obstacleObj->GetScale().z);
	ObstacleUpRightBottom = DirectX::XMFLOAT3(
		obstacleObj->GetPosition().x + obstacleObj->GetScale().x,
		obstacleObj->GetPosition().y + obstacleObj->GetScale().y,
		obstacleObj->GetPosition().z - obstacleObj->GetScale().z);

	//ブロック当たる下側4点のポジション取得
	ObstacleDownLeftTop = DirectX::XMFLOAT3(
		obstacleObj->GetPosition().x - obstacleObj->GetScale().x,
		obstacleObj->GetPosition().y - obstacleObj->GetScale().y,
		obstacleObj->GetPosition().z + obstacleObj->GetScale().z);
	ObstacleDownRightTop = DirectX::XMFLOAT3(
		obstacleObj->GetPosition().x + obstacleObj->GetScale().x,
		obstacleObj->GetPosition().y - obstacleObj->GetScale().y,
		obstacleObj->GetPosition().z + obstacleObj->GetScale().z);
	ObstacleDownLeftBottom = DirectX::XMFLOAT3(
		obstacleObj->GetPosition().x - obstacleObj->GetScale().x,
		obstacleObj->GetPosition().y - obstacleObj->GetScale().y,
		obstacleObj->GetPosition().z - obstacleObj->GetScale().z);
	ObstacleDownRightBottom = DirectX::XMFLOAT3(
		obstacleObj->GetPosition().x + obstacleObj->GetScale().x,
		obstacleObj->GetPosition().y - obstacleObj->GetScale().y,
		obstacleObj->GetPosition().z - obstacleObj->GetScale().z);

	obstacleObj->Update();
}

//描画処理
void ObstacleBlock::Render(ID3D11DeviceContext* immediateContext,
	const DirectX::XMFLOAT4X4& view,
	const DirectX::XMFLOAT4X4& projection,
	const DirectX::XMFLOAT4& light,
	const DirectX::XMFLOAT4& materialColor,
	bool wireframe)
{
	obstacleObj->Render(immediateContext, view, projection, light, materialColor, 0.0f, false);
}

// デバッグ 障害物情報表示
void ObstacleBlock::DrawDebugGUI()
{
	std::string str = "";
	/*if (activeNode != nullptr)
	{
		str = activeNode->GetName();
	}*/
	//トランスフォーム
	if (ImGui::CollapsingHeader("EnemyBlueSlime", ImGuiTreeNodeFlags_DefaultOpen))
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

		ImGui::Text(u8"Behavior　%s", str.c_str());
	}
}