#include "Basic.hlsli"

// 3D实例顶点着色器
VOut VS(InstanceVIn vIn) {
	VOut vOut;

	matrix viewProj = mul(gView, gProj);
	vector posW = mul(float4(vIn.PosL, 1.0f), vIn.World);

	vOut.PosW = posW.xyz;
	vOut.PosH = mul(posW, viewProj);
	vOut.NormalW = mul(vIn.NormalL, (float3x3) vIn.WorldInvTranspose);
	vOut.Tex = vIn.Tex;
	return vOut;
}