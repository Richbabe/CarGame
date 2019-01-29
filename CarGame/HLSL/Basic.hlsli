#include "LightHelper.hlsli"

Texture2D gTex : register(t0);  // 保存2D纹理的信息
SamplerState gSam : register(s0);  // 保存采样器采样状态
/*
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
	DirectionalLight gDirLight[10];
	PointLight gPointLight[10];
	SpotLight gSpotLight[10];
	Material gMaterial;
	int gNumDirLight;  // 平行光源数量
	int gNumPointLight;  // 点光源数量
	int gNumSpotLight;  // 聚光灯光源数量
	float3 gEyePosW;  // 观察点坐标（即摄像机坐标）
}
*/

cbuffer CBChangesEveryDrawing : register(b0)
{
	matrix gWorld;
	matrix gWorldInvTranspose;
	Material gMaterial;  // 不同物体有不同的材质
}

cbuffer CBDrawingStates : register(b1)
{
	int gIsReflection;
	int gIsShadow;
}

cbuffer CBChangesEveryFrame : register(b2)
{
	matrix gView;
	float3 gEyePosW;
}

cbuffer CBChangesOnResize : register(b3)
{
	matrix gProj;
}

cbuffer CBChangesRarely : register(b4)
{
	matrix gReflection;
	matrix gShadow;
	matrix gRefShadow;
	DirectionalLight gDirLight[5];
	PointLight gPointLight[5];
	SpotLight gSpotLight[5];
}

/*
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
*/

// 3D顶点着色器输入（
struct VertexPosNormalTex {
	float3 PosL : POSITION;
	float3 NormalL : NORMAL;
	float2 Tex: TEXCOORD;
};

// 2D顶点着色器输入
struct VertexPosTex
{
	float3 PosL : POSITION;
	float2 Tex : TEXCOORD;
};

// 3D顶点着色器输出
struct VertexPosHWNormalTex
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;     // 在世界中的位置
	float3 NormalW : NORMAL;    // 法向量在世界中的方向
	float2 Tex : TEXCOORD;
};

// 2D顶点着色器输出
struct VertexPosHTex
{
	float4 PosH : SV_POSITION;
	float2 Tex : TEXCOORD;
};