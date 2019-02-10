#include "Collision.h"
#include "d3dUtil.h"
using namespace DirectX;
using namespace Microsoft::WRL;


Collision::WireFrameData Collision::CreateBoundingBox(const DirectX::BoundingBox & box, const DirectX::XMFLOAT4 & color)
{
	XMFLOAT3 corners[8];
	box.GetCorners(corners);
	return CreateFromCorners(corners, color);
}

Collision::WireFrameData Collision::CreateBoundingOrientedBox(const DirectX::BoundingOrientedBox & box, const DirectX::XMFLOAT4 & color)
{
	XMFLOAT3 corners[8];
	box.GetCorners(corners);
	return CreateFromCorners(corners, color);
}

Collision::WireFrameData Collision::CreateBoundingSphere(const DirectX::BoundingSphere & sphere, const DirectX::XMFLOAT4 & color, int slices)
{
	WireFrameData data;
	XMVECTOR center = XMLoadFloat3(&sphere.Center), posVec;
	XMFLOAT3 pos;
	float theta = 0.0f;
	for (int i = 0; i < slices; ++i)
	{
		posVec = XMVector3Transform(center + XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f), XMMatrixRotationY(theta));
		XMStoreFloat3(&pos, posVec);
		data.vertexVec.push_back({ pos, color });
		posVec = XMVector3Transform(center + XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f), XMMatrixRotationZ(theta));
		XMStoreFloat3(&pos, posVec);
		data.vertexVec.push_back({ pos, color });
		posVec = XMVector3Transform(center + XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f), XMMatrixRotationX(theta));
		XMStoreFloat3(&pos, posVec);
		data.vertexVec.push_back({ pos, color });
		theta += XM_2PI / slices;
	}
	for (int i = 0; i < slices; ++i)
	{
		data.indexVec.push_back(i * 3);
		data.indexVec.push_back((i + 1) % slices * 3);

		data.indexVec.push_back(i * 3 + 1);
		data.indexVec.push_back((i + 1) % slices * 3 + 1);

		data.indexVec.push_back(i * 3 + 2);
		data.indexVec.push_back((i + 1) % slices * 3 + 2);
	}


	return data;
}

Collision::WireFrameData Collision::CreateBoundingFrustum(const DirectX::BoundingFrustum & frustum, const DirectX::XMFLOAT4 & color)
{
	XMFLOAT3 corners[8];
	frustum.GetCorners(corners);
	return CreateFromCorners(corners, color);
}

// ��׵��ü�����1
/*
	�������İ�Χ��λ������ֲ�����ϵ��������ͨ���������任����������ϵ��Ȼ��ͨ��ͶӰ�������������׶��λ��������ֲ�����ϵ�У�
	���۲�����ǽ�������������任����׵�����ڵľֲ�����ϵ�С���ˣ����ǿ���ʹ�ù۲���������󣬽���׵���Χ�б任����������ϵ�У�
	Ȼ������������ϵ�ж��������׵�������ײ��⡣
*/
std::vector<XMMATRIX> XM_CALLCONV Collision::FrustumCulling(
	const std::vector<XMMATRIX>& Matrices,const BoundingBox& localBox, FXMMATRIX View, CXMMATRIX Proj)
{
	std::vector<DirectX::XMMATRIX> acceptedData;  // ����׵���е�����

	BoundingFrustum frustum;  // ��׵��
	BoundingFrustum::CreateFromMatrix(frustum, Proj);  // ͨ��ͶӰ��������׵��
	XMMATRIX InvView = XMMatrixInverse(nullptr, View);  // ����۲����������
	// ����׶��Ӿֲ�����ϵ�任����������ϵ��
	frustum.Transform(frustum, InvView);

	BoundingOrientedBox localOrientedBox, orientedBox;
	BoundingOrientedBox::CreateFromBoundingBox(localOrientedBox, localBox);
	for (auto& mat : Matrices)
	{
		// �������Χ�дӾֲ�����ϵ�任����������ϵ��
		localOrientedBox.Transform(orientedBox, mat);
		// �ཻ���(����������ϵ��)
		if (frustum.Intersects(orientedBox))
			acceptedData.push_back(mat);
	}

	return acceptedData;
}


// ��׵��ü�����2
/*
	�����еĲü��������Թ۲���������������棬Ȼ��ʹ�ù۲������������׵�����������ϵת������������ϵ����ʹ���������������
	����׵�����������ϵת��������ľֲ�����ϵ�����������ľֲ�����ϵ�½�����׵����������ײ��⡣
*/
std::vector<DirectX::XMMATRIX> XM_CALLCONV Collision::FrustumCulling2(
	const std::vector<DirectX::XMMATRIX>& Matrices,const DirectX::BoundingBox& localBox, DirectX::FXMMATRIX View, DirectX::CXMMATRIX Proj)
{
	std::vector<DirectX::XMMATRIX> acceptedData;  // ����׵���е�����

	BoundingFrustum frustum, localFrustum;
	BoundingFrustum::CreateFromMatrix(frustum, Proj);  // ͨ��ͶӰ��������׵��
	XMMATRIX InvView = XMMatrixInverse(nullptr, View);  // ����۲����������
	for (auto& mat : Matrices)
	{
		XMMATRIX InvWorld = XMMatrixInverse(nullptr, mat);  // �����������������

		// ����׶��ӹ۲�����ϵ(��ֲ�����ϵ)�任���������ڵľֲ�����ϵ��
		frustum.Transform(localFrustum, InvView * InvWorld);
		// �ཻ��⣨������ľֲ�����ϵ�£�
		if (localFrustum.Intersects(localBox))
			acceptedData.push_back(mat);
	}

	return acceptedData;
}

// ��׵��ü�����3
/*
	ͨ�������������Ӿֲ�����ϵ�任����������ϵ����ͨ���۲�����������������ϵ�任����׵��ľֲ�����ϵ��
	�������׵��ľֲ�����ϵ�½����������׵�����ײ��⡣
*/
std::vector<DirectX::XMMATRIX> XM_CALLCONV Collision::FrustumCulling3(
	const std::vector<DirectX::XMMATRIX>& Matrices,const DirectX::BoundingBox& localBox, DirectX::FXMMATRIX View, DirectX::CXMMATRIX Proj)
{
	std::vector<DirectX::XMMATRIX> acceptedData;  // ����׵���е�����

	BoundingFrustum frustum;
	BoundingFrustum::CreateFromMatrix(frustum, Proj);  // ͨ��ͶӰ��������׵��

	BoundingOrientedBox localOrientedBox, orientedBox;
	BoundingOrientedBox::CreateFromBoundingBox(localOrientedBox, localBox);
	for (auto& mat : Matrices)
	{
		// ������������Χ�дӾֲ�����ϵ�任����׶�����ڵľֲ�����ϵ(�۲�����ϵ)��
		localOrientedBox.Transform(orientedBox, mat * View);
		// �ཻ��⣨����׵��ľֲ�����ϵ�£�
		if (frustum.Intersects(orientedBox))
			acceptedData.push_back(mat);
	}

	return acceptedData;
}

Collision::WireFrameData Collision::CreateFromCorners(const DirectX::XMFLOAT3(&corners)[8], const DirectX::XMFLOAT4 & color)
{
	WireFrameData data;
	// AABB/OBB������������    ��׶�嶥����������
	//     3_______2             4__________5
	//    /|      /|             |\        /|
	//  7/_|____6/ |             | \      / |
	//  |  |____|__|            7|_0\____/1_|6
	//  | /0    | /1              \ |    | /
	//  |/______|/                 \|____|/
	//  4       5                   3     2
	for (int i = 0; i < 8; ++i)
		data.vertexVec.push_back({ corners[i], color });

	data.indexVec.push_back(0);
	data.indexVec.push_back(1);

	data.indexVec.push_back(0);
	data.indexVec.push_back(4);

	data.indexVec.push_back(4);
	data.indexVec.push_back(5);

	data.indexVec.push_back(1);
	data.indexVec.push_back(5);

	data.indexVec.push_back(4);
	data.indexVec.push_back(7);

	data.indexVec.push_back(0);
	data.indexVec.push_back(3);

	data.indexVec.push_back(6);
	data.indexVec.push_back(5);

	data.indexVec.push_back(1);
	data.indexVec.push_back(2);

	data.indexVec.push_back(7);
	data.indexVec.push_back(3);

	data.indexVec.push_back(3);
	data.indexVec.push_back(2);

	data.indexVec.push_back(2);
	data.indexVec.push_back(6);

	data.indexVec.push_back(6);
	data.indexVec.push_back(7);

	return data;
}

void Collision::Draw(ComPtr<ID3D11Device> device, WireFrameData wireFrameData, ComPtr<ID3D11DeviceContext> deviceContext, BoundingBoxEffect& boundingBoxEffect, DirectX::FXMMATRIX WVP) {
	// ���㻺��������
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(VertexPosColor) * (UINT)wireFrameData.vertexVec.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = wireFrameData.vertexVec.data();

	HR(device->CreateBuffer(&vbd, &InitData, &mVertexBuffer));

	// ��������������
	mIndexCount = (UINT)wireFrameData.indexVec.size();

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(WORD) * mIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.StructureByteStride = 0;
	ibd.MiscFlags = 0;

	InitData.pSysMem = wireFrameData.indexVec.data();

	HR(device->CreateBuffer(&ibd, &InitData, &mIndexBuffer));
	
	UINT strides = sizeof(VertexPosColor);
	UINT offsets = 0;

	deviceContext->IASetVertexBuffers(0, 1, mVertexBuffer.GetAddressOf(), &strides, &offsets);
	deviceContext->IASetIndexBuffer(mIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

	// �������ݲ�Ӧ��
	boundingBoxEffect.SetWorldViewProjMatrix(WVP);
	boundingBoxEffect.Apply(deviceContext);

	deviceContext->DrawIndexed(mIndexCount, 0, 0);
}