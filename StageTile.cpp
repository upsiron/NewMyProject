#include "stageTile.h"
#include "collision.h"
#include "scene.h"
#include <iostream>
#include <string>    
#include <fstream> 

//コンストラクタ
StageTile::StageTile()
{
	////ファイル読み取り
	//std::string Filename = "Text/stage.txt";
	//std::ifstream ReadingFile(Filename, std::ios::in);
	//for (int i = 0; i < 6; i++)
	//{
	//	for (int j = 0; j < 10; j++)
	//	{
	//		for (int k = 0; k < 9; k++)
	//		{
	//			ReadingFile >> StageTileMapTxt[i][j][k];
	//			StageTileMap[i][j][k] = StageTileMapTxt[i][j][k];
	//		}
	//	}
	//}

	////ファイル書き込み
	//Filename = "Text/stage.txt";
	//std::ofstream WritingFile;
	//WritingFile.open(Filename, std::ios::out);
	//for (int i = 0; i < 6; i++)
	//{
	//	for (int j = 0; j < 10; j++)
	//	{
	//		for (int k = 0; k < 9; k++)
	//		{
	//			WritingFile << StageTileMap[i][j][k]<< endl;
	//		}
	//	}
	//}

	////ファイル読み取り
	///*std::string line;
	//std::string line2;*/
	//std::ifstream ReadingFile("Text/Book.csv");
	//std::ofstream WritingFile("Text/Book.csv");
	///*while (std::getline(ReadingFile,line))
	//{
	//	std::istringstream i_stream(line);

	//	while (std::getline(i_stream, line2,','))
	//	{
	//		ofs << line2 << ',';
	//	}
	//	ofs << endl;
	//}*/

	//for (int i = 0; i < 6; i++)
	//{
	//	for (int j = 0; j < 10; j++)
	//	{
	//		for (int k = 0; k < 9; k++)
	//		{
	//			ReadingFile >> StageTileMapTxt[i][j][k];
	//			StageTileMap[i][j][k] = StageTileMapTxt[i][j][k];
	//			//WritingFile << StageTileMapTxt[i][j][k];
	//			//if ((k + 1) % 3 == 0)WritingFile << endl;
	// 
	//		}
	//	}
	//}

	////ファイル書き込み
	//std::ofstream ofs("Text/Book.csv");
	//for (int i = 0; i < 6; i++)
	//{
	//	for (int j = 0; j < 10; j++)
	//	{
	//		for (int k = 0; k < 9; k++)
	//		{
	//			WritingFile << StageTileMap[i][j][k];
	//			//WritingFile << StageTileMapTxt[i][j][k];
	//			//if ((k + 1) % 3 == 0)WritingFile << endl;
	//		}
	//	}
	//}

	stageTileObj = std::make_unique<SkinnedObject>(stageTileMesh);
	stageTileObj->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	stageTileObj->SetScale(DirectX::XMFLOAT3(1.6f, 0.1f, 1.6f));
	stageTileObj->SetAngle(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	StageScaleGap = 0.7f;
}

StageTile::~StageTile()
{
}

// 更新処理
void StageTile::Update(float elapsedTime)
{
	if (ColorNum == RED || ColorNum == GREEN || ColorNum == BLUE || ColorNum == YELLOW || ColorNum == LIGHTBLUE || ColorNum == PURPLE)
	{
		stageTileObj = std::make_unique<SkinnedObject>(gimmickTileMesh);
		stageTileObj->SetScale(DirectX::XMFLOAT3(1.6f, 0.1f, 1.6f));
	}
	else
	{
		stageTileObj = std::make_unique<SkinnedObject>(stageTileMesh);
		stageTileObj->SetScale(DirectX::XMFLOAT3(1.6f, 0.1f, 1.6f));
	}

	stageTileObj->SetPosition(position);
	
	//タイルの4点ポジション取得
	tileLeftTop = DirectX::XMFLOAT3(
		stageTileObj->GetPosition().x - stageTileObj->GetScale().x - StageScaleGap,
		0.0f,
		stageTileObj->GetPosition().z + stageTileObj->GetScale().z + StageScaleGap);
	tileRightTop = DirectX::XMFLOAT3(
		stageTileObj->GetPosition().x + stageTileObj->GetScale().x + StageScaleGap,
		0.0f,
		stageTileObj->GetPosition().z + stageTileObj->GetScale().z + StageScaleGap);
	tileLeftBottom = DirectX::XMFLOAT3(
		stageTileObj->GetPosition().x - stageTileObj->GetScale().x - StageScaleGap,
		0.0f,
		stageTileObj->GetPosition().z - stageTileObj->GetScale().z - StageScaleGap);
	tileRightBottom = DirectX::XMFLOAT3(
		stageTileObj->GetPosition().x + stageTileObj->GetScale().x + StageScaleGap,
		0.0f,
		stageTileObj->GetPosition().z - stageTileObj->GetScale().z - StageScaleGap);

	stageTileObj->Update(elapsedTime);
}

//描画処理
void StageTile::Render(ID3D11DeviceContext* immediateContext,
	const DirectX::XMFLOAT4X4& view,
	const DirectX::XMFLOAT4X4& projection,
	const DirectX::XMFLOAT4& light,
	const DirectX::XMFLOAT4& materialColor,
	bool wireframe)
{
	stageTileObj->Render(immediateContext, view, projection, light, TileColor[ColorNum], false);
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