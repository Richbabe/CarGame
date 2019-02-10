// 存放光照参数结构体和不同类型光源的光照计算函数
// 光照模型为Blinn-Phong光照模型


// 方向光
struct DirectionalLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float3 Direction;
    float Pad;
};

// 点光
struct PointLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;

    float3 Position;
    float Range;

    float3 Att;
    float Pad;
};

// 聚光灯
struct SpotLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;

    float3 Position;
    float Range;

    float3 Direction;
    float Spot;

    float3 Att;
    float Pad;
};

// 物体表面材质
struct Material
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular; // w = SpecPower
    float4 Reflect;
};


// 计算Blinn-Phong光照模型下的平行光
void ComputeDirectionalLight(Material mat, DirectionalLight L,
	float3 normal, float3 toEye,
	out float4 ambient,
	out float4 diffuse,
	out float4 spec)
{
	// 初始化输出
    ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// 光向量与照射方向相反
    float3 lightVec = -L.Direction;

	// ********添加环境光********
    ambient = mat.Ambient * L.Ambient;

	// ********添加漫反射光和镜面光********
	// 计算漫反射因子
    float diffuseFactor = 0.5f * dot(lightVec, normal) + 0.5f;  // half-lambert
	
	// 计算半角向量（光线方向+视线方向，结果归一化）
	float3 halfDir = normalize(lightVec + toEye);

	// 计算Blinn-Phong镜面反射因子
    float specFactor = pow(max(dot(normal, halfDir), 0.0f), mat.Specular.w);

	// 计算漫反射光照强度
    diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;

	// 计算镜面反射光照强度
    spec = specFactor * mat.Specular * L.Specular;

}

// 计算Blinn-Phong光照模型下的点光
void ComputePointLight(Material mat, PointLight L, float3 pos, float3 normal, float3 toEye,
	out float4 ambient, out float4 diffuse, out float4 spec)
{
	// 初始化输出
    ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// 从表面到光源的向量
    float3 lightVec = L.Position - pos;

	// 表面到光线的距离
    float d = length(lightVec);

	// 灯光范围测试
    if (d > L.Range)
        return;

	// 标准化光向量
    lightVec /= d;

	// ********添加环境光********
	ambient = mat.Ambient * L.Ambient;

	// ********添加漫反射光和镜面光********
	// 计算漫反射因子
	float diffuseFactor = 0.5f * dot(lightVec, normal) + 0.5f;  // half-lambert

	// 计算半角向量（光线方向+视线方向，结果归一化）
	float3 halfDir = normalize(lightVec + toEye);

	// 计算Blinn-Phong镜面反射因子
	float specFactor = pow(max(dot(normal, halfDir), 0.0f), mat.Specular.w);

	// 计算漫反射光照强度
	diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;

	// 计算镜面反射光照强度
	spec = specFactor * mat.Specular * L.Specular;

	// 光的衰弱
    float att = 1.0f / dot(L.Att, float3(1.0f, d, d * d));

    diffuse *= att;
    spec *= att;
}


void ComputeSpotLight(Material mat, SpotLight L, float3 pos, float3 normal, float3 toEye,
	out float4 ambient, out float4 diffuse, out float4 spec)
{
	// 初始化输出
    ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// // 从表面到光源的向量
    float3 lightVec = L.Position - pos;

    // 表面到光源的距离
    float d = length(lightVec);

	// 范围测试
    if (d > L.Range)
        return;

	// 标准化光向量
    lightVec /= d;

	// ********添加环境光********
	ambient = mat.Ambient * L.Ambient;

	// ********添加漫反射光和镜面光********
	// 计算漫反射因子
	float diffuseFactor = 0.5f * dot(lightVec, normal) + 0.5f;  // half-lambert

	// 计算半角向量（光线方向+视线方向，结果归一化）
	float3 halfDir = normalize(lightVec + toEye);

	// 计算Blinn-Phong镜面反射因子
	float specFactor = pow(max(dot(normal, halfDir), 0.0f), mat.Specular.w);

	// 计算漫反射光照强度
	diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;

	// 计算镜面反射光照强度
	spec = specFactor * mat.Specular * L.Specular;

	// 计算汇聚因子和衰弱系数
    float spot = pow(max(dot(-lightVec, L.Direction), 0.0f), L.Spot);
    float att = spot / dot(L.Att, float3(1.0f, d, d * d));

    ambient *= spot;
    diffuse *= att;
    spec *= att;
}