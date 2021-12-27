#pragma once
#include <memory>
#include "StaticObject.h"

class Collision 
{
private:  
	static std::shared_ptr<StaticObject> terrain; 
public:  
	// 地形の登録と解除  
	static void RegisterTerrain( std::shared_ptr<StaticObject>& obj);  
	static void UnregisterTerrain();   

	// 登録されている地形にたいしてのレイピック 
	static int RayPick(   const DirectX::XMFLOAT3& startPosition,   
		                        const DirectX::XMFLOAT3& endPosition,   
								DirectX::XMFLOAT3* outPosition,   
								DirectX::XMFLOAT3* outNormal);   
	// 登録されている地形に対しての反射  
	static int Reflect(  const DirectX::XMFLOAT3& startPosition,   
							 const DirectX::XMFLOAT3& endPosition,   
		                     float rate,   
							 DirectX::XMFLOAT3* outPosition,  
							 DirectX::XMFLOAT3* outReflect); 

	//球同士の当たり判定
	static bool	HitSphere(	const DirectX::XMFLOAT3& p1, 
									float r1, 
									const DirectX::XMFLOAT3& p2, 
									float r2);


	static int MoveCheck(const DirectX::XMFLOAT3& startPosition,
							      const DirectX::XMFLOAT3& endPosition,
								  DirectX::XMFLOAT3* outPosition);
	//床当たり判定
	static bool FloorVsPlayer(
		const DirectX::XMFLOAT3& LeftTop,
		const DirectX::XMFLOAT3& RightTop,
		const DirectX::XMFLOAT3& LeftBottom,
		const DirectX::XMFLOAT3& RightBottom,
		const DirectX::XMFLOAT3& PlayerPos);
	//横の壁当たり判定
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

	// 球と球の交差判定
	static bool IntersectSphereVsSphere(
		const DirectX::XMFLOAT3& positionA,
		float radiusA,
		const DirectX::XMFLOAT3& positionB,
		float radiusB,
		DirectX::XMFLOAT3& outPositionB
	);





}; 