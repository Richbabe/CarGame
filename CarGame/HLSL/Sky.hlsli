// Sky.hlsli
// 用于渲染天空盒

TextureCube gTexCube : register(t0);
SamplerState sam : register(s0);

cbuffer CBChangesEveryFrame : register(b0)
{
	matrix gWorldViewProj;
}

// 顶点着色器输入
struct VIn {
	float3 PosL : POSITION;
};

// 顶点着色器输出
struct VOut {
	float4 PosH : SV_POSITION;
	float3 PosL : POSITION;
};