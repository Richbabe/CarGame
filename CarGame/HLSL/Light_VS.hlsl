#include "Light.hlsli"

// 定义顶点着色器
VertexOut VS(VertexIn vIn) {
	VertexOut vOut;
	matrix viewProj = mul(gView, gProj);
	float4 posW = mul(float4(vIn.PosL, 1.0f), gWorld);

	vOut.PosH = mul(posW, viewProj);  // 计算裁剪空间坐标系下的顶点位置
	vOut.PosW = posW.xyz;  // 计算世界坐标系下的顶点位置
	vOut.NormalW = mul(vIn.NormalL, (float3x3) gWorldInvTranspose);  // 计算世界坐标系下的法向量
	vOut.Color = vIn.Color; // 这里alpha通道的值默认为1.0
	return vOut;
}