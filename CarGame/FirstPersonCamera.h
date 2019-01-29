#ifndef FIRSTPERSONCAMERA_H
#define FIRSTPERSONCAMERA_H

#include "Camera.h"

class FirstPersonCamera : public Camera {
public:
	FirstPersonCamera();
	~FirstPersonCamera() override;

	// 设置摄像机位置
	void SetPosition(float x, float y, float z);
	void SetPosition(const DirectX::XMFLOAT3& v);

	// 设置摄像机的朝向
	void XM_CALLCONV LookAt(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR target, DirectX::FXMVECTOR up);
	void LookAt(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up);
	void XM_CALLCONV LookTo(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR to, DirectX::FXMVECTOR up);
	void LookTo(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& to, const DirectX::XMFLOAT3& up);

	// 左右平移
	void Strafe(float d);
	// 直行(平面移动)
	void Walk(float d);
	// 前进(朝前向移动,用于自由视角的时候)
	void MoveForward(float d);
	// 上下观察(绕摄像机右向量旋转)
	void Pitch(float rad);
	// 左右观察
	void RotateY(float rad);

	// 更新观察矩阵
	void UpdateViewMatrix() override;
};

#endif 
