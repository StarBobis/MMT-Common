#include "VertexBufferTxtFileDetect.h"
#include "VertexBufferTxtFile.h"

#include "MMTStringUtils.h"
#include "MMTFileUtils.h"
#include "MMTLogUtils.h"

VertexBufferDetect::VertexBufferDetect() {

}


VertexBufferDetect::VertexBufferDetect(std::wstring VBFilePath) {
	//���ݴ����VBFilePath����ȡ��ǰ��vb��
	//LOG.LogOutput(L"Try to parse vbSlotNumber: ");
	std::wstring txtFileName = MMTString_GetFileNameFromFilePath(VBFilePath);
	std::wstring vbSlotNumber = txtFileName.substr(9, 1);
	std::wstring currentIndex = txtFileName.substr(0, 6);
	//LOG.Info(L"Current VB SlotNumber: " + vbSlotNumber);
	this->vbSlotNumber = std::stoi(vbSlotNumber);
	this->Index = currentIndex;
	//����ÿ��Trianglelist��VB�ļ�
	std::wifstream file(VBFilePath);
	if (!file.is_open()) {
		LOG.Error(L"Can't open: " + VBFilePath);
	}
	else {
		//LOG.Info(L"Parsing ElementList For: " + VBFilePath);
	}

	//�����õ��ı���
	std::wstring line;

	std::string topology = "";
	int fileShowStride = 0;

	while (std::getline(file, line)) {
		boost::algorithm::to_lower(line);
		boost::algorithm::trim(line);

		// 1.���Ƚ����ļ�����ʾ��stride
		std::wstring vertexCountStr = L"vertex count: ";
		std::wstring topologyStr = L"topology: ";
		if (boost::algorithm::starts_with(line, "stride")) {
			std::wstring str;
			std::size_t colonPos = line.find(':');
			if (colonPos != std::wstring::npos) {
				// ��ȡð�ź���������ַ���+1 ��Ϊ������ð�ű���
				str = line.substr(colonPos + 1);
				boost::algorithm::trim(str);
			}
			fileShowStride = std::stoi(str);
			this->fileShowStride = fileShowStride;
			//LOG.LogOutput(L"Detect file show stride:" + str);
		}
		else if (boost::algorithm::starts_with(line, topologyStr)) {
			std::wstring valueStr = line.substr(topologyStr.length());
			boost::algorithm::trim(valueStr);
			topology = MMTString_ToByteString(valueStr);
			this->Topology = valueStr;
		}
		else if (boost::algorithm::starts_with(line, vertexCountStr)) {
			std::wstring valueStr = line.substr(vertexCountStr.length());
			boost::algorithm::trim(valueStr);
			this->fileShowVertexCount = std::stoi(valueStr);
		}

		if (boost::algorithm::starts_with(line, "element")) {
			break;
		}
	}
	file.close();


	VertexBufferTxtFile vertexBufferFileData;
	std::vector<D3D11Element> showD3d11ElementList = vertexBufferFileData.parseShowElementList(VBFilePath);
	//LOG.Info(L"Show D3D11Element List: ");
	std::vector<std::string> showD3D11ElementNameList;
	for (D3D11Element showD3d11Element : showD3d11ElementList) {
		//LOG.Info(showD3d11Element.SemanticName);
		showD3D11ElementNameList.push_back(showD3d11Element.SemanticName);
	}
	this->showElementNameList = showD3D11ElementNameList;

	std::vector<std::string> readD3D11ElementNameList = vertexBufferFileData.parseRealElementList(VBFilePath, true);
	//LOG.Info(L"real D3D11Element List: ");
	//for (std::string realD3D11ElementName : readD3D11ElementNameList) {
	//	LOG.Info(MMTString_ToWideString(realD3D11ElementName));
	//}

	//�ڽ���RealElementListʱ�����ǵ���ʵVertexCount�ͱ����ú��ˣ�����Ҫ��ֵ��ȥ
	this->fileRealVertexCount = std::stoi(vertexBufferFileData.VertexCount);

	//LOG.Info(L"Start to add real d3d11Element:");
	std::vector<D3D11Element> realD3D11ElementList;
	for (D3D11Element d3d11Element : showD3d11ElementList) {
		//�������SemanticIndex֮������֣����������readD3D11ElementNameList�в���
		std::string d3d11ElementName = "";
		if (d3d11Element.SemanticIndex != "0") {
			d3d11ElementName = d3d11Element.SemanticName + d3d11Element.SemanticIndex;
		}
		else {
			d3d11ElementName = d3d11Element.SemanticName;
		}

		//�����б�����Ƿ����������֣���������Ӳ�break
		for (std::string elementName : readD3D11ElementNameList) {
			boost::algorithm::to_lower(elementName);
			if (elementName == d3d11ElementName) {
				//LOG.Info(L"Add: " + MMTString_ToWideString(elementName));
				realD3D11ElementList.push_back(d3d11Element);
				break;
			}
		}

	}
	//LOG.Info(L"Add real d3d11Element over.");

	// ������Ҫ���ȸ���alignedbyteoffsetȷ���������һ��Ԫ�صĸ���Ԫ�صĲ���
	int lastAlignedByteOffset = 0;
	std::string lastSemanticName = "";
	int lastByteWidth = 0;
	std::unordered_map<std::string, int> semanticNameByteWidthMap;
	for (size_t i = 0; i < realD3D11ElementList.size(); i++)
	{
		D3D11Element d3d11Element = realD3D11ElementList[i];
		//LOG.LogOutput(L"Processing: " + d3d11Element.SemanticName);
		if (i == 0) {
			lastSemanticName = d3d11Element.SemanticName;
			lastAlignedByteOffset = d3d11Element.AlignedByteOffset;
			continue;
		}
		else {
			lastByteWidth = d3d11Element.AlignedByteOffset - lastAlignedByteOffset;
			semanticNameByteWidthMap[lastSemanticName] = lastByteWidth;
			lastSemanticName = d3d11Element.SemanticName;
			lastAlignedByteOffset = d3d11Element.AlignedByteOffset;
		}
	}

	//׼��������ʵ��ElementList�б������ʵ���ļ�����
	int fileRealStride = 0;
	if (realD3D11ElementList.size() >= 1) {
		//�������һ�������ݱ�ǵ�stride�����
		D3D11Element lastD3D11Element = realD3D11ElementList[realD3D11ElementList.size() - 1];
		lastByteWidth = fileShowStride - lastD3D11Element.AlignedByteOffset;
		semanticNameByteWidthMap[lastD3D11Element.SemanticName] = lastByteWidth;
		//LOG.LogOutputSplitStr();

		//LOG.LogOutput(L"SemanticName's stride calculated by file read stride: ");
		for (const auto& pair : semanticNameByteWidthMap) {
			std::string semanticName = pair.first;
			int byteWidth = pair.second;
			//LOG.LogOutput(L"SemanticName: " + semanticName + L" ByteWidth: " + std::to_wstring(byteWidth));
		}
		//LOG.LogOutputSplitStr();
		//���ƴ�ӳ�һ��d3d11ElementName-d3d11Element��Map
		//�����ļ�����ʾ��Stride����������Stride����������������Ҫ��ÿһ��
		std::unordered_map<std::string, D3D11Element>  elementNameD3D11ElementMap;

		//����Խ����Խ�ã���ʱ��������������Ҫ�ٲ���
		std::unordered_map<std::string, int>  d3d11FormatByteWidthMap;
		d3d11FormatByteWidthMap["r32g32b32_float"] = 12;
		d3d11FormatByteWidthMap["r32g32b32a32_float"] = 16;
		d3d11FormatByteWidthMap["r8g8b8a8_unorm"] = 4;
		d3d11FormatByteWidthMap["r8g8b8a8_snorm"] = 4;
		d3d11FormatByteWidthMap["r32g32_float"] = 8;
		d3d11FormatByteWidthMap["r32g32_uint"] = 8;
		d3d11FormatByteWidthMap["r32g32_sint"] = 8;
		d3d11FormatByteWidthMap["r16g16_float"] = 4;
		d3d11FormatByteWidthMap["r16g16_sint"] = 4;
		d3d11FormatByteWidthMap["r16g16_uint"] = 4;
		d3d11FormatByteWidthMap["r32g32b32a32_sint"] = 16;
		d3d11FormatByteWidthMap["r32g32b32a32_uint"] = 16;
		d3d11FormatByteWidthMap["r16g16b16a16_float"] = 8;
		d3d11FormatByteWidthMap["r32_uint"] = 4;
		d3d11FormatByteWidthMap["r32_float"] = 4;

		//LOG.Info(L"Real d3d11 element name byte width: ");
		//����d3d11Element�б���ֵ
		for (D3D11Element d3d11Element : realD3D11ElementList) {
			// ������Ҫ���Ǹ���һ��d3d11 format��byte width��map����ȡ
			if (!d3d11FormatByteWidthMap.contains(d3d11Element.Format)) {
				LOG.Error("Unknown d3d11 format" + d3d11Element.Format + " not in d3d11ElementNameByteWidthMap!");
			}
			d3d11Element.ByteWidth = d3d11FormatByteWidthMap[d3d11Element.Format];
			d3d11Element.ExtractTechnique = topology;
			std::string d3d11ElementName = d3d11Element.SemanticName;
			if (d3d11Element.SemanticIndex != "0") {
				d3d11ElementName = d3d11ElementName + d3d11Element.SemanticIndex;
			}

			//LOG.Info("real add real stride ElementName: " + d3d11ElementName + " ByteWidth: " + std::to_string(d3d11Element.ByteWidth));
			elementNameD3D11ElementMap[d3d11ElementName] = d3d11Element;
			this->realElementNameList.push_back(d3d11ElementName);
			this->realElementList.push_back(d3d11Element);
			fileRealStride += d3d11Element.ByteWidth;
		}

		int fileElementListShowStride = 0;
		for (D3D11Element d3d11Element : showD3d11ElementList) {
			// ������Ҫ���Ǹ���һ��d3d11 format��byte width��map����ȡ
			if (!d3d11FormatByteWidthMap.contains(d3d11Element.Format)) {
				LOG.Error("Unknown d3d11 format" + d3d11Element.Format + " not in d3d11ElementNameByteWidthMap!");
			}
			d3d11Element.ByteWidth = d3d11FormatByteWidthMap[d3d11Element.Format];
			std::string d3d11ElementName = d3d11Element.SemanticName;
			if (d3d11Element.SemanticIndex != "0") {
				d3d11ElementName = d3d11ElementName + d3d11Element.SemanticIndex;
			}
			//LOG.Info(" add show stride ElementName: " + d3d11ElementName + " ByteWidth: " + std::to_string(d3d11Element.ByteWidth));
			fileElementListShowStride += d3d11Element.ByteWidth;
		}

		this->fileRealStride = fileRealStride;
		this->elementNameD3D11ElementMap = elementNameD3D11ElementMap;
	}
	else {
		this->fileRealStride = fileRealStride;
	}

	// ��buf�ļ��л�ȡ��Ӧ����
	
	std::wstring txtFileNamePrefix = txtFileName.substr(0,txtFileName.length() - 4);
	std::wstring bufFileName = txtFileNamePrefix + L".buf";
	std::wstring txtFilePath = MMTString_GetFolderPathFromFilePath(VBFilePath);
	std::wstring bufFilePath = txtFilePath + L"/" + bufFileName;
	// ȷ����ȡ������
	int countStride = fileShowStride;
	if (countStride == 0) {
		countStride = fileRealStride;
	}
	//LOG.Info(L".buf file path: " + bufFilePath);
	int fileSize = MMTFile_GetFileSize(bufFilePath);
	this->fileBufferVertexCount = fileSize / countStride;
	//LOG.Info(L"file buffer vertex count: " + std::to_wstring(this->fileBufferVertexCount));
	//LOG.NewLine();
	uint64_t fileRealSize = MMTFile_GetRealFileSize_NullTerminated(bufFilePath);
	this->fileBufferRealVertexCount = fileRealSize / countStride;

}