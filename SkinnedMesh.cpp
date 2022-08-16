#include "misc.h"
#include <sstream>
#include <functional>
#include "Texture.h"
#include <filesystem>
#include "SkinnedMesh.h"
#include "shader.h"
#include "ConvertMatrix.h"
#include <fstream>
using namespace DirectX;

//グローバルスコープに追加で大丈夫なんすか…？
struct BoneInfluence
{
	uint32_t BoneIndex;
	float BoneWeight;
};

using BoneInfluencePerControlPoint = vector<BoneInfluence>;
//↑ボーンの影響の制御点にあたる？

/*func ボーン影響度をFBXのデータから取得する関数*/
void FetchBoneInfluence(const FbxMesh* FBXMesh,
	vector<BoneInfluencePerControlPoint>& BoneInfluences)
{
	//ジオメトリ(多分ここだと立体そのもの)に割り当てられたコンロトールポイントの数を返してる
	int ControlPointCount = FBXMesh->GetControlPointsCount();
	BoneInfluences.resize(ControlPointCount);

	//モデルが持ってるデフォーマの数を返してる
	const int SkinCount = FBXMesh->GetDeformerCount(fbxsdk::FbxDeformer::eSkin);
	for (int SkinIndex = 0; SkinIndex < SkinCount; ++SkinIndex)
	{
		//指定したインデックスにある指定した型のデフォーマを返してる(GetDeformer(指定したインデックス,指定した型(4つある、今回はスキンなのでeSkinタイプ)))
		const FbxSkin* FBXSkin = static_cast<FbxSkin*>(FBXMesh->GetDeformer(SkinIndex, fbxsdk::FbxDeformer::eSkin));
		//このオブジェクトの追加されたクラスターの数を返してる、ただ調べてもよくわからなかったから何とも言えない
		//集合みたいな感じの意味だから一つのデフォーマが持ってるポリゴン頂点のことを言ってるのかな
		//いやそこのデフォーマが持ってる最大数かも
		const int ClusterCount = FBXSkin->GetClusterCount();
		for (int ClusterIndex = 0; ClusterIndex < ClusterCount; ++ClusterIndex)
		{
			//指定したインデックスでクラスタを取得する
			const FbxCluster* FBXCluster = FBXSkin->GetCluster(ClusterIndex);

			//そのクラスタの持ってるコントロールポイントを返す
			//コントロールポイントってなーんんだ！
			//インデックスバッファの指し示すデータらしい
			const int ControlPointIndecesCount = FBXCluster->GetControlPointIndicesCount();
			for (int ControlPointIndecesIndex = 0; ControlPointIndecesIndex < ControlPointIndecesCount; ++ControlPointIndecesIndex)
			{
				//コントロールポイントのインデックスの配列へのポインターを返してる
				int ControlPointIndex = FBXCluster->GetControlPointIndices()[ControlPointIndecesIndex];
				//コントロールポイントのウェイトの配列を取得
				double ControlPointWeight = FBXCluster->GetControlPointWeights()[ControlPointIndecesIndex];

				BoneInfluence& BoneInfluence = BoneInfluences.at(ControlPointIndex).emplace_back();
				BoneInfluence.BoneIndex = static_cast<uint32_t>(ClusterIndex);
				BoneInfluence.BoneWeight = static_cast<float>(ControlPointWeight);
			}
		}
	}
}

SkinnedMesh::SkinnedMesh(ID3D11Device* Device, const char* FBX_Filename,
	bool Triangulate, float SamplingRate)
{
	//引数のFBX_Filenameの拡張子をcerealに変えて、そのファイルが存在している場合にはそっちのシリアライズされた方を
	//ロードする、だけど存在しない場合にはfbxファイルのほうからロードするようにする
	//だけど、シリアライズされたデータは元データのほうの変更を検知できないので、変更された場合はまた新しくデータを作り直す必要がある
	filesystem::path CerealFilename(FBX_Filename);
	CerealFilename.replace_extension("cereal");

	//もうすでにシリアライズされたファイルがあった場合はこちら側で読み込んでる
	if (filesystem::exists(CerealFilename.c_str()))
	{
		ifstream IFS(CerealFilename.c_str(), ios::binary);
		cereal::BinaryInputArchive deserialization(IFS);
		deserialization(SceneView, Meshes, Materials, AnimationClips);
	}
	else
	{
		//シリアライズされたファイルがなかった時

		//これでFbxManagerを作成
		//これは後で作るSceneとかImportを作成するために必要な管理クラス
		FbxManager* FBXManager = FbxManager::Create();

		//FbxImporterの作成
		//引数はFbxManagerのポインターとそのImporterの名前
		//これはFbxファイルをImportするためのパーサークラス、データをメッシュとかカメラの情報に分けてくれる
		FbxImporter* FBXImporter = FbxImporter::Create(FBXManager, "");

		//FbxSceneを作成
		//引数はFbxManagerのポインターとそのSceneの名前
		//上で作ったImporterが分解したFbxのデータを保存するためのクラス
		FbxScene* FBXScene = FbxScene::Create(FBXManager, "");


		/*FBXファイルはOpenの代わりにInitializeでファイルを開く*/
		_ASSERT_EXPR_A(FBXImporter->Initialize(FBX_Filename), FBXImporter->GetStatus().GetErrorString());

		/*FbxファイルをImportする*/
		//第一引数はFbxDocumentはFbxSceneの親クラス
		//これは分解処理とかまで含まれるからコストが高いらしい
		//第二引数にフラグがあってそれが非同期とかのフラグなので多く出すとかなら
		//スレッドを作ったりとかで対応しよう
		_ASSERT_EXPR_A(FBXImporter->Import(FBXScene), FBXImporter->GetStatus().GetErrorString());

		FbxGeometryConverter FBXConverter(FBXManager);
		if (Triangulate)
		{
			/*ここの処理は、DirectXはポリゴン描画は三角形一つあたり1ポリゴンになってる
			だからもし、四角形とかの多角形の時、三角形にする処理が必要で、この関数はそれをしてる
			ただ、変換するのはそれ相応のコストがかかるのでグラフィック側で対応可能ならそっちでした方がいい*/

			FBXConverter.Triangulate(FBXScene,
				true,	//置き換えするかどうかのフラグ
				false);	//古い形式のポリゴン分割処理を使用(下位互換用なのでデフォはfalse)
			FBXConverter.RemoveBadPolygonsFromMeshes(FBXScene);	//メッシュ削除(詳しいリファレンスなかったからもしわかったら追記しよう)
		}

		//ラムダ…式…？
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
		FetchMeshes(FBXScene, Meshes);
		FetchMaterials(FBXScene, Materials);
		//たぶんn秒ごとのフレームを切り取って描画してる？
		//float SamplingRate = 1;
		
		FetchAnimation(FBXScene, AnimationClips, SamplingRate);


#if 0
		for (const SceneSkin::Node& nd : SceneView.Nodes)
		{
			FbxNode* FBXNode = FBXScene->FindNodeByName(nd.Name.c_str());
			//ノードデータをデバッグとして出力ウィンドウに表示する
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
		//ofstream...OutputFilestream(出力用)
		// ofstream Variable("ファイル名にしたい奴","開くときのモード")
		//第一引数に入れた文字列がファイル名になる(この場合だとCerealFilename.c_str())
		//第二引数のios::binaryはこれをバイナリモードで開くようにしてる
		ofstream OFS(CerealFilename.c_str(), ios::binary);
		cereal::BinaryOutputArchive serialization(OFS);
		serialization(SceneView, Meshes, Materials, AnimationClips);
	}
	CreateComObject(Device, FBX_Filename);
}

//アニメーション付き用
SkinnedMesh::SkinnedMesh(ID3D11Device* Device, const char* FBX_Filename, std::vector<std::string>& AnimationFilename, bool Triangulate, float SamplingRate)
{
	//引数のFBX_Filenameの拡張子をcerealに変えて、そのファイルが存在している場合にはそっちのシリアライズされた方を
	//ロードする、だけど存在しない場合にはfbxファイルのほうからロードするようにする
	//だけど、シリアライズされたデータは元データのほうの変更を検知できないので、変更された場合はまた新しくデータを作り直す必要がある
	filesystem::path CerealFilename(FBX_Filename);
	CerealFilename.replace_extension("cereal");

	//もうすでにシリアライズされたファイルがあった場合はこちら側で読み込んでる
	if (filesystem::exists(CerealFilename.c_str()))
	{
		ifstream IFS(CerealFilename.c_str(), ios::binary);
		cereal::BinaryInputArchive deserialization(IFS);
		deserialization(SceneView, Meshes, Materials, AnimationClips);
	}
	else
	{
		//シリアライズされたファイルがなかった時

		//これでFbxManagerを作成
		//これは後で作るSceneとかImportを作成するために必要な管理クラス
		FbxManager* FBXManager = FbxManager::Create();

		//FbxImporterの作成
		//引数はFbxManagerのポインターとそのImporterの名前
		//これはFbxファイルをImportするためのパーサークラス、データをメッシュとかカメラの情報に分けてくれる
		FbxImporter* FBXImporter = FbxImporter::Create(FBXManager, "");

		//FbxSceneを作成
		//引数はFbxManagerのポインターとそのSceneの名前
		//上で作ったImporterが分解したFbxのデータを保存するためのクラス
		FbxScene* FBXScene = FbxScene::Create(FBXManager, "");


		/*FBXファイルはOpenの代わりにInitializeでファイルを開く*/
		_ASSERT_EXPR_A(FBXImporter->Initialize(FBX_Filename), FBXImporter->GetStatus().GetErrorString());

		/*FbxファイルをImportする*/
		//第一引数はFbxDocumentはFbxSceneの親クラス
		//これは分解処理とかまで含まれるからコストが高いらしい
		//第二引数にフラグがあってそれが非同期とかのフラグなので多く出すとかなら
		//スレッドを作ったりとかで対応しよう
		_ASSERT_EXPR_A(FBXImporter->Import(FBXScene), FBXImporter->GetStatus().GetErrorString());

		FbxGeometryConverter FBXConverter(FBXManager);
		if (Triangulate)
		{
			/*ここの処理は、DirectXはポリゴン描画は三角形一つあたり1ポリゴンになってる
			だからもし、四角形とかの多角形の時、三角形にする処理が必要で、この関数はそれをしてる
			ただ、変換するのはそれ相応のコストがかかるのでグラフィック側で対応可能ならそっちでした方がいい*/

			FBXConverter.Triangulate(FBXScene,
				true,	//置き換えするかどうかのフラグ
				false);	//古い形式のポリゴン分割処理を使用(下位互換用なのでデフォはfalse)
			FBXConverter.RemoveBadPolygonsFromMeshes(FBXScene);	//メッシュ削除(詳しいリファレンスなかったからもしわかったら追記しよう)
		}

		//ラムダ…式…？
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
		FetchMeshes(FBXScene, Meshes);
		FetchMaterials(FBXScene, Materials);
		//たぶんn秒ごとのフレームを切り取って描画してる？
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
			//ノードデータをデバッグとして出力ウィンドウに表示する
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
		//ofstream...OutputFilestream(出力用)
		// ofstream Variable("ファイル名にしたい奴","開くときのモード")
		//第一引数に入れた文字列がファイル名になる(この場合だとCerealFilename.c_str())
		//第二引数のios::binaryはこれをバイナリモードで開くようにしてる
		ofstream OFS(CerealFilename.c_str(), ios::binary);
		cereal::BinaryOutputArchive serialization(OFS);
		serialization(SceneView, Meshes, Materials, AnimationClips);
	}
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

		//引数で入れてる名前のノードの最初のとこを取得
		FbxNode* FBXNode = FBXScene->FindNodeByName(nd.Name.c_str());
		//そのノードのメッシュをとってくる
		FbxMesh* FBXMesh = FBXNode->GetMesh();

		Mesh& ms = Meshes.emplace_back();
		{
			ms.UniqueID = FBXMesh->GetNode()->GetUniqueID();
			ms.Name = FBXMesh->GetNode()->GetName();
			ms.NodeIndex = SceneView.IndexOf(ms.UniqueID);

			Face f;
			vector<BoneInfluencePerControlPoint> BoneInfluence;
			FetchBoneInfluence(FBXMesh, BoneInfluence);
			FetchSkeleton(FBXMesh, ms.BindPose);

			//複数メッシュの対応
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
				//なんかforで定義したら使えない謎の減少に当たったので
				//とりあえずこんな感じでやってみる
				for (int PolygonIndex = 0; PolygonIndex < PolygonCount; ++PolygonIndex)
				{
					const int MaterialIndex = FBXMesh->GetElementMaterial()
						->GetIndexArray().GetAt(PolygonIndex);
					//ポリゴン数カウント
					Subsets.at(MaterialIndex).IndexCount += 3;

				}
				uint32_t Offset = 0;
				for (Mesh::Subset& sb : Subsets)
				{
					sb.StartIndexLocation = Offset;
					Offset += sb.IndexCount;
					//次のループで使うためsubsetのindexカウントを
					//Offsetに入れて保存してから次ように0に戻す
					sb.IndexCount = 0;
				}
			}

			//ポリゴン数を数える
			const int PolygonCount = FBXMesh->GetPolygonCount();
			//LLって何っていう話...long long int型？なんか整数リテラルとかの話(この場合だとlong long intの3を掛けてる？)
			//ポリゴンの頂点だから3を掛けてるのかなるほどなぁ…
			ms.Vertices.resize(PolygonCount * 3LL);
			ms.Indices.resize(PolygonCount * 3LL);

			FbxStringList UVNames;
			//引数のジオメトリにUV名を入れてる？
			FBXMesh->GetUVSetNames(UVNames);
			const FbxVector4* ControlPoint = FBXMesh->GetControlPoints();

			for (int PolygonIndex = 0; PolygonIndex < PolygonCount; ++PolygonIndex)
			{
				const int MaterialIndex = MaterialCount > 0 ?
					FBXMesh->GetElementMaterial()->GetIndexArray().GetAt(PolygonIndex) : 0;
				Mesh::Subset& sb = Subsets.at(MaterialIndex);
				const uint32_t Offset = sb.StartIndexLocation + sb.IndexCount;

				for (int PositionInPolygon = 0; PositionInPolygon < 3; ++PositionInPolygon)
				{
					//頂点番号＝　ポリゴンの番号*3+ポジションの場所(なんか理解できそうだけどなんかつっかえててうまくまとめれない)
					const int VertexIndex = PolygonIndex * 3 + PositionInPolygon;

					//頂点情報を取得
					Vertex vertex;
					{
						DirectX::XMMATRIX MeshTranceform = DirectX::XMLoadFloat4x4(&ms.DefaultGlobalTransform);

						//インデックス付きのポリゴン頂点を返す
						//インデックスとポリゴンの位置からとってきてそれを頂点に入れる(?いまいちわかってない)
						const int PolygonVertex = FBXMesh->GetPolygonVertex(PolygonIndex, PositionInPolygon);
						vertex.Position = { static_cast<float>(ControlPoint[PolygonVertex][0]),
						static_cast<float>(ControlPoint[PolygonVertex][1]),
						static_cast<float>(ControlPoint[PolygonVertex][2]) };

						XMVECTOR LocalPosition = DirectX::XMLoadFloat3(&vertex.Position);
						XMVECTOR GlobalPosition = DirectX::XMVector3TransformCoord(LocalPosition, MeshTranceform);
						DirectX::XMStoreFloat3(&f.Position[PositionInPolygon], GlobalPosition);

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
					//法線情報を取得
					if (FBXMesh->GetElementNormalCount() > 0)
					{
						//ジオメトリ要素の総数をとってきてそれがまだあった時に入る
						FbxVector4 Normal;
						//指定したポリゴン頂点に関連漬けられた法線を取得する
						//PolygonIndexとPositionInPolygonで法線をとってそれを3個目の引数に格納
						FBXMesh->GetPolygonVertexNormal(PolygonIndex, PositionInPolygon, Normal);
						vertex.Normal = { static_cast<float>(Normal[0]),
						static_cast<float>(Normal[1]),
						static_cast<float>(Normal[2]) };
					}

					//uv情報を取得
					if (FBXMesh->GetElementUVCount() > 0)
					{
						FbxVector2 UV;
						bool Unmapped_UV;
						FBXMesh->GetPolygonVertexUV(PolygonIndex, PositionInPolygon,
							UVNames[0], UV, Unmapped_UV);
						vertex.Texcoord = { static_cast<float>(UV[0]),
						1.0f - static_cast<float>(UV[1]) };
						//ここで1から引いてるのは画像が逆向きに描画されるのを防いでる？
						//一回できたらここ直してみたらどうだろう
					}

					//法線情報の取得
					/*
					* GenerateTangentsData
					* 特定のレイヤーでuvsetの接戦データを生成する
					* もうすでに作られてたりする場合はpOverwrite(boolの値)
					* がfalse
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
					int test = ms.Vertices.capacity();

					ms.Vertices.at(VertexIndex) = move(vertex);
					ms.Indices.at(static_cast<size_t>(Offset) + PositionInPolygon) = VertexIndex;
					sb.IndexCount++;
				}

				//これは、モデルの位置姿勢スケール情報をとってそれ自体の行列(姿勢行列？)
				//を変数にぶちこんでる
				//こうしないと複数メッシュのモデルの時全部同じ位置(原点)に出てしまって変になってしまうから
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

		//ここでマテリアル数を取得
		const int MaterialCount = FBXNode->GetMaterialCount();

		//マテリアル数分ループ(こことかIndex使うやつってCurrentのほうが個人的にわかりやすいのでは？)
		for (int MaterialIndex = 0; MaterialIndex < MaterialCount; ++MaterialIndex)
		{
			//マテリアル自体の情報を取得
			const FbxSurfaceMaterial* FBXMaterial = FBXNode->GetMaterial(MaterialIndex);

			//ただここに追加するかはわからない
			const FbxSurfaceMaterial* FBXSurfaceMaterial = FBXNode->GetMaterial(MaterialIndex);

			Material mat;
			{
				//上でとった奴をMaterial構造体にぶち込み
				mat.Name = FBXMaterial->GetName();
				mat.UniqueID = FBXMaterial->GetUniqueID();
			}
			//プロパティを取得
			FbxProperty FBXProperty;
			{
				FBXProperty = FBXMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
				//上でとったプロパティが有効かどうか(isVaild()で調べる)
				//ここはFbxSurfaceMaterialでsDiffuseを指定してるのでDiffuseが帰ってきてる
				if (FBXProperty.IsValid())
				{
					//Getはテンプレート(これ<??>)で指定した型のデータを返してる
					const FbxDouble3 Color = FBXProperty.Get<FbxDouble3>();
					mat.Kd = { static_cast<float>(Color[0]),
					static_cast<float>(Color[1]),
					static_cast<float>(Color[2]),
					1.0f };

					const FbxFileTexture* FBXTexture = FBXProperty.GetSrcObject<FbxFileTexture>();
					//あったらどうかの判別？
					mat.TextureName[0] = FBXTexture ? FBXTexture->GetRelativeFileName() : "";
				}
#if 0
				FBXProperty = FBXMaterial->FindProperty(FbxSurfaceMaterial::sSpecular);
				//上でとったプロパティが有効かどうか(isVaild()で調べる)
				if (FBXProperty.IsValid())
				{
					//Getはテンプレート(これ<??>)で指定した型のデータを返してる
					const FbxDouble3 Color = FBXProperty.Get<FbxDouble3>();
					mat.Ks = { static_cast<float>(Color[0]),
					static_cast<float>(Color[1]),
					static_cast<float>(Color[2]),
					1.0f };

					const FbxFileTexture* FBXTexture = FBXProperty.GetSrcObject<FbxFileTexture>();
					//あったらどうかの判別？
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
				//上でとったプロパティが有効かどうか(isVaild()で調べる)
				if (FBXProperty.IsValid())
				{
					//Getはテンプレート(これ<??>)で指定した型のデータを返してる
					const FbxDouble3 Color = FBXProperty.Get<FbxDouble3>();
					mat.Ka = { static_cast<float>(Color[0]),
					static_cast<float>(Color[1]),
					static_cast<float>(Color[2]),
					1.0f };
					const FbxFileTexture* FBXTexture = FBXProperty.GetSrcObject<FbxFileTexture>();
					//あったらどうかの判別？
					mat.TextureName[2] = FBXTexture ? FBXTexture->GetRelativeFileName() : "";
				}
				material.emplace(mat.UniqueID, move(mat));
			}
		}
	}
}

void SkinnedMesh::FetchSkeleton(FbxMesh* FBXMesh, Skeleton& BindPose)
{
	const int DeformerCount = FBXMesh->GetDeformerCount(FbxDeformer::eSkin);
	for (int DeformerIndex = 0; DeformerIndex < DeformerCount; ++DeformerIndex)
	{
		FbxSkin* Skin = static_cast<FbxSkin*>(FBXMesh->GetDeformer(DeformerIndex, FbxDeformer::eSkin));
		/*
		* 前々からでてたクラスタ(Cluster)はいろいろな情報(頂点インデックスやらボーン、それに対応したボーンとかウェイト)
		* を取りまとめてるfbxsdkが持ってるクラスのことらしい
		* ただそれがどこからどこまで(頂点インデックス1つにつきの数かそれよりおおいか)
		* っていうのはわかってないからもう少し調べたほうがいいかも
		*/
		const int ClusterCount = Skin->GetClusterCount();
		BindPose.Bones.resize(ClusterCount);
		for (int ClusterIndex = 0; ClusterIndex < ClusterCount; ++ClusterIndex)
		{
			FbxCluster* Cluster = Skin->GetCluster(ClusterIndex);

			Skeleton::Bone& Bone = BindPose.Bones.at(ClusterIndex);
			{
				/*
				* ここの名前取得だけどなんでGetLinkを一回踏むのか
				* どこ見ても一回Link踏むけど直接名前とってくるようにはしてないから
				* そもそもそのやり方を想定されてないのかも？
				* 一回ビルド終わったら直接名前とってみたらどうですか
				*/

				//ここであらかじめ作って置くことで少しだけコストを減らす？
				FbxNode* ClusterLink = Cluster->GetLink();
#if 0
				Bone.name = Cluster->GetName();
#else
				//Bone.name	 = Cluster->GetLink()->GetName();
				Bone.name = ClusterLink->GetName();
#endif
				//Bone.UniqueID	 = Cluster->GetLink()->GetUniqueID();
				Bone.UniqueID = ClusterLink->GetUniqueID();
				//した二つはインデックス番号を返してる
				Bone.ParentIndex = BindPose.IndexOf(ClusterLink->GetParent()->GetUniqueID());
				Bone.NodeIndex = SceneView.IndexOf(Bone.UniqueID);

				/*
				* ReferenceGlobalInitPosition
				* モデルのローカル座標からグローバル座標に変換するときに使う
				*/
				FbxAMatrix ReferenceGlobalInitPosition;
				//リンクを含むノードに紐づけられた行列を取得してそれを変換して格納してる？
				Cluster->GetTransformMatrix(ReferenceGlobalInitPosition);


				/*
				* ClusterFlobalInitPosition
				* ボーンのローカル座標からワールド座標に変換するために使う
				*/
				FbxAMatrix ClusterGlobalInitPosition;
				//ボーンの初期姿勢を取得(デフォルトの行列を取得)
				//名前が似通ってるけど全く違うやつなので注意
				Cluster->GetTransformLinkMatrix(ClusterGlobalInitPosition);

				/*
				* Matrices(行列)はColumnMajorスキームを用いて定義される
				* ColumnMajorスキーム...行列の格納方法Column(縦から順番に格納)
				*
				* FbxAMatrixが姿勢行列を表す場合は最後の行(Row)は移動の部分を表す
				* モデル空間からワールド座標にPositionを変換するOffsetTransform行列を作成
				* OffsetTransformはオフセット行列(Offset..偏り)
				*/
				Bone.OffsetTransform = FBXMatToDirMat4x4(ClusterGlobalInitPosition.Inverse() *
					ReferenceGlobalInitPosition);	//Inverseは逆行列を出してる
			}
		}
	}
}

void SkinnedMesh::FetchAnimation(FbxScene* FBXScene, vector<Animation>& AnimationClips, float SamplingRate)
{
	//アニメーションを格納する配列
	FbxArray<FbxString*> AnimationStackNames;
	//シーンからアニメーション一覧を取得して、引数に格納する
	FBXScene->FillAnimStackNameArray(AnimationStackNames);
	const int AnimationStackCount = AnimationStackNames.GetCount();

	for (int AnimationStackIndex = 0; AnimationStackIndex < AnimationStackCount; ++AnimationStackIndex)
	{
		Animation& AnimationClip = AnimationClips.emplace_back();
		AnimationClip.Name = AnimationStackNames[AnimationStackIndex]->Buffer();

		//<Template>のメンバーを検索
		FBXScene->SetCurrentAnimationStack(FBXScene
			->FindMember<FbxAnimStack>(AnimationClip.Name.c_str()));

		//シーン内のグローバル設定を参照して、タイムモードをとってくる
		//タイムモードについての詳しい記述がなかったから細かく書けない…ごめん…
		const FbxTime::EMode TimeMode = FBXScene->GetGlobalSettings().GetTimeMode();

		FbxTime OneSecond;
		//このSetTimeで時刻を設定する
		OneSecond.SetTime(0, 0, 1,//ここは時分秒を与えてる
			0,					//フレーム数
			0,					//
			TimeMode);			//タイムモード
		//もう一つ引数があるけどデフォルトの値が決められてるので今は書かない
		//カスタムフレームレートの値

		AnimationClip.SamplingRate = SamplingRate > 0 ?
			SamplingRate : static_cast<float>(OneSecond.GetFrameRate(TimeMode));
		const FbxTime SamplingInterval = static_cast<FbxLongLong>(OneSecond.Get() / AnimationClip.SamplingRate);
		const FbxTakeInfo* TakeInfo = FBXScene->GetTakeInfo(AnimationClip.Name.c_str());
		const FbxTime StartTime = TakeInfo->mLocalTimeSpan.GetStart();
		const FbxTime StopTime = TakeInfo->mLocalTimeSpan.GetStop();

		//アニメーションをぐるぐるするところ
		//TimeにStartを入れてInterval(間のフレーム？)を足して回してる 
		for (FbxTime Time = StartTime; Time < StopTime; Time += SamplingInterval)
		{
			Animation::KeyFrame& KeyFlame = AnimationClip.Sequence.emplace_back();

			const size_t NodeCount = SceneView.Nodes.size();
			KeyFlame.Nodes.resize(NodeCount);
			for (size_t NodeIndex = 0; NodeIndex < NodeCount; ++NodeIndex)
			{
				//ノードの名前をとってきてそれを検索してる(なんであるのに検索するんや…？)
				FbxNode* FBXNode = FBXScene->FindNodeByName(SceneView.Nodes.at(NodeIndex).Name.c_str());
				if (FBXNode)
				{
					Animation::KeyFrame::Node& Nd = KeyFlame.Nodes.at(NodeIndex);
					/*
					* GlobalTransform
					* ノードの座標とワールド座標に対するノードの対応する変換行列
					*/
					Nd.GlobalTransform =
						FBXMatToDirMat4x4(FBXNode->EvaluateGlobalTransform(Time));

					/*
					* LocalTransform
					* 親のローカル座標を基準にした、ノードの変換行列
					*/
					//指定した時間(引数のTime)のノードのローカル変換行列をとってくる
					const FbxAMatrix& LocalTransform = FBXNode->EvaluateLocalTransform(Time);
					{
						//こいつらのGet~は~のとこの文字によってとってくる行列が変わるからそこを意識しよう
						Nd.Scaling = FBXDobleToXMFLOAT3(LocalTransform.GetS());
						//ここRotationだからGetRかと思ったらGetQでクォータニオンをとってきてる
						//今回アニメーションのスケーリングをクォータニオンでやってるから？
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
		//XMMatrixIdentity その単位行列を返してる
		XMMATRIX P = ParentIndex < 0 ? XMMatrixIdentity() :
			XMLoadFloat4x4(&KeyFlame.Nodes.at(ParentIndex).GlobalTransform);
		XMStoreFloat4x4(&nd.GlobalTransform, S * R * T * P);//ここでPを掛けるのは親子付けするため
	}
}

bool SkinnedMesh::AppendAnimation(const char* AnimationFilename, float SamplingRate)
{
	//マネージャ(管理する実体作成,最初らへんでやったよね)
	FbxManager* FBXManager = FbxManager::Create();
	FbxScene* FBXScene = FbxScene::Create(FBXManager, "");
	FbxImporter* FBXImporter = FbxImporter::Create(FBXManager, "");

	//できたかどうかのチェック
	_ASSERT_EXPR_A(FBXImporter->Initialize(AnimationFilename),
		FBXImporter->GetStatus().GetErrorString());
	_ASSERT_EXPR_A(FBXImporter->Import(FBXScene),
		FBXImporter->GetStatus().GetErrorString());
	FetchAnimation(FBXScene, AnimationClips, SamplingRate);
	//用済みなので殺す
	FBXManager->Destroy();

	return true;
}

void SkinnedMesh::BlendAnimation(const Animation::KeyFrame* KeyFlames[2], float Factor, Animation::KeyFrame& KeyFlame)
{
	size_t NodeCount = KeyFlames[0]->Nodes.size();
	KeyFlame.Nodes.resize(NodeCount);

	for (size_t NodeIndex = 0; NodeIndex < NodeCount; ++NodeIndex)
	{
		//Scaling(拡大縮小)
		XMVECTOR S[2] =
		{
			XMLoadFloat3(&KeyFlames[0]->Nodes.at(NodeIndex).Scaling),
			XMLoadFloat3(&KeyFlames[1]->Nodes.at(NodeIndex).Scaling)
		};
		XMStoreFloat3(&KeyFlame.Nodes.at(NodeIndex).Scaling,
			XMVectorLerp(S[0], S[1], Factor));
		/*
		* XMVectorLerp(XMVECTOR,XMVECTOR,float)
		* 第一引数と第二引数のベクトル間を線形補完する、2年ときの数学でやったね
		* 第三引数は補間係数
		*/

		//Rotation(回転、Quartenionなので注意)
		XMVECTOR R[2] =
		{
			XMLoadFloat4(&KeyFlames[0]->Nodes.at(NodeIndex).Rotation),
			XMLoadFloat4(&KeyFlames[1]->Nodes.at(NodeIndex).Rotation)
		};
		XMStoreFloat4(&KeyFlame.Nodes.at(NodeIndex).Rotation,
			XMQuaternionSlerp(R[0], R[1], Factor));
		/*
		* XMQuaternionSlerp(XMVECTOR,XMVECTOR,float)
		* 上で書いたSlerp関数のQuarternion版
		* 戻り値は四元数になってる
		* ちなみに引数どちらか(もしくはどちらも)単位四元数じゃないときは結果の補間は未定義になる
		*/

		//Translation(姿勢)
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
			//頂点バッファの設定
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

			//インデックスバッファの設定
			BufferDesc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * ms.Indices.size());
			BufferDesc.Usage = D3D11_USAGE_DEFAULT;
			BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			SubresourceData.pSysMem = ms.Indices.data();
			hr = Device->CreateBuffer(&BufferDesc, &SubresourceData,
				ms.IndexBuffer.ReleaseAndGetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}

#if 1
		//もういらなくなったからここで殺す？
		ms.Vertices.clear();
		ms.Indices.clear();
#endif // 1
	}

	D3D11_INPUT_ELEMENT_DESC InputElementDesc[]
	{
		////ほんとにここのアライメントは間違えないようにしよう！！！！！マジで！！！！！！
		{ "POSITION",	0,  DXGI_FORMAT_R32G32B32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "NORMAL",		0,	DXGI_FORMAT_R32G32B32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		//29回目
		{ "TANGENT",	0,	DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "TEXCOORD",	0,	DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		//22回目追加
		{ "WEIGHTS",	0,	DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "BONES",		0,	DXGI_FORMAT_R32G32B32A32_UINT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	create_vs_from_cso(Device, "Shaders/Phong_vs.cso", VertexShader.GetAddressOf(), InputLayout.GetAddressOf(), InputElementDesc, ARRAYSIZE(InputElementDesc));
	create_ps_from_cso(Device, "Shaders/Phong_ps.cso", PixelShader.GetAddressOf());

	/*create_vs_from_cso(Device, "Shaders/skinned_mesh_vs.cso", VertexShader.GetAddressOf(), InputLayout.GetAddressOf(), InputElementDesc, ARRAYSIZE(InputElementDesc));
	create_ps_from_cso(Device, "Shaders/skinned_mesh_ps.cso", PixelShader.GetAddressOf());*/

	//なんでReleaseしてるのかわからない
	//FBXに元々あるやつをあてがってるからかわからないけど何とも言えない…なので頑張ってくださいね

#if 1

#endif

	/*ここにmtlファイルとかがなかった時のダミー云々を描く？*/
	for (unordered_map<uint64_t, Material>::iterator itr = Materials.begin();
		itr != Materials.end(); ++itr)
	{
		for (size_t TextureIndex = 0; TextureIndex < 2; ++TextureIndex)
		{
			//ここからちょっと出てくるsecondはmapに保存されてる実際の値をとってる
			//ちなみにfirstはkeyの値をとってる
			/*itr->first...keyの値 itr->second...そのkeyに保存されてる値*/
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

void SkinnedMesh::Render(ID3D11DeviceContext* DeviceContext,
	const XMFLOAT4X4& WorldViewProjection,
	const XMFLOAT4X4& World,
	const XMFLOAT4& lightdirection, const XMFLOAT4& MaterialColor,
	const Animation::KeyFrame* KeyFlame)
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

		
		//アニメーションの数分まわしてる
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
			//↑今のボーンの数がボーンの最大値を超えたよっていうエラー

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

		//テクスチャ関連はこっち側
		Data.MaterialColor = MaterialColor;


		//subset分の描画を追加
		for (const Mesh::Subset& sb : ms.Subsets)
		{
			if (sb.Materrial_UniqueID != 0)
			{
				const Material& mt = Materials.at(sb.Materrial_UniqueID);
				if (Materials.size() > 0)
				{
					//テクスチャ無しでマテリアルのみで着色してたらこれを描かないと色が反映されないから
					XMStoreFloat4(&Data.MaterialColor,
						XMLoadFloat4(&MaterialColor) * XMLoadFloat4(&mt.Kd));

					DeviceContext->PSSetShaderResources(0, 1,
						mt.ShaderResourceView[0].GetAddressOf());
				}
			}
			else
			{
				//ここでダミーを設定したい
				DeviceContext->PSSetShaderResources(0, 1,
					Dummy.GetAddressOf());

			}
			DeviceContext->UpdateSubresource(Constantbuffer.Get(), 0, 0, &Data, 0, 0);
			DeviceContext->VSSetConstantBuffers(0, 1, Constantbuffer.GetAddressOf());
			DeviceContext->PSSetConstantBuffers(0, 1, Constantbuffer.GetAddressOf());

			D3D11_BUFFER_DESC BufferDesc;
			ms.IndexBuffer->GetDesc(&BufferDesc);
			DeviceContext->DrawIndexed(sb.IndexCount, sb.StartIndexLocation, 0);
		}
	}
}

