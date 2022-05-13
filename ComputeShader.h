#pragma once
#include <d3d11.h>
#include <wrl.h>
#include "Framework.h"
#include "ParticleSystem.h"
//CComPtr関連
#include <atlbase.h>
#include <atlcom.h>
#include <atlcoll.h>
#include <atlstr.h>

class ParticleSystem;

class ComputeShader
{
private:
	Framework& framework = Framework::Instance();
	ID3D11Device* device = framework.GetDevice();
	ID3D11DeviceContext* context = framework.GetImmediateContext();
public:
	ParticleSystem* particleSystem;
	void CreateShader(void);									    //シェーダの作成(シェーダー作成)
	void CreateResource(void);									    //リソースの作成(バッファ確保)
	void CreateSRV(void);										    //シェーダ リソース ビューの作成（入力ビュー）
	void CreateUAV(void);										    //アンオーダード・アクセス・ビューの作成（出力ビュー）
	void RunComputeShader(void);								    //コンピュート シェーダを使った演算


	CComPtr <ID3D11ComputeShader> pComputeShader = NULL;			// コンピュート・シェーダ
	CComPtr <ID3D11UnorderedAccessView> pUAV = NULL;
	CComPtr <ID3D11UnorderedAccessView> pCpuUAV = NULL;
	CComPtr <ID3D11ShaderResourceView>  pSRV = NULL;
	CComPtr <ID3D11Buffer> pToCpuReadBackBuffer = NULL;
	CComPtr <ID3D11Buffer> pToCpuWriteBuffer = NULL;
	CComPtr <ID3D11Buffer> pBufferInput = NULL;
	CComPtr <ID3D11Buffer> pBufferOutput = NULL;
	CComPtr <ID3D11Buffer> pCpuBufferOutput = NULL;
	CComPtr <ID3D11Buffer> pBufferDrawFrom = NULL;

	CComPtr <ID3D11Buffer> pCBuffer = NULL;						    // 定数バッファ

	// シェーダのコンパイル オプション
#if defined(DEBUG) || defined(_DEBUG)
	UINT g_flagCompile = D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION
		| D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR;
#else
	UINT g_flagCompile = D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR;
#endif

	struct VERTEX
	{
		DirectX::XMFLOAT3 Pos;	//位置
		DirectX::XMFLOAT3 Normal;//法線
		DirectX::XMFLOAT2 Tex;	//UV座標
		DirectX::XMFLOAT4 Color;	//頂点色
		DirectX::XMFLOAT4 Param;	//汎用パラメータ
	};
	VERTEX* v;			//頂点データ

public:
	ComputeShader() {};
	~ComputeShader()
	{
		//delete device;
		//device = nullptr;
		//delete context;
		//context = nullptr;
		
		//delete particleSystem;
		//particleSystem = nullptr;
		delete v;
		v = nullptr;
	}
	void Initialize(ParticleSystem* ParticleSystem);
	void Update();

	DirectX::XMFLOAT3 data;
};
