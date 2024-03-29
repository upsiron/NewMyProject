#include <imgui.h>
#include "Player.h"
#include "Input/Input.h"
#include "Camera.h"
#include "ObstacleBlockManager.h"
#include "CoinManager.h"
#include "EnemyManager.h"
#include "Collision.h"
#include "KeyInput.h"


static Player* instance = nullptr;
static DirectX::XMFLOAT3 SavePos;

// インスタンス取得
Player& Player::Instance()
{
	return *instance;
}

// コンストラクタ
Player::Player()
{
	// インスタンスポイント設定
	instance = this;

	Framework& framework = Framework::Instance();
	ID3D11Device* device = framework.GetDevice();

	//アニメーション初期化
	std::vector<std::string> Animationfilenames{
		"Data/Banana/Animations/runAnime.fbx",
		"Data/Banana/Animations/Jump.fbx",
		"Data/Banana/Animations/RunningForwardFlip.fbx",
		"Data/Banana/Animations/KnockedOut.fbx",
		"Data/Banana/Animations/KnockOutRight.fbx",
		"Data/Banana/Animations/KnockOutLeft.fbx",
		"Data/Banana/Animations/KnockOutFront.fbx",
	}; 

	//プレイヤー初期化
	playerMesh = std::make_shared<SkinnedMesh>(device, "Data/Banana/banana.fbx", Animationfilenames, true);
	playerObj = std::make_unique<SkinnedObject>(playerMesh);
	SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	SetScale(DirectX::XMFLOAT3(0.013f, 0.013f, 0.013f));
	SetAngle(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));

	//プレイヤーカラー設定
	Color = { 0.7f, 0.7f, 0.7f, 1.0f };

	//スクロールスピード初期化
	playerSpeed = 0.25f;
	oldPlayerSpeed = 0.25f;

	//コイン初期化
	for (int i = 0; i < 3; i++)
	{
		coinState[i] = 0;
		coinAngle[i] = 0.1f;
		coinPositionY[i] = 0.0f;
	}

	//ゲームオーバーフラグ初期化
	gameOverFlg = false;

	//スピードアップUI表示用
	speedLevelCount = 0;
	renderTimer = 0;

	//プレイヤーSE初期化
	jumpSE = framework.GetSoundManager()->CreateSoundSource("Data/Sounds/Jump.wav");
	jumpEndSE = framework.GetSoundManager()->CreateSoundSource("Data/Sounds/JumpEnd.wav");
	collisionSE = framework.GetSoundManager()->CreateSoundSource("Data/Sounds/Collision.wav");
	coinSE = framework.GetSoundManager()->CreateSoundSource("Data/Sounds/Coin.wav");
	gimmickSE = framework.GetSoundManager()->CreateSoundSource("Data/Sounds/Gimmick.wav");
	speedUpSE = framework.GetSoundManager()->CreateSoundSource("Data/Sounds/SpeedUp.wav");
}

// デストラクタ
Player::~Player()
{
}

// プレイヤー更新処理
void Player::Update(float elapsedTime)
{
	//リリース用
	GamePad& gamePad = Input::Instance().GetGamePad();
	if (gamePad.GetButtonDown() & GamePad::BTN_B || KeyInput::KeyRelease() & KEY_START)debugFlg = true;

	//デバッグ用
	//if (gamePad.GetButtonDown() & GamePad::BTN_Y || KeyInput::KeyRelease() & KEY_START)debugFlg = !debugFlg;

	//現在のelapsedTime保持
	saveElapsedTime = elapsedTime;

	//位置をセット
	playerObj->SetPosition(position);
	playerObj->SetAngle(angle);
	playerObj->SetScale(scale);

	//自動スクロール
	if(debugFlg)position.z += playerSpeed;

	//ギミック更新処理
	if (!playerInvincibleFlg)GimmickUpdate();
	
	// 速力更新処理
	UpdateVelocity(elapsedTime);
	
	// 移動入力
	if(!gameOverFlg)InputMove(elapsedTime);

	// ジャンプ入力
	if (!gameOverFlg)InputJump();

	// プレイヤーと障害物との衝突判定
	if(!playerInvincibleFlg)CollisionPlayerVsObstacle();

	// プレイヤーとコインとの衝突判定
	CollisionPlayerVsCoin();

	//プレイヤーの無敵処理
	InvincibleUpdate();

	/*if (KeyInput::KeyRelease() & KEY_X && playerCoinCount == 3 && playerSpeed > 0.25f)
	{
		PlayerSpeedDown(0.1f);
		playerCoinCount = 0;
	}*/

	//プレイヤーの更新
	playerObj->Update(elapsedTime);

	//落下ゲームオーバー
	if (position.y < -1.5f)
	{
		playerSpeed = 0.0f;
		gameOverFlg = true;
	}

	//現在のプレイヤーのポジションを保持
	SavePos = position;
}

void Player::Render(ID3D11DeviceContext* immediateContext,
	const DirectX::XMFLOAT4X4& view,
	const DirectX::XMFLOAT4X4& projection,
	const DirectX::XMFLOAT4& light,
	const DirectX::XMFLOAT4& materialColor,
	bool wireframe)
{
	//プレイヤーの描画
	playerObj->Render(immediateContext, view, projection, light, Color, wireframe);
}

//デバック用GUI
void Player::DrawDebugGUI()
{
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None))
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
			//地面についているかどうか
			ImGui::Text("true(1)/false(0):%d", IsGround());
			//デバッグ
			GamePad& gamePad = Input::Instance().GetGamePad();
			if (gamePad.GetButtonDown() & GamePad::BTN_Y)debugFlg = !debugFlg;
			if (ImGui::Button("Debug"))debugFlg = true;
			if (ImGui::Button("NoDebug"))debugFlg = false;
			ImGui::SliderFloat("scroll", &oldPlayerSpeed, 0.0f, 1.0f);
			ImGui::SliderFloat("jump", &jumpSpeed, 0.0f, 100.0f);
			ImGui::SliderFloat("gravity", &gravity, -100.0f, 0.0f);
			ImGui::InputInt("playerCoinCount", &playerCoinCount, 0.0f, 100.0f);
		}
	}
	ImGui::End();
}

//プレイヤー関係ギミック更新処理
void Player::GimmickUpdate()
{
	//プレイヤー色とスピードの初期値
	SetPlayerColor({ 1.0f,1.0f,1.0f,1.0f });
	playerSpeed = oldPlayerSpeed;

	//////////////////
	//赤パネル
	//////////////////
	if (RedGimmickFlg)
	{
		playerCoinCount = 0;
		SetPlayerColor({ 1.0f,0.0f,0.0f,1.0f });
	}

	//////////////////
	//青パネル
	//////////////////
	//スクロール加速処理
	if (BlueGimmickFlg)blueGimmickTime = 300.0f;
	//ギミックタイムの時間だけスクロールスピードを上げる
	if (blueGimmickTime >= 0.0f)
	{
		blueGimmickTime--;
		playerSpeed = oldPlayerSpeed + 0.15f;
		SetPlayerColor({ 0.0f,1.0f,2.0f,1.0f });
	}

	//////////////////
	//黄パネル
	//////////////////
	if (YellowGimmickFlg)yellowGimmickTime = 300.0f;
	//ギミックタイムの時間だけ左右操作を反転
	if (yellowGimmickTime >= 0.0f)
	{
		yellowGimmickTime--;
		reversalFlg = true;
		SetPlayerColor({ 2.0f,1.0f,0.0f,1.0f });
	}
	else
	{
		reversalFlg = false;
	}

	//////////////////
	//紫パネル
	//////////////////
	if (PurpleGimmickFlg)purpleGimmickTime = 300.0f;
	//ギミックタイムの時間だけスクロールスピードを下げる
	if (purpleGimmickTime >= 0.0f)
	{
		purpleGimmickTime--;
		playerSpeed = oldPlayerSpeed - 0.15f;
		SetPlayerColor({ 1.0f,0.0f,1.0f,1.0f });
	}

	//青と黄色を踏んだ時
	if (blueGimmickTime >= 0.0f && yellowGimmickTime >= 0.0f)
	{
		SetPlayerColor({ 0.0f,1.0f,0.0f,1.0f });
	}

	
	///////////////
	//緑パネル
	///////////////
	//大ジャンプ処理
	if (GreenGimmickFlg)
	{
		if (jumpCount < jumpLimit)
		{
			//ジャンプアニメーションセット
			playerObj->SetMotion(FLIP, 0, 0.02f);
			jumpCount++;
			Jump(28.5f);
		}
	}
}

//プレイヤー無敵更新処理
void Player::InvincibleUpdate()
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	//無敵の準備完了を知らせるプレイヤー点滅処理
	if (playerCoinCount >= 5)
	{
		SetPlayerColor({ R,R,R,1.0f });
		switch (colorState)
		{
		case 0:
			R -= 0.1f;
			if (R < 0.5f)colorState = 1;
			break;
		case 1:
			R += 0.1f;
			if (R > 1.0f)colorState = 0;
			break;
		}

	}

	//5個コインをためてBボタンで無敵モードに入る
	if (playerCoinCount >= 5 && gamePad.GetButtonDown() & GamePad::BTN_B)
	{
		playerInvincibleTime = 600;
		playerCoinCount -= 5;
	}
	if (playerInvincibleTime >= 0)
	{
		//時間を減らしてる
		playerInvincibleTime--;

		playerInvincibleFlg = true;

		//プレイヤーのスピードを無敵中だけアップ
		playerSpeed = oldPlayerSpeed + 0.40f;

		//無敵中がわかりやすいように色を変えている
		SetPlayerColor({ R,G,B,1.0f });
		switch (colorState)
		{
		case 0:
			R -= 0.1f;
			if (R < 0.0f)colorState = 1;
			break;
		case 1:
			R += 0.1f;
			if (R > 1.0f)colorState = 2;
			break;
		case 2:
			G -= 0.1f;
			if (G < 0.0f)colorState = 3;
			break;
		case 3:
			G += 0.1f;
			if (G > 1.0f)colorState = 4;
			break;
		case 4:
			B -= 0.1f;
			if (B < 0.0f)colorState = 5;
			break;
		case 5:
			B += 0.1f;
			if (B > 1.0f)colorState = 0;
			break;
		}
	}
	else playerInvincibleFlg = false;
}

// スティック入力値から移動ベクトルを取得
DirectX::XMFLOAT3 Player::GetMoveVec()const
{
	// 入力情報を取得
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ax = gamePad.GetAxisLX();
	float ay = gamePad.GetAxisLY();
	//float ay = 0.0f;

	// カメラ方向とスティックの入力値によって進行方向を計算する
	Camera& camera = Camera::Instance();
	const DirectX::XMFLOAT3& cameraRight = camera.GetRight();
	const DirectX::XMFLOAT3& cameraFront = camera.GetFront();

	// 移動ベクトルはXZ平面に水平なベクトルになるようにする

	// カメラ右方向ベクトルをXZ単位ベクトルに変換
	float cameraRightX = cameraRight.x;
	float cameraRightZ = cameraRight.z;
	float cameraRightLength = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);
	if (cameraRightLength > 0.0f)
	{
		// 単位ベクトル化
		cameraRightX /= cameraRightLength;
		cameraRightZ /= cameraRightLength;
	}

	// カメラ前方向ベクトルをXZ単位ベクトルに変換
	float cameraFrontX = cameraFront.x;
	float cameraFrontZ = cameraFront.z;
	float cameraFrontLength = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);
	if (cameraFrontLength > 0.0)
	{
		// 単位ベクトル化
		cameraFrontX /= cameraFrontLength;
		cameraFrontZ /= cameraFrontLength;
	}

	// スティックの水平入力値をカメラ右方向に反映し、
	// スティックの垂直入力値をカメラ前方向に反映し、
	// 進行ベクトルを計算
	DirectX::XMFLOAT3 vec;
	//黄色の床を踏んだ時の左右反転処理
	if (reversalFlg)vec.x = -ax;
	else vec.x = ax;
	//vec.x = (cameraRightX * ax) + (cameraFrontX * ay);
	//vec.z = (cameraRightZ * ax) + (cameraFrontZ * ay);
	if (debugFlg)
	{
		vec.z = 0.0f;
	}
	else
	{
		//vec.z = 0.0f;
		vec.z = ay;
	}
	// Y軸方向には移動しない
	vec.y = 0.0f;

	return vec;
}

// 移動入力処理
void Player::InputMove(float elapsedTime)
{
	//左右の移動制限
	if (position.x > stageSideEndPos)position.x = SavePos.x;
	if (position.x < -1.0f * stageSideEndPos)position.x = SavePos.x;

	// 進行ベクトルを取得
	DirectX::XMFLOAT3 moveVec = GetMoveVec();

	// 移動処理
	Move(moveVec.x, moveVec.z, moveSpeed);

	// 旋回処理
	//Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);

	//移動方向の向きを向く
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ax = gamePad.GetAxisLX();

	if (ax > 0.0f)
	{
		if(angle.y < 0.30f)angle.y += 0.02f;
		playerObj->SetAngle(angle);
	}
	else if (ax < 0.0f)
	{
		if (angle.y > -0.30f)angle.y -= 0.02f;
		playerObj->SetAngle(angle);
	}
	else
	{
		angle.y = 0.0f;
		playerObj->SetAngle(angle);
	}
}

//プレイヤーとコインの当たり判定
void Player::CollisionPlayerVsCoin()
{
	CoinManager& coinManager = CoinManager::Instance();
	int coinCount = coinManager.GetCoinCount();
	for (int i = 0; i < coinCount; i++)
	{
		Object* object = coinManager.GetCoin(i);
		if (!object->GetExistFlg())continue;

		//コインが当たっている時といない時で状態を変える
		if (debugFlg && Collision::HitSphere(position, 0.4f, { object->GetPosition().x,  object->GetPosition().y - 1.5f, object->GetPosition().z }, 0.4f))
		{
			//コインSE鳴らす
			coinSE->Play(false);

			coinState[i] = 1;
		}
		else
		{
			coinState[i] = 0;
		}

		//コインの回転量を設定
		if (coinState[i] == 1 && coinAngle[i] > 0.0f)
		{
			coinAngle[i] += 0.05f;
			coinPositionY[i] += 2.5f;
		}
		else
		{
			if (coinAngle[i] > 0.1f)coinAngle[i] -= 0.01f;
			if (coinPositionY[i] > 0.1f)coinPositionY[i] -= 0.1f;
		}

		//コイン数をカウント
		if (coinPositionY[i] > 0 && coinState[i] == 1)
		{
			playerCoinCount = oldPlayerCoinCount + 1;
		}
		else
		{
			oldPlayerCoinCount = playerCoinCount;
		}

		//移動量セット
		object->SetMovePosition(coinPositionY[i]);
		//回転量セット
		object->SetMoveAngle(coinAngle[i]);
	}
}

// プレイヤーと障害物の衝突判定
void Player::CollisionPlayerVsObstacle()
{
	ObstacleBlockManager& obstacleBlockManager = ObstacleBlockManager::Instance();

	// 全ての障害物を見て衝突しているかチェック
	int obstacleCount = obstacleBlockManager.GetObstacleCount();
	for (int i = 0; i < obstacleCount; ++i)
	{
		Object* obstacle = obstacleBlockManager.GetObstacle(i);

		//描画されてないなら衝突処理を飛ばす
		if (!obstacle->GetExistFlg())continue;

		// 衝突処理
		/////////////////////
		//LeftSide　当たり判定
		/////////////////////
		//プレイヤーが障害物より右にいるなら左壁の当たり判定処理
		if (position.x > obstacle->GetPosition().x + obstacle->GetScale().x &&
			position.z > obstacle->GetPosition().z - obstacle->GetScale().z && 
			position.y <= obstacle->GetPosition().y + obstacle->GetScale().y)
		{
			//左壁に当たってるかチェック
			if (Collision::SideLeftWallVsPlayer(
				obstacle->ObstacleUpRightTop,
				obstacle->ObstacleUpRightBottom,
				obstacle->ObstacleDownRightTop,
				obstacle->ObstacleDownRightBottom,
				position)
				)
			{
				position.x = SavePos.x;
				if (position.x > obstacle->GetPosition().x)
				{
					//衝突SE鳴らす
					collisionSE->Play(false);
					
					//スクロールストップ
					playerSpeed = 0.0f;
					oldPlayerSpeed = 0.0f;
					
					//左倒れアニメーションセット
					playerObj->SetMotion(KNOCKLEFT, 0, 0.0f);
					
					//ゲームオーバーフラグを立てる
					gameOverFlg = true;
				}
			}
		}
		///////////////////////
		//RightSide　当たり判定
		///////////////////////
		//プレイヤーが障害物より右にいないなら障害物より左にいるので右壁当たり判定
		else if (position.x < obstacle->GetPosition().x - obstacle->GetScale().x &&
				 position.z > obstacle->GetPosition().z - obstacle->GetScale().z && 
				 position.y <= obstacle->GetPosition().y + obstacle->GetScale().y)
		{
			//右壁に当たってるかチェック
			if (Collision::SideRightWallVsPlayer(
				obstacle->ObstacleUpLeftTop,
				obstacle->ObstacleUpLeftBottom,
				obstacle->ObstacleDownLeftTop,
				obstacle->ObstacleDownLeftBottom,
				position)
				)
			{
				position.x = SavePos.x;
				if (position.x < obstacle->GetPosition().x)
				{
					//衝突SE鳴らす
					collisionSE->Play(false);
					
					//スクロールストップ
					playerSpeed = 0.0f;
					oldPlayerSpeed = 0.0f;
					
					//右倒れアニメーションセット
					playerObj->SetMotion(KNOCKRIGHT, 0, 0.0f);
					
					//ゲームオーバーフラグを立てる
					gameOverFlg = true;
				}
			}
		}
		///////////////////
		//front　当たり判定
		///////////////////
		//プレイヤーが障害物全面より前にいるかどうか
		else if (position.z < obstacle->GetPosition().z- obstacle->GetScale().z && 
				 position.y <= obstacle->GetPosition().y + obstacle->GetScale().y)
		{
			if (Collision::FrontWallVsPlayer(
				obstacle->ObstacleUpLeftBottom,
				obstacle->ObstacleUpRightBottom,
				obstacle->ObstacleDownLeftBottom,
				obstacle->ObstacleDownRightBottom,
				position)
				)
			{
				position.z = SavePos.z;
				position.x = SavePos.x;
				if (position.z < obstacle->GetPosition().z)
				{
					//衝突SE鳴らす
					collisionSE->Play(false);
					
					//スクロールストップ
					playerSpeed = 0.0f;
					oldPlayerSpeed = 0.0f;
					
					//後ろ倒れアニメーションセット
					playerObj->SetMotion(KNOCK, 0, 0.0f);
					
					//ゲームオーバーフラグを立てる
					gameOverFlg = true;
					
				}
			}
		}
		/////////////////////
		//ブロック上 当たり判定
		/////////////////////
		if (Collision::FloorVsPlayer(
			obstacle->ObstacleUpLeftTop,
			obstacle->ObstacleUpRightTop,
			obstacle->ObstacleUpLeftBottom,
			obstacle->ObstacleUpRightBottom,
			position)
			)
		{	
			//当たる直前に保持していた位置を代入	
			position = SavePos;
			SetVelocity({ 0,0,0 });
			if (position.y >= obstacle->GetPosition().y)
			{
				//衝突SE鳴らす
				collisionSE->Play(false);
				
				//スクロールストップ
				playerSpeed = 0.0f;
				oldPlayerSpeed = 0.0f;
				
				//前倒れアニメーションセット
				playerObj->SetMotion(KNOCKFRONT, 0, 0.0f);
				
				//ゲームオーバーフラグを立てる
				gameOverFlg = true;
				
			}
		}

	}
}

// ジャンプ入力処理
void Player::InputJump()
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	if (gamePad.GetButtonDown() & GamePad::BTN_A /*&& IsGround()*/ && position.y > -1.0f)
	{
		// ジャンプ回数制限
		if (jumpCount < jumpLimit)
		{
			//ジャンプのSE鳴らす
			jumpSE->Play(false);
			//ジャンプアニメーションセット
			playerObj->SetMotion(JUMP, 0, 0.03f);
			jumpCount++;
			Jump(jumpSpeed);
		}
	}
}

//着地している時にする処理
void Player::OnLanding()
{
	//走りアニメーションセット
	if (!gameOverFlg)playerObj->SetMotion(RUN, 0, 0.0f);
	jumpCount = 0;
	jumpEndSE->Play(false);
}

