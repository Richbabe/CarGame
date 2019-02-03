#ifndef GAMEAPP_H
#define GAMEAPP_H

#include "d3dApp.h"
#include "Camera.h"
#include "FirstPersonCamera.h"
#include "ThirdPersonCamera.h"
#include "GameObject.h"
#include "Car.h"
#include "Collision.h"

class GameApp : public D3DApp
{
public:
	// �����ģʽ
	enum class CameraMode { FirstPerson, ThirdPerson, Free };

public:
	GameApp(HINSTANCE hInstance);
	~GameApp();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

private:
	bool InitResource();
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

	std::shared_ptr<Camera> mCamera;						// �����
	CameraMode mCameraMode;									// �����ģʽ

	ObjReader mObjReader;									// ģ�Ͷ�ȡ����
};


#endif