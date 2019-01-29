#include "Camera.h"
using namespace DirectX;

Camera::Camera()
{
}

Camera::~Camera() {

}

XMVECTOR Camera::GetPositionVector() const {
	return XMLoadFloat3(&mPosition);
}

XMFLOAT3 Camera::GetPosition() const {
	return mPosition;
}

XMVECTOR Camera::GetRightVector() const {
	return XMLoadFloat3(&mRight);
}

XMFLOAT3 Camera::GetRight() const {
	return mRight;
}

XMVECTOR Camera::GetUpVector() const {
	return XMLoadFloat3(&mUp);
}

XMFLOAT3 Camera::GetUp() const {
	return mUp;
}

XMVECTOR Camera::GetLookVector() const {
	return XMLoadFloat3(&mLook);
}

XMFLOAT3 Camera::GetLook() const {
	return mLook;
}

float Camera::GetNearWindowWidth() const {
	return mAspect * mNearWindowHeight;
}

float Camera::GetNearWindowHeight() const {
	return mNearWindowHeight;
}

float Camera::GetFarWindowWidth() const {
	return mAspect * mFarWindowHeight;
}

float Camera::GetFarWindowHeight() const {
	return mFarWindowHeight;
}

XMMATRIX Camera::GetViewMatrix() const {
	return XMLoadFloat4x4(&mView);
}

XMMATRIX Camera::GetProjMatrix() const {
	return XMLoadFloat4x4(&mProj);
}

XMMATRIX Camera::GetViewProjMatrix() const {
	return XMLoadFloat4x4(&mView) * XMLoadFloat4x4(&mProj);
}

D3D11_VIEWPORT Camera::GetViewPort() const
{
	return mViewPort;
}

void Camera::SetFrustum(float fovY, float aspect, float nearZ, float farZ) {
	// 更新当前摄像机的视锥体属性
	mFovY = fovY;
	mAspect = aspect;
	mNearZ = nearZ;
	mFarZ = farZ;
	// 通过相似三角形计算近远平面的高度
	mNearWindowHeight = 2.0f * mNearZ * tanf(0.5f * mFovY);
	mFarWindowHeight = 2.0f * mFarZ * tanf(0.5f * mFovY);

	XMStoreFloat4x4(&mProj, XMMatrixPerspectiveFovLH(mFovY, mAspect, mNearZ, mFarZ));
}

void Camera::SetViewPort(const D3D11_VIEWPORT & viewPort)
{
	mViewPort = viewPort;
}

void Camera::SetViewPort(float topLeftX, float topLeftY, float width, float height, float minDepth, float maxDepth)
{
	mViewPort.TopLeftX = topLeftX;
	mViewPort.TopLeftY = topLeftY;
	mViewPort.Width = width;
	mViewPort.Height = height;
	mViewPort.MinDepth = minDepth;
	mViewPort.MaxDepth = maxDepth;
}