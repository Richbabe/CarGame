// BoundingBox.hlsli
// ������Ⱦ��Χ��

cbuffer CBChangesEveryFrame : register(b0)
{
	matrix gWorldViewProj;
}

// ������ɫ������
struct VIn {
	float3 PosL : POSITION;
	float4 color : COLOR;
};

// ������ɫ�����
struct VOut {
	float4 PosH : SV_POSITION;
	float4 color : COLOR;
};