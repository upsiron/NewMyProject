// UNIT.02
#pragma once
#include <d3d11.h>
#include <wrl.h>


HRESULT create_vs_from_cso(ID3D11Device *device, const char *cso_name, ID3D11VertexShader **vertex_shader, ID3D11InputLayout **input_layout, D3D11_INPUT_ELEMENT_DESC *input_element_desc, UINT num_elements);
HRESULT create_ps_from_cso(ID3D11Device *device, const char *cso_name, ID3D11PixelShader **pixel_shader);
HRESULT create_gs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11GeometryShader** geometry_shader);
HRESULT create_cs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11ComputeShader** compute_shader);

class Shader
{
private:
protected:
	Microsoft::WRL::ComPtr<ID3D11InputLayout> InputLayout{};
	Microsoft::WRL::ComPtr<ID3D11VertexShader> VertexShader{};
	Microsoft::WRL::ComPtr<ID3D11PixelShader> PixelShader{};
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> GeometryShader{};
	Microsoft::WRL::ComPtr<ID3D11HullShader> HullShader{};
	Microsoft::WRL::ComPtr<ID3D11DomainShader> DomainShader{};



public:
	Shader() {}
	//shader(ID3D11Device* device,  const char* vertex_cso_name, const char* pixel_cso_name);
	//shader(ID3D11Device* device,  const char* vertex_cso_name, const char* geometry_cso_name, const char* pixel_cso_name);

	void Activate(ID3D11DeviceContext* immediate_context);
	void Disactivate(ID3D11DeviceContext* immediate_context);
	friend class skinned_mesh;

};

class SkinnedMeshShader : public Shader
{
public:
	SkinnedMeshShader(ID3D11Device* device, const char* vertex_cso_name, const char* pixel_cso_name);
};

class StaticMeshShader : public Shader
{
public:
	StaticMeshShader(ID3D11Device* device, const char* vertex_cso_name, const char* pixel_cso_name);
};

class PointSpriteShader : public Shader
{
public:
	PointSpriteShader(ID3D11Device* device, const char* vertex_cso_name, const char* geometry_cso_name, const char* pixel_cso_name);

};

class BrightSpriteShader : public Shader
{
public:
	BrightSpriteShader(ID3D11Device* device, const char* vertex_cso_name, const char* pixel_cso_name);
};

class BokehSpriteShader : public Shader
{
public:
	BokehSpriteShader(ID3D11Device* device, const char* vertex_cso_name, const char* pixel_cso_name);
};

//class SpriteShader : public Shader
//{
//public:
//	SpriteShader(ID3D11Device* device, const char* vertex_cso_name, const char* pixel_cso_name);
//};
