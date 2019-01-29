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
	// 将上下视野角度Phi限制在[pi/6, pi/2]，
	// 即余弦值[0, cos(pi/6)]之间
	if (mPhi < XM_PI / 6)
		mPhi = XM_PI / 6;
	else if (mPhi > XM_PIDIV2)
		mPhi = XM_PIDIV2;
}

void ThirdPersonCamera::RotateY(float rad) {
	// 将左右视野角度mTheta通过XMScalarModAngle限制在[-pi, pi]
	mTheta = XMScalarModAngle(mTheta - rad);
}

void ThirdPersonCamera::Approach(float dist) {
	mDistance += dist;
	// 限制距离在[mMinDist, mMaxDist]之间
	if (mDistance < mMinDist)
		mDistance = mMinDist;
	else if (mDistance > mMaxDist)
		mDistance = mMaxDist;
}

void ThirdPersonCamera::SetRotationX(float phi) {
	mPhi = XMScalarModAngle(phi);
	// 将上下视野角度Phi限制在[pi/6, pi/2]，
	// 即余弦值[0, cos(pi/6)]之间
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

	// 后续把计算距离的代码加在这
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
	// 根据被跟踪物体的球面坐标系计算摄像机位置
	float x = mTarget.x + mDistance * sinf(mPhi) * cosf(mTheta);
	float z = mTarget.z + mDistance * sinf(mPhi) * sinf(mTheta);
	float y = mTarget.y + mDistance * cosf(mPhi);
	mPosition = { x, y, z };
	

	// 固定第三人称相机位置（还需要知道物体的朝向！！）
	//mPosition = { mTarget.x, mTarget.y + 3.0f, mTarget.z - 3.0f};

	XMVECTOR Position = XMLoadFloat3(&mPosition);
	XMVECTOR Look = XMVector3Normalize(XMLoadFloat3(&mTarget) - Position);
	XMVECTOR Right = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), Look));
	XMVECTOR Up = XMVector3Normalize(XMVector3Cross(Look, Right));
	
	XMStoreFloat3(&mRight, Right);
	XMStoreFloat3(&mUp, Up);
	XMStoreFloat3(&mLook, Look);

	/*
	// 按照龙书5.6.2章的定义填充观察矩阵
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

	// 通过XMMatrixLookAtLH函数计算观察矩阵
	XMMATRIX viewMatrix = XMMatrixLookAtLH(Position, XMLoadFloat3(&mTarget), Up);
	XMStoreFloat4x4(&mView, viewMatrix);
}