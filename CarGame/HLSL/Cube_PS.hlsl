// Cube_PS.hlsl
#include "Cube.hlsli"

// ����������ɫ��
float4 PS(VertexOut pIn) : SV_Target
{
	return pIn.color;
}