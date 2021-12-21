#include "sprite.h"

#include "shader.h"
#include "misc.h"

#include "texture.h"

Sprite::Sprite(ID3D11Device *device, const wchar_t *file_name)
{
	HRESULT hr = S_OK;

	Vertex vertices[] = {
		{ DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT4(1, 1, 1, 1), DirectX::XMFLOAT2(0, 0) },
		{ DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT4(1, 1, 1, 1), DirectX::XMFLOAT2(0, 0) },
		{ DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT4(1, 1, 1, 1), DirectX::XMFLOAT2(0, 0) },
		{ DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT4(1, 1, 1, 1), DirectX::XMFLOAT2(0, 0) },
	};
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = sizeof(vertices);
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC; // UNIT.03
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // UNIT.03
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA subresource_data = {};
	subresource_data.pSysMem = vertices;
	subresource_data.SysMemPitch = 0; //Not use for vertex buffers.
	subresource_data.SysMemSlicePitch = 0; //Not use for vertex buffers.
	hr = device->CreateBuffer(&bufferDesc, &subresource_data, vertexBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	_ASSERT_EXPR(!inputLayout, L"'input_layout' must be uncreated.");
	D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	create_vs_from_cso(device, "Shaders/sprite_vs.cso", vertexShader.GetAddressOf(), inputLayout.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
	create_ps_from_cso(device, "Shaders/sprite_ps.cso", pixelShader.GetAddressOf());

	D3D11_RASTERIZER_DESC rasterizerDesc = {}; //https://msdn.microsoft.com/en-us/library/windows/desktop/ff476198(v=vs.85).aspx
	rasterizerDesc.FillMode = D3D11_FILL_SOLID; //D3D11_FILL_WIREFRAME, D3D11_FILL_SOLID
	rasterizerDesc.CullMode = D3D11_CULL_NONE; //D3D11_CULL_NONE, D3D11_CULL_FRONT, D3D11_CULL_BACK   
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.DepthBias = 0; //https://msdn.microsoft.com/en-us/library/windows/desktop/cc308048(v=vs.85).aspx
	rasterizerDesc.DepthBiasClamp = 0;
	rasterizerDesc.SlopeScaledDepthBias = 0;
	rasterizerDesc.DepthClipEnable = FALSE;
	rasterizerDesc.ScissorEnable = FALSE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.AntialiasedLineEnable = FALSE;
	hr = device->CreateRasterizerState(&rasterizerDesc, rasterizerState.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// UNIT.04
	hr = load_texture_from_file(device, file_name, &shaderResourceView, &texture2dDesc);

	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC; //UNIT.06
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	memcpy(samplerDesc.BorderColor, &DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), sizeof(DirectX::XMFLOAT4));
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = device->CreateSamplerState(&samplerDesc, samplerState.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// UNIT.06
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	depthStencilDesc.DepthEnable = FALSE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.StencilEnable = FALSE;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	hr = device->CreateDepthStencilState(&depthStencilDesc, depthStencilState.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// Reference web pages for blending
	// https://msdn.microsoft.com/en-us/library/bb976070.aspx
	// https://en.wikipedia.org/wiki/Alpha_compositing
	// https://love2d.org/wiki/BlendMode_Formulas
	// UNIT.07
	//D3D11_blendDesc blendDesc = {}; //https://msdn.microsoft.com/en-us/library/windows/desktop/ff476087(v=vs.85).aspx
	//blendDesc.AlphaToCoverageEnable = FALSE; //https://msdn.microsoft.com/en-us/library/windows/desktop/bb205072(v=vs.85).aspx
	//blendDesc.IndependentBlendEnable = FALSE; //Set to TRUE to enable independent blending. If set to FALSE, only the RenderTarget[0] members are used; RenderTarget[1..7] are ignored.
	//blendDesc.RenderTarget[0].BlendEnable = TRUE;
	//blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	//blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	//blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	//blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	//blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	//blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	//blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	//hr = device->CreateBlendState(&blendDesc, blend_state.GetAddressOf());
	//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void Sprite::Render(ID3D11DeviceContext *immediateContext, float dx, float dy, float dw, float dh, float sx, float sy, float sw, float sh, float angle/*degree*/, float r, float g, float b, float a) const
// dx, dy : Coordinate of sprite's left-top corner in screen space 
// dw, dh : Size of sprite in screen space 
// angle : Raotation angle (Rotation centre is sprite's centre), unit is degree
// r, g, b : Color of sprite's each vertices 
{
	// UNIT.03
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	immediateContext->RSGetViewports(&numViewports, &viewport);
	float screenWidth = viewport.Width;
	float screenHeight = viewport.Height;

	// Set each sprite's vertices coordinate to screen spaceenum BLEND_STATE
	// left-top
	float x0 = dx;
	float y0 = dy;
	// right-top
	float x1 = dx + dw;
	float y1 = dy;
	// left-bottom
	float x2 = dx;
	float y2 = dy + dh;
	// right-bottom
	float x3 = dx + dw;
	float y3 = dy + dh;

	// Translate sprite's centre to origin (rotate centre)
	float mx = dx + dw*0.5f;
	float my = dy + dh*0.5f;
	x0 -= mx;
	y0 -= my;
	x1 -= mx;
	y1 -= my;
	x2 -= mx;
	y2 -= my;
	x3 -= mx;
	y3 -= my;

	// Rotate each sprite's vertices by angle
	float rx, ry;
	float cos = cosf(angle*0.01745f);
	float sin = sinf(angle*0.01745f);
	rx = x0;
	ry = y0;
	x0 = cos*rx + -sin*ry;
	y0 = sin*rx + cos*ry;
	rx = x1;
	ry = y1;
	x1 = cos*rx + -sin*ry;
	y1 = sin*rx + cos*ry;
	rx = x2;
	ry = y2;
	x2 = cos*rx + -sin*ry;
	y2 = sin*rx + cos*ry;
	rx = x3;
	ry = y3;
	x3 = cos*rx + -sin*ry;
	y3 = sin*rx + cos*ry;

	// Translate sprite's centre to original position
	x0 += mx;
	y0 += my;
	x1 += mx;
	y1 += my;
	x2 += mx;
	y2 += my;
	x3 += mx;
	y3 += my;

	// Convert to NDC space
	x0 = 2.0f*x0 / screenWidth - 1.0f;
	y0 = 1.0f - 2.0f*y0 / screenHeight;
	x1 = 2.0f*x1 / screenWidth - 1.0f;
	y1 = 1.0f - 2.0f*y1 / screenHeight;
	x2 = 2.0f*x2 / screenWidth - 1.0f;
	y2 = 1.0f - 2.0f*y2 / screenHeight;
	x3 = 2.0f*x3 / screenWidth - 1.0f;
	y3 = 1.0f - 2.0f*y3 / screenHeight;

	HRESULT hr = S_OK;
	D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
	D3D11_MAPPED_SUBRESOURCE mappedBuffer;
	hr = immediateContext->Map(vertexBuffer.Get(), 0, map, 0, &mappedBuffer);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	Vertex *vertices = static_cast<Vertex *>(mappedBuffer.pData);
	vertices[0].position.x = x0;
	vertices[0].position.y = y0;
	vertices[1].position.x = x1;
	vertices[1].position.y = y1;
	vertices[2].position.x = x2;
	vertices[2].position.y = y2;
	vertices[3].position.x = x3;
	vertices[3].position.y = y3;
	vertices[0].position.z = vertices[1].position.z = vertices[2].position.z = vertices[3].position.z = 0.0f;

	DirectX::XMFLOAT4 color(r, g, b, a);
	vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = color;

	vertices[0].texcoord.x = static_cast<FLOAT>(sx) / texture2dDesc.Width;
	vertices[0].texcoord.y = static_cast<FLOAT>(sy) / texture2dDesc.Height;
	vertices[1].texcoord.x = static_cast<FLOAT>(sx + sw) / texture2dDesc.Width;
	vertices[1].texcoord.y = static_cast<FLOAT>(sy) / texture2dDesc.Height;
	vertices[2].texcoord.x = static_cast<FLOAT>(sx) / texture2dDesc.Width;
	vertices[2].texcoord.y = static_cast<FLOAT>(sy + sh) / texture2dDesc.Height;
	vertices[3].texcoord.x = static_cast<FLOAT>(sx + sw) / texture2dDesc.Width;
	vertices[3].texcoord.y = static_cast<FLOAT>(sy + sh) / texture2dDesc.Height;

	immediateContext->Unmap(vertexBuffer.Get(), 0);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	immediateContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	immediateContext->IASetInputLayout(inputLayout.Get());

	immediateContext->RSSetState(rasterizerState.Get());

	immediateContext->VSSetShader(vertexShader.Get(), nullptr, 0);
	immediateContext->PSSetShader(pixelShader.Get(), nullptr, 0);

	immediateContext->PSSetShaderResources(0, 1, shaderResourceView.GetAddressOf());
	immediateContext->PSSetSamplers(0, 1, samplerState.GetAddressOf());

	immediateContext->OMSetDepthStencilState(depthStencilState.Get(), 1);

	immediateContext->Draw(4, 0);
}

void Sprite::TextOut(ID3D11DeviceContext *immediateContext, std::string s, float x, float y, float w, float h, float r, float g, float b, float a) const
{
	float sw = static_cast<float>(texture2dDesc.Width / 16);
	float sh = static_cast<float>(texture2dDesc.Height / 16);
	float cursor = 0;
	for (auto c : s)
	{
		LONG sx = c % 0x0F;
		Render(immediateContext, x + cursor, y, w, h, sw*(c & 0x0F), sh*(c >> 4), sw, sh, 0, r, g, b, a);
		cursor += w;
	}
}

SpriteBatch::SpriteBatch(ID3D11Device *device, const wchar_t *filename, size_t maxInstance) : MAX_INSTANCES(maxInstance)
{
	HRESULT hr = S_OK;

	vertex vertices[] =
	{
		{ DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT2(0, 0) },
		{ DirectX::XMFLOAT3(1, 0, 0), DirectX::XMFLOAT2(1, 0) },
		{ DirectX::XMFLOAT3(0, 1, 0), DirectX::XMFLOAT2(0, 1) },
		{ DirectX::XMFLOAT3(1, 1, 0), DirectX::XMFLOAT2(1, 1) },
	};

	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = sizeof(vertices);
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA subresource_data = {};
	subresource_data.pSysMem = vertices;
	subresource_data.SysMemPitch = 0; //Not use for vertex buffers.
	subresource_data.SysMemSlicePitch = 0; //Not use for vertex buffers.
	hr = device->CreateBuffer(&bufferDesc, &subresource_data, vertexBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NDC_TRANSFORM", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "NDC_TRANSFORM", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "NDC_TRANSFORM", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "NDC_TRANSFORM", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD_TRANSFORM", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	};
	create_vs_from_cso(device, "sprite_batch_vs.cso", vertexShader.GetAddressOf(), inputLayout.GetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
	create_ps_from_cso(device, "sprite_batch_ps.cso", pixelShader.GetAddressOf());

	Instance *instances = new Instance[MAX_INSTANCES];
	{
		D3D11_BUFFER_DESC bufferDesc = {};
		D3D11_SUBRESOURCE_DATA subresourceData = {};

		bufferDesc.ByteWidth = static_cast<UINT>(sizeof(Instance)*MAX_INSTANCES);
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;
		subresourceData.pSysMem = instances;
		subresourceData.SysMemPitch = 0; //Not use for vertex buffers.mm 
		subresourceData.SysMemSlicePitch = 0; //Not use for vertex buffers.
		hr = device->CreateBuffer(&bufferDesc, &subresourceData, instanceBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	delete[] instances;

	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID; //D3D11_FILL_WIREFRAME, D3D11_FILL_SOLID
	rasterizerDesc.CullMode = D3D11_CULL_NONE; //D3D11_CULL_NONE, D3D11_CULL_FRONT, D3D11_CULL_BACK   
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0;
	rasterizerDesc.SlopeScaledDepthBias = 0;
	rasterizerDesc.DepthClipEnable = FALSE;
	rasterizerDesc.ScissorEnable = FALSE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.AntialiasedLineEnable = FALSE;
	hr = device->CreateRasterizerState(&rasterizerDesc, rasterizerState.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	hr = load_texture_from_file(device, filename, &shaderResourceView, &texture2d_desc);

	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC; //UNIT.06
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	memcpy(samplerDesc.BorderColor, &DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), sizeof(DirectX::XMFLOAT4));
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = device->CreateSamplerState(&samplerDesc, samplerState.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	depthStencilDesc.DepthEnable = FALSE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.StencilEnable = FALSE;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	hr = device->CreateDepthStencilState(&depthStencilDesc, depthStencilState.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void SpriteBatch::Begin(ID3D11DeviceContext *immediateContext)
{
	HRESULT hr = S_OK;

	UINT strides[2] = { sizeof(vertex), sizeof(Instance) };
	UINT offsets[2] = { 0, 0 };
	ID3D11Buffer *vbs[2] = { vertexBuffer.Get(), instanceBuffer.Get() };
	immediateContext->IASetVertexBuffers(0, 2, vbs, strides, offsets);
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	immediateContext->IASetInputLayout(inputLayout.Get());
	immediateContext->OMSetDepthStencilState(depthStencilState.Get(), 1);
	immediateContext->RSSetState(rasterizerState.Get());
	immediateContext->VSSetShader(vertexShader.Get(), nullptr, 0);
	immediateContext->PSSetShader(pixelShader.Get(), nullptr, 0);
	immediateContext->PSSetShaderResources(0, 1, shaderResourceView.GetAddressOf());
	immediateContext->PSSetSamplers(0, 1, samplerState.GetAddressOf());

	UINT numViewports = 1;
	immediateContext->RSGetViewports(&numViewports, &viewport);

	D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
	D3D11_MAPPED_SUBRESOURCE mappedBuffer;
	hr = immediateContext->Map(instanceBuffer.Get(), 0, map, 0, &mappedBuffer);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	instances = static_cast<Instance *>(mappedBuffer.pData);

	countInstance = 0;
}
void SpriteBatch::Render(ID3D11DeviceContext *immediateContext, float dx, float dy, float dw, float dh, float sx, float sy, float sw, float sh, float angle/*degree*/, float r, float g, float b, float a)
{
	_ASSERT_EXPR(countInstance < MAX_INSTANCES, L"Number of instances must be less than MAX_INSTANCES.");

	float cx = dw*0.5f, cy = dh*0.5f; /*Center of Rotation*/
#if 0
	DirectX::XMVECTOR scaling = DirectX::XMVectorSet(dw, dh, 1.0f, 0.0f);
	DirectX::XMVECTOR origin = DirectX::XMVectorSet(cx, cy, 0.0f, 0.0f);
	DirectX::XMVECTOR translation = DirectX::XMVectorSet(dx, dy, 0.0f, 0.0f);
	DirectX::XMMATRIX M = DirectX::XMMatrixAffineTransformation2D(scaling, origin, angle*0.01745f, translation);
	DirectX::XMMATRIX N(
		2.0f / viewport.Width, 0.0f, 0.0f, 0.0f,
		0.0f, -2.0f / viewport.Height, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f, 1.0f);
	XMStoreFloat4x4(&instances[countInstance].ndcTransform, DirectX::XMMatrixTranspose(M*N)); //column_major
#else
	FLOAT c = cosf(angle*0.01745f);
	FLOAT s = sinf(angle*0.01745f);
	FLOAT w = 2.0f / viewport.Width;
	FLOAT h = -2.0f / viewport.Height;
	instances[countInstance].ndcTransform._11 = w*dw*c;
	instances[countInstance].ndcTransform._21 = h*dw*s;
	instances[countInstance].ndcTransform._31 = 0.0f;
	instances[countInstance].ndcTransform._41 = 0.0f;
	instances[countInstance].ndcTransform._12 = w*dh*-s;
	instances[countInstance].ndcTransform._22 = h*dh*c;
	instances[countInstance].ndcTransform._32 = 0.0f;
	instances[countInstance].ndcTransform._42 = 0.0f;
	instances[countInstance].ndcTransform._13 = 0.0f;
	instances[countInstance].ndcTransform._23 = 0.0f;
	instances[countInstance].ndcTransform._33 = 1.0f;
	instances[countInstance].ndcTransform._43 = 0.0f;
	instances[countInstance].ndcTransform._14 = w*(-cx*c + -cy*-s + cx + dx) - 1.0f;
	instances[countInstance].ndcTransform._24 = h*(-cx*s + -cy*c + cy + dy) + 1.0f;
	instances[countInstance].ndcTransform._34 = 0.0f;
	instances[countInstance].ndcTransform._44 = 1.0f;
#endif
	float tw = static_cast<float>(texture2d_desc.Width);
	float th = static_cast<float>(texture2d_desc.Height);
	instances[countInstance].texcoordTransform = DirectX::XMFLOAT4( sx / tw, sy / th, sw / tw, sh / th);
	instances[countInstance].color = DirectX::XMFLOAT4(r, g, b, a);

	countInstance++;
}
void SpriteBatch::End(ID3D11DeviceContext *immediateContext)
{
	immediateContext->Unmap(instanceBuffer.Get(), 0);

	immediateContext->DrawInstanced(4, countInstance, 0, 0);
}
