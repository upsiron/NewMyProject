#include "scene.h"
#include "KeyInput.h"
#include "framework.h"
#include "collision.h"
#include "Input/Input.h"
#include "Camera.h"
#include "EnemyManager.h"
#include "ObstacleBlockManager.h"
#include "StageManager.h"
#include "StageTileManager.h"

#include <iostream>
#include <string>    
#include <fstream>   
//**********************************************
//
//		メインシーン
//
//**********************************************

static bool ShowDemoWindow = false;
static bool ShowAnotherWindow = false;

static DirectX::XMFLOAT4 ambientColor = { 0.564f,0.564f,0.564f,1 };
static float diffuseColor[3] = { 0.3f, 0.3f, 0.3f };
DirectX::XMFLOAT4 LightDir(1, 1, 1, 1);
DirectX::XMFLOAT4 DirLightColor(1, 1, 1, 1);


//--------------------------------------------------------
//		初期化
//--------------------------------------------------------

void SceneMain::Initialize()
{
	Framework& framework = Framework::Instance();
	ID3D11Device* device = framework.GetDevice();
	ID3D11DeviceContext* Context = framework.GetImmediateContext();
	HWND hwnd = framework.GetWindowHandle();

	//コンスタントバッファ
	CreateConstantBuffer(&ConstantBuffer, sizeof(ConstantBufferForPerFrame));

	FadeBlack = std::make_unique<Transition>();
	FadeBlack->init(device, L"Data/unnamed.png", { 0,0 }, { 1920,1080 }, { 0,0 }, { 1920,1080 }, { 1.0f,1.0f,1.0f,1.0f });

	float screenWidth = static_cast<float>(framework.GetScreenWidth());
	float screenHeight = static_cast<float>(framework.GetScreenHeight());

	//カメラ初期化
	Camera& camera = Camera::Instance();

	//カメラの設定
	camera.SetLookAt(
		DirectX::XMFLOAT3(0, 100, -20),  //カメラの位置
		DirectX::XMFLOAT3(0, 0, 0),      //カメラの焦点
		DirectX::XMFLOAT3(0, 1, 0)		 //カメラの上方向を決める
	);

	//カメラの視野角
	camera.SetPerspecticeFov(
		DirectX::XMConvertToRadians(45),
		screenWidth / screenHeight,
		0.1f,
		1000.0f
	);


	skyMesh = std::make_shared<SkinnedMesh>(device, "Data/Sky/sky.obj",true);
	skyObj = std::make_unique<SkinnedObject>(skyMesh);
	skyObj->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	skyObj->SetScale(DirectX::XMFLOAT3(7.0f, 7.0f, 7.0f));
	skyObj->SetAngle(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));


	// カメラコントローラー初期化
	cameraController = new CameraController();

	//プレイヤー初期化
	player = new Player();

	//基底ステージ初期化
	stage = new Stage();

	//------------
	// ステージ
	//------------
	//ステージマネージャー初期化
	StageManager& stageManager = StageManager::Instance();
	//ステージ初期化
	for (int i = 0; i < StageMax; i++)
	{
		stageBase[i] = new StageBase();

		//座標指定
		stageBase[i]->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 10.0f * i));

		//初期位置だけ穴がないパターン適用
		stageBase[0]->Rand = 5;

		//ステージマネージャーに追加
		stageManager.Register(stageBase[i]);
	}


	//--------------
	// ステージタイル
	//--------------
	//ステージタイルマネージャー初期化
	StageTileManager& stageTileManager = StageTileManager::Instance();
	//ステージタイル初期化
	for (int i = 0; i < StageTileMax; i++)
	{
		for (int j = 0; j < StageMax; j++)
		{
			stageTile[j][i] = new StageTile();

			//座標指定
			stageTile[j][i]->SetPosition(stageBase[j]->squea.SpritPosition[i]);

			//ステージタイルマネージャーに追加
			stageTileManager.Register(stageTile[j][i]);
		}
	}

	
	//------------
	// 障害物 
	//------------
	// 障害物マネージャー初期化
	ObstacleBlockManager& obstacleBlockManager = ObstacleBlockManager::Instance();
	// 障害物初期化
	for (int j = 0; j < StageMax; j++)
	{
		for (int i = 0; i < ObstacleMax; i++)
		{
			obstacle[j][i] = new ObstacleBlock();
			obstacle[j][i]->SetPosition(DirectX::XMFLOAT3(
				stageBase[0]->squea.SpritPosition[0].x,
				-10.0f,
				stageBase[0]->squea.SpritPosition[0].z));
			//エネミーマネージャーに追加
			obstacleBlockManager.Register(obstacle[j][i]);
		}
	}

	// エネミーマネージャー初期化
	//EnemyManager& enemyManager = EnemyManager::Instance();
	//for (int j = 0; j < StageMax; j++)
	//{
	//	for (int i = 0; i < 2; i++)
	//	{
	//		enemy[j][i] = new EnemyBlueSlime();
	//		enemy[j][i]->SetPosition(DirectX::XMFLOAT3(
	//			stageBase[0]->squea.SpritPosition[0].x,
	//			-10.0f,
	//			stageBase[0]->squea.SpritPosition[0].z));
	//		//エネミーマネージャーに追加
	//		enemyManager.Register(enemy[j][i]);
	//	}
	//}

	

	//for (int i = 0; i < StageMax; i++)
	//{
	//	//座標指定
	//	stageBase[i]->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 10.0f * i));

	//	//初期位置だけ穴がないパターン適用
	//	stageBase[0]->Rand = 5;

	//	//ステージマネージャーに追加
	//	stageManager.Register(stageBase[i]);
	//}

	//

	//for (int i = 0; i < 9; i++)
	//{
	//	for (int j = 0; j < StageMax; j++)
	//	{
	//		//座標指定
	//		stageTile[j][i]->SetPosition(stageBase[j]->squea.SpritPosition[i]);
	//		//ステージタイルマネージャーに追加
	//		stageTileManager.Register(stageTile[j][i]);
	//	}
	//}


	state = 0;
	timer = 0;
	fadeInFlg = true;
	fadeOutFlg = false;

	//音楽初期化
	BGM = framework.GetSoundManager()->CreateSoundSource("Data/Sounds/stage.wav");

	//音楽再生
	//BGM->Play(true);

	//imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& IO = ImGui::GetIO();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(device, Context);
	ImGui::StyleColorsClassic();

}

//--------------------------------------------------------
//		終了
//--------------------------------------------------------
void SceneMain::Finalize()
{
	//実体解放
	if (player != nullptr)
	{
		delete player;
		player = nullptr;
	}
	ObstacleBlockManager::Instance().Clear();
	EnemyManager::Instance().Clear();
	StageManager::Instance().Clear();
	StageTileManager::Instance().Clear();
}

//--------------------------------------------------------
//		更新
//--------------------------------------------------------
void SceneMain::Update(float elapsedTime)
{
	timer++;
	
	//ライト設定
	DirectX::XMFLOAT3 dir;
	dir.x = LightDir.x;
	dir.y = LightDir.y;
	dir.z = LightDir.z;
	SetDirLight(dir, DirectX::XMFLOAT3(diffuseColor));


	//if (fadeInFlg)
	//{
	//	FadeBlack->fadeIn(0.03f);

    //  //フェードインする前のステージ更新	
	//	StageManager::Instance().Update(elapsedTime);
	//	StageTileManager::Instance().Update(elapsedTime);
	//	EnemyManager::Instance().Update(elapsedTime);

	//	//フェードインする前のカメラの位置、ターゲット設定
	//	DirectX::XMFLOAT3 target = player->GetPosition();
	//	target.y += 3.5f;
	//	cameraController->SetTarget(DirectX::XMFLOAT3(0.0f, target.y, target.z));

	//	//フェードインする前のカメラ更新
	//	cameraController->Update(elapsedTime);

	//	if (FadeBlack->GetAlpha() <= 0.0f) fadeInFlg = false;
	//}

	//if (!fadeInFlg)
	{
		FadeBlack->fadeIn(0.03f);

		//ターゲットする物のpos入れる変数
		DirectX::XMFLOAT3 target = player->GetPosition();
		//カメラの高さ調整
		target.z -= 2.0f;

		//カメラにターゲットさせるものを設定
		cameraController->SetTarget(DirectX::XMFLOAT3(0.0f, 5.8f, target.z));
		//カメラ更新
		cameraController->Update(elapsedTime);

		skyObj->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, player->GetPosition().z));
		skyObj->Update();

		//自機更新
		player->Update(elapsedTime);

		
		//ステージタイルをランダムで選ばれたパターンごとに配置
		for (int j = 0; j < StageMax; j++)
		{
			for (int i = 0; i < StageTileMax; i++)
			{
				//ステージのHOLEの部分以外で配置
				//if (stageTile[j][i]->stageTileMap[stageBase[j]->Pattern][stageBase[j]->Rand][i] != HOLE)
				if (stageTile[j][i]->stageTileMapEasy[stageBase[j]->EasyRand][j][i] != 1)
				{
					//------------
					// タイル配置 
					//------------
					stageTile[j][i]->SetPosition(stageBase[j]->squea.SpritPosition[i]);

					//------------
					// 障害物配置 
					//------------
					//今見ているステージタイルとランダムで決まった障害物配置の番号が一緒なら配置
					//if (i == stageBase[j]->SquareRand[0] && stageBase[j]->Rand != 5)
					//障害物１
					//if (stageTile[j][i]->stageTileMap[stageBase[j]->Pattern][stageBase[j]->Rand][i] == OB1)
					if (stageTile[j][i]->stageTileMapEasy[stageBase[j]->EasyRand][j][i] == 2)
					{
						//エネミーをステージを9等分して指定した位置にセット
						obstacle[j][0]->SetPosition(DirectX::XMFLOAT3(
							stageBase[j]->squea.SpritPosition[i].x,
							1.5f, 
							stageBase[j]->squea.SpritPosition[i].z));
						//生存
						obstacle[j][0]->SetExistFlg(true);
					}
					//障害物２
					//if (stageTile[j][i]->stageTileMap[stageBase[j]->Pattern][stageBase[j]->Rand][i] == OB2)
					if (stageTile[j][i]->stageTileMapEasy[stageBase[j]->EasyRand][j][i] == 3)
					{
						//エネミーをステージを9等分して指定した位置にセット
						obstacle[j][1]->SetPosition(DirectX::XMFLOAT3(
							stageBase[j]->squea.SpritPosition[i].x,
							1.5f,
							stageBase[j]->squea.SpritPosition[i].z));
						//生存
						obstacle[j][1]->SetExistFlg(true);
					}
					//障害物3
					//if (stageTile[j][i]->stageTileMap[stageBase[j]->Pattern][stageBase[j]->Rand][i] == OB3)
					if (stageTile[j][i]->stageTileMapEasy[stageBase[j]->EasyRand][j][i] == 4)
					{
						//エネミーをステージを9等分して指定した位置にセット
						obstacle[j][2]->SetPosition(DirectX::XMFLOAT3(
							stageBase[j]->squea.SpritPosition[i].x,
							1.5f,
							stageBase[j]->squea.SpritPosition[i].z));
						//生存
						obstacle[j][2]->SetExistFlg(true);
					}

					//-------------------
					// タイルカラーセット 
					//-------------------
					//今見ているステージタイルとランダムで決まったステージベースのstageTileMap番号をカラーをセット
					//stageTile[j][i]->SetTileColor(stageTile[j][i]->stageTileMap[stageBase[j]->Pattern][stageBase[j]->Rand][i]);
					stageTile[j][i]->SetTileColor(stageTile[j][i]->stageTileMapEasy[stageBase[j]->EasyRand][j][i]);
					/*if (stageTile[j][i]->stageTileMap[stageBase[j]->Pattern][stageBase[j]->Rand][i] == 5)
					{
						stageTile[j][i]->SetTileColor(stageTile[j][i]->stageTileMap[stageBase[j]->Pattern][stageBase[j]->Rand][i]);
					}
					else
					{
						stageTile[j][i]->SetTileColor(stageTile[j][i]->stageTileMap[stageBase[j]->Pattern][stageBase[j]->Rand][i]);
					}*/
				}
				else
				{
					//------------
					// タイル配置 
					//------------
					//穴が開く部分のタイルを重ねて隠す
					stageTile[j][i]->SetPosition(DirectX::XMFLOAT3(stageBase[j]->squea.SpritPosition[6].x, -20.0f, stageBase[j]->squea.SpritPosition[6].z-100.0f));

					//------------
					// 障害物配置 
					//------------
					//使わない障害物を下に配置している
					//obstacle[j][0]->SetPosition(DirectX::XMFLOAT3(
					//	stageBase[0]->squea.SpritPosition[0].x,
					//	-10.0f,
					//	stageBase[0]->squea.SpritPosition[0].z));
					//obstacle[j][1]->SetPosition(DirectX::XMFLOAT3(
					//	stageBase[0]->squea.SpritPosition[0].x,
					//	-10.0f,
					//	stageBase[0]->squea.SpritPosition[0].z));

					////下に行った障害物の存在を消す（当たり判定や描画をしなくしている）
					//obstacle[j][0]->SetExistFlg(false);
					//obstacle[j][1]->SetExistFlg(false);
				}
			}
		}

		//エネミーマネージャー更新
		//EnemyManager::Instance().Update(elapsedTime);

		//障害物マネージャー更新
		ObstacleBlockManager::Instance().Update(elapsedTime);

		//ステージ更新
		StageManager::Instance().Update(elapsedTime);
		StageTileManager::Instance().Update(elapsedTime);
		//ゲームオーバー処理
		if (player->GetGameOverFlg())
		{
			fadeOutFlg = true;
		}
	}

	if (fadeOutFlg)
	{
		FadeBlack->fadeOut(0.04f);
		if (FadeBlack->GetAlpha() >= 1.0f)
		{
			ResultMeter();
			Finalize();
			SceneManager::Instance().ChangeScene((new SceneOver()));
			return;
		}
	}
}




void SceneMain::imGuiUpdate()
{

	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (ShowDemoWindow)
		ImGui::ShowDemoWindow(&ShowDemoWindow);

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	{

		ImGui::Begin(u8"Lightinig");      // Create a window called "Hello, world!" and append into it.
		ImGui::TextColored(ImVec4(1, 1, 0, 1), u8"---------Light---------");

		ImGui::TextColored(ImVec4(1, 1, 0, 1), u8"---------Ambient---------");
		ImGui::ColorEdit3(u8"環境光", (float*)&ambientColor); // Edit 3 floats representing a color
		ImGui::TextColored(ImVec4(1, 1, 0, 1), u8"---------Diffuse---------");
		ImGui::ColorEdit3(u8"平行光", (float*)&diffuseColor); // Edit 3 floats representing a color
		for (int i = 0; i < StageMax; i++)
		{
			ImGui::InputInt("Rand", &stageBase[i]->EasyRand);
		}
		//ImGui::TextColored(ImVec4(1, 1, 0, 1), u8"---------半球ライト---------");
		//ImGui::ColorEdit3(u8"SkyColor", (float*)&SkyColor); // Edit 3 floats representing a color
		//ImGui::ColorEdit3(u8"GroundColor", (float*)&GroundColor); // Edit 3 floats representing a color
		//ImGui::ColorEdit3(u8"FogColor", (float*)&FogColor);
		//ImGui::SliderFloat(u8"Near", &FogNear, 0.0f, 100.0f);
		//ImGui::SliderFloat(u8"far", &FogFar, 0.0f, 100.0f);
		ImGui::Separator();

		ImGui::End();
	}

	// 3. Show another simple window.
	if (ShowAnotherWindow)
	{
		ImGui::Begin("Another Window", &ShowAnotherWindow);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			ShowAnotherWindow = false;
		ImGui::End();
	}

	// プレイヤーデバッグ描画
	player->DrawDebugGUI();
	//EnemyManager::Instance().DrawDebugGUI();
	StageManager::Instance().DrawDebugGUI();
	//StageTileManager::Instance().DrawDebugGUI();
}



//--------------------------------------------------------
//		描画
//--------------------------------------------------------

void SceneMain::Render()
{
	imGuiUpdate();

	Framework& framework = Framework::Instance();

	ID3D11DeviceContext* context = framework.GetImmediateContext();

	FLOAT color[] = { 0.4f, 0.4f, 0.9f, 1.0f };
	//FLOAT color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	ID3D11RenderTargetView* rtv = framework.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = framework.GetDepthStencilView();
	context->ClearRenderTargetView(rtv, color);
	context->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->OMSetRenderTargets(1, &rtv, dsv);

	// カメラパラメータ設定
	Camera& camera = Camera::Instance();

	ID3D11BlendState* bs = framework.GetBlendState(Blender::BS_ALPHA);
	context->OMSetBlendState(bs, nullptr, 0xFFFFFFFF);

	DirectX::XMFLOAT4 lightDirection(0, -1, 0, 0);

	DirectX::XMFLOAT4X4 view = camera.GetView();

	DirectX::XMFLOAT4X4 projection = camera.GetProjection();

	DirectX::XMFLOAT4 materialColor(1.0f, 1.0f, 1.0f, 1.0f);

	
	{
		ConstantBufferForPerFrame cb;
		cb.AmbientColor = ambientColor;
		cb.LightColor = DirLightColor;
		cb.EyePos.x = camera.GetEye().x;
		cb.EyePos.y = camera.GetEye().y;
		cb.EyePos.z = camera.GetEye().z;
		cb.EyePos.w = 1.0f;

		context->UpdateSubresource(ConstantBuffer.Get(), 0, NULL, &cb, 0, 0);
		context->VSSetConstantBuffers(2, 1, ConstantBuffer.GetAddressOf());
		context->PSSetConstantBuffers(2, 1, ConstantBuffer.GetAddressOf());
	}

	//プレイヤー描画
	player->Render(context, view, projection, lightDirection, materialColor, false);

	//ステージ描画
	//StageManager::Instance().Render(context, view, projection, lightDirection, materialColor, false);
	StageTileManager::Instance().Render(context, view, projection, lightDirection, materialColor, false);

	//障害物描画
	ObstacleBlockManager::Instance().Render(context, view, projection, lightDirection, materialColor, false);

	// エネミー描画
	//EnemyManager::Instance().Render(context, view, projection, lightDirection, materialColor, false);

	//skyObj->Render(context, view, projection, lightDirection, DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), 0.0f, false);

	// ワールド座標からスクリーン座標へ
	DirectX::XMFLOAT3 worldPosition = player->GetPosition();
	DirectX::XMFLOAT3 screenPosition;
	worldPosition.y += 1.5f;
	WorldToScreen(&screenPosition, worldPosition);

	// 戦車の頭上に名前を表示
	float fontSize = 16;
	const char* name = "TANK";
	float width = fontSize * strlen(name);
	Sprite* Font = framework.GetFont();
	/*font->TextOut(context, name,
		screenPosition.x - width / 2,
		screenPosition.y,
		fontSize, fontSize);*/

	name = "M";
	std::string pMeter = to_string((int)player->GetPosition().z);
	pMeter += name;
	//font->TextOut(context, name, 0, 0, 32, 32);
	Font->TextOut(context, pMeter, framework.GetScreenWidth()/2 - 100, 0, 64, 64);

	FadeBlack->render(context);

	//imGui
	Scene::imGuiRender();
}

void SceneMain::ResultMeter()
{
	//ファイル読み取り
	std::string Filename = "Text/score.txt";
	std::ifstream ReadingFile(Filename, std::ios::in);
	for (int i = 0; i < 5; i++)
	{
		ReadingFile >> Result[i];
	}

	//スコア並び替え
	for (int i = 0; i < 5; i++)
	{
		if ((int)player->GetPosition().z > Result[i])
		{
			for (int j = 4; j >= (i + 1); j--)
			{
				Result[j] = Result[j - 1];
			}
			Result[i] = (int)player->GetPosition().z;
			break;
		}
	}

	//ファイル書き込み
	Filename = "Text/score.txt";
	std::ofstream WritingFile;
	WritingFile.open(Filename, std::ios::out);
	for (int i = 0; i < 5; i++)
	{
		WritingFile << Result[i] << std::endl;
	}
	//今回のリザルト書き込み
	WritingFile << (int)player->GetPosition().z << std::endl;
}


void SceneMain::WorldToScreen(DirectX::XMFLOAT3* screenPosition, const DirectX::XMFLOAT3& worldPosition)
{
	Framework& framework = Framework::Instance();

	Camera& camera = Camera::Instance();
	// ビューポート
	float viewportX = 0.0f;
	float viewportY = 0.0f;
	float viewportW = static_cast<float>(framework.GetScreenWidth());
	float viewportH = static_cast<float>(framework.GetScreenHeight());
	float viewportMinZ = 0.0f;
	float viewportMaxZ = 1.0f;

	// ビュー行列
	DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&camera.GetView());

	// プロジェクション行列
	DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&camera.GetProjection());

	// ワールド行列
	DirectX::XMMATRIX W = DirectX::XMMatrixIdentity();

	DirectX::XMVECTOR wp = DirectX::XMLoadFloat3(&worldPosition);

#if 0
	DirectX::XMVECTOR SPos = DirectX::XMVector3Project(wp, viewportX, viewportY, viewportW, viewportH, viewportMinZ, viewportMaxZ, P, V, W);
	DirectX::XMStoreFloat3(screenPosition, SPos);
#else 1
	// ワールド座標から NDC 座標へ変換
	DirectX::XMMATRIX WVP = W * V * P;
	DirectX::XMVECTOR NDCPosition = DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&worldPosition), WVP);
	DirectX::XMFLOAT3 ndcPosition;
	DirectX::XMStoreFloat3(&ndcPosition, NDCPosition);


	//viewportX = viewportY = viewportMinZ = 0.0f
	//ndc座標からスクリーン座標への変換
	screenPosition->x = viewportX + (1.0f + ndcPosition.x) * viewportW * 0.5;
	screenPosition->y = viewportY + (1.0f - ndcPosition.y) * viewportH * 0.5;
	screenPosition->z = viewportMinZ + ndcPosition.z * (viewportMaxZ - viewportMinZ);

#endif // 1

}


void SceneMain::ScreenToWorld(DirectX::XMFLOAT3* worldPosition, const DirectX::XMFLOAT3& screenPosition)
{
	Framework& framework = Framework::Instance();

	Camera& camera = Camera::Instance();
	// ビューポート
	float viewportX = 0.0f;
	float viewportY = 0.0f;
	float viewportW = static_cast<float>(framework.GetScreenWidth());
	float viewportH = static_cast<float>(framework.GetScreenHeight());
	float viewportMinZ = 0.0f;
	float viewportMaxZ = 1.0f;

	// ビュー行列
	DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&camera.GetView());

	// プロジェクション行列
	DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&camera.GetProjection());

	// ワールド行列
	DirectX::XMMATRIX W = DirectX::XMMatrixIdentity();

	DirectX::XMVECTOR sp = DirectX::XMLoadFloat3(&screenPosition);

#if 0
	DirectX::XMVECTOR WPos = DirectX::XMVector3Unproject(sp, viewportX, viewportY, viewportW, viewportH, viewportMinZ, viewportMaxZ, P, V, W);
	DirectX::XMStoreFloat3(worldPosition, WPos);
#else 1
	DirectX::XMVECTOR NDCpos = DirectX::XMVectorSet(2.0f * (screenPosition.x - viewportX) / viewportW - 1.0f,
		1.0f - 2.0f * (screenPosition.y - viewportY) / viewportH,
		(screenPosition.z - viewportMinZ) / (viewportMaxZ - viewportMinZ), 1.0f);

	DirectX::XMMATRIX WVP = W * V * P;
	DirectX::XMMATRIX IWVP = DirectX::XMMatrixInverse(nullptr, WVP);
	DirectX::XMVECTOR WPos = DirectX::XMVector3TransformCoord(NDCpos, IWVP);
	DirectX::XMStoreFloat3(worldPosition, WPos);

#endif // 1

}


void Scene::imGuiRender()
{
	// Rendering
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

//--------------------------------------------------------
//		ライトセット
//--------------------------------------------------------
void SceneMain::SetDirLight(DirectX::XMFLOAT3 dir, DirectX::XMFLOAT3 color)
{
	float d = sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);

	if (d > 0) { dir.x /= d; dir.y /= d; dir.z /= d; }
	LightDir = DirectX::XMFLOAT4(dir.x, dir.y, dir.z, 0);
	LightDir = DirectX::XMFLOAT4(dir.x, dir.y, dir.z, 0);
	DirLightColor = DirectX::XMFLOAT4(color.x, color.y, color.z, 1);
}