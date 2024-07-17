#pragma once
#include <iostream>
#include <cstdint>
#include <vector>

class IndexBufferBufFile {
public:

	//��С�Ķ�����ֵ
	uint32_t MinNumber = -1;

	//���Ķ�����ֵ
	uint32_t MaxNumber = -1;

	//�ܹ��м�������
	uint32_t NumberCount = -1;

	//�ܹ��õ��Ķ�����������
	uint32_t UniqueVertexCount = -1;

	//�����б�
	std::vector<uint32_t> NumberList;

	IndexBufferBufFile();

	//IB�ļ�����·���������ִ�Сд�ĸ�ʽ����DXGI_FORMAT_R32_UINT
	IndexBufferBufFile(std::wstring FileReadPath,std::wstring Format);

	void SelfDivide(int FirstIndex,int IndexCount);

	void SaveToFile_UINT32(std::wstring FileWritePath, uint32_t Offset);
	void SaveToFile_UINT16(std::wstring FileWritePath, uint16_t Offset);

	void SaveToFile_MinSize(std::wstring FileWritePath, uint32_t Offset);
	std::string GetSelfMinFormat();
};