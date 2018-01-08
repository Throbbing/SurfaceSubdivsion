#include"../Config/JmxRConfig.h"
#include"../JmxRRT/D3DUti.h"
#include"../3rdParty/IMGUI/imgui.h"
#include"../Timer.h"
#include<map>
#include<string>


#pragma once

namespace jmxRCore
{
	class jmxRGUILayout;

	class jmxRGUIDx11
	{
	public:


		static void guiInit(u32 w,u32 h,
			HWND hwnd,HINSTANCE hinstance,
			ID3D11Device* device,ID3D11DeviceContext* context,
			ID3D11RenderTargetView* rtv,ID3D11DepthStencilView* dsv,
			const D3D11_VIEWPORT& vp);
		static void guiNewFrame(f32 deltaTime);
		static void guiRender(jmxRGUILayout* guiLayout);
		static void guiShutDown();
		




	private:
//		void initWin32();
//		void initD3D();
		static void initGUI();
		static void createFontsTexture();
		static void createShader();
		static void createBuf();
		static void createStates();
		static void render(ImDrawData* drawData);

		static HWND							mWndHwnd;
		static HINSTANCE					mWndHinstance;
		static ID3D11Device*				md3dDevice;
		static ID3D11DeviceContext*			md3dImmediateContext;
		static ID3D11RenderTargetView*		mRTV;
		static ID3D11DepthStencilView*		mDSV;


		static ID3D11VertexShader*			mGuiVs;
		static ID3D11PixelShader*			mGuiPs;
		static ID3D11Buffer*				mCBTrans;
		static ID3D11Buffer*				mVB ;
		static u32							mVerticesCount;
		static ID3D11Buffer*				mIB ;
		static u32							mIndicesCount;
		static ID3D11InputLayout*			mLayout;
		static ID3D11SamplerState*			mSam;
		static ID3D11ShaderResourceView*	mFontSRV;

		static ID3D11BlendState*			mBS;
		static ID3D11DepthStencilState*		mDSS;
		static ID3D11RasterizerState*		mRS;
		static D3D11_VIEWPORT				mViewPort;

		static u32							mWidth;
		static u32							mHeight;




//		IPCManager					mIPCMgr;
		
		
	};

	class jmxRGUILayout
	{
	public:
		jmxRGUILayout(){}
		virtual ~jmxRGUILayout(){}

		virtual void run() {}
	};
	/*
	class GUITest:public jmxRGUILayout
	{
	public:
		GUITest(ID3D11Device* device,ID3D11DeviceContext* dc)
		{
			HR(D3DX11CreateShaderResourceViewFromFileA(device, "Ã“π»ªÊ¿Ôœ„.jpg", nullptr, nullptr,
				&srv, nullptr));

			HR(D3DX11GetImageInfoFromFileA("Ã“π»ªÊ¿Ôœ„.jpg", nullptr, &imageInfo, nullptr));

		}
		~GUITest()
		{
			ReleaseCom(srv);
		}

		virtual void run();

	private:
	//	bool show;
		float f = 0.f;
		ID3D11ShaderResourceView*		srv;
		D3DX11_IMAGE_INFO				imageInfo;

	};

	*/




}