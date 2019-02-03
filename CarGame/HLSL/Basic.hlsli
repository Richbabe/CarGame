#include "LightHelper.hlsli"

Texture2D gTex : register(t0);  // ����2D�������Ϣ
SamplerState gSam : register(s0);  // �������������״̬

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

// 3D������ɫ������
struct VIn {
	float3 PosL : POSITION;
	float3 NormalL : NORMAL;
	float2 Tex: TEXCOORD;
};

// 2D������ɫ������
struct VertexPosTex
{
	float3 PosL : POSITION;
	float2 Tex : TEXCOORD;
};

// 3Dʵ��������ɫ������
struct InstanceVIn {
	float3 PosL : POSITION;
	float3 NormalL : NORMAL;
	float2 Tex : TEXCOORD;
	matrix World : World;
	matrix WorldInvTranspose : WorldInvTranspose;
};

// 3D������ɫ�����
struct VOut
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;     // �������е�λ��
	float3 NormalW : NORMAL;    // �������������еķ���
	float2 Tex : TEXCOORD;
};

// 2D������ɫ�����
struct VertexPosHTex
{
	float4 PosH : SV_POSITION;
	float2 Tex : TEXCOORD;
};

