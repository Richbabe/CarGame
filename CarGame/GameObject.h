#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Model.h"

// 游戏对象类
class GameObject
{
public:
	// 使用模板别名(C++11)简化类型名
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	GameObject();

	// 获取位置
	DirectX::XMFLOAT3 GetPosition() const;
	// 设置纹理
	void SetTexture(ComPtr<ID3D11ShaderResourceView> texture);
	// 设置材质
	void SetMaterial(const Material & material);
	// 设置模型矩阵
	void SetWorldMatrix(const DirectX::XMFLOAT4X4& world);
	void XM_CALLCONV SetWorldMatrix(DirectX::FXMMATRIX world);

	// 获取包围盒
	DirectX::BoundingBox GetLocalBoundingBox() const;                // 获取局部坐标系的AABB包围盒
	DirectX::BoundingBox GetBoundingBox() const;                     // 获取世界坐标系的AABB包围盒
	DirectX::BoundingOrientedBox GetBoundingOrientedBox() const;     // 获取世界坐标系的OBB包围盒

	// 设置模型
	void SetModel(Model&& model);
	void SetModel(const Model& model);


	// 绘制
	void Draw(ComPtr<ID3D11DeviceContext> deviceContext, BasicEffect& effect);
private:
	Model mModel;								    // 模型
	DirectX::XMFLOAT3 mWorldPosition;               // 世界坐标系下的位置
	DirectX::XMFLOAT4X4 mWorldMatrix;               // 世界矩阵
	Material mMaterial;								// 物体材质
	ComPtr<ID3D11ShaderResourceView> mTexture;      // 纹理
};

#endif
