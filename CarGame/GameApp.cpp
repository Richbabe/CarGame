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

	// 务必先初始化所有渲染状态，以供下面的特效使用
	RenderStates::InitAll(md3dDevice);

	if (!mBasicEffect.InitAll(md3dDevice))
		return false;

	if (!mSkyEffect.InitAll(md3dDevice))
		return false;

	if (!mBoundingBoxEffect.InitAll(md3dDevice))
		return false;

	if (!InitResource())
		return false;

	// 初始化鼠标，键盘不需要
	mMouse->SetWindow(mhMainWnd);
	mMouse->SetMode(DirectX::Mouse::MODE_RELATIVE);

	return true;
}

void GameApp::OnResize()
{
	assert(md2dFactory);
	assert(mdwriteFactory);
	// 释放D2D的相关资源
	mColorBrush.Reset();
	md2dRenderTarget.Reset();

	D3DApp::OnResize();

	// 为D2D创建DXGI表面渲染目标
	ComPtr<IDXGISurface> surface;
	HR(mSwapChain->GetBuffer(0, __uuidof(IDXGISurface), reinterpret_cast<void**>(surface.GetAddressOf())));
	D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
		D2D1_RENDER_TARGET_TYPE_DEFAULT,
		D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED));
	HRESULT hr = md2dFactory->CreateDxgiSurfaceRenderTarget(surface.Get(), &props, md2dRenderTarget.GetAddressOf());
	surface.Reset();

	if (hr == E_NOINTERFACE)
	{
		OutputDebugString(L"\n警告：Direct2D与Direct3D互操作性功能受限，你将无法看到文本信息。现提供下述可选方法：\n"
			"1. 对于Win7系统，需要更新至Win7 SP1，并安装KB2670838补丁以支持Direct2D显示。\n"
			"2. 自行完成Direct3D 10.1与Direct2D的交互。详情参阅："
			"https://docs.microsoft.com/zh-cn/windows/desktop/Direct2D/direct2d-and-direct3d-interoperation-overview""\n"
			"3. 使用别的字体库，比如FreeType。\n\n");
	}
	else if (hr == S_OK)
	{
		// 创建固定颜色刷和文本格式
		HR(md2dRenderTarget->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::White),
			mColorBrush.GetAddressOf()));
		HR(mdwriteFactory->CreateTextFormat(L"宋体", nullptr, DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 15, L"zh-cn",
			mTextFormat.GetAddressOf()));
	}
	else
	{
		// 报告异常问题
		assert(md2dRenderTarget);
	}

	// 摄像机变更显示
	if (mCamera != nullptr)
	{
		mCamera->SetFrustum(XM_PI / 3, AspectRatio(), 0.5f, 1000.0f);
		mCamera->SetViewPort(0.0f, 0.0f, (float)mClientWidth, (float)mClientHeight);
		mBasicEffect.SetProjMatrix(mCamera->GetProjMatrix());
	}
}

void GameApp::InitMaterial() {
	// 初始化普通材质
	mNormalMeterialMat.Ambient = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	mNormalMeterialMat.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	mNormalMeterialMat.Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 16.0f);

	// 初始化阴影材质
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
	thirdPersonCamera->RotateY(-XM_PIDIV2); // 初始化时朝汽车前方看
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
	// 设置平行环境光
	DirectionalLight dirLight;
	dirLight.Ambient = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	dirLight.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	dirLight.Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	dirLight.Direction = XMFLOAT3(0.0f, -1.0f, 0.0f);
	mBasicEffect.SetDirLight(0, dirLight);

	// 关闭车库和房子的点光源
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

	// 关闭车库和房子的点光源
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

	// 开启车库和房子的点光源
	mBasicEffect.SetPointLight(0, mCarPortLight);
	mBasicEffect.SetPointLight(1, mHouseLight);
}

void GameApp::InitCarPort() {
	ComPtr<ID3D11ShaderResourceView> texture;

	// 初始化车库墙体
	mWalls.resize(5);
	HR(CreateDDSTextureFromFile(md3dDevice.Get(), L"Texture\\wall\\wall.dds", nullptr, texture.ReleaseAndGetAddressOf()));
	// 这里控制墙体五个面的生成
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

	// 初始化车库点光源
	XMFLOAT3 carWorldPos = mCar.GetCarPosition();
	mCarPortLight.Position = XMFLOAT3(carWorldPos.x, carWorldPos.y + 10.0f, carWorldPos.z);
	mCarPortLight.Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	mCarPortLight.Diffuse = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	mCarPortLight.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mCarPortLight.Att = XMFLOAT3(0.0f, 0.1f, 0.0f);
	mCarPortLight.Range = 18.0f;
}

void GameApp::InitHouse() {
	// 初始化房屋模型
	mObjReader.Read(L"Model\\house.mbo", L"Model\\house.obj");
	mHouse.SetModel(Model(md3dDevice, mObjReader));
	mHouse.SetMaterial(mNormalMeterialMat);
	// 获取房屋包围盒
	XMMATRIX S = XMMatrixScaling(0.05f, 0.05f, 0.05f) * XMMatrixTranslation(0.0f, 0.0f, 100.0f);
	BoundingBox houseBox = mHouse.GetLocalBoundingBox();
	// 获取房子包围盒顶点
	mHouseBoxData = Collision::CreateBoundingBox(houseBox, XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	houseBox.Transform(houseBox, S);
	// 让房屋底部紧贴地面
	mHouse.SetWorldMatrix(S * XMMatrixTranslation(0.0f, -(houseBox.Center.y - houseBox.Extents.y + 1.0f), 0.0f));

	// 初始化房子点光源
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

	// 获取汽车包围盒
	BoundingBox carBox = mCar.GetLocalBoundingBox();
	// 获取房子包围盒顶点
	mCarBoxData = Collision::CreateBoundingBox(carBox, XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));

	// 初始化左车灯
	mCarLeftLight.Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	mCarLeftLight.Diffuse = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	mCarLeftLight.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mCarLeftLight.Position = mCar.GetCarLeftLightPosition();
	mCarLeftLight.Direction = mCar.GetCarDirection();
	mCarLeftLight.Att = XMFLOAT3(1.0f, 0.0f, 0.0f);
	mCarLeftLight.Spot = 8.0f;
	mCarLeftLight.Range = 50.0f;

	// 初始化右车灯
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
	// 初始化三个天空盒
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

	// 默认天空盒为白天
	mSkyBoxMode = SkyBoxMode::Daylight;
}

void GameApp::InitPlane() {
	ComPtr<ID3D11ShaderResourceView> texture;

	// 初始化地板
	mObjReader.Read(L"Model\\ground.mbo", L"Model\\ground.obj");
	mGround.SetModel(Model(md3dDevice, mObjReader));
	mGround.SetMaterial(mNormalMeterialMat);
	mGround.SetWorldMatrix(XMMatrixTranslation(0.0f, 1.0f, 0.0f));

	// 初始化道路
	HR(CreateDDSTextureFromFile(md3dDevice.Get(), L"Texture\\Plane\\road.dds", nullptr, texture.ReleaseAndGetAddressOf()));
	mRoad.SetModel(Model(md3dDevice,
		Geometry::CreatePlane(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(180.0f, 8.0f), XMFLOAT2(10.0f, 1.0f))));
	mRoad.SetWorldMatrix(XMMatrixRotationY(-XM_PIDIV2) * XMMatrixTranslation(0.0f, -0.999f, 0.0f));
	mRoad.SetTexture(texture);
	mRoad.SetMaterial(mNormalMeterialMat);
}

void GameApp::InitTree()
{
	vector<pair<float, float>> temp;  // 用于排除重复坐标

	srand((unsigned)time(nullptr));

	// 初始化树
	mObjReader.Read(L"Model\\tree.mbo", L"Model\\tree.obj");
	mTrees.SetModel(Model(md3dDevice, mObjReader));
	mTrees.SetMaterial(mNormalMeterialMat);
	XMMATRIX S = XMMatrixScaling(0.015f, 0.015f, 0.015f);

	BoundingBox treeBox = mTrees.GetLocalBoundingBox();
	// 获取树包围盒顶点
	mTreeBoxData = Collision::CreateBoundingBox(treeBox, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	// 让树木底部紧贴地面位于y = 1.0的平面
	treeBox.Transform(treeBox, S);
	XMMATRIX T0 = XMMatrixTranslation(0.0f, -(treeBox.Center.y - treeBox.Extents.y + 1.0f), 0.0f);

	// 在道路右边随机生成256棵树
	for (int i = 0; i < 256; ++i) {
		float x = (rand() % (100 - 6 + 1)) + 6;  // 生成[6,100]之间的随机数作为树的x坐标
		float z = (rand() % (90 + 90 + 1)) - 90;  // 生成[-90,90]之间的随机数作为树的z坐标
		// 如果有重复坐标则重新生成
		while (find(temp.begin(), temp.end(), make_pair(x, z)) != temp.end()) {
			x = (rand() % (100 - 6 + 1)) + 6;  // 生成[5,100]之间的随机数作为树的x坐标
			z = (rand() % (90 + 90 + 1)) - 90;  // 生成[-90,90]之间的随机数作为树的z坐标
		}
		temp.push_back(make_pair(x, z));
		XMMATRIX T1 = XMMatrixTranslation(x, 0.0f, z);
		XMMATRIX R = XMMatrixRotationY(rand() % 256 / 256.0f * XM_2PI);
		XMMATRIX World = S * R * T0 * T1;
		mInstancedData.push_back(World);
	}

	temp.clear();

	// 在道路左边随机生成256棵树
	for (int i = 0; i < 256; ++i) {
		float x = (rand() % (100 - 6 + 1)) + 6;  // 生成[6,100]之间的随机数作为树的x坐标
		float z = (rand() % (90 + 90 + 1)) - 90;  // 生成[-90,90]之间的随机数作为树的z坐标
		// 如果有重复坐标则重新生成
		while (find(temp.begin(), temp.end(), make_pair(x, z)) != temp.end()) {
			x = (rand() % (100 - 6 + 1)) + 6;  // 生成[5,100]之间的随机数作为树的x坐标
			z = (rand() % (90 + 90 + 1)) - 90;  // 生成[-90,90]之间的随机数作为树的z坐标
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
	mRenderBox = false;  // 默认不渲染碰撞盒

	// ********初始化天空盒********
	GameApp::InitSkyBox();

	// ********初始化材质********
	GameApp::InitMaterial();

	// ********初始化场景中的物体********
	// 初始化汽车
	GameApp::InitCar();

	// 初始化房子
	GameApp::InitHouse();

	// 初始化车库
	GameApp::InitCarPort();

	// 初始化树木
	GameApp::InitTree();

	// 初始化地形
	GameApp::InitPlane();

	// ********初始化摄像机********
	// 初始化为第三人称摄像机
	GameApp::InitThirdPersonCamera();
	
	// ********初始化反射矩阵、阴影矩阵和反射阴影矩阵********
	mBasicEffect.SetReflectionMatrix(XMMatrixReflect(XMVectorSet(0.0f, 0.0f, -1.0f, 10.0f)));
	// 稍微高一点位置以显示阴影
	mBasicEffect.SetShadowMatrix(XMMatrixShadow(XMVectorSet(0.0f, 1.0f, 0.0f, 0.99f), XMVectorSet(0.0f, 10.0f, -10.0f, 1.0f)));
	mBasicEffect.SetRefShadowMatrix(XMMatrixShadow(XMVectorSet(0.0f, 1.0f, 0.0f, 0.99f), XMVectorSet(0.0f, 10.0f, 30.0f, 1.0f)));

	// ********初始化平行环境光********
	GameApp::InitDayLight();

	return true;
}


void GameApp::UpdateScene(float dt)
{
	static float phi = 0.0f, theta = 0.0f;
	// 更新鼠标事件，获取相对偏移量
	Mouse::State mouseState = mMouse->GetState();
	Mouse::State lastMouseState = mMouseTracker.GetLastState();
	mMouseTracker.Update(mouseState);

	Keyboard::State keyState = mKeyboard->GetState();
	mKeyboardTracker.Update(keyState);

	// 控制汽车运动逻辑
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

	// 控制摄像机运动逻辑
	auto firstPersonCamera = std::dynamic_pointer_cast<FirstPersonCamera>(mCamera);
	auto thirdPersonCamera = std::dynamic_pointer_cast<ThirdPersonCamera>(mCamera);
	if (mCameraMode == CameraMode::FirstPerson) {
		XMFLOAT3 cameraPos = mCar.GetCarPosition();
		cameraPos.y += 0.4f;
		firstPersonCamera->LookTo(cameraPos, mCar.GetCarDirection(), XMFLOAT3(0.0f, 1.0f, 0.0f));
	}
	else {
		// ******************
		// 第三人称摄像机的操作
		//
		thirdPersonCamera->SetTarget(mCar.GetCarPosition());

		// 绕物体旋转
		thirdPersonCamera->RotateX(mouseState.y * dt * 1.25f);
		thirdPersonCamera->RotateY(mouseState.x * dt * 1.25f);
		thirdPersonCamera->Approach(-mouseState.scrollWheelValue / 120 * 1.0f);
	}
	mCamera->UpdateViewMatrix();
	mBasicEffect.SetViewMatrix(mCamera->GetViewMatrix());
	mBasicEffect.SetEyePos(mCamera->GetPositionVector());

	// 摄像机模式切换逻辑(按Q进行切换)
	if (mKeyboardTracker.IsKeyPressed(Keyboard::Q)) {
		if (mCameraMode == CameraMode::FirstPerson) {
			InitThirdPersonCamera();
		}
		else if (mCameraMode == CameraMode::ThirdPerson) {
			InitFirstPersonCamera();
		}
	}
	
	// 选择天空盒
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

	// 按H打开关闭车灯
	if (mKeyboardTracker.IsKeyPressed(Keyboard::H)) {
		mCar.CarLightOnOff(md3dDevice);
	}

	// 更新车灯
	if (mCar.GetCarLightState()) {
		// 打开左车灯
		mCarLeftLight.Position = mCar.GetCarLeftLightPosition();
		mCarLeftLight.Direction = mCar.GetCarDirection();
		mBasicEffect.SetSpotLight(0, mCarLeftLight);

		// 打开右车灯
		mCarRightLight.Position = mCar.GetCarRightLightPosition();
		mCarRightLight.Direction = mCar.GetCarDirection();
		mBasicEffect.SetSpotLight(1, mCarRightLight);
	}
	else {
		// 关闭左车灯和右车灯
		mBasicEffect.SetSpotLight(0, SpotLight());
		mBasicEffect.SetSpotLight(1, SpotLight());
	}

	// 按B打开关闭碰撞盒渲染
	if (mKeyboardTracker.IsKeyPressed(Keyboard::B)) {
		mRenderBox = !mRenderBox;
	}

	// 碰撞检测
	if (mCar.GetBoundingBox().Intersects(mHouse.GetBoundingBox())) {
		mIsCollision = true;

		mCar.SetCarMoveBack();
		mCar.CarMove(dt * -50.0f);
	}
	else {
		mIsCollision = false;
	}
	
	// 退出程序，这里应向窗口发送销毁信息
	if (mKeyboardTracker.IsKeyPressed(Keyboard::Escape))
		SendMessage(MainWnd(), WM_DESTROY, 0, 0);
	
}

void GameApp::DrawScene()
{
	assert(md3dImmediateContext);
	assert(mSwapChain);

	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView.Get(), reinterpret_cast<const float*>(&Colors::Silver));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// 确定使用的数据集
	const std::vector<XMMATRIX>& refData = mInstancedData;

	// ************************
	// 绘制不透明的正常物体
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
	// 绘制不透明正常物体的阴影
	//
	mCar.SetCarMaterial(mShadowMat);

	mBasicEffect.SetShadowState(true);	// 阴影开启
	mBasicEffect.SetRenderNoDoubleBlend(md3dImmediateContext, 0, BasicEffect::RenderObject);

	mCar.Draw(md3dImmediateContext, mBasicEffect);

	mBasicEffect.SetShadowState(false);		// 阴影关闭
	mCar.SetCarMaterial(mNormalMeterialMat);

	// 绘制天空盒
	mSkyEffect.SetRenderDefault(md3dImmediateContext);
	switch (mSkyBoxMode)
	{
		case SkyBoxMode::Daylight: mDaylight->Draw(md3dImmediateContext, mSkyEffect, *mCamera); break;
		case SkyBoxMode::Sunset: mSunset->Draw(md3dImmediateContext, mSkyEffect, *mCamera); break;
		case SkyBoxMode::Night: mNight->Draw(md3dImmediateContext, mSkyEffect, *mCamera); break;
	}

	// ************************
	if (mRenderBox) {
		// 绘制包围盒
		mBoundingBoxEffect.SetRenderDefault(md3dImmediateContext);
		// 绘制房子包围盒
		XMMATRIX WVP = mHouse.GetWorldMatrix() * mCamera->GetViewMatrix() * mCamera->GetProjMatrix();
		mCollision.Draw(md3dDevice, mHouseBoxData, md3dImmediateContext, mBoundingBoxEffect, WVP);
		// 绘制汽车包围盒
		WVP = mCar.GetCarWorldMatrix() * mCamera->GetViewMatrix() * mCamera->GetProjMatrix();
		mCollision.Draw(md3dDevice, mCarBoxData, md3dImmediateContext, mBoundingBoxEffect, WVP);
	}

	// ******************
	// 绘制Direct2D部分
	//
	if (md2dRenderTarget != nullptr)
	{
		md2dRenderTarget->BeginDraw();
		std::wstring text = L"按Q切换摄像机模式:\n"
			"W/S/A/D 前进/后退/左转/右转\n"
			"按H打开关闭车灯 按B打开关闭碰撞盒的渲染\n"
			"按1：白天天空盒 按2：黄昏天空盒 按3：夜晚天空盒\n"
			"鼠标移动控制视野 滚轮控制观察距离(第三人称模式下)\n"
			"Alt+Enter：全屏  Esc：退出\n"
			"当前模式: ";
		if (mCameraMode == CameraMode::FirstPerson)
			text += L"第一人称\n";
		else if (mCameraMode == CameraMode::ThirdPerson)
			text += L"第三人称\n";
		if (mIsCollision)
			text += L"发生碰撞！";
		else
			text += L"未发生碰撞！";
		md2dRenderTarget->DrawTextW(text.c_str(), (UINT32)text.length(), mTextFormat.Get(),
			D2D1_RECT_F{ 0.0f, 0.0f, 600.0f, 200.0f }, mColorBrush.Get());
		HR(md2dRenderTarget->EndDraw());
	}

	HR(mSwapChain->Present(0, 0));
	
}


