// Triangle.hlsli
// HLSL的标头文件

//定义顶点着色器输入
struct VertexIn {
	float3 pos : POSITION;
	float4 color : COLOR;
};

//定义顶点着色器输出
struct VertexOut {
	float4 posH : SV_POSITION;
	float4 color : COLOR;
};