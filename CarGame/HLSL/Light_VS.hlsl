#include "Light.hlsli"

// ���嶥����ɫ��
VertexOut VS(VertexIn vIn) {
	VertexOut vOut;
	matrix viewProj = mul(gView, gProj);
	float4 posW = mul(float4(vIn.PosL, 1.0f), gWorld);

	vOut.PosH = mul(posW, viewProj);  // ����ü��ռ�����ϵ�µĶ���λ��
	vOut.PosW = posW.xyz;  // ������������ϵ�µĶ���λ��
	vOut.NormalW = mul(vIn.NormalL, (float3x3) gWorldInvTranspose);  // ������������ϵ�µķ�����
	vOut.Color = vIn.Color; // ����alphaͨ����ֵĬ��Ϊ1.0
	return vOut;
}