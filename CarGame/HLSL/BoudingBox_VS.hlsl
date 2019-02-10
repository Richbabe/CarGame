// BoudingBox_VS.hlsl
#include "BoudingBox.hlsli"

// 定义包围盒顶点着色器
VOut VS(VIn vIn) {
	VOut vOut;
	vOut.PosH = mul(float4(vIn.PosL, 1.0f), gWorldViewProj);
	vOut.color = vIn.color;                         // 这里alpha通道的值默认为1.0
	return vOut;
}