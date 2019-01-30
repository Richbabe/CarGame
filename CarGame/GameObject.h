#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Model.h"

// ��Ϸ������
class GameObject
{
public:
	// ʹ��ģ�����(C++11)��������
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	GameObject();

	// ��ȡλ��
	DirectX::XMFLOAT3 GetPosition() const;
	// ��������
	void SetTexture(ComPtr<ID3D11ShaderResourceView> texture);
	// ���ò���
	void SetMaterial(const Material & material);
	// ����ģ�;���
	void SetWorldMatrix(const DirectX::XMFLOAT4X4& world);
	void XM_CALLCONV SetWorldMatrix(DirectX::FXMMATRIX world);

	// ��ȡ��Χ��
	DirectX::BoundingBox GetLocalBoundingBox() const;                // ��ȡ�ֲ�����ϵ��AABB��Χ��
	DirectX::BoundingBox GetBoundingBox() const;                     // ��ȡ��������ϵ��AABB��Χ��
	DirectX::BoundingOrientedBox GetBoundingOrientedBox() const;     // ��ȡ��������ϵ��OBB��Χ��

	// ����ģ��
	void SetModel(Model&& model);
	void SetModel(const Model& model);


	// ����
	void Draw(ComPtr<ID3D11DeviceContext> deviceContext, BasicEffect& effect);
private:
	Model mModel;								    // ģ��
	DirectX::XMFLOAT3 mWorldPosition;               // ��������ϵ�µ�λ��
	DirectX::XMFLOAT4X4 mWorldMatrix;               // �������
	Material mMaterial;								// �������
	ComPtr<ID3D11ShaderResourceView> mTexture;      // ����
};

#endif
