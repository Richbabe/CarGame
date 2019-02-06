#include "FirstPersonCamera.h"
using namespace DirectX;

FirstPersonCamera::FirstPersonCamera()
	: Camera()
{
}

FirstPersonCamera::~FirstPersonCamera()
{
}

void FirstPersonCamera::SetPosition(float x, float y, float z)
{
	SetPosition(XMFLOAT3(x, y, z));
}

void FirstPersonCamera::SetPosition(const DirectX::XMFLOAT3& v) {
	mPosition = v;
}

void XM_CALLCONV FirstPersonCamera::LookAt(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR target, DirectX::FXMVECTOR up) {
	LookTo(pos, target - pos, up);
}

void FirstPersonCamera::LookAt(const DirectX::XMFLOAT3 & pos, const DirectX::XMFLOAT3 & target, const DirectX::XMFLOAT3 & up)
{
	LookAt(XMLoadFloat3(&pos), XMLoadFloat3(&target), XMLoadFloat3(&up));
}

void XM_CALLCONV FirstPersonCamera::LookTo(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR to, DirectX::FXMVECTOR up) {
	XMVECTOR Look = XMVector3Normalize(to);
	XMVECTOR Right = XMVector3Normalize(XMVector3Cross(up, Look));  // ������ = ǰ�������������
	XMVECTOR Up = XMVector3Normalize(XMVector3Cross(Look, Right));  // ���� = ǰ�������������

	XMStoreFloat3(&mPosition, pos);
	XMStoreFloat3(&mRight, Right);
	XMStoreFloat3(&mUp, Up);
	XMStoreFloat3(&mLook, Look);
}

void FirstPersonCamera::LookTo(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& to, const DirectX::XMFLOAT3& up) {
	LookTo(XMLoadFloat3(&pos), XMLoadFloat3(&to), XMLoadFloat3(&up));
}

void FirstPersonCamera::Pitch(float rad) {
	XMMATRIX rotateMatrix = XMMatrixRotationAxis(XMLoadFloat3(&mRight), rad);  // ������������������תrad�ȵ���ת����
	XMVECTOR Up = XMVector3TransformNormal(XMLoadFloat3(&mUp), rotateMatrix);  // ����ͨ����ת������ת�������
	XMVECTOR Look = XMVector3TransformNormal(XMLoadFloat3(&mLook), rotateMatrix);  // ����ͨ����ת������ת���ǰ����
	float cosPhi = XMVectorGetY(Look);  // ����������Ұ�Ƕȵ�����ֵ
	// ��������Ұ�ĽǶ�Phi������[2pi/9, 7pi/9]��������ֵ[-cos(2pi/9), cos(2pi/9)]֮��
	if (fabs(cosPhi) > cosf(XM_2PI / 9))
		return;

	XMStoreFloat3(&mUp, Up);
	XMStoreFloat3(&mLook, Look);
}

void FirstPersonCamera::RotateY(float rad) {
	XMMATRIX rotateMatrix = XMMatrixRotationY(rad);  // �����y����תrad�ȵ���ת����
	XMVECTOR Up = XMVector3TransformNormal(XMLoadFloat3(&mUp), rotateMatrix);  // ����ͨ����ת������ת�������
	XMVECTOR Look = XMVector3TransformNormal(XMLoadFloat3(&mLook), rotateMatrix);  // ����ͨ����ת������ת���ǰ����
	XMVECTOR Right = XMVector3TransformNormal(XMLoadFloat3(&mRight), rotateMatrix);  // ����ͨ����ת������ת���������

	XMStoreFloat3(&mUp, Up);
	XMStoreFloat3(&mLook, Look);
	XMStoreFloat3(&mRight, Right);
}

void FirstPersonCamera::UpdateViewMatrix() {
	XMVECTOR Right = XMLoadFloat3(&mRight);
	XMVECTOR Up = XMLoadFloat3(&mUp);
	XMVECTOR Look = XMLoadFloat3(&mLook);
	XMVECTOR Position = XMLoadFloat3(&mPosition);

	// ������������ụΪ�������ҳ��ȶ�Ϊ1
	Look = XMVector3Normalize(Look);
	Up = XMVector3Normalize(XMVector3Cross(Look, Right));

	if (XMVector3Equal(Up, XMVectorZero())) {
		Up = XMVectorSetY(Up, 1.0f);
	}

	// Up, Look�Ѿ�����������Ҫ�����Ӧ��˵õ�Right
	Right = XMVector3Cross(Up, Look);

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
	XMMATRIX viewMatrix = XMMatrixLookAtLH(Position, Position + Look, Up);
	XMStoreFloat4x4(&mView, viewMatrix);
}