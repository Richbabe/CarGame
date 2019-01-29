#ifndef CAMERA_H
#define CAMERA_H

#include <d3d11_1.h>
#include <DirectXMath.h>

class Camera {
public:
	Camera();
	virtual ~Camera() = 0;

	// 获取摄像机位置
	DirectX::XMVECTOR GetPositionVector() const;
	DirectX::XMFLOAT3 GetPosition() const;

	// 获取摄像机的坐标轴向量
	DirectX::XMVECTOR GetRightVector() const;  // 获取右向量
	DirectX::XMFLOAT3 GetRight() const;
	DirectX::XMVECTOR GetUpVector() const;  // 获取上向量
	DirectX::XMFLOAT3 GetUp() const;
	DirectX::XMVECTOR GetLookVector() const;  // 获取视线向量
	DirectX::XMFLOAT3 GetLook() const;

	// 获取视锥体信息
	float GetNearWindowWidth() const;  // 获取近平面宽度
	float GetNearWindowHeight() const;  // 获取近平面高度
	float GetFarWindowWidth() const;  // 获取远平面宽度
	float GetFarWindowHeight() const;  // 获取远平面高度

	// 获取矩阵
	DirectX::XMMATRIX GetViewMatrix() const;  // 获取视角矩阵
	DirectX::XMMATRIX GetProjMatrix() const;  // 获取投影矩阵
	DirectX::XMMATRIX GetViewProjMatrix() const;  // 获取视角*投影矩阵

	// 获取视口
	D3D11_VIEWPORT GetViewPort() const;

	// 设置视锥体
	void SetFrustum(float fovY, float aspect, float nearZ, float farZ);

	// 设置视口
	void SetViewPort(const D3D11_VIEWPORT& viewPort);
	void SetViewPort(float topLeftX, float topLeftY, float width, float height, float minDepth = 0.0f, float maxDepth = 1.0f);

	// 更新观察矩阵（第一人称摄像机和第三人称摄像机分别实现此方法）
	virtual void UpdateViewMatrix() = 0;

protected:
	// 摄像机的观察空间坐标系对应在世界坐标系中的表示
	DirectX::XMFLOAT3 mPosition;
	DirectX::XMFLOAT3 mRight;
	DirectX::XMFLOAT3 mUp;
	DirectX::XMFLOAT3 mLook;

	// 视锥体属性
	float mNearZ;
	float mFarZ;
	float mAspect;  // 宽高比
	float mFovY;
	float mNearWindowHeight;
	float mFarWindowHeight;

	// 观察矩阵和透视投影矩阵
	DirectX::XMFLOAT4X4 mView;
	DirectX::XMFLOAT4X4 mProj;

	// 当前视口
	D3D11_VIEWPORT mViewPort;
};

#endif
