// Cube_PS.hlsl
#include "Cube.hlsli"

// 定义像素着色器
float4 PS(VertexOut pIn) : SV_Target
{
	return pIn.color;
}