// Triangle.hlsli
// HLSL�ı�ͷ�ļ�

//���嶥����ɫ������
struct VertexIn {
	float3 pos : POSITION;
	float4 color : COLOR;
};

//���嶥����ɫ�����
struct VertexOut {
	float4 posH : SV_POSITION;
	float4 color : COLOR;
};