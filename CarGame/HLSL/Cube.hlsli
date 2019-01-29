//Cube.hlsli

// 常量缓冲区，保存MVP矩阵
cbuffer ConstantBuffer : register(b0) {
	matrix World;  // matrix可以用float4x4替代。不加row_major的情况下，矩阵默认为列主矩阵，
	matrix View;   // 可以在前面添加row_major表示行主矩阵
	matrix Proj;   // 需要在C++代码端预先将矩阵进行转置。
}

// 定义顶点着色器输入
struct VertexIn {
	float3 posL : POSITION;
	float4 color : COLOR;
};

// 定义顶点着色器输出
struct VertexOut {
	float4 posH : SV_POSITION;
	float4 color : COLOR;
};