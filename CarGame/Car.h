#ifndef Car_H
#define Car_H

#include "GameObject.h"

class Car{
public:
	// ����״̬(ǰ������ֹ������)
	enum class CarState { Forward, Static, Back };

	// ��������
	class CarComponet : public GameObject {
	public:
		CarComponet();

	public:
		DirectX::XMMATRIX modelMatrix;  // �������ڳ�ģ������ϵ��ת������	
		DirectX::XMMATRIX rotation;     // ����������������ϵ�µ�ת������
		DirectX::XMMATRIX translation;  // ����������������ϵ�µ�ƽ�ƾ���
	};

public:
	// ʹ��ģ�����(C++11)��������
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

	void CarMove(float distance);  // ����ǰ������
	void CarTurn(float &totalDegree, float degree, CarState carState);  // ����ת�亯��
	void SetCarMoveBack();  // ��������Ϊ����
	void SetCarMoveForward();  // ����������ǰ
	void SetCarStatic();  // ����������ֹ
	CarState GetCarState();  // ��ȡ������ǰ״̬

	void Draw(ComPtr<ID3D11DeviceContext> deviceContext, BasicEffect& effect);

private:
	Material car_material;  // ���Ĳ���
	DirectX::XMFLOAT3 car_world_position;  // ������������ϵ�е�λ��
	DirectX::XMFLOAT3 car_head_direction;  // ������ͷ�ĳ���
	DirectX::XMFLOAT3 car_right_direction;
	CarComponet car_body;  // ����
	CarComponet car_front_left_wheel;  // ��ǰ��
	CarComponet car_front_right_wheel;  // ��ǰ��
	CarComponet car_back_left_wheel;  // �����
	CarComponet car_back_right_wheel;  // �Һ���
	std::vector<CarComponet*> car_components;

	DirectX::XMMATRIX rotation;
	DirectX::XMMATRIX translation;

	DirectX::XMFLOAT4X4 carWorldMatrix;

	CarState mCarState;
};

#endif
