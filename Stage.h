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
	std::shared_ptr<SkinnedMesh> stageBaseMesh;
	std::shared_ptr<SkinnedMesh> stageBaseMesh2;
	std::unique_ptr<SkinnedObject> stageBaseObj;

	std::unique_ptr<SkinnedObject> stageTileObj;

public:
	Stage();
	Stage(std::shared_ptr<SkinnedMesh>& mesh) :
		SkinnedObject(mesh) {};
	~Stage();

	// インスタンス取得
	static Stage& Instance();

	// 更新処理
	/*void Update(float elapsedTime);*/

	// 描画処理
	/*void Render(ID3D11DeviceContext* immediateContext,
		const DirectX::XMFLOAT4X4& view,
		const DirectX::XMFLOAT4X4& projection,
		const DirectX::XMFLOAT4& light,
		const DirectX::XMFLOAT4& materialColor,
		bool wireframe);*/

	// デバッグ用GUI表示
	void DrawDebugGUI();

	//タイルの4点を取る変数
	DirectX::XMFLOAT3 tileLeftTop;
	DirectX::XMFLOAT3 tileRightTop;
	DirectX::XMFLOAT3 tileLeftBottom;
	DirectX::XMFLOAT3 tileRightBottom;

private:
	//Model* model = nullptr;
};