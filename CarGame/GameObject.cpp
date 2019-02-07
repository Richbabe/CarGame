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

uint32_t GameObject::ColorRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	return (r | (g << 8) | (b << 16) | (a << 24));
}

void GameObject::InitTexture(ComPtr<ID3D11Device> md3dDevice) {
	uint32_t white = ColorRGBA(255, 255, 255, 255);

	// 纹理内存映射，用白色初始化
	std::vector<uint32_t> textureArrayMap(128 * 128, white);
	uint32_t(*textureMap)[128] = reinterpret_cast<uint32_t(*)[128]>(textureArrayMap.data());

	// 创建纹理数组
	D3D11_TEXTURE2D_DESC texArrayDesc;
	texArrayDesc.Width = 128;
	texArrayDesc.Height = 128;
	texArrayDesc.MipLevels = 1;
	texArrayDesc.ArraySize = 1;
	texArrayDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texArrayDesc.SampleDesc.Count = 1;      // 不使用多重采样
	texArrayDesc.SampleDesc.Quality = 0;
	texArrayDesc.Usage = D3D11_USAGE_DEFAULT;
	texArrayDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texArrayDesc.CPUAccessFlags = 0;
	texArrayDesc.MiscFlags = 0; // 指定需要生成mipmap

	D3D11_SUBRESOURCE_DATA sd;
	uint32_t * pData = textureArrayMap.data();
	sd.pSysMem = pData;
	sd.SysMemPitch = 128 * sizeof(uint32_t);
	sd.SysMemSlicePitch = 128 * 128 * sizeof(uint32_t);


	ComPtr<ID3D11Texture2D> tex;
	HR(md3dDevice->CreateTexture2D(&texArrayDesc, &sd, tex.GetAddressOf()));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	HR(md3dDevice->CreateShaderResourceView(tex.Get(), &srvDesc, mTexture.GetAddressOf()));
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
	// 设置实例缓冲区描述
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = (UINT)count * sizeof(InstancedData);
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	// 创建实例缓冲区
	HR(device->CreateBuffer(&vbd, nullptr, mInstancedBuffer.ReleaseAndGetAddressOf()));

	// 重新调整mCapacity
	mCapacity = count;
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

void GameObject::DrawInstanced(ComPtr<ID3D11DeviceContext> deviceContext, BasicEffect & effect, const std::vector<DirectX::XMMATRIX>& data)
{
	D3D11_MAPPED_SUBRESOURCE mappedData;
	UINT numInsts = (UINT)data.size();
	// 若传入的数据比实例缓冲区还大，需要重新分配
	if (numInsts > mCapacity)
	{
		ComPtr<ID3D11Device> device;
		deviceContext->GetDevice(device.GetAddressOf());
		ResizeBuffer(device, numInsts);
	}

	// ********开始实例缓冲区数据的修改********
	HR(deviceContext->Map(mInstancedBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));

	InstancedData * iter = reinterpret_cast<InstancedData *>(mappedData.pData);
	for (auto& mat : data)
	{
		iter->world = XMMatrixTranspose(mat);
		iter->worldInvTranspose = XMMatrixInverse(nullptr, mat);	// 两次转置抵消
		iter++;
	}

	deviceContext->Unmap(mInstancedBuffer.Get(), 0);
	// ********结束实例缓冲区数据的修改********

	UINT strides[2] = { mModel.vertexStride, sizeof(InstancedData) };
	UINT offsets[2] = { 0, 0 };
	ID3D11Buffer * buffers[2] = { nullptr, mInstancedBuffer.Get() };
	for (auto& part : mModel.modelParts)
	{
		buffers[0] = part.vertexBuffer.Get();

		// 设置顶点/索引缓冲区
		deviceContext->IASetVertexBuffers(0, 2, buffers, strides, offsets);
		deviceContext->IASetIndexBuffer(part.indexBuffer.Get(), part.indexFormat, 0);

		// 更新数据并应用
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