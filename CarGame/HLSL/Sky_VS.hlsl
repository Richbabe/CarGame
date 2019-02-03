// Sky_VS.hlsl
#include "Sky.hlsli"

// ������պж�����ɫ��
VOut VS(VIn vIn) {
	VOut vOut;

	float4 posH = mul(float4(vIn.PosL, 1.0f), gWorldViewProj);
	// ����z = wʹ��z/w = 1������պ�ʼ�ձ�����Զƽ�棩
	vOut.PosH = posH.xyww;
	vOut.PosL = vIn.PosL;
	return vOut;
}