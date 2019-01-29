#include "Car.h"
#include "d3dUtil.h"
#include "iostream"
#pragma warning(disable:4996)
using namespace std;
using namespace DirectX;

Car::CarComponet::CarComponet() {  
	rotation = XMMatrixIdentity();
	translation = XMMatrixIdentity();
	modelMatrix = XMMatrixIdentity();
}

Car::Car(){
	carWorldPostition = XMFLOAT3(0.0f, 0.0f, 0.0f);
	carHeadDirection = XMFLOAT3(0.0f, 0.0f, 1.0f);
	carRightDirection = XMFLOAT3(1.0f, 0.0f, 0.0f);
	mCarState = CarState::Static;

	carComponents.assign({
		&carBody,
		&carFrontLeftWheel,
		&carFrontRightWheel,
		&carBackLeftWheel,
		&carBackRightWheel });

	carWheels.assign({
		&carFrontLeftWheel,
		&carFrontRightWheel,
		&carBackLeftWheel,
		&carBackRightWheel });

	rotation = XMMatrixIdentity();
	translation = XMMatrixIdentity();
}

void Car::InitCarBody(ComPtr<ID3D11Device> device) {
	ComPtr<ID3D11ShaderResourceView> texture;
	Material material;
	material.Ambient = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	material.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	material.Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 16.0f);

	carBody.translation = XMMatrixTranslation(0.0f, 1.0f, 0.0f);
	carBody.modelMatrix = XMMatrixTranslation(0.0f, 1.0f, 0.0f);
	carBody.SetWorldMatrix(carBody.modelMatrix);

	HR(CreateDDSTextureFromFile(device.Get(), L"Texture\\WoodCrate.dds", nullptr, texture.GetAddressOf()));
	carBody.SetBuffer(device, Geometry::CreateBox(2.0f, 2.0f, 5.0f));
	carBody.SetTexture(texture);
	carBody.SetMaterial(material);
}

void Car::InitCarFrontLeftWheel(ComPtr<ID3D11Device> device) {
	ComPtr<ID3D11ShaderResourceView> texture;
	Material material;
	material.Ambient = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	material.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	material.Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 16.0f);
	carFrontLeftWheel.rotation = XMMatrixRotationZ(XM_PIDIV2);
	carFrontLeftWheel.translation = XMMatrixTranslation(-1.0f, 0.1f, 1.5f);
	carFrontLeftWheel.modelMatrix = XMMatrixRotationZ(XM_PIDIV2) * XMMatrixTranslation(-1.0f, 0.1f, 1.5f);
	carFrontLeftWheel.SetWorldMatrix(carFrontLeftWheel.modelMatrix);

	HR(CreateDDSTextureFromFile(device.Get(), L"Texture\\brick.dds", nullptr, texture.GetAddressOf()));
	carFrontLeftWheel.SetBuffer(device, Geometry::CreateCylinder(0.5f, 0.5f));
	carFrontLeftWheel.SetTexture(texture);
	carFrontLeftWheel.SetMaterial(material);
}

void Car::InitCarFrontRightWheel(ComPtr<ID3D11Device> device) {
	ComPtr<ID3D11ShaderResourceView> texture;
	Material material;
	material.Ambient = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	material.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	material.Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 16.0f);
	carFrontRightWheel.rotation = XMMatrixRotationZ(XM_PIDIV2);
	carFrontRightWheel.translation = XMMatrixTranslation(1.0f, 0.1f, 1.5f);
	carFrontRightWheel.modelMatrix = XMMatrixRotationZ(XM_PIDIV2) * XMMatrixTranslation(1.0f, 0.1f, 1.5f);
	carFrontRightWheel.SetWorldMatrix(carFrontRightWheel.modelMatrix);

	HR(CreateDDSTextureFromFile(device.Get(), L"Texture\\brick.dds", nullptr, texture.GetAddressOf()));
	carFrontRightWheel.SetBuffer(device, Geometry::CreateCylinder(0.5f, 0.5f));
	carFrontRightWheel.SetTexture(texture);
	carFrontRightWheel.SetMaterial(material);
}

void Car::InitCarBackLeftWheel(ComPtr<ID3D11Device> device) {
	ComPtr<ID3D11ShaderResourceView> texture;
	Material material;
	material.Ambient = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	material.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	material.Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 16.0f);
	carBackLeftWheel.rotation = XMMatrixRotationZ(XM_PIDIV2);
	carBackLeftWheel.translation = XMMatrixTranslation(-1.0f, 0.1f, -1.5f);
	carBackLeftWheel.modelMatrix = XMMatrixRotationZ(XM_PIDIV2) * XMMatrixTranslation(-1.0f, 0.1f, -1.5f);
	carBackLeftWheel.SetWorldMatrix(carBackLeftWheel.modelMatrix);

	HR(CreateDDSTextureFromFile(device.Get(), L"Texture\\brick.dds", nullptr, texture.GetAddressOf()));
	carBackLeftWheel.SetBuffer(device, Geometry::CreateCylinder(0.5f, 0.5f));
	carBackLeftWheel.SetTexture(texture);
	carBackLeftWheel.SetMaterial(material);
}

void Car::InitCarBackRightWheel(ComPtr<ID3D11Device> device) {
	ComPtr<ID3D11ShaderResourceView> texture;
	Material material;
	material.Ambient = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	material.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	material.Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 16.0f);
	carBackRightWheel.rotation = XMMatrixRotationZ(XM_PIDIV2);
	carBackRightWheel.translation = XMMatrixTranslation(1.0f, 0.1f, -1.5f);
	carBackRightWheel.modelMatrix = XMMatrixRotationZ(XM_PIDIV2) * XMMatrixTranslation(1.0f, 0.1f, -1.5f);
	carBackRightWheel.SetWorldMatrix(carBackRightWheel.modelMatrix);

	HR(CreateDDSTextureFromFile(device.Get(), L"Texture\\brick.dds", nullptr, texture.GetAddressOf()));
	carBackRightWheel.SetBuffer(device, Geometry::CreateCylinder(0.5f, 0.5f));
	carBackRightWheel.SetTexture(texture);
	carBackRightWheel.SetMaterial(material);
}

void Car::SetCarMaterial(Material material) {
	for (auto& car_item : carComponents)
	{
		car_item->SetMaterial(material);
	}
}

void Car::SetCarWorldMatrix(DirectX::XMMATRIX worldMatrix) {
	
	for (auto& car_item : carComponents)
	{
		car_item->SetWorldMatrix(car_item->modelMatrix * worldMatrix);
	}
}


DirectX::XMFLOAT3 Car::GetCarPosition() {
	return carWorldPostition;
}

DirectX::XMFLOAT3 Car::GetCarDirection() {
	return carHeadDirection;
}

void Car::InitCar(ComPtr<ID3D11Device> device) {
	InitCarBody(device);
	InitCarFrontLeftWheel(device);
	InitCarFrontRightWheel(device);
	InitCarBackLeftWheel(device);
	InitCarBackRightWheel(device);
}

void Car::CarMove(float distance) {
	static float angle = 0.0f;
	angle += distance > 0 ? 0.1f : -0.1f;
	XMVECTOR Front = XMVector3Normalize(XMLoadFloat3(&carHeadDirection));
	XMVECTOR Right = XMVector3Normalize(XMLoadFloat3(&carRightDirection));
	XMVECTOR Pos = XMLoadFloat3(&carWorldPostition);
	XMVECTOR Dist = XMVectorReplicate(distance);  // 得到向量（d,d,d）
	XMVECTOR DestPos = XMVectorMultiplyAdd(Dist, Front, Pos);  // 目标位置DestPos = Dist * Front + Pos
	XMStoreFloat3(&carWorldPostition, DestPos);
	translation = XMMatrixTranslation(carWorldPostition.x, carWorldPostition.y, carWorldPostition.z);

	SetCarWorldMatrix(rotation * translation);
	
	// 计算轮子的旋转
	for (auto wheel : carWheels) {
		XMFLOAT3 wheelWolrdPos = wheel->GetPosition();
		XMMATRIX WheelRunTranslation = XMMatrixTranslation(wheelWolrdPos.x, wheelWolrdPos.y, wheelWolrdPos.z);
		XMVECTOR Axis = XMVector3Normalize(XMLoadFloat3(&carRightDirection));
		XMMATRIX WheelRunRotation = XMMatrixRotationAxis(Axis, angle);
		wheel->SetWorldMatrix(wheel->rotation * rotation * WheelRunRotation * WheelRunTranslation);
	}
}

void Car::CarTurn(float &totalDegree, float degree, CarState carState) {
	XMMATRIX wheel_rotationY = XMMatrixRotationY(degree > 0 ? XM_PI / 36 : -XM_PI / 36);
	XMVECTOR CarHeadDirection = XMLoadFloat3(&carHeadDirection);
	XMVECTOR CarRightDirection = XMLoadFloat3(&carRightDirection);
	if (carState == CarState::Forward) {
		// 计算汽车转向后的前向量
		CarHeadDirection = XMVector3Transform(CarHeadDirection, XMMatrixRotationY(degree));
		XMStoreFloat3(&carHeadDirection, CarHeadDirection);

		// 计算汽车转向后的右向量
		CarRightDirection = XMVector3Transform(CarRightDirection, XMMatrixRotationY(degree));
		XMStoreFloat3(&carRightDirection, CarRightDirection);

		
		totalDegree += degree;
	}
	else if (carState == CarState::Back) {
		// 计算汽车转向后的前向量
		CarHeadDirection = XMVector3Transform(CarHeadDirection, XMMatrixRotationY(-degree));
		XMStoreFloat3(&carHeadDirection, CarHeadDirection);

		// 计算汽车转向后的右向量
		CarRightDirection = XMVector3Transform(CarRightDirection, XMMatrixRotationY(-degree));
		XMStoreFloat3(&carRightDirection, CarRightDirection);

		// 计算最终偏移度数
		totalDegree -= degree;
	}

	// 更新旋转矩阵
	rotation = XMMatrixRotationY(totalDegree);
}

void Car::SetCarMoveBack(){
	mCarState = CarState::Back;
}

void Car::SetCarMoveForward() {
	mCarState = CarState::Forward;
}

void Car::SetCarStatic() {
	mCarState = CarState::Static;
}

Car::CarState Car::GetCarState() {
	return mCarState;
}

void Car::Draw(ComPtr<ID3D11DeviceContext> deviceContext, BasicEffect& effect) {
	for (auto carComponent : carComponents) {
		carComponent->Draw(deviceContext, effect);
	}
}