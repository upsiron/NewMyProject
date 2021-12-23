#include "stageBase.h"
#include "stageManager.h"

static Stage* instance = nullptr;


//コンストラクタ
StageBase::StageBase()
{
	//ステージベース初期化
	stageBaseObj = std::make_unique<SkinnedObject>(stageBaseMesh);
	stageBaseObj->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	stageBaseObj->SetScale(DirectX::XMFLOAT3(5.0f, 5.0f, 5.0f));
	stageBaseObj->SetAngle(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	//ステージパターン初期化
	Rand = rand() % 5;
	//障害物の配置場所初期化
	SquareRand[0] = rand() % 9;
	SquareRand[1] = rand() % 9;
}

StageBase::~StageBase()
{
	// ステージモデルを破棄
	//delete model;
}

// 更新処理
void StageBase::Update(float elapsedTime)
{
	//test
	stageBaseObj->SetPosition(position);

	//左上
	squea.SpritPosition[0] = DirectX::XMFLOAT3
	(
		stageBaseObj->GetPosition().x - (stageBaseObj->GetScale().x * 2.0f) / 3.0f,
		0.0f,
		stageBaseObj->GetPosition().z + (stageBaseObj->GetScale().z * 2.0f) / 3.0f
	);

	//上
	squea.SpritPosition[1] = DirectX::XMFLOAT3
	(
		stageBaseObj->GetPosition().x,
		0.0f,
		stageBaseObj->GetPosition().z + (stageBaseObj->GetScale().z * 2.0f) / 3.0f
	);

	//右上
	squea.SpritPosition[2] = DirectX::XMFLOAT3
	(
		stageBaseObj->GetPosition().x + (stageBaseObj->GetScale().x * 2.0f) / 3.0f,
		0.0f,
		stageBaseObj->GetPosition().z + (stageBaseObj->GetScale().z * 2.0f) / 3.0f
	);

	//左
	squea.SpritPosition[3] = DirectX::XMFLOAT3
	(
		stageBaseObj->GetPosition().x - (stageBaseObj->GetScale().x * 2.0f) / 3.0f,
		0.0f,
		stageBaseObj->GetPosition().z
	);

	//真ん中
	squea.SpritPosition[4] = DirectX::XMFLOAT3
	(
		stageBaseObj->GetPosition().x,
		0.0f,
		stageBaseObj->GetPosition().z
	);

	//右
	squea.SpritPosition[5] = DirectX::XMFLOAT3
	(
		stageBaseObj->GetPosition().x + (stageBaseObj->GetScale().x * 2.0f) / 3.0f,
		0.0f,
		stageBaseObj->GetPosition().z
	);

	//左下
	squea.SpritPosition[6] = DirectX::XMFLOAT3
	(
		stageBaseObj->GetPosition().x - (stageBaseObj->GetScale().x * 2.0f) / 3.0f,
		0.0f,
		stageBaseObj->GetPosition().z - (stageBaseObj->GetScale().z * 2.0f) / 3.0f
	);

	//下
	squea.SpritPosition[7] = DirectX::XMFLOAT3
	(
		stageBaseObj->GetPosition().x,
		0.0f,
		stageBaseObj->GetPosition().z - (stageBaseObj->GetScale().z * 2.0f) / 3.0f
	);

	//右下
	squea.SpritPosition[8] = DirectX::XMFLOAT3
	(
		stageBaseObj->GetPosition().x + (stageBaseObj->GetScale().x * 2.0f) / 3.0f,
		0.0f,
		stageBaseObj->GetPosition().z - (stageBaseObj->GetScale().z * 2.0f) / 3.0f
	);

	//更新処理
	stageBaseObj->Update();
}

//描画処理
void StageBase::Render(ID3D11DeviceContext* immediateContext,
	const DirectX::XMFLOAT4X4& view,
	const DirectX::XMFLOAT4X4& projection,
	const DirectX::XMFLOAT4& light,
	const DirectX::XMFLOAT4& materialColor,
	bool wireframe)
{
	stageBaseObj->Render(immediateContext, view, projection, light, materialColor, 0.0f, false);
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
			//ランド
			int r = Rand;
			ImGui::InputInt("rand", &r);
			//if (ImGui::CollapsingHeader("rand", ImGuiTreeNodeFlags_DefaultOpen))
			//{
			//	//ランド
			//	int r = Rand;
			//	ImGui::InputInt("rand", &r);
			//	for (int i = 0; i < 2; i++)
			//	{
			//		ImGui::InputInt("SquareRand" + i, &SquareRand[i] + 1);
			//	}
			//}
		}
	}
	ImGui::End();
}
