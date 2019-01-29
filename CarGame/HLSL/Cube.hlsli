//Cube.hlsli

// ����������������MVP����
cbuffer ConstantBuffer : register(b0) {
	matrix World;  // matrix������float4x4���������row_major������£�����Ĭ��Ϊ��������
	matrix View;   // ������ǰ�����row_major��ʾ��������
	matrix Proj;   // ��Ҫ��C++�����Ԥ�Ƚ��������ת�á�
}

// ���嶥����ɫ������
struct VertexIn {
	float3 posL : POSITION;
	float4 color : COLOR;
};

// ���嶥����ɫ�����
struct VertexOut {
	float4 posH : SV_POSITION;
	float4 color : COLOR;
};