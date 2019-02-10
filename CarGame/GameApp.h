#ifndef GAMEAPP_H
#define GAMEAPP_H

#include "d3dApp.h"
#include "Camera.h"
#include "FirstPersonCamera.h"
#include "ThirdPersonCamera.h"
#include "GameObject.h"
#include "Car.h"
#include "Collision.h"
#include "SkyRender.h"

class GameApp : public D3DApp
{
public:
	// 摄像机模式
	enum class CameraMode { FirstPerson, ThirdPerson, Free };

	// 天空盒模式
	enum class SkyBoxMode { Daylight, Sunset, Night };

public:
	GameApp(HINSTANCE hInstance);
	~GameApp();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

private:
	bool InitResource();                          // 初始化资源函数
	void InitMaterial();                          // 初始化材质
	void InitFirstPersonCamera();                 // 初始化第一人称摄像机
	void InitThirdPersonCamera();                 // 初始化第三人称摄像机
	void InitDayLight();                          // 初始化白天环境光
	void InitSunSetLight();                       // 初始化黄昏环境光
	void InitNightLight();                        // 初始化夜晚环境光
	void InitCarPort();                           // 初始化车库
	void InitHouse();                             // 初始化房子
	void InitCar();                               // 初始化汽车
	void InitSkyBox();                            // 初始化天空盒
	void InitTree();                              // 初始化树木
	void InitPlane();                             // 初始化地形

private:
	ComPtr<ID2D1SolidColorBrush> mColorBrush;	// 单色笔刷
	ComPtr<IDWriteFont> mFont;					// 字体
	ComPtr<IDWriteTextFormat> mTextFormat;		// 文本格式

	std::vector<GameObject> mWalls;							// 墙壁
	GameObject mMirror;										// 镜面
	Car mCar;                                               // 汽车
	GameObject mHouse;										// 房屋
	GameObject mGround;										// 地面
	GameObject mRoad;                                       // 道路

	DirectionalLight mDirectionalLight;                     // 平行环境光
	PointLight mCarPortLight;                               // 车库点光源
	PointLight mHouseLight;                                 // 房子点光源
	SpotLight mCarLeftLight;                                // 汽车左前灯
	SpotLight mCarRightLight;                               // 汽车右前灯     

	GameObject mTrees;										// 树
	std::vector<DirectX::XMMATRIX> mInstancedData;			// 树的实例数据
	Collision::WireFrameData mTreeBoxData;					// 树包围盒线框数据

	Material mShadowMat;									// 阴影材质
	Material mNormalMeterialMat;							// 普通材质

	BasicEffect mBasicEffect;								// 对象渲染特效管理

	SkyEffect mSkyEffect;									// 天空盒特效管理
	std::unique_ptr<SkyRender> mDaylight;					// 天空盒(白天)
	std::unique_ptr<SkyRender> mSunset;						// 天空盒(日落)
	std::unique_ptr<SkyRender> mNight;						// 天空盒(夜晚)
	SkyBoxMode mSkyBoxMode;									// 天空盒模式

	std::shared_ptr<Camera> mCamera;						// 摄像机
	CameraMode mCameraMode;									// 摄像机模式

	ObjReader mObjReader;									// 模型读取对象
};


#endif