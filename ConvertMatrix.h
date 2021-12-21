#pragma once
#include <DirectXMath.h>
#include <fbxsdk.h>

using namespace DirectX;

inline XMFLOAT4X4 FBXMatToDirMat4x4(const FbxAMatrix FBXMatrix)
{
	/*FbxAMatrix��double�^��xmfloat4x4��float�^�Ȃ̂�
	���̂܂ܓ���悤�Ƃ���ƌ^���Ⴄ���Č����ĎE�����
	�Ȃ̂ŁA�ϊ�����֐�������*/

	XMFLOAT4X4 DirMAt4x4;
	for (int Row = 0; Row < 4; Row++)
	{
		//����...�s
		for (int Column = 0; Column < 4; Column++)
		{
			//�c��...��
			//SQL�Ƃ��Ō���z�����킩��͂�
			{
				/*���ۂɑ������Ƃ���*/
				DirMAt4x4.m[Row][Column] =
					static_cast<float>(FBXMatrix[Row][Column]);
			}
		}
	}
	return DirMAt4x4;
};

//////////////////////////////////////////////////////////
/*FBX���ł�double�ō���Ă邩�瓯���悤�ɕϊ��̊֐������*/
//////////////////////////////////////////////////////////
inline XMFLOAT3 FBXDobleToXMFLOAT3(const FbxDouble3 FBXDouble3)
{
	XMFLOAT3 xmfloat3;
	xmfloat3 = {
	static_cast<float>(FBXDouble3[0]),
	static_cast<float>(FBXDouble3[1]),
	static_cast<float>(FBXDouble3[2])
	};
	return xmfloat3;
}

inline XMFLOAT4 FBXDobleToXMFLOAT4(const FbxDouble4 FBXDouble4)
{
	XMFLOAT4 xmfloat4;
	xmfloat4 = {
	static_cast<float>(FBXDouble4[0]),
	static_cast<float>(FBXDouble4[1]),
	static_cast<float>(FBXDouble4[2]),
	static_cast<float>(FBXDouble4[3])
	};
	return xmfloat4;
}