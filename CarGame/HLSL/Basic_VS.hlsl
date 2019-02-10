#include "Basic.hlsli"

// 3D顶点着色器
VOut VS(VIn vIn) {
	VOut vOut;
	matrix viewProj = mul(gView, gProj);
	float4 posW = mul(float4(vIn.PosL, 1.0f), gWorld);
	// 若当前在绘制反射物体，先进行反射操作
	[flatten]
	if (gIsReflection)
	{
		posW = mul(posW, gReflection);
	}
	// 若当前在绘制阴影，先进行投影操作
	[flatten]
	if (gIsShadow)
	{
		posW = (gIsReflection ? mul(posW, gRefShadow) : mul(posW, gShadow));
	}
	vOut.PosH = mul(posW, viewProj);  // 计算视口坐标
	vOut.PosW = posW.xyz;  // 计算世界坐标
	vOut.NormalW = mul(vIn.NormalL, (float3x3) gWorldInvTranspose);  // 计算世界坐标系下的法向量
	
	// 计算世界坐标系下的视线向量
	vOut.ViewDir = normalize(gEyePosW - vOut.PosW);

	vOut.Tex = vIn.Tex;
	return vOut;
}