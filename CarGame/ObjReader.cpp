#include "ObjReader.h"

using namespace DirectX;
using namespace std::experimental;
bool ObjReader::Read(const wchar_t * mboFileName, const wchar_t * objFileName)
{
	// ���ָ����.mbo�ļ���.mbo�ļ����ڣ����ȶ�ȡ.mbo�ļ�
	if (mboFileName && filesystem::exists(mboFileName))
	{
		return ReadMbo(mboFileName);
	}
	else if (objFileName && filesystem::exists(objFileName))
	{
		bool status = ReadObj(objFileName);
		// ��ȡ.obj�ļ�����ݶ�ȡ�����ݴ���.mbo�ļ�
		if (status && mboFileName)
			return WriteMbo(mboFileName);
		return status;
	}

	return false;
}

bool ObjReader::ReadObj(const wchar_t * objFileName)
{
	// �������
	objParts.clear();
	vertexCache.clear();

	MtlReader mtlReader;

	std::vector<XMFLOAT3>   positions;  // ���涥������
	std::vector<XMFLOAT3>   normals;    // ���淨����
	std::vector<XMFLOAT2>   texCoords;  // ������������

	// ��ʼ��AABB��Χ�е���Сֵ�����ֵ
	XMVECTOR vecMin = g_XMInfinity, vecMax = g_XMNegInfinity;

	std::wifstream wfin(objFileName);
	// �л�����
	std::locale china("chs");
	wfin.imbue(china);
	for (;;)
	{
		std::wstring wstr;
		if (!(wfin >> wstr))
			break;

		if (wstr[0] == '#')
		{
			//
			// ���������ע�ͣ������ע��������
			//
			while (!wfin.eof() && wfin.get() != '\n')
				continue;
		}
		else if (wstr == L"o" || wstr == L"g")
		{
			// 
			// ������(����)
			//
			objParts.emplace_back(ObjPart());
			// �ṩĬ�ϲ���
			objParts.back().material.Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
			objParts.back().material.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
			objParts.back().material.Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

			vertexCache.clear();
		}
		else if (wstr == L"v")
		{
			//
			// ����λ��
			//

			// ע��objʹ�õ�����������ϵ����������������ϵ
			// ��Ҫ��zֵ��ת
			XMFLOAT3 pos;
			wfin >> pos.x >> pos.y >> pos.z;
			pos.z = -pos.z;
			positions.push_back(pos);
			XMVECTOR vecPos = XMLoadFloat3(&pos);
			// ����AABB��Χ�еĶ������ֵ����Сֵ
			vecMax = XMVectorMax(vecMax, vecPos);
			vecMin = XMVectorMin(vecMin, vecPos);
		}
		else if (wstr == L"vt")
		{
			//
			// ������������
			//

			// ע��objʹ�õ��ǵѿ�������ϵ����������������ϵ
			float u, v;
			wfin >> u >> v;
			v = 1.0f - v;  // ��Ҫ��vֵ����ת��
			texCoords.emplace_back(XMFLOAT2(u, v));
		}
		else if (wstr == L"vn")
		{
			//
			// ���㷨����
			//

			// ע��objʹ�õ�����������ϵ����������������ϵ
			// ��Ҫ��zֵ��ת
			float x, y, z;
			wfin >> x >> y >> z;
			z = -z;
			normals.emplace_back(XMFLOAT3(x, y, z));
		}
		else if (wstr == L"mtllib")
		{
			//
			// ָ��ĳһ�ļ��Ĳ���
			//
			std::wstring mtlFile;
			wfin >> mtlFile;
			// ȥ��ǰ��ո�
			size_t beg = 0, ed = mtlFile.size();
			while (iswspace(mtlFile[beg]))
				beg++;
			while (ed > beg && iswspace(mtlFile[ed - 1]))
				ed--;
			mtlFile = mtlFile.substr(beg, ed - beg);
			// ��ȡ·��
			std::wstring dir = objFileName;
			size_t pos;
			if ((pos = dir.find_last_of('/')) == std::wstring::npos &&
				(pos = dir.find_last_of('\\')) == std::wstring::npos)
			{
				pos = 0;
			}
			else
			{
				pos += 1;
			}
				

			mtlReader.ReadMtl((dir.erase(pos) + mtlFile).c_str());
		}
		else if (wstr == L"usemtl")
		{
			//
			// ʹ��֮ǰָ���ļ��ڲ���ĳһ����
			//
			std::wstring mtlName;
			std::getline(wfin, mtlName);
			// ȥ��ǰ��ո�
			size_t beg = 0, ed = mtlName.size();
			while (iswspace(mtlName[beg]))
				beg++;
			while (ed > beg && iswspace(mtlName[ed - 1]))
				ed--;
			mtlName = mtlName.substr(beg, ed - beg);

			objParts.back().material = mtlReader.materials[mtlName];
			objParts.back().texStrDiffuse = mtlReader.mapKdStrs[mtlName];
		}
		else if (wstr == L"f")
		{
			//
			// ������
			//
			VertexPosNormalTex vertex;
			DWORD vpi[3], vni[3], vti[3];
			wchar_t ignore;

			// ����λ������/������������/����������
			// ԭ����������ϵ�¶���˳������ʱ���Ų�
			// ������Ҫת��Ϊ��������ϵ����Ҫ�������ζ��㷴��������
			for (int i = 2; i >= 0; --i)
			{
				wfin >> vpi[i] >> ignore >> vti[i] >> ignore >> vni[i];
			}

			for (int i = 0; i < 3; ++i)
			{
				vertex.pos = positions[vpi[i] - 1];
				vertex.normal = normals[vni[i] - 1];
				vertex.tex = texCoords[vti[i] - 1];
				AddVertex(vertex, vpi[i], vti[i], vni[i]);
			}
			

			while (iswblank(wfin.peek()))
				wfin.get();
			// �����涥�������ܳ�����3����֧�ָø�ʽ
			if (wfin.peek() != '\n')
				return false;
		}
	}

	// ������������WORD�����ֵ�Ļ���ʹ��16λWORD�洢
	for (auto& part : objParts)
	{
		if (part.vertices.size() < 65535)
		{
			for (auto& i : part.indices32)
			{
				part.indices16.push_back((WORD)i);
			}
			part.indices32.clear();
		}
	}

	XMStoreFloat3(&vMax, vecMax);
	XMStoreFloat3(&vMin, vecMin);

	return true;
}

bool ObjReader::ReadMbo(const wchar_t * mboFileName)
{
	// [Part��Ŀ] 4�ֽ�
	// [AABB�ж���vMax] 12�ֽ�
	// [AABB�ж���vMin] 12�ֽ�
	// [Part
	//   [���������ļ���]520�ֽ�
	//   [����]64�ֽ�
	//   [������]4�ֽ�
	//   [������]4�ֽ�
	//   [����]32*������ �ֽ�
	//   [����]2(��4)*������ �ֽڣ�ȡ���ڶ������Ƿ񲻳���65535
	// ]
	// ...
	std::ifstream fin(mboFileName, std::ios::in | std::ios::binary);
	if (!fin.is_open())
		return false;

	UINT parts = (UINT)objParts.size();
	// [Part��Ŀ] 4�ֽ�
	fin.read(reinterpret_cast<char*>(&parts), sizeof(UINT));
	objParts.resize(parts);

	// [AABB�ж���vMax] 12�ֽ�
	fin.read(reinterpret_cast<char*>(&vMax), sizeof(XMFLOAT3));
	// [AABB�ж���vMin] 12�ֽ�
	fin.read(reinterpret_cast<char*>(&vMin), sizeof(XMFLOAT3));


	for (UINT i = 0; i < parts; ++i)
	{
		wchar_t filePath[MAX_PATH];
		// [���������ļ���]520�ֽ�
		fin.read(reinterpret_cast<char*>(filePath), MAX_PATH * sizeof(wchar_t));
		objParts[i].texStrDiffuse = filePath;
		// [����]64�ֽ�
		fin.read(reinterpret_cast<char*>(&objParts[i].material), sizeof(Material));
		UINT vertexCount, indexCount;
		// [������]4�ֽ�
		fin.read(reinterpret_cast<char*>(&vertexCount), sizeof(UINT));
		// [������]4�ֽ�
		fin.read(reinterpret_cast<char*>(&indexCount), sizeof(UINT));
		// [����]32*������ �ֽ�
		objParts[i].vertices.resize(vertexCount);
		fin.read(reinterpret_cast<char*>(objParts[i].vertices.data()), vertexCount * sizeof(VertexPosNormalTex));

		if (vertexCount > 65535)
		{
			// [����]4*������ �ֽ�
			objParts[i].indices32.resize(indexCount);
			fin.read(reinterpret_cast<char*>(objParts[i].indices32.data()), indexCount * sizeof(DWORD));
		}
		else
		{
			// [����]2*������ �ֽ�
			objParts[i].indices16.resize(indexCount);
			fin.read(reinterpret_cast<char*>(objParts[i].indices16.data()), indexCount * sizeof(WORD));
		}
	}

	fin.close();

	return true;
}

bool ObjReader::WriteMbo(const wchar_t * mboFileName)
{
	// [Part��Ŀ] 4�ֽ�
	// [AABB�ж���vMax] 12�ֽ�
	// [AABB�ж���vMin] 12�ֽ�
	// [Part
	//   [����������ļ���]520�ֽ�
	//   [���������ļ���]520�ֽ�
	//   [����]64�ֽ�
	//   [������]4�ֽ�
	//   [������]4�ֽ�
	//   [����]32*������ �ֽ�
	//   [����]2(��4)*������ �ֽڣ�ȡ���ڶ������Ƿ񲻳���65535
	// ]
	// ...
	std::ofstream fout(mboFileName, std::ios::out | std::ios::binary);
	UINT parts = (UINT)objParts.size();
	// [Part��Ŀ] 4�ֽ�
	fout.write(reinterpret_cast<const char*>(&parts), sizeof(UINT));

	// [AABB�ж���vMax] 12�ֽ�
	fout.write(reinterpret_cast<const char*>(&vMax), sizeof(XMFLOAT3));
	// [AABB�ж���vMin] 12�ֽ�
	fout.write(reinterpret_cast<const char*>(&vMin), sizeof(XMFLOAT3));

	// [Part
	for (UINT i = 0; i < parts; ++i)
	{
		wchar_t filePath[MAX_PATH];
		wcscpy_s(filePath, objParts[i].texStrDiffuse.c_str());
		// [���������ļ���]520�ֽ�
		fout.write(reinterpret_cast<const char*>(filePath), MAX_PATH * sizeof(wchar_t));
		// [����]64�ֽ�
		fout.write(reinterpret_cast<const char*>(&objParts[i].material), sizeof(Material));
		UINT vertexCount = (UINT)objParts[i].vertices.size();
		// [������]4�ֽ�
		fout.write(reinterpret_cast<const char*>(&vertexCount), sizeof(UINT));

		UINT indexCount;
		if (vertexCount > 65535)
		{
			indexCount = (UINT)objParts[i].indices32.size();
			// [������]4�ֽ�
			fout.write(reinterpret_cast<const char*>(&indexCount), sizeof(UINT));
			// [����]32*������ �ֽ�
			fout.write(reinterpret_cast<const char*>(objParts[i].vertices.data()), vertexCount * sizeof(VertexPosNormalTex));
			// [����]4*������ �ֽ�
			fout.write(reinterpret_cast<const char*>(objParts[i].indices32.data()), indexCount * sizeof(DWORD));
		}
		else
		{
			indexCount = (UINT)objParts[i].indices16.size();
			// [������]4�ֽ�
			fout.write(reinterpret_cast<const char*>(&indexCount), sizeof(UINT));
			// [����]32*������ �ֽ�
			fout.write(reinterpret_cast<const char*>(objParts[i].vertices.data()), vertexCount * sizeof(VertexPosNormalTex));
			// [����]2*������ �ֽ�
			fout.write(reinterpret_cast<const char*>(objParts[i].indices16.data()), indexCount * sizeof(WORD));
		}
	}
	// ]
	fout.close();

	return true;
}

void ObjReader::AddVertex(const VertexPosNormalTex& vertex, DWORD vpi, DWORD vti, DWORD vni)
{
	std::wstring idxStr = std::to_wstring(vpi) + L"/" + std::to_wstring(vti) + L"/" + std::to_wstring(vni);

	// Ѱ���Ƿ����ظ�����
	auto it = vertexCache.find(idxStr);
	if (it != vertexCache.end())
	{
		objParts.back().indices32.push_back(it->second);
	}
	else
	{
		objParts.back().vertices.push_back(vertex);
		DWORD pos = (DWORD)objParts.back().vertices.size() - 1;
		vertexCache[idxStr] = pos;
		objParts.back().indices32.push_back(pos);
	}
}



bool MtlReader::ReadMtl(const wchar_t * mtlFileName)
{
	materials.clear();
	mapKdStrs.clear();


	std::wifstream wfin(mtlFileName);
	std::locale china("chs");
	wfin.imbue(china);


	if (!wfin.is_open())
		return false;

	std::wstring wstr;
	std::wstring currMtl;
	for (;;)
	{
		if (!(wfin >> wstr))
			break;

		if (wstr[0] == '#')
		{
			//
			// ����ע��������
			//
			while (wfin.get() != '\n')
				continue;
		}
		else if (wstr == L"newmtl")
		{
			//
			// �²���
			//

			std::getline(wfin, currMtl);
			// ȥ��ǰ��ո�
			size_t beg = 0, ed = currMtl.size();
			while (iswspace(currMtl[beg]))
				beg++;
			while (ed > beg && iswspace(currMtl[ed - 1]))
				ed--;
			currMtl = currMtl.substr(beg, ed - beg);
		}
		else if (wstr == L"Ka")
		{
			//
			// �����ⷴ����ɫ
			//
			XMFLOAT4& ambient = materials[currMtl].Ambient;
			wfin >> ambient.x >> ambient.y >> ambient.z;
			if (ambient.w == 0.0f)
				ambient.w = 1.0f;
		}
		else if (wstr == L"Kd")
		{
			//
			// ����ⷴ����ɫ
			//
			XMFLOAT4& diffuse = materials[currMtl].Diffuse;
			wfin >> diffuse.x >> diffuse.y >> diffuse.z;
			if (diffuse.w == 0.0f)
				diffuse.w = 1.0f;
		}
		else if (wstr == L"Ks")
		{
			//
			// ����ⷴ����ɫ
			//
			XMFLOAT4& specular = materials[currMtl].Specular;
			wfin >> specular.x >> specular.y >> specular.z;
		}
		else if (wstr == L"Ns")
		{
			//
			// ����ϵ��
			//
			wfin >> materials[currMtl].Specular.w;
		}
		else if (wstr == L"d" || wstr == L"Tr")
		{
			//
			// dΪ��͸���� TrΪ͸����
			//
			float alpha;
			wfin >> alpha;
			if (wstr == L"Tr")
				alpha = 1.0f - alpha;
			materials[currMtl].Ambient.w = alpha;
			materials[currMtl].Diffuse.w = alpha;
		}
		else if (wstr == L"map_Kd")
		{
			//
			// map_KdΪ������ʹ�õ�����
			//
			std::wstring fileName;
			std::getline(wfin, fileName);
			// ȥ��ǰ��ո�
			size_t beg = 0, ed = fileName.size();
			while (iswspace(fileName[beg]))
				beg++;
			while (ed > beg && iswspace(fileName[ed - 1]))
				ed--;
			fileName = fileName.substr(beg, ed - beg);

			// ׷��·��
			std::wstring dir = mtlFileName;
			size_t pos;
			if ((pos = dir.find_last_of('/')) == std::wstring::npos &&
				(pos = dir.find_last_of('\\')) == std::wstring::npos)
				pos = 0;
			else
				pos += 1;

			mapKdStrs[currMtl] = dir.erase(pos) + fileName;
		}
	}

	return true;
}

