#include"Editor\IMGUIDx11.h"
#include"JmxREX\JmxREX.h"
#include"Subdivide\HalfEdge.h"
using namespace jmxRCore;



extern LRESULT CALLBACK wndGUIProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

int main()
{
	jmxREX::jmxREXInit(800, 600, wndGUIProcHandler);
	jmxRGUIDx11::guiInit(jmxREX::wnd.screenWidth, jmxREX::wnd.screenHeight,
		jmxREX::wnd.wndHwnd, jmxREX::wnd.wndHinstance,
		jmxREX::hw.d3dDevice, jmxREX::hw.d3dImmediateContext,
		jmxREX::hw.renderTargetView, jmxREX::hw.depthStencilView,
		jmxREX::hw.viewPort);


	jmxRGUILayout* layout = new SubdivideLayout();
	//	std::cout << render.mTables[0]->samplesBuf.size() << std::endl;

	auto device = jmxREX::hw.d3dDevice;
	auto context = jmxREX::hw.d3dImmediateContext;
	auto rtv = jmxREX::hw.renderTargetView;
	auto dsv = jmxREX::hw.depthStencilView;
	auto sc = jmxREX::hw.swapChain;


	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			continue;
		}
		jmxREX::timer.tick();
		jmxRGUIDx11::guiNewFrame(jmxREX::timer.deltaTime());

		static float sliver[] = { 0.5f, 0.5f, 0.5f, 0.f };
		context->ClearRenderTargetView(rtv, sliver);


		jmxRGUIDx11::guiRender(layout);


		sc->Present(0, 0);
	}

	jmxRGUIDx11::guiShutDown();
	jmxREX::jmxREXShutDown();
}