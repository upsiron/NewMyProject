#pragma once

#include <memory>
#include <vector>
#include <directxmath.h>
#include "SkinnedMesh.h"

class SkinnedObject
{
protected:
	DirectX::XMFLOAT3 position = { 0, 0, 0 };
	DirectX::XMFLOAT3 scale = { 1, 1, 1 };
	DirectX::XMFLOAT3 angle = { 0, 0, 0 };
	DirectX::XMFLOAT4X4 world = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	std::shared_ptr<SkinnedMesh>	mesh;
	Animation::KeyFrame keyframe;
	
	//ギミック用フラグ
	bool RedGimmickFlg = false;
	bool GreenGimmickFlg = false;
	bool BlueGimmickFlg = false;

	//アニメーション関連の変数
	int NextMotion{ 4 };
	float MotionSpeed{ 1.0f };
	int TargetFrame{ 0 };
	int StatusStep{ 1 };
	int NowMotion{ 0 };
	int NowFrame{ 0 };
	int FrameIndex{ 0 };
	float Interval{ 1.0f };
	float AnimationTick{ 0 };
	int AnimationState = 0;
	int State{ 1 };

public:
	SkinnedObject() {};
	SkinnedObject(std::shared_ptr<SkinnedMesh>& mesh);
	~SkinnedObject();

public:
	// setter
	void SetPosition(const DirectX::XMFLOAT3& p) { position = p; }
	void SetScale(const DirectX::XMFLOAT3& s) { scale = s; }
	void SetAngle(const DirectX::XMFLOAT3& a) { angle = a; }
	void SetWorld(const DirectX::XMFLOAT4X4& m) { world = m; }
	void SetAnime(const int animationState) { AnimationState = animationState; }
	// getter
	const DirectX::XMFLOAT3& GetPosition() const { return position; }
	const DirectX::XMFLOAT3& GetScale() const { return scale; }
	const DirectX::XMFLOAT3& GetAngle() const { return angle; }
	const DirectX::XMFLOAT4X4& GetWorld() const { return world; }
	const bool& GetRedFlg() const { return RedGimmickFlg; }
	const bool& GetBlueFlg() const { return BlueGimmickFlg; }

	void Update(float elapsedTime);

	void Animate(float elapsedTime);

	void Render(
		ID3D11DeviceContext* immediateContext,
		const DirectX::XMFLOAT4X4& view,
		const DirectX::XMFLOAT4X4& projection,
		const DirectX::XMFLOAT4& light,
		const DirectX::XMFLOAT4& materialColor,
		const Animation::KeyFrame*keyframe,
		bool wireframe = false);


	void Render(
		ID3D11DeviceContext* immediateContext,
		const DirectX::XMFLOAT4X4& view,
		const DirectX::XMFLOAT4X4& projection,
		const DirectX::XMFLOAT4& light,
		const DirectX::XMFLOAT4& materialColor,
		bool wireframe = false
		);

	void SetMotion(int NextMotion, int TargetFrame, float Interval = 1.0f);


	//ステージベース分割用構造体
	struct Squea
	{
		DirectX::XMFLOAT3 spritPosition[9] = { { 0.0f, 0.0f, 0.0f } };
	};
	Squea squea;
};
