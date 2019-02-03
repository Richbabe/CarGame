// Sky.hlsli
// ������Ⱦ��պ�

TextureCube gTexCube : register(t0);
SamplerState sam : register(s0);

cbuffer CBChangesEveryFrame : register(b0)
{
	matrix gWorldViewProj;
}

// ������ɫ������
struct VIn {
	float3 PosL : POSITION;
};

// ������ɫ�����
struct VOut {
	float4 PosH : SV_POSITION;
	float3 PosL : POSITION;
};