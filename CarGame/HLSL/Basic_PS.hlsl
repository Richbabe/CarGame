#include "Basic.hlsli"

// 3D������ɫ��
float4 PS(VOut pIn) : SV_Target{
	// ��ǰ���вü����Բ�����Ҫ������ؿ��Ա����������
	float4 texColor = gTex.Sample(gSam, pIn.Tex);  // ������ɫ
	clip(texColor.a - 0.1f);  // clip���������texColor.a - 0.1f < 0 ����������

	// ��׼��������
	pIn.NormalW = normalize(pIn.NormalW);

	// ������������
	float3 toEyeW = normalize(pIn.ViewDir);

	// ��ʼ��Ϊ0 
	float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 A = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 D = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 S = float4(0.0f, 0.0f, 0.0f, 0.0f);
	int i;

	// ********����ƽ�й�Դ********
	// ǿ��չ��ѭ���Լ���ָ����
	[unroll]
	for (i = 0; i < 5; i++) {
		ComputeDirectionalLight(gMaterial, gDirLight[i], pIn.NormalW, toEyeW, A, D, S);
		ambient += A;
		diffuse += D;
		spec += S;
	}

	// ********������Դ********
	// ǿ��չ��ѭ���Լ���ָ����
	[unroll]
	for (i = 0; i < 5; ++i)
	{
		PointLight pointLight = gPointLight[i];
		// ����ǰ�ڻ��Ʒ������壬��Ҫ�Թ��ս��з������任
		[flatten]
		if (gIsReflection)
		{
			pointLight.Position = (float3) mul(float4(pointLight.Position, 1.0f), gReflection);
		}

		ComputePointLight(gMaterial, gPointLight[i], pIn.PosW, pIn.NormalW, toEyeW, A, D, S);
		ambient += A;
		diffuse += D;
		spec += S;
	}

	// ********����۹�ƹ�Դ********
	// ǿ��չ��ѭ���Լ���ָ����
	[unroll]
	for (i = 0; i < 5; ++i)
	{
		SpotLight spotLight = gSpotLight[i];
		// ����ǰ�ڻ��Ʒ������壬��Ҫ�Թ��ս��з������任
		[flatten]
		if (gIsReflection)
		{
			spotLight.Position = (float3) mul(float4(spotLight.Position, 1.0f), gReflection);
		}

		ComputeSpotLight(gMaterial, gSpotLight[i], pIn.PosW, pIn.NormalW, toEyeW, A, D, S);
		ambient += A;
		diffuse += D;
		spec += S;
	}

	// ������ɫ���

	float4 litColor = texColor * (ambient + diffuse) + spec;  // ����+����
	litColor.a = texColor.a * gMaterial.Diffuse.a;

	return litColor;
}