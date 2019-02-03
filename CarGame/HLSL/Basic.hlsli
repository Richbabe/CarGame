#include "LightHelper.hlsli"

Texture2D gTex : register(t0);  // 保存2D纹理的信息
SamplerState gSam : register(s0);  // 保存采样器采样状态

cbuffer CBChangesEveryDrawing : register(b0)
{
	matrix gWorld;
	matrix gWorldInvTranspose;
	Material gMaterial;
}

cbuffer CBDrawingStates : register(b1)
{
	int gIsReflection;
	int gIsShadow;
	float2 gPad;
}

cbuffer CBChangesEveryFrame : register(b2)
{
	matrix gView;
	float3 gEyePosW;
	float gPad2;
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

// 3D顶点着色器输入
struct VIn {
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

// 3D实例顶点着色器输入
struct InstanceVIn {
	float3 PosL : POSITION;
	float3 NormalL : NORMAL;
	float2 Tex : TEXCOORD;
	matrix World : World;
	matrix WorldInvTranspose : WorldInvTranspose;
};

// 3D顶点着色器输出
struct VOut
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

