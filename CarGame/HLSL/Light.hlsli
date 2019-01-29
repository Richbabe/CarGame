#include "LightHelper.hlsli"

Texture2D gTex : register(t0);  // 保存2D纹理的信息
SamplerState gSamLinear : register(s0);  // 保存采样器采样状态

// 顶点着色器的常量缓冲区
cbuffer VSConstantBuffer : register(b0) {
	matrix gWorld;
	matrix gView;
	matrix gProj;
	matrix gWorldInvTranspose;  // 模型矩阵的逆转置矩阵（用于将法向量从模型坐标转换到世界坐标）
}

// 片段着色器的常量缓冲区
cbuffer PSConstantBuffer : register(b1)
{
	DirectionalLight gDirLight;
	PointLight gPointLight;
	SpotLight gSpotLight;
	Material gMaterial;
	float3 gEyePosW;  // 观察点坐标（即摄像机坐标）
}

// 定义顶点着色器输入
struct VertexIn
{
	float3 PosL : POSITION;
	float3 NormalL : NORMAL;
	float4 Color : COLOR;
};

// 定义顶点着色器输出
struct VertexOut
{
	float4 PosH : SV_POSITION;  // 裁剪空间坐标系中的顶点位置
	float3 PosW : POSITION;     // 世界坐标系中的顶点位置
	float3 NormalW : NORMAL;    // 世界坐标系中的法向量
	float4 Color : COLOR;
};
