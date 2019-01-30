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

void GameObject::Draw(ComPtr<ID3D11DeviceContext> deviceContext, BasicEffect & effect)
{
	UINT strides = mModel.vertexStride;
	UINT offsets = 0;

	for (auto& part : mModel.modelParts)
	{
		// 设置顶点/索引缓冲区
		deviceContext->IASetVertexBuffers(0, 1, part.vertexBuffer.GetAddressOf(), &strides, &offsets);
		deviceContext->IASetIndexBuffer(part.indexBuffer.Get(), part.indexFormat, 0);

		// 更新数据并应用
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
