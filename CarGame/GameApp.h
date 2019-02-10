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
	enum class SkyBoxMode { Daylight, Sunset, Night };

public:
	GameApp(HINSTANCE hInstance);
	~GameApp();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

private:
	bool InitResource();                          // ��ʼ����Դ����
	void InitMaterial();                          // ��ʼ������
	void InitFirstPersonCamera();                 // ��ʼ����һ�˳������
	void InitThirdPersonCamera();                 // ��ʼ�������˳������
	void InitDayLight();                          // ��ʼ�����컷����
	void InitSunSetLight();                       // ��ʼ���ƻ軷����
	void InitNightLight();                        // ��ʼ��ҹ������
	void InitCarPort();                           // ��ʼ������
	void InitHouse();                             // ��ʼ������
	void InitCar();                               // ��ʼ������
	void InitSkyBox();                            // ��ʼ����պ�
	void InitTree();                              // ��ʼ����ľ
	void InitPlane();                             // ��ʼ������

private:
	ComPtr<ID2D1SolidColorBrush> mColorBrush;	// ��ɫ��ˢ
	ComPtr<IDWriteFont> mFont;					// ����
	ComPtr<IDWriteTextFormat> mTextFormat;		// �ı���ʽ

	std::vector<GameObject> mWalls;							// ǽ��
	GameObject mMirror;										// ����
	Car mCar;                                               // ����
	GameObject mHouse;										// ����
	GameObject mGround;										// ����
	GameObject mRoad;                                       // ��·

	DirectionalLight mDirectionalLight;                     // ƽ�л�����
	PointLight mCarPortLight;                               // ������Դ
	PointLight mHouseLight;                                 // ���ӵ��Դ
	SpotLight mCarLeftLight;                                // ������ǰ��
	SpotLight mCarRightLight;                               // ������ǰ��     

	GameObject mTrees;										// ��
	std::vector<DirectX::XMMATRIX> mInstancedData;			// ����ʵ������
	Collision::WireFrameData mTreeBoxData;					// ����Χ���߿�����

	Material mShadowMat;									// ��Ӱ����
	Material mNormalMeterialMat;							// ��ͨ����

	BasicEffect mBasicEffect;								// ������Ⱦ��Ч����

	SkyEffect mSkyEffect;									// ��պ���Ч����
	std::unique_ptr<SkyRender> mDaylight;					// ��պ�(����)
	std::unique_ptr<SkyRender> mSunset;						// ��պ�(����)
	std::unique_ptr<SkyRender> mNight;						// ��պ�(ҹ��)
	SkyBoxMode mSkyBoxMode;									// ��պ�ģʽ

	std::shared_ptr<Camera> mCamera;						// �����
	CameraMode mCameraMode;									// �����ģʽ

	ObjReader mObjReader;									// ģ�Ͷ�ȡ����
};


#endif