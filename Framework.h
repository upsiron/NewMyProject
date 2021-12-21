#pragma once

#include <windows.h>
#include <tchar.h>
#include <sstream>
#include <memory>

#include <d3d11.h>
#include <wrl.h>

#include "misc.h"
#include "high_resolution_timer.h"

#include "blender.h"
#include "sprite.h"
#include "GeometricPrimitive.h"

#include "Sound.h"

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#include "Input/input.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


class Framework
{
private:
	static Framework* inst;

	const HWND hwnd = NULL;
	
	//0727
	Input					input;

	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediateContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerStates[2];
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;

	UINT screenWidth;
	UINT screenHeight;

	std::unique_ptr<Sprite> font;
	std::unique_ptr<Blender> blend;

	std::unique_ptr<SoundManager> soundManager;

public:
	ID3D11Device* GetDevice() const { return device.Get(); }
	ID3D11DeviceContext* GetImmediateContext() const { return immediateContext.Get(); }
	ID3D11RenderTargetView* GetRenderTargetView() const { return renderTargetView.Get(); }
	ID3D11DepthStencilView* GetDepthStencilView() const { return depthStencilView.Get(); }
	ID3D11BlendState* GetBlendState(Blender::BLEND_STATE blend_state) const { return blend->states[blend_state].Get(); }
	HWND GetWindowHandle() { return hwnd; }
	Sprite* GetFont() const { return font.get(); }
	UINT GetScreenWidth() const { return screenWidth; }
	UINT GetScreenHeight() const { return screenHeight; }

	ID3D11RasterizerState* GetRasterizerState0() const { return rasterizerStates[0].Get(); }
	ID3D11RasterizerState* GetRasterizerState1() const { return rasterizerStates[0].Get(); }
	ID3D11DepthStencilState* GetDepthStencilState() const { return depthStencilState.Get(); }
	ID3D11SamplerState* GetSamplerState() const { return samplerState.Get(); }

	SoundManager* GetSoundManager()const { return soundManager.get(); }


public:
	static Framework& Instance() { return *inst; }

	Framework(HWND hwnd) : hwnd(hwnd),input(hwnd)
	{
		inst = this;
	}
	~Framework();
	/*{

	}*/
	int run()
	{
		MSG msg = {};

		if (!Initialize()) return 0;
		while (WM_QUIT != msg.message)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				timer.Tick();
				calculate_frame_stats();
				Update(timer.TimeInterval());
				Render(timer.TimeInterval());
			}
		}
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
		return static_cast<int>(msg.wParam);
	}

	LRESULT CALLBACK handle_message(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) { return true; }
		switch (msg)
		{
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc;
			hdc = BeginPaint(hwnd, &ps);
			EndPaint(hwnd, &ps);
			break;
		}
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_CREATE:
			break;
		case WM_KEYDOWN:
			if (wparam == VK_ESCAPE) PostMessage(hwnd, WM_CLOSE, 0, 0);
			break;
		case WM_ENTERSIZEMOVE:
			// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
			timer.Stop();
			break;
		case WM_EXITSIZEMOVE:
			// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
			// Here we reset everything based on the new window dimensions.
			timer.Start();
			break;
		default:
			return DefWindowProc(hwnd, msg, wparam, lparam);
		}
		return 0;
	}

private:
	bool Initialize();
	void Update(float elapsedTime/*Elapsed seconds from last frame*/);
	void Render(float elapsedTime/*Elapsed seconds from last frame*/);

private:
	HighResolutionTimer timer;
	void calculate_frame_stats()
	{
		// Code computes the average frames per second, and also the 
		// average time it takes to render one frame.  These stats 
		// are appended to the window caption bar.
		static int frames = 0;
		static float timeTlapsed = 0.0f;

		frames++;

		// Compute averages over one second period.
		if ((timer.time_stamp() - timeTlapsed) >= 1.0f)
		{
			float fps = static_cast<float>(frames); // fps = frameCnt / 1
			float mspf = 1000.0f / fps;
			std::ostringstream outs;
			outs.precision(6);
			outs << "FPS : " << fps << " / " << "Frame Time : " << mspf << " (ms)";
			SetWindowTextA(hwnd, outs.str().c_str());

			// Reset for next average.
			frames = 0;
			timeTlapsed += 1.0f;
		}
	}
};

