#include"IMGUIDx11.h"
#include"../JmxRRT/DirectXDemo.h"
#include"../JmxREX/JmxREX.h"
#include <tchar.h>
#include<windowsx.h>
using namespace jmxRCore;
bool ld = false;
HWND							jmxRCore::jmxRGUIDx11::mWndHwnd				= nullptr;
HINSTANCE						jmxRCore::jmxRGUIDx11::mWndHinstance		= nullptr;
ID3D11Device*					jmxRCore::jmxRGUIDx11::md3dDevice			= nullptr;
ID3D11DeviceContext*			jmxRCore::jmxRGUIDx11::md3dImmediateContext = nullptr;
ID3D11RenderTargetView*			jmxRCore::jmxRGUIDx11::mRTV					= nullptr;
ID3D11DepthStencilView*			jmxRCore::jmxRGUIDx11::mDSV					= nullptr;
ID3D11VertexShader*				jmxRCore::jmxRGUIDx11::mGuiVs				= nullptr;
ID3D11PixelShader*				jmxRCore::jmxRGUIDx11::mGuiPs				= nullptr;
ID3D11Buffer*					jmxRCore::jmxRGUIDx11::mCBTrans				= nullptr;
ID3D11Buffer*					jmxRCore::jmxRGUIDx11::mVB					= nullptr;
u32								jmxRCore::jmxRGUIDx11::mVerticesCount		= 0;
ID3D11Buffer*					jmxRCore::jmxRGUIDx11::mIB					= nullptr;
u32								jmxRCore::jmxRGUIDx11::mIndicesCount		= 0;
ID3D11InputLayout*				jmxRCore::jmxRGUIDx11::mLayout				= nullptr;
ID3D11SamplerState*				jmxRCore::jmxRGUIDx11::mSam					= nullptr;
ID3D11ShaderResourceView*		jmxRCore::jmxRGUIDx11::mFontSRV				= nullptr;
ID3D11BlendState*				jmxRCore::jmxRGUIDx11::mBS					= nullptr;
ID3D11DepthStencilState*		jmxRCore::jmxRGUIDx11::mDSS					= nullptr;
ID3D11RasterizerState*			jmxRCore::jmxRGUIDx11::mRS					= nullptr;
D3D11_VIEWPORT					jmxRCore::jmxRGUIDx11::mViewPort;

u32								jmxRCore::jmxRGUIDx11::mWidth				= 0;
u32								jmxRCore::jmxRGUIDx11::mHeight				= 0;


extern s32  gMouseX;
extern s32  gMouseY;
extern s32  gLastMouseX;
extern s32  gLastMouseY;
extern bool gMouseDown;
extern bool gMouseMove;
extern HWND  gHwnd;

LRESULT CALLBACK wndGUIProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ImGuiIO& io = ImGui::GetIO();
	switch (msg)
	{
	case WM_LBUTTONDOWN:
		io.MouseDown[0] = true;
		ld = true;
		
		gLastMouseX = GET_X_LPARAM(lParam);
		gLastMouseY = GET_Y_LPARAM(lParam);
		SetCapture(hwnd);
		return true;
	case WM_LBUTTONUP:
		io.MouseDown[0] = false;
		ld = false;
		ReleaseCapture();
		return true;
	case WM_RBUTTONDOWN:
		io.MouseDown[1] = true;
		return true;
	case WM_RBUTTONUP:
		io.MouseDown[1] = false;
		return true;
	case WM_MBUTTONDOWN:
		io.MouseDown[2] = true;
		return true;
	case WM_MBUTTONUP:
		io.MouseDown[2] = false;
		return true;
	case WM_MOUSEWHEEL:
		io.MouseWheel += GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? +1.0f : -1.0f;
		return true;
	case WM_MOUSEMOVE:
		io.MousePos.x = (signed short)(lParam);
		io.MousePos.y = (signed short)(lParam >> 16);
//		std::cout << io.MousePos.x << "  " << io.MousePos.y << std::endl;
		if (((wParam & MK_LBUTTON) != 0) )
			gMouseMove = true;
		gMouseX = GET_X_LPARAM(lParam);
		gMouseY = GET_Y_LPARAM(lParam);
		return true;
	case WM_KEYDOWN:
		if (wParam < 256)
			io.KeysDown[wParam] = 1;
		return true;
	case WM_KEYUP:
		if (wParam < 256)
			io.KeysDown[wParam] = 0;
		return true;
	case WM_CHAR:
		// You can also use ToAscii()+GetKeyboardState() to retrieve characters.
		if (wParam > 0 && wParam < 0x10000)
			io.AddInputCharacter((unsigned short)wParam);
		return true;

	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);

}

void jmxRGUIDx11::guiInit(u32 w, u32 h, HWND hwnd, HINSTANCE hinstance,
	ID3D11Device* device, ID3D11DeviceContext* context,
	ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv,
	const D3D11_VIEWPORT& vp)
{
	mWidth = w;
	mHeight = h;
	mWndHwnd = hwnd;
	mWndHinstance = hinstance;
	md3dDevice = device;
	md3dImmediateContext = context;
	mRTV = rtv;
	mDSV = dsv;
	mViewPort = vp;

	initGUI();
	createFontsTexture();
	createShader();
	createBuf();
	createStates();

}


void jmxRGUIDx11::guiShutDown()
{
	ReleaseCom(mGuiVs);
	ReleaseCom(mGuiPs);
	ReleaseCom(mCBTrans);
	ReleaseCom(mVB);
	ReleaseCom(mIB);
	ReleaseCom(mLayout);
	ReleaseCom(mSam);
	ReleaseCom(mFontSRV);
	ReleaseCom(mBS);
	ReleaseCom(mDSS);
	ReleaseCom(mRS);
}
void jmxRGUIDx11::guiNewFrame(f32 deltaTime)
{
	ImGuiIO& io = ImGui::GetIO();
	static f32 totalTime = 0.f;
	static u32 frames = 0;
	RECT rect;
	GetClientRect(mWndHwnd, &rect);
	io.DisplaySize = ImVec2((float)(rect.right - rect.left), (float)(rect.bottom - rect.top));

	// Setup time step

	io.DeltaTime = deltaTime; 
	totalTime += deltaTime;
	frames++;
	if (totalTime > 1.f)
	{
		
		jmxREX::wnd.fps = (f32)frames / totalTime;
		totalTime = 0.f;
		frames = 0;
	}

	// Read keyboard modifiers inputs
	io.KeyCtrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
	io.KeyShift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
	io.KeyAlt = (GetKeyState(VK_MENU) & 0x8000) != 0;
	io.KeySuper = false;
	// io.KeysDown : filled by WM_KEYDOWN/WM_KEYUP events
	// io.MousePos : filled by WM_MOUSEMOVE events
	// io.MouseDown : filled by WM_*BUTTON* events
	// io.MouseWheel : filled by WM_MOUSEWHEEL events

	// Hide OS mouse cursor if ImGui is drawing it
	if (io.MouseDrawCursor)
		SetCursor(NULL);

	// Start the frame
	ImGui::NewFrame();
}

void jmxRGUIDx11::guiRender(jmxRGUILayout* guiLayout)
{
	




	guiLayout->run();




	ImGui::Render();

	

}

/*
void jmxREditor::initWin32()
{
	/*
	mHinstance = GetModuleHandle(NULL);

	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = wndProcHandler;//jmxRCore::WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = mHinstance;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"JmxREditor";

	if (!RegisterClass(&wc))
	{
		MessageBox(0, L"RegisterClass Failed.", 0, 0);

	}


	RECT R = { 0, 0, mWidth, mHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	mHwnd = CreateWindow(L"JmxREditor", L"FuckYou",
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, mHinstance, 0);
	if (!mHwnd)
	{
		MessageBox(0, L"CreateWindow Failed.", 0, 0);

	}
	
	mHinstance = GetModuleHandle(NULL);
	
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, wndProcHandler, 0L, 0L, mHinstance, NULL, LoadCursor(NULL, IDC_ARROW), NULL, NULL, _T("ImGui Example"), NULL };
	RegisterClassEx(&wc);
	RECT R = { 0, 0, mWidth, mHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	mHwnd = CreateWindow(L"ImGui Example", L"FuckYou",
		WS_OVERLAPPEDWINDOW, 0, 0, width, height, 0, 0, mHinstance, 0);


	ShowWindow(mHwnd, SW_SHOW);
	UpdateWindow(mHwnd);
}
*/
/*
void jmxREditor::initD3D()
{
	UINT createDeviceFlag = 0;
#if defined(DEBUG)|defined(_DEBUG)
	createDeviceFlag |= D3D11_CREATE_DEVICE_DEBUG;

#endif // defined(DEBUG)|

	D3D_FEATURE_LEVEL feature;
	HR(D3D11CreateDevice(
		NULL, D3D_DRIVER_TYPE_HARDWARE,
		NULL, createDeviceFlag,
		0, 0,
		D3D11_SDK_VERSION,
		&md3dDevice,
		&feature,
		&md3dImmediateContext));
	if (feature != D3D_FEATURE_LEVEL_11_0)
	{
		ErrorBox(L"错误，Dx11不被支持!");
		return;
	}


	DXGI_SWAP_CHAIN_DESC sd;

	sd.BufferDesc.Width = mWidth;
	sd.BufferDesc.Height = mHeight;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;


	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;

	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	sd.BufferCount = 1;
	sd.OutputWindow = mHwnd;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	//Create DXGISWAPCHAIN
	//first,we need do it by IDXGIFactory
	//
	IDXGIDevice* dxgiDevice = 0;
	HR(md3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice));

	IDXGIAdapter* dxgiAdapter = 0;
	HR(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter));

	IDXGIFactory* dxgiFactory = 0;
	HR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory));

	HR(dxgiFactory->CreateSwapChain(md3dDevice, &sd, &mSwapChain));

	ReleaseCom(dxgiDevice);
	ReleaseCom(dxgiAdapter);
	ReleaseCom(dxgiFactory);

	assert(md3dDevice);
	assert(md3dImmediateContext);
	assert(mSwapChain);

	ReleaseCom(mRenderTargetView);
	ReleaseCom(mDepthStencilView);
	ReleaseCom(mDepthStencilBuffer);
	//ReleaseCom(mBackBuffer);


	HR(mSwapChain->ResizeBuffers(
		1,
		mWidth,
		mHeight,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		NULL
		));
	//	ID3D11Texture2D* mBackBuffer;
	HR(mSwapChain->GetBuffer(0,
		__uuidof(ID3D11Texture2D),
		reinterpret_cast<void**>(&mBackBuffer)));

	HR(md3dDevice->CreateRenderTargetView(mBackBuffer,
		NULL,
		&mRenderTargetView));


	//	ReleaseCom(mBackBuffer);

	D3D11_TEXTURE2D_DESC td;
	td.Width = mWidth;
	td.Height = mHeight;
	td.MipLevels = 1;
	td.ArraySize = 1;
	td.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	td.SampleDesc.Count = 1;
	td.SampleDesc.Quality = 0;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	td.CPUAccessFlags = NULL;
	td.MiscFlags = NULL;

	HR(md3dDevice->CreateTexture2D(
		&td,
		NULL,
		&mDepthStencilBuffer));
	HR(md3dDevice->CreateDepthStencilView(
		mDepthStencilBuffer,
		NULL,
		&mDepthStencilView));

	md3dImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);

	mViewPort.TopLeftX = 0;
	mViewPort.TopLeftY = 0;
	mViewPort.Width = static_cast<float>(mWidth);
	mViewPort.Height = static_cast<float>(mHeight);
	mViewPort.MinDepth = 0.0f;
	mViewPort.MaxDepth = 1.0f;

	md3dImmediateContext->RSSetViewports(1, &mViewPort);



}
*/
void jmxRGUIDx11::initGUI()
{


	ImGuiIO& io = ImGui::GetIO();
	io.KeyMap[ImGuiKey_Tab] = VK_TAB;                       // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array that we will update during the application lifetime.
	io.KeyMap[ImGuiKey_LeftArrow] = VK_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = VK_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = VK_UP;
	io.KeyMap[ImGuiKey_DownArrow] = VK_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = VK_PRIOR;
	io.KeyMap[ImGuiKey_PageDown] = VK_NEXT;
	io.KeyMap[ImGuiKey_Home] = VK_HOME;
	io.KeyMap[ImGuiKey_End] = VK_END;
	io.KeyMap[ImGuiKey_Delete] = VK_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = VK_BACK;
	io.KeyMap[ImGuiKey_Enter] = VK_RETURN;
	io.KeyMap[ImGuiKey_Escape] = VK_ESCAPE;
	io.KeyMap[ImGuiKey_A] = 'A';
	io.KeyMap[ImGuiKey_C] = 'C';
	io.KeyMap[ImGuiKey_V] = 'V';
	io.KeyMap[ImGuiKey_X] = 'X';
	io.KeyMap[ImGuiKey_Y] = 'Y';
	io.KeyMap[ImGuiKey_Z] = 'Z';

	io.RenderDrawListsFn = std::bind(&jmxRGUIDx11::render,std::placeholders::_1);  // Alternatively you can set this to NULL and call ImGui::GetDrawData() after ImGui::Render() to get the same ImDrawData pointer.
	io.ImeWindowHandle = mWndHwnd;
}


void jmxRGUIDx11::createFontsTexture()
{
	ImGuiIO& io = ImGui::GetIO();
	u8* pixels = nullptr;
	s32 width, height;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

	// Upload texture to graphics system
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;

		ID3D11Texture2D *pTexture = NULL;
		D3D11_SUBRESOURCE_DATA subResource;
		subResource.pSysMem = pixels;
		subResource.SysMemPitch = desc.Width * 4;
		subResource.SysMemSlicePitch = 0;
		md3dDevice->CreateTexture2D(&desc, &subResource, &pTexture);

		// Create texture view
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(srvDesc));
		srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = desc.MipLevels;
		srvDesc.Texture2D.MostDetailedMip = 0;
		md3dDevice->CreateShaderResourceView(pTexture, &srvDesc, &mFontSRV);
		pTexture->Release();
	}

	// Store our identifier
	io.Fonts->TexID = (void *)mFontSRV;

	// Create texture sampler
	{
		D3D11_SAMPLER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.MipLODBias = 0.f;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		desc.MinLOD = -FLT_MAX;
		desc.MaxLOD = FLT_MAX;
		md3dDevice->CreateSamplerState(&desc, &mSam);
	}

}

void jmxRGUIDx11::createShader()
{
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, (size_t)(&((ImDrawVert*)0)->pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, (size_t)(&((ImDrawVert*)0)->uv), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, (size_t)(&((ImDrawVert*)0)->col), D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	mGuiVs = (ID3D11VertexShader*)createShaderAndLayout(L"HLSL/EditorVS.hlsl", nullptr, nullptr, "main", "vs_5_0",
		EVs, md3dDevice, layout, 3,&mLayout);

	mGuiPs = (ID3D11PixelShader*)createShaderAndLayout(L"HLSL/EditorPS.hlsl", nullptr, nullptr, "main", "ps_5_0",
		EPs, md3dDevice);
}

void jmxRGUIDx11::createBuf()
{
	mCBTrans = createConstantBuffer(PAD16(sizeof(XMFLOAT4X4)), md3dDevice);
}

void jmxRGUIDx11::createStates()
{
	{
		D3D11_BLEND_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.AlphaToCoverageEnable = false;
		desc.RenderTarget[0].BlendEnable = true;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		md3dDevice->CreateBlendState(&desc, &mBS);
	}

	// Create the rasterizer state
	{
		D3D11_RASTERIZER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_NONE;
		desc.ScissorEnable = true;
		desc.DepthClipEnable = true;
		md3dDevice->CreateRasterizerState(&desc, &mRS);
	}

	// Create depth-stencil State
	{
		D3D11_DEPTH_STENCIL_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.DepthEnable = false;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
		desc.StencilEnable = false;
		desc.FrontFace.StencilFailOp = desc.FrontFace.StencilDepthFailOp = desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		desc.BackFace = desc.FrontFace;
		md3dDevice->CreateDepthStencilState(&desc, &mDSS);
	}
}

void jmxRGUIDx11::render(ImDrawData* drawData)
{
	if (!mVB || mVerticesCount < drawData->TotalVtxCount)
	{
		if (mVB)
			ReleaseCom(mVB);

		mVerticesCount = drawData->TotalVtxCount + 5000;
		mVB = createBuffer(D3D11_USAGE_DYNAMIC,
			D3D11_BIND_VERTEX_BUFFER,
			mVerticesCount*sizeof(ImDrawVert),
			0, D3D11_CPU_ACCESS_WRITE, 0, nullptr, md3dDevice);
	}

	if (!mIB || mIndicesCount < drawData->TotalVtxCount)
	{
		if (mIB)
			ReleaseCom(mIB);

		mIndicesCount = drawData->TotalVtxCount + 10000;
	
		mIB = createBuffer(D3D11_USAGE_DYNAMIC,
			D3D11_BIND_INDEX_BUFFER,
			mIndicesCount*sizeof(ImDrawIdx),
			0, D3D11_CPU_ACCESS_WRITE, 0, nullptr, md3dDevice);
	}

	D3D11_MAPPED_SUBRESOURCE vtx_resource, idx_resource;
	if (md3dImmediateContext->Map(mVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &vtx_resource) != S_OK)
		return;
	if (md3dImmediateContext->Map(mIB, 0, D3D11_MAP_WRITE_DISCARD, 0, &idx_resource) != S_OK)
		return;
	ImDrawVert* vtx_dst = (ImDrawVert*)vtx_resource.pData;
	ImDrawIdx* idx_dst = (ImDrawIdx*)idx_resource.pData;
	for (int n = 0; n < drawData->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = drawData->CmdLists[n];
		memcpy(vtx_dst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
		memcpy(idx_dst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
		vtx_dst += cmd_list->VtxBuffer.Size;
		idx_dst += cmd_list->IdxBuffer.Size;
	}
	md3dImmediateContext->Unmap(mVB, 0);
	md3dImmediateContext->Unmap(mIB, 0);

	// Setup orthographic projection matrix into our constant buffer
	{
		D3D11_MAPPED_SUBRESOURCE mapped_resource;
		if (md3dImmediateContext->Map(mCBTrans, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource) != S_OK)
			return;
		XMFLOAT4X4* constant_buffer = (XMFLOAT4X4*)mapped_resource.pData;
		float L = 0.0f;
		float R = ImGui::GetIO().DisplaySize.x;
		float B = ImGui::GetIO().DisplaySize.y;
		float T = 0.0f;
		XMMATRIX mvp = XMMATRIX(2.0f / (R - L), 0.0f, 0.0f, 0.0f,
			0.0f, 2.0f / (T - B), 0.0f, 0.0f,
			0.0f, 0.0f, 0.5f, 0.0f,
			(R + L) / (L - R), (T + B) / (B - T), 0.5f, 1.0f);
		
		XMStoreFloat4x4(constant_buffer, XMMatrixTranspose(mvp));
		md3dImmediateContext->Unmap(mCBTrans, 0);
	}

	// Backup DX state that will be modified to restore it afterwards (unfortunately this is very ugly looking and verbose. Close your eyes!)
	struct BACKUP_DX11_STATE
	{
		UINT                        ScissorRectsCount, ViewportsCount;
		D3D11_RECT                  ScissorRects[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
		D3D11_VIEWPORT              Viewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
		ID3D11RasterizerState*      RS;
		ID3D11BlendState*           BlendState;
		FLOAT                       BlendFactor[4];
		UINT                        SampleMask;
		UINT                        StencilRef;
		ID3D11DepthStencilState*    DepthStencilState;
		ID3D11ShaderResourceView*   PSShaderResource;
		ID3D11SamplerState*         PSSampler;
		ID3D11PixelShader*          PS;
		ID3D11VertexShader*         VS;
		UINT                        PSInstancesCount, VSInstancesCount;
		ID3D11ClassInstance*        PSInstances[256], *VSInstances[256];   // 256 is max according to PSSetShader documentation
		D3D11_PRIMITIVE_TOPOLOGY    PrimitiveTopology;
		ID3D11Buffer*               IndexBuffer, *VertexBuffer, *VSConstantBuffer;
		UINT                        IndexBufferOffset, VertexBufferStride, VertexBufferOffset;
		DXGI_FORMAT                 IndexBufferFormat;
		ID3D11InputLayout*          InputLayout;
	};
	BACKUP_DX11_STATE old;
	old.ScissorRectsCount = old.ViewportsCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
	md3dImmediateContext->RSGetScissorRects(&old.ScissorRectsCount, old.ScissorRects);
	md3dImmediateContext->RSGetViewports(&old.ViewportsCount, old.Viewports);
	md3dImmediateContext->RSGetState(&old.RS);
	md3dImmediateContext->OMGetBlendState(&old.BlendState, old.BlendFactor, &old.SampleMask);
	md3dImmediateContext->OMGetDepthStencilState(&old.DepthStencilState, &old.StencilRef);
	md3dImmediateContext->PSGetShaderResources(0, 1, &old.PSShaderResource);
	md3dImmediateContext->PSGetSamplers(0, 1, &old.PSSampler);
	old.PSInstancesCount = old.VSInstancesCount = 256;
	md3dImmediateContext->PSGetShader(&old.PS, old.PSInstances, &old.PSInstancesCount);
	md3dImmediateContext->VSGetShader(&old.VS, old.VSInstances, &old.VSInstancesCount);
	md3dImmediateContext->VSGetConstantBuffers(0, 1, &old.VSConstantBuffer);
	md3dImmediateContext->IAGetPrimitiveTopology(&old.PrimitiveTopology);
	md3dImmediateContext->IAGetIndexBuffer(&old.IndexBuffer, &old.IndexBufferFormat, &old.IndexBufferOffset);
	md3dImmediateContext->IAGetVertexBuffers(0, 1, &old.VertexBuffer, &old.VertexBufferStride, &old.VertexBufferOffset);
	md3dImmediateContext->IAGetInputLayout(&old.InputLayout);

	// Setup viewport
	D3D11_VIEWPORT vp;
	memset(&vp, 0, sizeof(D3D11_VIEWPORT));
	vp.Width = ImGui::GetIO().DisplaySize.x;
	vp.Height = ImGui::GetIO().DisplaySize.y;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = vp.TopLeftY = 0.0f;
	md3dImmediateContext->RSSetViewports(1, &vp);

	// Bind shader and vertex buffers
	unsigned int stride = sizeof(ImDrawVert);
	unsigned int offset = 0;
	md3dImmediateContext->IASetInputLayout(mLayout);
	md3dImmediateContext->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
	md3dImmediateContext->IASetIndexBuffer(mIB, sizeof(ImDrawIdx) == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	md3dImmediateContext->VSSetShader(mGuiVs, NULL, 0);
	md3dImmediateContext->VSSetConstantBuffers(0, 1, &mCBTrans);
	md3dImmediateContext->PSSetShader(mGuiPs, NULL, 0);
	md3dImmediateContext->PSSetSamplers(0, 1, &mSam);

	// Setup render state
	const float blend_factor[4] = { 0.f, 0.f, 0.f, 0.f };
	md3dImmediateContext->OMSetBlendState(mBS, blend_factor, 0xffffffff);
	md3dImmediateContext->OMSetDepthStencilState(mDSS, 0);
	md3dImmediateContext->RSSetState(mRS);

	// Render command lists
	int vtx_offset = 0;
	int idx_offset = 0;
	for (int n = 0; n < drawData->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = drawData->CmdLists[n];
		for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
		{
			const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
			if (pcmd->UserCallback)
			{
				pcmd->UserCallback(cmd_list, pcmd);
			}
			else
			{
				const D3D11_RECT r = { (LONG)pcmd->ClipRect.x, (LONG)pcmd->ClipRect.y, (LONG)pcmd->ClipRect.z, (LONG)pcmd->ClipRect.w };
				md3dImmediateContext->PSSetShaderResources(0, 1, (ID3D11ShaderResourceView**)&pcmd->TextureId);
				md3dImmediateContext->RSSetScissorRects(1, &r);
				md3dImmediateContext->DrawIndexed(pcmd->ElemCount, idx_offset, vtx_offset);
			}
			idx_offset += pcmd->ElemCount;
		}
		vtx_offset += cmd_list->VtxBuffer.Size;
	}

	// Restore modified DX state
	md3dImmediateContext->RSSetScissorRects(old.ScissorRectsCount, old.ScissorRects);
	md3dImmediateContext->RSSetViewports(old.ViewportsCount, old.Viewports);
	md3dImmediateContext->RSSetState(old.RS); if (old.RS) old.RS->Release();
	md3dImmediateContext->OMSetBlendState(old.BlendState, old.BlendFactor, old.SampleMask); if (old.BlendState) old.BlendState->Release();
	md3dImmediateContext->OMSetDepthStencilState(old.DepthStencilState, old.StencilRef); if (old.DepthStencilState) old.DepthStencilState->Release();
	md3dImmediateContext->PSSetShaderResources(0, 1, &old.PSShaderResource); if (old.PSShaderResource) old.PSShaderResource->Release();
	md3dImmediateContext->PSSetSamplers(0, 1, &old.PSSampler); if (old.PSSampler) old.PSSampler->Release();
	md3dImmediateContext->PSSetShader(old.PS, old.PSInstances, old.PSInstancesCount); if (old.PS) old.PS->Release();
	for (UINT i = 0; i < old.PSInstancesCount; i++) if (old.PSInstances[i]) old.PSInstances[i]->Release();
	md3dImmediateContext->VSSetShader(old.VS, old.VSInstances, old.VSInstancesCount); if (old.VS) old.VS->Release();
	md3dImmediateContext->VSSetConstantBuffers(0, 1, &old.VSConstantBuffer); if (old.VSConstantBuffer) old.VSConstantBuffer->Release();
	for (UINT i = 0; i < old.VSInstancesCount; i++) if (old.VSInstances[i]) old.VSInstances[i]->Release();
	md3dImmediateContext->IASetPrimitiveTopology(old.PrimitiveTopology);
	md3dImmediateContext->IASetIndexBuffer(old.IndexBuffer, old.IndexBufferFormat, old.IndexBufferOffset); if (old.IndexBuffer) old.IndexBuffer->Release();
	md3dImmediateContext->IASetVertexBuffers(0, 1, &old.VertexBuffer, &old.VertexBufferStride, &old.VertexBufferOffset); if (old.VertexBuffer) old.VertexBuffer->Release();
	md3dImmediateContext->IASetInputLayout(old.InputLayout); if (old.InputLayout) old.InputLayout->Release();
}

/*
void GUITest::run()
{
	ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiSetCond_FirstUseEver);
	ImGui::Begin("Fuck", nullptr, ImGuiWindowFlags_MenuBar);
	ImGui::PushItemWidth(-140);
	ImGui::Text("Dear ImGui says hello.");
	ImGui::BulletText("BulletText");

	if (ImGui::CollapsingHeader("JMX"))
	{
		if (ImGui::TreeNode("Render"))
		{
			static bool rb = false;
			if (ImGui::RadioButton("DI", rb)) rb ^= 1;
			if (ImGui::RadioButton("PT", !rb)) rb ^= 1;
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Picture"))
		{
			ImGui::Image(ImTextureID(srv), ImVec2(imageInfo.Width, imageInfo.Height));
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImVec2 relativeMouse = ImGui::GetCursorPos();
				ImVec2 mouse = ImGui::GetCursorScreenPos();

				ImGui::Text("Relative: x= %f  y= %f", relativeMouse.x, relativeMouse.y);
				ImGui::Text("Absolute: x= %f  y= %f", mouse.x, mouse.y);


				static float sinData[] = { std::sinf(0), std::sinf(PI / 8),
					std::sinf(PI / 6), std::sinf(PI / 4), std::sinf(PI / 2) };
				ImGui::PlotLines("Sin", sinData, 5);

				ImGui::EndTooltip();
			}

			ImGui::TreePop();

		}

		ImGui::BeginChild("Child", ImVec2(0, 200));

		ImGui::Columns(2);
		static bool sel[10] = { false };
		for (u32 i = 0; i < 10; ++i)
		{
			if (i == 5)
				ImGui::NextColumn();

			ImGui::PushID(i);
			ImGui::Selectable("Select", &sel[i]);

			ImGui::PopID();
		}
		ImGui::Columns();

		ImGui::EndChild();


	}

	ImGui::End();
//	ImGui::Button("BUTTON");
}
*/