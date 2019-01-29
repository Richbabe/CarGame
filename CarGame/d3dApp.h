#ifndef D3DAPP_H
#define D3DAPP_H

#include <wrl/client.h>
#include <string>
#include <d2d1.h>
#include <dwrite.h>
#include <d3d11_1.h>
#include <DirectXMath.h>
#include "Mouse.h"		// Mouse.h��Keyboard.h��ҪԤ�Ȱ���Windows.h
#include "Keyboard.h"
#include "GameTimer.h"

// �������Ҫ���õĿ�
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "winmm.lib")

class D3DApp
{
public:
	D3DApp(HINSTANCE hInstance);    // �ڹ��캯���ĳ�ʼ���б�Ӧ�����úó�ʼ����
	virtual ~D3DApp();

	HINSTANCE AppInst()const;       // ��ȡӦ��ʵ���ľ��
	HWND      MainWnd()const;       // ��ȡ�����ھ��
	float     AspectRatio()const;   // ��ȡ��Ļ��߱�

	int Run();                      // ���г���ִ����Ϣ�¼���ѭ��

									// ��ܷ������ͻ���������Ҫ������Щ������ʵ���ض���Ӧ������
	virtual bool Init();            // �ø��෽����Ҫ��ʼ�����ڡ�Direct2D��Direct3D����
	virtual void OnResize();        // �ø��෽����Ҫ�ڴ��ڴ�С�䶯��ʱ�����
	virtual void UpdateScene(float dt) = 0;   // ������Ҫʵ�ָ÷��������ÿһ֡�ĸ���
	virtual void DrawScene() = 0;             // ������Ҫʵ�ָ÷��������ÿһ֡�Ļ���
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	// ���ڵ���Ϣ�ص�����
protected:
	bool InitMainWindow();      // ���ڳ�ʼ��
	bool InitDirect2D();		// Direct2D��ʼ��
	bool InitDirect3D();        // Direct3D��ʼ��


	void CalculateFrameStats(); // ����ÿ��֡�����ڴ�����ʾ

protected:

	HINSTANCE mhAppInst;        // Ӧ��ʵ�����
	HWND      mhMainWnd;        // �����ھ��
	bool      mAppPaused;       // Ӧ���Ƿ���ͣ
	bool      mMinimized;       // Ӧ���Ƿ���С��
	bool      mMaximized;       // Ӧ���Ƿ����
	bool      mResizing;        // ���ڴ�С�Ƿ�仯
	bool	  mEnable4xMsaa;	// �Ƿ���4�����ز���
	UINT      m4xMsaaQuality;   // MSAA֧�ֵ������ȼ�

	GameTimer mTimer;           // ��ʱ��


	// ʹ��ģ�����(C++11)��������
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
	// Direct2D
	ComPtr<ID2D1Factory> md2dFactory;							// D2D����
	ComPtr<ID2D1RenderTarget> md2dRenderTarget;					// D2D��ȾĿ��
	ComPtr<IDWriteFactory> mdwriteFactory;						// DWrite����
	// Direct3D 11
	ComPtr<ID3D11Device> md3dDevice;							// D3D11�豸
	ComPtr<ID3D11DeviceContext> md3dImmediateContext;			// D3D11�豸������
	ComPtr<IDXGISwapChain> mSwapChain;							// D3D11������
	// Direct3D 11.1
	ComPtr<ID3D11Device1> md3dDevice1;							// D3D11.1�豸
	ComPtr<ID3D11DeviceContext1> md3dImmediateContext1;			// D3D11.1�豸������
	ComPtr<IDXGISwapChain1> mSwapChain1;						// D3D11.1������
	// ������Դ
	ComPtr<ID3D11Texture2D> mDepthStencilBuffer;				// ���ģ�建����
	ComPtr<ID3D11RenderTargetView> mRenderTargetView;			// ��ȾĿ����ͼ
	ComPtr<ID3D11DepthStencilView> mDepthStencilView;			// ���ģ����ͼ
	D3D11_VIEWPORT mScreenViewport;                             // �ӿ�
	// ��������
	std::unique_ptr<DirectX::Mouse> mMouse;						// ���
	DirectX::Mouse::ButtonStateTracker mMouseTracker;			// ���״̬׷����
	std::unique_ptr<DirectX::Keyboard> mKeyboard;				// ����
	DirectX::Keyboard::KeyboardStateTracker mKeyboardTracker;	// ����״̬׷����
	// ������Ӧ���ڹ��캯�����ú���Щ�Զ���ĳ�ʼ����
	std::wstring mMainWndCaption;                               // �����ڱ���
	int mClientWidth;                                           // �ӿڿ��
	int mClientHeight;                                          // �ӿڸ߶�
};

#endif // D3DAPP_H