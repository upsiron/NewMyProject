#include <imgui.h>
#include "Player.h"
#include "Input/Input.h"
#include "Camera.h"
#include "ObstacleBlockManager.h"
#include "collision.h"
#include "KeyInput.h"
#include "Scene.h"


static Player* instance = nullptr;
static DirectX::XMFLOAT3 savePos;

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
		"Data/Banana/runAnime.fbx",
		"Data/Banana/RunningForwardFlip.fbx",
		"Data/Banana/KnockedOut.fbx",
		"Data/Banana/KnockOutRight.fbx",
		"Data/Banana/KnockOutLeft.fbx",
	}; 
	//初期化
	playerMesh = std::make_shared<SkinnedMesh>(device, "Data/Banana/banana.fbx", Animationfilenames, true);

	playerObj = std::make_unique<SkinnedObject>(playerMesh);
	SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	SetScale(DirectX::XMFLOAT3(0.013f, 0.013f, 0.013f));
	SetAngle(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));

	//スクロールスピード初期化
	scrollSpeed = 0.2f;

	playerObj->SetMotion(RUN, 0, 0.08f);

	// モデルが大きいのでスケーリング
	//scale.x = scale.y = scale.z = 0.01f;
}

// デストラクタ
Player::~Player()
{
	//delete model;
}

// プレイヤー更新処理
void Player::Update(float elapsedTime)
{
	//現在のelapsedTime保持
	SaveElapsedTime = elapsedTime;

	//位置をセット
	playerObj->SetPosition(position);
	playerObj->SetAngle(angle);
	playerObj->SetScale(scale);

	//自動スクロール
	if(debugflg)position.z += scrollSpeed;

	//スクロール加速処理
	if (RedGimmickFlg)scrollSpeed = oldScrollSpeed + 0.05f;
	else oldScrollSpeed = scrollSpeed;

	if (GreenGimmickFlg)
	{
		if (jumpCount < jumpLimit)
		{
			//ジャンプアニメーションセット
			playerObj->SetMotion(JUMP, 0, 0.08f);
			jumpCount++;
			Jump(30.0f);
		}
	}
	
	// 速力更新処理
	UpdateVelocity(elapsedTime);
	
	// 移動入力
	InputMove(elapsedTime);

	// ジャンプ入力
	InputJump();

	// プレイヤーと敵との衝突判定
	CollisionPlayerVsEnemis();

	//プレイヤーの更新
	playerObj->Update();

	//落下ゲームオーバー
	if (position.y < -1.5f)
	{
		scrollSpeed = 0.0f;
		GameOverFlg = true;
	}

	//現在のプレイヤーのポジションを保持
	savePos = position;
}

void Player::Render(ID3D11DeviceContext* immediateContext,
	const DirectX::XMFLOAT4X4& view,
	const DirectX::XMFLOAT4X4& projection,
	const DirectX::XMFLOAT4& light,
	const DirectX::XMFLOAT4& materialColor,
	bool wireframe)
{
	//プレイヤーの描画
	//playerObj->Render(immediateContext, view, projection, light, materialColor, &keyframe, wireframe);
	playerObj->Render(immediateContext, view, projection, light, materialColor, SaveElapsedTime, wireframe);
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
			if (gamePad.GetButtonDown() & GamePad::BTN_Y)debugflg = !debugflg;
			if (ImGui::Button("Debug"))debugflg = true;
			if (ImGui::Button("NoDebug"))debugflg = false;
			ImGui::SliderFloat("scroll", &scrollSpeed, 0.0f, 1.0f);
			ImGui::SliderFloat("jump", &jumpSpeed, 0.0f, 100.0f);
		}
	}
	ImGui::End();
}

// スティック入力値から移動ベクトルを取得
DirectX::XMFLOAT3 Player::GetMoveVec()const
{
	// 入力情報を取得
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ax = gamePad.GetAxisLX();
	float ay = gamePad.GetAxisLY();

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
	//vec.x = (cameraRightX * ax) + (cameraFrontX * ay);
	vec.x = ax;
	//vec.z = (cameraRightZ * ax) + (cameraFrontZ * ay);
	if (debugflg)
	{
		vec.z = 0.0f;
	}
	else
	{
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
	if (position.x > StageSideEndPos)position.x = savePos.x;
	if (position.x < -1.0f * StageSideEndPos)position.x = savePos.x;

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

// プレイヤーと敵の衝突判定
void Player::CollisionPlayerVsEnemis()
{
	ObstacleBlockManager& obstacleBlockManager = ObstacleBlockManager::Instance();

	// 全ての障害物を見て衝突しているかチェック
	int obstacleCount = obstacleBlockManager.GetObstacleCount();
	for (int i = 0; i < obstacleCount; ++i)
	{
		Obstacle* obstacle = obstacleBlockManager.GetObstacle(i);

		//描画されてないなら衝突処理を飛ばす
		if (!obstacle->GetExistFlg())continue;

		// 衝突処理
		//side　当たり判定
		//プレイヤーが障害物より右にいるなら左壁の当たり判定処理
		if (position.x > obstacle->GetPosition().x && position.z > obstacle->GetPosition().z - obstacle->GetScale().z)
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
				position.x = savePos.x;
				if (position.x > obstacle->GetPosition().x)
				{
					//スクロールストップ
					scrollSpeed = 0.0f;
					//左倒れアニメーションセット
					playerObj->SetMotion(KNOCKLEFT, 0, 0.0f);
					//playerObj->SetAnime(KNOCKLEFT);
				}
			}
		}
		//プレイヤーが障害物より右にいないなら障害物より左にいるので右壁当たり判定
		else if (position.x < obstacle->GetPosition().x && position.z > obstacle->GetPosition().z - obstacle->GetScale().z)
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
				position.x = savePos.x;
				if (position.x < obstacle->GetPosition().x)
				{
					//スクロールストップ
					scrollSpeed = 0.0f;
					//右倒れアニメーションセット
					playerObj->SetMotion(KNOCKRIGHT, 0, 0.0f);
					//playerObj->SetAnime(KNOCKRIGHT);
				}
			}
		}
		//front　当たり判定
		if (position.z < obstacle->GetPosition().z- obstacle->GetScale().z)
		{
			if (Collision::VerticalFrontWallVsPlayer(
				obstacle->ObstacleUpLeftBottom,
				obstacle->ObstacleUpRightBottom,
				obstacle->ObstacleDownLeftBottom,
				obstacle->ObstacleDownRightBottom,
				position)
				)
			{
				position.z = savePos.z;
				if (position.z < obstacle->GetPosition().z)
				{
					//後ろ倒れアニメーションセット
					playerObj->SetMotion(KNOCK, 0, 0.0f);
					GameOverFlg = true;
					//playerObj->SetAnime(KNOCK);
				}
			}
		}
	}
}

void Player::InputShot()
{
	if (KeyInput::KeyTrigger() & KEY_C)
	{
		shot_timer = 0;
		//通常弾
		for (auto& s : shotObj)
		{
			if (s->Exist)continue;	//存在していれば別を検索
			s->Set(DirectX::XMFLOAT3(position.x, position.y, position.z), angle, SHOT::LOOP);
			break;
		}
	}
	//ショットタイマーで射程
	if (shot_timer > 20)
	{
		for (auto& s : shotObj) {
			s->Exist = false;
		}
	}

	for (auto& s : shotObj) {
		if (!s->Exist)continue;		//存在しなければ別を検索
		switch (s->ShotNum)
		{
		case SHOT::NONE:
			break;
		case SHOT::LOOP:
			s->Loop_Move();
			break;
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
			//scrollSpeed = scrollSpeed - (saveScrollSpeed - 0.15f);
			//ジャンプアニメーションセット
			playerObj->SetMotion(JUMP, 0, 0.08f);
			//playerObj->SetAnime(JUMP);
			jumpCount++;
			Jump(jumpSpeed);
		}
	}
}

// デバッグプリミティブ描画
//void Player::DrawDebugPrimitive()
//{
//	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();
//
//	// 衝突判定用デバッグ球を描画
//	debugRenderer->DrawSphere(position, radius, DirectX::XMFLOAT4(0, 0, 0, 1));
//
//	// 弾丸デバッグプリミティブ描画処理
//	//projectileManager.DrawDebugPrimitive();
//}

//着地している時にする処理
void Player::OnLanding()
{
	//scrollSpeed = saveScrollSpeed;
	//走りアニメーションセット
	playerObj->SetMotion(RUN, 0, 0.0f);
	//playerObj->SetAnime(RUN);
	jumpCount = 0;
}

