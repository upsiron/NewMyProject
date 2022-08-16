// TECK.01
#include "SkinnedObject.h"
#include "stage.h"
#include "player.h"

SkinnedObject::SkinnedObject(std::shared_ptr<SkinnedMesh>& mesh)
{
	this->mesh = mesh;
}

SkinnedObject::~SkinnedObject()
{
}

void SkinnedObject::Update(float elapsedTime)
{
	// ワールド行列作成
	DirectX::XMMATRIX s = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX r = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX t = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

	const XMFLOAT4X4 CoordinateSystemTransform = { -1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1 };
		DirectX::XMMATRIX C = XMLoadFloat4x4(&CoordinateSystemTransform);

	DirectX::XMStoreFloat4x4(&world, s * r * t);

	if (mesh->AnimationClips.size() > 0)Animate(elapsedTime);
}

void SkinnedObject::Animate(float elapsedTime)
{
	//アニメーション補間
	switch (StatusStep) 
	{
		case 0:
		{
			const float blend = Interval > 0 ? AnimationTick / Interval : AnimationTick;
			AnimationTick += elapsedTime;

			if (AnimationTick > Interval) {
				StatusStep = 1;
				NowMotion = NextMotion;
				NowFrame = TargetFrame;
				Animation& animation{ mesh->AnimationClips.at(NextMotion) };
				AnimationTick = TargetFrame / animation.SamplingRate;
			}

			const Animation::KeyFrame* keyframes[2]{
				&mesh->AnimationClips.at(NowMotion).Sequence.at(NowFrame),
				&mesh->AnimationClips.at(NextMotion).Sequence.at(TargetFrame)
			};
			mesh->BlendAnimation(keyframes, blend, keyframe);

			mesh->UpdateAnimation(keyframe);

			break;
		}
		case 1:
		{

			Animation& animation{ mesh->AnimationClips.at(NowMotion) };
			NowFrame = static_cast<int>(AnimationTick * animation.SamplingRate);
			if (NowFrame > animation.Sequence.size() - 1)
			{
				NowFrame = 0;
				AnimationTick = 0;
			}
			else
			{
				AnimationTick += elapsedTime * MotionSpeed;
			}
			keyframe = animation.Sequence.at(NowFrame);
			mesh->UpdateAnimation(keyframe);

			break;
		}
	}
}

void SkinnedObject::Render(
	ID3D11DeviceContext* immediateContext,
	const DirectX::XMFLOAT4X4& view,
	const DirectX::XMFLOAT4X4& projection,
	const DirectX::XMFLOAT4& light,
	const DirectX::XMFLOAT4& materialColor,
	const Animation::KeyFrame* keyframe,
	bool wireframe)
{
	// ワールドビュープロジェクション行列作成
	DirectX::XMMATRIX w = XMLoadFloat4x4(&world);
	DirectX::XMMATRIX v = XMLoadFloat4x4(&view);
	DirectX::XMMATRIX p = XMLoadFloat4x4(&projection);
	DirectX::XMMATRIX wvp = w * v * p;

	DirectX::XMFLOAT4X4 worldViewProjection;
	DirectX::XMStoreFloat4x4(&worldViewProjection, wvp);

	// 描画
	mesh->Render(immediateContext, worldViewProjection, world, light, materialColor, keyframe);
}


void SkinnedObject::Render(
	ID3D11DeviceContext* immediateContext,
	const DirectX::XMFLOAT4X4& view,
	const DirectX::XMFLOAT4X4& projection,
	const DirectX::XMFLOAT4& light,
	const DirectX::XMFLOAT4& materialColor,
	bool wireframe)
{
	// ワールドビュープロジェクション行列作成
	DirectX::XMMATRIX w = XMLoadFloat4x4(&world);
	DirectX::XMMATRIX v = XMLoadFloat4x4(&view);
	DirectX::XMMATRIX p = XMLoadFloat4x4(&projection);
	DirectX::XMMATRIX wvp = w * v * p;

	DirectX::XMFLOAT4X4 worldViewProjection;
	DirectX::XMStoreFloat4x4(&worldViewProjection, wvp);

	if (mesh->AnimationClips.size() > 0)
	{
		mesh->Render(immediateContext, worldViewProjection, world, light, materialColor, &keyframe);
	}
	else
	{
		mesh->Render(immediateContext, worldViewProjection, world, light, materialColor, nullptr);
	}
}

void SkinnedObject::SetMotion(int NextMotion, int TargetFrame, float Interval)
{
	if (this->NowMotion == NextMotion) return;

	this->NextMotion = NextMotion;
	this->TargetFrame = TargetFrame;
	this->Interval = Interval;
	Animation::KeyFrame keyframe = mesh->AnimationClips.at(NowMotion).Sequence.at(NowFrame);
	AnimationTick = 0;
	StatusStep = 0;
}




