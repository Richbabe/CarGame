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

	// 务必先初始化所有渲染状态，以供下面的特效使用
	RenderStates::InitAll(md3dDevice);

	if (!mBasicEffect.InitAll(md3dDevice))
		return false;

	if (!mSkyEffect.InitAll(md3dDevice))
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
		firstPersonCamera->LookTo(mCar.GetCarPosition(), mCar.GetCarDirection(), XMFLOAT3(0.0f, 1.0f, 0.0f));
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

	// 选择天空盒
	if (mKeyboardTracker.IsKeyPressed(Keyboard::D1))
	{
		mSkyBoxMode = SkyBoxMode::Daylight;
	}
	if (mKeyboardTracker.IsKeyPressed(Keyboard::D2))
	{
		mSkyBoxMode = SkyBoxMode::Sunset;
	}
	if (mKeyboardTracker.IsKeyPressed(Keyboard::D3))
	{
		mSkyBoxMode = SkyBoxMode::Desert;
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

	// 统计实际绘制的物体数目
	std::vector<XMMATRIX> acceptedData;
	// 是否开启视锥体裁剪
	if (mEnableFrustumCulling)
	{
		acceptedData = Collision::FrustumCulling(mInstancedData, mTrees.GetLocalBoundingBox(),
			mCamera->GetViewMatrix(), mCamera->GetProjMatrix());
	}
	// 确定使用的数据集
	const std::vector<XMMATRIX>& refData = mEnableFrustumCulling ? acceptedData : mInstancedData;

	// *********************
	// 1. 给镜面反射区域写入值1到模板缓冲区
	// 

	mBasicEffect.SetWriteStencilOnly(md3dImmediateContext, 1, BasicEffect::RenderObject);
	mMirror.Draw(md3dImmediateContext, mBasicEffect);

	// ***********************
	// 2. 绘制不透明的反射物体
	//

	// 开启反射绘制
	mBasicEffect.SetReflectionState(true);
	mBasicEffect.SetRenderDefaultWithStencil(md3dImmediateContext, 1, BasicEffect::RenderObject);

	mWalls[2].Draw(md3dImmediateContext, mBasicEffect);
	mWalls[3].Draw(md3dImmediateContext, mBasicEffect);
	mWalls[4].Draw(md3dImmediateContext, mBasicEffect);

	mCar.Draw(md3dImmediateContext, mBasicEffect);
	mGround.Draw(md3dImmediateContext, mBasicEffect);
	mHouse.Draw(md3dImmediateContext, mBasicEffect);

	// ***********************
	// 3. 绘制不透明反射物体的阴影
	//

	mCar.SetCarMaterial(mShadowMat);
	mHouse.SetMaterial(mShadowMat);
	mBasicEffect.SetShadowState(true);	// 反射开启，阴影开启			
	mBasicEffect.SetRenderNoDoubleBlend(md3dImmediateContext, 1, BasicEffect::RenderObject);

	mCar.Draw(md3dImmediateContext, mBasicEffect);
	mHouse.Draw(md3dImmediateContext, mBasicEffect);

	// 恢复到原来的状态
	mBasicEffect.SetShadowState(false);
	mCar.SetCarMaterial(mNormalMeterialMat);
	mHouse.SetMaterial(mNormalMeterialMat);

	// ***********************
	// 4. 绘制透明镜面
	//

	// 关闭反射绘制
	mBasicEffect.SetReflectionState(false);
	mBasicEffect.SetRenderAlphaBlendWithStencil(md3dImmediateContext, 1, BasicEffect::RenderObject);

	mMirror.Draw(md3dImmediateContext, mBasicEffect);

	// ************************
	// 5. 绘制不透明的正常物体
	//
	mBasicEffect.SetRenderDefault(md3dImmediateContext, BasicEffect::RenderObject);

	for (auto& wall : mWalls)
		wall.Draw(md3dImmediateContext, mBasicEffect);
	mCar.Draw(md3dImmediateContext, mBasicEffect);
	mGround.Draw(md3dImmediateContext, mBasicEffect);
	mHouse.Draw(md3dImmediateContext, mBasicEffect);

	mBasicEffect.SetRenderDefault(md3dImmediateContext, BasicEffect::RenderInstance);
	mTrees.DrawInstanced(md3dImmediateContext, mBasicEffect, refData);


	// ************************
	// 6. 绘制不透明正常物体的阴影
	//
	mCar.SetCarMaterial(mShadowMat);
	mHouse.SetMaterial(mShadowMat);

	mBasicEffect.SetShadowState(true);	// 反射关闭，阴影开启
	mBasicEffect.SetRenderNoDoubleBlend(md3dImmediateContext, 0, BasicEffect::RenderObject);

	mCar.Draw(md3dImmediateContext, mBasicEffect);
	mHouse.Draw(md3dImmediateContext, mBasicEffect);

	mBasicEffect.SetShadowState(false);		// 阴影关闭
	mCar.SetCarMaterial(mNormalMeterialMat);
	mHouse.SetMaterial(mNormalMeterialMat);

	// 绘制天空盒
	mSkyEffect.SetRenderDefault(md3dImmediateContext);
	switch (mSkyBoxMode)
	{
	case SkyBoxMode::Daylight: mDaylight->Draw(md3dImmediateContext, mSkyEffect, *mCamera); break;
	case SkyBoxMode::Sunset: mSunset->Draw(md3dImmediateContext, mSkyEffect, *mCamera); break;
	case SkyBoxMode::Desert: mDesert->Draw(md3dImmediateContext, mSkyEffect, *mCamera); break;
	}

	// ******************
	// 绘制Direct2D部分
	//
	if (md2dRenderTarget != nullptr)
	{
		md2dRenderTarget->BeginDraw();
		std::wstring text = L"切换摄像机模式: 1-第一人称 2-第三人称 3-自由视角\n"
			"W/S/A/D 前进/后退/左平移/右平移 (第三人称无效)  Esc退出\n"
			"鼠标移动控制视野 滚轮控制第三人称观察距离\n"
			"当前模式: ";
		if (mCameraMode == CameraMode::FirstPerson)
			text += L"第一人称(控制箱子移动)";
		else if (mCameraMode == CameraMode::ThirdPerson)
			text += L"第三人称";
		else
			text += L"自由视角";
		md2dRenderTarget->DrawTextW(text.c_str(), (UINT32)text.length(), mTextFormat.Get(),
			D2D1_RECT_F{ 0.0f, 0.0f, 600.0f, 200.0f }, mColorBrush.Get());
		HR(md2dRenderTarget->EndDraw());
	}

	HR(mSwapChain->Present(0, 0));
	
}



bool GameApp::InitResource()
{
	// 默认开启视锥体裁剪和硬件实例化
	mEnableInstancing = true;
	mEnableFrustumCulling = true;

	// ******************
	// 初始化天空盒相关

	
	mDaylight = std::make_unique<SkyRender>(
		md3dDevice, md3dImmediateContext,
		L"Texture\\daylight.jpg",
		5000.0f);
		
	mSunset = std::make_unique<SkyRender>(
		md3dDevice, md3dImmediateContext,
		std::vector<std::wstring>{
		L"Texture\\sunset_posX.bmp", L"Texture\\sunset_negX.bmp",
			L"Texture\\sunset_posY.bmp", L"Texture\\sunset_negY.bmp",
			L"Texture\\sunset_posZ.bmp", L"Texture\\sunset_negZ.bmp", },
		5000.0f);

	mDesert = std::make_unique<SkyRender>(
		md3dDevice, md3dImmediateContext,
		L"Texture\\desertcube1024.dds",
		5000.0f);
	
	mSkyBoxMode = SkyBoxMode::Daylight;

	// ******************
	// 初始化游戏对象
	ComPtr<ID3D11ShaderResourceView> texture;
	Material material;
	material.Ambient = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	material.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	material.Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 16.0f);

	mNormalMeterialMat = material;
	mShadowMat.Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mShadowMat.Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.5f);
	mShadowMat.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 16.0f);

	// 初始化汽车
	mCar.InitCar(md3dDevice);

	// 初始化地板
	mObjReader.Read(L"Model\\ground.mbo", L"Model\\ground.obj");
	mGround.SetModel(Model(md3dDevice, mObjReader));
	mGround.SetMaterial(material);

	// 初始化房屋模型
	mObjReader.Read(L"Model\\house.mbo", L"Model\\house.obj");
	mHouse.SetModel(Model(md3dDevice, mObjReader));
	mHouse.SetMaterial(material);
	// 获取房屋包围盒
	XMMATRIX S = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixTranslation(0.0f, 0.0f, -2.0f);
	BoundingBox houseBox = mHouse.GetLocalBoundingBox();
	houseBox.Transform(houseBox, S);
	// 让房屋底部紧贴地面
	mHouse.SetWorldMatrix(S * XMMatrixTranslation(0.0f, -(houseBox.Center.y - houseBox.Extents.y + 2.0f), 0.0f));

	// 初始化墙体
	mWalls.resize(5);
	HR(CreateDDSTextureFromFile(md3dDevice.Get(), L"Texture\\brick.dds", nullptr, texture.ReleaseAndGetAddressOf()));
	// 这里控制墙体五个面的生成，0和1的中间位置用于放置镜面
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
	mWalls[0].SetModel(Model(md3dDevice, Geometry::CreatePlane(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(6.0f, 8.0f), XMFLOAT2(1.5f, 2.0f))));
	mWalls[1].SetModel(Model(md3dDevice, Geometry::CreatePlane(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(6.0f, 8.0f), XMFLOAT2(1.5f, 2.0f))));
	mWalls[2].SetModel(Model(md3dDevice, Geometry::CreatePlane(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(20.0f, 8.0f), XMFLOAT2(5.0f, 2.0f))));
	mWalls[3].SetModel(Model(md3dDevice, Geometry::CreatePlane(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(20.0f, 8.0f), XMFLOAT2(5.0f, 2.0f))));
	mWalls[4].SetModel(Model(md3dDevice, Geometry::CreatePlane(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(20.0f, 8.0f), XMFLOAT2(5.0f, 2.0f))));

	mWalls[0].SetWorldMatrix(XMMatrixRotationX(-XM_PIDIV2) * XMMatrixTranslation(-7.0f, 3.0f, 10.0f));
	mWalls[1].SetWorldMatrix(XMMatrixRotationX(-XM_PIDIV2) * XMMatrixTranslation(7.0f, 3.0f, 10.0f));
	mWalls[2].SetWorldMatrix(XMMatrixRotationY(-XM_PIDIV2) * XMMatrixRotationZ(XM_PIDIV2) * XMMatrixTranslation(10.0f, 3.0f, 0.0f));
	mWalls[3].SetWorldMatrix(XMMatrixRotationX(XM_PIDIV2) * XMMatrixTranslation(0.0f, 3.0f, -10.0f));
	mWalls[4].SetWorldMatrix(XMMatrixRotationY(XM_PIDIV2) * XMMatrixRotationZ(-XM_PIDIV2) * XMMatrixTranslation(-10.0f, 3.0f, 0.0f));

	// 初始化镜面
	material.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);
	material.Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 16.0f);
	HR(CreateDDSTextureFromFile(md3dDevice.Get(), L"Texture\\ice.dds", nullptr, texture.ReleaseAndGetAddressOf()));
	mMirror.SetModel(Model(md3dDevice,
		Geometry::CreatePlane(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(8.0f, 8.0f), XMFLOAT2(1.0f, 1.0f))));
	mMirror.SetWorldMatrix(XMMatrixRotationX(-XM_PIDIV2) * XMMatrixTranslation(0.0f, 3.0f, 10.0f));
	mMirror.SetTexture(texture);
	mMirror.SetMaterial(material);

	// 创建随机的树
	CreateRandomTrees();

	// ******************
	// 初始化摄像机
	//
	/*
	// 初始化为第一人称摄像机
	mCameraMode = CameraMode::FirstPerson;
	auto camera = std::shared_ptr<FirstPersonCamera>(new FirstPersonCamera);
	mCamera = camera;
	camera->SetViewPort(0.0f, 0.0f, (float)mClientWidth, (float)mClientHeight);
	camera->LookAt(mCar.GetCarPosition(), mCar.GetCarDirection(), XMFLOAT3(0.0f, 1.0f, 0.0f));
	mCamera->UpdateViewMatrix();
	*/
	// 初始化为第三人称摄像机
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
	// 初始化不会变化的值
	//

	mBasicEffect.SetReflectionMatrix(XMMatrixReflect(XMVectorSet(0.0f, 0.0f, -1.0f, 10.0f)));
	// 稍微高一点位置以显示阴影
	mBasicEffect.SetShadowMatrix(XMMatrixShadow(XMVectorSet(0.0f, 1.0f, 0.0f, 0.99f), XMVectorSet(0.0f, 10.0f, -10.0f, 1.0f)));
	mBasicEffect.SetRefShadowMatrix(XMMatrixShadow(XMVectorSet(0.0f, 1.0f, 0.0f, 0.99f), XMVectorSet(0.0f, 10.0f, 30.0f, 1.0f)));

	// 环境光
	DirectionalLight dirLight;
	dirLight.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	dirLight.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	dirLight.Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	dirLight.Direction = XMFLOAT3(0.0f, -1.0f, 0.0f);
	mBasicEffect.SetDirLight(0, dirLight);
	// 灯光
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

void GameApp::CreateRandomTrees()
{
	srand((unsigned)time(nullptr));
	Material material;
	material.Ambient = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	material.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	material.Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 16.0f);

	// 初始化树
	mObjReader.Read(L"Model\\tree.mbo", L"Model\\tree.obj");
	mTrees.SetModel(Model(md3dDevice, mObjReader));
	mTrees.SetMaterial(material);
	XMMATRIX S = XMMatrixScaling(0.015f, 0.015f, 0.015f);

	BoundingBox treeBox = mTrees.GetLocalBoundingBox();
	// 获取树包围盒顶点
	mTreeBoxData = Collision::CreateBoundingBox(treeBox, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	// 让树木底部紧贴地面位于y = -2的平面
	treeBox.Transform(treeBox, S);
	XMMATRIX T0 = XMMatrixTranslation(0.0f, -(treeBox.Center.y - treeBox.Extents.y + 2.0f), 0.0f);
	/*
	// 随机生成256颗随机朝向的树
	float theta = 0.0f;
	for (int i = 0; i < 16; ++i)
	{
		// 取5-125的半径放置随机的树
		for (int j = 0; j < 4; ++j)
		{
			// 距离越远，树木越多
			for (int k = 0; k < 2 * j + 1; ++k)
			{
				float radius = (float)(rand() % 30 + 30 * j + 5);
				float randomRad = rand() % 256 / 256.0f * XM_2PI / 16;
				XMMATRIX T1 = XMMatrixTranslation(radius * cosf(theta + randomRad), 0.0f, radius * sinf(theta + randomRad));
				XMMATRIX R = XMMatrixRotationY(rand() % 256 / 256.0f * XM_2PI);
				XMMATRIX World = S * R * T0 * T1;
				mInstancedData.push_back(World);
			}
		}
		theta += XM_2PI / 16;
	}
	*/

	for (int i = 0; i < 256; ++i) {
		XMMATRIX T1 = XMMatrixTranslation(-5.0f, 0.0f, i * 2);
		XMMATRIX R = XMMatrixRotationY(rand() % 256 / 256.0f * XM_2PI);
		XMMATRIX World = S * R * T0 * T1;
		mInstancedData.push_back(World);
	}

}

