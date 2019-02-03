#include "Collision.h"

using namespace DirectX;

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

// 视椎体裁剪方法1
/*
	如果物体的包围盒位于自身局部坐标系，将物体通过世界矩阵变换到世界坐标系。然后，通过投影矩阵构造出来的视锥体位于其自身局部坐标系中，
	而观察矩阵是将物体从世界矩阵变换到视椎体所在的局部坐标系中。因此，我们可以使用观察矩阵的逆矩阵，将视椎体包围盒变换到世界坐标系中，
	然后在世界坐标系中对物体和视椎体进行碰撞检测。
*/
std::vector<XMMATRIX> XM_CALLCONV Collision::FrustumCulling(
	const std::vector<XMMATRIX>& Matrices,const BoundingBox& localBox, FXMMATRIX View, CXMMATRIX Proj)
{
	std::vector<DirectX::XMMATRIX> acceptedData;  // 在视椎体中的物体

	BoundingFrustum frustum;  // 视椎体
	BoundingFrustum::CreateFromMatrix(frustum, Proj);  // 通过投影矩阵构造视椎体
	XMMATRIX InvView = XMMatrixInverse(nullptr, View);  // 计算观察矩阵的逆矩阵
	// 将视锥体从局部坐标系变换到世界坐标系中
	frustum.Transform(frustum, InvView);

	BoundingOrientedBox localOrientedBox, orientedBox;
	BoundingOrientedBox::CreateFromBoundingBox(localOrientedBox, localBox);
	for (auto& mat : Matrices)
	{
		// 将有向包围盒从局部坐标系变换到世界坐标系中
		localOrientedBox.Transform(orientedBox, mat);
		// 相交检测(在世界坐标系下)
		if (frustum.Intersects(orientedBox))
			acceptedData.push_back(mat);
	}

	return acceptedData;
}


// 视椎体裁剪方法2
/*
	龙书中的裁剪方法：对观察矩阵和世界矩阵求逆，然后使用观察矩阵的逆矩阵将视椎体从自身坐标系转换到世界坐标系，再使用世界矩阵的逆矩阵
	将视椎体从世界坐标系转换到物体的局部坐标系。最后在物体的局部坐标系下进行视椎体和物体的碰撞检测。
*/
std::vector<DirectX::XMMATRIX> XM_CALLCONV Collision::FrustumCulling2(
	const std::vector<DirectX::XMMATRIX>& Matrices,const DirectX::BoundingBox& localBox, DirectX::FXMMATRIX View, DirectX::CXMMATRIX Proj)
{
	std::vector<DirectX::XMMATRIX> acceptedData;  // 在视椎体中的物体

	BoundingFrustum frustum, localFrustum;
	BoundingFrustum::CreateFromMatrix(frustum, Proj);  // 通过投影矩阵构造视椎体
	XMMATRIX InvView = XMMatrixInverse(nullptr, View);  // 计算观察矩阵的逆矩阵
	for (auto& mat : Matrices)
	{
		XMMATRIX InvWorld = XMMatrixInverse(nullptr, mat);  // 计算世界矩阵的逆矩阵

		// 将视锥体从观察坐标系(或局部坐标系)变换到物体所在的局部坐标系中
		frustum.Transform(localFrustum, InvView * InvWorld);
		// 相交检测（在物体的局部坐标系下）
		if (localFrustum.Intersects(localBox))
			acceptedData.push_back(mat);
	}

	return acceptedData;
}

// 视椎体裁剪方法3
/*
	通过世界矩阵将物体从局部坐标系变换到世界坐标系，再通过观察矩阵将物体从世界坐标系变换到视椎体的局部坐标系，
	最后在视椎体的局部坐标系下进行物体和视椎体的碰撞检测。
*/
std::vector<DirectX::XMMATRIX> XM_CALLCONV Collision::FrustumCulling3(
	const std::vector<DirectX::XMMATRIX>& Matrices,const DirectX::BoundingBox& localBox, DirectX::FXMMATRIX View, DirectX::CXMMATRIX Proj)
{
	std::vector<DirectX::XMMATRIX> acceptedData;  // 在视椎体中的物体

	BoundingFrustum frustum;
	BoundingFrustum::CreateFromMatrix(frustum, Proj);  // 通过投影矩阵构造视椎体

	BoundingOrientedBox localOrientedBox, orientedBox;
	BoundingOrientedBox::CreateFromBoundingBox(localOrientedBox, localBox);
	for (auto& mat : Matrices)
	{
		// 将物体的有向包围盒从局部坐标系变换到视锥体所在的局部坐标系(观察坐标系)中
		localOrientedBox.Transform(orientedBox, mat * View);
		// 相交检测（在视椎体的局部坐标系下）
		if (frustum.Intersects(orientedBox))
			acceptedData.push_back(mat);
	}

	return acceptedData;
}

Collision::WireFrameData Collision::CreateFromCorners(const DirectX::XMFLOAT3(&corners)[8], const DirectX::XMFLOAT4 & color)
{
	WireFrameData data;
	// AABB/OBB顶点索引如下    视锥体顶点索引如下
	//     3_______2             4__________5
	//    /|      /|             |\        /|
	//  7/_|____6/ |             | \      / |
	//  |  |____|__|            7|_0\____/1_|6
	//  | /0    | /1              \ |    | /
	//  |/______|/                 \|____|/
	//  4       5                   3     2
	for (int i = 0; i < 8; ++i)
		data.vertexVec.push_back({ corners[i], color });
	for (int i = 0; i < 4; ++i)
	{
		data.indexVec.push_back(i);
		data.indexVec.push_back(i);

		data.indexVec.push_back(i);
		data.indexVec.push_back((i + 1) % 4);

		data.indexVec.push_back(i + 4);
		data.indexVec.push_back((i + 1) % 4 + 4);
	}
	return data;
}
