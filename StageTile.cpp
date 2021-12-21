#include "stageTile.h"
#include "collision.h"
#include "scene.h"

//コンストラクタ
StageTile::StageTile()
{
	stageTileObj = std::make_unique<SkinnedObject>(stageBaseMesh);
	stageTileObj->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	stageTileObj->SetScale(DirectX::XMFLOAT3(1.67f, 1.67f, 1.67f));
	//stageTileObj->SetScale(DirectX::XMFLOAT3(1.6f, 1.6f, 1.6f));
	stageTileObj->SetAngle(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
}

StageTile::~StageTile()
{
	// ステージモデルを破棄
	//delete model;
}

// 更新処理
void StageTile::Update(float elapsedTime)
{
	stageTileObj->SetPosition(position);

	//レイピック用
	/*TriangleCorners[0][0] = DirectX::XMFLOAT3(stageTileObj->GetPosition().x - stageTileObj->GetScale().x, 0.0f, stageTileObj->GetPosition().z + stageTileObj->GetScale().z);
	TriangleCorners[0][1] = DirectX::XMFLOAT3(stageTileObj->GetPosition().x + stageTileObj->GetScale().x, 0.0f, stageTileObj->GetPosition().z + stageTileObj->GetScale().z);
	TriangleCorners[0][2] = DirectX::XMFLOAT3(stageTileObj->GetPosition().x + stageTileObj->GetScale().x, 0.0f, stageTileObj->GetPosition().z - stageTileObj->GetScale().z);

	TriangleCorners[1][0] = DirectX::XMFLOAT3(stageTileObj->GetPosition().x + stageTileObj->GetScale().x, 0.0f, stageTileObj->GetPosition().z - stageTileObj->GetScale().z);
	TriangleCorners[1][1] = DirectX::XMFLOAT3(stageTileObj->GetPosition().x - stageTileObj->GetScale().x, 0.0f, stageTileObj->GetPosition().z - stageTileObj->GetScale().z);
	TriangleCorners[1][2] = DirectX::XMFLOAT3(stageTileObj->GetPosition().x - stageTileObj->GetScale().x, 0.0f, stageTileObj->GetPosition().z + stageTileObj->GetScale().z);*/

	//タイルの4点ポジション取得
	TileLeftTop = DirectX::XMFLOAT3(
		stageTileObj->GetPosition().x - stageTileObj->GetScale().x,
		0.0f,
		stageTileObj->GetPosition().z + stageTileObj->GetScale().z);
	TileRightTop = DirectX::XMFLOAT3(
		stageTileObj->GetPosition().x + stageTileObj->GetScale().x,
		0.0f,
		stageTileObj->GetPosition().z + stageTileObj->GetScale().z);
	TileLeftBottom = DirectX::XMFLOAT3(
		stageTileObj->GetPosition().x - stageTileObj->GetScale().x,
		0.0f,
		stageTileObj->GetPosition().z - stageTileObj->GetScale().z);
	TileRightBottom = DirectX::XMFLOAT3(
		stageTileObj->GetPosition().x + stageTileObj->GetScale().x,
		0.0f,
		stageTileObj->GetPosition().z - stageTileObj->GetScale().z);

	stageTileObj->Update();
}

//描画処理
void StageTile::Render(ID3D11DeviceContext* immediateContext,
	const DirectX::XMFLOAT4X4& view,
	const DirectX::XMFLOAT4X4& projection,
	const DirectX::XMFLOAT4& light,
	const DirectX::XMFLOAT4& materialColor,
	bool wireframe)
{
	//stageTileObj->Render(immediateContext, view, projection, light, materialColor, 0.0f, false);
	stageTileObj->Render(immediateContext, view, projection, light, TileColor[ColorNum], 0.0f, false);
}

void StageTile::DrawDebugGUI()
{
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("StageTile", nullptr, ImGuiWindowFlags_None))
	{
		//トランスフォーム
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			//ImGui::InputInt("value",)
			// 位置
			//ImGui::InputFloat3("Position", &position.x);
			// 回転
			DirectX::XMFLOAT3 a;
			a.x = DirectX::XMConvertToDegrees(angle.x);
			a.y = DirectX::XMConvertToDegrees(angle.y);
			a.z = DirectX::XMConvertToDegrees(angle.z);
			//ImGui::InputFloat3("Angle", &a.x);
			angle.x = DirectX::XMConvertToRadians(a.x);
			angle.y = DirectX::XMConvertToRadians(a.y);
			angle.z = DirectX::XMConvertToRadians(a.z);
			// スケール
			//ImGui::InputFloat3("Scale", &scale.x);
			/*ImGui::InputFloat3("cornerLT", &TriangleCorners[0][0].x);
			ImGui::InputFloat3("cornerLB", &TriangleCorners[1][1].x);
			ImGui::InputFloat3("cornerRT", &TriangleCorners[0][1].x);
			ImGui::InputFloat3("cornerRB", &TriangleCorners[0][2].x);*/
		}
	}
	ImGui::End();
}

//int StageTile::RayCast(const DirectX::XMFLOAT3& startPosition, const DirectX::XMFLOAT3& endPosition, DirectX::XMFLOAT3* outPosition, DirectX::XMFLOAT3* outNormal, float* outLength)
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
//	}
//	if (ret != -1) {
//		DirectX::XMStoreFloat3(outPosition, position);
//		DirectX::XMStoreFloat3(outNormal, normal);
//	}
//	//最も近いヒット位置までの距離  
//	*outLength = neart;
//	return ret;
//}
