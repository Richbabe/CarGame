#ifndef LIGHTHELPER_H
#define LIGHTHELPER_H

#include <cstring>
#include <DirectXMath.h>


// 环境光
struct DirectionalLight
{
	DirectionalLight() { memset(this, 0, sizeof(DirectionalLight)); }

	DirectX::XMFLOAT4 Ambient;  // 环境光
	DirectX::XMFLOAT4 Diffuse;  // 漫反射光
	DirectX::XMFLOAT4 Specular;  // 镜面反射光
	DirectX::XMFLOAT3 Direction;  // 光照方向
	float Pad; // 最后用一个浮点数填充使得该结构体大小满足16的倍数，便于我们以后在HLSL设置数组
};

// 点光
struct PointLight
{
	PointLight() { memset(this, 0, sizeof(PointLight)); }

	DirectX::XMFLOAT4 Ambient;  // 环境光
	DirectX::XMFLOAT4 Diffuse;    // 漫反射光
	DirectX::XMFLOAT4 Specular;  // 镜面反射光

	// 打包成4D向量: (Position, Range)
	DirectX::XMFLOAT3 Position;  // 光源位置
	float Range;  // 点光源照射范围

	// 打包成4D向量: (A0, A1, A2, Pad)
	DirectX::XMFLOAT3 Att;  // 衰弱因子
	float Pad; // 最后用一个浮点数填充使得该结构体大小满足16的倍数，便于我们以后在HLSL设置数组
};

// 聚光灯
struct SpotLight
{
	SpotLight() { memset(this, 0, sizeof(SpotLight)); }

	DirectX::XMFLOAT4 Ambient;  // 环境光
	DirectX::XMFLOAT4 Diffuse;    // 漫反射光
	DirectX::XMFLOAT4 Specular;  // 镜面反射光

	// 打包成4D向量: (Position, Range)
	DirectX::XMFLOAT3 Position;  // 光源位置
	float Range;  // 点光源照射范围

	// 打包成4D向量: (Direction, Spot)
	DirectX::XMFLOAT3 Direction;  // 光照方向
	float Spot;  // 光的汇聚程度

	// 打包成4D向量: (Att, Pad)
	DirectX::XMFLOAT3 Att;  // 衰弱因子
	float Pad; // 最后用一个浮点数填充使得该结构体大小满足16的倍数，便于我们以后在HLSL设置数组
};

// 物体表面材质
struct Material
{
	Material() { memset(this, 0, sizeof(Material)); }

	DirectX::XMFLOAT4 Ambient;  // 物体自身颜色
	DirectX::XMFLOAT4 Diffuse;  // 漫反射系数
	DirectX::XMFLOAT4 Specular; // w = 镜面反射强度
	DirectX::XMFLOAT4 Reflect;
};

#endif