//***************************************************************************************
// �ṩһЩ��װ�õĶ������ײ��ⷽ��
//***************************************************************************************

#ifndef COLLISION_H
#define COLLISION_H

#include <DirectXCollision.h>
#include <vector>
#include "Vertex.h"

class Collision
{
public:

	// �߿򶥵�/��������
	struct WireFrameData
	{
		std::vector<VertexPosColor> vertexVec;		// ��������
		std::vector<WORD> indexVec;					// ��������
	};

	//
	// ��Χ���߿�Ĵ���
	//

	// ����AABB���߿�
	static WireFrameData CreateBoundingBox(const DirectX::BoundingBox& box, const DirectX::XMFLOAT4& color);
	// ����OBB���߿�
	static WireFrameData CreateBoundingOrientedBox(const DirectX::BoundingOrientedBox& box, const DirectX::XMFLOAT4& color);
	// ������Χ���߿�
	static WireFrameData CreateBoundingSphere(const DirectX::BoundingSphere& sphere, const DirectX::XMFLOAT4& color, int slices = 20);
	// ������׶���߿�
	static WireFrameData CreateBoundingFrustum(const DirectX::BoundingFrustum& frustum, const DirectX::XMFLOAT4& color);

	//
	// ���ֵȼ۵Ĳ�����׶��ü��ķ�������ȡ��������׶����ײ����ײ���Ӧ�������������
	//

	// ��׶��ü�
	static std::vector<DirectX::XMMATRIX> XM_CALLCONV FrustumCulling(
		const std::vector<DirectX::XMMATRIX>& Matrices, const DirectX::BoundingBox& localBox, DirectX::FXMMATRIX View, DirectX::CXMMATRIX Proj);
	// ��׶��ü�2
	static std::vector<DirectX::XMMATRIX> XM_CALLCONV FrustumCulling2(
		const std::vector<DirectX::XMMATRIX>& Matrices, const DirectX::BoundingBox& localBox, DirectX::FXMMATRIX View, DirectX::CXMMATRIX Proj);
	// ��׶��ü�3
	static std::vector<DirectX::XMMATRIX> XM_CALLCONV FrustumCulling3(
		const std::vector<DirectX::XMMATRIX>& Matrices, const DirectX::BoundingBox& localBox, DirectX::FXMMATRIX View, DirectX::CXMMATRIX Proj);

private:
	// ���ݰ�Χ�ж��㹹���Χ��
	static WireFrameData CreateFromCorners(const DirectX::XMFLOAT3(&corners)[8], const DirectX::XMFLOAT4& color);
};





#endif