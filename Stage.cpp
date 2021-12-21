#include "stage.h"


static Stage* instance = nullptr;

// インスタンス取得
Stage& Stage::Instance()
{
	return *instance;
}

//コンストラクタ
Stage::Stage()
{
	instance = this;
	
	Framework& framework = Framework::Instance();
	ID3D11Device* device = framework.GetDevice();

	//ステージ初期化
	stageBaseMesh = std::make_shared<SkinnedMesh>(device, "Data/tile/tile.fbx", true);
	//stageBaseMesh = std::make_shared<SkinnedMesh>(device, "Data/tile/StageTile.fbx", true);
}

Stage::~Stage()
{
	// ステージモデルを破棄
	//delete model;
}

// 更新処理
//void Stage::Update(float elapsedTime)
//{
//	
//}

//描画処理
//void Stage::Render(ID3D11DeviceContext* immediateContext,
//	const DirectX::XMFLOAT4X4& view,
//	const DirectX::XMFLOAT4X4& projection,
//	const DirectX::XMFLOAT4& light,
//	const DirectX::XMFLOAT4& materialColor,
//	bool wireframe)
//{
//	//ステージ描画
//	//stageBaseObj->Render(immediateContext, view, projection, light, materialColor, false);
//
//}

void Stage::DrawDebugGUI()
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


//レイキャスト(今使えないのでコメントアウト)
//int Stage::RayCast(
// const DirectX::XMFLOAT3& startPosition, 
// const DirectX::XMFLOAT3& endPosition,
//  DirectX::XMFLOAT3* outPosition,
//  DirectX::XMFLOAT3* outNormal, float* outLength)
//{
//	/*引数
//	startPosition : レイを飛ばす開始座標
//	endPosition   : レイを飛ばす終了座標
//	outPosition   : レイが当たった座標
//	outNormal     : レイが当たった面の法線
//	outLength     : レイが当たった面までの距離  戻り値 : マテリアル番号*/
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
//		// 面頂点取得   
//		DirectX::XMVECTOR a = DirectX::XMLoadFloat3(&TriangleCorners[i][0]);
//		DirectX::XMVECTOR b = DirectX::XMLoadFloat3(&TriangleCorners[i][1]);
//		DirectX::XMVECTOR c = DirectX::XMLoadFloat3(&TriangleCorners[i][2]);
//		//  3 辺算出 
//		DirectX::XMVECTOR ab = DirectX::XMVectorSubtract(b, a);
//		DirectX::XMVECTOR bc = DirectX::XMVectorSubtract(c, b);
//		DirectX::XMVECTOR ca = DirectX::XMVectorSubtract(a, c);
//		// 外積による法線算出 
//		DirectX::XMVECTOR n = DirectX::XMVector3Cross(ab, bc);
//		// 内積の結果がプラスならば裏向き
//		DirectX::XMVECTOR dot = DirectX::XMVector3Dot(dir, n);
//		float fdot;
//		DirectX::XMStoreFloat(&fdot, dot);
//		if (fdot >= 0) continue;
//
//		// 交点算出  
//		//cp  あたった座標
//		//cp = start(レイの開始座標) + dir(レイの向き) * t(スカラー)    tを求める
//		//レイの開始位置から頂点aへのベクトルと法線を使って直角三角形を作る
//		//そこから射影(d)を作り、d+法線でd'を作る
//		//  [　　　|-----------------------|start
//		//　|　 　d|射影                 / ↓     
//		//  |　　　|-------------------/---↓dir ]
//		//　|　　　↑|                /     |     |
//		//　|　　　↑|              /       |     |
//		//　|　　　↑|            /         |     |
//		//d'{ 法線 ↑|         /            |      } t(比率)
//		//　|　　　↑|       /              |     |
//		//　|　　　↑|     /                |     |
//		//　|　　　↑|  /                   |     |
//		//　[　 　　|/_____________________|____]______c
//		//     　　a　\                    |           |
//		//       　　　　\                 |           |
//		//          　　　　\              |           |
//		//             　　　　\           cp          |
//		//                　　　　\                    |
//		//                   　　　　\                 | 
//		//                      　　　　\              |
//		//                         　　　　\           |
//		//                            　　　　\        |
//		//                               　　　　\     |
//		//                                  　　　　\  |
//		//                                    　　　　\|b
//		//
//		DirectX::XMVECTOR v0 = DirectX::XMVectorSubtract(a, start);
//		DirectX::XMVECTOR t = DirectX::XMVectorDivide(DirectX::XMVector3Dot(n, v0), dot);
//		float ft;
//		DirectX::XMStoreFloat(&ft, t);
//		if (ft < 0.0f || ft > neart) continue;
//
//		DirectX::XMVECTOR cp = DirectX::XMVectorAdd(DirectX::XMVectorMultiply(dir, t), start);
//
//		//  内点判定  
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
//		// 情報保存 
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
//	//最も近いヒット位置までの距離  
//	*outLength = neart;
//	return ret;
//}
