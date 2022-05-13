#include "ComputeShader.h"
#include "Shader.h"

void ComputeShader::Initialize(ParticleSystem* _ParticleSystem)
{
	particleSystem =_ParticleSystem;

	// コンピュート・シェーダの作成
	CreateShader();

	// リソースの作成（各種データバッファ）
	CreateResource();

	// シェーダ リソース ビューの作成（入力用ビュー）
	CreateSRV();

	// アンオーダード・アクセス・ビューの作成　（出力用ビュー）
	CreateUAV();



}

void ComputeShader::Update()
{
	// コンピュート・シェーダを使った演算
	RunComputeShader();
	return;
}

//コンピュートシェーダの作成
void ComputeShader::CreateShader(void)
{
	HRESULT hr = S_OK;
	hr = create_cs_from_cso(device, "Shaders/PointSprite_cs.cso", &pComputeShader.p);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	return;
}

//入力リソースバッファ、出力リソースバッファ、CPUからのリード用バッファ等の作成
void ComputeShader::CreateResource(void)
{

	HRESULT hr/* = S_OK*/;

	D3D11_BUFFER_DESC Desc;
	ZeroMemory(&Desc, sizeof(Desc));
	Desc.ByteWidth = sizeof(ParticleSystem::ParticleData) * particleSystem->NumParticles; // バッファ サイズ
	Desc.Usage = D3D11_USAGE_DEFAULT;//ステージの入出力はOK。GPUの入出力OK。
	Desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED; // 構造化バッファ
	Desc.StructureByteStride = sizeof(ParticleSystem::ParticleData);

	D3D11_SUBRESOURCE_DATA SubResource;//サブリソースの初期化用データを定義
	SubResource.pSysMem = particleSystem->data;//
	SubResource.SysMemPitch = 0;
	SubResource.SysMemSlicePitch = 0;

	//Particle
	// 最初の入力リソース(データを初期化する)
	hr = device->CreateBuffer(&Desc, &SubResource, &pBufferInput.p);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// 最初の出力リソース（初期化用データは必要ない）
	hr = device->CreateBuffer(&Desc, NULL, &pCpuBufferOutput.p);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	//HRESULT hr = S_OK;

	//D3D11_BUFFER_DESC Desc;
	ZeroMemory(&Desc, sizeof(Desc));
	Desc.ByteWidth = sizeof(VERTEX) * particleSystem->NumParticles; // バッファ サイズ
	Desc.Usage = D3D11_USAGE_DEFAULT;//ステージの入出力はOK。GPUの入出力OK。
	Desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED; // 構造化バッファ
	Desc.StructureByteStride = sizeof(VERTEX);

	struct VERTEX* posVertex = new VERTEX[particleSystem->NumParticles];

	D3D11_SUBRESOURCE_DATA VerSubResource;//サブリソースの初期化用データを定義
	VerSubResource.pSysMem = posVertex;//
	VerSubResource.SysMemPitch = 0;
	VerSubResource.SysMemSlicePitch = 0;


	// 最初の出力リソース（初期化用データは必要ない）
	hr = device->CreateBuffer(&Desc, NULL, &pBufferOutput.p);
	delete[] posVertex;
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	//--------------------------------------------------------
	// リードバック用バッファ リソースの作成（CPUからもアクセスできるステージングバッファ）
	//--------------------------------------------------------
	ZeroMemory(&Desc, sizeof(Desc));
	Desc.ByteWidth = sizeof(ParticleSystem::ParticleData) * particleSystem->NumParticles;	// バッファ サイズ

	Desc.Usage = D3D11_USAGE_STAGING;  // CPUから読み書き可能なリソース
	Desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ; // CPUから読み込む
	Desc.StructureByteStride = sizeof(ParticleSystem::ParticleData);

	hr = device->CreateBuffer(&Desc, NULL, &pToCpuReadBackBuffer.p);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//--------------------------------------------------------
	// 書き込み用バッファ リソースの作成（CPUからもアクセスできるステージングバッファ）
	//--------------------------------------------------------
	ZeroMemory(&Desc, sizeof(Desc));
	Desc.ByteWidth = sizeof(ParticleSystem::ParticleData) * particleSystem->NumParticles;	// バッファ サイズ

	//Desc.Usage = D3D11_USAGE_STAGING;  // CPUから読み書き可能なリソース
	Desc.Usage = D3D11_USAGE_DYNAMIC;  // CPUから読み書き可能なリソース
	Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // CPUから読み込む
	Desc.StructureByteStride = sizeof(ParticleSystem::ParticleData);

	hr = device->CreateBuffer(&Desc, NULL, &pToCpuWriteBuffer.p);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	{
		// 頂点バッファの定義　（書き込みにも対応）
		D3D11_BUFFER_DESC vBufferDesc;
		vBufferDesc.Usage = D3D11_USAGE_DEFAULT;      // デフォルト使用法
		vBufferDesc.ByteWidth = sizeof(VERTEX) * particleSystem->NumParticles; // PerticleCount = 頂点数
		vBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // 頂点バッファとストリーム出力として使う
		vBufferDesc.CPUAccessFlags = 0;
		vBufferDesc.MiscFlags = 0;
		vBufferDesc.StructureByteStride = 0;

		// 頂点バッファのサブリソースの初期値(頂点座標)
		struct VERTEX* posVertex = new VERTEX[particleSystem->NumParticles];

		// 頂点バッファのサブリソースの定義(バッファーリソースでも一つだけ持てる）
		D3D11_SUBRESOURCE_DATA vSubData;//初期化用データを作成
		vSubData.pSysMem = posVertex;  // バッファ・データの初期値
		vSubData.SysMemPitch = 0;
		vSubData.SysMemSlicePitch = 0;

		// 描画用頂点入力バッファの作成　　初期化用データを送っている
		hr = device->CreateBuffer(&vBufferDesc, &vSubData, &pBufferDrawFrom.p);
		delete[] posVertex;
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}


	return;
}

//シェーダ リソース ビューの作成（入力用ビュー）
void ComputeShader::CreateSRV(void)
{
	HRESULT hr = S_OK;

	D3D11_SHADER_RESOURCE_VIEW_DESC DescSRV;
	ZeroMemory(&DescSRV, sizeof(DescSRV));
	DescSRV.Format = DXGI_FORMAT_UNKNOWN;
	DescSRV.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	DescSRV.Buffer.ElementWidth = particleSystem->NumParticles; // データ数

	//Particle Data  シェーダ リソース ビューの作成
	hr = device->CreateShaderResourceView(pBufferInput.p, &DescSRV, &pSRV.p);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	return;
}

//アンオーダード・アクセス・ビューの作成　出力用ビュー
void ComputeShader::CreateUAV(void)
{
	HRESULT hr = S_OK;


	D3D11_UNORDERED_ACCESS_VIEW_DESC DescUAV;
	ZeroMemory(&DescUAV, sizeof(DescUAV));
	DescUAV.Format = DXGI_FORMAT_UNKNOWN;
	DescUAV.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	DescUAV.Buffer.NumElements = particleSystem->NumParticles; // データ数

    // Particle Data アンオーダード・アクセス・ビューの作成
	hr = device->CreateUnorderedAccessView(pBufferOutput.p, &DescUAV, &pUAV.p);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// Particle Data アンオーダード・アクセス・ビューの作成
	hr = device->CreateUnorderedAccessView(pCpuBufferOutput.p, &DescUAV, &pCpuUAV.p);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	return;
}

//コンピュート シェーダを使った演算
void ComputeShader::RunComputeShader(void)
{
	HRESULT hr = S_OK;
	
	//入力データをバッファに入れる
	D3D11_MAPPED_SUBRESOURCE MappedResource = { 0 };
	context->Map(pToCpuWriteBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	ParticleSystem::ParticleData* pBuf = (ParticleSystem::ParticleData*)MappedResource.pData;
	memcpy(MappedResource.pData, particleSystem->data, sizeof(ParticleSystem::ParticleData) * particleSystem->NumParticles);
	context->Unmap(pToCpuWriteBuffer, 0);

	context->CopyResource(pBufferInput.p, pToCpuWriteBuffer.p);

	//定数バッファをコンピュートシェーダーに設定
	context->CSSetConstantBuffers(0, 1, &pCBuffer.p);

	// シェーダを設定
	context->CSSetShader(pComputeShader.p, NULL, 0);

	// アンオーダード・アクセス・ビューの設定（出力用）
	context->CSSetUnorderedAccessViews(0, 1, &pUAV.p, NULL);
	context->CSSetUnorderedAccessViews(1, 1, &pCpuUAV.p, NULL);

	// シェーダ リソース ビューの設定（入力用）
	context->CSSetShaderResources(0, 1, &pSRV.p);

	// コンピュート・シェーダの実行
	context->Dispatch(2000, 1, 1);//グループの数

	//GPU内の結果をCPUからリード可能なステージングバッファに転送
	context->CopyResource(pToCpuReadBackBuffer.p, pCpuBufferOutput.p);

	//vertexbufferに入れるbufferに出力したbufferをコピー
	context->CopyResource(pBufferDrawFrom.p, pBufferOutput.p);

	//出力データをバッファに入れる
	D3D11_MAPPED_SUBRESOURCE MappedResourceResult = { 0 };
	context->Map(pToCpuReadBackBuffer, 0, D3D11_MAP_READ, 0, &MappedResourceResult);
	ParticleSystem::ParticleData* pBufRes = (ParticleSystem::ParticleData*)MappedResourceResult.pData;
	memcpy(particleSystem->data, pBufRes, sizeof(ParticleSystem::ParticleData) * particleSystem->NumParticles);
	context->Unmap(pToCpuReadBackBuffer, 0);

	return;
}
