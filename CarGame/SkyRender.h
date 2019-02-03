//***************************************************************************************
// ��պм�������Ⱦ��
// Skybox loader and render classes.
//***************************************************************************************

#ifndef SKYRENDER_H
#define SKYRENDER_H

#include <vector>
#include <string>
#include "Effects.h"
#include "Camera.h"

class SkyRender {
public:
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	// ��Ҫ�ṩ��������պ���ͼ�������Ѿ������õ���պ�����.dds�ļ�
	SkyRender(ComPtr<ID3D11Device> device,              // D3D�豸
		ComPtr<ID3D11DeviceContext> deviceContext,      // D3D�豸������
		const std::wstring& cubemapFilename,            // ��պ�·��
		float skySphereRadius,		                    // �����뾶
		bool generateMips = false);	                    // Ĭ�ϲ�Ϊ��պ�����mipmaps

	// ��Ҫ�ṩ��պе�������������ͼ
	SkyRender(ComPtr<ID3D11Device> device,						// D3D�豸
		ComPtr<ID3D11DeviceContext> deviceContext,				// D3D�豸������
		const std::vector<std::wstring>& cubemapFilenames,      // ��պ�������������ͼ·������
		float skySphereRadius,									// �����뾶
		bool generateMips = false);								// Ĭ�ϲ�Ϊ��պ�����mipmaps

	ComPtr<ID3D11ShaderResourceView> GetTextureCube();  // ��ȡ��պ�

	void Draw(ComPtr<ID3D11DeviceContext> deviceContext, SkyEffect& skyEffect, const Camera& camera);  // ������պ�

private:
	void InitResource(ComPtr<ID3D11Device> device, float skySphereRadius);  // ��ʼ����Դ

private:
	ComPtr<ID3D11Buffer> mVertexBuffer;  // ���㻺����
	ComPtr<ID3D11Buffer> mIndexBuffer;   // ����������

	UINT mIndexCount;                    // ��������

	ComPtr<ID3D11ShaderResourceView> mTextureCubeSRV;       // ��պ���ɫ����Դ��ͼ
};

#endif