#pragma once
#include <memory>
#include "StaticObject.h"

class Collision 
{
private:  
	static std::shared_ptr<StaticObject> terrain; 
public:  
	//ãÖìØémÇÃìñÇΩÇËîªíË
	static bool	HitSphere(	const DirectX::XMFLOAT3& p1, 
									float r1, 
									const DirectX::XMFLOAT3& p2, 
									float r2);

	//è∞ìñÇΩÇËîªíË
	static bool FloorVsPlayer(
		const DirectX::XMFLOAT3& LeftTop,
		const DirectX::XMFLOAT3& RightTop,
		const DirectX::XMFLOAT3& LeftBottom,
		const DirectX::XMFLOAT3& RightBottom,
		const DirectX::XMFLOAT3& PlayerPos);
	//â°ÇÃï«ìñÇΩÇËîªíË
   static bool SideLeftWallVsPlayer(
	   const DirectX::XMFLOAT3& LeftTop,
	   const DirectX::XMFLOAT3& RightTop,
	   const DirectX::XMFLOAT3& LeftBottom,
	   const DirectX::XMFLOAT3& RightBottom,
	   const DirectX::XMFLOAT3& PlayerPos);
   static bool SideRightWallVsPlayer(
	   const DirectX::XMFLOAT3& LeftTop,
	   const DirectX::XMFLOAT3& RightTop,
	   const DirectX::XMFLOAT3& LeftBottom,
	   const DirectX::XMFLOAT3& RightBottom,
	   const DirectX::XMFLOAT3& PlayerPos);

   static bool FrontWallVsPlayer(
	   const DirectX::XMFLOAT3& LeftTop,
	   const DirectX::XMFLOAT3& RightTop,
	   const DirectX::XMFLOAT3& LeftBottom,
	   const DirectX::XMFLOAT3& RightBottom,
	   const DirectX::XMFLOAT3& PlayerPos);

	static bool AllWallVsPlayer(
		const DirectX::XMFLOAT3& UpLeftTop,
		const DirectX::XMFLOAT3& UpRightTop,
		const DirectX::XMFLOAT3& UpLeftBottom,
		const DirectX::XMFLOAT3& UpRightBottom,
		const DirectX::XMFLOAT3& DownLeftTop,
		const DirectX::XMFLOAT3& DownRightTop,
		const DirectX::XMFLOAT3& DownLeftBottom,
		const DirectX::XMFLOAT3& DownRightBottom,
		const DirectX::XMFLOAT3& PlayerPos);

	// ãÖÇ∆ãÖÇÃåç∑îªíË
	static bool IntersectSphereVsSphere(
		const DirectX::XMFLOAT3& positionA,
		float radiusA,
		const DirectX::XMFLOAT3& positionB,
		float radiusB,
		DirectX::XMFLOAT3& outPositionB
	);
}; 