#include "d3dUtil.h"

using namespace Microsoft::WRL;
using namespace DirectX;
using namespace std::experimental;

HRESULT CreateShaderFromFile(const WCHAR * csoFileNameInOut, const WCHAR * hlslFileName,
	LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob ** ppBlobOut)
{
	HRESULT hr = S_OK;

	// Ѱ���Ƿ����Ѿ�����õĶ�����ɫ��
	if (csoFileNameInOut && filesystem::exists(csoFileNameInOut))
	{
		HR(D3DReadFileToBlob(csoFileNameInOut, ppBlobOut));
	}
	else
	{
		DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
		// ���� D3DCOMPILE_DEBUG ��־���ڻ�ȡ��ɫ��������Ϣ���ñ�־���������������飬
		// ����Ȼ������ɫ�������Ż�����
		dwShaderFlags |= D3DCOMPILE_DEBUG;

		// ��Debug�����½����Ż��Ա������һЩ����������
		dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
		ComPtr<ID3DBlob> errorBlob = nullptr;
		hr = D3DCompileFromFile(hlslFileName, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint, shaderModel,
			dwShaderFlags, 0, ppBlobOut, errorBlob.GetAddressOf());
		if (FAILED(hr))
		{
			if (errorBlob != nullptr)
			{
				OutputDebugStringA(reinterpret_cast<const char*>(errorBlob->GetBufferPointer()));
			}
			return hr;
		}

		// ��ָ��������ļ���������ɫ����������Ϣ���
		if (csoFileNameInOut)
		{
			HR(D3DWriteBlobToFile(*ppBlobOut, csoFileNameInOut, FALSE));
		}
	}

	return hr;
}

ComPtr<ID3D11ShaderResourceView> CreateDDSTexture2DArrayFromFile(
	ComPtr<ID3D11Device> device,
	ComPtr<ID3D11DeviceContext> deviceContext,
	const std::vector<std::wstring>& filenames,
	UINT maxMipMapSize)
{
	// ����豸���豸�������Ƿ�ǿ�
	if (!device || !deviceContext)
		return nullptr;

	// ******************
	// 1. ��ȡ��������
	//
	UINT size = (UINT)filenames.size();
	UINT mipLevel = maxMipMapSize;
	std::vector<ComPtr<ID3D11Texture2D>> srcTex(size);
	UINT width, height;
	DXGI_FORMAT format;
	for (size_t i = 0; i < size; ++i)
	{
		// ������Щ�������ᱻGPUʹ�ã�����ʹ��D3D11_USAGE_STAGINGö��ֵ
		// ʹ��CPU���Զ�ȡ��Դ
		HR(CreateDDSTextureFromFileEx(device.Get(),
			deviceContext.Get(),
			filenames[i].c_str(),
			maxMipMapSize,
			D3D11_USAGE_STAGING,							// Usage
			0,												// BindFlags
			D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ,	// CpuAccessFlags
			0,												// MiscFlags
			false,
			(ID3D11Resource**)srcTex[i].GetAddressOf(),
			nullptr));

		// ��ȡ�����õ�����Mipmap�ȼ�, ��Ⱥ͸߶�
		D3D11_TEXTURE2D_DESC texDesc;
		srcTex[i]->GetDesc(&texDesc);
		if (i == 0)
		{
			mipLevel = texDesc.MipLevels;
			width = texDesc.Width;
			height = texDesc.Height;
			format = texDesc.Format;
		}
		// ����������������MipMap�ȼ�����Ⱥ͸߶�Ӧ��һ��
		assert(mipLevel == texDesc.MipLevels);
		assert(texDesc.Width == width && texDesc.Height == height);
		// ����Ҫ�������ṩ��ͼƬ���ݸ�ʽӦ����һ�µģ������ڲ�һ�µ��������
		// ʹ��dxtex.exe(DirectX Texture Tool)�����е�ͼƬת��һ�µ����ݸ�ʽ
		assert(texDesc.Format == format);

	}

	// ******************
	// 2.������������
	//
	D3D11_TEXTURE2D_DESC texDesc, texArrayDesc;
	srcTex[0]->GetDesc(&texDesc);
	texArrayDesc.Width = texDesc.Width;
	texArrayDesc.Height = texDesc.Height;
	texArrayDesc.MipLevels = texDesc.MipLevels;
	texArrayDesc.ArraySize = size;
	texArrayDesc.Format = texDesc.Format;
	texArrayDesc.SampleDesc.Count = 1;		// ����ʹ�ö��ز���
	texArrayDesc.SampleDesc.Quality = 0;
	texArrayDesc.Usage = D3D11_USAGE_DEFAULT;
	texArrayDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texArrayDesc.CPUAccessFlags = 0;
	texArrayDesc.MiscFlags = 0;

	ComPtr<ID3D11Texture2D> texArray;
	HR(device->CreateTexture2D(&texArrayDesc, nullptr, texArray.GetAddressOf()));

	// ******************
	// 3.�����е���������Դ��ֵ������������
	//

	// ÿ������Ԫ��
	for (UINT i = 0; i < size; ++i)
	{
		// �����е�ÿ��mipmap�ȼ�
		for (UINT j = 0; j < mipLevel; ++j)
		{
			D3D11_MAPPED_SUBRESOURCE mappedTex2D;
			// ����ӳ������i�����У�����j��mipmap�ȼ���2D����
			HR(deviceContext->Map(srcTex[i].Get(),
				j, D3D11_MAP_READ, 0, &mappedTex2D));

			deviceContext->UpdateSubresource(
				texArray.Get(),
				D3D11CalcSubresource(j, i, mipLevel),	// i * mipLevel + j
				nullptr,
				mappedTex2D.pData,
				mappedTex2D.RowPitch,
				mappedTex2D.DepthPitch);
			// ֹͣӳ��
			deviceContext->Unmap(srcTex[i].Get(), j);
		}
	}

	// ******************
	// 4.�������������SRV
	//
	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = texArrayDesc.Format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	viewDesc.Texture2DArray.MostDetailedMip = 0;
	viewDesc.Texture2DArray.MipLevels = texArrayDesc.MipLevels;
	viewDesc.Texture2DArray.FirstArraySlice = 0;
	viewDesc.Texture2DArray.ArraySize = size;

	ComPtr<ID3D11ShaderResourceView> texArraySRV;
	HR(device->CreateShaderResourceView(texArray.Get(), &viewDesc, texArraySRV.GetAddressOf()));


	// �Ѿ�ȷ��������Դ��ComPtr���������ֶ��ͷ�

	return texArraySRV;
}

ComPtr<ID3D11ShaderResourceView> CreateWICTextureCubeFromFile(
	ComPtr<ID3D11Device> device,
	ComPtr<ID3D11DeviceContext> deviceContext,
	std::wstring cubemapFileName,
	bool generateMips)
{
	// ����豸���豸�������Ƿ�ǿ�
	if (!device || !deviceContext)
		return nullptr;

	// ******************
	// 1.��ȡ��պ�����
	//

	ComPtr<ID3D11Texture2D> srcTex;
	ComPtr<ID3D11ShaderResourceView> srcTexSRV;

	// ����Դ����GPU����
	HR(CreateWICTextureFromFileEx(device.Get(),
		deviceContext.Get(),
		cubemapFileName.c_str(),
		0,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_SHADER_RESOURCE | (generateMips ? D3D11_BIND_RENDER_TARGET : 0),
		0,
		(generateMips ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0),
		WIC_LOADER_DEFAULT,
		(ID3D11Resource**)srcTex.GetAddressOf(),
		(generateMips ? srcTexSRV.GetAddressOf() : nullptr)));
	// (��ѡ)����mipmap��
	if (generateMips)
	{
		deviceContext->GenerateMips(srcTexSRV.Get());
	}


	// ******************
	// 2.��������6�����������
	//

	D3D11_TEXTURE2D_DESC texDesc, texCubeDesc;
	srcTex->GetDesc(&texDesc);

	// ȷ����߱�4:3
	assert(texDesc.Width * 3 == texDesc.Height * 4);

	UINT squareLength = texDesc.Width / 4;

	texCubeDesc.Width = squareLength;
	texCubeDesc.Height = squareLength;
	texCubeDesc.MipLevels = (generateMips ? 0 : 1);	// ָ��0������½���������mipmap��
	texCubeDesc.ArraySize = 6;
	texCubeDesc.Format = texDesc.Format;
	texCubeDesc.SampleDesc.Count = 1;
	texCubeDesc.SampleDesc.Quality = 0;
	texCubeDesc.Usage = D3D11_USAGE_DEFAULT;
	texCubeDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texCubeDesc.CPUAccessFlags = 0;
	texCubeDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;	// ���ΪTextureCube

	ComPtr<ID3D11Texture2D> texCube;
	HR(device->CreateTexture2D(&texCubeDesc, nullptr, texCube.GetAddressOf()));

	// ******************
	// 3.ѡȡԭ��պ������6�������������򣬿�������������
	//

	D3D11_BOX box;
	// box����������: 
	//    front
	//   / 
	//  /_____right
	//  |
	//  |
	//  bottom
	box.front = 0;
	box.back = 1;

	for (UINT i = 0; i < texCubeDesc.MipLevels; ++i)
	{
		// +X�濽��
		box.left = squareLength * 2;
		box.top = squareLength;
		box.right = squareLength * 3;
		box.bottom = squareLength * 2;
		deviceContext->CopySubresourceRegion(
			texCube.Get(),
			D3D11CalcSubresource(i, D3D11_TEXTURECUBE_FACE_POSITIVE_X, texCubeDesc.MipLevels),
			0, 0, 0,
			srcTex.Get(),
			i,
			&box);

		// -X�濽��
		box.left = 0;
		box.top = squareLength;
		box.right = squareLength;
		box.bottom = squareLength * 2;
		deviceContext->CopySubresourceRegion(
			texCube.Get(),
			D3D11CalcSubresource(i, D3D11_TEXTURECUBE_FACE_NEGATIVE_X, texCubeDesc.MipLevels),
			0, 0, 0,
			srcTex.Get(),
			i,
			&box);

		// +Y�濽��
		box.left = squareLength;
		box.top = 0;
		box.right = squareLength * 2;
		box.bottom = squareLength;
		deviceContext->CopySubresourceRegion(
			texCube.Get(),
			D3D11CalcSubresource(i, D3D11_TEXTURECUBE_FACE_POSITIVE_Y, texCubeDesc.MipLevels),
			0, 0, 0,
			srcTex.Get(),
			i,
			&box);


		// -Y�濽��
		box.left = squareLength;
		box.top = squareLength * 2;
		box.right = squareLength * 2;
		box.bottom = squareLength * 3;
		deviceContext->CopySubresourceRegion(
			texCube.Get(),
			D3D11CalcSubresource(i, D3D11_TEXTURECUBE_FACE_NEGATIVE_Y, texCubeDesc.MipLevels),
			0, 0, 0,
			srcTex.Get(),
			i,
			&box);

		// +Z�濽��
		box.left = squareLength;
		box.top = squareLength;
		box.right = squareLength * 2;
		box.bottom = squareLength * 2;
		deviceContext->CopySubresourceRegion(
			texCube.Get(),
			D3D11CalcSubresource(i, D3D11_TEXTURECUBE_FACE_POSITIVE_Z, texCubeDesc.MipLevels),
			0, 0, 0,
			srcTex.Get(),
			i,
			&box);

		// -Z�濽��
		box.left = squareLength * 3;
		box.top = squareLength;
		box.right = squareLength * 4;
		box.bottom = squareLength * 2;
		deviceContext->CopySubresourceRegion(
			texCube.Get(),
			D3D11CalcSubresource(i, D3D11_TEXTURECUBE_FACE_NEGATIVE_Z, texCubeDesc.MipLevels),
			0, 0, 0,
			srcTex.Get(),
			i,
			&box);

		// ��һ��mipLevel�������߶���ԭ����1/2
		squareLength /= 2;
	}


	// ******************
	// 4.���������������SRV
	//

	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = texCubeDesc.Format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	viewDesc.TextureCube.MostDetailedMip = 0;
	viewDesc.TextureCube.MipLevels = texCubeDesc.MipLevels;

	ComPtr<ID3D11ShaderResourceView> texCubeSRV;
	HR(device->CreateShaderResourceView(texCube.Get(), &viewDesc, texCubeSRV.GetAddressOf()));

	return texCubeSRV;
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> CreateWICTextureCubeFromFile(
	Microsoft::WRL::ComPtr<ID3D11Device> device,
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext,
	std::vector<std::wstring> cubemapFileNames,
	bool generateMips)
{
	// ����豸���豸�������Ƿ�ǿ�
	if (!device || !deviceContext)
		return nullptr;

	// ������������Ҫ6����������ͼ�ļ�
	assert(cubemapFileNames.size() == 6);

	// ******************
	// 1.��ȡ����
	//

	std::vector<ComPtr<ID3D11Texture2D>> srcTex(6);
	std::vector<ComPtr<ID3D11ShaderResourceView>> srcTexSRV(6);
	UINT width, height;
	DXGI_FORMAT format;

	for (int i = 0; i < 6; ++i)
	{
		// ����Դ����GPU����
		HR(CreateWICTextureFromFileEx(device.Get(),
			deviceContext.Get(),
			cubemapFileNames[i].c_str(),
			0,
			D3D11_USAGE_DEFAULT,
			D3D11_BIND_SHADER_RESOURCE | (generateMips ? D3D11_BIND_RENDER_TARGET : 0),
			0,
			(generateMips ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0),
			WIC_LOADER_DEFAULT,
			(ID3D11Resource**)srcTex[i].GetAddressOf(),
			(generateMips ? srcTexSRV[i].GetAddressOf() : nullptr)));
		// (��ѡ)����mipmap��
		if (generateMips)
		{
			deviceContext->GenerateMips(srcTexSRV[i].Get());
		}

		D3D11_TEXTURE2D_DESC texDesc;
		srcTex[i]->GetDesc(&texDesc);
		if (i == 0)
		{
			width = texDesc.Width;
			height = texDesc.Height;
			format = texDesc.Format;
		}
		// ���������������Ŀ�Ⱥ͸߶�Ӧ��һ��
		assert(texDesc.Width == width && texDesc.Height == height);
		// ����Ҫ�������ṩ��ͼƬ���ݸ�ʽӦ����һ�µģ������ڲ�һ�µ��������
		// ʹ��dxtex.exe(DirectX Texture Tool)�����е�ͼƬת��һ�µ����ݸ�ʽ
		assert(texDesc.Format == format);
	}

	// ******************
	// 2.��������6�����������
	//

	D3D11_TEXTURE2D_DESC texDesc, texCubeDesc;
	srcTex[0]->GetDesc(&texDesc);

	texCubeDesc.Width = texDesc.Width;
	texCubeDesc.Height = texDesc.Height;
	texCubeDesc.MipLevels = (generateMips ? 0 : 1); // ָ��0������½���������mipmap��
	texCubeDesc.ArraySize = 6;
	texCubeDesc.Format = texDesc.Format;
	texCubeDesc.SampleDesc.Count = 1;
	texCubeDesc.SampleDesc.Quality = 0;
	texCubeDesc.Usage = D3D11_USAGE_DEFAULT;
	texCubeDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texCubeDesc.CPUAccessFlags = 0;
	texCubeDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;	// ���ΪTextureCube

	ComPtr<ID3D11Texture2D> texCube;
	HR(device->CreateTexture2D(&texCubeDesc, nullptr, texCube.GetAddressOf()));

	// ******************
	// 3.ѡȡԭλͼ��6�������������򣬿�������������
	//
	for (int i = 0; i < 6; ++i)
	{
		for (UINT j = 0; j < texCubeDesc.MipLevels; ++j)
		{
			deviceContext->CopySubresourceRegion(
				texCube.Get(),
				D3D11CalcSubresource(j, i, texCubeDesc.MipLevels),
				0, 0, 0,
				srcTex[i].Get(),
				j,
				nullptr);
		}
	}

	// ******************
	// 4.���������������SRV
	//

	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = texCubeDesc.Format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	viewDesc.TextureCube.MostDetailedMip = 0;
	viewDesc.TextureCube.MipLevels = texCubeDesc.MipLevels;

	ComPtr<ID3D11ShaderResourceView> texCubeSRV;
	HR(device->CreateShaderResourceView(texCube.Get(), &viewDesc, texCubeSRV.GetAddressOf()));

	return texCubeSRV;
}

