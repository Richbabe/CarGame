#include "Basic.hlsli"

// 3D像素着色器
float4 PS(VOut pIn) : SV_Target{
	// 提前进行裁剪，对不符合要求的像素可以避免后续运算
	float4 texColor = gTex.Sample(gSam, pIn.Tex);  // 纹理颜色
	clip(texColor.a - 0.1f);  // clip函数：如果texColor.a - 0.1f < 0 则丢弃该像素

	// 标准化法向量
	pIn.NormalW = normalize(pIn.NormalW);

	// 计算视线向量
	float3 toEyeW = normalize(pIn.ViewDir);

	// 初始化为0 
	float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 A = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 D = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 S = float4(0.0f, 0.0f, 0.0f, 0.0f);
	int i;

	// ********计算平行光源********
	// 强制展开循环以减少指令数
	[unroll]
	for (i = 0; i < 5; i++) {
		ComputeDirectionalLight(gMaterial, gDirLight[i], pIn.NormalW, toEyeW, A, D, S);
		ambient += A;
		diffuse += D;
		spec += S;
	}

	// ********计算点光源********
	// 强制展开循环以减少指令数
	[unroll]
	for (i = 0; i < 5; ++i)
	{
		PointLight pointLight = gPointLight[i];
		// 若当前在绘制反射物体，需要对光照进行反射矩阵变换
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

	// ********计算聚光灯光源********
	// 强制展开循环以减少指令数
	[unroll]
	for (i = 0; i < 5; ++i)
	{
		SpotLight spotLight = gSpotLight[i];
		// 若当前在绘制反射物体，需要对光照进行反射矩阵变换
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

	// 最终颜色混合

	float4 litColor = texColor * (ambient + diffuse) + spec;  // 纹理+光照
	litColor.a = texColor.a * gMaterial.Diffuse.a;

	return litColor;
}