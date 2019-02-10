#include "Effects.h"
#include "d3dUtil.h"
#include "EffectHelper.h"	// 必须晚于Effects.h和d3dUtil.h包含
#include "Vertex.h"
using namespace DirectX;
using namespace std::experimental;

//
// BoundingBoxEffect::Impl 需要先于BoundingBoxEffect的定义
//

class BoundingBoxEffect::Impl : public AlignedType<BoundingBoxEffect::Impl>
{
public:
	//
	// 这些结构体对应HLSL的结构体。需要按16字节对齐
	//

	struct CBChangesEveryFrame
	{
		DirectX::XMMATRIX worldViewProj;
	};

public:
	// 必须显式指定
	Impl() = default;
	~Impl() = default;

public:
	CBufferObject<0, CBChangesEveryFrame>	cbFrame;	// 每帧绘制的常量缓冲区

	BOOL isDirty;										// 是否有值变更
	std::vector<CBufferBase*> cBufferPtrs;				// 统一管理上面所有的常量缓冲区

	ComPtr<ID3D11VertexShader> boundingBoxVS;
	ComPtr<ID3D11PixelShader> boundingBoxPS;

	ComPtr<ID3D11InputLayout> vertexPosLayout;

};

//
// BoundingBoxEffect
//

namespace
{
	// BoundingBoxEffect单例
	static BoundingBoxEffect * pInstance = nullptr;
}

BoundingBoxEffect::BoundingBoxEffect()
{
	if (pInstance)
		throw std::exception("BoundingBoxEffect is a singleton!");
	pInstance = this;
	pImpl = std::make_unique<BoundingBoxEffect::Impl>();
}

BoundingBoxEffect::~BoundingBoxEffect()
{
}

BoundingBoxEffect::BoundingBoxEffect(BoundingBoxEffect && moveFrom)
{
	pImpl.swap(moveFrom.pImpl);
}

BoundingBoxEffect & BoundingBoxEffect::operator=(BoundingBoxEffect && moveFrom)
{
	pImpl.swap(moveFrom.pImpl);
	return *this;
}

BoundingBoxEffect & BoundingBoxEffect::Get()
{
	if (!pInstance)
		throw std::exception("BoundingBoxEffect needs an instance!");
	return *pInstance;
}

bool BoundingBoxEffect::InitAll(ComPtr<ID3D11Device> device)
{
	if (!device)
		return false;

	if (!pImpl->cBufferPtrs.empty())
		return true;

	if (!RenderStates::IsInit())
		throw std::exception("RenderStates need to be initialized first!");

	ComPtr<ID3DBlob> blob;

	// ******************
	// 创建顶点着色器
	//

	HR(CreateShaderFromFile(L"HLSL\\BoudingBox_VS.cso", L"HLSL\\BoudingBox_VS.hlsl", "VS", "vs_5_0", blob.ReleaseAndGetAddressOf()));
	HR(device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, pImpl->boundingBoxVS.GetAddressOf()));
	// 创建顶点布局
	HR(device->CreateInputLayout(VertexPosColor::inputLayout, ARRAYSIZE(VertexPosColor::inputLayout),
		blob->GetBufferPointer(), blob->GetBufferSize(), pImpl->vertexPosLayout.GetAddressOf()));

	// ******************
	// 创建像素着色器
	//

	HR(CreateShaderFromFile(L"HLSL\\BoudingBox_PS.cso", L"HLSL\\BoudingBox_PS.hlsl", "PS", "ps_5_0", blob.ReleaseAndGetAddressOf()));
	HR(device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, pImpl->boundingBoxPS.GetAddressOf()));


	pImpl->cBufferPtrs.assign({
		&pImpl->cbFrame,
		});

	// 创建常量缓冲区
	for (auto& pBuffer : pImpl->cBufferPtrs)
	{
		pBuffer->CreateBuffer(device);
	}

	return true;
}

void BoundingBoxEffect::SetRenderDefault(ComPtr<ID3D11DeviceContext> deviceContext)
{
	deviceContext->IASetInputLayout(pImpl->vertexPosLayout.Get());
	deviceContext->VSSetShader(pImpl->boundingBoxVS.Get(), nullptr, 0);
	deviceContext->PSSetShader(pImpl->boundingBoxPS.Get(), nullptr, 0);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	deviceContext->GSSetShader(nullptr, nullptr, 0);
	deviceContext->RSSetState(RenderStates::RSNoCull.Get());

	deviceContext->PSSetSamplers(0, 1, RenderStates::SSLinearWrap.GetAddressOf());
	deviceContext->OMSetDepthStencilState(RenderStates::DSSLessEqual.Get(), 0);
	deviceContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
}

void XM_CALLCONV BoundingBoxEffect::SetWorldViewProjMatrix(DirectX::FXMMATRIX W, DirectX::CXMMATRIX V, DirectX::CXMMATRIX P)
{
	auto& cBuffer = pImpl->cbFrame;
	cBuffer.data.worldViewProj = XMMatrixTranspose(W * V * P);
	pImpl->isDirty = cBuffer.isDirty = true;
}

void XM_CALLCONV BoundingBoxEffect::SetWorldViewProjMatrix(DirectX::FXMMATRIX WVP)
{
	auto& cBuffer = pImpl->cbFrame;
	cBuffer.data.worldViewProj = XMMatrixTranspose(WVP);
	pImpl->isDirty = cBuffer.isDirty = true;
}

void BoundingBoxEffect::Apply(ComPtr<ID3D11DeviceContext> deviceContext)
{
	auto& pCBuffers = pImpl->cBufferPtrs;
	// 将缓冲区绑定到渲染管线上
	pCBuffers[0]->BindVS(deviceContext);

	if (pImpl->isDirty)
	{
		pImpl->isDirty = false;
		for (auto& pCBuffer : pCBuffers)
		{
			pCBuffer->UpdateBuffer(deviceContext);
		}
	}
}