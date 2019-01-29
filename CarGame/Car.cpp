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
	car_world_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	car_head_direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
	car_right_direction = XMFLOAT3(1.0f, 0.0f, 0.0f);
	mCarState = CarState::Static;

	car_components.assign({
		&car_body,
		&car_front_left_wheel,
		&car_front_right_wheel,
		&car_back_left_wheel,
		&car_back_right_wheel });

	XMMATRIX rotation = XMMatrixIdentity();
	XMMATRIX translation = XMMatrixIdentity();
}

void Car::InitCarBody(ComPtr<ID3D11Device> device) {
	ComPtr<ID3D11ShaderResourceView> texture;
	Material material;
	material.Ambient = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	material.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	material.Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 16.0f);

	car_body.translation = XMMatrixTranslation(0.0f, 1.0f, 0.0f);
	car_body.modelMatrix = XMMatrixTranslation(0.0f, 1.0f, 0.0f);
	car_body.SetWorldMatrix(car_body.modelMatrix);

	// 初始化木盒
	HR(CreateDDSTextureFromFile(device.Get(), L"Texture\\WoodCrate.dds", nullptr, texture.GetAddressOf()));
	car_body.SetBuffer(device, Geometry::CreateBox(2.0f, 2.0f, 5.0f));
	car_body.SetTexture(texture);
	car_body.SetMaterial(material);
}

void Car::InitCarFrontLeftWheel(ComPtr<ID3D11Device> device) {
	ComPtr<ID3D11ShaderResourceView> texture;
	Material material;
	material.Ambient = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	material.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	material.Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 16.0f);
	car_front_left_wheel.rotation = XMMatrixRotationZ(XM_PIDIV2);
	car_front_left_wheel.translation = XMMatrixTranslation(-1.0f, 0.1f, 1.5f);
	car_front_left_wheel.modelMatrix = XMMatrixRotationZ(XM_PIDIV2) * XMMatrixTranslation(-1.0f, 0.1f, 1.5f);
	car_front_left_wheel.SetWorldMatrix(car_front_left_wheel.modelMatrix);

	// 初始化木盒
	HR(CreateDDSTextureFromFile(device.Get(), L"Texture\\brick.dds", nullptr, texture.GetAddressOf()));
	car_front_left_wheel.SetBuffer(device, Geometry::CreateCylinder(0.5f, 0.5f));
	car_front_left_wheel.SetTexture(texture);
	car_front_left_wheel.SetMaterial(material);
}

void Car::InitCarFrontRightWheel(ComPtr<ID3D11Device> device) {
	ComPtr<ID3D11ShaderResourceView> texture;
	Material material;
	material.Ambient = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	material.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	material.Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 16.0f);
	car_front_right_wheel.rotation = XMMatrixRotationZ(XM_PIDIV2);
	car_front_right_wheel.translation = XMMatrixTranslation(1.0f, 0.1f, 1.5f);
	car_front_right_wheel.modelMatrix = XMMatrixRotationZ(XM_PIDIV2) * XMMatrixTranslation(1.0f, 0.1f, 1.5f);
	car_front_right_wheel.SetWorldMatrix(car_front_right_wheel.modelMatrix);

	// 初始化木盒
	HR(CreateDDSTextureFromFile(device.Get(), L"Texture\\brick.dds", nullptr, texture.GetAddressOf()));
	car_front_right_wheel.SetBuffer(device, Geometry::CreateCylinder(0.5f, 0.5f));
	car_front_right_wheel.SetTexture(texture);
	car_front_right_wheel.SetMaterial(material);
}

void Car::InitCarBackLeftWheel(ComPtr<ID3D11Device> device) {
	ComPtr<ID3D11ShaderResourceView> texture;
	Material material;
	material.Ambient = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	material.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	material.Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 16.0f);
	car_back_left_wheel.rotation = XMMatrixRotationZ(XM_PIDIV2);
	car_back_left_wheel.translation = XMMatrixTranslation(-1.0f, 0.1f, -1.5f);
	car_back_left_wheel.modelMatrix = XMMatrixRotationZ(XM_PIDIV2) * XMMatrixTranslation(-1.0f, 0.1f, -1.5f);
	car_back_left_wheel.SetWorldMatrix(car_back_left_wheel.modelMatrix);

	// 初始化木盒
	HR(CreateDDSTextureFromFile(device.Get(), L"Texture\\brick.dds", nullptr, texture.GetAddressOf()));
	car_back_left_wheel.SetBuffer(device, Geometry::CreateCylinder(0.5f, 0.5f));
	car_back_left_wheel.SetTexture(texture);
	car_back_left_wheel.SetMaterial(material);
}

void Car::InitCarBackRightWheel(ComPtr<ID3D11Device> device) {
	ComPtr<ID3D11ShaderResourceView> texture;
	Material material;
	material.Ambient = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	material.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	material.Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 16.0f);
	car_back_right_wheel.rotation = XMMatrixRotationZ(XM_PIDIV2);
	car_back_right_wheel.translation = XMMatrixTranslation(1.0f, 0.1f, -1.5f);
	car_back_right_wheel.modelMatrix = XMMatrixRotationZ(XM_PIDIV2) * XMMatrixTranslation(1.0f, 0.1f, -1.5f);
	car_back_right_wheel.SetWorldMatrix(car_back_right_wheel.modelMatrix);

	// 初始化木盒
	HR(CreateDDSTextureFromFile(device.Get(), L"Texture\\brick.dds", nullptr, texture.GetAddressOf()));
	car_back_right_wheel.SetBuffer(device, Geometry::CreateCylinder(0.5f, 0.5f));
	car_back_right_wheel.SetTexture(texture);
	car_back_right_wheel.SetMaterial(material);
}

void Car::SetCarMaterial(Material material) {
	for (auto& car_item : car_components)
	{
		car_item->SetMaterial(material);
	}
}

void Car::SetCarWorldMatrix(DirectX::XMMATRIX worldMatrix) {
	
	for (auto& car_item : car_components)
	{
		car_item->SetWorldMatrix(car_item->modelMatrix * worldMatrix);
	}
}


DirectX::XMFLOAT3 Car::GetCarPosition() {
	return car_world_position;
}

DirectX::XMFLOAT3 Car::GetCarDirection() {
	return car_head_direction;
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
	XMVECTOR Front = XMVector3Normalize(XMLoadFloat3(&car_head_direction));
	XMVECTOR Right = XMVector3Normalize(XMLoadFloat3(&car_right_direction));
	XMVECTOR Pos = XMLoadFloat3(&car_world_position);
	XMVECTOR Dist = XMVectorReplicate(distance);  // 得到向量（d,d,d）
	XMVECTOR DestPos = XMVectorMultiplyAdd(Dist, Front, Pos);  // 目标位置DestPos = Dist * Front + Pos
	XMStoreFloat3(&car_world_position, DestPos);
	translation = XMMatrixTranslation(car_world_position.x, car_world_position.y, car_world_position.z);

	SetCarWorldMatrix(rotation * translation);

	for (int i = 1; i < car_components.size(); i++) {
		XMFLOAT3 wheelWolrdPos = car_components[i]->GetPosition();
		XMMATRIX WheelRunTranslation = XMMatrixTranslation(wheelWolrdPos.x, wheelWolrdPos.y, wheelWolrdPos.z);
		XMVECTOR Axis = XMVector3Normalize(XMLoadFloat3(&car_right_direction));
		XMMATRIX WheelRunRotation = XMMatrixRotationAxis(Axis, angle);
		car_components[i]->SetWorldMatrix(car_components[i]->rotation * rotation * WheelRunRotation * WheelRunTranslation);
	}
	//SetCarWorldMatrix(rotation * translation);
}

void Car::CarTurn(float &totalDegree, float degree, CarState carState) {
	XMMATRIX wheel_rotationY = XMMatrixRotationY(degree > 0 ? XM_PI / 36 : -XM_PI / 36);
	if (carState == CarState::Forward) {
		XMVECTOR Car_head_direction = XMLoadFloat3(&car_head_direction);
		Car_head_direction = XMVector3Transform(Car_head_direction, XMMatrixRotationY(degree));
		XMStoreFloat3(&car_head_direction, Car_head_direction);

		XMVECTOR Car_right_direction = XMLoadFloat3(&car_right_direction);
		Car_right_direction = XMVector3Transform(Car_right_direction, XMMatrixRotationY(degree));
		XMStoreFloat3(&car_right_direction, Car_right_direction);

		totalDegree += degree;

	}
	else if (carState == CarState::Back) {
		XMVECTOR Car_head_direction = XMLoadFloat3(&car_head_direction);
		Car_head_direction = XMVector3Transform(Car_head_direction, XMMatrixRotationY(-degree));
		XMStoreFloat3(&car_head_direction, Car_head_direction);

		XMVECTOR Car_right_direction = XMLoadFloat3(&car_right_direction);
		Car_right_direction = XMVector3Transform(Car_right_direction, XMMatrixRotationY(-degree));
		XMStoreFloat3(&car_right_direction, Car_right_direction);

		totalDegree -= degree;
	}
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
	car_body.Draw(deviceContext, effect);
	car_front_left_wheel.Draw(deviceContext, effect);
	car_front_right_wheel.Draw(deviceContext, effect);
	car_back_left_wheel.Draw(deviceContext, effect);
	car_back_right_wheel.Draw(deviceContext, effect);
}