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
	carWorldPostition = XMFLOAT3(0.0f, 0.0f, -100.0f);
	carHeadDirection = XMFLOAT3(0.0f, 0.0f, 1.0f);
	carRightDirection = XMFLOAT3(1.0f, 0.0f, 0.0f);
	mCarState = CarState::Static;
	carLightOn = false;

	carComponents.assign({
		&carBody,
		&carFrontLeftWheel,
		&carFrontRightWheel,
		&carBackLeftWheel,
		&carBackRightWheel,
		&carLeftLight,
		&carRightLight});

	carWheels.assign({
		&carFrontLeftWheel,
		&carFrontRightWheel,
		&carBackLeftWheel,
		&carBackRightWheel });

	rotation = XMMatrixIdentity();
	translation = XMMatrixTranslation(0.0f, 0.0f, -100.0f);
}

void Car::InitCarBody(ComPtr<ID3D11Device> device) {
	ComPtr<ID3D11ShaderResourceView> texture;
	Material material;
	material.Ambient = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	material.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	material.Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 16.0f);

	carBody.translation = XMMatrixTranslation(0.0f, -0.5f, 0.0f);
	carBody.modelMatrix = XMMatrixTranslation(0.0f, -0.5f, 0.0f);
	carBody.SetWorldMatrix(carBody.modelMatrix);

	HR(CreateDDSTextureFromFile(device.Get(), L"Texture\\car\\car_body.dds", nullptr, texture.GetAddressOf()));
	carBody.SetModel(Model(device, Geometry::CreateBox(0.8f, 0.8f, 2.0f)));
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
	carFrontLeftWheel.translation = XMMatrixTranslation(-0.4f, -0.8f, 1.0f);
	carFrontLeftWheel.modelMatrix = XMMatrixRotationZ(XM_PIDIV2) * XMMatrixTranslation(-0.4f, -0.8f, 1.0f);
	carFrontLeftWheel.SetWorldMatrix(carFrontLeftWheel.modelMatrix);

	HR(CreateDDSTextureFromFile(device.Get(), L"Texture\\car\\car_wheel.dds", nullptr, texture.GetAddressOf()));
	carFrontLeftWheel.SetModel(Model(device, Geometry::CreateCylinder(0.2f, 0.2f)));
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
	carFrontRightWheel.translation = XMMatrixTranslation(0.4f, -0.8f, 1.0f);
	carFrontRightWheel.modelMatrix = XMMatrixRotationZ(XM_PIDIV2) * XMMatrixTranslation(0.4f, -0.8f, 1.0f);
	carFrontRightWheel.SetWorldMatrix(carFrontRightWheel.modelMatrix);

	HR(CreateDDSTextureFromFile(device.Get(), L"Texture\\car\\car_wheel.dds", nullptr, texture.GetAddressOf()));
	carFrontRightWheel.SetModel(Model(device, Geometry::CreateCylinder(0.2f, 0.2f)));
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
	carBackLeftWheel.translation = XMMatrixTranslation(-0.4f, -0.8f, -1.0f);
	carBackLeftWheel.modelMatrix = XMMatrixRotationZ(XM_PIDIV2) * XMMatrixTranslation(-0.4f, -0.8f, -1.0f);
	carBackLeftWheel.SetWorldMatrix(carBackLeftWheel.modelMatrix);

	HR(CreateDDSTextureFromFile(device.Get(), L"Texture\\car\\car_wheel.dds", nullptr, texture.GetAddressOf()));
	carBackLeftWheel.SetModel(Model(device, Geometry::CreateCylinder(0.2f, 0.2f)));
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
	carBackRightWheel.translation = XMMatrixTranslation(0.4f, -0.8f, -1.0f);
	carBackRightWheel.modelMatrix = XMMatrixRotationZ(XM_PIDIV2) * XMMatrixTranslation(0.4f, -0.8f, -1.0f);
	carBackRightWheel.SetWorldMatrix(carBackRightWheel.modelMatrix);

	HR(CreateDDSTextureFromFile(device.Get(), L"Texture\\car\\car_wheel.dds", nullptr, texture.GetAddressOf()));
	carBackRightWheel.SetModel(Model(device, Geometry::CreateCylinder(0.2f, 0.2f)));
	carBackRightWheel.SetTexture(texture);
	carBackRightWheel.SetMaterial(material);
}

void Car::InitCarLeftLight(ComPtr<ID3D11Device> device) {
	ComPtr<ID3D11ShaderResourceView> texture;
	Material material;
	material.Ambient = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	material.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	material.Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 16.0f);
	carLeftLight.translation = XMMatrixTranslation(-0.2f, -0.5f, 1.0f);
	carLeftLight.modelMatrix = XMMatrixTranslation(-0.2f, -0.5f, 1.0f);
	carLeftLight.SetWorldMatrix(carLeftLight.modelMatrix);

	carLeftLight.SetModel(Model(device, Geometry::CreateSphere(0.05f)));
	carLeftLight.SetTexture(CreateTexture(device, XMINT4(0, 0, 0, 255)));
	carLeftLight.SetMaterial(material);
}

void Car::InitCarRightLight(ComPtr<ID3D11Device> device) {
	ComPtr<ID3D11ShaderResourceView> texture;
	Material material;
	material.Ambient = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	material.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	material.Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 16.0f);
	carRightLight.translation = XMMatrixTranslation(0.2f, -0.5f, 1.0f);
	carRightLight.modelMatrix = XMMatrixTranslation(0.2f, -0.5f, 1.0f);
	carRightLight.SetWorldMatrix(carRightLight.modelMatrix);

	carRightLight.SetModel(Model(device, Geometry::CreateSphere(0.05f)));
	carRightLight.SetTexture(CreateTexture(device, XMINT4(0, 0, 0, 255)));
	carRightLight.SetMaterial(material);
}

void Car::SetCarMaterial(Material material) {
	for (auto& car_item : carComponents)
	{
		car_item->SetMaterial(material);
	}
}

XMMATRIX Car::GetCarWorldMatrix() {
	return rotation * translation;
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

DirectX::XMFLOAT3 Car::GetCarLeftLightPosition() {
	return carLeftLight.GetPosition();
}

DirectX::XMFLOAT3 Car::GetCarRightLightPosition() {
	return carRightLight.GetPosition();
}

void Car::InitCar(ComPtr<ID3D11Device> device) {
	InitCarBody(device);
	InitCarFrontLeftWheel(device);
	InitCarFrontRightWheel(device);
	InitCarBackLeftWheel(device);
	InitCarBackRightWheel(device);
	InitCarLeftLight(device);
	InitCarRightLight(device);

	// 初始化AABB包围盒的最小值和最大值
	XMVECTOR vecMin = g_XMInfinity, vecMax = g_XMNegInfinity;

	for (auto& car_item : carComponents)
	{
		XMFLOAT3 tempMax = car_item->GetMaxPoint();
		XMFLOAT3 tempMin = car_item->GetMinPoint();

		XMVECTOR vectorMax = XMLoadFloat3(&tempMax);
		XMVECTOR vectorMin = XMLoadFloat3(&tempMin);

		// 更新AABB包围盒的顶点最大值和最小值
		vecMax = XMVectorMax(vecMax, vectorMax);
		vecMin = XMVectorMin(vecMin, vectorMin);
	}

	XMStoreFloat3(&vMax, vecMax);
	XMStoreFloat3(&vMin, vecMin);

	vMax = XMFLOAT3(0.6f, 0.0f, 1.2f);
	vMin = XMFLOAT3(-0.6f, -1.0f, -1.2f);

	// 创建包围盒
	BoundingBox::CreateFromPoints(boundingBox, XMLoadFloat3(&vMin), XMLoadFloat3(&vMax));
}

uint32_t ColorRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	return (r | (g << 8) | (b << 16) | (a << 24));
}

Car::ComPtr<ID3D11ShaderResourceView> Car::CreateTexture(ComPtr<ID3D11Device> md3dDevice, XMINT4 color) {
	uint32_t textureColor = ColorRGBA(color.x, color.y, color.z, color.w);
	ComPtr<ID3D11ShaderResourceView> texture;

	// 纹理内存映射，用白色初始化
	std::vector<uint32_t> textureArrayMap(128 * 128, textureColor);
	uint32_t(*textureMap)[128] = reinterpret_cast<uint32_t(*)[128]>(textureArrayMap.data());

	// 创建纹理数组
	D3D11_TEXTURE2D_DESC texArrayDesc;
	texArrayDesc.Width = 128;
	texArrayDesc.Height = 128;
	texArrayDesc.MipLevels = 1;
	texArrayDesc.ArraySize = 1;
	texArrayDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texArrayDesc.SampleDesc.Count = 1;      // 不使用多重采样
	texArrayDesc.SampleDesc.Quality = 0;
	texArrayDesc.Usage = D3D11_USAGE_DEFAULT;
	texArrayDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texArrayDesc.CPUAccessFlags = 0;
	texArrayDesc.MiscFlags = 0; // 指定需要生成mipmap

	D3D11_SUBRESOURCE_DATA sd;
	uint32_t * pData = textureArrayMap.data();
	sd.pSysMem = pData;
	sd.SysMemPitch = 128 * sizeof(uint32_t);
	sd.SysMemSlicePitch = 128 * 128 * sizeof(uint32_t);


	ComPtr<ID3D11Texture2D> tex;
	HR(md3dDevice->CreateTexture2D(&texArrayDesc, &sd, tex.GetAddressOf()));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	HR(md3dDevice->CreateShaderResourceView(tex.Get(), &srvDesc, texture.GetAddressOf()));

	return texture;
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

void Car::CarLightOnOff(ComPtr<ID3D11Device> md3dDevice) {
	carLightOn = !carLightOn;
	if (carLightOn) {
		carLeftLight.SetTexture(CreateTexture(md3dDevice, XMINT4(255, 255, 255, 255)));
		carRightLight.SetTexture(CreateTexture(md3dDevice, XMINT4(255, 255, 255, 255)));
	}
	else {
		carLeftLight.SetTexture(CreateTexture(md3dDevice, XMINT4(0, 0, 0, 255)));
		carRightLight.SetTexture(CreateTexture(md3dDevice, XMINT4(0, 0, 0, 255)));
	}
}

Car::CarState Car::GetCarState() {
	return mCarState;
}

bool Car::GetCarLightState() {
	return carLightOn;
}

DirectX::BoundingBox Car::GetLocalBoundingBox() const {
	return boundingBox;
	//return carBody.GetLocalBoundingBox();
}

DirectX::BoundingBox Car::GetBoundingBox() const {
	BoundingBox box;
	boundingBox.Transform(box, rotation * translation);
	return box;
}

DirectX::BoundingOrientedBox Car::GetBoundingOrientedBox() const {
	BoundingOrientedBox box;
	BoundingOrientedBox::CreateFromBoundingBox(box, boundingBox);
	box.Transform(box, rotation * translation);
	return box;
}

void Car::Draw(ComPtr<ID3D11DeviceContext> deviceContext, BasicEffect& effect) {
	for (auto carComponent : carComponents) {
		carComponent->Draw(deviceContext, effect);
	}
}