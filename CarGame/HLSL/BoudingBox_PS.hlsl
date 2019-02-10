// BoudingBox_PS.hlsl
#include "BoudingBox.hlsli"

// 定义包围盒像素着色器
float4 PS(VOut pIn) : SV_Target
{
	return pIn.color;
}