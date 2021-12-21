#pragma once

#include <d3d11.h>
#include <wrl.h>
HRESULT load_texture_from_file(ID3D11Device* device, const wchar_t* file_name, ID3D11ShaderResourceView** shader_resource_view, D3D11_TEXTURE2D_DESC* texture2d_desc);
HRESULT make_dummy_texture(ID3D11Device* device, ID3D11ShaderResourceView** shader_resource_view);

void combine_resource_path(wchar_t(&combined_resource_path)[256], const wchar_t* referrer_filename, const wchar_t* referent_filename);

class Texture
{
protected:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ShaderResourceView = nullptr;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RenderTargetView = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DepthStencilView = nullptr;

	// テクスチャ情報
	D3D11_TEXTURE2D_DESC texture2d_desc;

public:
	Texture();
	virtual ~Texture();
	bool Load(ID3D11Device* device, const wchar_t* filename = nullptr);
	//bool LoadMipMap(ID3D11Device*device, const wchar_t* filename = nullptr);

	void Set(ID3D11DeviceContext* device_context, UINT Slot = 0, BOOL flg = TRUE);

	UINT GetWidth() { return texture2d_desc.Width; }
	UINT GetHeight() { return texture2d_desc.Height; }
	bool Create(ID3D11Device* device, u_int width, u_int height, DXGI_FORMAT format);
	//bool CreateMipMap(ID3D11Device* device, u_int width, u_int height, DXGI_FORMAT format);
	// 深度ステンシルバッファ作成
	bool CreateDepth(ID3D11Device* device, u_int width, u_int height, DXGI_FORMAT format = DXGI_FORMAT_R24G8_TYPELESS);


	ID3D11RenderTargetView* GetRenderTargetView() { return RenderTargetView.Get(); }
	ID3D11DepthStencilView* GetDepthStencilView() { return DepthStencilView.Get(); }

	//CubeMap用
	//bool CreateCube(ID3D11Device* device, u_int width, u_int height, DXGI_FORMAT format, int miplevel = 1);
	//bool CreateCubeDepthStencil(ID3D11Device* device, u_int width, u_int height);


};