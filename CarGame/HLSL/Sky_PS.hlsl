// Sky_PS.hlsl
#include "Sky.hlsli"

// ������պ�������ɫ��
float4 PS(VOut pIn) : SV_Target
{
	return gTexCube.Sample(sam, pIn.PosL);
}