//***************************************************************************************
// 天空盒加载与渲染类
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

	// 需要提供完整的天空盒贴图，或者已经创建好的天空盒纹理.dds文件
	SkyRender(ComPtr<ID3D11Device> device,              // D3D设备
		ComPtr<ID3D11DeviceContext> deviceContext,      // D3D设备上下文
		const std::wstring& cubemapFilename,            // 天空盒路径
		float skySphereRadius,		                    // 天空球半径
		bool generateMips = false);	                    // 默认不为天空盒生成mipmaps

	// 需要提供天空盒的六张正方形贴图
	SkyRender(ComPtr<ID3D11Device> device,						// D3D设备
		ComPtr<ID3D11DeviceContext> deviceContext,				// D3D设备上下文
		const std::vector<std::wstring>& cubemapFilenames,      // 天空盒六张正方形贴图路径集合
		float skySphereRadius,									// 天空球半径
		bool generateMips = false);								// 默认不为天空盒生成mipmaps

	ComPtr<ID3D11ShaderResourceView> GetTextureCube();  // 获取天空盒

	void Draw(ComPtr<ID3D11DeviceContext> deviceContext, SkyEffect& skyEffect, const Camera& camera);  // 绘制天空盒

private:
	void InitResource(ComPtr<ID3D11Device> device, float skySphereRadius);  // 初始化资源

private:
	ComPtr<ID3D11Buffer> mVertexBuffer;  // 顶点缓冲区
	ComPtr<ID3D11Buffer> mIndexBuffer;   // 索引缓冲区

	UINT mIndexCount;                    // 顶点数量

	ComPtr<ID3D11ShaderResourceView> mTextureCubeSRV;       // 天空盒着色器资源视图
};

#endif