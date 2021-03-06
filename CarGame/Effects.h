//***************************************************************************************
// Effects.h by X_Jun(MKXJun) (C) 2018-2019 All Rights Reserved.
// Licensed under the MIT License.
//
// 简易特效管理框架
// Simple effect management framework.
//***************************************************************************************

#ifndef EFFECTS_H
#define EFFECTS_H

#include <memory>
#include "LightHelper.h"
#include "RenderStates.h"


class IEffect
{
public:
	// 使用模板别名(C++11)简化类型名
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	IEffect() = default;

	// 不支持复制构造
	IEffect(const IEffect&) = delete;
	IEffect& operator=(const IEffect&) = delete;

	// 允许转移
	IEffect(IEffect&& moveFrom) = default;
	IEffect& operator=(IEffect&& moveFrom) = default;

	virtual ~IEffect() = default;

	// 更新并绑定常量缓冲区
	virtual void Apply(ComPtr<ID3D11DeviceContext> deviceContext) = 0;
};


class BasicEffect : public IEffect
{
public:
	// 渲染类型，RenderObject为渲染普通物体，RenderInstance为实例化渲染
	enum RenderType { RenderObject, RenderInstance };

	BasicEffect();
	virtual ~BasicEffect() override;

	BasicEffect(BasicEffect&& moveFrom);
	BasicEffect& operator=(BasicEffect&& moveFrom);

	// 获取单例
	static BasicEffect& Get();

	

	// 初始化Basic.hlsli所需资源并初始化渲染状态
	bool InitAll(ComPtr<ID3D11Device> device);


	//
	// 渲染模式的变更
	//

	// 默认状态来绘制
	void SetRenderDefault(ComPtr<ID3D11DeviceContext> deviceContext, RenderType type);
	// Alpha混合绘制
	void SetRenderAlphaBlend(ComPtr<ID3D11DeviceContext> deviceContext, RenderType type);
	// 无二次混合
	void SetRenderNoDoubleBlend(ComPtr<ID3D11DeviceContext> deviceContext, UINT stencilRef, RenderType type);
	// 仅写入模板值
	void SetWriteStencilOnly(ComPtr<ID3D11DeviceContext> deviceContext, UINT stencilRef, RenderType type);
	// 对指定模板值的区域进行绘制，采用默认状态
	void SetRenderDefaultWithStencil(ComPtr<ID3D11DeviceContext> deviceContext, UINT stencilRef, RenderType type);
	// 对指定模板值的区域进行绘制，采用Alpha混合
	void SetRenderAlphaBlendWithStencil(ComPtr<ID3D11DeviceContext> deviceContext, UINT stencilRef, RenderType type);

	//
	// 矩阵设置
	//

	void XM_CALLCONV SetWorldMatrix(DirectX::FXMMATRIX W);
	void XM_CALLCONV SetViewMatrix(DirectX::FXMMATRIX V);
	void XM_CALLCONV SetProjMatrix(DirectX::FXMMATRIX P);
	void XM_CALLCONV SetWorldViewProjMatrix(DirectX::FXMMATRIX W, DirectX::CXMMATRIX V, DirectX::CXMMATRIX P);

	void XM_CALLCONV SetReflectionMatrix(DirectX::FXMMATRIX R);
	void XM_CALLCONV SetShadowMatrix(DirectX::FXMMATRIX S);
	void XM_CALLCONV SetRefShadowMatrix(DirectX::FXMMATRIX RefS);
	
	//
	// 光照、材质和纹理相关设置
	//

	// 各种类型灯光允许的最大数目
	static const int maxLights = 5;

	void SetDirLight(size_t pos, const DirectionalLight& dirLight);
	void SetPointLight(size_t pos, const PointLight& pointLight);
	void SetSpotLight(size_t pos, const SpotLight& spotLight);

	void SetMaterial(const Material& material);

	void SetTextureDiffuse(ComPtr<ID3D11ShaderResourceView> texture);

	void XM_CALLCONV SetEyePos(DirectX::FXMVECTOR eyePos);



	//
	// 状态开关设置
	//

	void SetReflectionState(bool isOn);
	void SetShadowState(bool isOn);
	

	// 应用常量缓冲区和纹理资源的变更
	void Apply(ComPtr<ID3D11DeviceContext> deviceContext);
	
private:
	class Impl;
	std::unique_ptr<Impl> pImpl;
};


class SkyEffect : IEffect
{
public:
	SkyEffect();
	virtual ~SkyEffect() override;

	SkyEffect(SkyEffect&& moveFrom);
	SkyEffect& operator=(SkyEffect&& moveFrom);

	// 获取单例
	static SkyEffect& Get();

	// 初始化Sky.hlsli所需资源并初始化渲染状态
	bool InitAll(ComPtr<ID3D11Device> device);

	// 
	// 渲染模式的变更
	//

	// 默认状态来绘制
	void SetRenderDefault(ComPtr<ID3D11DeviceContext> deviceContext);

	//
	// 矩阵设置
	//

	void XM_CALLCONV SetWorldViewProjMatrix(DirectX::FXMMATRIX W, DirectX::CXMMATRIX V, DirectX::CXMMATRIX P);
	void XM_CALLCONV SetWorldViewProjMatrix(DirectX::FXMMATRIX WVP);

	//
	// 纹理立方体映射设置
	//

	void SetTextureCube(ComPtr<ID3D11ShaderResourceView> textureCube);


	// 应用常量缓冲区和纹理资源的变更
	void Apply(ComPtr<ID3D11DeviceContext> deviceContext);

private:
	class Impl;
	std::unique_ptr<Impl> pImpl;
};

class BoundingBoxEffect : IEffect
{
public:
	BoundingBoxEffect();
	virtual ~BoundingBoxEffect() override;

	BoundingBoxEffect(BoundingBoxEffect&& moveFrom);
	BoundingBoxEffect& operator=(BoundingBoxEffect&& moveFrom);

	// 获取单例
	static BoundingBoxEffect& Get();

	// 初始化Sky.hlsli所需资源并初始化渲染状态
	bool InitAll(ComPtr<ID3D11Device> device);

	// 
	// 渲染模式的变更
	//

	// 默认状态来绘制
	void SetRenderDefault(ComPtr<ID3D11DeviceContext> deviceContext);

	//
	// 矩阵设置
	//

	void XM_CALLCONV SetWorldViewProjMatrix(DirectX::FXMMATRIX W, DirectX::CXMMATRIX V, DirectX::CXMMATRIX P);
	void XM_CALLCONV SetWorldViewProjMatrix(DirectX::FXMMATRIX WVP);


	// 应用常量缓冲区和纹理资源的变更
	void Apply(ComPtr<ID3D11DeviceContext> deviceContext);

private:
	class Impl;
	std::unique_ptr<Impl> pImpl;
};







#endif