#include "stageBase.h"
#include "stageManager.h"

static Stage* instance = nullptr;


//�R���X�g���N�^
StageBase::StageBase()
{
	//�X�e�[�W�x�[�X������
	stageBaseObj = std::make_unique<SkinnedObject>(stageBaseMesh2);
	stageBaseObj->SetPosition(DirectX::XMFLOAT3(0.0f, -1.5f, 0.0f));
	stageBaseObj->SetScale(DirectX::XMFLOAT3(5.0f, 0.1f, 5.0f));
	stageBaseObj->SetAngle(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
}

StageBase::~StageBase()
{
	delete instance;
}

// �X�V����
void StageBase::Update(float elapsedTime)
{
	//test
	stageBaseObj->SetPosition({ position.x,-0.1f,position.z });

	//����
	squea.spritPosition[0] = DirectX::XMFLOAT3
	(
		stageBaseObj->GetPosition().x - (stageBaseObj->GetScale().x * 2.0f) / 3.0f,
		0.0f,
		stageBaseObj->GetPosition().z + (stageBaseObj->GetScale().z * 2.0f) / 3.0f
	);

	//��
	squea.spritPosition[1] = DirectX::XMFLOAT3
	(
		stageBaseObj->GetPosition().x,
		0.0f,
		stageBaseObj->GetPosition().z + (stageBaseObj->GetScale().z * 2.0f) / 3.0f
	);

	//�E��
	squea.spritPosition[2] = DirectX::XMFLOAT3
	(
		stageBaseObj->GetPosition().x + (stageBaseObj->GetScale().x * 2.0f) / 3.0f,
		0.0f,
		stageBaseObj->GetPosition().z + (stageBaseObj->GetScale().z * 2.0f) / 3.0f
	);

	//��
	squea.spritPosition[3] = DirectX::XMFLOAT3
	(
		stageBaseObj->GetPosition().x - (stageBaseObj->GetScale().x * 2.0f) / 3.0f,
		0.0f,
		stageBaseObj->GetPosition().z
	);

	//�^��
	squea.spritPosition[4] = DirectX::XMFLOAT3
	(
		stageBaseObj->GetPosition().x,
		0.0f,
		stageBaseObj->GetPosition().z
	);

	//�E
	squea.spritPosition[5] = DirectX::XMFLOAT3
	(
		stageBaseObj->GetPosition().x + (stageBaseObj->GetScale().x * 2.0f) / 3.0f,
		0.0f,
		stageBaseObj->GetPosition().z
	);

	//����
	squea.spritPosition[6] = DirectX::XMFLOAT3
	(
		stageBaseObj->GetPosition().x - (stageBaseObj->GetScale().x * 2.0f) / 3.0f,
		0.0f,
		stageBaseObj->GetPosition().z - (stageBaseObj->GetScale().z * 2.0f) / 3.0f
	);

	//��
	squea.spritPosition[7] = DirectX::XMFLOAT3
	(
		stageBaseObj->GetPosition().x,
		0.0f,
		stageBaseObj->GetPosition().z - (stageBaseObj->GetScale().z * 2.0f) / 3.0f
	);

	//�E��
	squea.spritPosition[8] = DirectX::XMFLOAT3
	(
		stageBaseObj->GetPosition().x + (stageBaseObj->GetScale().x * 2.0f) / 3.0f,
		0.0f,
		stageBaseObj->GetPosition().z - (stageBaseObj->GetScale().z * 2.0f) / 3.0f
	);

	//�X�V����
	stageBaseObj->Update(elapsedTime);
}

//�`�揈��
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
