#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <directxmath.h>
#include <vector>

class StaticMesh
{
private:
	// レイピック用ポリゴン 
	struct Face {
		DirectX::XMFLOAT3 position[3];
		int materialIndex;  
	};  
	std::vector<Face> faces;
public:  
	// レイピック関数  
	int RayPick( const DirectX::XMFLOAT3& startPosition,
		const DirectX::XMFLOAT3& endPosition, 
		DirectX::XMFLOAT3* outPosition,
		DirectX::XMFLOAT3* outNormal,
		float* outLength); 
	/*引数 
	startPosition : レイを飛ばす開始座標  
	endPosition   : レイを飛ばす終了座標 
	outPosition   : レイが当たった座標 
	outNormal     : レイが当たった面の法線 
	outLength     : レイが当たった面までの距離  戻り値 : マテリアル番号*/
public:
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 texcoord;
	};
	struct ConstantBuffer
	{
		DirectX::XMFLOAT4X4 worldViewProjection;
		DirectX::XMFLOAT4X4 worldInverseTranspose;
		DirectX::XMFLOAT4 materialColor;
		DirectX::XMFLOAT4 lightDirection;
	};

	struct Material
	{
		DirectX::XMFLOAT4 color = { 0.8f, 0.8f, 0.8f, 1.0f }; // w channel is used as shininess by only specular.
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	};
	struct Subset
	{
		u_int indexStart = 0;	// start number of index buffer
		u_int indexCount = 0;	// number of vertices (indices)
		Material diffuse;
	};
	struct Mesh
	{
		Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
		std::vector<Subset> subsets;
		
		DirectX::XMFLOAT4X4 globalTransform = { 
			1, 0, 0, 0,
			0, 1, 0, 0, 
			0, 0, 1, 0, 
			0, 0, 0, 1 
		};

		void createBuffers(ID3D11Device *device, Vertex *vertices, int num_vertices, u_int *indices, int num_indices);
	};
	std::vector<Mesh> meshes;

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerStates[2];
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;

public:
	class StaticMesh(ID3D11Device *device, const char *fbxFilename);
	class StaticMesh(ID3D11Device* device, const char* fbxFilename, const char* shader_filename);
	virtual ~StaticMesh() {}

	void Render(ID3D11DeviceContext *immediateContext,
		const DirectX::XMFLOAT4X4 &worldViewProjection,
		const DirectX::XMFLOAT4X4 &worldInverseTranspose,
		const DirectX::XMFLOAT4 &lightDirection,
		const DirectX::XMFLOAT4 &materialColor,
		bool wireframe = false);

};
