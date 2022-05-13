#include "scene.h"
#include "KeyInput.h"
#include "framework.h"
#include "collision.h"
#include "Input/Input.h"
#include "Camera.h"
#include "EnemyManager.h"
#include "ObstacleBlockManager.h"
#include "CoinManager.h"
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
	//コンスタントバッファ
	CreateConstantBuffer(&ConstantBuffer, sizeof(ConstantBufferForPerFrame));
	CreateConstantBuffer(&ConstantBufferBloom, sizeof(ConstantBufferForBloom));

	//フェード初期化
	FadeBlack = std::make_unique<Transition>();
	FadeBlack->init(device, L"Data/Image/unnamed.png", { 0.0f,0.0f }, { 1920.0f,1080.0f }, { 0.0f,0.0f }, { 1920.0f,1080.0f }, { 1.0f,1.0f,1.0f,1.0f });

	//スクリーン縦横サイズ変数
	float screenWidth = static_cast<float>(framework.GetScreenWidth());
	float screenHeight = static_cast<float>(framework.GetScreenHeight());

	//------------
	// シェーダー
	//------------
	//シェーダー初期化
	SkinnedShader = std::make_unique<SkinnedMeshShader>(device, "Shaders/skinned_mesh_vs.cso", "Shaders/skinned_mesh_ps.cso");
	NoLightShader = std::make_unique<StaticMeshShader>(device, "Shaders/NoLight_vs.cso", "Shaders/NoLight_ps.cso");
	ParticleShader = std::make_unique<PointSpriteShader>(device, "Shaders/PointSprite_vs.cso", "Shaders/PointSprite_gs.cso", "Shaders/PointSprite_ps.cso");
	BrightShader = std::make_unique<BrightSpriteShader>(device, "Shaders/bright_vs.cso", "Shaders/bright_ps.cso");
	BokehShader = std::make_unique<BokehSpriteShader>(device, "Shaders/bokeh_vs.cso", "Shaders/bokeh_ps.cso");

	//------------
	// カメラ
	//------------
	//カメラ初期化
	Camera& camera = Camera::Instance();

	//カメラの設定
	camera.SetLookAt(
		DirectX::XMFLOAT3(0.0f, 100.0f, -20.0f),  //カメラの位置
		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),      //カメラの焦点
		DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f)		 //カメラの上方向を決める
	);

	//カメラの視野角
	camera.SetPerspecticeFov(
		DirectX::XMConvertToRadians(45),
		screenWidth / screenHeight,
		0.1f,
		1000.0f
	);
	
	//カメラコントローラー初期化
	cameraController = new CameraController();


	/*skyMesh = std::make_shared<SkinnedMesh>(device, "Data/Sky/sky.obj",true);
	skyObj = std::make_unique<SkinnedObject>(skyMesh);
	skyObj->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	skyObj->SetScale(DirectX::XMFLOAT3(7.0f, 7.0f, 7.0f));
	skyObj->SetAngle(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));*/

	//---------------
	// パーティクル
	//---------------
	//パーティクル初期化
	//Particles = std::make_unique<ParticleSystem>(device, 10000);
	Particles = std::make_unique<ParticleSystem>(device, 200000);


	//------------
	// ブルーム
	//------------
	//ブルーム初期化
	FullScreen = std::make_unique<FullScreenQuad>(device);
	FullScreenBright = std::make_unique<FullScreenQuad>(device);
	FullScreenBokeh = std::make_unique<FullScreenQuad>(device);
	OffScreen = std::make_unique<FrameBuffer>(device, 1280 / SCREEN, 720 / SCREEN);
	Bright = std::make_unique<FrameBuffer>(device, 1280 / SCREEN, 720 / SCREEN);
	Bloom1 = std::make_unique<FrameBuffer>(device, 1280 / BLOOM1, 720 / BLOOM1);
	Bloom2 = std::make_unique<FrameBuffer>(device, 1280 / BLOOM2, 720 / BLOOM2);
	Bloom3 = std::make_unique<FrameBuffer>(device, 1280 / BLOOM3, 720 / BLOOM3);
	Bloom4 = std::make_unique<FrameBuffer>(device, 1280 / BLOOM4, 720 / BLOOM4);

	//------------
	// プレイヤー
	//------------
	//プレイヤー初期化
	player = new Player();

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
	// コイン 
	//------------
	//コインマネージャー初期化
	CoinManager& coinManager = CoinManager::Instance();
	//コイン初期化
	for (int i = 0; i < CoinMax; i++)
	{
		for (int j = 0; j < StageMax; j++)
		{
			coin[j][i] = new Coin();

			//座標指定
			coin[j][i]->SetPosition(DirectX::XMFLOAT3(
				stageBase[0]->squea.SpritPosition[0].x,
				-10.0f,
				stageBase[0]->squea.SpritPosition[0].z));

			//コインマネージャーに追加
			coinManager.Register(coin[j][i]);
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

	//------------
	// エネミー 
	//------------
	// エネミーマネージャー初期化
	/*EnemyManager& enemyManager = EnemyManager::Instance();
	enemy = new EnemyBlueSlime;
	enemy->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	enemyManager.Register(enemy);
	*/


	fadeInFlg = true;
	fadeOutFlg = false;
	ObstacleMove[0] = 0.0f;
	ObstacleMove[1] = -3.0f;

	//音楽初期化
	BGM = framework.GetSoundManager()->CreateSoundSource("Data/Sounds/stage.wav");

	//音楽再生
	//BGM->Play(true);

	//imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& IO = ImGui::GetIO();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(device, context);
	ImGui::StyleColorsClassic();
}

//--------------------------------------------------------
//		終了
//--------------------------------------------------------
void SceneMain::Finalize()
{
	ObstacleBlockManager::Instance().Clear();
	EnemyManager::Instance().Clear();
	StageManager::Instance().Clear();
	StageTileManager::Instance().Clear();
	CoinManager::Instance().Clear();

	//実体解放
	if (player != nullptr)
	{
		delete player;
		player = nullptr;
	}

	if (cameraController != nullptr)
	{
		delete cameraController;
		cameraController = nullptr;
	}
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

		//test sky
		/*skyObj->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, player->GetPosition().z));
		skyObj->Update(elapsedTime);*/

		//自機更新
		player->Update(elapsedTime);
		
		//ステージ全体の更新処理
		StageUpdate();

		//エネミーマネージャー更新
		/*EnemyManager::Instance().Update(elapsedTime);
		EnemyManager::Instance().GetEnemy(0)->SetPosition(DirectX::XMFLOAT3(stageBase[0]->squea.SpritPosition[0].x,0.0f,stageBase[0]->squea.SpritPosition[0].z));*/

		//コイン更新
		CoinManager::Instance().Update(elapsedTime);

		//障害物マネージャー更新
		ObstacleBlockManager::Instance().Update(elapsedTime);

		//ステージ更新
		StageManager::Instance().Update(elapsedTime);
		StageTileManager::Instance().Update(elapsedTime);

		//パーティクル更新
		Particles->Update(elapsedTime);
		Particles->Star({ player->GetPosition().x,player->GetPosition().y + 5.0f,player->GetPosition().z }, { 0.0f, 0.0f, -(rand() % 10001) * 0.002f - 0.001f }, 200000);

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
			//リザルトの順位計算
			ResultMeter();
			//終了処理
			Finalize();
			SceneManager::Instance().ChangeScene((new SceneClear()));
			return;
		}
	}
}

//ステージ全体の更新処理
void SceneMain::StageUpdate()
{
	//青のギミック処理
	if (player->GetBlueFlg() == true && ObstacleMove[0] > -3.5f) ObstacleMove[0] -= 0.1f;
	else if (player->GetBlueFlg() == false && ObstacleMove[0] < 0.0f) ObstacleMove[0] += 0.1f;
	if (player->GetBlueFlg() == false && ObstacleMove[1] > -3.5f) ObstacleMove[1] -= 0.1f;
	else if (player->GetBlueFlg() == true && ObstacleMove[1] < 0.0f) ObstacleMove[1] += 0.1f;

	//ステージタイルをランダムで選ばれたパターンごとに配置
	for (int j = 0; j < StageMax; j++)
	{
		for (int i = 0; i < StageTileMax; i++)
		{
			//ステージのHOLEの部分以外で配置
			//if (stageTile[j][i]->stageTileMap[stageBase[j]->Pattern][stageBase[j]->Rand][i] != HOLE)
			if (stageTile[j][i]->StageTileMap[stageBase[j]->EasyRand][j][i] != stageTile[0][0]->HOLE)
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

				//動かない
				//障害物１
				//if (stageTile[j][i]->stageTileMap[stageBase[j]->Pattern][stageBase[j]->Rand][i] == OB1)
				if (stageTile[j][i]->StageTileMap[stageBase[j]->EasyRand][j][i] == stageTile[0][0]->OB1)
				{
					//エネミーをステージを9等分して指定した位置にセット
					obstacle[j][0]->SetPosition(DirectX::XMFLOAT3(
						stageBase[j]->squea.SpritPosition[i].x,
						1.5f,
						stageBase[j]->squea.SpritPosition[i].z));
					//生存 後で無敵とかで使うかも
					obstacle[j][0]->SetExistFlg(true);
				}
				//障害物２
				//if (stageTile[j][i]->stageTileMap[stageBase[j]->Pattern][stageBase[j]->Rand][i] == OB2)
				if (stageTile[j][i]->StageTileMap[stageBase[j]->EasyRand][j][i] == stageTile[0][0]->OB2)
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
				if (stageTile[j][i]->StageTileMap[stageBase[j]->EasyRand][j][i] == stageTile[0][0]->OB3)
				{
					//エネミーをステージを9等分して指定した位置にセット
					obstacle[j][2]->SetPosition(DirectX::XMFLOAT3(
						stageBase[j]->squea.SpritPosition[i].x,
						1.5f,
						stageBase[j]->squea.SpritPosition[i].z));
					//生存
					obstacle[j][2]->SetExistFlg(true);
				}

				//動く
				//障害物4
				if (stageTile[j][i]->StageTileMap[stageBase[j]->EasyRand][j][i] == stageTile[0][0]->MOB1)
				{
					if (stageBase[j]->EasyRand == 2)
					{
						obstacle[j][3]->SetObstacleColor(stageTile[j][i]->StageTileMap[stageBase[j]->EasyRand][j][i]);
						//エネミーをステージを9等分して指定した位置にセット
						obstacle[j][3]->SetPosition(DirectX::XMFLOAT3(
							stageBase[j]->squea.SpritPosition[i].x,
							1.5f + ObstacleMove[0],
							stageBase[j]->squea.SpritPosition[i].z));
					}
					else
					{
						//エネミーをステージを9等分して指定した位置にセット
						obstacle[j][3]->SetPosition(DirectX::XMFLOAT3(
							stageBase[j]->squea.SpritPosition[i].x,
							1.5f,
							stageBase[j]->squea.SpritPosition[i].z));
					}
					//生存
					obstacle[j][3]->SetExistFlg(true);
				}

				//障害物5
				if (stageTile[j][i]->StageTileMap[stageBase[j]->EasyRand][j][i] == stageTile[0][0]->MOB2)
				{
					if (stageBase[j]->EasyRand == 2)
					{
						obstacle[j][4]->SetObstacleColor(stageTile[j][i]->StageTileMap[stageBase[j]->EasyRand][j][i]);
						/*	if (player->GetBlueFlg() == true && ObstacleMove[0] > -3.5f) ObstacleMove[0] -= 0.05f;
							else if (player->GetBlueFlg() == false && ObstacleMove[0] < 0.0f) ObstacleMove[0] += 0.05f;*/
							//エネミーをステージを9等分して指定した位置にセット
						obstacle[j][4]->SetPosition(DirectX::XMFLOAT3(
							stageBase[j]->squea.SpritPosition[i].x,
							1.5f + ObstacleMove[0],
							stageBase[j]->squea.SpritPosition[i].z));
					}
					else
					{
						//エネミーをステージを9等分して指定した位置にセット
						obstacle[j][4]->SetPosition(DirectX::XMFLOAT3(
							stageBase[j]->squea.SpritPosition[i].x,
							1.5f,
							stageBase[j]->squea.SpritPosition[i].z));
					}
					//生存
					obstacle[j][4]->SetExistFlg(true);
				}

				//障害物6
				if (stageTile[j][i]->StageTileMap[stageBase[j]->EasyRand][j][i] == stageTile[0][0]->MOB3)
				{
					if (stageBase[j]->EasyRand == 2)
					{
						obstacle[j][5]->SetObstacleColor(stageTile[j][i]->StageTileMap[stageBase[j]->EasyRand][j][i]);
						//エネミーをステージを9等分して指定した位置にセット
						obstacle[j][5]->SetPosition(DirectX::XMFLOAT3(
							stageBase[j]->squea.SpritPosition[i].x,
							1.5f + ObstacleMove[1],
							stageBase[j]->squea.SpritPosition[i].z));
					}
					else
					{
						//エネミーをステージを9等分して指定した位置にセット
						obstacle[j][5]->SetPosition(DirectX::XMFLOAT3(
							stageBase[j]->squea.SpritPosition[i].x,
							1.5f,
							stageBase[j]->squea.SpritPosition[i].z));
					}
					//生存
					obstacle[j][5]->SetExistFlg(true);
				}

				if (stageTile[j][i]->StageTileMap[stageBase[j]->EasyRand][j][i] == stageTile[0][0]->COIN)
				{
					coin[j][0]->SetPosition(DirectX::XMFLOAT3(
						stageBase[j]->squea.SpritPosition[i].x,
						1.5f,
						stageBase[j]->squea.SpritPosition[i].z));
				}

				//-------------------
				// タイルカラーセット 
				//-------------------
				//今見ているステージタイルをランダムで決まったステージベースのstageTileMap番号をカラーをセット
				//stageTile[j][i]->SetTileColor(stageTile[j][i]->stageTileMap[stageBase[j]->Pattern][stageBase[j]->Rand][i]);
				stageTile[j][i]->SetTileColor(stageTile[j][i]->StageTileMap[stageBase[j]->EasyRand][j][i]);
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
				stageTile[j][i]->SetPosition(DirectX::XMFLOAT3(stageBase[j]->squea.SpritPosition[6].x, -20.0f, stageBase[j]->squea.SpritPosition[6].z - 100.0f));

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
		/*for (int i = 0; i < StageMax; i++)
		{
			ImGui::InputInt("Rand", &stageBase[i]->EasyRand);
		}*/
		ImGui::TextColored(ImVec4(1, 1, 0, 1), u8"---------StagePatternNum---------");
		ImGui::InputInt("Rand", &stageBase[0]->EasyRand);
		ImGui::Text("true(1)/false(0):%d", player->GetBlueFlg());
		ImGui::InputFloat("ObstacleMove[0]", &ObstacleMove[0]);
		ImGui::InputFloat("ObstacleMove[1]", &ObstacleMove[1]);
		ImGui::SliderFloat(u8"bloom", &threshold, 10.0f, 0.0f);
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
	CoinManager::Instance().DrawDebugGUI();
	//StageTileManager::Instance().DrawDebugGUI();
}


//--------------------------------------------------------
//		描画
//--------------------------------------------------------
void SceneMain::RenderBloom()
{
	// カメラパラメータ設定
	Camera& camera = Camera::Instance();

	//Framework& framework = Framework::Instance();
	//ID3D11DeviceContext* context = framework.GetImmediateContext();
	
	//ラスタライザ―設定
	context->RSSetState(framework.GetRasterizerState0());
	//デプスステンシルステート設定
	context->OMSetDepthStencilState(framework.GetNoneDepthStencilState(), 1);
	// ビューポートの設定
	framework.SetViewPort(1280.0f / SCREEN, 720.0f / SCREEN);
	{
		DirectX::XMFLOAT4 lightDirection(0.0f, -1.0f, 0.0f, 0.0f);
		DirectX::XMFLOAT4X4 view = camera.GetView();
		DirectX::XMFLOAT4X4 projection = camera.GetProjection();
		DirectX::XMFLOAT4 materialColor(1.0f, 1.0f, 1.0f, 1.0f);
		{
			// コンスタントバッファ設定
			ConstantBufferForPerFrame cb{};
			cb.AmbientColor = ambientColor;
			cb.LightColor = DirLightColor;
			cb.EyePos.x = camera.GetEye().x;
			cb.EyePos.y = camera.GetEye().y;
			cb.EyePos.z = camera.GetEye().z;
			cb.EyePos.w = 1.0f;

			context->UpdateSubresource(ConstantBuffer.Get(), 0, NULL, &cb, 0, 0);
			context->PSSetConstantBuffers(2, 1, ConstantBuffer.GetAddressOf());
		}

		framework.SetSampler(0);
		//オフスクリーン有効
		OffScreen->Activate(context);
		OffScreen->Clear(context);

		Particles->Render(context, ParticleShader.get(), view, projection, framework.GetBlendState(Blender::BS_ALPHA));
		context->OMSetDepthStencilState(framework.GetDepthStencilState(), 1);
		//プレイヤー描画
		player->Render(context, view, projection, lightDirection, materialColor, false);
		CoinManager::Instance().Render(context, view, projection, lightDirection, materialColor, false);
		//EnemyManager::Instance().Render(context, view, projection, lightDirection, materialColor, false);
		StageTileManager::Instance().Render(context, view, projection, lightDirection, materialColor, false);
		ObstacleBlockManager::Instance().Render(context, view, projection, lightDirection, materialColor, false);
		//進んだ距離表示
		float fontSize = 16;
		const char* name = "M";
		Sprite* Font = framework.GetFont();
		std::string pMeter = to_string((int)player->GetPosition().z);
		pMeter += name;
		Font->TextOut(context, pMeter, 0, 0, 96, 96, 0, 1, 1, 1);
		//オフスクリーン無効
		OffScreen->Deactivate(context);
	}
	//Bright
	{
		//ラスタライザ―設定
		context->RSSetState(framework.GetRasterizerState0());
		//デプスステンシルステート設定
		context->OMSetDepthStencilState(framework.GetDepthStencilState(), 1);
		// ビューポートの設定
		framework.SetViewPort(1280.0f / SCREEN, 720.0f / SCREEN);
		{
			DirectX::XMFLOAT4 lightDirection(0.0f, -1.0f, 0.0f, 0.0f);
			DirectX::XMFLOAT4X4 view = camera.GetView();
			DirectX::XMFLOAT4X4 projection = camera.GetProjection();
			DirectX::XMFLOAT4 materialColor(1.0f, 1.0f, 1.0f, 1.0f);
			{
				// コンスタントバッファ設定
				ConstantBufferForBloom cb{};
				cb.threshold = threshold;
				context->UpdateSubresource(ConstantBufferBloom.Get(), 0, NULL, &cb, 0, 0);
				context->PSSetConstantBuffers(3, 1, ConstantBufferBloom.GetAddressOf());
			}

			//オフスクリーン有効
			Bright->Activate(context);
			Bright->Clear(context);

			framework.SetSamplerCramp(0);
			ID3D11ShaderResourceView* ShaderResourceViews[2]{
				OffScreen->getShaderResourceView(0),
				OffScreen->getShaderResourceView(1),
			};
			FullScreenBright->Blit(context, ShaderResourceViews, 0, 2, BrightShader.get());


			//オフスクリーン無効
			Bright->Deactivate(context);
		}
	}
	//Bloom1
	{
		
		//ラスタライザ―設定
		context->RSSetState(framework.GetRasterizerState0());
		//デプスステンシルステート設定
		context->OMSetDepthStencilState(framework.GetDepthStencilState(), 1);
		// ビューポートの設定
		framework.SetViewPort(1280.0f / BLOOM1, 720.0f / BLOOM1);
		{
			DirectX::XMFLOAT4 lightDirection(0.0f, -1.0f, 0.0f, 0.0f);
			DirectX::XMFLOAT4X4 view = camera.GetView();
			DirectX::XMFLOAT4X4 projection = camera.GetProjection();
			DirectX::XMFLOAT4 materialColor(1.0f, 1.0f, 1.0f, 1.0f);
			{
				// コンスタントバッファ設定
				ConstantBufferForBloom cb{};
				cb.threshold = threshold;
				context->UpdateSubresource(ConstantBufferBloom.Get(), 0, NULL, &cb, 0, 0);
				context->PSSetConstantBuffers(3, 1, ConstantBufferBloom.GetAddressOf());
			}

			//オフスクリーン有効
			Bloom1->Activate(context);
			Bloom1->Clear(context);

			framework.SetSamplerCramp(0);
			ID3D11ShaderResourceView* ShaderResourceViews[2]{
				Bright->getShaderResourceView(0),
				Bright->getShaderResourceView(1),
			};
			FullScreenBright->Blit(context, ShaderResourceViews, 0, 2, BrightShader.get());

			//オフスクリーン無効
			Bloom1->Deactivate(context);
		}
	}
	//Bloom2
	{
		//ラスタライザ―設定
		context->RSSetState(framework.GetRasterizerState0());
		//デプスステンシルステート設定
		context->OMSetDepthStencilState(framework.GetDepthStencilState(), 1);
		// ビューポートの設定
		framework.SetViewPort(1280.0f / BLOOM2, 720.0f / BLOOM2);
		{
			DirectX::XMFLOAT4 lightDirection(0, -1, 0, 0);
			DirectX::XMFLOAT4X4 view = camera.GetView();
			DirectX::XMFLOAT4X4 projection = camera.GetProjection();
			DirectX::XMFLOAT4 materialColor(1.0f, 1.0f, 1.0f, 1.0f);
			{
				// コンスタントバッファ設定
				ConstantBufferForBloom cb{};
				cb.threshold = threshold;
				context->UpdateSubresource(ConstantBufferBloom.Get(), 0, NULL, &cb, 0, 0);
				context->PSSetConstantBuffers(3, 1, ConstantBufferBloom.GetAddressOf());
			}

			//オフスクリーン有効
			Bloom2->Activate(context);
			Bloom2->Clear(context);

			framework.SetSamplerCramp(0);
			ID3D11ShaderResourceView* ShaderResourceViews[2]{
				Bloom1->getShaderResourceView(0),
				Bloom1->getShaderResourceView(1),
			};
			FullScreenBright->Blit(context, ShaderResourceViews, 0, 2, BrightShader.get());

			//オフスクリーン無効
			Bloom2->Deactivate(context);
		}
	}
	//Bloom3
	{
		//ラスタライザ―設定
		context->RSSetState(framework.GetRasterizerState0());
		//デプスステンシルステート設定
		context->OMSetDepthStencilState(framework.GetDepthStencilState(), 1);
		// ビューポートの設定
		framework.SetViewPort(1280.0f / BLOOM3, 720.0f / BLOOM3);
		{
			DirectX::XMFLOAT4 lightDirection(0.0f, -1.0f, 0.0f, 0.0f);
			DirectX::XMFLOAT4X4 view = camera.GetView();
			DirectX::XMFLOAT4X4 projection = camera.GetProjection();
			DirectX::XMFLOAT4 materialColor(1.0f, 1.0f, 1.0f, 1.0f);
			{
				// コンスタントバッファ設定
				ConstantBufferForBloom cb{};
				cb.threshold = threshold;
				context->UpdateSubresource(ConstantBufferBloom.Get(), 0, NULL, &cb, 0, 0);
				context->PSSetConstantBuffers(3, 1, ConstantBufferBloom.GetAddressOf());
			}
		    
			//オフスクリーン有効
			Bloom3->Activate(context);
			Bloom3->Clear(context);

			framework.SetSamplerCramp(0);
			ID3D11ShaderResourceView* ShaderResourceViews[2]{
				Bloom2->getShaderResourceView(0),
				Bloom2->getShaderResourceView(1),
			};
			FullScreenBright->Blit(context, ShaderResourceViews, 0, 2, BrightShader.get());

			//オフスクリーン無効
			Bloom3->Deactivate(context);
		}
	}
	//Bloom4
	{
		
		//ラスタライザ―設定
		context->RSSetState(framework.GetRasterizerState0());
		//デプスステンシルステート設定
		context->OMSetDepthStencilState(framework.GetDepthStencilState(), 1);
		// ビューポートの設定
		framework.SetViewPort(1280.0f / BLOOM4, 720.0f / BLOOM4);
		{
			DirectX::XMFLOAT4 lightDirection(0.0f, -1.0f, 0.0f, 0.0f);
			DirectX::XMFLOAT4X4 view = camera.GetView();
			DirectX::XMFLOAT4X4 projection = camera.GetProjection();
			DirectX::XMFLOAT4 materialColor(1.0f, 1.0f, 1.0f, 1.0f);
			{
				// コンスタントバッファ設定
				ConstantBufferForBloom cb{};
				cb.threshold = threshold;
				context->UpdateSubresource(ConstantBufferBloom.Get(), 0, NULL, &cb, 0, 0);
				context->PSSetConstantBuffers(3, 1, ConstantBufferBloom.GetAddressOf());
			}

			
			//オフスクリーン有効
			Bloom4->Activate(context);
			Bloom4->Clear(context);

			framework.SetSamplerCramp(0);
			ID3D11ShaderResourceView* ShaderResourceViews[2]{
				Bloom3->getShaderResourceView(0),
				Bloom3->getShaderResourceView(1),
			};
			FullScreenBright->Blit(context, ShaderResourceViews, 0, 2, BrightShader.get());

			//オフスクリーン無効
			Bloom4->Deactivate(context);
		}
	}

	//レンダーターゲットの回復
	//ID3D11RenderTargetView* backbuffer = framework.GetRenderTargetView();
	//context->OMSetRenderTargets(1, &backbuffer, framework.GetDepthStencilView());
}

void SceneMain::Render()
{
	imGuiUpdate();

	/*Framework& framework = Framework::Instance();

	ID3D11DeviceContext* context = framework.GetImmediateContext();*/

	//ブルーム
	RenderBloom();

	// 画面クリア
	FLOAT color[] = { 0.0f, 0.0f, 0.0f, 1.0f };

	ID3D11RenderTargetView* rtv = framework.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = framework.GetDepthStencilView();

	// レンダーターゲットを設定
	context->OMSetRenderTargets(1, &rtv, dsv);
	context->ClearRenderTargetView(rtv, color);
	context->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->OMSetDepthStencilState(framework.GetNoneDepthStencilState(), 1);

	// ビューポートの設定
	framework.SetViewPort(static_cast<float>(framework.GetScreenWidth()), static_cast<float>(framework.GetScreenHeight()));

	// カメラパラメータ設定
	Camera& camera = Camera::Instance();

	ID3D11BlendState* BsAlpha = framework.GetBlendState(Blender::BS_ALPHA);
	ID3D11BlendState* BsAdd = framework.GetBlendState(Blender::BS_ADD);
	context->OMSetBlendState(BsAlpha, nullptr, 0xFFFFFFFF);

	DirectX::XMFLOAT4 lightDirection(0.0f, -1.0f, 0.0f, 0.0f);

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

	//ブルーム関係レンダリング
	framework.SetSamplerCramp(0);
	ID3D11ShaderResourceView* ShaderResourceViews[2]{
		OffScreen->getShaderResourceView(0),
		OffScreen->getShaderResourceView(1),
	};
	FullScreen->Blit(context, ShaderResourceViews, 0, 2, PixelShaders[1].Get());

	context->OMSetBlendState(BsAdd, nullptr, 0xFFFFFFFF);

	ID3D11ShaderResourceView* BokehShaderResourceViews1[2]{
		Bloom1->getShaderResourceView(0),
		Bloom1->getShaderResourceView(1),
	};
	FullScreenBokeh->Blit(context, BokehShaderResourceViews1, 0, 2, BokehShader.get());

	ID3D11ShaderResourceView* BokehShaderResourceViews2[2]{
		Bloom2->getShaderResourceView(0),
		Bloom2->getShaderResourceView(1),
	};
	FullScreenBokeh->Blit(context, BokehShaderResourceViews2, 0, 2, BokehShader.get());

	ID3D11ShaderResourceView* BokehShaderResourceViews3[2]{
		Bloom3->getShaderResourceView(0),
		Bloom3->getShaderResourceView(1),
	};
	FullScreenBokeh->Blit(context, BokehShaderResourceViews3, 0, 2, BokehShader.get());

	ID3D11ShaderResourceView* BokehShaderResourceViews4[2]{
		Bloom4->getShaderResourceView(0),
		Bloom4->getShaderResourceView(1),
	};
	FullScreenBokeh->Blit(context, BokehShaderResourceViews4, 0, 2, BokehShader.get());

	context->OMSetBlendState(BsAlpha, nullptr, 0xFFFFFFFF);


	//Screen->Render(context, ScreenTexture.get(), 0, 0, SCREEN.x, SCREEN.y, 0, 0, 1024, 512);

	//パーティクル描画
	//Particles->Render(context, ParticleShader.get(), view, projection, BsAlpha);

	/*context->OMSetDepthStencilState(framework.GetDepthStencilState(), 1);
	framework.SetSampler(0);
	Particles->Render(context, ParticleShader.get(), view, projection, framework.GetBlendState(Blender::BS_ALPHA));*/

	//プレイヤー描画
	//player->Render(context, view, projection, lightDirection, materialColor, false);

	//ステージ描画
	//StageManager::Instance().Render(context, view, projection, lightDirection, materialColor, false);
	//StageTileManager::Instance().Render(context, view, projection, lightDirection, materialColor, false);

	//障害物描画
	//ObstacleBlockManager::Instance().Render(context, view, projection, lightDirection, materialColor, false);

	// エネミー描画
	//EnemyManager::Instance().Render(context, view, projection, lightDirection, materialColor, false);

	//skyObj->Render(context, view, projection, lightDirection, DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),false);

	// ワールド座標からスクリーン座標へ
	DirectX::XMFLOAT3 worldPosition = player->GetPosition();
	DirectX::XMFLOAT3 screenPosition;
	worldPosition.y += 1.5f;
	WorldToScreen(&screenPosition, worldPosition);

	////進んだ距離表示
	//float fontSize = 16;
	//const char* name = "M";
	//Sprite* Font = framework.GetFont();
	//std::string pMeter = to_string((int)player->GetPosition().z);
	//pMeter += name;
	//Font->TextOut(context, pMeter, 0, 0, 64, 64 ,0,183,206,1);

	FadeBlack->render(context);
	
	//imGui
	Scene::imGuiRender();
}

void Scene::imGuiRender()
{
	// Rendering
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
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


//--------------------------------------------------------
//　　ワールド・スクリーン変換関数	
//--------------------------------------------------------
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