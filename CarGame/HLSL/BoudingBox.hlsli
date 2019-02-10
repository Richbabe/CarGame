// BoundingBox.hlsli
// 用于渲染包围盒

cbuffer CBChangesEveryFrame : register(b0)
{
	matrix gWorldViewProj;
}

// 顶点着色器输入
struct VIn {
	float3 PosL : POSITION;
	float4 color : COLOR;
};

// 顶点着色器输出
struct VOut {
	float4 PosH : SV_POSITION;
	float4 color : COLOR;
};