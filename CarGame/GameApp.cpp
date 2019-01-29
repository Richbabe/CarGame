#include "GameApp.h"
#include "d3dUtil.h"
using namespace DirectX;
using namespace std::experimental;

GameApp::GameApp(HINSTANCE hInstance)
	: D3DApp(hInstance)
{
}

GameApp::~GameApp()
{
}

bool GameApp::Init()
{
	if (!D3DApp::Init())
		return false;

	// ����ȳ�ʼ��������Ⱦ״̬���Թ��������Чʹ��
	RenderStates::InitAll(md3dDevice);

	if (!mBasicEffect.InitAll(md3dDevice))
		return false;

	if (!InitResource())
		return false;

	// ��ʼ����꣬���̲���Ҫ
	mMouse->SetWindow(mhMainWnd);
	mMouse->SetMode(DirectX::Mouse::MODE_RELATIVE);

	return true;
}

void GameApp::OnResize()
{
	assert(md2dFactory);
	assert(mdwriteFactory);
	// �ͷ�D2D�������Դ
	mColorBrush.Reset();
	md2dRenderTarget.Reset();

	D3DApp::OnResize();

	// ΪD2D����DXGI������ȾĿ��
	ComPtr<IDXGISurface> surface;
	HR(mSwapChain->GetBuffer(0, __uuidof(IDXGISurface), reinterpret_cast<void**>(surface.GetAddressOf())));
	D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
		D2D1_RENDER_TARGET_TYPE_DEFAULT,
		D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED));
	HRESULT hr = md2dFactory->CreateDxgiSurfaceRenderTarget(surface.Get(), &props, md2dRenderTarget.GetAddressOf());
	surface.Reset();

	if (hr == E_NOINTERFACE)
	{
		OutputDebugString(L"\n���棺Direct2D��Direct3D�������Թ������ޣ��㽫�޷������ı���Ϣ�����ṩ������ѡ������\n"
			"1. ����Win7ϵͳ����Ҫ������Win7 SP1������װKB2670838������֧��Direct2D��ʾ��\n"
			"2. �������Direct3D 10.1��Direct2D�Ľ�����������ģ�"
			"https://docs.microsoft.com/zh-cn/windows/desktop/Direct2D/direct2d-and-direct3d-interoperation-overview""\n"
			"3. ʹ�ñ������⣬����FreeType��\n\n");
	}
	else if (hr == S_OK)
	{
		// �����̶���ɫˢ���ı���ʽ
		HR(md2dRenderTarget->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::White),
			mColorBrush.GetAddressOf()));
		HR(mdwriteFactory->CreateTextFormat(L"����", nullptr, DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 15, L"zh-cn",
			mTextFormat.GetAddressOf()));
	}
	else
	{
		// �����쳣����
		assert(md2dRenderTarget);
	}

	// ����������ʾ
	if (mCamera != nullptr)
	{
		mCamera->SetFrustum(XM_PI / 3, AspectRatio(), 0.5f, 1000.0f);
		mCamera->SetViewPort(0.0f, 0.0f, (float)mClientWidth, (float)mClientHeight);
		mBasicEffect.SetProjMatrix(mCamera->GetProjMatrix());
	}
}

void GameApp::UpdateScene(float dt)
{
	static float phi = 0.0f, theta = 0.0f;
	// ��������¼�����ȡ���ƫ����
	Mouse::State mouseState = mMouse->GetState();
	Mouse::State lastMouseState = mMouseTracker.GetLastState();
	mMouseTracker.Update(mouseState);

	Keyboard::State keyState = mKeyboard->GetState();
	mKeyboardTracker.Update(keyState);

	// ���������˶��߼�
	if (keyState.IsKeyDown(Keyboard::W))
	{
		mCar.SetCarMoveForward();
		mCar.CarMove(dt * 3.0f);
	}

	if (keyState.IsKeyDown(Keyboard::S))
	{
		mCar.SetCarMoveBack();
		mCar.CarMove(dt * -3.0f);

	}
	if (!keyState.IsKeyDown(Keyboard::W) && !keyState.IsKeyDown(Keyboard::S)) {
		mCar.SetCarStatic();
	}
	if (keyState.IsKeyDown(Keyboard::A)) {
		float d = -dt * 0.3f;
		mCar.CarTurn(theta, d, mCar.GetCarState());
	}
	if (keyState.IsKeyDown(Keyboard::D)) {
		float d = dt * 0.3f;
		mCar.CarTurn(theta, d, mCar.GetCarState());
	}
	
	// ����������˶��߼�
	auto firstPersonCamera = std::dynamic_pointer_cast<FirstPersonCamera>(mCamera);
	auto thirdPersonCamera = std::dynamic_pointer_cast<ThirdPersonCamera>(mCamera);
	if (mCameraMode == CameraMode::FirstPerson) {
		firstPersonCamera->LookTo(mCar.GetCarPosition(), mCar.GetCarDirection(), XMFLOAT3(0.0f, 1.0f, 0.0f));
	}
	else {
		// ******************
		// �����˳�������Ĳ���
		//
		thirdPersonCamera->SetTarget(mCar.GetCarPosition());

		// ��������ת
		thirdPersonCamera->RotateX(mouseState.y * dt * 1.25f);
		thirdPersonCamera->RotateY(mouseState.x * dt * 1.25f);
		thirdPersonCamera->Approach(-mouseState.scrollWheelValue / 120 * 1.0f);
	}
	mCamera->UpdateViewMatrix();

	mBasicEffect.SetViewMatrix(mCamera->GetViewMatrix());
	mBasicEffect.SetEyePos(mCamera->GetPositionVector());
	
	// �˳���������Ӧ�򴰿ڷ���������Ϣ
	if (mKeyboardTracker.IsKeyPressed(Keyboard::Escape))
		SendMessage(MainWnd(), WM_DESTROY, 0, 0);

}

void GameApp::DrawScene()
{
	assert(md3dImmediateContext);
	assert(mSwapChain);

	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView.Get(), reinterpret_cast<const float*>(&Colors::Black));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);



	// *********************
	// 1. �����淴������д��ֵ1��ģ�建����
	// 

	mBasicEffect.SetWriteStencilOnly(md3dImmediateContext, 1);
	mMirror.Draw(md3dImmediateContext, mBasicEffect);

	// ***********************
	// 2. ���Ʋ�͸���ķ�������
	//

	// �����������
	mBasicEffect.SetReflectionState(true);
	mBasicEffect.SetRenderDefaultWithStencil(md3dImmediateContext, 1);

	mWalls[2].Draw(md3dImmediateContext, mBasicEffect);
	mWalls[3].Draw(md3dImmediateContext, mBasicEffect);
	mWalls[4].Draw(md3dImmediateContext, mBasicEffect);
	mFloor.Draw(md3dImmediateContext, mBasicEffect);
	mWoodCrate.Draw(md3dImmediateContext, mBasicEffect);
	mCar.Draw(md3dImmediateContext, mBasicEffect);

	// ***********************
	// 3. ���Ʋ�͸�������������Ӱ
	//

	mWoodCrate.SetMaterial(mShadowMat);
	mCar.SetCarMaterial(mShadowMat);
	mBasicEffect.SetShadowState(true);	// ���俪������Ӱ����			
	mBasicEffect.SetRenderNoDoubleBlend(md3dImmediateContext, 1);

	mWoodCrate.Draw(md3dImmediateContext, mBasicEffect);
	mCar.Draw(md3dImmediateContext, mBasicEffect);

	// �ָ���ԭ����״̬
	mBasicEffect.SetShadowState(false);
	mWoodCrate.SetMaterial(mWoodCrateMat);
	mCar.SetCarMaterial(mWoodCrateMat);

	// ***********************
	// 4. ����͸������
	//

	// �رշ������
	mBasicEffect.SetReflectionState(false);
	mBasicEffect.SetRenderAlphaBlendWithStencil(md3dImmediateContext, 1);

	mMirror.Draw(md3dImmediateContext, mBasicEffect);

	// ************************
	// 5. ���Ʋ�͸������������
	//
	mBasicEffect.SetRenderDefault(md3dImmediateContext);

	for (auto& wall : mWalls)
		wall.Draw(md3dImmediateContext, mBasicEffect);
	mFloor.Draw(md3dImmediateContext, mBasicEffect);
	mWoodCrate.Draw(md3dImmediateContext, mBasicEffect);
	mCar.Draw(md3dImmediateContext, mBasicEffect);

	// ************************
	// 6. ���Ʋ�͸�������������Ӱ
	//
	mWoodCrate.SetMaterial(mShadowMat);
	mCar.SetCarMaterial(mShadowMat);
	mBasicEffect.SetShadowState(true);	// ����رգ���Ӱ����
	mBasicEffect.SetRenderNoDoubleBlend(md3dImmediateContext, 0);

	mWoodCrate.Draw(md3dImmediateContext, mBasicEffect);
	mCar.Draw(md3dImmediateContext, mBasicEffect);

	mBasicEffect.SetShadowState(false);		// ��Ӱ�ر�
	mWoodCrate.SetMaterial(mWoodCrateMat);
	mCar.SetCarMaterial(mWoodCrateMat);

	// ******************
	// ����Direct2D����
	//
	if (md2dRenderTarget != nullptr)
	{
		md2dRenderTarget->BeginDraw();
		std::wstring text = L"�л������ģʽ: 1-��һ�˳� 2-�����˳� 3-�����ӽ�\n"
			"W/S/A/D ǰ��/����/��ƽ��/��ƽ�� (�����˳���Ч)  Esc�˳�\n"
			"����ƶ�������Ұ ���ֿ��Ƶ����˳ƹ۲����\n"
			"��ǰģʽ: ";
		if (mCameraMode == CameraMode::FirstPerson)
			text += L"��һ�˳�(���������ƶ�)";
		else if (mCameraMode == CameraMode::ThirdPerson)
			text += L"�����˳�";
		else
			text += L"�����ӽ�";
		md2dRenderTarget->DrawTextW(text.c_str(), (UINT32)text.length(), mTextFormat.Get(),
			D2D1_RECT_F{ 0.0f, 0.0f, 600.0f, 200.0f }, mColorBrush.Get());
		HR(md2dRenderTarget->EndDraw());
	}

	HR(mSwapChain->Present(0, 0));
}



bool GameApp::InitResource()
{
	// ******************
	// ��ʼ����Ϸ����
	ComPtr<ID3D11ShaderResourceView> texture;
	Material material;
	material.Ambient = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	material.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	material.Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 16.0f);

	mWoodCrateMat = material;
	mShadowMat.Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mShadowMat.Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.5f);
	mShadowMat.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 16.0f);

	// ��ʼ��ľ��
	HR(CreateDDSTextureFromFile(md3dDevice.Get(), L"Texture\\WoodCrate.dds", nullptr, texture.GetAddressOf()));
	mWoodCrate.SetBuffer(md3dDevice, Geometry::CreateBox());
	// ̧��߶ȱ�����Ȼ�������Դ����
	mWoodCrate.SetWorldMatrix(XMMatrixTranslation(0.0f, 0.01f, 5.0f));
	mWoodCrate.SetTexture(texture);
	mWoodCrate.SetMaterial(material);

	// ��ʼ������
	mCar.InitCar(md3dDevice);

	// ��ʼ���ذ�
	HR(CreateDDSTextureFromFile(md3dDevice.Get(), L"Texture\\floor.dds", nullptr, texture.ReleaseAndGetAddressOf()));
	mFloor.SetBuffer(md3dDevice,
		Geometry::CreatePlane(XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(20.0f, 20.0f), XMFLOAT2(5.0f, 5.0f)));
	mFloor.SetTexture(texture);
	mFloor.SetMaterial(material);

	// ��ʼ��ǽ��
	mWalls.resize(5);
	HR(CreateDDSTextureFromFile(md3dDevice.Get(), L"Texture\\brick.dds", nullptr, texture.ReleaseAndGetAddressOf()));
	// �������ǽ�����������ɣ�0��1���м�λ�����ڷ��þ���
	//     ____     ____
	//    /| 0 |   | 1 |\
	//   /4|___|___|___|2\
	//  /_/_ _ _ _ _ _ _\_\
	// | /       3       \ |
	// |/_________________\|
	//
	for (int i = 0; i < 5; ++i)
	{
		mWalls[i].SetMaterial(material);
		mWalls[i].SetTexture(texture);
	}
	mWalls[0].SetBuffer(md3dDevice, Geometry::CreatePlane(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(6.0f, 8.0f), XMFLOAT2(1.5f, 2.0f)));
	mWalls[1].SetBuffer(md3dDevice, Geometry::CreatePlane(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(6.0f, 8.0f), XMFLOAT2(1.5f, 2.0f)));
	mWalls[2].SetBuffer(md3dDevice, Geometry::CreatePlane(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(20.0f, 8.0f), XMFLOAT2(5.0f, 2.0f)));
	mWalls[3].SetBuffer(md3dDevice, Geometry::CreatePlane(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(20.0f, 8.0f), XMFLOAT2(5.0f, 2.0f)));
	mWalls[4].SetBuffer(md3dDevice, Geometry::CreatePlane(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(20.0f, 8.0f), XMFLOAT2(5.0f, 2.0f)));

	mWalls[0].SetWorldMatrix(XMMatrixRotationX(-XM_PIDIV2) * XMMatrixTranslation(-7.0f, 3.0f, 10.0f));
	mWalls[1].SetWorldMatrix(XMMatrixRotationX(-XM_PIDIV2) * XMMatrixTranslation(7.0f, 3.0f, 10.0f));
	mWalls[2].SetWorldMatrix(XMMatrixRotationY(-XM_PIDIV2) * XMMatrixRotationZ(XM_PIDIV2) * XMMatrixTranslation(10.0f, 3.0f, 0.0f));
	mWalls[3].SetWorldMatrix(XMMatrixRotationX(XM_PIDIV2) * XMMatrixTranslation(0.0f, 3.0f, -10.0f));
	mWalls[4].SetWorldMatrix(XMMatrixRotationY(XM_PIDIV2) * XMMatrixRotationZ(-XM_PIDIV2) * XMMatrixTranslation(-10.0f, 3.0f, 0.0f));

	// ��ʼ������
	material.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);
	material.Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 16.0f);
	HR(CreateDDSTextureFromFile(md3dDevice.Get(), L"Texture\\ice.dds", nullptr, texture.ReleaseAndGetAddressOf()));
	mMirror.SetBuffer(md3dDevice,
		Geometry::CreatePlane(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(8.0f, 8.0f), XMFLOAT2(1.0f, 1.0f)));
	mMirror.SetWorldMatrix(XMMatrixRotationX(-XM_PIDIV2) * XMMatrixTranslation(0.0f, 3.0f, 10.0f));
	mMirror.SetTexture(texture);
	mMirror.SetMaterial(material);

	// ******************
	// ��ʼ�������
	//
	/*
	// ��ʼ��Ϊ��һ�˳������
	mCameraMode = CameraMode::FirstPerson;
	auto camera = std::shared_ptr<FirstPersonCamera>(new FirstPersonCamera);
	mCamera = camera;
	camera->SetViewPort(0.0f, 0.0f, (float)mClientWidth, (float)mClientHeight);
	camera->LookAt(mCar.GetCarPosition(), mCar.GetCarDirection(), XMFLOAT3(0.0f, 1.0f, 0.0f));
	mCamera->UpdateViewMatrix();
	*/
	
	// ��ʼ��Ϊ�����˳������
	mCameraMode = CameraMode::ThirdPerson;
	auto camera = std::shared_ptr<ThirdPersonCamera>(new ThirdPersonCamera);
	mCamera = camera;
	camera->SetViewPort(0.0f, 0.0f, (float)mClientWidth, (float)mClientHeight);
	camera->SetTarget(mCar.GetCarPosition());
	camera->SetDistance(5.0f);
	camera->SetDistanceMinMax(2.0f, 14.0f);
	
	
	mBasicEffect.SetViewMatrix(mCamera->GetViewMatrix());
	mBasicEffect.SetEyePos(mCamera->GetPositionVector());

	mCamera->SetFrustum(XM_PI / 3, AspectRatio(), 0.5f, 1000.0f);
	mBasicEffect.SetProjMatrix(mCamera->GetProjMatrix());
	
	// ******************
	// ��ʼ������仯��ֵ
	//

	mBasicEffect.SetReflectionMatrix(XMMatrixReflect(XMVectorSet(0.0f, 0.0f, -1.0f, 10.0f)));
	// ��΢��һ��λ������ʾ��Ӱ
	mBasicEffect.SetShadowMatrix(XMMatrixShadow(XMVectorSet(0.0f, 1.0f, 0.0f, 0.99f), XMVectorSet(0.0f, 10.0f, -10.0f, 1.0f)));
	mBasicEffect.SetRefShadowMatrix(XMMatrixShadow(XMVectorSet(0.0f, 1.0f, 0.0f, 0.99f), XMVectorSet(0.0f, 10.0f, 30.0f, 1.0f)));

	// ������
	DirectionalLight dirLight;
	dirLight.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	dirLight.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	dirLight.Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	dirLight.Direction = XMFLOAT3(0.0f, -1.0f, 0.0f);
	mBasicEffect.SetDirLight(0, dirLight);
	// �ƹ�
	PointLight pointLight;
	pointLight.Position = XMFLOAT3(0.0f, 10.0f, -10.0f);
	pointLight.Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	pointLight.Diffuse = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	pointLight.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	pointLight.Att = XMFLOAT3(0.0f, 0.1f, 0.0f);
	pointLight.Range = 25.0f;
	mBasicEffect.SetPointLight(0, pointLight);

	return true;
}



