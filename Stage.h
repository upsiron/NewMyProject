#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "Character.h"


#define mapWidth 3
#define mapHight 3

// �X�e�[�W
class Stage : public SkinnedObject
{
public:
	std::shared_ptr<SkinnedMesh> stageTileMesh;
	std::shared_ptr<SkinnedMesh> stageBaseMesh;
	std::unique_ptr<SkinnedObject> stageBaseObj;
	std::unique_ptr<SkinnedObject> stageTileObj;

public:
	Stage();
	Stage(std::shared_ptr<SkinnedMesh>& mesh) :
		SkinnedObject(mesh) {};
	~Stage();

	// �C���X�^���X�擾
	static Stage& Instance();

	// �f�o�b�O�pGUI�\��
	void DrawDebugGUI();

	//�^�C����4�_�����ϐ�
	DirectX::XMFLOAT3 tileLeftTop;
	DirectX::XMFLOAT3 tileRightTop;
	DirectX::XMFLOAT3 tileLeftBottom;
	DirectX::XMFLOAT3 tileRightBottom;
};