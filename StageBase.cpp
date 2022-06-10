#include "stageBase.h"
#include "stageManager.h"

static Stage* instance = nullptr;


//コンストラクタ
StageBase::StageBase()
{
	//ステージベース初期化
	stageBaseObj = std::make_unique<SkinnedObject>(stageBaseMesh2);
	stageBaseObj->SetPosition(DirectX::XMFLOAT3(0.0f, -1.5f, 0.0f));
	stageBaseObj->SetScale(DirectX::XMFLOAT3(5.0f, 0.1f, 5.0f));
	stageBaseObj->SetAngle(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
}

StageBase::~StageBase()
{
	delete instance;
}

// 更新処理
void StageBase::Update(float elapsedTime)
{
	//test
	stageBaseObj->SetPosition({ position.x,-0.1f,position.z });

	//左上
	squea.spritPosition[0] = DirectX::XMFLOAT3
	(
		stageBaseObj->GetPosition().x - (stageBaseObj->GetScale().x * 2.0f) / 3.0f,
		0.0f,
		stageBaseObj->GetPosition().z + (stageBaseObj->GetScale().z * 2.0f) / 3.0f
	);

	//上
	squea.spritPosition[1] = DirectX::XMFLOAT3
	(
		stageBaseObj->GetPosition().x,
		0.0f,
		stageBaseObj->GetPosition().z + (stageBaseObj->GetScale().z * 2.0f) / 3.0f
	);

	//右上
	squea.spritPosition[2] = DirectX::XMFLOAT3
	(
		stageBaseObj->GetPosition().x + (stageBaseObj->GetScale().x * 2.0f) / 3.0f,
		0.0f,
		stageBaseObj->GetPosition().z + (stageBaseObj->GetScale().z * 2.0f) / 3.0f
	);

	//左
	squea.spritPosition[3] = DirectX::XMFLOAT3
	(
		stageBaseObj->GetPosition().x - (stageBaseObj->GetScale().x * 2.0f) / 3.0f,
		0.0f,
		stageBaseObj->GetPosition().z
	);

	//真ん中
	squea.spritPosition[4] = DirectX::XMFLOAT3
	(
		stageBaseObj->GetPosition().x,
		0.0f,
		stageBaseObj->GetPosition().z
	);

	//右
	squea.spritPosition[5] = DirectX::XMFLOAT3
	(
		stageBaseObj->GetPosition().x + (stageBaseObj->GetScale().x * 2.0f) / 3.0f,
		0.0f,
		stageBaseObj->GetPosition().z
	);

	//左下
	squea.spritPosition[6] = DirectX::XMFLOAT3
	(
		stageBaseObj->GetPosition().x - (stageBaseObj->GetScale().x * 2.0f) / 3.0f,
		0.0f,
		stageBaseObj->GetPosition().z - (stageBaseObj->GetScale().z * 2.0f) / 3.0f
	);

	//下
	squea.spritPosition[7] = DirectX::XMFLOAT3
	(
		stageBaseObj->GetPosition().x,
		0.0f,
		stageBaseObj->GetPosition().z - (stageBaseObj->GetScale().z * 2.0f) / 3.0f
	);

	//右下
	squea.spritPosition[8] = DirectX::XMFLOAT3
	(
		stageBaseObj->GetPosition().x + (stageBaseObj->GetScale().x * 2.0f) / 3.0f,
		0.0f,
		stageBaseObj->GetPosition().z - (stageBaseObj->GetScale().z * 2.0f) / 3.0f
	);

	//更新処理
	stageBaseObj->Update(elapsedTime);
}

//描画処理
void StageBase::Render(ID3D11DeviceContext* immediateContext,
	const DirectX::XMFLOAT4X4& view,
	const DirectX::XMFLOAT4X4& projection,
	const DirectX::XMFLOAT4& light,
	const DirectX::XMFLOAT4& materialColor,
	bool wireframe)
{
	stageBaseObj->Render(immediateContext, view, projection, light, {1.0f,0.0f,0.0f,1.0f}/*materialColor*/, false);
}

void StageBase::DrawDebugGUI()
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
