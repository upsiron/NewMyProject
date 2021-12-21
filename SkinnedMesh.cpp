#include "misc.h"
#include <sstream>
#include <functional>
#include "Texture.h"
//19���
#include <filesystem>
#include "SkinnedMesh.h"
#include "shader.h"

//21���
#include "ConvertMatrix.h"
//30���
#include <fstream>
using namespace DirectX;
//22���
//�O���[�o���X�R�[�v�ɒǉ��ő��v�Ȃ񂷂��c�H
struct BoneInfluence
{
	uint32_t BoneIndex;
	float BoneWeight;
};

using BoneInfluencePerControlPoint = vector<BoneInfluence>;
//���{�[���̉e���̐���_�ɂ�����H

/*func �{�[���e���x��FBX�̃f�[�^����擾����֐�*/
void FetchBoneInfluence(const FbxMesh* FBXMesh,
	vector<BoneInfluencePerControlPoint>& BoneInfluences)
{
	//�W�I���g��(�����������Ɨ��̂��̂���)�Ɋ��蓖�Ă�ꂽ�R�����g�[���|�C���g�̐���Ԃ��Ă�
	int ControlPointCount = FBXMesh->GetControlPointsCount();
	BoneInfluences.resize(ControlPointCount);

	//���f���������Ă�f�t�H�[�}�̐���Ԃ��Ă�
	const int SkinCount = FBXMesh->GetDeformerCount(fbxsdk::FbxDeformer::eSkin);
	for (int SkinIndex = 0; SkinIndex < SkinCount; ++SkinIndex)
	{
		//�w�肵���C���f�b�N�X�ɂ���w�肵���^�̃f�t�H�[�}��Ԃ��Ă�(GetDeformer(�w�肵���C���f�b�N�X,�w�肵���^(4����A����̓X�L���Ȃ̂�eSkin�^�C�v)))
		const FbxSkin* FBXSkin = static_cast<FbxSkin*>(FBXMesh->GetDeformer(SkinIndex, fbxsdk::FbxDeformer::eSkin));
		//���̃I�u�W�F�N�g�̒ǉ����ꂽ�N���X�^�[�̐���Ԃ��Ă�A�������ׂĂ��悭�킩��Ȃ��������牽�Ƃ������Ȃ�
		//�W���݂����Ȋ����̈Ӗ��������̃f�t�H�[�}�������Ă�|���S�����_�̂��Ƃ������Ă�̂���
		//���₻���̃f�t�H�[�}�������Ă�ő吔����
		const int ClusterCount = FBXSkin->GetClusterCount();
		for (int ClusterIndex = 0; ClusterIndex < ClusterCount; ++ClusterIndex)
		{
			//�w�肵���C���f�b�N�X�ŃN���X�^���擾����
			const FbxCluster* FBXCluster = FBXSkin->GetCluster(ClusterIndex);

			//���̃N���X�^�̎����Ă�R���g���[���|�C���g��Ԃ�
			//�R���g���[���|�C���g���Ăȁ[��񂾁I
			//�C���f�b�N�X�o�b�t�@�̎w�������f�[�^�炵��
			const int ControlPointIndecesCount = FBXCluster->GetControlPointIndicesCount();
			for (int ControlPointIndecesIndex = 0; ControlPointIndecesIndex < ControlPointIndecesCount; ++ControlPointIndecesIndex)
			{
				//�R���g���[���|�C���g�̃C���f�b�N�X�̔z��ւ̃|�C���^�[��Ԃ��Ă�
				int ControlPointIndex = FBXCluster->GetControlPointIndices()[ControlPointIndecesIndex];
				//�R���g���[���|�C���g�̃E�F�C�g�̔z����擾
				double ControlPointWeight = FBXCluster->GetControlPointWeights()[ControlPointIndecesIndex];

				BoneInfluence& BoneInfluence = BoneInfluences.at(ControlPointIndex).emplace_back();
				BoneInfluence.BoneIndex = static_cast<uint32_t>(ClusterIndex);
				BoneInfluence.BoneWeight = static_cast<float>(ControlPointWeight);
			}
		}
	}
}



//int SkinnedMesh::RayPick(const DirectX::XMFLOAT3& startPosition, const DirectX::XMFLOAT3& endPosition, DirectX::XMFLOAT3* outPosition, DirectX::XMFLOAT3* outNormal, float* outLength)
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
//	for (const auto& it : faces) {
//		// �ʒ��_�擾   
//		DirectX::XMVECTOR a = DirectX::XMLoadFloat3(&it.Position[0]);
//		DirectX::XMVECTOR b = DirectX::XMLoadFloat3(&it.Position[1]);
//		DirectX::XMVECTOR c = DirectX::XMLoadFloat3(&it.Position[2]);
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
//		ret = it.MaterialIndex;
//	}
//	if (ret != -1) {
//		DirectX::XMStoreFloat3(outPosition, position);
//		DirectX::XMStoreFloat3(outNormal, normal);
//	}
//	//�ł��߂��q�b�g�ʒu�܂ł̋���  
//	*outLength = neart;
//	return ret;
//}


SkinnedMesh::SkinnedMesh(ID3D11Device* Device, const char* FBX_Filename,
	bool Triangulate, float SamplingRate)
{
	/*30���*/
	//������FBX_Filename�̊g���q��cereal�ɕς��āA���̃t�@�C�������݂��Ă���ꍇ�ɂ͂������̃V���A���C�Y���ꂽ����
	//���[�h����A�����Ǒ��݂��Ȃ��ꍇ�ɂ�fbx�t�@�C���̂ق����烍�[�h����悤�ɂ���
	//�����ǁA�V���A���C�Y���ꂽ�f�[�^�͌��f�[�^�̂ق��̕ύX�����m�ł��Ȃ��̂ŁA�ύX���ꂽ�ꍇ�͂܂��V�����f�[�^����蒼���K�v������
	filesystem::path CerealFilename(FBX_Filename);
	CerealFilename.replace_extension("cereal");

	//�������łɃV���A���C�Y���ꂽ�t�@�C�����������ꍇ�͂����瑤�œǂݍ���ł�
	if (filesystem::exists(CerealFilename.c_str()))
	{
		ifstream IFS(CerealFilename.c_str(), ios::binary);
		cereal::BinaryInputArchive deserialization(IFS);
		deserialization(SceneView, Meshes, Materials, AnimationClips);
	}
	else
	{
		//�V���A���C�Y���ꂽ�t�@�C�����Ȃ�������

		//�����FbxManager���쐬
		//����͌�ō��Scene�Ƃ�Import���쐬���邽�߂ɕK�v�ȊǗ��N���X
		FbxManager* FBXManager = FbxManager::Create();

		//FbxImporter�̍쐬
		//������FbxManager�̃|�C���^�[�Ƃ���Importer�̖��O
		//�����Fbx�t�@�C����Import���邽�߂̃p�[�T�[�N���X�A�f�[�^�����b�V���Ƃ��J�����̏��ɕ����Ă����
		FbxImporter* FBXImporter = FbxImporter::Create(FBXManager, "");

		//FbxScene���쐬
		//������FbxManager�̃|�C���^�[�Ƃ���Scene�̖��O
		//��ō����Importer����������Fbx�̃f�[�^��ۑ����邽�߂̃N���X
		FbxScene* FBXScene = FbxScene::Create(FBXManager, "");


		/*FBX�t�@�C����Open�̑����Initialize�Ńt�@�C�����J��*/
		_ASSERT_EXPR_A(FBXImporter->Initialize(FBX_Filename), FBXImporter->GetStatus().GetErrorString());

		/*Fbx�t�@�C����Import����*/
		//��������FbxDocument��FbxScene�̐e�N���X
		//����͕��������Ƃ��܂Ŋ܂܂�邩��R�X�g�������炵��
		//�������Ƀt���O�������Ă��ꂪ�񓯊��Ƃ��̃t���O�Ȃ̂ő����o���Ƃ��Ȃ�
		//�X���b�h���������Ƃ��őΉ����悤
		_ASSERT_EXPR_A(FBXImporter->Import(FBXScene), FBXImporter->GetStatus().GetErrorString());

		FbxGeometryConverter FBXConverter(FBXManager);
		if (Triangulate)
		{
			/*�����̏����́ADirectX�̓|���S���`��͎O�p�`�������1�|���S���ɂȂ��Ă�
			����������A�l�p�`�Ƃ��̑��p�`�̎��A�O�p�`�ɂ��鏈�����K�v�ŁA���̊֐��͂�������Ă�
			�����A�ϊ�����̂͂��ꑊ���̃R�X�g��������̂ŃO���t�B�b�N���őΉ��\�Ȃ炻�����ł�����������*/

			FBXConverter.Triangulate(FBXScene,
				true,	//�u���������邩�ǂ����̃t���O
				false);	//�Â��`���̃|���S�������������g�p(���ʌ݊��p�Ȃ̂Ńf�t�H��false)
			FBXConverter.RemoveBadPolygonsFromMeshes(FBXScene);	//���b�V���폜(�ڂ������t�@�����X�Ȃ�������������킩������ǋL���悤)
		}

		//�����_�c���c�H
		function<void(FbxNode*)> Traverse
		{ [&](FbxNode* FBXNode)
			{
				SceneSkin::Node& nd{SceneView.Nodes.emplace_back()};
				nd.Attribute = FBXNode->GetNodeAttribute() ?
					FBXNode->GetNodeAttribute()->GetAttributeType() : FbxNodeAttribute::EType::eUnknown;
				nd.Name = FBXNode->GetName();
				nd.UniqueID = FBXNode->GetUniqueID();
				nd.ParentIndex = SceneView.IndexOf(FBXNode->GetParent() ?
					FBXNode->GetParent()->GetUniqueID() : 0);
				for (int ChildIndex = 0; ChildIndex < FBXNode->GetChildCount(); ++ChildIndex)
				{
					Traverse(FBXNode->GetChild(ChildIndex));
				}
			}
		};
		Traverse(FBXScene->GetRootNode());
		//18��ڒǉ�
		FetchMeshes(FBXScene, Meshes);
		//19��ڒǉ�
		FetchMaterials(FBXScene, Materials);
		//25��ڒǉ� ���Ԃ�n�b���Ƃ̃t���[����؂����ĕ`�悵�Ă�H
		//float SamplingRate = 1;
		
		FetchAnimation(FBXScene, AnimationClips, SamplingRate);


#if 0
		for (const SceneSkin::Node& nd : SceneView.Nodes)
		{
			FbxNode* FBXNode = FBXScene->FindNodeByName(nd.Name.c_str());
			//�m�[�h�f�[�^���f�o�b�O�Ƃ��ďo�̓E�B���h�E�ɕ\������
			string NodeName = FBXNode->GetName();
			uint64_t	UID = FBXNode->GetUniqueID();
			uint64_t	ParentUID = FBXNode->GetParent() ? FBXNode->GetParent()->GetUniqueID() : 0;
			int32_t		Type = FBXNode->GetNodeAttribute() ? FBXNode->GetNodeAttribute()->GetAttributeType() : 0;

			stringstream DebugString;
			DebugString << NodeName << ":" << UID << ":" << ParentUID << ":" << Type << "\n";
			OutputDebugStringA(DebugString.str().c_str());
		}
#endif
		FBXManager->Destroy();
		/*30��ڒǉ�*/
		//ofstream...OutputFilestream(�o�͗p)
		// ofstream Variable("�t�@�C�����ɂ������z","�J���Ƃ��̃��[�h")
		//�������ɓ��ꂽ�����񂪃t�@�C�����ɂȂ�(���̏ꍇ����CerealFilename.c_str())
		//��������ios::binary�͂�����o�C�i�����[�h�ŊJ���悤�ɂ��Ă�
		ofstream OFS(CerealFilename.c_str(), ios::binary);
		cereal::BinaryOutputArchive serialization(OFS);
		serialization(SceneView, Meshes, Materials, AnimationClips);
	}
	//18��ڒǉ�
	CreateComObject(Device, FBX_Filename);
}

SkinnedMesh::SkinnedMesh(ID3D11Device* Device, const char* FBX_Filename, std::vector<std::string>& AnimationFilename, bool Triangulate, float SamplingRate)
{
	/*30���*/
	//������FBX_Filename�̊g���q��cereal�ɕς��āA���̃t�@�C�������݂��Ă���ꍇ�ɂ͂������̃V���A���C�Y���ꂽ����
	//���[�h����A�����Ǒ��݂��Ȃ��ꍇ�ɂ�fbx�t�@�C���̂ق����烍�[�h����悤�ɂ���
	//�����ǁA�V���A���C�Y���ꂽ�f�[�^�͌��f�[�^�̂ق��̕ύX�����m�ł��Ȃ��̂ŁA�ύX���ꂽ�ꍇ�͂܂��V�����f�[�^����蒼���K�v������
	filesystem::path CerealFilename(FBX_Filename);
	CerealFilename.replace_extension("cereal");

	//�������łɃV���A���C�Y���ꂽ�t�@�C�����������ꍇ�͂����瑤�œǂݍ���ł�
	if (filesystem::exists(CerealFilename.c_str()))
	{
		ifstream IFS(CerealFilename.c_str(), ios::binary);
		cereal::BinaryInputArchive deserialization(IFS);
		deserialization(SceneView, Meshes, Materials, AnimationClips);
	}
	else
	{
		//�V���A���C�Y���ꂽ�t�@�C�����Ȃ�������

		//�����FbxManager���쐬
		//����͌�ō��Scene�Ƃ�Import���쐬���邽�߂ɕK�v�ȊǗ��N���X
		FbxManager* FBXManager = FbxManager::Create();

		//FbxImporter�̍쐬
		//������FbxManager�̃|�C���^�[�Ƃ���Importer�̖��O
		//�����Fbx�t�@�C����Import���邽�߂̃p�[�T�[�N���X�A�f�[�^�����b�V���Ƃ��J�����̏��ɕ����Ă����
		FbxImporter* FBXImporter = FbxImporter::Create(FBXManager, "");

		//FbxScene���쐬
		//������FbxManager�̃|�C���^�[�Ƃ���Scene�̖��O
		//��ō����Importer����������Fbx�̃f�[�^��ۑ����邽�߂̃N���X
		FbxScene* FBXScene = FbxScene::Create(FBXManager, "");


		/*FBX�t�@�C����Open�̑����Initialize�Ńt�@�C�����J��*/
		_ASSERT_EXPR_A(FBXImporter->Initialize(FBX_Filename), FBXImporter->GetStatus().GetErrorString());

		/*Fbx�t�@�C����Import����*/
		//��������FbxDocument��FbxScene�̐e�N���X
		//����͕��������Ƃ��܂Ŋ܂܂�邩��R�X�g�������炵��
		//�������Ƀt���O�������Ă��ꂪ�񓯊��Ƃ��̃t���O�Ȃ̂ő����o���Ƃ��Ȃ�
		//�X���b�h���������Ƃ��őΉ����悤
		_ASSERT_EXPR_A(FBXImporter->Import(FBXScene), FBXImporter->GetStatus().GetErrorString());

		FbxGeometryConverter FBXConverter(FBXManager);
		if (Triangulate)
		{
			/*�����̏����́ADirectX�̓|���S���`��͎O�p�`�������1�|���S���ɂȂ��Ă�
			����������A�l�p�`�Ƃ��̑��p�`�̎��A�O�p�`�ɂ��鏈�����K�v�ŁA���̊֐��͂�������Ă�
			�����A�ϊ�����̂͂��ꑊ���̃R�X�g��������̂ŃO���t�B�b�N���őΉ��\�Ȃ炻�����ł�����������*/

			FBXConverter.Triangulate(FBXScene,
				true,	//�u���������邩�ǂ����̃t���O
				false);	//�Â��`���̃|���S�������������g�p(���ʌ݊��p�Ȃ̂Ńf�t�H��false)
			FBXConverter.RemoveBadPolygonsFromMeshes(FBXScene);	//���b�V���폜(�ڂ������t�@�����X�Ȃ�������������킩������ǋL���悤)
		}

		//�����_�c���c�H
		function<void(FbxNode*)> Traverse
		{ [&](FbxNode* FBXNode)
			{
				SceneSkin::Node& nd{SceneView.Nodes.emplace_back()};
				nd.Attribute = FBXNode->GetNodeAttribute() ?
					FBXNode->GetNodeAttribute()->GetAttributeType() : FbxNodeAttribute::EType::eUnknown;
				nd.Name = FBXNode->GetName();
				nd.UniqueID = FBXNode->GetUniqueID();
				nd.ParentIndex = SceneView.IndexOf(FBXNode->GetParent() ?
					FBXNode->GetParent()->GetUniqueID() : 0);
				for (int ChildIndex = 0; ChildIndex < FBXNode->GetChildCount(); ++ChildIndex)
				{
					Traverse(FBXNode->GetChild(ChildIndex));
				}
			}
		};
		Traverse(FBXScene->GetRootNode());
		//18��ڒǉ�
		FetchMeshes(FBXScene, Meshes);
		//19��ڒǉ�
		FetchMaterials(FBXScene, Materials);
		//25��ڒǉ� ���Ԃ�n�b���Ƃ̃t���[����؂����ĕ`�悵�Ă�H
		//float SamplingRate = 1;

		for (const std::string AnimationFilename : AnimationFilename)
		{
			AppendAnimation(AnimationFilename.c_str(), SamplingRate);
		}
		//FetchAnimation(FBXScene, AnimationClips, SamplingRate);


#if 0
		for (const SceneSkin::Node& nd : SceneView.Nodes)
		{
			FbxNode* FBXNode = FBXScene->FindNodeByName(nd.Name.c_str());
			//�m�[�h�f�[�^���f�o�b�O�Ƃ��ďo�̓E�B���h�E�ɕ\������
			string NodeName = FBXNode->GetName();
			uint64_t	UID = FBXNode->GetUniqueID();
			uint64_t	ParentUID = FBXNode->GetParent() ? FBXNode->GetParent()->GetUniqueID() : 0;
			int32_t		Type = FBXNode->GetNodeAttribute() ? FBXNode->GetNodeAttribute()->GetAttributeType() : 0;

			stringstream DebugString;
			DebugString << NodeName << ":" << UID << ":" << ParentUID << ":" << Type << "\n";
			OutputDebugStringA(DebugString.str().c_str());
		}
#endif
		FBXManager->Destroy();
		/*30��ڒǉ�*/
		//ofstream...OutputFilestream(�o�͗p)
		// ofstream Variable("�t�@�C�����ɂ������z","�J���Ƃ��̃��[�h")
		//�������ɓ��ꂽ�����񂪃t�@�C�����ɂȂ�(���̏ꍇ����CerealFilename.c_str())
		//��������ios::binary�͂�����o�C�i�����[�h�ŊJ���悤�ɂ��Ă�
		ofstream OFS(CerealFilename.c_str(), ios::binary);
		cereal::BinaryOutputArchive serialization(OFS);
		serialization(SceneView, Meshes, Materials, AnimationClips);
	}
	//18��ڒǉ�
	CreateComObject(Device, FBX_Filename);
}

void SkinnedMesh::FetchMeshes(FbxScene* FBXScene,
	vector<Mesh>& Meshes)
{
	for (const SceneSkin::Node& nd : SceneView.Nodes)
	{
		if (nd.Attribute != FbxNodeAttribute::EType::eMesh)
		{
			continue;
		}

		//�����œ���Ă閼�O�̃m�[�h�̍ŏ��̂Ƃ����擾
		FbxNode* FBXNode = FBXScene->FindNodeByName(nd.Name.c_str());
		//���̃m�[�h�̃��b�V�����Ƃ��Ă���
		FbxMesh* FBXMesh = FBXNode->GetMesh();

		Mesh& ms = Meshes.emplace_back();
		{
			ms.UniqueID = FBXMesh->GetNode()->GetUniqueID();
			ms.Name = FBXMesh->GetNode()->GetName();
			ms.NodeIndex = SceneView.IndexOf(ms.UniqueID);

			Face f;
			//22���
			vector<BoneInfluencePerControlPoint> BoneInfluence;
			FetchBoneInfluence(FBXMesh, BoneInfluence);
			//24���
			FetchSkeleton(FBXMesh, ms.BindPose);

			//20���(�������b�V���̑Ή�)
			vector<Mesh::Subset>& Subsets = ms.Subsets;
			const int MaterialCount = FBXMesh->GetNode()->GetMaterialCount();
			f.MaterialIndex = MaterialCount;

			Subsets.resize(MaterialCount > 0 ? MaterialCount : 1);
			for (int MaterialIndex = 0; MaterialIndex < MaterialCount; ++MaterialIndex)
			{
				const FbxSurfaceMaterial* FBXMaterial = FBXMesh->GetNode()->GetMaterial(MaterialIndex);
				Subsets.at(MaterialIndex).MaterialName = FBXMaterial->GetName();
				Subsets.at(MaterialIndex).Materrial_UniqueID = FBXMaterial->GetUniqueID();
			}
			if (MaterialCount > 0)
			{
				const int PolygonCount = FBXMesh->GetPolygonCount();
				//int PolygonIndex = 0;
				//�Ȃ�for�Œ�`������g���Ȃ���̌����ɓ��������̂�
				//�Ƃ肠��������Ȋ����ł���Ă݂�
				for (int PolygonIndex = 0; PolygonIndex < PolygonCount; ++PolygonIndex)
				{
					const int MaterialIndex = FBXMesh->GetElementMaterial()
						->GetIndexArray().GetAt(PolygonIndex);
					//�|���S�����J�E���g
					Subsets.at(MaterialIndex).IndexCount += 3;

				}
				uint32_t Offset = 0;
				for (Mesh::Subset& sb : Subsets)
				{
					sb.StartIndexLocation = Offset;
					Offset += sb.IndexCount;
					//���̃��[�v�Ŏg������subset��index�J�E���g��
					//Offset�ɓ���ĕۑ����Ă��玟�悤��0�ɖ߂�
					sb.IndexCount = 0;
				}
			}

			//�|���S�����𐔂���
			const int PolygonCount = FBXMesh->GetPolygonCount();
			//LL���ĉ����Ă����b...long long int�^�H�Ȃ񂩐������e�����Ƃ��̘b(���̏ꍇ����long long int��3���|���Ă�H)
			//�|���S���̒��_������3���|���Ă�̂��Ȃ�قǂȂ��c
			ms.Vertices.resize(PolygonCount * 3LL);
			ms.Indices.resize(PolygonCount * 3LL);

			FbxStringList UVNames;
			//�����̃W�I���g����UV�������Ă�H
			FBXMesh->GetUVSetNames(UVNames);
			const FbxVector4* ControlPoint = FBXMesh->GetControlPoints();

			for (int PolygonIndex = 0; PolygonIndex < PolygonCount; ++PolygonIndex)
			{
				//20��ڒǉ�
				const int MaterialIndex = MaterialCount > 0 ?
					FBXMesh->GetElementMaterial()->GetIndexArray().GetAt(PolygonIndex) : 0;
				Mesh::Subset& sb = Subsets.at(MaterialIndex);
				const uint32_t Offset = sb.StartIndexLocation + sb.IndexCount;

				for (int PositionInPolygon = 0; PositionInPolygon < 3; ++PositionInPolygon)
				{
					//���_�ԍ����@�|���S���̔ԍ�*3+�|�W�V�����̏ꏊ(�Ȃ񂩗����ł����������ǂȂ񂩂������ĂĂ��܂��܂Ƃ߂�Ȃ�)
					const int VertexIndex = PolygonIndex * 3 + PositionInPolygon;

					//���_�����擾
					Vertex vertex;
					{
						DirectX::XMMATRIX MeshTranceform = DirectX::XMLoadFloat4x4(&ms.DefaultGlobalTransform);

						//�C���f�b�N�X�t���̃|���S�����_��Ԃ�
						//�C���f�b�N�X�ƃ|���S���̈ʒu����Ƃ��Ă��Ă���𒸓_�ɓ����(?���܂����킩���ĂȂ�)
						const int PolygonVertex = FBXMesh->GetPolygonVertex(PolygonIndex, PositionInPolygon);
						vertex.Position = { static_cast<float>(ControlPoint[PolygonVertex][0]),
						static_cast<float>(ControlPoint[PolygonVertex][1]),
						static_cast<float>(ControlPoint[PolygonVertex][2]) };

						XMVECTOR LocalPosition = DirectX::XMLoadFloat3(&vertex.Position);
						XMVECTOR GlobalPosition = DirectX::XMVector3TransformCoord(LocalPosition, MeshTranceform);
						DirectX::XMStoreFloat3(&f.Position[PositionInPolygon], GlobalPosition);


						//22���(�{�[�����̒ǉ�)
						const BoneInfluencePerControlPoint& InfluencesPerControlPoint = BoneInfluence.at(PolygonVertex);
						//int Loopcount;
						for (size_t InfluencesIndex = 0; InfluencesIndex < InfluencesPerControlPoint.size(); ++InfluencesIndex)
						{
							vertex.BoneWeights[InfluencesIndex] = InfluencesPerControlPoint.at(InfluencesIndex).BoneWeight;
							vertex.BoneIndeces[InfluencesIndex] = InfluencesPerControlPoint.at(InfluencesIndex).BoneIndex;
							if (InfluencesIndex > 4)
							{
								break;
							}
						}
					}
					//�@�������擾
					if (FBXMesh->GetElementNormalCount() > 0)
					{
						//�W�I���g���v�f�̑������Ƃ��Ă��Ă��ꂪ�܂����������ɓ���
						FbxVector4 Normal;
						//�w�肵���|���S�����_�Ɋ֘A�Ђ���ꂽ�@�����擾����
						//PolygonIndex��PositionInPolygon�Ŗ@�����Ƃ��Ă����3�ڂ̈����Ɋi�[
						FBXMesh->GetPolygonVertexNormal(PolygonIndex, PositionInPolygon, Normal);
						vertex.Normal = { static_cast<float>(Normal[0]),
						static_cast<float>(Normal[1]),
						static_cast<float>(Normal[2]) };
					}

					//uv�����擾
					if (FBXMesh->GetElementUVCount() > 0)
					{
						FbxVector2 UV;
						bool Unmapped_UV;
						FBXMesh->GetPolygonVertexUV(PolygonIndex, PositionInPolygon,
							UVNames[0], UV, Unmapped_UV);
						vertex.Texcoord = { static_cast<float>(UV[0]),
						1.0f - static_cast<float>(UV[1]) };
						//������1��������Ă�͉̂摜���t�����ɕ`�悳���̂�h���ł�H
						//���ł����炱�������Ă݂���ǂ����낤
					}

					//29���
					//�@�����̎擾
					/*
					* GenerateTangentsData
					* ����̃��C���[��uvset�̐ڐ�f�[�^�𐶐�����
					* �������łɍ���Ă��肷��ꍇ��pOverwrite(bool�̒l)
					* ��false
					*/
					if (FBXMesh->GenerateTangentsData(0, false))
					{
						const FbxGeometryElementTangent* Tangent = FBXMesh->GetElementTangent();
						vertex.Tangent =
						{
							static_cast<float>(Tangent->GetDirectArray().GetAt(VertexIndex)[0]),
							static_cast<float>(Tangent->GetDirectArray().GetAt(VertexIndex)[1]),
							static_cast<float>(Tangent->GetDirectArray().GetAt(VertexIndex)[2]),
							static_cast<float>(Tangent->GetDirectArray().GetAt(VertexIndex)[3])
						};
					}

					ms.Vertices.at(VertexIndex) = move(vertex);
					//ms.Indices.at(VertexIndex) = VertexIndex;
					//20��ڒǉ�
					ms.Indices.at(static_cast<size_t>(Offset) + PositionInPolygon) = VertexIndex;
					sb.IndexCount++;
				}
				//21���
				//����́A���f���̈ʒu�p���X�P�[�������Ƃ��Ă��ꎩ�̂̍s��(�p���s��H)
				//��ϐ��ɂԂ�����ł�
				//�������Ȃ��ƕ������b�V���̃��f���̎��S�������ʒu(���_)�ɏo�Ă��܂��ĕςɂȂ��Ă��܂�����
				ms.DefaultGlobalTransform = FBXMatToDirMat4x4(FBXMesh->GetNode()->EvaluateGlobalTransform());

			}
			faces.push_back(f);

		}
		for (const Vertex& v : ms.Vertices)
		{
			ms.BoundingBox[0] =
			{
				min<float>(ms.BoundingBox[0].x,v.Position.x),
				min<float>(ms.BoundingBox[0].y,v.Position.y),
				min<float>(ms.BoundingBox[0].z,v.Position.z),
			};
			ms.BoundingBox[1] =
			{
				max<float>(ms.BoundingBox[1].x,v.Position.x),
				max<float>(ms.BoundingBox[1].y,v.Position.y),
				max<float>(ms.BoundingBox[1].z,v.Position.z),
			};
		}


	}

}

void SkinnedMesh::FetchMaterials(FbxScene* FBXScene,
	unordered_map<uint64_t, Material>& material)
{
	const size_t NodeCount = SceneView.Nodes.size();
	for (size_t NodeIndex = 0; NodeIndex < NodeCount; ++NodeIndex)
	{

		const SceneSkin::Node& Node = SceneView.Nodes.at(NodeIndex);
		const FbxNode* FBXNode = FBXScene->FindNodeByName(Node.Name.c_str());

		//�����Ń}�e���A�������擾
		const int MaterialCount = FBXNode->GetMaterialCount();

		//�}�e���A���������[�v(�����Ƃ�Index�g�������Current�̂ق����l�I�ɂ킩��₷���̂ł́H)
		for (int MaterialIndex = 0; MaterialIndex < MaterialCount; ++MaterialIndex)
		{
			//�}�e���A�����̂̏����擾
			const FbxSurfaceMaterial* FBXMaterial = FBXNode->GetMaterial(MaterialIndex);

			//29���
			//���������ɒǉ����邩�͂킩��Ȃ�
			const FbxSurfaceMaterial* FBXSurfaceMaterial = FBXNode->GetMaterial(MaterialIndex);

			Material mat;
			{
				//��łƂ����z��Material�\���̂ɂԂ�����
				mat.Name = FBXMaterial->GetName();
				mat.UniqueID = FBXMaterial->GetUniqueID();
			}
			//�v���p�e�B���擾
			FbxProperty FBXProperty;
			{
				FBXProperty = FBXMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
				//��łƂ����v���p�e�B���L�����ǂ���(isVaild()�Œ��ׂ�)
				//������FbxSurfaceMaterial��sDiffuse���w�肵�Ă�̂�Diffuse���A���Ă��Ă�
				if (FBXProperty.IsValid())
				{
					//Get�̓e���v���[�g(����<??>)�Ŏw�肵���^�̃f�[�^��Ԃ��Ă�
					const FbxDouble3 Color = FBXProperty.Get<FbxDouble3>();
					mat.Kd = { static_cast<float>(Color[0]),
					static_cast<float>(Color[1]),
					static_cast<float>(Color[2]),
					1.0f };

					const FbxFileTexture* FBXTexture = FBXProperty.GetSrcObject<FbxFileTexture>();
					//��������ǂ����̔��ʁH
					mat.TextureName[0] = FBXTexture ? FBXTexture->GetRelativeFileName() : "";
				}
#if 0
				FBXProperty = FBXMaterial->FindProperty(FbxSurfaceMaterial::sSpecular);
				//��łƂ����v���p�e�B���L�����ǂ���(isVaild()�Œ��ׂ�)
				if (FBXProperty.IsValid())
				{
					//Get�̓e���v���[�g(����<??>)�Ŏw�肵���^�̃f�[�^��Ԃ��Ă�
					const FbxDouble3 Color = FBXProperty.Get<FbxDouble3>();
					mat.Ks = { static_cast<float>(Color[0]),
					static_cast<float>(Color[1]),
					static_cast<float>(Color[2]),
					1.0f };

					const FbxFileTexture* FBXTexture = FBXProperty.GetSrcObject<FbxFileTexture>();
					//��������ǂ����̔��ʁH
					mat.TextureName[1] = FBXTexture ? FBXTexture->GetRelativeFileName() : "";
				}

#else
				FBXProperty = FBXSurfaceMaterial->FindProperty(FbxSurfaceMaterial::sNormalMap);
				if (FBXProperty.IsValid())
				{
					const FbxFileTexture* FileTexture = FBXProperty.GetSrcObject<FbxFileTexture>();
					mat.TextureName[1] = FileTexture ? FileTexture->GetRelativeFileName() : "";
				}
#endif // 0

				FBXProperty = FBXMaterial->FindProperty(FbxSurfaceMaterial::sAmbient);
				//��łƂ����v���p�e�B���L�����ǂ���(isVaild()�Œ��ׂ�)
				if (FBXProperty.IsValid())
				{
					//Get�̓e���v���[�g(����<??>)�Ŏw�肵���^�̃f�[�^��Ԃ��Ă�
					const FbxDouble3 Color = FBXProperty.Get<FbxDouble3>();
					mat.Ka = { static_cast<float>(Color[0]),
					static_cast<float>(Color[1]),
					static_cast<float>(Color[2]),
					1.0f };
					const FbxFileTexture* FBXTexture = FBXProperty.GetSrcObject<FbxFileTexture>();
					//��������ǂ����̔��ʁH
					mat.TextureName[2] = FBXTexture ? FBXTexture->GetRelativeFileName() : "";
				}
				material.emplace(mat.UniqueID, move(mat));
			}
		}
	}
}
//24���
void SkinnedMesh::FetchSkeleton(FbxMesh* FBXMesh, Skeleton& BindPose)
{
	const int DeformerCount = FBXMesh->GetDeformerCount(FbxDeformer::eSkin);
	for (int DeformerIndex = 0; DeformerIndex < DeformerCount; ++DeformerIndex)
	{
		FbxSkin* Skin = static_cast<FbxSkin*>(FBXMesh->GetDeformer(DeformerIndex, FbxDeformer::eSkin));
		/*
		* �O�X����łĂ��N���X�^(Cluster)�͂��낢��ȏ��(���_�C���f�b�N�X���{�[���A����ɑΉ������{�[���Ƃ��E�F�C�g)
		* �����܂Ƃ߂Ă�fbxsdk�������Ă�N���X�̂��Ƃ炵��
		* �������ꂪ�ǂ�����ǂ��܂�(���_�C���f�b�N�X1�ɂ��̐��������肨������)
		* ���Ă����̂͂킩���ĂȂ���������������ׂ��ق�����������
		*/
		const int ClusterCount = Skin->GetClusterCount();
		BindPose.Bones.resize(ClusterCount);
		for (int ClusterIndex = 0; ClusterIndex < ClusterCount; ++ClusterIndex)
		{
			FbxCluster* Cluster = Skin->GetCluster(ClusterIndex);

			Skeleton::Bone& Bone = BindPose.Bones.at(ClusterIndex);
			{
				/*
				* �����̖��O�擾�����ǂȂ��GetLink����񓥂ނ̂�
				* �ǂ����Ă����Link���ނ��ǒ��ږ��O�Ƃ��Ă���悤�ɂ͂��ĂȂ�����
				* �����������̂�����z�肳��ĂȂ��̂����H
				* ���r���h�I������璼�ږ��O�Ƃ��Ă݂���ǂ��ł���
				*/

				//�����ł��炩���ߍ���Ēu�����Ƃŏ��������R�X�g�����炷�H
				FbxNode* ClusterLink = Cluster->GetLink();
#if 0
				Bone.name = Cluster->GetName();
#else
				//Bone.name	 = Cluster->GetLink()->GetName();
				Bone.name = ClusterLink->GetName();
#endif
				//Bone.UniqueID	 = Cluster->GetLink()->GetUniqueID();
				Bone.UniqueID = ClusterLink->GetUniqueID();
				//������̓C���f�b�N�X�ԍ���Ԃ��Ă�
				Bone.ParentIndex = BindPose.IndexOf(ClusterLink->GetParent()->GetUniqueID());
				Bone.NodeIndex = SceneView.IndexOf(Bone.UniqueID);

				/*
				* ReferenceGlobalInitPosition
				* ���f���̃��[�J�����W����O���[�o�����W�ɕϊ�����Ƃ��Ɏg��
				*/
				FbxAMatrix ReferenceGlobalInitPosition;
				//�����N���܂ރm�[�h�ɕR�Â���ꂽ�s����擾���Ă����ϊ����Ċi�[���Ă�H
				Cluster->GetTransformMatrix(ReferenceGlobalInitPosition);


				/*
				* ClusterFlobalInitPosition
				* �{�[���̃��[�J�����W���烏�[���h���W�ɕϊ����邽�߂Ɏg��
				*/
				FbxAMatrix ClusterGlobalInitPosition;
				//�{�[���̏����p�����擾(�f�t�H���g�̍s����擾)
				//���O�����ʂ��Ă邯�ǑS���Ⴄ��Ȃ̂Œ���
				Cluster->GetTransformLinkMatrix(ClusterGlobalInitPosition);

				/*
				* Matrices(�s��)��ColumnMajor�X�L�[����p���Ē�`�����
				* ColumnMajor�X�L�[��...�s��̊i�[���@Column(�c���珇�ԂɊi�[)
				*
				* FbxAMatrix���p���s���\���ꍇ�͍Ō�̍s(Row)�͈ړ��̕�����\��
				* ���f����Ԃ��烏�[���h���W��Position��ϊ�����OffsetTransform�s����쐬
				* OffsetTransform�̓I�t�Z�b�g�s��(Offset..�΂�)
				*/
				Bone.OffsetTransform = FBXMatToDirMat4x4(ClusterGlobalInitPosition.Inverse() *
					ReferenceGlobalInitPosition);	//Inverse�͋t�s����o���Ă�
			}
		}
	}
}


//25���
void SkinnedMesh::FetchAnimation(FbxScene* FBXScene, vector<Animation>& AnimationClips, float SamplingRate)
{
	//�A�j���[�V�������i�[����z��
	FbxArray<FbxString*> AnimationStackNames;
	//�V�[������A�j���[�V�����ꗗ���擾���āA�����Ɋi�[����
	FBXScene->FillAnimStackNameArray(AnimationStackNames);
	const int AnimationStackCount = AnimationStackNames.GetCount();

	for (int AnimationStackIndex = 0; AnimationStackIndex < AnimationStackCount; ++AnimationStackIndex)
	{
		Animation& AnimationClip = AnimationClips.emplace_back();
		AnimationClip.Name = AnimationStackNames[AnimationStackIndex]->Buffer();

		//<Template>�̃����o�[������
		FBXScene->SetCurrentAnimationStack(FBXScene
			->FindMember<FbxAnimStack>(AnimationClip.Name.c_str()));

		//�V�[�����̃O���[�o���ݒ���Q�Ƃ��āA�^�C�����[�h���Ƃ��Ă���
		//�^�C�����[�h�ɂ��Ă̏ڂ����L�q���Ȃ���������ׂ��������Ȃ��c���߂�c
		const FbxTime::EMode TimeMode = FBXScene->GetGlobalSettings().GetTimeMode();

		FbxTime OneSecond;
		//����SetTime�Ŏ�����ݒ肷��
		OneSecond.SetTime(0, 0, 1,//�����͎����b��^���Ă�
			0,					//�t���[����
			0,					//
			TimeMode);			//�^�C�����[�h
		//��������������邯�ǃf�t�H���g�̒l�����߂��Ă�̂ō��͏����Ȃ�
		//�J�X�^���t���[�����[�g�̒l

		AnimationClip.SamplingRate = SamplingRate > 0 ?
			SamplingRate : static_cast<float>(OneSecond.GetFrameRate(TimeMode));
		const FbxTime SamplingInterval = static_cast<FbxLongLong>(OneSecond.Get() / AnimationClip.SamplingRate);
		const FbxTakeInfo* TakeInfo = FBXScene->GetTakeInfo(AnimationClip.Name.c_str());
		const FbxTime StartTime = TakeInfo->mLocalTimeSpan.GetStart();
		const FbxTime StopTime = TakeInfo->mLocalTimeSpan.GetStop();

		//�A�j���[�V���������邮�邷��Ƃ���
		//Time��Start������Interval(�Ԃ̃t���[���H)�𑫂��ĉ񂵂Ă� 
		for (FbxTime Time = StartTime; Time < StopTime; Time += SamplingInterval)
		{
			Animation::KeyFrame& KeyFlame = AnimationClip.Sequence.emplace_back();

			const size_t NodeCount = SceneView.Nodes.size();
			KeyFlame.Nodes.resize(NodeCount);
			for (size_t NodeIndex = 0; NodeIndex < NodeCount; ++NodeIndex)
			{
				//�m�[�h�̖��O���Ƃ��Ă��Ă�����������Ă�(�Ȃ�ł���̂Ɍ���������c�H)
				FbxNode* FBXNode = FBXScene->FindNodeByName(SceneView.Nodes.at(NodeIndex).Name.c_str());
				if (FBXNode)
				{
					Animation::KeyFrame::Node& Nd = KeyFlame.Nodes.at(NodeIndex);
					/*
					* GlobalTransform
					* �m�[�h�̍��W�ƃ��[���h���W�ɑ΂���m�[�h�̑Ή�����ϊ��s��
					*/
					Nd.GlobalTransform =
						FBXMatToDirMat4x4(FBXNode->EvaluateGlobalTransform(Time));

					//27���
					/*
					* LocalTransform
					* �e�̃��[�J�����W����ɂ����A�m�[�h�̕ϊ��s��
					*/
					//�w�肵������(������Time)�̃m�[�h�̃��[�J���ϊ��s����Ƃ��Ă���
					const FbxAMatrix& LocalTransform = FBXNode->EvaluateLocalTransform(Time);
					{
						//�������Get~��~�̂Ƃ��̕����ɂ���ĂƂ��Ă���s�񂪕ς�邩�炻�����ӎ����悤
						Nd.Scaling = FBXDobleToXMFLOAT3(LocalTransform.GetS());
						//����Rotation������GetR���Ǝv������GetQ�ŃN�H�[�^�j�I�����Ƃ��Ă��Ă�
						//����A�j���[�V�����̃X�P�[�����O���N�H�[�^�j�I���ł���Ă邩��H
						Nd.Rotation = FBXDobleToXMFLOAT4(LocalTransform.GetQ());
						Nd.Translation = FBXDobleToXMFLOAT3(LocalTransform.GetT());
					}
				}
			}
		}
	}
	for (int AnimationStackIndex = 0; AnimationStackIndex < AnimationStackCount; ++AnimationStackIndex)
	{
		delete AnimationStackNames[AnimationStackIndex];
	}
}

void SkinnedMesh::UpdateAnimation(Animation::KeyFrame& KeyFlame)
{
	size_t NodeCount = KeyFlame.Nodes.size();
	for (size_t NodeIndex = 0; NodeIndex < NodeCount; ++NodeIndex)
	{
		Animation::KeyFrame::Node& nd = KeyFlame.Nodes.at(NodeIndex);
		XMMATRIX S = XMMatrixScaling(nd.Scaling.x, nd.Scaling.y, nd.Scaling.z);
		XMMATRIX R = XMMatrixRotationQuaternion(XMLoadFloat4(&nd.Rotation));
		XMMATRIX T = XMMatrixTranslation(nd.Translation.x, nd.Translation.y, nd.Translation.z);

		int64_t ParentIndex = SceneView.Nodes.at(NodeIndex).ParentIndex;
		//XMMatrixIdentity ���̒P�ʍs���Ԃ��Ă�
		XMMATRIX P = ParentIndex < 0 ? XMMatrixIdentity() :
			XMLoadFloat4x4(&KeyFlame.Nodes.at(ParentIndex).GlobalTransform);
		XMStoreFloat4x4(&nd.GlobalTransform, S * R * T * P);//������P���|����̂͐e�q�t�����邽��
	}
}

bool SkinnedMesh::AppendAnimation(const char* AnimationFilename, float SamplingRate)
{
	//�}�l�[�W��(�Ǘ�������̍쐬,�ŏ���ւ�ł�������)
	FbxManager* FBXManager = FbxManager::Create();
	FbxScene* FBXScene = FbxScene::Create(FBXManager, "");
	FbxImporter* FBXImporter = FbxImporter::Create(FBXManager, "");

	//�ł������ǂ����̃`�F�b�N
	_ASSERT_EXPR_A(FBXImporter->Initialize(AnimationFilename),
		FBXImporter->GetStatus().GetErrorString());
	_ASSERT_EXPR_A(FBXImporter->Import(FBXScene),
		FBXImporter->GetStatus().GetErrorString());
	FetchAnimation(FBXScene, AnimationClips, SamplingRate);
	//�p�ς݂Ȃ̂ŎE��
	FBXManager->Destroy();

	return true;
}

void SkinnedMesh::BlendAnimation(const Animation::KeyFrame* KeyFlames[2], float Factor, Animation::KeyFrame& KeyFlame)
{
	size_t NodeCount = KeyFlames[0]->Nodes.size();
	KeyFlame.Nodes.resize(NodeCount);

	for (size_t NodeIndex = 0; NodeIndex < NodeCount; ++NodeIndex)
	{
		//Scaling(�g��k��)
		XMVECTOR S[2] =
		{
			XMLoadFloat3(&KeyFlames[0]->Nodes.at(NodeIndex).Scaling),
			XMLoadFloat3(&KeyFlames[1]->Nodes.at(NodeIndex).Scaling)
		};
		XMStoreFloat3(&KeyFlame.Nodes.at(NodeIndex).Scaling,
			XMVectorLerp(S[0], S[1], Factor));
		/*
		* XMVectorLerp(XMVECTOR,XMVECTOR,float)
		* �������Ƒ������̃x�N�g���Ԃ���`�⊮����A2�N�Ƃ��̐��w�ł������
		* ��O�����͕�ԌW��
		*/

		//Rotation(��]�AQuartenion�Ȃ̂Œ���)
		XMVECTOR R[2] =
		{
			XMLoadFloat4(&KeyFlames[0]->Nodes.at(NodeIndex).Rotation),
			XMLoadFloat4(&KeyFlames[1]->Nodes.at(NodeIndex).Rotation)
		};
		XMStoreFloat4(&KeyFlame.Nodes.at(NodeIndex).Rotation,
			XMQuaternionSlerp(R[0], R[1], Factor));
		/*
		* XMQuaternionSlerp(XMVECTOR,XMVECTOR,float)
		* ��ŏ�����Slerp�֐���Quarternion��
		* �߂�l�͎l�����ɂȂ��Ă�
		* ���Ȃ݂Ɉ����ǂ��炩(�������͂ǂ����)�P�ʎl��������Ȃ��Ƃ��͌��ʂ̕�Ԃ͖���`�ɂȂ�
		*/

		//Translation(�p��)
		XMVECTOR T[2] =
		{
			XMLoadFloat3(&KeyFlames[0]->Nodes.at(NodeIndex).Translation),
			XMLoadFloat3(&KeyFlames[1]->Nodes.at(NodeIndex).Translation)
		};
		XMStoreFloat3(&KeyFlame.Nodes.at(NodeIndex).Translation,
			XMVectorLerp(T[0], T[1], Factor));
	}
}

void SkinnedMesh::CreateComObject(ID3D11Device* Device,
	const char* FBX_Filename)
{
	HRESULT hr = S_OK;
	for (Mesh& ms : Meshes)
	{
		D3D11_BUFFER_DESC BufferDesc;
		D3D11_SUBRESOURCE_DATA SubresourceData;
		{
			//���_�o�b�t�@�̐ݒ�
			BufferDesc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * ms.Vertices.size());
			BufferDesc.Usage = D3D11_USAGE_DEFAULT;
			BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			BufferDesc.CPUAccessFlags = 0;
			BufferDesc.MiscFlags = 0;
			BufferDesc.StructureByteStride = 0;
			SubresourceData.pSysMem = ms.Vertices.data();
			SubresourceData.SysMemPitch = 0;
			SubresourceData.SysMemSlicePitch = 0;
			hr = Device->CreateBuffer(&BufferDesc, &SubresourceData,
				ms.VertexBuffer.ReleaseAndGetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			//�C���f�b�N�X�o�b�t�@�̐ݒ�
			BufferDesc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * ms.Indices.size());
			BufferDesc.Usage = D3D11_USAGE_DEFAULT;
			BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			SubresourceData.pSysMem = ms.Indices.data();
			hr = Device->CreateBuffer(&BufferDesc, &SubresourceData,
				ms.IndexBuffer.ReleaseAndGetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}

#if 1
		//��������Ȃ��Ȃ������炱���ŎE���H
		ms.Vertices.clear();
		ms.Indices.clear();
#endif // 1
	}

	D3D11_INPUT_ELEMENT_DESC InputElementDesc[]
	{
		////�ق�Ƃɂ����̃A���C�����g�͊ԈႦ�Ȃ��悤�ɂ��悤�I�I�I�I�I�}�W�ŁI�I�I�I�I�I
		{ "POSITION",	0,  DXGI_FORMAT_R32G32B32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "NORMAL",		0,	DXGI_FORMAT_R32G32B32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		//29���
		{ "TANGENT",	0,	DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "TEXCOORD",	0,	DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		//22��ڒǉ�
		{ "WEIGHTS",	0,	DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "BONES",		0,	DXGI_FORMAT_R32G32B32A32_UINT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},

		//{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//// UNIT.17
		//{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//// UNIT.20
		//{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//{ "BONES", 0, DXGI_FORMAT_R32G32B32A32_SINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	create_vs_from_cso(Device, "Shaders/Phong_vs.cso", VertexShader.GetAddressOf(), InputLayout.GetAddressOf(), InputElementDesc, ARRAYSIZE(InputElementDesc));
	create_ps_from_cso(Device, "Shaders/Phong_ps.cso", PixelShader.GetAddressOf());

	/*create_vs_from_cso(Device, "Shaders/skinned_mesh_vs.cso", VertexShader.GetAddressOf(), InputLayout.GetAddressOf(), InputElementDesc, ARRAYSIZE(InputElementDesc));
	create_ps_from_cso(Device, "Shaders/skinned_mesh_ps.cso", PixelShader.GetAddressOf());*/

	//�Ȃ��Release���Ă�̂��킩��Ȃ�
	//FBX�Ɍ��X���������Ă����Ă邩�炩�킩��Ȃ����ǉ��Ƃ������Ȃ��c�Ȃ̂Ŋ撣���Ă���������

#if 1
	//D3D11_RASTERIZER_DESC rasterizerDesc = {};
	//rasterizerDesc.FillMode = D3D11_FILL_SOLID; //D3D11_FILL_WIREFRAME, D3D11_FILL_SOLID
	//rasterizerDesc.CullMode = D3D11_CULL_BACK; //D3D11_CULL_NONE, D3D11_CULL_FRONT, D3D11_CULL_BACK   
	//rasterizerDesc.FrontCounterClockwise = FALSE;
	//rasterizerDesc.DepthBias = 0;
	//rasterizerDesc.DepthBiasClamp = 0;
	//rasterizerDesc.SlopeScaledDepthBias = 0;
	//rasterizerDesc.DepthClipEnable = TRUE;
	//rasterizerDesc.ScissorEnable = FALSE;
	//rasterizerDesc.MultisampleEnable = FALSE;
	//rasterizerDesc.AntialiasedLineEnable = FALSE;
	//hr = Device->CreateRasterizerState(&rasterizerDesc, rasterizerStates[0].GetAddressOf());
	//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	//depthStencilDesc.DepthEnable = TRUE;
	//depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	//depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	//depthStencilDesc.StencilEnable = FALSE;
	//depthStencilDesc.StencilReadMask = 0xFF;
	//depthStencilDesc.StencilWriteMask = 0xFF;
	//depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	//depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	//depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	//depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	//depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	//depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	//depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	//depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	//hr = Device->CreateDepthStencilState(&depthStencilDesc, depthStencilState.GetAddressOf());
	//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


#endif

	/*������mtl�t�@�C���Ƃ����Ȃ��������̃_�~�[�]�X��`���H*/
	for (unordered_map<uint64_t, Material>::iterator itr = Materials.begin();
		itr != Materials.end(); ++itr)
	{
		//29���
		for (size_t TextureIndex = 0; TextureIndex < 2; ++TextureIndex)
		{
			//�������炿����Əo�Ă���second��map�ɕۑ�����Ă���ۂ̒l���Ƃ��Ă�
			//���Ȃ݂�first��key�̒l���Ƃ��Ă�
			/*itr->first...key�̒l itr->second...����key�ɕۑ�����Ă�l*/
			if (itr->second.TextureName[TextureIndex].size() > 0)
			{
				filesystem::path Path = FBX_Filename;
				Path.replace_filename(itr->second.TextureName[TextureIndex]);
				D3D11_TEXTURE2D_DESC Texture2DDesc;
				load_texture_from_file(Device, Path.c_str(),
					itr->second.ShaderResourceView[TextureIndex].GetAddressOf(), &Texture2DDesc);
			}
			else
			{
				make_dummy_texture(Device,
					itr->second.ShaderResourceView[TextureIndex].GetAddressOf());
			}
		}
	}



	D3D11_BUFFER_DESC BufferDesc;
	{
		BufferDesc.ByteWidth = sizeof(Constant);
		BufferDesc.Usage = D3D11_USAGE_DEFAULT;
		BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		BufferDesc.CPUAccessFlags = 0;
		BufferDesc.MiscFlags = 0;
		BufferDesc.StructureByteStride = 0;

		hr = Device->CreateBuffer(&BufferDesc, nullptr,
			Constantbuffer.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
}

//void SkinnedMesh::Render(ID3D11DeviceContext* DeviceContext,
//	const XMFLOAT4X4& World, const XMFLOAT4& MaterialColor,
//	const Animation::KeyFrame* KeyFlame)
//{
//	for (const Mesh& ms : Meshes)
//	{
//		uint32_t Stride = sizeof(Vertex);
//		uint32_t Offset = 0;
//		DeviceContext->IASetVertexBuffers(0, 1, ms.VertexBuffer.GetAddressOf(),
//			&Stride, &Offset);
//		DeviceContext->IASetIndexBuffer(ms.IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
//		DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//		DeviceContext->IASetInputLayout(InputLayout.Get());
//
//		DeviceContext->VSSetShader(VertexShader.Get(), nullptr, 0);
//		DeviceContext->PSSetShader(PixelShader.Get(), nullptr, 0);
//
//		/*test*/
//		//DeviceContext->OMSetDepthStencilState(depthStencilState.Get(), 1);
//		//DeviceContext->RSSetState(rasterizerStates[0].Get());
//
//		Constant Data;
//		if (KeyFlame && KeyFlame->Nodes.size() > 0)
//		{
//			const Animation::KeyFrame::Node& msnd = KeyFlame->Nodes.at(ms.NodeIndex);
//
//			XMStoreFloat4x4(&Data.World,
//				XMLoadFloat4x4(&msnd.GlobalTransform) * XMLoadFloat4x4(&World));
//			const size_t BoneCount = ms.BindPose.Bones.size();
//			_ASSERT_EXPR((BoneCount < MAXBONES),
//				L"The value of the 'bone_count' has exceeded MAX_BONES.");
//			//�����̃{�[���̐����{�[���̍ő�l�𒴂�������Ă����G���[
//
//			for (int BoneIndex = 0; BoneIndex < BoneCount; ++BoneIndex)
//			{
//				const Skeleton::Bone& bn = ms.BindPose.Bones.at(BoneIndex);
//				const Animation::KeyFrame::Node& BoneNode = KeyFlame->Nodes.at(bn.NodeIndex);
//				XMStoreFloat4x4(&Data.BoneTransform[BoneIndex],
//					XMLoadFloat4x4(&bn.OffsetTransform) *
//					XMLoadFloat4x4(&BoneNode.GlobalTransform) *
//					XMMatrixInverse(nullptr, XMLoadFloat4x4(&ms.DefaultGlobalTransform)));
//			}
//		}
//		else
//		{
//			XMStoreFloat4x4(&Data.World,
//				XMLoadFloat4x4(&ms.DefaultGlobalTransform) * XMLoadFloat4x4(&World));
//			for (size_t BoneIndex = 0; BoneIndex < MAXBONES; ++BoneIndex)
//			{
//				Data.BoneTransform[BoneIndex] =
//				{
//					1.0f,.0f,.0f,.0f,
//					.0f,1.0f,.0f,.0f,
//					.0f,.0f,1.0f,.0f,
//					.0f,.0f,.0f,1.0f
//				};
//			}
//		}
//
//		//Data.World = World;
//		//21���
//		//XMStoreFloat4x4(&Data.World,
//		//	XMLoadFloat4x4(&ms.DefaultGlobalTransform) * XMLoadFloat4x4(&World));
//		//26���  �Ȃ�Data���̂̍s��ς���ĂȂ��ȁH KeyFlame���ςȒl�͌����Ă�H
//		//const Animation::KeyFrame::Node& msnd = KeyFlame->Nodes.at(ms.NodeIndex);
//		//XMStoreFloat4x4(&Data.World,
//		//	XMLoadFloat4x4(&msnd.GlobalTransform) * XMLoadFloat4x4(&World));
//		/*
//		* �Ȃ�ŕς��������Ă����ƁA���[�h���Ă�I�u�W�F�N�g�̃A�j���[�V�����ō��W��p��(�s��)���ς���Ă�̂ŁA
//		* �I�u�W�F�N�g�������Ă�L�[�t���[������s����擾���Ȃ��ƁA�A�j���[�V�������Ă�悤�Ɍ����Ȃ�����
//
//		*����Ă邱�Ƃ͎擾����GlobalTransform�s���萔�o�b�t�@�̃��[���h�ϊ��s��ɍ������Ă�
//		*/
//
//
//		//const size_t BoneCount = ms.BindPose.Bones.size();
//		//for (int BoneIndex = 0; BoneIndex < BoneCount; ++BoneIndex)
//		//{
//		//	const Skeleton::Bone& bn = ms.BindPose.Bones.at(BoneIndex);
//		//	const Animation::KeyFrame::Node& BoneNode = KeyFlame->Nodes.at(bn.NodeIndex);
//		//	XMStoreFloat4x4(&Data.BoneTransform[BoneIndex],
//		//		XMLoadFloat4x4(&bn.OffsetTransform) *
//		//		XMLoadFloat4x4(&BoneNode.GlobalTransform) *
//		//		XMMatrixInverse(nullptr, XMLoadFloat4x4(&ms.DefaultGlobalTransform)));
//		//}
//#if 0
//		//23���
//		XMStoreFloat4x4(&Data.BoneTransform[0], XMMatrixIdentity());
//		//45�x���̂˂���������Ń{�[����[n]�Ԗڂɗ^���Ă�
//		XMStoreFloat4x4(&Data.BoneTransform[1],
//			XMMatrixRotationRollPitchYaw(.0f, .0f, XMConvertToRadians(+45)));
//		XMStoreFloat4x4(&Data.BoneTransform[2],
//			XMMatrixRotationRollPitchYaw(.0f, .0f, XMConvertToRadians(-45)));
//
//		/*
//		* �o�C���h�|�[�Y�g�����X�t�H�[��(BindPoseTransform)
//		* ���f����Ԃ��烏�[���h��Ԃւ̕Ԋ�
//		*/
//		XMMATRIX B[3];//Bone��B?
//		B[0] = XMLoadFloat4x4(&ms.BindPose.Bones.at(0).OffsetTransform);
//		B[1] = XMLoadFloat4x4(&ms.BindPose.Bones.at(1).OffsetTransform);
//		B[2] = XMLoadFloat4x4(&ms.BindPose.Bones.at(2).OffsetTransform);
//
//		/*
//		* �A�j���[�V�����{�[���g�����X�t�H�[��(AnimationBoneTransform)
//		* �{�[����Ԃ���e�{�[����Ԃւ̕Ԋ�(�e�q�t���]�X�̂�H)
//		*/
//		XMMATRIX A[3];
//		//�܂��AA0�̋�Ԃ��烂�f���̋�Ԃ֕ϊ�
//		A[0] = XMMatrixRotationRollPitchYaw(XMConvertToRadians(90.0f), 0.0f, 0.0f);
//		//����A1�̋�Ԃ���A0�̋�Ԃ֕ϊ�(A0���e)
//		A[1] = XMMatrixRotationRollPitchYaw(0, 0,
//			XMConvertToRadians(45)) * XMMatrixTranslation(0.0f, 2.0f, 0.0f);
//		//�Ō��A2�̋�Ԃ���A1�̋��
//		A[2] = XMMatrixRotationRollPitchYaw(0, 0,
//			XMConvertToRadians(-45)) * XMMatrixTranslation(0.0f, 2.0f, 0.0f);
//
//		//�e�q�t���Ƃ������˂��ꂽ����(Animation)�̉e�������̎q�{�[���ɗ^���Ȃ��Ƃ����Ȃ�����
//		//�e�̍s����|���Ă�
//		XMStoreFloat4x4(&Data.BoneTransform[0], B[0] * A[0]);
//		XMStoreFloat4x4(&Data.BoneTransform[1], B[1] * A[1] * A[0]);
//		XMStoreFloat4x4(&Data.BoneTransform[2], B[1] * A[2] * A[1] * A[0]);
//#endif 
//
//		//�e�N�X�`���֘A�͂�������
//		Data.MaterialColor = MaterialColor;
//
//		//20��ڒǉ���(subset���̕`���ǉ�)
//		for (const Mesh::Subset& sb : ms.Subsets)
//		{
//			if (sb.Materrial_UniqueID != 0)
//			{
//				const Material& mt = Materials.at(sb.Materrial_UniqueID);
//				if (Materials.size() > 0)
//				{
//					//19��ڒǉ�
//					//�e�N�X�`�������Ń}�e���A���݂̂Œ��F���Ă��炱���`���Ȃ��ƐF�����f����Ȃ�����
//					//XMStoreFloat4(&Data.MaterialColor,
//					//	XMLoadFloat4(&MaterialColor) * XMLoadFloat4(&Materials.cbegin()->second.Kd));
//					XMStoreFloat4(&Data.MaterialColor,
//						XMLoadFloat4(&MaterialColor) * XMLoadFloat4(&mt.Kd));
//
//					//19��ڒǉ�
//					//DeviceContext->PSSetShaderResources(0, 1,
//					//	Materials.cbegin()->second.ShaderResourceView[0].GetAddressOf());
//					DeviceContext->PSSetShaderResources(0, 1,
//						mt.ShaderResourceView[0].GetAddressOf());
//				}
//			}
//			else
//			{
//				//�����Ń_�~�[��ݒ肵����
//				DeviceContext->PSSetShaderResources(0, 1,
//					Dummy.GetAddressOf());
//
//			}
//			DeviceContext->UpdateSubresource(Constantbuffer.Get(), 0, 0, &Data, 0, 0);
//			DeviceContext->VSSetConstantBuffers(0, 1, Constantbuffer.GetAddressOf());
//
//			D3D11_BUFFER_DESC BufferDesc;
//			ms.IndexBuffer->GetDesc(&BufferDesc);
//			//DeviceContext->DrawIndexed(BufferDesc.ByteWidth / sizeof(uint32_t), 0, 0);
//			DeviceContext->DrawIndexed(sb.IndexCount, sb.StartIndexLocation, 0);
//		}
//	}
//}

//void SkinnedMesh::Render(ID3D11DeviceContext* DeviceContext,
//	const XMFLOAT4X4& WorldViewProjection,
//	const XMFLOAT4& lightdirection,
//	const XMFLOAT4& MaterialColor,
//	const Animation::KeyFrame* KeyFlame)
//{
//	for (const Mesh& ms : Meshes)
//	{
//		uint32_t Stride = sizeof(Vertex);
//		uint32_t Offset = 0;
//		DeviceContext->IASetVertexBuffers(0, 1, ms.VertexBuffer.GetAddressOf(),
//			&Stride, &Offset);
//		DeviceContext->IASetIndexBuffer(ms.IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
//		DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//		DeviceContext->IASetInputLayout(InputLayout.Get());
//
//		DeviceContext->VSSetShader(VertexShader.Get(), nullptr, 0);
//		DeviceContext->PSSetShader(PixelShader.Get(), nullptr, 0);
//
//		/*test*/
//		//DeviceContext->OMSetDepthStencilState(depthStencilState.Get(), 1);
//		//DeviceContext->RSSetState(rasterizerStates[0].Get());
//		//�A�j���[�V�����̐����܂킵�Ă�
//		static int i = 0;
//		if (i >= 70)
//		{
//			i = 0;
//		}
//			Constant Data;
//		if (KeyFlame && KeyFlame->Nodes.size() > 0)
//		{
//			//const Animation::KeyFrame::Node& msnd = KeyFlame->Nodes.at(ms.NodeIndex);
//			const Animation::KeyFrame::Node& msnd = KeyFlame->Nodes.at(i);
//			i++;
//			XMStoreFloat4x4(&Data.WorldViewProjection,
//				XMLoadFloat4x4(&msnd.GlobalTransform) * XMLoadFloat4x4(&WorldViewProjection));
//			/*XMStoreFloat4x4(&Data.World,
//				XMLoadFloat4x4(&msnd.GlobalTransform) *  XMLoadFloat4x4(&World));*/
//			const size_t BoneCount = ms.BindPose.Bones.size();
//			_ASSERT_EXPR((BoneCount < MAXBONES),
//				L"The value of the 'bone_count' has exceeded MAX_BONES.");
//			//�����̃{�[���̐����{�[���̍ő�l�𒴂�������Ă����G���[
//
//			for (int BoneIndex = 0; BoneIndex < BoneCount; ++BoneIndex)
//			{
//				const Skeleton::Bone& bn = ms.BindPose.Bones.at(BoneIndex);
//				const Animation::KeyFrame::Node& BoneNode = KeyFlame->Nodes.at(bn.NodeIndex);
//				XMStoreFloat4x4(&Data.BoneTransform[BoneIndex],
//					XMLoadFloat4x4(&bn.OffsetTransform) *
//					XMLoadFloat4x4(&BoneNode.GlobalTransform) *
//					XMMatrixInverse(nullptr, XMLoadFloat4x4(&ms.DefaultGlobalTransform)));
//			}
//		}
//		else
//		{
//			XMStoreFloat4x4(&Data.WorldViewProjection,
//				XMLoadFloat4x4(&ms.DefaultGlobalTransform) * XMLoadFloat4x4(&WorldViewProjection));
//			/*	XMStoreFloat4x4(&Data.World,
//					XMLoadFloat4x4(&ms.DefaultGlobalTransform) * XMLoadFloat4x4(&World));*/
//			for (size_t BoneIndex = 0; BoneIndex < MAXBONES; ++BoneIndex)
//			{
//				Data.BoneTransform[BoneIndex] =
//				{
//					1.0f,.0f,.0f,.0f,
//					.0f,1.0f,.0f,.0f,
//					.0f,.0f,1.0f,.0f,
//					.0f,.0f,.0f,1.0f
//				};
//			}
//		}
//
//		Data.LightDirection = lightdirection;
//		//Data.World = World;
//		//21���
//		//XMStoreFloat4x4(&Data.World,
//		//	XMLoadFloat4x4(&ms.DefaultGlobalTransform) * XMLoadFloat4x4(&World));
//		//26���  �Ȃ�Data���̂̍s��ς���ĂȂ��ȁH KeyFlame���ςȒl�͌����Ă�H
//		//const Animation::KeyFrame::Node& msnd = KeyFlame->Nodes.at(ms.NodeIndex);
//		//XMStoreFloat4x4(&Data.World,
//		//	XMLoadFloat4x4(&msnd.GlobalTransform) * XMLoadFloat4x4(&World));
//		/*
//		* �Ȃ�ŕς��������Ă����ƁA���[�h���Ă�I�u�W�F�N�g�̃A�j���[�V�����ō��W��p��(�s��)���ς���Ă�̂ŁA
//		* �I�u�W�F�N�g�������Ă�L�[�t���[������s����擾���Ȃ��ƁA�A�j���[�V�������Ă�悤�Ɍ����Ȃ�����
//
//		*����Ă邱�Ƃ͎擾����GlobalTransform�s���萔�o�b�t�@�̃��[���h�ϊ��s��ɍ������Ă�
//		*/
//
//
//		//const size_t BoneCount = ms.BindPose.Bones.size();
//		//for (int BoneIndex = 0; BoneIndex < BoneCount; ++BoneIndex)
//		//{
//		//	const Skeleton::Bone& bn = ms.BindPose.Bones.at(BoneIndex);
//		//	const Animation::KeyFrame::Node& BoneNode = KeyFlame->Nodes.at(bn.NodeIndex);
//		//	XMStoreFloat4x4(&Data.BoneTransform[BoneIndex],
//		//		XMLoadFloat4x4(&bn.OffsetTransform) *
//		//		XMLoadFloat4x4(&BoneNode.GlobalTransform) *
//		//		XMMatrixInverse(nullptr, XMLoadFloat4x4(&ms.DefaultGlobalTransform)));
//		//}
//#if 0
//		//23���
//		XMStoreFloat4x4(&Data.BoneTransform[0], XMMatrixIdentity());
//		//45�x���̂˂���������Ń{�[����[n]�Ԗڂɗ^���Ă�
//		XMStoreFloat4x4(&Data.BoneTransform[1],
//			XMMatrixRotationRollPitchYaw(.0f, .0f, XMConvertToRadians(+45)));
//		XMStoreFloat4x4(&Data.BoneTransform[2],
//			XMMatrixRotationRollPitchYaw(.0f, .0f, XMConvertToRadians(-45)));
//
//		/*
//		* �o�C���h�|�[�Y�g�����X�t�H�[��(BindPoseTransform)
//		* ���f����Ԃ��烏�[���h��Ԃւ̕Ԋ�
//		*/
//		XMMATRIX B[3];//Bone��B?
//		B[0] = XMLoadFloat4x4(&ms.BindPose.Bones.at(0).OffsetTransform);
//		B[1] = XMLoadFloat4x4(&ms.BindPose.Bones.at(1).OffsetTransform);
//		B[2] = XMLoadFloat4x4(&ms.BindPose.Bones.at(2).OffsetTransform);
//
//		/*
//		* �A�j���[�V�����{�[���g�����X�t�H�[��(AnimationBoneTransform)
//		* �{�[����Ԃ���e�{�[����Ԃւ̕Ԋ�(�e�q�t���]�X�̂�H)
//		*/
//		XMMATRIX A[3];
//		//�܂��AA0�̋�Ԃ��烂�f���̋�Ԃ֕ϊ�
//		A[0] = XMMatrixRotationRollPitchYaw(XMConvertToRadians(90.0f), 0.0f, 0.0f);
//		//����A1�̋�Ԃ���A0�̋�Ԃ֕ϊ�(A0���e)
//		A[1] = XMMatrixRotationRollPitchYaw(0, 0,
//			XMConvertToRadians(45)) * XMMatrixTranslation(0.0f, 2.0f, 0.0f);
//		//�Ō��A2�̋�Ԃ���A1�̋��
//		A[2] = XMMatrixRotationRollPitchYaw(0, 0,
//			XMConvertToRadians(-45)) * XMMatrixTranslation(0.0f, 2.0f, 0.0f);
//
//		//�e�q�t���Ƃ������˂��ꂽ����(Animation)�̉e�������̎q�{�[���ɗ^���Ȃ��Ƃ����Ȃ�����
//		//�e�̍s����|���Ă�
//		XMStoreFloat4x4(&Data.BoneTransform[0], B[0] * A[0]);
//		XMStoreFloat4x4(&Data.BoneTransform[1], B[1] * A[1] * A[0]);
//		XMStoreFloat4x4(&Data.BoneTransform[2], B[1] * A[2] * A[1] * A[0]);
//#endif 
//
//		//�e�N�X�`���֘A�͂�������
//		Data.MaterialColor = MaterialColor;
//
//
//		//20��ڒǉ���(subset���̕`���ǉ�)
//		for (const Mesh::Subset& sb : ms.Subsets)
//		{
//			if (sb.Materrial_UniqueID != 0)
//			{
//				const Material& mt = Materials.at(sb.Materrial_UniqueID);
//				if (Materials.size() > 0)
//				{
//					//19��ڒǉ�
//					//�e�N�X�`�������Ń}�e���A���݂̂Œ��F���Ă��炱���`���Ȃ��ƐF�����f����Ȃ�����
//					//XMStoreFloat4(&Data.MaterialColor,
//					//	XMLoadFloat4(&MaterialColor) * XMLoadFloat4(&Materials.cbegin()->second.Kd));
//					XMStoreFloat4(&Data.MaterialColor,
//						XMLoadFloat4(&MaterialColor) * XMLoadFloat4(&mt.Kd));
//
//					//19��ڒǉ�
//					//DeviceContext->PSSetShaderResources(0, 1,
//					//	Materials.cbegin()->second.ShaderResourceView[0].GetAddressOf());
//					DeviceContext->PSSetShaderResources(0, 1,
//						mt.ShaderResourceView[0].GetAddressOf());
//				}
//			}
//			else
//			{
//				//�����Ń_�~�[��ݒ肵����
//				DeviceContext->PSSetShaderResources(0, 1,
//					Dummy.GetAddressOf());
//
//			}
//			DeviceContext->UpdateSubresource(Constantbuffer.Get(), 0, 0, &Data, 0, 0);
//			DeviceContext->VSSetConstantBuffers(0, 1, Constantbuffer.GetAddressOf());
//			DeviceContext->PSSetConstantBuffers(0, 1, Constantbuffer.GetAddressOf());
//
//			D3D11_BUFFER_DESC BufferDesc;
//			ms.IndexBuffer->GetDesc(&BufferDesc);
//			//DeviceContext->DrawIndexed(BufferDesc.ByteWidth / sizeof(uint32_t), 0, 0);
//			DeviceContext->DrawIndexed(sb.IndexCount, sb.StartIndexLocation, 0);
//		}
//	}
//}

void SkinnedMesh::Render(ID3D11DeviceContext* DeviceContext,
	const XMFLOAT4X4& WorldViewProjection,
	const XMFLOAT4X4& World,
	const XMFLOAT4& lightdirection, const XMFLOAT4& MaterialColor,
	const Animation::KeyFrame* KeyFlame/*25��ڒǉ�*/)
{
	for (const Mesh& ms : Meshes)
	{
		uint32_t Stride = sizeof(Vertex);
		uint32_t Offset = 0;
		DeviceContext->IASetVertexBuffers(0, 1, ms.VertexBuffer.GetAddressOf(),
			&Stride, &Offset);
		DeviceContext->IASetIndexBuffer(ms.IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		DeviceContext->IASetInputLayout(InputLayout.Get());

		DeviceContext->VSSetShader(VertexShader.Get(), nullptr, 0);
		DeviceContext->PSSetShader(PixelShader.Get(), nullptr, 0);

		/*test*/
		//DeviceContext->OMSetDepthStencilState(depthStencilState.Get(), 1);
		//DeviceContext->RSSetState(rasterizerStates[0].Get());
		//�A�j���[�V�����̐����܂킵�Ă�

		Constant Data;
		if (KeyFlame && KeyFlame->Nodes.size() > 0)
		{
			const Animation::KeyFrame::Node& msnd = KeyFlame->Nodes.at(ms.NodeIndex);
			//const Animation::KeyFrame::Node& msnd = KeyFlame->Nodes.at(i);
			XMStoreFloat4x4(&Data.WorldViewProjection,
				XMLoadFloat4x4(&msnd.GlobalTransform) * XMLoadFloat4x4(&WorldViewProjection));
			XMStoreFloat4x4(&Data.World,
				XMLoadFloat4x4(&msnd.GlobalTransform) *  XMLoadFloat4x4(&World));
			const size_t BoneCount = ms.BindPose.Bones.size();
			_ASSERT_EXPR((BoneCount < MAXBONES),
				L"The value of the 'bone_count' has exceeded MAX_BONES.");
			//�����̃{�[���̐����{�[���̍ő�l�𒴂�������Ă����G���[

			for (int BoneIndex = 0; BoneIndex < BoneCount; ++BoneIndex)
			{
				const Skeleton::Bone& bn = ms.BindPose.Bones.at(BoneIndex);
				const Animation::KeyFrame::Node& BoneNode = KeyFlame->Nodes.at(bn.NodeIndex);
				XMStoreFloat4x4(&Data.BoneTransform[BoneIndex],
					XMLoadFloat4x4(&bn.OffsetTransform) *
					XMLoadFloat4x4(&BoneNode.GlobalTransform) *
					XMMatrixInverse(nullptr, XMLoadFloat4x4(&ms.DefaultGlobalTransform)));
			}
		}
		else
		{
			XMStoreFloat4x4(&Data.WorldViewProjection,
				XMLoadFloat4x4(&ms.DefaultGlobalTransform) * XMLoadFloat4x4(&WorldViewProjection));
			XMStoreFloat4x4(&Data.World,
				XMLoadFloat4x4(&ms.DefaultGlobalTransform) * XMLoadFloat4x4(&World));
			for (size_t BoneIndex = 0; BoneIndex < MAXBONES; ++BoneIndex)
			{
				Data.BoneTransform[BoneIndex] =
				{
					1.0f,.0f,.0f,.0f,
					.0f,1.0f,.0f,.0f,
					.0f,.0f,1.0f,.0f,
					.0f,.0f,.0f,1.0f
				};
			}
		}

		Data.LightDirection = lightdirection;
		//Data.World = World;
		//21���
		//XMStoreFloat4x4(&Data.World,
		//	XMLoadFloat4x4(&ms.DefaultGlobalTransform) * XMLoadFloat4x4(&World));
		//26���  �Ȃ�Data���̂̍s��ς���ĂȂ��ȁH KeyFlame���ςȒl�͌����Ă�H
		//const Animation::KeyFrame::Node& msnd = KeyFlame->Nodes.at(ms.NodeIndex);
		//XMStoreFloat4x4(&Data.World,
		//	XMLoadFloat4x4(&msnd.GlobalTransform) * XMLoadFloat4x4(&World));
		/*
		* �Ȃ�ŕς��������Ă����ƁA���[�h���Ă�I�u�W�F�N�g�̃A�j���[�V�����ō��W��p��(�s��)���ς���Ă�̂ŁA
		* �I�u�W�F�N�g�������Ă�L�[�t���[������s����擾���Ȃ��ƁA�A�j���[�V�������Ă�悤�Ɍ����Ȃ�����

		*����Ă邱�Ƃ͎擾����GlobalTransform�s���萔�o�b�t�@�̃��[���h�ϊ��s��ɍ������Ă�
		*/


		//const size_t BoneCount = ms.BindPose.Bones.size();
		//for (int BoneIndex = 0; BoneIndex < BoneCount; ++BoneIndex)
		//{
		//	const Skeleton::Bone& bn = ms.BindPose.Bones.at(BoneIndex);
		//	const Animation::KeyFrame::Node& BoneNode = KeyFlame->Nodes.at(bn.NodeIndex);
		//	XMStoreFloat4x4(&Data.BoneTransform[BoneIndex],
		//		XMLoadFloat4x4(&bn.OffsetTransform) *
		//		XMLoadFloat4x4(&BoneNode.GlobalTransform) *
		//		XMMatrixInverse(nullptr, XMLoadFloat4x4(&ms.DefaultGlobalTransform)));
		//}
#if 0
		//23���
		XMStoreFloat4x4(&Data.BoneTransform[0], XMMatrixIdentity());
		//45�x���̂˂���������Ń{�[����[n]�Ԗڂɗ^���Ă�
		XMStoreFloat4x4(&Data.BoneTransform[1],
			XMMatrixRotationRollPitchYaw(.0f, .0f, XMConvertToRadians(+45)));
		XMStoreFloat4x4(&Data.BoneTransform[2],
			XMMatrixRotationRollPitchYaw(.0f, .0f, XMConvertToRadians(-45)));

		/*
		* �o�C���h�|�[�Y�g�����X�t�H�[��(BindPoseTransform)
		* ���f����Ԃ��烏�[���h��Ԃւ̕Ԋ�
		*/
		XMMATRIX B[3];//Bone��B?
		B[0] = XMLoadFloat4x4(&ms.BindPose.Bones.at(0).OffsetTransform);
		B[1] = XMLoadFloat4x4(&ms.BindPose.Bones.at(1).OffsetTransform);
		B[2] = XMLoadFloat4x4(&ms.BindPose.Bones.at(2).OffsetTransform);

		/*
		* �A�j���[�V�����{�[���g�����X�t�H�[��(AnimationBoneTransform)
		* �{�[����Ԃ���e�{�[����Ԃւ̕Ԋ�(�e�q�t���]�X�̂�H)
		*/
		XMMATRIX A[3];
		//�܂��AA0�̋�Ԃ��烂�f���̋�Ԃ֕ϊ�
		A[0] = XMMatrixRotationRollPitchYaw(XMConvertToRadians(90.0f), 0.0f, 0.0f);
		//����A1�̋�Ԃ���A0�̋�Ԃ֕ϊ�(A0���e)
		A[1] = XMMatrixRotationRollPitchYaw(0, 0,
			XMConvertToRadians(45)) * XMMatrixTranslation(0.0f, 2.0f, 0.0f);
		//�Ō��A2�̋�Ԃ���A1�̋��
		A[2] = XMMatrixRotationRollPitchYaw(0, 0,
			XMConvertToRadians(-45)) * XMMatrixTranslation(0.0f, 2.0f, 0.0f);

		//�e�q�t���Ƃ������˂��ꂽ����(Animation)�̉e�������̎q�{�[���ɗ^���Ȃ��Ƃ����Ȃ�����
		//�e�̍s����|���Ă�
		XMStoreFloat4x4(&Data.BoneTransform[0], B[0] * A[0]);
		XMStoreFloat4x4(&Data.BoneTransform[1], B[1] * A[1] * A[0]);
		XMStoreFloat4x4(&Data.BoneTransform[2], B[1] * A[2] * A[1] * A[0]);
#endif 

		//�e�N�X�`���֘A�͂�������
		Data.MaterialColor = MaterialColor;


		//20��ڒǉ���(subset���̕`���ǉ�)
		for (const Mesh::Subset& sb : ms.Subsets)
		{
			if (sb.Materrial_UniqueID != 0)
			{
				const Material& mt = Materials.at(sb.Materrial_UniqueID);
				if (Materials.size() > 0)
				{
					//19��ڒǉ�
					//�e�N�X�`�������Ń}�e���A���݂̂Œ��F���Ă��炱���`���Ȃ��ƐF�����f����Ȃ�����
					//XMStoreFloat4(&Data.MaterialColor,
					//	XMLoadFloat4(&MaterialColor) * XMLoadFloat4(&Materials.cbegin()->second.Kd));
					XMStoreFloat4(&Data.MaterialColor,
						XMLoadFloat4(&MaterialColor) * XMLoadFloat4(&mt.Kd));

					//19��ڒǉ�
					//DeviceContext->PSSetShaderResources(0, 1,
					//	Materials.cbegin()->second.ShaderResourceView[0].GetAddressOf());
					DeviceContext->PSSetShaderResources(0, 1,
						mt.ShaderResourceView[0].GetAddressOf());
				}
			}
			else
			{
				//�����Ń_�~�[��ݒ肵����
				DeviceContext->PSSetShaderResources(0, 1,
					Dummy.GetAddressOf());

			}
			DeviceContext->UpdateSubresource(Constantbuffer.Get(), 0, 0, &Data, 0, 0);
			DeviceContext->VSSetConstantBuffers(0, 1, Constantbuffer.GetAddressOf());
			DeviceContext->PSSetConstantBuffers(0, 1, Constantbuffer.GetAddressOf());

			D3D11_BUFFER_DESC BufferDesc;
			ms.IndexBuffer->GetDesc(&BufferDesc);
			//DeviceContext->DrawIndexed(BufferDesc.ByteWidth / sizeof(uint32_t), 0, 0);
			DeviceContext->DrawIndexed(sb.IndexCount, sb.StartIndexLocation, 0);
		}
	}
}

