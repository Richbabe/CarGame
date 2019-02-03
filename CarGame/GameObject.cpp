#include "GameObject.h"
#include "d3dUtil.h"
using namespace DirectX;

struct InstancedData
{
	XMMATRIX world;
	XMMATRIX worldInvTranspose;
};

GameObject::GameObject() :mWorldMatrix(
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f),
	mWorldPosition(XMFLOAT3(0.0f, 0.0f, 0.0f))
{
	
}

XMFLOAT3 GameObject::GetPosition() const {
	return XMFLOAT3(mWorldMatrix(3, 0), mWorldMatrix(3, 1), mWorldMatrix(3, 2));
}

void GameObject::SetTexture(ComPtr<ID3D11ShaderResourceView> texture) {
	if (texture == nullptr) {
		throw ERROR("aaaaaa");
	}
	mTexture = texture;
}

void GameObject::SetMaterial(const Material & material)
{
	mMaterial = material;
}

void GameObject::SetWorldMatrix(const XMFLOAT4X4 & world)
{
	mWorldMatrix = world;
	mWorldPosition = XMFLOAT3(mWorldMatrix(3, 0), mWorldMatrix(3, 1), mWorldMatrix(3, 2));
}

void XM_CALLCONV GameObject::SetWorldMatrix(FXMMATRIX world)
{
	XMStoreFloat4x4(&mWorldMatrix, world);
}

BoundingBox GameObject::GetLocalBoundingBox() const
{
	return mModel.boundingBox;
}

BoundingBox GameObject::GetBoundingBox() const
{
	BoundingBox box;
	mModel.boundingBox.Transform(box, XMLoadFloat4x4(&mWorldMatrix));
	return box;
}

BoundingOrientedBox GameObject::GetBoundingOrientedBox() const
{
	BoundingOrientedBox box;
	BoundingOrientedBox::CreateFromBoundingBox(box, mModel.boundingBox);
	box.Transform(box, XMLoadFloat4x4(&mWorldMatrix));
	return box;
}

void GameObject::SetModel(Model && model)
{
	std::swap(mModel, model);
	model.modelParts.clear();
	model.boundingBox = BoundingBox();
}

void GameObject::SetModel(const Model & model)
{
	mModel = model;
}

size_t GameObject::GetCapacity() const
{
	return mCapacity;
}

void GameObject::ResizeBuffer(ComPtr<ID3D11Device> device, size_t count)
{
	// ����ʵ������������
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = (UINT)count * sizeof(InstancedData);
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	// ����ʵ��������
	HR(device->CreateBuffer(&vbd, nullptr, mInstancedBuffer.ReleaseAndGetAddressOf()));

	// ���µ���mCapacity
	mCapacity = count;
}

void GameObject::Draw(ComPtr<ID3D11DeviceContext> deviceContext, BasicEffect & effect)
{
	UINT strides = mModel.vertexStride;
	UINT offsets = 0;

	for (auto& part : mModel.modelParts)
	{
		// ���ö���/����������
		deviceContext->IASetVertexBuffers(0, 1, part.vertexBuffer.GetAddressOf(), &strides, &offsets);
		deviceContext->IASetIndexBuffer(part.indexBuffer.Get(), part.indexFormat, 0);

		// �������ݲ�Ӧ��
		effect.SetWorldMatrix(XMLoadFloat4x4(&mWorldMatrix));
		if (part.texDiffuse) {
			effect.SetTextureDiffuse(part.texDiffuse);
		}
		else {
			if (mTexture) {
				effect.SetTextureDiffuse(mTexture);
			}
		}
		effect.SetMaterial(mMaterial);

		effect.Apply(deviceContext);

		deviceContext->DrawIndexed(part.indexCount, 0, 0);
	}
}

void GameObject::DrawInstanced(ComPtr<ID3D11DeviceContext> deviceContext, BasicEffect & effect, const std::vector<DirectX::XMMATRIX>& data)
{
	D3D11_MAPPED_SUBRESOURCE mappedData;
	UINT numInsts = (UINT)data.size();
	// ����������ݱ�ʵ��������������Ҫ���·���
	if (numInsts > mCapacity)
	{
		ComPtr<ID3D11Device> device;
		deviceContext->GetDevice(device.GetAddressOf());
		ResizeBuffer(device, numInsts);
	}

	// ********��ʼʵ�����������ݵ��޸�********
	HR(deviceContext->Map(mInstancedBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));

	InstancedData * iter = reinterpret_cast<InstancedData *>(mappedData.pData);
	for (auto& mat : data)
	{
		iter->world = XMMatrixTranspose(mat);
		iter->worldInvTranspose = XMMatrixInverse(nullptr, mat);	// ����ת�õ���
		iter++;
	}

	deviceContext->Unmap(mInstancedBuffer.Get(), 0);
	// ********����ʵ�����������ݵ��޸�********

	UINT strides[2] = { mModel.vertexStride, sizeof(InstancedData) };
	UINT offsets[2] = { 0, 0 };
	ID3D11Buffer * buffers[2] = { nullptr, mInstancedBuffer.Get() };
	for (auto& part : mModel.modelParts)
	{
		buffers[0] = part.vertexBuffer.Get();

		// ���ö���/����������
		deviceContext->IASetVertexBuffers(0, 2, buffers, strides, offsets);
		deviceContext->IASetIndexBuffer(part.indexBuffer.Get(), part.indexFormat, 0);

		// �������ݲ�Ӧ��
		if (part.texDiffuse) {
			effect.SetTextureDiffuse(part.texDiffuse);
		}
		else {
			if (mTexture) {
				effect.SetTextureDiffuse(mTexture);
			}
		}
		effect.SetMaterial(mMaterial);

		effect.Apply(deviceContext);

		deviceContext->DrawIndexedInstanced(part.indexCount, numInsts, 0, 0, 0);
	}
}