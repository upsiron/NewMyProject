#include "FullScreenQuad.h"
#include "shader.h"
#include "misc.h"

FullScreenQuad::FullScreenQuad(ID3D11Device* device)
{
	create_vs_from_cso(device, "Shaders/FullScreenQuad_vs.cso", EmbeddedVertexShader.ReleaseAndGetAddressOf(), nullptr, nullptr, 0);
	create_ps_from_cso(device, "Shaders/FullScreenQuad_ps.cso", EmbeddedPixelShader.ReleaseAndGetAddressOf());
}

void FullScreenQuad::Blit(ID3D11DeviceContext* ImmediateContext, ID3D11ShaderResourceView** ShaderResourceView, uint32_t StartSlot, uint32_t NumViews, ID3D11PixelShader* ReplacedPixelShader)
{
	ImmediateContext->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	ImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	ImmediateContext->IASetInputLayout(nullptr);

	ImmediateContext->VSSetShader(EmbeddedVertexShader.Get(), 0, 0);
	ReplacedPixelShader ? ImmediateContext->PSSetShader(ReplacedPixelShader, 0, 0) : ImmediateContext->PSSetShader(EmbeddedPixelShader.Get(), 0, 0);

	ImmediateContext->PSSetShaderResources(StartSlot, NumViews, ShaderResourceView);

	ImmediateContext->Draw(4, 0);
}

//引数にセットしたシェーダーをかけてオフスクリーンレンダーしてる
void FullScreenQuad::Blit(ID3D11DeviceContext* ImmediateContext, ID3D11ShaderResourceView** ShaderResourceView, uint32_t StartSlot, uint32_t NumViews, Shader* shader)
{
	shader->Activate(ImmediateContext);
	ImmediateContext->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	ImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	ImmediateContext->PSSetShaderResources(StartSlot, NumViews, ShaderResourceView);

	ImmediateContext->Draw(4, 0);
	shader->Disactivate(ImmediateContext);
}


