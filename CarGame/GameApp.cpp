#include "GameApp.h"
#include "d3dUtil.h"
using namespace DirectX;
using namespace std::experimental;
#include "iostream"
#pragma warning(disable:4996)
using namespace std;

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

	if (!mSkyEffect.InitAll(md3dDevice))
		return false;

	if (!mBoundingBoxEffect.InitAll(md3dDevice))
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

void GameApp::InitMaterial() {
	// ��ʼ����ͨ����
	mNormalMeterialMat.Ambient = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	mNormalMeterialMat.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	mNormalMeterialMat.Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 16.0f);

	// ��ʼ����Ӱ����
	mShadowMat.Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mShadowMat.Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.5f);
	mShadowMat.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 16.0f);
}

void GameApp::InitFirstPersonCamera() {
	auto firstPersonCamera = std::dynamic_pointer_cast<FirstPersonCamera>(mCamera);
	firstPersonCamera.reset(new FirstPersonCamera);
	mCamera = firstPersonCamera;
	firstPersonCamera->SetViewPort(0.0f, 0.0f, (float)mClientWidth, (float)mClientHeight);

	XMFLOAT3 cameraPos = mCar.GetCarPosition();
	cameraPos.y += 0.4f;

	firstPersonCamera->LookTo(cameraPos, mCar.GetCarDirection(), XMFLOAT3(0.0f, 1.0f, 0.0f));
	mCameraMode = CameraMode::FirstPerson;
	mCamera->UpdateViewMatrix();
	mBasicEffect.SetViewMatrix(mCamera->GetViewMatrix());
	mBasicEffect.SetEyePos(mCamera->GetPositionVector());
	mCamera->SetFrustum(XM_PI / 3, AspectRatio(), 0.5f, 1000.0f);
	mBasicEffect.SetProjMatrix(mCamera->GetProjMatrix());
}

void GameApp::InitThirdPersonCamera() {
	auto thirdPersonCamera = std::dynamic_pointer_cast<ThirdPersonCamera>(mCamera);
	thirdPersonCamera.reset(new ThirdPersonCamera);
	mCamera = thirdPersonCamera;
	thirdPersonCamera->RotateY(-XM_PIDIV2); // ��ʼ��ʱ������ǰ����
	thirdPersonCamera->SetViewPort(0.0f, 0.0f, (float)mClientWidth, (float)mClientHeight);
	thirdPersonCamera->SetTarget(mCar.GetCarPosition());
	thirdPersonCamera->SetDistance(5.0f);
	thirdPersonCamera->SetDistanceMinMax(3.0f, 10.0f);
	mCameraMode = CameraMode::ThirdPerson;
	mCamera->UpdateViewMatrix();
	mBasicEffect.SetViewMatrix(mCamera->GetViewMatrix());
	mBasicEffect.SetEyePos(mCamera->GetPositionVector());
	mCamera->SetFrustum(XM_PI / 3, AspectRatio(), 0.5f, 1000.0f);
	mBasicEffect.SetProjMatrix(mCamera->GetProjMatrix());
}

void GameApp::InitDayLight()
{
	// ����ƽ�л�����
	DirectionalLight dirLight;
	dirLight.Ambient = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	dirLight.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	dirLight.Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	dirLight.Direction = XMFLOAT3(0.0f, -1.0f, 0.0f);
	mBasicEffect.SetDirLight(0, dirLight);

	// �رճ���ͷ��ӵĵ��Դ
	mBasicEffect.SetPointLight(0, PointLight());
	mBasicEffect.SetPointLight(1, PointLight());
}

void GameApp::InitSunSetLight()
{
	DirectionalLight dirLight;
	dirLight.Ambient = XMFLOAT4(0.5f, 0.4f, 0.4f, 1.0f);
	dirLight.Diffuse = XMFLOAT4(0.6f, 0.5f, 0.5f, 1.0f);
	dirLight.Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	dirLight.Direction = XMFLOAT3(0.0f, -1.0f, 0.0f);
	mBasicEffect.SetDirLight(0, dirLight);

	// �رճ���ͷ��ӵĵ��Դ
	mBasicEffect.SetPointLight(0, PointLight());
	mBasicEffect.SetPointLight(1, PointLight());
}

void GameApp::InitNightLight()
{
	DirectionalLight dirLight;
	dirLight.Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	dirLight.Diffuse = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	dirLight.Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	dirLight.Direction = XMFLOAT3(0.0f, -1.0f, 0.0f);
	mBasicEffect.SetDirLight(0, dirLight);

	// ��������ͷ��ӵĵ��Դ
	mBasicEffect.SetPointLight(0, mCarPortLight);
	mBasicEffect.SetPointLight(1, mHouseLight);
}

void GameApp::InitCarPort() {
	ComPtr<ID3D11ShaderResourceView> texture;

	// ��ʼ������ǽ��
	mWalls.resize(5);
	HR(CreateDDSTextureFromFile(md3dDevice.Get(), L"Texture\\wall\\wall.dds", nullptr, texture.ReleaseAndGetAddressOf()));
	// �������ǽ������������
	//     ____     ____
	//    /| 0 |   | 1 |\
	//   /4|___|___|___|2\
	//  /_/_ _ _ _ _ _ _\_\
	// | /       3       \ |
	// |/_________________\|
	//
	for (int i = 0; i < 5; ++i)
	{
		mWalls[i].SetMaterial(mNormalMeterialMat);
		mWalls[i].SetTexture(texture);
	}
	mWalls[0].SetModel(Model(md3dDevice, Geometry::CreatePlane(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(6.0f, 8.0f), XMFLOAT2(1.5f, 2.0f))));
	mWalls[1].SetModel(Model(md3dDevice, Geometry::CreatePlane(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(6.0f, 8.0f), XMFLOAT2(1.5f, 2.0f))));
	mWalls[2].SetModel(Model(md3dDevice, Geometry::CreatePlane(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(20.0f, 8.0f), XMFLOAT2(5.0f, 2.0f))));
	mWalls[3].SetModel(Model(md3dDevice, Geometry::CreatePlane(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(20.0f, 8.0f), XMFLOAT2(5.0f, 2.0f))));
	mWalls[4].SetModel(Model(md3dDevice, Geometry::CreatePlane(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(20.0f, 8.0f), XMFLOAT2(5.0f, 2.0f))));

	mWalls[0].SetWorldMatrix(XMMatrixRotationX(-XM_PIDIV2) * XMMatrixTranslation(-7.0f, 0.0f, 10.0f) * XMMatrixTranslation(0.0f, 0.0f, -100.0f));
	mWalls[1].SetWorldMatrix(XMMatrixRotationX(-XM_PIDIV2) * XMMatrixTranslation(7.0f, 0.0f, 10.0f) * XMMatrixTranslation(0.0f, 0.0f, -100.0f));
	mWalls[2].SetWorldMatrix(XMMatrixRotationY(-XM_PIDIV2) * XMMatrixRotationZ(XM_PIDIV2) * XMMatrixTranslation(10.0f, 0.0f, 0.0f) * XMMatrixTranslation(0.0f, 0.0f, -100.0f));
	mWalls[3].SetWorldMatrix(XMMatrixRotationX(XM_PIDIV2) * XMMatrixTranslation(0.0f, 0.0f, -10.0f) * XMMatrixTranslation(0.0f, 0.0f, -100.0f));
	mWalls[4].SetWorldMatrix(XMMatrixRotationY(XM_PIDIV2) * XMMatrixRotationZ(-XM_PIDIV2) * XMMatrixTranslation(-10.0f, 0.0f, 0.0f) * XMMatrixTranslation(0.0f, 0.0f, -100.0f));

	// ��ʼ��������Դ
	XMFLOAT3 carWorldPos = mCar.GetCarPosition();
	mCarPortLight.Position = XMFLOAT3(carWorldPos.x, carWorldPos.y + 10.0f, carWorldPos.z);
	mCarPortLight.Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	mCarPortLight.Diffuse = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	mCarPortLight.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mCarPortLight.Att = XMFLOAT3(0.0f, 0.1f, 0.0f);
	mCarPortLight.Range = 18.0f;
}

void GameApp::InitHouse() {
	// ��ʼ������ģ��
	mObjReader.Read(L"Model\\house.mbo", L"Model\\house.obj");
	mHouse.SetModel(Model(md3dDevice, mObjReader));
	mHouse.SetMaterial(mNormalMeterialMat);
	// ��ȡ���ݰ�Χ��
	XMMATRIX S = XMMatrixScaling(0.05f, 0.05f, 0.05f) * XMMatrixTranslation(0.0f, 0.0f, 100.0f);
	BoundingBox houseBox = mHouse.GetLocalBoundingBox();
	// ��ȡ���Ӱ�Χ�ж���
	mHouseBoxData = Collision::CreateBoundingBox(houseBox, XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	houseBox.Transform(houseBox, S);
	// �÷��ݵײ���������
	mHouse.SetWorldMatrix(S * XMMatrixTranslation(0.0f, -(houseBox.Center.y - houseBox.Extents.y + 1.0f), 0.0f));

	// ��ʼ�����ӵ��Դ
	XMFLOAT3 houseWorldPos = mHouse.GetPosition();
	mHouseLight.Position = XMFLOAT3(houseWorldPos.x, houseWorldPos.y + 10.0f, houseWorldPos.z);
	mHouseLight.Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	mHouseLight.Diffuse = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	mHouseLight.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mHouseLight.Att = XMFLOAT3(0.0f, 0.1f, 0.0f);
	mHouseLight.Range = 18.0f;
}

void GameApp::InitCar() {
	mCar.InitCar(md3dDevice);
	mCar.SetCarWorldMatrix(XMMatrixTranslation(0.0f, 0.0f, -100.0f));

	// ��ȡ������Χ��
	BoundingBox carBox = mCar.GetLocalBoundingBox();
	// ��ȡ���Ӱ�Χ�ж���
	mCarBoxData = Collision::CreateBoundingBox(carBox, XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));

	// ��ʼ���󳵵�
	mCarLeftLight.Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	mCarLeftLight.Diffuse = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	mCarLeftLight.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mCarLeftLight.Position = mCar.GetCarLeftLightPosition();
	mCarLeftLight.Direction = mCar.GetCarDirection();
	mCarLeftLight.Att = XMFLOAT3(1.0f, 0.0f, 0.0f);
	mCarLeftLight.Spot = 8.0f;
	mCarLeftLight.Range = 50.0f;

	// ��ʼ���ҳ���
	mCarRightLight.Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	mCarRightLight.Diffuse = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	mCarRightLight.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mCarRightLight.Position = mCar.GetCarRightLightPosition();
	mCarRightLight.Direction = mCar.GetCarDirection();
	mCarRightLight.Att = XMFLOAT3(1.0f, 0.0f, 0.0f);
	mCarRightLight.Spot = 8.0f;
	mCarRightLight.Range = 50.0f;
}

void GameApp::InitSkyBox() {
	// ��ʼ��������պ�
	mDaylight = std::make_unique<SkyRender>(
		md3dDevice, md3dImmediateContext,
		L"Texture\\skybox\\daylight.jpg",
		5000.0f);

	mSunset = std::make_unique<SkyRender>(
		md3dDevice, md3dImmediateContext,
		std::vector<std::wstring>{
		L"Texture\\skybox\\sunset_posX.bmp", L"Texture\\skybox\\sunset_negX.bmp",
			L"Texture\\skybox\\sunset_posY.bmp", L"Texture\\skybox\\sunset_negY.bmp",
			L"Texture\\skybox\\sunset_posZ.bmp", L"Texture\\skybox\\sunset_negZ.bmp", },
		5000.0f);

	mNight = std::make_unique<SkyRender>(
		md3dDevice, md3dImmediateContext,
		L"Texture\\skybox\\night.dds",
		5000.0f);

	// Ĭ����պ�Ϊ����
	mSkyBoxMode = SkyBoxMode::Daylight;
}

void GameApp::InitPlane() {
	ComPtr<ID3D11ShaderResourceView> texture;

	// ��ʼ���ذ�
	mObjReader.Read(L"Model\\ground.mbo", L"Model\\ground.obj");
	mGround.SetModel(Model(md3dDevice, mObjReader));
	mGround.SetMaterial(mNormalMeterialMat);
	mGround.SetWorldMatrix(XMMatrixTranslation(0.0f, 1.0f, 0.0f));

	// ��ʼ����·
	HR(CreateDDSTextureFromFile(md3dDevice.Get(), L"Texture\\Plane\\road.dds", nullptr, texture.ReleaseAndGetAddressOf()));
	mRoad.SetModel(Model(md3dDevice,
		Geometry::CreatePlane(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(180.0f, 8.0f), XMFLOAT2(10.0f, 1.0f))));
	mRoad.SetWorldMatrix(XMMatrixRotationY(-XM_PIDIV2) * XMMatrixTranslation(0.0f, -0.999f, 0.0f));
	mRoad.SetTexture(texture);
	mRoad.SetMaterial(mNormalMeterialMat);
}

void GameApp::InitTree()
{
	vector<pair<float, float>> temp;  // �����ų��ظ�����

	srand((unsigned)time(nullptr));

	// ��ʼ����
	mObjReader.Read(L"Model\\tree.mbo", L"Model\\tree.obj");
	mTrees.SetModel(Model(md3dDevice, mObjReader));
	mTrees.SetMaterial(mNormalMeterialMat);
	XMMATRIX S = XMMatrixScaling(0.015f, 0.015f, 0.015f);

	BoundingBox treeBox = mTrees.GetLocalBoundingBox();
	// ��ȡ����Χ�ж���
	mTreeBoxData = Collision::CreateBoundingBox(treeBox, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	// ����ľ�ײ���������λ��y = 1.0��ƽ��
	treeBox.Transform(treeBox, S);
	XMMATRIX T0 = XMMatrixTranslation(0.0f, -(treeBox.Center.y - treeBox.Extents.y + 1.0f), 0.0f);

	// �ڵ�·�ұ��������256����
	for (int i = 0; i < 256; ++i) {
		float x = (rand() % (100 - 6 + 1)) + 6;  // ����[6,100]֮����������Ϊ����x����
		float z = (rand() % (90 + 90 + 1)) - 90;  // ����[-90,90]֮����������Ϊ����z����
		// ������ظ���������������
		while (find(temp.begin(), temp.end(), make_pair(x, z)) != temp.end()) {
			x = (rand() % (100 - 6 + 1)) + 6;  // ����[5,100]֮����������Ϊ����x����
			z = (rand() % (90 + 90 + 1)) - 90;  // ����[-90,90]֮����������Ϊ����z����
		}
		temp.push_back(make_pair(x, z));
		XMMATRIX T1 = XMMatrixTranslation(x, 0.0f, z);
		XMMATRIX R = XMMatrixRotationY(rand() % 256 / 256.0f * XM_2PI);
		XMMATRIX World = S * R * T0 * T1;
		mInstancedData.push_back(World);
	}

	temp.clear();

	// �ڵ�·����������256����
	for (int i = 0; i < 256; ++i) {
		float x = (rand() % (100 - 6 + 1)) + 6;  // ����[6,100]֮����������Ϊ����x����
		float z = (rand() % (90 + 90 + 1)) - 90;  // ����[-90,90]֮����������Ϊ����z����
		// ������ظ���������������
		while (find(temp.begin(), temp.end(), make_pair(x, z)) != temp.end()) {
			x = (rand() % (100 - 6 + 1)) + 6;  // ����[5,100]֮����������Ϊ����x����
			z = (rand() % (90 + 90 + 1)) - 90;  // ����[-90,90]֮����������Ϊ����z����
		}
		temp.push_back(make_pair(x, z));

		XMMATRIX T1 = XMMatrixTranslation(-x, 0.0f, z);
		XMMATRIX R = XMMatrixRotationY(rand() % 256 / 256.0f * XM_2PI);
		XMMATRIX World = S * R * T0 * T1;
		mInstancedData.push_back(World);
	}
}

bool GameApp::InitResource()
{
	mRenderBox = false;  // Ĭ�ϲ���Ⱦ��ײ��

	// ********��ʼ����պ�********
	GameApp::InitSkyBox();

	// ********��ʼ������********
	GameApp::InitMaterial();

	// ********��ʼ�������е�����********
	// ��ʼ������
	GameApp::InitCar();

	// ��ʼ������
	GameApp::InitHouse();

	// ��ʼ������
	GameApp::InitCarPort();

	// ��ʼ����ľ
	GameApp::InitTree();

	// ��ʼ������
	GameApp::InitPlane();

	// ********��ʼ�������********
	// ��ʼ��Ϊ�����˳������
	GameApp::InitThirdPersonCamera();
	
	// ********��ʼ�����������Ӱ����ͷ�����Ӱ����********
	mBasicEffect.SetReflectionMatrix(XMMatrixReflect(XMVectorSet(0.0f, 0.0f, -1.0f, 10.0f)));
	// ��΢��һ��λ������ʾ��Ӱ
	mBasicEffect.SetShadowMatrix(XMMatrixShadow(XMVectorSet(0.0f, 1.0f, 0.0f, 0.99f), XMVectorSet(0.0f, 10.0f, -10.0f, 1.0f)));
	mBasicEffect.SetRefShadowMatrix(XMMatrixShadow(XMVectorSet(0.0f, 1.0f, 0.0f, 0.99f), XMVectorSet(0.0f, 10.0f, 30.0f, 1.0f)));

	// ********��ʼ��ƽ�л�����********
	GameApp::InitDayLight();

	return true;
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
		mCar.CarMove(dt * 10.0f);
	}

	if (keyState.IsKeyDown(Keyboard::S))
	{
		mCar.SetCarMoveBack();
		mCar.CarMove(dt * -10.0f);

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
		XMFLOAT3 cameraPos = mCar.GetCarPosition();
		cameraPos.y += 0.4f;
		firstPersonCamera->LookTo(cameraPos, mCar.GetCarDirection(), XMFLOAT3(0.0f, 1.0f, 0.0f));
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

	// �����ģʽ�л��߼�(��Q�����л�)
	if (mKeyboardTracker.IsKeyPressed(Keyboard::Q)) {
		if (mCameraMode == CameraMode::FirstPerson) {
			InitThirdPersonCamera();
		}
		else if (mCameraMode == CameraMode::ThirdPerson) {
			InitFirstPersonCamera();
		}
	}
	
	// ѡ����պ�
	if (mKeyboardTracker.IsKeyPressed(Keyboard::D1))
	{
		mSkyBoxMode = SkyBoxMode::Daylight;
		GameApp::InitDayLight();
	}
	if (mKeyboardTracker.IsKeyPressed(Keyboard::D2))
	{
		mSkyBoxMode = SkyBoxMode::Sunset;
		GameApp::InitSunSetLight();
	}
	if (mKeyboardTracker.IsKeyPressed(Keyboard::D3))
	{
		mSkyBoxMode = SkyBoxMode::Night;
		GameApp::InitNightLight();
	}

	// ��H�򿪹رճ���
	if (mKeyboardTracker.IsKeyPressed(Keyboard::H)) {
		mCar.CarLightOnOff(md3dDevice);
	}

	// ���³���
	if (mCar.GetCarLightState()) {
		// ���󳵵�
		mCarLeftLight.Position = mCar.GetCarLeftLightPosition();
		mCarLeftLight.Direction = mCar.GetCarDirection();
		mBasicEffect.SetSpotLight(0, mCarLeftLight);

		// ���ҳ���
		mCarRightLight.Position = mCar.GetCarRightLightPosition();
		mCarRightLight.Direction = mCar.GetCarDirection();
		mBasicEffect.SetSpotLight(1, mCarRightLight);
	}
	else {
		// �ر��󳵵ƺ��ҳ���
		mBasicEffect.SetSpotLight(0, SpotLight());
		mBasicEffect.SetSpotLight(1, SpotLight());
	}

	// ��B�򿪹ر���ײ����Ⱦ
	if (mKeyboardTracker.IsKeyPressed(Keyboard::B)) {
		mRenderBox = !mRenderBox;
	}

	// ��ײ���
	if (mCar.GetBoundingBox().Intersects(mHouse.GetBoundingBox())) {
		mIsCollision = true;

		mCar.SetCarMoveBack();
		mCar.CarMove(dt * -50.0f);
	}
	else {
		mIsCollision = false;
	}
	
	// �˳���������Ӧ�򴰿ڷ���������Ϣ
	if (mKeyboardTracker.IsKeyPressed(Keyboard::Escape))
		SendMessage(MainWnd(), WM_DESTROY, 0, 0);
	
}

void GameApp::DrawScene()
{
	assert(md3dImmediateContext);
	assert(mSwapChain);

	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView.Get(), reinterpret_cast<const float*>(&Colors::Silver));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// ȷ��ʹ�õ����ݼ�
	const std::vector<XMMATRIX>& refData = mInstancedData;

	// ************************
	// ���Ʋ�͸������������
	//
	mBasicEffect.SetRenderDefault(md3dImmediateContext, BasicEffect::RenderObject);

	for (auto& wall : mWalls)
		wall.Draw(md3dImmediateContext, mBasicEffect);
	mCar.Draw(md3dImmediateContext, mBasicEffect);
	mGround.Draw(md3dImmediateContext, mBasicEffect);
	mHouse.Draw(md3dImmediateContext, mBasicEffect);
	mRoad.Draw(md3dImmediateContext, mBasicEffect);

	mBasicEffect.SetRenderDefault(md3dImmediateContext, BasicEffect::RenderInstance);
	mTrees.DrawInstanced(md3dImmediateContext, mBasicEffect, refData);


	// ************************
	// ���Ʋ�͸�������������Ӱ
	//
	mCar.SetCarMaterial(mShadowMat);

	mBasicEffect.SetShadowState(true);	// ��Ӱ����
	mBasicEffect.SetRenderNoDoubleBlend(md3dImmediateContext, 0, BasicEffect::RenderObject);

	mCar.Draw(md3dImmediateContext, mBasicEffect);

	mBasicEffect.SetShadowState(false);		// ��Ӱ�ر�
	mCar.SetCarMaterial(mNormalMeterialMat);

	// ������պ�
	mSkyEffect.SetRenderDefault(md3dImmediateContext);
	switch (mSkyBoxMode)
	{
		case SkyBoxMode::Daylight: mDaylight->Draw(md3dImmediateContext, mSkyEffect, *mCamera); break;
		case SkyBoxMode::Sunset: mSunset->Draw(md3dImmediateContext, mSkyEffect, *mCamera); break;
		case SkyBoxMode::Night: mNight->Draw(md3dImmediateContext, mSkyEffect, *mCamera); break;
	}

	// ************************
	if (mRenderBox) {
		// ���ư�Χ��
		mBoundingBoxEffect.SetRenderDefault(md3dImmediateContext);
		// ���Ʒ��Ӱ�Χ��
		XMMATRIX WVP = mHouse.GetWorldMatrix() * mCamera->GetViewMatrix() * mCamera->GetProjMatrix();
		mCollision.Draw(md3dDevice, mHouseBoxData, md3dImmediateContext, mBoundingBoxEffect, WVP);
		// ����������Χ��
		WVP = mCar.GetCarWorldMatrix() * mCamera->GetViewMatrix() * mCamera->GetProjMatrix();
		mCollision.Draw(md3dDevice, mCarBoxData, md3dImmediateContext, mBoundingBoxEffect, WVP);
	}

	// ******************
	// ����Direct2D����
	//
	if (md2dRenderTarget != nullptr)
	{
		md2dRenderTarget->BeginDraw();
		std::wstring text = L"��Q�л������ģʽ:\n"
			"W/S/A/D ǰ��/����/��ת/��ת\n"
			"��H�򿪹رճ��� ��B�򿪹ر���ײ�е���Ⱦ\n"
			"��1��������պ� ��2���ƻ���պ� ��3��ҹ����պ�\n"
			"����ƶ�������Ұ ���ֿ��ƹ۲����(�����˳�ģʽ��)\n"
			"Alt+Enter��ȫ��  Esc���˳�\n"
			"��ǰģʽ: ";
		if (mCameraMode == CameraMode::FirstPerson)
			text += L"��һ�˳�\n";
		else if (mCameraMode == CameraMode::ThirdPerson)
			text += L"�����˳�\n";
		if (mIsCollision)
			text += L"������ײ��";
		else
			text += L"δ������ײ��";
		md2dRenderTarget->DrawTextW(text.c_str(), (UINT32)text.length(), mTextFormat.Get(),
			D2D1_RECT_F{ 0.0f, 0.0f, 600.0f, 200.0f }, mColorBrush.Get());
		HR(md2dRenderTarget->EndDraw());
	}

	HR(mSwapChain->Present(0, 0));
	
}


