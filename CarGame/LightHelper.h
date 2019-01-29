#ifndef LIGHTHELPER_H
#define LIGHTHELPER_H

#include <cstring>
#include <DirectXMath.h>


// ������
struct DirectionalLight
{
	DirectionalLight() { memset(this, 0, sizeof(DirectionalLight)); }

	DirectX::XMFLOAT4 Ambient;  // ������
	DirectX::XMFLOAT4 Diffuse;  // �������
	DirectX::XMFLOAT4 Specular;  // ���淴���
	DirectX::XMFLOAT3 Direction;  // ���շ���
	float Pad; // �����һ�����������ʹ�øýṹ���С����16�ı��������������Ժ���HLSL��������
};

// ���
struct PointLight
{
	PointLight() { memset(this, 0, sizeof(PointLight)); }

	DirectX::XMFLOAT4 Ambient;  // ������
	DirectX::XMFLOAT4 Diffuse;    // �������
	DirectX::XMFLOAT4 Specular;  // ���淴���

	// �����4D����: (Position, Range)
	DirectX::XMFLOAT3 Position;  // ��Դλ��
	float Range;  // ���Դ���䷶Χ

	// �����4D����: (A0, A1, A2, Pad)
	DirectX::XMFLOAT3 Att;  // ˥������
	float Pad; // �����һ�����������ʹ�øýṹ���С����16�ı��������������Ժ���HLSL��������
};

// �۹��
struct SpotLight
{
	SpotLight() { memset(this, 0, sizeof(SpotLight)); }

	DirectX::XMFLOAT4 Ambient;  // ������
	DirectX::XMFLOAT4 Diffuse;    // �������
	DirectX::XMFLOAT4 Specular;  // ���淴���

	// �����4D����: (Position, Range)
	DirectX::XMFLOAT3 Position;  // ��Դλ��
	float Range;  // ���Դ���䷶Χ

	// �����4D����: (Direction, Spot)
	DirectX::XMFLOAT3 Direction;  // ���շ���
	float Spot;  // ��Ļ�۳̶�

	// �����4D����: (Att, Pad)
	DirectX::XMFLOAT3 Att;  // ˥������
	float Pad; // �����һ�����������ʹ�øýṹ���С����16�ı��������������Ժ���HLSL��������
};

// ����������
struct Material
{
	Material() { memset(this, 0, sizeof(Material)); }

	DirectX::XMFLOAT4 Ambient;  // ����������ɫ
	DirectX::XMFLOAT4 Diffuse;  // ������ϵ��
	DirectX::XMFLOAT4 Specular; // w = ���淴��ǿ��
	DirectX::XMFLOAT4 Reflect;
};

#endif