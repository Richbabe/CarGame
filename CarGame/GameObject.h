#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Effects.h"
#include "Geometry.h"

// һ��������С����Ϸ������
class GameObject
{
public:
	// ʹ��ģ�����(C++11)��������
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	GameObject();

	// ��ȡλ��
	DirectX::XMFLOAT3 GetPosition() const;
	// ���û�����
	template<class VertexType, class IndexType>
	void SetBuffer(ComPtr<ID3D11Device> device, const Geometry::MeshData<VertexType, IndexType>& meshData);
	// ��������
	void SetTexture(ComPtr<ID3D11ShaderResourceView> texture);
	// ���ò���
	void SetMaterial(const Material & material);
	// ����ģ�;���
	void SetWorldMatrix(const DirectX::XMFLOAT4X4& world);
	void XM_CALLCONV SetWorldMatrix(DirectX::FXMMATRIX world);
	// ����
	void Draw(ComPtr<ID3D11DeviceContext> deviceContext, BasicEffect& effect);
private:
	DirectX::XMFLOAT4X4 mWorldMatrix;               // �������
	DirectX::XMFLOAT3 mWorldPosition;                    // ��������ϵ�е�λ��
	Material mMaterial;								// �������
	ComPtr<ID3D11ShaderResourceView> mTexture;      // ����
	ComPtr<ID3D11Buffer> mVertexBuffer;             // ���㻺����
	ComPtr<ID3D11Buffer> mIndexBuffer;              // ����������
	UINT mVertexStride;                             // �����ֽڴ�С
	UINT mIndexCount;                               // ������Ŀ 
};

template<class VertexType, class IndexType>
inline void GameObject::SetBuffer(ComPtr<ID3D11Device> device, const Geometry::MeshData<VertexType, IndexType>& meshData)
{
	// �ͷž���Դ
	mVertexBuffer.Reset();
	mIndexBuffer.Reset();

	// ���D3D�豸
	if (device == nullptr)
		return;

	// ���ö��㻺��������
	mVertexStride = sizeof(VertexType);
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = (UINT)meshData.vertexVec.size() * mVertexStride;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	// �½����㻺����
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = meshData.vertexVec.data();
	device->CreateBuffer(&vbd, &InitData, mVertexBuffer.GetAddressOf());

	// ������������������
	mIndexCount = (UINT)meshData.indexVec.size();
	D3D11_BUFFER_DESC ibd;
	ZeroMemory(&ibd, sizeof(ibd));
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = mIndexCount * sizeof(IndexType);
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	// �½�����������
	InitData.pSysMem = meshData.indexVec.data();
	device->CreateBuffer(&ibd, &InitData, mIndexBuffer.GetAddressOf());


}

#endif
