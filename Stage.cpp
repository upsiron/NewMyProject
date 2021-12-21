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
	stageBaseMesh = std::make_shared<SkinnedMesh>(device, "Data/tile/tile.fbx", true);
	//stageBaseMesh = std::make_shared<SkinnedMesh>(device, "Data/tile/StageTile.fbx", true);
}

Stage::~Stage()
{
	// �X�e�[�W���f����j��
	//delete model;
}

// �X�V����
//void Stage::Update(float elapsedTime)
//{
//	
//}

//�`�揈��
//void Stage::Render(ID3D11DeviceContext* immediateContext,
//	const DirectX::XMFLOAT4X4& view,
//	const DirectX::XMFLOAT4X4& projection,
//	const DirectX::XMFLOAT4& light,
//	const DirectX::XMFLOAT4& materialColor,
//	bool wireframe)
//{
//	//�X�e�[�W�`��
//	//stageBaseObj->Render(immediateContext, view, projection, light, materialColor, false);
//
//}

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


//���C�L���X�g(���g���Ȃ��̂ŃR�����g�A�E�g)
//int Stage::RayCast(
// const DirectX::XMFLOAT3& startPosition, 
// const DirectX::XMFLOAT3& endPosition,
//  DirectX::XMFLOAT3* outPosition,
//  DirectX::XMFLOAT3* outNormal, float* outLength)
//{
//	/*����
//	startPosition : ���C���΂��J�n���W
//	endPosition   : ���C���΂��I�����W
//	outPosition   : ���C�������������W
//	outNormal     : ���C�����������ʂ̖@��
//	outLength     : ���C�����������ʂ܂ł̋���  �߂�l : �}�e���A���ԍ�*/
//
//	int ret = -1;
//	DirectX::XMVECTOR start = DirectX::XMLoadFloat3(&startPosition);
//	DirectX::XMVECTOR end = DirectX::XMLoadFloat3(&endPosition);
//	DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(end, start);
//	DirectX::XMVECTOR length = DirectX::XMVector3Length(vec);
//	DirectX::XMVECTOR dir = DirectX::XMVector3Normalize(vec);
//	float neart;
//	DirectX::XMStoreFloat(&neart, length);
//
//	DirectX::XMVECTOR position, normal;
//
//	for (int i = 0; i < 2; i++) {
//		// �ʒ��_�擾   
//		DirectX::XMVECTOR a = DirectX::XMLoadFloat3(&TriangleCorners[i][0]);
//		DirectX::XMVECTOR b = DirectX::XMLoadFloat3(&TriangleCorners[i][1]);
//		DirectX::XMVECTOR c = DirectX::XMLoadFloat3(&TriangleCorners[i][2]);
//		//  3 �ӎZ�o 
//		DirectX::XMVECTOR ab = DirectX::XMVectorSubtract(b, a);
//		DirectX::XMVECTOR bc = DirectX::XMVectorSubtract(c, b);
//		DirectX::XMVECTOR ca = DirectX::XMVectorSubtract(a, c);
//		// �O�ςɂ��@���Z�o 
//		DirectX::XMVECTOR n = DirectX::XMVector3Cross(ab, bc);
//		// ���ς̌��ʂ��v���X�Ȃ�Η�����
//		DirectX::XMVECTOR dot = DirectX::XMVector3Dot(dir, n);
//		float fdot;
//		DirectX::XMStoreFloat(&fdot, dot);
//		if (fdot >= 0) continue;
//
//		// ��_�Z�o  
//		//cp  �����������W
//		//cp = start(���C�̊J�n���W) + dir(���C�̌���) * t(�X�J���[)    t�����߂�
//		//���C�̊J�n�ʒu���璸�_a�ւ̃x�N�g���Ɩ@�����g���Ē��p�O�p�`�����
//		//��������ˉe(d)�����Ad+�@����d'�����
//		//  [�@�@�@|-----------------------|start
//		//�@|�@ �@d|�ˉe                 / ��     
//		//  |�@�@�@|-------------------/---��dir ]
//		//�@|�@�@�@��|                /     |     |
//		//�@|�@�@�@��|              /       |     |
//		//�@|�@�@�@��|            /         |     |
//		//d'{ �@�� ��|         /            |      } t(�䗦)
//		//�@|�@�@�@��|       /              |     |
//		//�@|�@�@�@��|     /                |     |
//		//�@|�@�@�@��|  /                   |     |
//		//�@[�@ �@�@|/_____________________|____]______c
//		//     �@�@a�@\                    |           |
//		//       �@�@�@�@\                 |           |
//		//          �@�@�@�@\              |           |
//		//             �@�@�@�@\           cp          |
//		//                �@�@�@�@\                    |
//		//                   �@�@�@�@\                 | 
//		//                      �@�@�@�@\              |
//		//                         �@�@�@�@\           |
//		//                            �@�@�@�@\        |
//		//                               �@�@�@�@\     |
//		//                                  �@�@�@�@\  |
//		//                                    �@�@�@�@\|b
//		//
//		DirectX::XMVECTOR v0 = DirectX::XMVectorSubtract(a, start);
//		DirectX::XMVECTOR t = DirectX::XMVectorDivide(DirectX::XMVector3Dot(n, v0), dot);
//		float ft;
//		DirectX::XMStoreFloat(&ft, t);
//		if (ft < 0.0f || ft > neart) continue;
//
//		DirectX::XMVECTOR cp = DirectX::XMVectorAdd(DirectX::XMVectorMultiply(dir, t), start);
//
//		//  ���_����  
//		DirectX::XMVECTOR v1 = DirectX::XMVectorSubtract(a, cp);
//		DirectX::XMVECTOR temp1 = DirectX::XMVector3Cross(v1, ab);
//		DirectX::XMVECTOR work1 = DirectX::XMVector3Dot(temp1, n);
//		float fwork1;
//		DirectX::XMStoreFloat(&fwork1, work1);
//		if (fwork1 < 0.0f) continue;
//
//		DirectX::XMVECTOR v2 = DirectX::XMVectorSubtract(b, cp);
//		DirectX::XMVECTOR temp2 = DirectX::XMVector3Cross(v2, bc);
//		DirectX::XMVECTOR work2 = DirectX::XMVector3Dot(temp2, n);
//		float fwork2;
//		DirectX::XMStoreFloat(&fwork2, work2);
//		if (fwork2 < 0.0f) continue;
//
//		DirectX::XMVECTOR v3 = DirectX::XMVectorSubtract(c, cp);
//		DirectX::XMVECTOR temp3 = DirectX::XMVector3Cross(v3, ca);
//		DirectX::XMVECTOR work3 = DirectX::XMVector3Dot(temp3, n);
//		float fwork3;
//		DirectX::XMStoreFloat(&fwork3, work3);
//		if (fwork3 < 0.0f) continue;
//
//		// ���ۑ� 
//		position = cp;
//		normal = n;
//		neart = ft;
//		//ret = it.MaterialIndex;
//		ret = 1;
//	}
//	if (ret != -1) {
//		DirectX::XMStoreFloat3(outPosition, position);
//		DirectX::XMStoreFloat3(outNormal, normal);
//	}
//	//�ł��߂��q�b�g�ʒu�܂ł̋���  
//	*outLength = neart;
//	return ret;
//}
