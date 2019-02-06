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

	void InitCar(ComPtr<ID3D11Device> device);                 // 初始化汽车
	void InitCarBody(ComPtr<ID3D11Device> device);             // 初始化车身
	void InitCarFrontLeftWheel(ComPtr<ID3D11Device> device);   // 初始化汽车左前轮
	void InitCarFrontRightWheel(ComPtr<ID3D11Device> device);  // 初始化汽车右前轮
	void InitCarBackLeftWheel(ComPtr<ID3D11Device> device);    // 初始化汽车左后轮
	void InitCarBackRightWheel(ComPtr<ID3D11Device> device);   // 初始化汽车右后轮
	void InitCarLeftLight(ComPtr<ID3D11Device> device);        // 初始化汽车左前灯
	void InitCarRightLight(ComPtr<ID3D11Device> device);       // 初始化汽车右前灯
	void SetCarMaterial(Material material);                 // 设置汽车整体材质
	void SetCarWorldMatrix(DirectX::XMMATRIX worldMatrix);  // 设置汽车的世界矩阵
	DirectX::XMFLOAT3 GetCarPosition();                    // 获得汽车在世界坐标系下的位置函数
  	DirectX::XMFLOAT3 GetCarDirection();                   // 获得汽车方向函数
	DirectX::XMFLOAT3 GetCarLeftLightPosition();           // 获得汽车左车灯世界坐标函数
	DirectX::XMFLOAT3 GetCarRightLightPosition();          // 获得汽车右车灯世界坐标函数

	void CarMove(float distance);  // 汽车前进函数
	void CarTurn(float &totalDegree, float degree, CarState carState);  // 汽车转弯函数
	void SetCarMoveBack();     // 设置汽车为后退
	void SetCarMoveForward();  // 设置汽车向前
	void SetCarStatic();       // 设置汽车静止
	void CarLightOnOff();      // 开关车灯
	CarState GetCarState();    // 获取汽车当前状态
	bool GetCarLightState();   // 获取当前车灯状态

	void Draw(ComPtr<ID3D11DeviceContext> deviceContext, BasicEffect& effect);

private:
	Material carMaterial;  // 车的材质
	DirectX::XMFLOAT3 carWorldPostition;   // 车在世界坐标系中的位置
	DirectX::XMFLOAT3 carHeadDirection;   // 汽车车头的朝向
	DirectX::XMFLOAT3 carRightDirection;  // 汽车右向量
	CarComponet carBody;  // 车身
	CarComponet carFrontLeftWheel;   // 左前轮
	CarComponet carFrontRightWheel;  // 右前轮
	CarComponet carBackLeftWheel;    // 左后轮
	CarComponet carBackRightWheel;   // 右后轮
	CarComponet carLeftLight;        // 左前灯
	CarComponet carRightLight;       // 右前灯
	std::vector<CarComponet*> carComponents;    // 组成汽车的四个部件
	std::vector<CarComponet*> carWheels;  //汽车的四个轮子

	DirectX::XMMATRIX rotation;     // 汽车整体的旋转矩阵
	DirectX::XMMATRIX translation;  // 汽车整体的平移矩阵

	CarState mCarState;  // 汽车状态
	bool carLightOn;     // 汽车车灯是否亮着
};

#endif
