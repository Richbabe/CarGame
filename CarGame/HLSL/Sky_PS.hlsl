// Sky_PS.hlsl
#include "Sky.hlsli"

// 定义天空盒像素着色器
float4 PS(VOut pIn) : SV_Target
{
	return gTexCube.Sample(sam, pIn.PosL);
}