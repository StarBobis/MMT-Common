#pragma once
#include "IndexBufferBufFile.h"
#include "VertexBufferBufFile.h"
#include "FmtData.h"

//TODO 这里仅仅是用于测试，测试完找到了更简单的方法，那就是从log文件中读取，所以我们这里暂时废弃了，但是不要删除
//万一后面真的用上了这种数据结构。

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