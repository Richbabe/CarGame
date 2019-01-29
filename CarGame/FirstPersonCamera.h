#ifndef FIRSTPERSONCAMERA_H
#define FIRSTPERSONCAMERA_H

#include "Camera.h"

class FirstPersonCamera : public Camera {
public:
	FirstPersonCamera();
	~FirstPersonCamera() override;

	// ���������λ��
	void SetPosition(float x, float y, float z);
	void SetPosition(const DirectX::XMFLOAT3& v);

	// ����������ĳ���
	void XM_CALLCONV LookAt(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR target, DirectX::FXMVECTOR up);
	void LookAt(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up);
	void XM_CALLCONV LookTo(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR to, DirectX::FXMVECTOR up);
	void LookTo(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& to, const DirectX::XMFLOAT3& up);

	// ����ƽ��
	void Strafe(float d);
	// ֱ��(ƽ���ƶ�)
	void Walk(float d);
	// ǰ��(��ǰ���ƶ�,���������ӽǵ�ʱ��)
	void MoveForward(float d);
	// ���¹۲�(���������������ת)
	void Pitch(float rad);
	// ���ҹ۲�
	void RotateY(float rad);

	// ���¹۲����
	void UpdateViewMatrix() override;
};

#endif 
