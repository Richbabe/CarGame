#ifndef CAMERA_H
#define CAMERA_H

#include <d3d11_1.h>
#include <DirectXMath.h>

class Camera {
public:
	Camera();
	virtual ~Camera() = 0;

	// ��ȡ�����λ��
	DirectX::XMVECTOR GetPositionVector() const;
	DirectX::XMFLOAT3 GetPosition() const;

	// ��ȡ�����������������
	DirectX::XMVECTOR GetRightVector() const;  // ��ȡ������
	DirectX::XMFLOAT3 GetRight() const;
	DirectX::XMVECTOR GetUpVector() const;  // ��ȡ������
	DirectX::XMFLOAT3 GetUp() const;
	DirectX::XMVECTOR GetLookVector() const;  // ��ȡ��������
	DirectX::XMFLOAT3 GetLook() const;

	// ��ȡ��׶����Ϣ
	float GetNearWindowWidth() const;  // ��ȡ��ƽ����
	float GetNearWindowHeight() const;  // ��ȡ��ƽ��߶�
	float GetFarWindowWidth() const;  // ��ȡԶƽ����
	float GetFarWindowHeight() const;  // ��ȡԶƽ��߶�

	// ��ȡ����
	DirectX::XMMATRIX GetViewMatrix() const;  // ��ȡ�ӽǾ���
	DirectX::XMMATRIX GetProjMatrix() const;  // ��ȡͶӰ����
	DirectX::XMMATRIX GetViewProjMatrix() const;  // ��ȡ�ӽ�*ͶӰ����

	// ��ȡ�ӿ�
	D3D11_VIEWPORT GetViewPort() const;

	// ������׶��
	void SetFrustum(float fovY, float aspect, float nearZ, float farZ);

	// �����ӿ�
	void SetViewPort(const D3D11_VIEWPORT& viewPort);
	void SetViewPort(float topLeftX, float topLeftY, float width, float height, float minDepth = 0.0f, float maxDepth = 1.0f);

	// ���¹۲���󣨵�һ�˳�������͵����˳�������ֱ�ʵ�ִ˷�����
	virtual void UpdateViewMatrix() = 0;

protected:
	// ������Ĺ۲�ռ�����ϵ��Ӧ����������ϵ�еı�ʾ
	DirectX::XMFLOAT3 mPosition;
	DirectX::XMFLOAT3 mRight;
	DirectX::XMFLOAT3 mUp;
	DirectX::XMFLOAT3 mLook;

	// ��׶������
	float mNearZ;
	float mFarZ;
	float mAspect;  // ��߱�
	float mFovY;
	float mNearWindowHeight;
	float mFarWindowHeight;

	// �۲�����͸��ͶӰ����
	DirectX::XMFLOAT4X4 mView;
	DirectX::XMFLOAT4X4 mProj;

	// ��ǰ�ӿ�
	D3D11_VIEWPORT mViewPort;
};

#endif
