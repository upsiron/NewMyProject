#include "shader.h"
#include "misc.h"

#include<memory>

#include<map>
#include <wrl.h>
#include <string>

//vs
HRESULT create_vs_from_cso(ID3D11Device *device, const char *cso_name, ID3D11VertexShader **vertex_shader, ID3D11InputLayout **input_layout, D3D11_INPUT_ELEMENT_DESC *input_element_desc, UINT num_elements)
{
	struct set_of_vertex_shader_and_input_layout
	{
		set_of_vertex_shader_and_input_layout(ID3D11VertexShader  *vertex_shader, ID3D11InputLayout *input_layout) : vertex_shader(vertex_shader), input_layout(input_layout) {}
		Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
	};
	static std::map<std::string, set_of_vertex_shader_and_input_layout> cache;

	auto it = cache.find(cso_name);
	if (it != cache.end())
	{
		*vertex_shader = it->second.vertex_shader.Get();
		(*vertex_shader)->AddRef();
		*input_layout = it->second.input_layout.Get();
		(*input_layout)->AddRef();
		return S_OK;
	}

	FILE* fp = nullptr;
	fopen_s(&fp, cso_name, "rb");
	_ASSERT_EXPR_A(fp, "CSO File not found");

	fseek(fp, 0, SEEK_END);
	long cso_sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> cso_data = std::make_unique<unsigned char[]>(cso_sz);
	fread(cso_data.get(), cso_sz, 1, fp);
	fclose(fp);

	HRESULT hr = device->CreateVertexShader(cso_data.get(), cso_sz, nullptr, vertex_shader);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	if (input_layout)
	{
		hr = device->CreateInputLayout(input_element_desc, num_elements, cso_data.get(), cso_sz, input_layout);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	//これが何かわかってない
	//cache.insert(std::make_pair(cso_name, set_of_vertex_shader_and_input_layout(*vertex_shader, *input_layout)));

	return hr;
}
//gs
HRESULT create_gs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11GeometryShader** geometry_shader)
{
	FILE* fp{ nullptr };
	fopen_s(&fp, cso_name, "rb");
	_ASSERT_EXPR_A(fp, "CSO File not found");

	fseek(fp, 0, SEEK_END);
	long cso_sz{ ftell(fp) };
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> cso_data{ std::make_unique<unsigned char[]>(cso_sz) };
	fread(cso_data.get(), cso_sz, 1, fp);
	fclose(fp);

	HRESULT hr{ S_OK };
	hr = device->CreateGeometryShader(cso_data.get(), cso_sz, nullptr, geometry_shader);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	return hr;
}
//ps
HRESULT create_ps_from_cso(ID3D11Device *device, const char *cso_name, ID3D11PixelShader **pixel_shader)
{
	static std::map<std::string, Microsoft::WRL::ComPtr<ID3D11PixelShader>> cache;
	auto it = cache.find(cso_name);
	if (it != cache.end())
	{
		//it->second.Attach(*pixel_shader);
		*pixel_shader = it->second.Get();
		(*pixel_shader)->AddRef();
		return S_OK;
	}

	FILE* fp = nullptr;
	fopen_s(&fp, cso_name, "rb");
	_ASSERT_EXPR_A(fp, "CSO File not found");

	fseek(fp, 0, SEEK_END);
	long cso_sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> cso_data = std::make_unique<unsigned char[]>(cso_sz);
	fread(cso_data.get(), cso_sz, 1, fp);
	fclose(fp);

	HRESULT hr = device->CreatePixelShader(cso_data.get(), cso_sz, nullptr, pixel_shader);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	cache.insert(std::make_pair(cso_name, *pixel_shader));

	return hr;
}
//cs
HRESULT create_cs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11ComputeShader** compute_shader)
{
	static std::map<std::string, Microsoft::WRL::ComPtr<ID3D11ComputeShader>> cache;//二回開こうとしているかチェック。
	auto it = cache.find(cso_name);
	//if (it != cache.end())
	//{
	//	//it->second.Attach(*pixel_shader);
	//	*pixel_shader = it->second.Get();
	//	(*pixel_shader)->AddRef();
	//	return S_OK;
	//}

	FILE* fp = nullptr;
	fopen_s(&fp, cso_name, "rb");
	_ASSERT_EXPR_A(fp, "CSO File not found");

	fseek(fp, 0, SEEK_END);
	long cso_sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> cso_data = std::make_unique<unsigned char[]>(cso_sz);
	fread(cso_data.get(), cso_sz, 1, fp);
	fclose(fp);

	HRESULT hr = device->CreateComputeShader(cso_data.get(), cso_sz, nullptr, compute_shader);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// UNIT.05
	cache.insert(std::make_pair(cso_name, *compute_shader));

	return hr;
}



static D3D11_INPUT_ELEMENT_DESC InputSkinnedElementDesc[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }, // UNIT.29
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "BONES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

static D3D11_INPUT_ELEMENT_DESC InputStaticElementDesc[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }, // UNIT.29
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "BONES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};


static D3D11_INPUT_ELEMENT_DESC InputParticleElementDesc[] =
{
	//{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//{ "SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//{ "PARAM", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "PARAM", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

static D3D11_INPUT_ELEMENT_DESC InputBrightElementDesc[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};


SkinnedMeshShader::SkinnedMeshShader(ID3D11Device* device, const char* vertex_cso_name, const char* pixel_cso_name)
{
	create_vs_from_cso(device, vertex_cso_name, VertexShader.ReleaseAndGetAddressOf(),
		InputLayout.ReleaseAndGetAddressOf(), InputSkinnedElementDesc, ARRAYSIZE(InputSkinnedElementDesc));

	create_ps_from_cso(device, pixel_cso_name, PixelShader.ReleaseAndGetAddressOf());

}

StaticMeshShader::StaticMeshShader(ID3D11Device* device, const char* vertex_cso_name, const char* pixel_cso_name)
{
	create_vs_from_cso(device, vertex_cso_name, VertexShader.ReleaseAndGetAddressOf(),
		InputLayout.ReleaseAndGetAddressOf(), InputStaticElementDesc, ARRAYSIZE(InputStaticElementDesc));

	create_ps_from_cso(device, pixel_cso_name, PixelShader.ReleaseAndGetAddressOf());

}

PointSpriteShader::PointSpriteShader(ID3D11Device* device, const char* vertex_cso_name, const char* geometry_cso_name, const char* pixel_cso_name)
{
	create_vs_from_cso(device, vertex_cso_name, VertexShader.ReleaseAndGetAddressOf(),
		InputLayout.ReleaseAndGetAddressOf(), InputParticleElementDesc, ARRAYSIZE(InputParticleElementDesc));

	create_gs_from_cso(device, geometry_cso_name, GeometryShader.ReleaseAndGetAddressOf());
	create_ps_from_cso(device, pixel_cso_name, PixelShader.ReleaseAndGetAddressOf());

}

BrightSpriteShader::BrightSpriteShader(ID3D11Device* device, const char* vertex_cso_name, const char* pixel_cso_name)
{
	create_vs_from_cso(device, vertex_cso_name, VertexShader.ReleaseAndGetAddressOf(),
		InputLayout.ReleaseAndGetAddressOf(), InputBrightElementDesc, ARRAYSIZE(InputBrightElementDesc));

	create_ps_from_cso(device, pixel_cso_name, PixelShader.ReleaseAndGetAddressOf());
}

BokehSpriteShader::BokehSpriteShader(ID3D11Device* device, const char* vertex_cso_name, const char* pixel_cso_name)
{
	create_vs_from_cso(device, vertex_cso_name, VertexShader.ReleaseAndGetAddressOf(),
		InputLayout.ReleaseAndGetAddressOf(), InputBrightElementDesc, ARRAYSIZE(InputBrightElementDesc));

	create_ps_from_cso(device, pixel_cso_name, PixelShader.ReleaseAndGetAddressOf());
}

//シェーダー有効化
void Shader::Activate(ID3D11DeviceContext* immediate_context)
{
	immediate_context->IASetInputLayout(InputLayout.Get());
	immediate_context->VSSetShader(VertexShader.Get(), nullptr, 0);
	immediate_context->PSSetShader(PixelShader.Get(), nullptr, 0);
	immediate_context->GSSetShader(GeometryShader.Get(), nullptr, 0);
	immediate_context->DSSetShader(DomainShader.Get(), nullptr, 0);
	immediate_context->HSSetShader(HullShader.Get(), nullptr, 0);

}
//シェーダー無効化
void Shader::Disactivate(ID3D11DeviceContext* immediate_context)
{
	immediate_context->IASetInputLayout(nullptr);

	immediate_context->VSSetShader(nullptr, nullptr, 0);
	immediate_context->PSSetShader(nullptr, nullptr, 0);
	immediate_context->GSSetShader(nullptr, nullptr, 0);
	immediate_context->DSSetShader(nullptr, nullptr, 0);
	immediate_context->HSSetShader(nullptr, nullptr, 0);

}


