// BoudingBox_PS.hlsl
#include "BoudingBox.hlsli"

// �����Χ��������ɫ��
float4 PS(VOut pIn) : SV_Target
{
	return pIn.color;
}