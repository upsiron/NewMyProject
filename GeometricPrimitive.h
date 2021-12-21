#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <directxmath.h>

class GeometricPrimitive
{
public:
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
	};
	struct ConstantBuffer
	{
		DirectX::XMFLOAT4X4 worldViewProjection;
		DirectX::XMFLOAT4X4 worldInverseTranspose;
		DirectX::XMFLOAT4 materialColor;
		DirectX::XMFLOAT4 lightDirection;
	};

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerStates[2];
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;

public:
	GeometricPrimitive(ID3D11Device *device);
	virtual ~GeometricPrimitive() {}

	void Render(ID3D11DeviceContext *immediateContext, const DirectX::XMFLOAT4X4 &worldViewProjection, const DirectX::XMFLOAT4X4 &worldInverseTranspose, const DirectX::XMFLOAT4 &lightDirection, const DirectX::XMFLOAT4 &materialColor, bool wireframe = false);

protected:
	void CreateBuffers(ID3D11Device *device, Vertex *vertices, int numVertices, u_int *indices, int numIndices);
};

class GeometricCube : public GeometricPrimitive
{
public:
	GeometricCube(ID3D11Device *device);
};

class GeometricCylinder : public GeometricPrimitive
{
public:
	GeometricCylinder(ID3D11Device *device, u_int slices);
};

class GeometricSphere : public GeometricPrimitive
{
public:
	GeometricSphere(ID3D11Device *device, u_int slices, u_int stacks);
};

class GeometricCapsule : public GeometricPrimitive
{
public:
	GeometricCapsule(ID3D11Device *device);
};

