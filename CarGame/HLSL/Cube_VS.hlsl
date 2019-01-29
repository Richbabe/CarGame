// Cube_VS.hlsl
#include "Cube.hlsli"

// 定义顶点着色器
VertexOut VS(VertexIn vIn) {
	VertexOut vOut;
	vOut.posH = mul(float4(vIn.posL, 1.0f), World);
	vOut.posH = mul(vOut.posH, View);
	vOut.posH = mul(vOut.posH, Proj);
	vOut.color = vIn.color;
	return vOut;
}