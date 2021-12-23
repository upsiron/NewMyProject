#include "stageBase.h"
#include "stageManager.h"

static Stage* instance = nullptr;


//�R���X�g���N�^
StageBase::StageBase()
{
	//�X�e�[�W�x�[�X������
	stageBaseObj = std::make_unique<SkinnedObject>(stageBaseMesh);
	stageBaseObj->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	stageBaseObj->SetScale(DirectX::XMFLOAT3(5.0f, 5.0f, 5.0f));
	stageBaseObj->SetAngle(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	//�X�e�[�W�p�^�[��������
	Rand = rand() % 5;
	//��Q���̔z�u�ꏊ������
	SquareRand[0] = rand() % 9;
	SquareRand[1] = rand() % 9;
}

StageBase::~StageBase()
{
	// �X�e�[�W���f����j��
	//delete model;
}

// �X�V����
void StageBase::Update(float elapsedTime)
{
	//test
	stageBaseObj->SetPosition(position);

	//����
	squea.SpritPosition[0] = DirectX::XMFLOAT3
	(
		stageBaseObj->GetPosition().x - (stageBaseObj->GetScale().x * 2.0f) / 3.0f,
		0.0f,
		stageBaseObj->GetPosition().z + (stageBaseObj->GetScale().z * 2.0f) / 3.0f
	);

	//��
	squea.SpritPosition[1] = DirectX::XMFLOAT3
	(
		stageBaseObj->GetPosition().x,
		0.0f,
		stageBaseObj->GetPosition().z + (stageBaseObj->GetScale().z * 2.0f) / 3.0f
	);

	//�E��
	squea.SpritPosition[2] = DirectX::XMFLOAT3
	(
		stageBaseObj->GetPosition().x + (stageBaseObj->GetScale().x * 2.0f) / 3.0f,
		0.0f,
		stageBaseObj->GetPosition().z + (stageBaseObj->GetScale().z * 2.0f) / 3.0f
	);

	//��
	squea.SpritPosition[3] = DirectX::XMFLOAT3
	(
		stageBaseObj->GetPosition().x - (stageBaseObj->GetScale().x * 2.0f) / 3.0f,
		0.0f,
		stageBaseObj->GetPosition().z
	);

	//�^��
	squea.SpritPosition[4] = DirectX::XMFLOAT3
	(
		stageBaseObj->GetPosition().x,
		0.0f,
		stageBaseObj->GetPosition().z
	);

	//�E
	squea.SpritPosition[5] = DirectX::XMFLOAT3
	(
		stageBaseObj->GetPosition().x + (stageBaseObj->GetScale().x * 2.0f) / 3.0f,
		0.0f,
		stageBaseObj->GetPosition().z
	);

	//����
	squea.SpritPosition[6] = DirectX::XMFLOAT3
	(
		stageBaseObj->GetPosition().x - (stageBaseObj->GetScale().x * 2.0f) / 3.0f,
		0.0f,
		stageBaseObj->GetPosition().z - (stageBaseObj->GetScale().z * 2.0f) / 3.0f
	);

	//��
	squea.SpritPosition[7] = DirectX::XMFLOAT3
	(
		stageBaseObj->GetPosition().x,
		0.0f,
		stageBaseObj->GetPosition().z - (stageBaseObj->GetScale().z * 2.0f) / 3.0f
	);

	//�E��
	squea.SpritPosition[8] = DirectX::XMFLOAT3
	(
		stageBaseObj->GetPosition().x + (stageBaseObj->GetScale().x * 2.0f) / 3.0f,
		0.0f,
		stageBaseObj->GetPosition().z - (stageBaseObj->GetScale().z * 2.0f) / 3.0f
	);

	//�X�V����
	stageBaseObj->Update();
}

//�`�揈��
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
			//�����h
			int r = Rand;
			ImGui::InputInt("rand", &r);
			//if (ImGui::CollapsingHeader("rand", ImGuiTreeNodeFlags_DefaultOpen))
			//{
			//	//�����h
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
