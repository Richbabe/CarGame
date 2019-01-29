#ifndef Car_H
#define Car_H

#include "GameObject.h"

class Car{
public:
	// 汽车状态(前进、静止、后退)
	enum class CarState { Forward, Static, Back };

	// 车部件类
	class CarComponet : public GameObject {
	public:
		CarComponet();

	public:
		DirectX::XMMATRIX modelMatrix;  // 车部件在车模型坐标系的转换矩阵	
		DirectX::XMMATRIX rotation;     // 车部件在世界坐标系下的转换矩阵
		DirectX::XMMATRIX translation;  // 车部件在世界坐标系下的平移矩阵
	};

public:
	// 使用模板别名(C++11)简化类型名
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	Car();

	void InitCar(ComPtr<ID3D11Device> device);
	void InitCarBody(ComPtr<ID3D11Device> device);
	void InitCarFrontLeftWheel(ComPtr<ID3D11Device> device);
	void InitCarFrontRightWheel(ComPtr<ID3D11Device> device);
	void InitCarBackLeftWheel(ComPtr<ID3D11Device> device);
	void InitCarBackRightWheel(ComPtr<ID3D11Device> device);
	void SetCarMaterial(Material material);
	void SetCarWorldMatrix(DirectX::XMMATRIX worldMatrix);
	DirectX::XMFLOAT3 GetCarPosition();
	DirectX::XMFLOAT3 GetCarDirection();

	void CarMove(float distance);  // 汽车前进函数
	void CarTurn(float &totalDegree, float degree, CarState carState);  // 汽车转弯函数
	void SetCarMoveBack();  // 设置汽车为后退
	void SetCarMoveForward();  // 设置汽车向前
	void SetCarStatic();  // 设置汽车静止
	CarState GetCarState();  // 获取汽车当前状态

	void Draw(ComPtr<ID3D11DeviceContext> deviceContext, BasicEffect& effect);

private:
	Material car_material;  // 车的材质
	DirectX::XMFLOAT3 car_world_position;  // 车在世界坐标系中的位置
	DirectX::XMFLOAT3 car_head_direction;  // 汽车车头的朝向
	DirectX::XMFLOAT3 car_right_direction;
	CarComponet car_body;  // 车身
	CarComponet car_front_left_wheel;  // 左前轮
	CarComponet car_front_right_wheel;  // 右前轮
	CarComponet car_back_left_wheel;  // 左后轮
	CarComponet car_back_right_wheel;  // 右后轮
	std::vector<CarComponet*> car_components;

	DirectX::XMMATRIX rotation;
	DirectX::XMMATRIX translation;

	DirectX::XMFLOAT4X4 carWorldMatrix;

	CarState mCarState;
};

#endif
