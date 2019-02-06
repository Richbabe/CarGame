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
	enum class SkyBoxMode { Daylight, Sunset, Desert };

public:
	GameApp(HINSTANCE hInstance);
	~GameApp();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

private:
	bool InitResource();
	void InitFirstPersonCamera();
	void InitThirdPersonCamera();
	void CreateRandomTrees();

private:
	ComPtr<ID2D1SolidColorBrush> mColorBrush;	// 单色笔刷
	ComPtr<IDWriteFont> mFont;					// 字体
	ComPtr<IDWriteTextFormat> mTextFormat;		// 文本格式

	std::vector<GameObject> mWalls;							// 墙壁
	GameObject mMirror;										// 镜面
	Car mCar;                                                // 汽车
	GameObject mHouse;										// 房屋
	GameObject mGround;										// 地面

	GameObject mTrees;										// 树
	std::vector<DirectX::XMMATRIX> mInstancedData;			// 树的实例数据
	Collision::WireFrameData mTreeBoxData;					// 树包围盒线框数据

	Material mShadowMat;									// 阴影材质
	Material mNormalMeterialMat;							// 普通材质

	BasicEffect mBasicEffect;								// 对象渲染特效管理
	bool mEnableFrustumCulling;								// 视锥体裁剪开启
	bool mEnableInstancing;									// 硬件实例化开启

	SkyEffect mSkyEffect;									// 天空盒特效管理
	std::unique_ptr<SkyRender> mDaylight;					// 天空盒(白天)
	std::unique_ptr<SkyRender> mSunset;						// 天空盒(日落)
	std::unique_ptr<SkyRender> mDesert;						// 天空盒(沙漠)
	SkyBoxMode mSkyBoxMode;									// 天空盒模式

	std::shared_ptr<Camera> mCamera;						// 摄像机
	CameraMode mCameraMode;									// 摄像机模式

	ObjReader mObjReader;									// 模型读取对象
};


#endif