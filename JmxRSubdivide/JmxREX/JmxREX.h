//		
//
//
//
//
//
//
//
#pragma  once
#include"../Config/JmxRConfig.h"
//#include"../Editor/IMGUIDx11.h"
#include"../JmxRRT/D3DUti.h"

#include"../Timer.h"

namespace jmxRCore
{
	class jmxREX
	{
	public:

		static
		struct _wnd 
		{
			HWND									wndHwnd = nullptr;
			HINSTANCE								wndHinstance = nullptr;
			u32										screenWidth;
			u32										screenHeight;
			f32										fps = 0.f;
		}wnd;

		static 
		struct _hw
		{
			ID3D11Device*							d3dDevice = nullptr;
			ID3D11DeviceContext*					d3dImmediateContext = nullptr;
			IDXGISwapChain*							swapChain = nullptr;
			ID3D11Texture2D*						depthStencilBuffer = nullptr;

			ID3D11Texture2D*						backBuffer = nullptr;
			ID3D11RenderTargetView*					renderTargetView = nullptr;
			ID3D11DepthStencilView*					depthStencilView = nullptr;
			D3D11_VIEWPORT							viewPort;
		}hw;

		static 
		Timer										timer;

		static 
		bool										jmxREXInit(u32 w, u32 h, WNDPROC wndProc);
		
		static
		void										jmxREXShutDown();


	};


	
}
