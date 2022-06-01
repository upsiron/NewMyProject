#include "collision.h" 

bool Collision::HitSphere(const DirectX::XMFLOAT3& p1, float r1, const DirectX::XMFLOAT3& p2, float r2)
{
	//	���a�̍��Z��2��
	const float R2 = (r1 + r2) * (r1 + r2);

	//	���S���m�̋�����2��
	DirectX::XMFLOAT3 vec;
	vec.x = p1.x - p2.x;
	vec.y = p1.y - p2.y;
	vec.z = p1.z - p2.z;
	float L2 = vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;

	//	�Փ˔���
	if (L2 < R2) return	true;

	return false;
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
	//��
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
	//�E
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
		PlayerPos.y + 1.0f < LeftTop.y+1.0f &&
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

	//��
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

	//��
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

	//��
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

	//�E
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

//// ���Ƌ��̌�������
//bool Collision::IntersectSphereVsSphere(
//	const DirectX::XMFLOAT3& positionA,
//	float radiusA,
//	const DirectX::XMFLOAT3& positionB,
//	float radiusB,
//	DirectX::XMFLOAT3& outPositionB)
//{
//	// B��A�̒P�ʃx�N�g�����Z�o
//	DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&positionA);
//	DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&positionB);
//	DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(PositionB, PositionA);
//	DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);
//	float lengthSq;
//	DirectX::XMStoreFloat(&lengthSq, LengthSq);
//
//	// ��������
//	float range = radiusA + radiusB;
//	if (lengthSq > range)
//	{
//		return false;
//	}
//
//	// A��B�������o��
//	Vec = DirectX::XMVector3Normalize(Vec);
//	Vec = DirectX::XMVectorScale(Vec, range);
//	PositionB = DirectX::XMVectorAdd(PositionA, Vec);
//	DirectX::XMStoreFloat3(&outPositionB, PositionB);
//
//	return true;
//}


// ���Ƌ��̌�������iA�������o���j
bool Collision::IntersectSphereVsSphere(
	const DirectX::XMFLOAT3& positionA,
	float radiusA,
	const DirectX::XMFLOAT3& positionB,
	float radiusB,
	DirectX::XMFLOAT3& outPositionA)
{
	// A��B�̒P�ʃx�N�g�����Z�o
	DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&positionA);
	DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&positionB);
	DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(PositionA, PositionB);
	DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);
	float lengthSq;
	DirectX::XMStoreFloat(&lengthSq, LengthSq);

	// ��������
	float range = radiusA + radiusB;
	if (lengthSq > range)
	{
		return false;
	}

	// A��B�������o��
	Vec = DirectX::XMVector3Normalize(Vec);
	Vec = DirectX::XMVectorScale(Vec, range);
	PositionA = DirectX::XMVectorAdd(PositionB, Vec);
	DirectX::XMStoreFloat3(&outPositionA, PositionA);

	return true;
}