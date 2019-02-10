// ��Ź��ղ����ṹ��Ͳ�ͬ���͹�Դ�Ĺ��ռ��㺯��
// ����ģ��ΪBlinn-Phong����ģ��


// �����
struct DirectionalLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float3 Direction;
    float Pad;
};

// ���
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

// �۹��
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

// ����������
struct Material
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular; // w = SpecPower
    float4 Reflect;
};


// ����Blinn-Phong����ģ���µ�ƽ�й�
void ComputeDirectionalLight(Material mat, DirectionalLight L,
	float3 normal, float3 toEye,
	out float4 ambient,
	out float4 diffuse,
	out float4 spec)
{
	// ��ʼ�����
    ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// �����������䷽���෴
    float3 lightVec = -L.Direction;

	// ********��ӻ�����********
    ambient = mat.Ambient * L.Ambient;

	// ********����������;����********
	// ��������������
    float diffuseFactor = 0.5f * dot(lightVec, normal) + 0.5f;  // half-lambert
	
	// ���������������߷���+���߷��򣬽����һ����
	float3 halfDir = normalize(lightVec + toEye);

	// ����Blinn-Phong���淴������
    float specFactor = pow(max(dot(normal, halfDir), 0.0f), mat.Specular.w);

	// �������������ǿ��
    diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;

	// ���㾵�淴�����ǿ��
    spec = specFactor * mat.Specular * L.Specular;

}

// ����Blinn-Phong����ģ���µĵ��
void ComputePointLight(Material mat, PointLight L, float3 pos, float3 normal, float3 toEye,
	out float4 ambient, out float4 diffuse, out float4 spec)
{
	// ��ʼ�����
    ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// �ӱ��浽��Դ������
    float3 lightVec = L.Position - pos;

	// ���浽���ߵľ���
    float d = length(lightVec);

	// �ƹⷶΧ����
    if (d > L.Range)
        return;

	// ��׼��������
    lightVec /= d;

	// ********��ӻ�����********
	ambient = mat.Ambient * L.Ambient;

	// ********����������;����********
	// ��������������
	float diffuseFactor = 0.5f * dot(lightVec, normal) + 0.5f;  // half-lambert

	// ���������������߷���+���߷��򣬽����һ����
	float3 halfDir = normalize(lightVec + toEye);

	// ����Blinn-Phong���淴������
	float specFactor = pow(max(dot(normal, halfDir), 0.0f), mat.Specular.w);

	// �������������ǿ��
	diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;

	// ���㾵�淴�����ǿ��
	spec = specFactor * mat.Specular * L.Specular;

	// ���˥��
    float att = 1.0f / dot(L.Att, float3(1.0f, d, d * d));

    diffuse *= att;
    spec *= att;
}


void ComputeSpotLight(Material mat, SpotLight L, float3 pos, float3 normal, float3 toEye,
	out float4 ambient, out float4 diffuse, out float4 spec)
{
	// ��ʼ�����
    ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// // �ӱ��浽��Դ������
    float3 lightVec = L.Position - pos;

    // ���浽��Դ�ľ���
    float d = length(lightVec);

	// ��Χ����
    if (d > L.Range)
        return;

	// ��׼��������
    lightVec /= d;

	// ********��ӻ�����********
	ambient = mat.Ambient * L.Ambient;

	// ********����������;����********
	// ��������������
	float diffuseFactor = 0.5f * dot(lightVec, normal) + 0.5f;  // half-lambert

	// ���������������߷���+���߷��򣬽����һ����
	float3 halfDir = normalize(lightVec + toEye);

	// ����Blinn-Phong���淴������
	float specFactor = pow(max(dot(normal, halfDir), 0.0f), mat.Specular.w);

	// �������������ǿ��
	diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;

	// ���㾵�淴�����ǿ��
	spec = specFactor * mat.Specular * L.Specular;

	// ���������Ӻ�˥��ϵ��
    float spot = pow(max(dot(-lightVec, L.Direction), 0.0f), L.Spot);
    float att = spot / dot(L.Att, float3(1.0f, d, d * d));

    ambient *= spot;
    diffuse *= att;
    spec *= att;
}