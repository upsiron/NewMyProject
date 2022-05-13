#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <cstdint>
#include "Shader.h"


class FullScreenQuad
{
public:
	FullScreenQuad(ID3D11Device* device);
	virtual ~FullScreenQuad() = default;

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> EmbeddedVertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> EmbeddedPixelShader;
	Microsoft::WRL::ComPtr<ID3D11Buffer> VertexBuffer;

public:
	void Blit(ID3D11DeviceContext* ImmediateContext, ID3D11ShaderResourceView** ShaderResourceView, uint32_t StartSlot, uint32_t NumViews, ID3D11PixelShader* ReplacedPixelShader = nullptr);
	void Blit(ID3D11DeviceContext* ImmediateContext, ID3D11ShaderResourceView** ShaderResourceView, uint32_t StartSlot, uint32_t NumViews, Shader* shader);
};