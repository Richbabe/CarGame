#include "Light.hlsli"

// ������ɫ��
float4 PS(VertexOut pIn) : SV_Target
{
	// ��׼��������
	pIn.NormalW = normalize(pIn.NormalW);

	// ����ָ���۾�������
	float3 toEyeW = normalize(gEyePosW - pIn.PosW);

	// ��ʼ��Ϊ0 
	float4 ambient, diffuse, spec;
	float4 A, D, S;
	ambient = diffuse = spec = A = D = S = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// ����ƽ�й�Դ
	ComputeDirectionalLight(gMaterial, gDirLight, pIn.NormalW, toEyeW, A, D, S);
	ambient += A;
	diffuse += D;
	spec += S;

	// ������Դ
	ComputePointLight(gMaterial, gPointLight, pIn.PosW, pIn.NormalW, toEyeW, A, D, S);
	ambient += A;
	diffuse += D;
	spec += S;

	// ����۹��
	ComputeSpotLight(gMaterial, gSpotLight, pIn.PosW, pIn.NormalW, toEyeW, A, D, S);
	ambient += A;
	diffuse += D;
	spec += S;

	float4 litColor = pIn.Color * (ambient + diffuse) + spec;

	litColor.a = gMaterial.Diffuse.a * pIn.Color.a;

	return litColor;
}