#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "Character.h"


#define mapWidth 3
#define mapHight 3

// ステージ
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

	// インスタンス取得
	static Stage& Instance();

	// デバッグ用GUI表示
	void DrawDebugGUI();

	//タイルの4点を取る変数
	DirectX::XMFLOAT3 tileLeftTop;
	DirectX::XMFLOAT3 tileRightTop;
	DirectX::XMFLOAT3 tileLeftBottom;
	DirectX::XMFLOAT3 tileRightBottom;
};