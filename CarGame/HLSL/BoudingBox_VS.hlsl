// BoudingBox_VS.hlsl
#include "BoudingBox.hlsli"

// �����Χ�ж�����ɫ��
VOut VS(VIn vIn) {
	VOut vOut;
	vOut.PosH = mul(float4(vIn.PosL, 1.0f), gWorldViewProj);
	vOut.color = vIn.color;                         // ����alphaͨ����ֵĬ��Ϊ1.0
	return vOut;
}