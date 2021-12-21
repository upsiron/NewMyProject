#pragma once
#include "Character.h"
//#include "Telegram.h"
//#include "DebugRenderer.h"


// エネミー
class Enemy : public Character
{
public:
	Enemy() {}
	Enemy(std::shared_ptr<SkinnedMesh>& mesh) :
		Character(mesh) {};
	~Enemy() override {}

	// 更新処理
	virtual void Update(float elapsedTime) = 0;

	// 描画処理
	virtual void Render(ID3D11DeviceContext* immediateContext,
		const DirectX::XMFLOAT4X4& view,
		const DirectX::XMFLOAT4X4& projection,
		const DirectX::XMFLOAT4& light,
		const DirectX::XMFLOAT4& materialColor,
		bool wireframe) = 0;

	// 破棄
	void Destroy();

	// デバッグプリミティブ描画
	virtual void DrawDebugPrimitive();

	// デバッグエネミー情報表示
	virtual void DrawDebugGUI();

	// 攻撃フラグ取得
	bool	GetAttackFlg() { return attackFlg; }
	// 攻撃フラグセット
	void	SetAttackFlg(bool flg) { attackFlg = flg; };
	void	SetId(int id) { this->id = id; }
	int		GetId() { return id; }

	// メッセージ受信したときの処理
	/*virtual bool OnMessage(const Telegram& msg);

	virtual bool HandleMessage(const Telegram& msg);*/

	// 攻撃範囲取得
	float GetAttackRange() { return attackRange; }

	// デバッグレンダラ取得
	//DebugRenderer* GetDebugRenderer() const { return debugRenderer.get(); }
	
protected:
	int	state = 0;
	bool attackFlg = false;
	int id = 0;
	float searchRange = 0.0f;
	float attackRange = 0.0f;

private:
	//std::unique_ptr<DebugRenderer>					debugRenderer;
};