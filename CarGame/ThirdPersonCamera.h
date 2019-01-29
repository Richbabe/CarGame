#ifndef THIRDPERSONCAMERA_H
#define THIRDPERSONCAMERA_H

#include "Camera.h"

class ThirdPersonCamera : public Camera {
public:
	ThirdPersonCamera();
	~ThirdPersonCamera() override;

	// ��ȡ��ǰ���������λ��
	DirectX::XMFLOAT3 GetTargetPosition() const;
	// ��ȡ������ľ���
	float GetDistance() const;
	// ��ȡ��X�����ת�Ƕ�
	float GetRotationX() const;
	// ��ȡ��Y�����ת�Ƕ�
	float GetRotationY() const;
	// �����崹ֱ��ת(ע��������Ұ�Ƕ�Phi������[pi/6, pi/2])
	void RotateX(float rad);
	// ������ˮƽ��ת
	void RotateY(float rad);
	// ��������
	void Approach(float dist);
	// ���ó�ʼ��X��Ļ���(ע��������Ұ�Ƕ�Phi������[pi/6, pi/2])
	void SetRotationX(float phi);
	// ���ó�ʼ��Y��Ļ���
	void SetRotationY(float theta);
	// ���ò��󶨴����������λ��
	void SetTarget(const DirectX::XMFLOAT3& target);
	// ���ó�ʼ����
	void SetDistance(float dist);
	// ������С����������
	void SetDistanceMinMax(float minDist, float maxDist);
	// ���¹۲����
	void UpdateViewMatrix() override;

private:
	DirectX::XMFLOAT3 mTarget;  // �����ٵ������λ��
	float mDistance;  // ���������ľ���
	// ��С������룬����������
	float mMinDist, mMaxDist;
	// ����������ϵΪ��׼����ǰ����ת�Ƕ�
	float mTheta;  // ��y�ᣨˮƽ����ת�Ķ���
	float mPhi;  // ��x�ᣨ��ֱ����ת�Ķ���
};

#endif