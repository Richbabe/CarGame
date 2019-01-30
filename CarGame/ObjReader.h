// *******************************************************************************
// - ObjReader֧��ͨ��.obj�ļ�����.mtl(����)������.mtl(����)֧����������
// - ��֧��ʹ��/����һ�е�����������һ���ʾһ��
// - ��֧������Ϊ����
// - ��֧��ʹ������1//2�����Ķ��㣨����������������Ķ��㣩
// - ��.mtl�ļ�����������ñ��������·������ʽ�ṩ����û��֧��.��..����·����ʽ��
// - ��.mtl�����ļ������ڣ����ڲ���ʹ��Ĭ�ϲ���ֵ
// - ��.mtl�ڲ�û��ָ�������ļ����ã���Ҫ�������м�������
// - Ҫ������ֻ���������ι���
// - .mbo�ļ���һ�ֶ������ļ������ڼӿ�ģ�ͼ��ص��ٶȣ��ڲ���ʽ���Զ����
// - .mbo�ļ��Ѿ����ɲ�������ı��ļ�λ�ã���ҪǨ������ļ���Ҫ��������.mbo�ļ�
// *******************************************************************************

#ifndef OBJREADER_H
#define OBJREADER_H

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <map>
#include <algorithm>
#include <locale>
#include <filesystem>
#include "Vertex.h"
#include "LightHelper.h"


class MtlReader;

class ObjReader
{
public:
	struct ObjPart
	{
		Material material;							// ����
		std::vector<VertexPosNormalTex> vertices;	// ���㼯��
		std::vector<WORD> indices16;				// ������������65535ʱʹ��
		std::vector<DWORD> indices32;				// ����������65535ʱʹ��
		std::wstring texStrDiffuse;					// ����������ļ�������Ϊ���·������mbo����ռ260�ֽ�
	};

	// ָ��.mbo�ļ�������£���.mbo�ļ����ڣ����ȶ�ȡ���ļ�
	// ������ȡ.obj�ļ�
	// ��.obj�ļ�����ȡ�����ṩ��.mbo�ļ���·�����������Ѿ���ȡ�����ݴ���.mbo�ļ�
	bool Read(const wchar_t* mboFileName, const wchar_t* objFileName);
	
	bool ReadObj(const wchar_t* objFileName);
	bool ReadMbo(const wchar_t* mboFileName);
	bool WriteMbo(const wchar_t* mboFileName);
public:
	std::vector<ObjPart> objParts;
	DirectX::XMFLOAT3 vMin, vMax;					// AABB��˫����
private:
	void AddVertex(const VertexPosNormalTex& vertex, DWORD vpi, DWORD vti, DWORD vni);

	// ������v/vt/vn�ַ�����Ϣ
	std::unordered_map<std::wstring, DWORD> vertexCache;
};

class MtlReader
{
public:
	bool ReadMtl(const wchar_t* mtlFileName);


public:
	std::map<std::wstring, Material> materials;
	std::map<std::wstring, std::wstring> mapKdStrs;
};


#endif
