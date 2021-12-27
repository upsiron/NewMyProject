#include "collision.h" 

std::shared_ptr<StaticObject> Collision::terrain;

void Collision::RegisterTerrain(std::shared_ptr<StaticObject>& obj) { terrain = obj; }

void Collision::UnregisterTerrain() { terrain.reset(); }

//今使えない
int Collision::RayPick(const DirectX::XMFLOAT3& startPosition, 
							  const DirectX::XMFLOAT3& endPosition, 
						      DirectX::XMFLOAT3* outPosition, 
							  DirectX::XMFLOAT3* outNormal) 
{ 
	return terrain->RayPick(startPosition, endPosition, outPosition, outNormal); 
}

int Collision::Reflect(const DirectX::XMFLOAT3& startPosition, const DirectX::XMFLOAT3& endPosition, float rate, DirectX::XMFLOAT3* outPosition, DirectX::XMFLOAT3* outReflect)
{
	// レイピック  
	DirectX::XMFLOAT3 hitPosition, hitNormal;  
	int materialIndex = RayPick(startPosition, endPosition, &hitPosition, &hitNormal);  
	if (materialIndex == -1)  {   return materialIndex;  } 

     // 壁までのベクトル  
	DirectX::XMVECTOR hit = DirectX::XMLoadFloat3(&hitPosition);  
	DirectX::XMVECTOR start = DirectX::XMLoadFloat3(&startPosition);  
	DirectX::XMVECTOR end = DirectX::XMLoadFloat3(&endPosition);  
	DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(end, start); 

	// 壁の法線  
	DirectX::XMVECTOR normal = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&hitNormal));

	// 入射ベクトルを法線に射影  
	DirectX::XMVECTOR Vec_Inversion = DirectX::XMVectorNegate(vec);
	DirectX::XMVECTOR dot = DirectX::XMVector3Dot(Vec_Inversion, normal);

	// 反射位置の計算  
	// out = vec + normal * (dot * 2);  
	DirectX::XMVECTOR reflect = DirectX::XMVectorAdd(vec, DirectX::XMVectorMultiply(normal , DirectX::XMVectorScale(dot, 2.0f))) ;

	// 反射率の補正   
	reflect = DirectX::XMVectorScale(reflect, rate);

	// 反射位置(hit予定位置)  
    DirectX::XMVECTOR position = hit;  
	DirectX::XMStoreFloat3(outPosition, position); 
	DirectX::XMStoreFloat3(outReflect, reflect); 

	return materialIndex;
}

bool Collision::HitSphere(const DirectX::XMFLOAT3& p1, float r1, const DirectX::XMFLOAT3& p2, float r2)
{
	//	半径の合算の2乗
	const float R2 = (r1 + r2) * (r1 + r2);

	//	中心同士の距離の2乗
	DirectX::XMFLOAT3 vec;
	vec.x = p1.x - p2.x;
	vec.y = p1.y - p2.y;
	vec.z = p1.z - p2.z;
	float L2 = vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;

	//	衝突判定
	if (L2 < R2)	return	true;

	return false;
}

int Collision::MoveCheck(const DirectX::XMFLOAT3& startPosition, const DirectX::XMFLOAT3& endPosition, DirectX::XMFLOAT3* outPosition)
{
	DirectX::XMFLOAT3 hitPosition, hitNormal;
	int materialIndex = RayPick(startPosition, endPosition, &hitPosition, &hitNormal);
	if (materialIndex == -1)
	{ // ヒットしなかったら移動後の位置は終点   
		*outPosition = endPosition;
		return materialIndex;
	}

	// 壁をつきぬけたベクトル  
	DirectX::XMVECTOR start = DirectX::XMLoadFloat3(&hitPosition);
	DirectX::XMVECTOR end = DirectX::XMLoadFloat3(&endPosition);
	DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(end, start);

	// 壁の法線ベクトルを単位化  
	DirectX::XMVECTOR normal = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&hitNormal));

	// 入射ベクトルを法線に射影  
	DirectX::XMVECTOR Vec_Inversion = DirectX::XMVectorNegate(vec);
	DirectX::XMVECTOR dot = DirectX::XMVector3Dot(Vec_Inversion, normal);

	// 補正位置の計算  
	DirectX::XMVECTOR position = DirectX::XMVectorAdd(DirectX::XMVectorMultiply(normal, dot), end);
	DirectX::XMStoreFloat3(outPosition, position);

	// 補正後の位置が壁にめり込んでいた場合は移動しないようにする 
	if (-1 != RayPick(hitPosition, *outPosition, &hitPosition, &hitNormal))
	{
		*outPosition = startPosition;
	}



	return materialIndex;
}

bool Collision::FloorVsPlayer(
	const DirectX::XMFLOAT3& LeftTop,
	const DirectX::XMFLOAT3& RightTop,
	const DirectX::XMFLOAT3& LeftBottom,
	const DirectX::XMFLOAT3& RightBottom,
	const DirectX::XMFLOAT3& PlayerPos)
{
	if (PlayerPos.x > LeftTop.x &&
		PlayerPos.x < RightTop.x &&
		PlayerPos.z < LeftTop.z &&
		PlayerPos.z > LeftBottom.z)
	{
		if (PlayerPos.y <= LeftBottom.y)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}


bool Collision::SideLeftWallVsPlayer(
	const DirectX::XMFLOAT3& LeftTop,
	const DirectX::XMFLOAT3& RightTop,
	const DirectX::XMFLOAT3& LeftBottom,
	const DirectX::XMFLOAT3& RightBottom,
	const DirectX::XMFLOAT3& PlayerPos)
{
	//左
	if (PlayerPos.z < LeftTop.z &&
		PlayerPos.z > RightTop.z &&
		PlayerPos.y + 1.0f < RightTop.y &&
		PlayerPos.y + 1.0f > RightBottom.y)
	{
		if (PlayerPos.x - 0.3f < RightBottom.x)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	return false;
}

bool Collision::SideRightWallVsPlayer(
	const DirectX::XMFLOAT3& LeftTop,
	const DirectX::XMFLOAT3& RightTop,
	const DirectX::XMFLOAT3& LeftBottom,
	const DirectX::XMFLOAT3& RightBottom,
	const DirectX::XMFLOAT3& PlayerPos)
{
	//右
	if (
		PlayerPos.z < LeftTop.z &&
		PlayerPos.z > RightTop.z &&
		PlayerPos.y + 1.0f < RightTop.y &&
		PlayerPos.y + 1.0f > RightBottom.y)
	{
	if (PlayerPos.x + 0.3f > RightBottom.x)
	{
		return true;
	}
	else
	{
		return false;
	}
	}
	return false;
}

bool Collision::FrontWallVsPlayer(
	const DirectX::XMFLOAT3& LeftTop,
	const DirectX::XMFLOAT3& RightTop,
	const DirectX::XMFLOAT3& LeftBottom,
	const DirectX::XMFLOAT3& RightBottom,
	const DirectX::XMFLOAT3& PlayerPos)
{
	if (
		PlayerPos.x > LeftTop.x &&
		PlayerPos.x < RightTop.x &&
		PlayerPos.y + 1.0f < LeftTop.y &&
		PlayerPos.y + 1.0f > RightBottom.y)
	{
		if (PlayerPos.z > RightBottom.z)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}

bool Collision::AllWallVsPlayer(
	const DirectX::XMFLOAT3& UpLeftTop,
	const DirectX::XMFLOAT3& UpRightTop,
	const DirectX::XMFLOAT3& UpLeftBottom,
	const DirectX::XMFLOAT3& UpRightBottom,
	const DirectX::XMFLOAT3& DownLeftTop,
	const DirectX::XMFLOAT3& DownRightTop,
	const DirectX::XMFLOAT3& DownLeftBottom,
	const DirectX::XMFLOAT3& DownRightBottom,
	const DirectX::XMFLOAT3& PlayerPos)
{

	//下
	if (
		PlayerPos.x > UpLeftBottom.x &&
		PlayerPos.x < UpRightBottom.x &&
		PlayerPos.y + 1.0f < UpLeftBottom.y &&
		PlayerPos.y + 1.0f > DownLeftBottom.y &&
		PlayerPos.z < DownLeftTop.z)
	{
		if (PlayerPos.z > DownLeftBottom.z)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	//上
	else if (
		PlayerPos.x > UpLeftTop.x &&
		PlayerPos.x < UpRightTop.x && 
		PlayerPos.y + 1.0f < UpLeftTop.y 
		&& PlayerPos.y + 1.0f > DownLeftTop.y)
	{
		if (PlayerPos.z < DownLeftTop.z)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	//左
	else if (PlayerPos.x > UpRightBottom.x && PlayerPos.x < UpRightTop.x && PlayerPos.y + 1.0f < UpRightBottom.y && PlayerPos.y + 1.0f > DownRightBottom.y)
	{
		if (PlayerPos.x < DownRightBottom.x)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	//右
	else if (PlayerPos.x > UpLeftBottom.x && PlayerPos.x < UpLeftTop.x && PlayerPos.y + 1.0f < UpLeftBottom.y && PlayerPos.y + 1.0f > DownLeftBottom.y)
	{
		if (PlayerPos.x < DownRightBottom.x)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	return false;
}

//// 球と球の交差判定
//bool Collision::IntersectSphereVsSphere(
//	const DirectX::XMFLOAT3& positionA,
//	float radiusA,
//	const DirectX::XMFLOAT3& positionB,
//	float radiusB,
//	DirectX::XMFLOAT3& outPositionB)
//{
//	// B⇒Aの単位ベクトルを算出
//	DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&positionA);
//	DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&positionB);
//	DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(PositionB, PositionA);
//	DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);
//	float lengthSq;
//	DirectX::XMStoreFloat(&lengthSq, LengthSq);
//
//	// 距離判定
//	float range = radiusA + radiusB;
//	if (lengthSq > range)
//	{
//		return false;
//	}
//
//	// AがBを押し出す
//	Vec = DirectX::XMVector3Normalize(Vec);
//	Vec = DirectX::XMVectorScale(Vec, range);
//	PositionB = DirectX::XMVectorAdd(PositionA, Vec);
//	DirectX::XMStoreFloat3(&outPositionB, PositionB);
//
//	return true;
//}


// 球と球の交差判定（Aを押し出す）
bool Collision::IntersectSphereVsSphere(
	const DirectX::XMFLOAT3& positionA,
	float radiusA,
	const DirectX::XMFLOAT3& positionB,
	float radiusB,
	DirectX::XMFLOAT3& outPositionA)
{
	// A⇒Bの単位ベクトルを算出
	DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&positionA);
	DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&positionB);
	DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(PositionA, PositionB);
	DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);
	float lengthSq;
	DirectX::XMStoreFloat(&lengthSq, LengthSq);

	// 距離判定
	float range = radiusA + radiusB;
	if (lengthSq > range)
	{
		return false;
	}

	// AがBを押し出す
	Vec = DirectX::XMVector3Normalize(Vec);
	Vec = DirectX::XMVectorScale(Vec, range);
	PositionA = DirectX::XMVectorAdd(PositionB, Vec);
	DirectX::XMStoreFloat3(&outPositionA, PositionA);

	return true;
}