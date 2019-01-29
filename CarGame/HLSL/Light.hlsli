#include "LightHelper.hlsli"

Texture2D gTex : register(t0);  // ����2D�������Ϣ
SamplerState gSamLinear : register(s0);  // �������������״̬

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
	DirectionalLight gDirLight;
	PointLight gPointLight;
	SpotLight gSpotLight;
	Material gMaterial;
	float3 gEyePosW;  // �۲�����꣨����������꣩
}

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
