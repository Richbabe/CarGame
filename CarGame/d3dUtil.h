//***************************************************************************************
// d3dUtil.h by X_Jun(MKXJun) (C) 2018-2019 All Rights Reserved.
// Licensed under the MIT License.
//
// D3Dʵ�ù��߼�
// Direct3D utility tools.
//***************************************************************************************

#ifndef D3DUTIL_H
#define D3DUTIL_H

#include <d3d11_1.h>			// �Ѱ���Windows.h
#include <DirectXCollision.h>	// �Ѱ���DirectXMath.h
#include <DirectXColors.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
#include <filesystem>
#include <vector>
#include <string>
#include "DXTrace.h"
#include "DDSTextureLoader.h"	
#include "WICTextureLoader.h"

//
// ��ɫ��������غ���
//

// ------------------------------
// CreateShaderFromFile����
// ------------------------------
// [In]csoFileNameInOut ����õ���ɫ���������ļ�(.cso)������ָ��������Ѱ�Ҹ��ļ�����ȡ
// [In]hlslFileName     ��ɫ�����룬��δ�ҵ���ɫ���������ļ��������ɫ������
// [In]entryPoint       ��ڵ�(ָ����ʼ�ĺ���)
// [In]shaderModel      ��ɫ��ģ�ͣ���ʽΪ"*s_5_0"��*����Ϊc,d,g,h,p,v֮һ
// [Out]ppBlobOut       �����ɫ����������Ϣ
HRESULT CreateShaderFromFile(const WCHAR* csoFileNameInOut, const WCHAR* hlslFileName, LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob** ppBlobOut);



//
// ����������غ���
//

// ���ݸ�����DDS�����ļ����ϣ�����2D��������
// Ҫ����������Ŀ�Ⱥ͸߶ȶ�һ��
// ��maxMipMapSizeΪ0��ʹ��Ĭ��mipmap�ȼ�
// ����mipmap�ȼ������ᳬ��maxMipMapSize
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> CreateDDSTexture2DArrayFromFile(
	Microsoft::WRL::ComPtr<ID3D11Device> device,
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext,
	const std::vector<std::wstring>& filenames,
	UINT maxMipMapSize = 0);


//
// ������������غ���
//

// ���ݸ�����һ�Ű����������������������������������
// Ҫ�������߱�Ϊ4:3���Ұ�������ʽ����:
// .  +Y .  .
// -X +Z +X -Z 
// .  -Y .  .
// �ú���Ĭ�ϲ�����mipmap(���ȼ���Ϊ1)������Ҫ������generateMipsΪtrue
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> CreateWICTextureCubeFromFile(
	Microsoft::WRL::ComPtr<ID3D11Device> device,
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext,
	std::wstring cubemapFileName,
	bool generateMips = false);

// ���ݰ�D3D11_TEXTURECUBE_FACE����˳�����������������������������
// Ҫ��������ͬ����С��������
// �ú���Ĭ�ϲ�����mipmap(���ȼ���Ϊ1)������Ҫ������generateMipsΪtrue
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> CreateWICTextureCubeFromFile(
	Microsoft::WRL::ComPtr<ID3D11Device> device,
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext,
	std::vector<std::wstring> cubemapFileNames,
	bool generateMips = false);




#endif
