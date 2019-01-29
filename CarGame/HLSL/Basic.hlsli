#include "LightHelper.hlsli"

Texture2D gTex : register(t0);  // ����2D�������Ϣ
SamplerState gSam : register(s0);  // �������������״̬
/*
// ������ɫ���ĳ���������
cbuffer VSConstantBuffer : register(b0) {
	matrix gWorld;
	matrix gView;
	matrix gProj;
	matrix gWorldInvTranspose;  // ģ�;������ת�þ������ڽ���������ģ������ת�����������꣩
}

// Ƭ����ɫ���ĳ���������
cbuffer PSConstantBuffer : register(b1)
{
	DirectionalLight gDirLight[10];
	PointLight gPointLight[10];
	SpotLight gSpotLight[10];
	Material gMaterial;
	int gNumDirLight;  // ƽ�й�Դ����
	int gNumPointLight;  // ���Դ����
	int gNumSpotLight;  // �۹�ƹ�Դ����
	float3 gEyePosW;  // �۲�����꣨����������꣩
}
*/

cbuffer CBChangesEveryDrawing : register(b0)
{
	matrix gWorld;
	matrix gWorldInvTranspose;
	Material gMaterial;  // ��ͬ�����в�ͬ�Ĳ���
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
// ���嶥����ɫ������
struct VertexIn
{
	float3 PosL : POSITION;
	float3 NormalL : NORMAL;
	float4 Color : COLOR;
};

// ���嶥����ɫ�����
struct VertexOut
{
	float4 PosH : SV_POSITION;  // �ü��ռ�����ϵ�еĶ���λ��
	float3 PosW : POSITION;     // ��������ϵ�еĶ���λ��
	float3 NormalW : NORMAL;    // ��������ϵ�еķ�����
	float4 Color : COLOR;
};
*/

// 3D������ɫ�����루
struct VertexPosNormalTex {
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

// 3D������ɫ�����
struct VertexPosHWNormalTex
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