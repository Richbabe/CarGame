#include "ThirdPersonCamera.h"
using namespace DirectX;

ThirdPersonCamera::ThirdPersonCamera()
	: Camera(), mTheta(XM_PI), mPhi(), mDistance(), mTarget()
{
}

ThirdPersonCamera::~ThirdPersonCamera()
{
}

XMFLOAT3 ThirdPersonCamera::GetTargetPosition() const{
	return mTarget;
}

float ThirdPersonCamera::GetDistance() const {
	return mDistance;
}

float ThirdPersonCamera::GetRotationX() const {
	return mPhi;
}

float ThirdPersonCamera::GetRotationY() const {
	return mTheta;
}

void ThirdPersonCamera::RotateX(float rad) {
	mPhi -= rad;
	// ��������Ұ�Ƕ�Phi������[pi/6, pi/2]��
	// ������ֵ[0, cos(pi/6)]֮��
	if (mPhi < XM_PI / 6)
		mPhi = XM_PI / 6;
	else if (mPhi > XM_PIDIV2)
		mPhi = XM_PIDIV2;
}

void ThirdPersonCamera::RotateY(float rad) {
	// ��������Ұ�Ƕ�mThetaͨ��XMScalarModAngle������[-pi, pi]
	mTheta = XMScalarModAngle(mTheta - rad);
}

void ThirdPersonCamera::Approach(float dist) {
	mDistance += dist;
	// ���ƾ�����[mMinDist, mMaxDist]֮��
	if (mDistance < mMinDist)
		mDistance = mMinDist;
	else if (mDistance > mMaxDist)
		mDistance = mMaxDist;
}

void ThirdPersonCamera::SetRotationX(float phi) {
	mPhi = XMScalarModAngle(phi);
	// ��������Ұ�Ƕ�Phi������[pi/6, pi/2]��
	// ������ֵ[0, cos(pi/6)]֮��
	if (mPhi < XM_PI / 6)
		mPhi = XM_PI / 6;
	else if (mPhi > XM_PIDIV2)
		mPhi = XM_PIDIV2;
}

void ThirdPersonCamera::SetRotationY(float theta) {
	mTheta = XMScalarModAngle(theta);
}

void ThirdPersonCamera::SetTarget(const DirectX::XMFLOAT3& target) {
	mTarget = target;

	// �����Ѽ������Ĵ��������
}

void ThirdPersonCamera::SetDistance(float dist)
{
	mDistance = dist;
}

void ThirdPersonCamera::SetDistanceMinMax(float minDist, float maxDist)
{
	mMinDist = minDist;
	mMaxDist = maxDist;
}

void ThirdPersonCamera::UpdateViewMatrix() {
	// ���ݱ������������������ϵ���������λ��
	float x = mTarget.x + mDistance * sinf(mPhi) * cosf(mTheta);
	float z = mTarget.z + mDistance * sinf(mPhi) * sinf(mTheta);
	float y = mTarget.y + mDistance * cosf(mPhi);
	mPosition = { x, y, z };
	

	// �̶������˳����λ�ã�����Ҫ֪������ĳ��򣡣���
	//mPosition = { mTarget.x, mTarget.y + 3.0f, mTarget.z - 3.0f};

	XMVECTOR Position = XMLoadFloat3(&mPosition);
	XMVECTOR Look = XMVector3Normalize(XMLoadFloat3(&mTarget) - Position);
	XMVECTOR Right = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), Look));
	XMVECTOR Up = XMVector3Normalize(XMVector3Cross(Look, Right));
	
	XMStoreFloat3(&mRight, Right);
	XMStoreFloat3(&mUp, Up);
	XMStoreFloat3(&mLook, Look);

	/*
	// ��������5.6.2�µĶ������۲����
	float x = -XMVectorGetX(XMVector3Dot(Position, Right));
	float y = -XMVectorGetX(XMVector3Dot(Position, Up));
	float z = -XMVectorGetX(XMVector3Dot(Position, Look));
	mView = {
		mRight.x, mUp.x, mLook.x, 0.0f,
		mRight.y, mUp.y, mLook.y, 0.0f,
		mRight.z, mUp.z, mLook.z, 0.0f,
		x, y, z, 1.0f
	};
	*/

	// ͨ��XMMatrixLookAtLH��������۲����
	XMMATRIX viewMatrix = XMMatrixLookAtLH(Position, XMLoadFloat3(&mTarget), Up);
	XMStoreFloat4x4(&mView, viewMatrix);
}