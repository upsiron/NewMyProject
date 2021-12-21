#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <directxmath.h>
#include <string>

class Sprite
{
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	D3D11_TEXTURE2D_DESC texture2dDesc;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;

public:
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 texcoord;
	};

	Sprite(ID3D11Device *device, const wchar_t *filename);
	virtual ~Sprite()
	{

	}

	void Render(ID3D11DeviceContext *immediateContext, float dx, float dy, float dw, float dh, float sx, float sy, float sw, float sh, float angle = 0/*degree*/, float r = 1, float g = 1, float b = 1, float a = 1) const;
	
	void TextOut(ID3D11DeviceContext *immediateContext, std::string s, float x, float y, float w, float h, float r = 1, float g = 1, float b = 1, float a = 1) const;
};

class SpriteBatch
{
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	D3D11_TEXTURE2D_DESC texture2d_desc;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;

	const size_t MAX_INSTANCES = 256;
	struct Instance
	{
		DirectX::XMFLOAT4X4 ndcTransform;
		DirectX::XMFLOAT4 texcoordTransform;
		DirectX::XMFLOAT4 color;
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> instanceBuffer;

public:
	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texcoord;
	};

	SpriteBatch(ID3D11Device *device, const wchar_t *filename, size_t maxInstance = 256);

	void Begin(ID3D11DeviceContext *immediate_context);
	void Render(ID3D11DeviceContext *immediate_context, float dx, float dy, float dw, float dh, float sx, float sy, float sw, float sh, float angle/*degree*/, float r, float g, float b, float a);
	void End(ID3D11DeviceContext *immediate_context);

private:
	D3D11_VIEWPORT viewport;

	UINT countInstance = 0;
	Instance *instances = nullptr;
};
