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
	// �����ģʽ
	enum class CameraMode { FirstPerson, ThirdPerson, Free };

	// ��պ�ģʽ
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
	ComPtr<ID2D1SolidColorBrush> mColorBrush;	// ��ɫ��ˢ
	ComPtr<IDWriteFont> mFont;					// ����
	ComPtr<IDWriteTextFormat> mTextFormat;		// �ı���ʽ

	std::vector<GameObject> mWalls;							// ǽ��
	GameObject mMirror;										// ����
	Car mCar;                                                // ����
	GameObject mHouse;										// ����
	GameObject mGround;										// ����

	GameObject mTrees;										// ��
	std::vector<DirectX::XMMATRIX> mInstancedData;			// ����ʵ������
	Collision::WireFrameData mTreeBoxData;					// ����Χ���߿�����

	Material mShadowMat;									// ��Ӱ����
	Material mNormalMeterialMat;							// ��ͨ����

	BasicEffect mBasicEffect;								// ������Ⱦ��Ч����
	bool mEnableFrustumCulling;								// ��׶��ü�����
	bool mEnableInstancing;									// Ӳ��ʵ��������

	SkyEffect mSkyEffect;									// ��պ���Ч����
	std::unique_ptr<SkyRender> mDaylight;					// ��պ�(����)
	std::unique_ptr<SkyRender> mSunset;						// ��պ�(����)
	std::unique_ptr<SkyRender> mDesert;						// ��պ�(ɳĮ)
	SkyBoxMode mSkyBoxMode;									// ��պ�ģʽ

	std::shared_ptr<Camera> mCamera;						// �����
	CameraMode mCameraMode;									// �����ģʽ

	ObjReader mObjReader;									// ģ�Ͷ�ȡ����
};


#endif