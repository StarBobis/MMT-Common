#pragma once
#include <iostream>
#include "D3d11GameType.h"



class VertexBufferBufFile {
public:
	//���ڶ�ȡ.ib .vb��vb�ļ����������ʹ洢����������ָ�λbuf�ļ�
	std::unordered_map<std::wstring, std::vector<std::byte>> CategoryVBDataMap;

	//���ڶ�ȡFA�ļ����зֳɲ�ͬ����BUF�ļ�����ϳ������������������
	std::vector<std::byte> FinalVB0Buf;

	VertexBufferBufFile();
	
	VertexBufferBufFile(std::wstring readVBBufFilePath,D3D11GameType d3d11GameType,std::vector<std::string> elementList);

	void SelfDivide(int MinNumber,int MaxNumber,int stride);
	void SaveToFile(std::wstring VB0OutputFilePath);
};