#pragma once
#include "IndexBufferBufFile.h"
#include "VertexBufferBufFile.h"
#include "FmtData.h"

//TODO ������������ڲ��ԣ��������ҵ��˸��򵥵ķ������Ǿ��Ǵ�log�ļ��ж�ȡ����������������ʱ�����ˣ����ǲ�Ҫɾ��
//��һ��������������������ݽṹ��

class VertexPoint {
public:
	std::unordered_map<std::string, std::vector<std::byte>> ElementName_ByteValueList_Map;

	VertexPoint(D3D11GameType d3d11GameType, std::vector<std::string> ElementList, std::vector<std::byte> SingPointVBBuf);
};


class IndexVertexModel {
public:
	IndexBufferBufFile ibBufFile;
	VertexBufferBufFile vbBufFile;
	FmtFileData fmtFile;

	void SplitOutputByVertexGroupWWMI(std::wstring OutputFolder, std::wstring DrawIB);
	void OutputModel(std::wstring FmtOutputPath, std::wstring IBFileOutputPath, std::wstring VBFileOutputPath, uint32_t IBOffset);
	IndexVertexModel();
};