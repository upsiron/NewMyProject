#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>
#include <vector>
#include <string>
#include <fbxsdk.h>
#include <unordered_map>
//↑mapとunordered_mapは別物だから気を付けよう！！

/*ここからシリアライズ始まるし
ほしい機能だからちゃんとやろうね*/
#include "cereal/archives/binary.hpp"
#include "cereal/types/memory.hpp"
#include "cereal/types/vector.hpp"
#include "cereal/types/set.hpp"
#include "cereal/types/unordered_map.hpp"
//↓シリアライズ云々について調べた話、見たいときに開いて↓
/*
* 前提として、シリアライズとかってなーんだって話
* シリアライズ(Serialize)..直列化、逐次化(ちくじか)それぞれ違う意味
* ここでは直列化のほうで調べた
* 複数の要素を一列に並べる操作とか処理のこと
* 大体シリアライズって言ったら直列化っていうので表現することが多いらしい
*
* これをすると何ができるかっていうと
* プログラムが処理してる複雑なデータ、オブジェクトのある瞬間(スナップショット)
* を固定することができるので、その状態(文字列状態)でストレージにずっと保存出来たり、
* ネットワークを通じてほかのコンピュータに送ったりすることができる(これが欲しかった、ｼﾘｱﾗｲｽﾞｻｲｺｳﾔｯﾀｰ)
*/


using namespace std;
using namespace Microsoft::WRL;
using namespace DirectX;

//DirectXMath構造体用のserializeテンプレート関数を定義
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


/*なんでここにシーンを持たせてるか云々*/
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

	XMFLOAT4 Ka = { .2f,.2f,.2f,1.0f };		//アンビエント
	XMFLOAT4 Kd = { .8f,.8f,.8f,1.0f };		//ディフーズ
	XMFLOAT4 Ks = { 1.0f,1.0f,1.0f,1.0f };	//スペキュラ

	string TextureName[4];	//4個入れる用(なんで４？)
	ComPtr<ID3D11ShaderResourceView> ShaderResourceView[4];

	template<class T>
	void serialize(T& archive)
	{
		archive(UniqueID, Name, Ka, Kd, Ks, TextureName);
	}
};
/*unordered_map...順不同map(?) <>の中の値はkayとvalue お互いに紐付てそれを検索できるようにしてる
何かっていうと同一キーの要素を複数格納できずに、格納順が規定されてないコンテナ
hash_mapって言われてるやつ、なんか授業でやったな？
mapとの違いはソート状態で保存されずに要素検索だけで使う場合が多いみたい
簡単に言ったらjsonかな？*/



//アニメーション
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
			* ノードのローカル座標からワールド座標への返還に使われる変数
			*/
			XMFLOAT4X4 GlobalTransform =
			{
				1.0f,.0f,.0f,.0f,
				.0f,1.0f,.0f,.0f,
				.0f,.0f,1.0f,.0f,
				.0f,.0f,.0f,1.0f
			};
			//ノードの変換データには移動回転スケーリングの各ベクトルが含まれる
			//親ノードに対する移動回転スケーリングのベクトルが含まれる
			//だからここは親に対する(親子付けするには行列を毛毛たりするから)行列になる？
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
	static const int MAX_BONEINFLUENCE = 4;//INFLUENCE..影響 多分関節とかそこらへん扱うからそういう名前
	struct Vertex
	{
		XMFLOAT3 Position;
		XMFLOAT3 Normal;
		XMFLOAT4 Tangent;
		XMFLOAT2 Texcoord;
		//ウェイト値とボーン番号
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

			//このParentIndexは自分自身を含む配列の中で親ボーンの位置を示すIndex
			//rootみたいなもの？
			int64_t ParentIndex = -1;//ここが-1ならボーンは一つしかない？
			//このNodeIndexはシーンのノード配列を参照するインデックス
			int64_t NodeIndex = 0;

			//このOffsetTransformはモデル空間からボーンシーンへの変換に使う
			//ローカル座標からワールド座標？
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
		//ポリゴンは三点で構成されてるから三個分の場所情報が必要になる

		int MaterialIndex;
	};
	std::vector<Face> faces;

	struct Mesh
	{
		uint64_t UniqueID = 0;
		string Name;

		//下の変数はシーンのノード配列を参照するインデックス
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

		Skeleton BindPose;//紐付するポーズ？されたポーズ？

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
	//↓をクラス内に入れないと複数回定義エラーが出てくる、多分グローバル扱いになってたから？
	unordered_map<uint64_t, Material>Materials;
	vector<Animation> AnimationClips;

private:
	ComPtr<ID3D11Buffer>Constantbuffer;		//定数バッファ
	ComPtr<ID3D11VertexShader>VertexShader;	//頂点シェーダー
	ComPtr<ID3D11PixelShader>PixelShader;	//ピクセルシェーダー
	ComPtr<ID3D11InputLayout>InputLayout;	//インプットレイアウト

	ComPtr<ID3D11ShaderResourceView> Dummy;	//マテリアルテクスチャ無し用のシェーダーリソースビュー
public:
	SkinnedMesh(ID3D11Device* Device, const char* FBX_Filename,
		bool Triangulate = false/*三角形？裏表？多分staticmeshの時のアレ*/,
		float SamplingRate = 0.0f/*サンプリングレート アニメーションをどの感覚で切り取るかみたいな？*/);

	SkinnedMesh(ID3D11Device* Device, const char* FBX_Filename,
		std::vector<std::string>& AnimationFilename,
		bool Triangulate = false/*三角形？裏表？多分staticmeshの時のアレ*/,
		float SamplingRate = 0.0f/*サンプリングレート アニメーションをどの感覚で切り取るかみたいな？*/);

	virtual ~SkinnedMesh() = default;
	void FetchMeshes(FbxScene* FBXScene, vector<Mesh>& Meshes);
	void FetchMaterials(FbxScene* FBXScene, unordered_map<uint64_t, Material>& material);
	/*バインドポーズ情報を抽出する関数*/
	void FetchSkeleton(FbxMesh* FBXMesh, Skeleton& BindPose);
	void FetchAnimation(FbxScene* FBXScene, vector<Animation>& AnimationClips,
		float SamplingRate/*この値が0の場合は、デフォルトのフレームレートでサンプリングされる(アニメーションスピード？)*/);
	void UpdateAnimation(Animation::KeyFrame& KeyFlame);
	//アニメーション付与？
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