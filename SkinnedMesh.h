#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>
#include <vector>
#include <string>
#include <fbxsdk.h>
#include <unordered_map>
//��map��unordered_map�͕ʕ�������C��t���悤�I�I

/*��������V���A���C�Y�n�܂邵
�ق����@�\�����炿���Ƃ�낤��*/
#include "cereal/archives/binary.hpp"
#include "cereal/types/memory.hpp"
#include "cereal/types/vector.hpp"
#include "cereal/types/set.hpp"
#include "cereal/types/unordered_map.hpp"
//���V���A���C�Y�]�X�ɂ��Ē��ׂ��b�A�������Ƃ��ɊJ���ā�
/*
* �O��Ƃ��āA�V���A���C�Y�Ƃ����Ăȁ[�񂾂��Ęb
* �V���A���C�Y(Serialize)..���񉻁A������(��������)���ꂼ��Ⴄ�Ӗ�
* �����ł͒��񉻂̂ق��Œ��ׂ�
* �����̗v�f�����ɕ��ׂ鑀��Ƃ������̂���
* ��̃V���A���C�Y���Č������璼�񉻂��Ă����̂ŕ\�����邱�Ƃ������炵��
*
* ���������Ɖ����ł��邩���Ă�����
* �v���O�������������Ă镡�G�ȃf�[�^�A�I�u�W�F�N�g�̂���u��(�X�i�b�v�V���b�g)
* ���Œ肷�邱�Ƃ��ł���̂ŁA���̏��(��������)�ŃX�g���[�W�ɂ����ƕۑ��o������A
* �l�b�g���[�N��ʂ��Ăق��̃R���s���[�^�ɑ������肷�邱�Ƃ��ł���(���ꂪ�~���������A�رײ�޻���ԯ��)
*/


using namespace std;
using namespace Microsoft::WRL;
using namespace DirectX;

//DirectXMath�\���̗p��serialize�e���v���[�g�֐����`
namespace DirectX
{
	template<class T>
	void serialize(T& archive, XMFLOAT2& Vec2)
	{
		archive(
			cereal::make_nvp("x", Vec2.x),
			cereal::make_nvp("y", Vec2.y)
		);
	}

	template<class T>
	void serialize(T& archive, XMFLOAT3& Vec3)
	{
		archive(
			cereal::make_nvp("x", Vec3.x),
			cereal::make_nvp("y", Vec3.y),
			cereal::make_nvp("z", Vec3.z)
		);
	}

	template<class T>
	void serialize(T& archive, XMFLOAT4& Vec4)
	{
		archive(
			cereal::make_nvp("x", Vec4.x),
			cereal::make_nvp("y", Vec4.y),
			cereal::make_nvp("z", Vec4.z),
			cereal::make_nvp("w", Vec4.w)
		);
	}

	template<class T>
	void serialize(T& archive, XMFLOAT4X4& M_4x4)
	{
		archive(
			cereal::make_nvp("_11", M_4x4._11), cereal::make_nvp("_12", M_4x4._12),
			cereal::make_nvp("_13", M_4x4._13), cereal::make_nvp("_14", M_4x4._14),
			cereal::make_nvp("_21", M_4x4._21), cereal::make_nvp("_22", M_4x4._22),
			cereal::make_nvp("_23", M_4x4._23), cereal::make_nvp("_24", M_4x4._24),
			cereal::make_nvp("_31", M_4x4._31), cereal::make_nvp("_32", M_4x4._32),
			cereal::make_nvp("_33", M_4x4._33), cereal::make_nvp("_34", M_4x4._34),
			cereal::make_nvp("_41", M_4x4._41), cereal::make_nvp("_42", M_4x4._42),
			cereal::make_nvp("_43", M_4x4._43), cereal::make_nvp("_44", M_4x4._44)
		);
	}
}


/*�Ȃ�ł����ɃV�[�����������Ă邩�]�X*/
struct SceneSkin
{
	struct Node
	{
		uint64_t UniqueID{ 0 };
		string Name;
		FbxNodeAttribute::EType Attribute{ FbxNodeAttribute::EType::eUnknown };
		int64_t ParentIndex{ -1 };

		template<class T>
		void serialize(T& archive)
		{
			archive(UniqueID, Name, Attribute, ParentIndex);
		}

	};
	vector<Node>Nodes;
	/*serialize func*/
	template<class T>
	void serialize(T& archive)
	{
		archive(Nodes);
	}

	/*func*/
	int64_t IndexOf(uint64_t UniqueID) const
	{
		int64_t Index{ 0 };
		for (const Node& nd : Nodes)
		{
			if (nd.UniqueID == UniqueID)
			{
				return Index;
			}
			++Index;
		}
		return -1;
	}
};

struct Material
{
	uint64_t UniqueID = 0;
	string Name;

	XMFLOAT4 Ka = { .2f,.2f,.2f,1.0f };		//�A���r�G���g
	XMFLOAT4 Kd = { .8f,.8f,.8f,1.0f };		//�f�B�t�[�Y
	XMFLOAT4 Ks = { 1.0f,1.0f,1.0f,1.0f };	//�X�y�L����

	string TextureName[4];	//4�����p(�Ȃ�łS�H)
	ComPtr<ID3D11ShaderResourceView> ShaderResourceView[4];

	template<class T>
	void serialize(T& archive)
	{
		archive(UniqueID, Name, Ka, Kd, Ks, TextureName);
	}
};
/*unordered_map...���s��map(?) <>�̒��̒l��kay��value ���݂��ɕR�t�Ă���������ł���悤�ɂ��Ă�
�������Ă����Ɠ���L�[�̗v�f�𕡐��i�[�ł����ɁA�i�[�����K�肳��ĂȂ��R���e�i
hash_map���Č����Ă��A�Ȃ񂩎��Ƃł�����ȁH
map�Ƃ̈Ⴂ�̓\�[�g��Ԃŕۑ����ꂸ�ɗv�f���������Ŏg���ꍇ�������݂���
�ȒP�Ɍ�������json���ȁH*/



//�A�j���[�V����
struct Animation
{
	string Name;
	float SamplingRate = 0;
	struct KeyFrame
	{
		struct Node
		{
			/*
			* GlobalTransform
			* �m�[�h�̃��[�J�����W���烏�[���h���W�ւ̕Ԋ҂Ɏg����ϐ�
			*/
			XMFLOAT4X4 GlobalTransform =
			{
				1.0f,.0f,.0f,.0f,
				.0f,1.0f,.0f,.0f,
				.0f,.0f,1.0f,.0f,
				.0f,.0f,.0f,1.0f
			};
			//�m�[�h�̕ϊ��f�[�^�ɂ͈ړ���]�X�P�[�����O�̊e�x�N�g�����܂܂��
			//�e�m�[�h�ɑ΂���ړ���]�X�P�[�����O�̃x�N�g�����܂܂��
			//�����炱���͐e�ɑ΂���(�e�q�t������ɂ͍s���іт��肷�邩��)�s��ɂȂ�H
			XMFLOAT3 Scaling = { 1.0f,1.0f,1.0f };
			XMFLOAT4 Rotation = { .0f,.0f,.0f,1.0f };
			XMFLOAT3 Translation = { .0f,.0f,.0f };

			template<class T>
			void serialize(T& archive)
			{
				archive(GlobalTransform, Scaling, Rotation, Translation);
			}
		};
		vector<Node> Nodes;
		template<class T>
		void serialize(T& archive)
		{
			archive(Nodes);
		}

	};
	vector<KeyFrame> Sequence;
	template<class T>
	void serialize(T& archive)
	{
		archive(Name, SamplingRate, Sequence);
	}
};

class SkinnedMesh
{
public:
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerStates[2];
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
public:
	static const int MAX_BONEINFLUENCE = 4;//INFLUENCE..�e�� �����֐߂Ƃ�������ւ񈵂����炻���������O
	struct Vertex
	{
		XMFLOAT3 Position;
		XMFLOAT3 Normal;
		XMFLOAT4 Tangent;
		XMFLOAT2 Texcoord;
		//�E�F�C�g�l�ƃ{�[���ԍ�
		float BoneWeights[MAX_BONEINFLUENCE] = { 1.0f,.0f,.0f,.0f };
		uint32_t BoneIndeces[MAX_BONEINFLUENCE] = {};

		/*serialize Func*/
		template<class T>
		void serialize(T& archive)
		{
			archive(Position, Normal, Tangent, Texcoord, BoneWeights, BoneIndeces);
		}
	};

	static const int MAXBONES = 256;
	struct Constant
	{
		XMFLOAT4X4	WorldViewProjection;
		XMFLOAT4X4	World;
		XMFLOAT4	MaterialColor;
		XMFLOAT4    LightDirection;
		XMFLOAT4X4 BoneTransform[MAXBONES] = {
			{1.0f,.0f,.0f,.0f,
			.0f,1.0f,.0f,.0f,
			.0f,.0f,1.0f,.0f,
			.0f,.0f,.0f,1.0f}
		};
	};
	
	struct Skeleton
	{

		struct Bone
		{
			uint64_t UniqueID = 0;
			string name;

			//����ParentIndex�͎������g���܂ޔz��̒��Őe�{�[���̈ʒu������Index
			//root�݂����Ȃ��́H
			int64_t ParentIndex = -1;//������-1�Ȃ�{�[���͈�����Ȃ��H
			//����NodeIndex�̓V�[���̃m�[�h�z����Q�Ƃ���C���f�b�N�X
			int64_t NodeIndex = 0;

			//����OffsetTransform�̓��f����Ԃ���{�[���V�[���ւ̕ϊ��Ɏg��
			//���[�J�����W���烏�[���h���W�H
			XMFLOAT4X4 OffsetTransform =
			{
				1.0f,.0f,.0f,.0f,
				.0f,1.0f,.0f,.0f,
				.0f,.0f,1.0f,.0f,
				.0f,.0f,.0f,1.0f
			};
			bool Orphan() const { return ParentIndex < 0; };

		
			template<class T>
			void serialize(T& archive)
			{
				archive(UniqueID, name, ParentIndex, NodeIndex, OffsetTransform);
			}
		};
		vector<Bone>Bones;
		int64_t IndexOf(uint64_t UniqueID)const
		{
			int64_t Index = 0;
			for (const Bone& bn : Bones)
			{
				if (bn.UniqueID == UniqueID)
				{
					return Index;
				}
				++Index;
			}
			return -1;
		}

		template<class T>
		void serialize(T& archive)
		{
			archive(Bones);
		}

	};

	struct Face
	{
		XMFLOAT3 Position[3];
		//�|���S���͎O�_�ō\������Ă邩��O���̏ꏊ��񂪕K�v�ɂȂ�

		int MaterialIndex;
	};
	std::vector<Face> faces;

	struct Mesh
	{
		uint64_t UniqueID = 0;
		string Name;

		//���̕ϐ��̓V�[���̃m�[�h�z����Q�Ƃ���C���f�b�N�X
		int64_t NodeIndex = 0;

		vector<Vertex>	 Vertices;
		vector<uint32_t> Indices;

		struct Subset
		{
			uint64_t Materrial_UniqueID = 0;
			string MaterialName;

			uint32_t StartIndexLocation = 0;
			uint32_t IndexCount = 0;

			template<class T>
			void serialize(T& archive)
			{
				archive(Materrial_UniqueID, MaterialName, StartIndexLocation, IndexCount);
			}
		};
		vector<Subset> Subsets;

		XMFLOAT4X4 DefaultGlobalTransform =
		{
		1.0f,0.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f
		};

		Skeleton BindPose;//�R�t����|�[�Y�H���ꂽ�|�[�Y�H

		XMFLOAT3 BoundingBox[2]
		{
			{  D3D11_FLOAT32_MAX,  D3D11_FLOAT32_MAX,  D3D11_FLOAT32_MAX },
			{ -D3D11_FLOAT32_MAX, -D3D11_FLOAT32_MAX, -D3D11_FLOAT32_MAX }
		};
	private:
		ComPtr<ID3D11Buffer> VertexBuffer;
		ComPtr<ID3D11Buffer> IndexBuffer;
		friend class SkinnedMesh;
	public:
		template<class T>
		void serialize(T& archive)
		{
			archive(UniqueID, Name, NodeIndex, Subsets, DefaultGlobalTransform,
				BindPose, BoundingBox, Vertices, Indices);
		}
	};
	vector<Mesh> Meshes;
	//�����N���X���ɓ���Ȃ��ƕ������`�G���[���o�Ă���A�����O���[�o�������ɂȂ��Ă�����H
	unordered_map<uint64_t, Material>Materials;
	vector<Animation> AnimationClips;

private:
	ComPtr<ID3D11Buffer>Constantbuffer;		//�萔�o�b�t�@
	ComPtr<ID3D11VertexShader>VertexShader;	//���_�V�F�[�_�[
	ComPtr<ID3D11PixelShader>PixelShader;	//�s�N�Z���V�F�[�_�[
	ComPtr<ID3D11InputLayout>InputLayout;	//�C���v�b�g���C�A�E�g

	ComPtr<ID3D11ShaderResourceView> Dummy;	//�}�e���A���e�N�X�`�������p�̃V�F�[�_�[���\�[�X�r���[
public:
	SkinnedMesh(ID3D11Device* Device, const char* FBX_Filename,
		bool Triangulate = false/*�O�p�`�H���\�H����staticmesh�̎��̃A��*/,
		float SamplingRate = 0.0f/*�T���v�����O���[�g �A�j���[�V�������ǂ̊��o�Ő؂��邩�݂����ȁH*/);

	SkinnedMesh(ID3D11Device* Device, const char* FBX_Filename,
		std::vector<std::string>& AnimationFilename,
		bool Triangulate = false/*�O�p�`�H���\�H����staticmesh�̎��̃A��*/,
		float SamplingRate = 0.0f/*�T���v�����O���[�g �A�j���[�V�������ǂ̊��o�Ő؂��邩�݂����ȁH*/);

	virtual ~SkinnedMesh() = default;
	void FetchMeshes(FbxScene* FBXScene, vector<Mesh>& Meshes);
	void FetchMaterials(FbxScene* FBXScene, unordered_map<uint64_t, Material>& material);
	/*�o�C���h�|�[�Y���𒊏o����֐�*/
	void FetchSkeleton(FbxMesh* FBXMesh, Skeleton& BindPose);
	void FetchAnimation(FbxScene* FBXScene, vector<Animation>& AnimationClips,
		float SamplingRate/*���̒l��0�̏ꍇ�́A�f�t�H���g�̃t���[�����[�g�ŃT���v�����O�����(�A�j���[�V�����X�s�[�h�H)*/);
	void UpdateAnimation(Animation::KeyFrame& KeyFlame);
	//�A�j���[�V�����t�^�H
	bool AppendAnimation(const char* AnimationFilename,
		float SamplingRate = 0.0f);
	void BlendAnimation(const Animation::KeyFrame* KeyFlames[2],
		float Factor, Animation::KeyFrame& KeyFlame);

	void CreateComObject(ID3D11Device* Device, const char* FBX_Filename);
	
	void Render(ID3D11DeviceContext* DeviceContext,
		const XMFLOAT4X4& WorldViewProjection,
		const XMFLOAT4X4& World,
		const XMFLOAT4& rightdirection, const XMFLOAT4& MaterialColor,
		const Animation::KeyFrame* KeyFlame);

	DirectX::XMFLOAT4X4 coordinate_conversion = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 1 };
protected:
	SceneSkin SceneView;
};