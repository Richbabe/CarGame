#include "Basic.hlsli"

// 3D������ɫ��
VertexPosHWNormalTex VS_3D(VertexPosNormalTex vIn) {
	VertexPosHWNormalTex vOut;
	matrix viewProj = mul(gView, gProj);
	float4 posW = mul(float4(vIn.PosL, 1.0f), gWorld);
	// ����ǰ�ڻ��Ʒ������壬�Ƚ��з������
	[flatten]
	if (gIsReflection)
	{
		posW = mul(posW, gReflection);
	}
	// ����ǰ�ڻ�����Ӱ���Ƚ���ͶӰ����
	[flatten]
	if (gIsShadow)
	{
		posW = (gIsReflection ? mul(posW, gRefShadow) : mul(posW, gShadow));
	}
	vOut.PosH = mul(posW, viewProj);  // �����ӿ�����
	vOut.PosW = posW.xyz;  // ������������
	vOut.NormalW = mul(vIn.NormalL, (float3x3) gWorldInvTranspose);  // ������������ϵ�µķ�����
	vOut.Tex = vIn.Tex;
	return vOut;
}