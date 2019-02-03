// Sky_VS.hlsl
#include "Sky.hlsli"

// 定义天空盒顶点着色器
VOut VS(VIn vIn) {
	VOut vOut;

	float4 posH = mul(float4(vIn.PosL, 1.0f), gWorldViewProj);
	// 设置z = w使得z/w = 1（即天空盒始终保持在远平面）
	vOut.PosH = posH.xyww;
	vOut.PosL = vIn.PosL;
	return vOut;
}