#include "Coin.h"

// �R���X�g���N�^
Coin::Coin()
{
	coinObj = std::make_unique<SkinnedObject>(coinMesh);
	coinObj->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	coinObj->SetScale(DirectX::XMFLOAT3(0.5f, 0.5f, 0.2f));
	coinObj->SetAngle(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));

	radius = 0.5f;
	height = 1.0f;
}

// �f�X�g���N�^
Coin::~Coin()
{
	//obstacleMesh = nullptr;
}

//�X�V����
void Coin::Update(float elapsedTime)
{
	position.y += movePosition;
	coinObj->SetPosition(position);
	angle.y += moveAngle;
	angle.z = DirectX::XMConvertToRadians(45);
	coinObj->SetAngle(angle);
	coinObj->Update(elapsedTime);
}

//�`�揈��
void Coin::Render(ID3D11DeviceContext* immediateContext,
	const DirectX::XMFLOAT4X4& view,
	const DirectX::XMFLOAT4X4& projection,
	const DirectX::XMFLOAT4& light,
	const DirectX::XMFLOAT4& materialColor,
	bool wireframe)
{
	coinObj->Render(immediateContext, view, projection, light, DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f), false);
}

// �f�o�b�O ��Q�����\��
void Coin::DrawDebugGUI()
{
	std::string str = "";
	/*if (activeNode != nullptr)
	{
		str = activeNode->GetName();
	}*/
	//�g�����X�t�H�[��
	if (ImGui::CollapsingHeader("Coin", ImGuiTreeNodeFlags_DefaultOpen))
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

		ImGui::Text("true(1)/false(0):%d", GetExistFlg());

		ImGui::Text(u8"Behavior�@%s", str.c_str());
	}
}