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

	void InitCar(ComPtr<ID3D11Device> device);                 // ��ʼ������
	void InitCarBody(ComPtr<ID3D11Device> device);             // ��ʼ������
	void InitCarFrontLeftWheel(ComPtr<ID3D11Device> device);   // ��ʼ��������ǰ��
	void InitCarFrontRightWheel(ComPtr<ID3D11Device> device);  // ��ʼ��������ǰ��
	void InitCarBackLeftWheel(ComPtr<ID3D11Device> device);    // ��ʼ�����������
	void InitCarBackRightWheel(ComPtr<ID3D11Device> device);   // ��ʼ�������Һ���
	void InitCarLeftLight(ComPtr<ID3D11Device> device);        // ��ʼ��������ǰ��
	void InitCarRightLight(ComPtr<ID3D11Device> device);       // ��ʼ��������ǰ��
	void SetCarMaterial(Material material);                 // ���������������
	void SetCarWorldMatrix(DirectX::XMMATRIX worldMatrix);  // �����������������
	DirectX::XMFLOAT3 GetCarPosition();                    // �����������������ϵ�µ�λ�ú���
  	DirectX::XMFLOAT3 GetCarDirection();                   // �������������
	DirectX::XMFLOAT3 GetCarLeftLightPosition();           // ��������󳵵��������꺯��
	DirectX::XMFLOAT3 GetCarRightLightPosition();          // ��������ҳ����������꺯��

	void CarMove(float distance);  // ����ǰ������
	void CarTurn(float &totalDegree, float degree, CarState carState);  // ����ת�亯��
	void SetCarMoveBack();     // ��������Ϊ����
	void SetCarMoveForward();  // ����������ǰ
	void SetCarStatic();       // ����������ֹ
	void CarLightOnOff();      // ���س���
	CarState GetCarState();    // ��ȡ������ǰ״̬
	bool GetCarLightState();   // ��ȡ��ǰ����״̬

	void Draw(ComPtr<ID3D11DeviceContext> deviceContext, BasicEffect& effect);

private:
	Material carMaterial;  // ���Ĳ���
	DirectX::XMFLOAT3 carWorldPostition;   // ������������ϵ�е�λ��
	DirectX::XMFLOAT3 carHeadDirection;   // ������ͷ�ĳ���
	DirectX::XMFLOAT3 carRightDirection;  // ����������
	CarComponet carBody;  // ����
	CarComponet carFrontLeftWheel;   // ��ǰ��
	CarComponet carFrontRightWheel;  // ��ǰ��
	CarComponet carBackLeftWheel;    // �����
	CarComponet carBackRightWheel;   // �Һ���
	CarComponet carLeftLight;        // ��ǰ��
	CarComponet carRightLight;       // ��ǰ��
	std::vector<CarComponet*> carComponents;    // ����������ĸ�����
	std::vector<CarComponet*> carWheels;  //�������ĸ�����

	DirectX::XMMATRIX rotation;     // �����������ת����
	DirectX::XMMATRIX translation;  // ���������ƽ�ƾ���

	CarState mCarState;  // ����״̬
	bool carLightOn;     // ���������Ƿ�����
};

#endif
