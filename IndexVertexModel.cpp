#include "IndexVertexModel.h"
#include <set>
#include "MMTFormatUtils.h"
#include "MMTLogUtils.h"


VertexPoint::VertexPoint(D3D11GameType d3d11GameType, std::vector<std::string> ElementList, std::vector<std::byte> SingPointVBBuf) {
	uint8_t offset = 0;
	for (std::string ElementName : ElementList) {
		uint8_t tmpByteWidth = d3d11GameType.ElementNameD3D11ElementMap[ElementName].ByteWidth;
		this->ElementName_ByteValueList_Map[ElementName] = MMTFormat_GetRange_Byte(SingPointVBBuf, offset, offset + tmpByteWidth);
		offset = offset + tmpByteWidth;
	}
}

IndexVertexModel::IndexVertexModel() {

}



void IndexVertexModel::OutputModel(std::wstring FmtOutputPath, std::wstring IBFileOutputPath, std::wstring VBFileOutputPath,uint32_t IBOffset) {
	this->fmtFile.OutputFmtFile(FmtOutputPath);
	this->ibBufFile.SaveToFile_UINT32(IBFileOutputPath, IBOffset);
	this->vbBufFile.SaveToFile(VBFileOutputPath);
}


void IndexVertexModel::SplitOutputByVertexGroupWWMI(std::wstring OutputFolder,std::wstring DrawIB) {
	std::map<int, std::set<int>> PartNameCount_VertexGroupIndexSet_Map;

	//先获取BLENDINDICES所在的索引偏移
	int BlendIndicesOffset = 0;
	for (std::string ElementName: fmtFile.ElementNameList) {
		
		int tmpByteWidth = fmtFile.d3d11GameType.ElementNameD3D11ElementMap[ElementName].ByteWidth;
		if (ElementName != "BLENDINDICES") {
			BlendIndicesOffset = BlendIndicesOffset + tmpByteWidth;
		}
		else {
			break;
		}
	}

	//再获取宽度
	int BlendIndicesByteWidth = fmtFile.d3d11GameType.ElementNameD3D11ElementMap["BLENDINDICES"].ByteWidth;

	//遍历每一个顶点数据
	for (int i = 0; i < this->vbBufFile.FinalVB0Buf.size(); i=i+ fmtFile.Stride) {
		VertexPoint vertexPoint(this->fmtFile.d3d11GameType,this->fmtFile.ElementNameList, MMTFormat_GetRange_Byte(vbBufFile.FinalVB0Buf, i , i + fmtFile.Stride));
		//获取权重数据
		std::vector<std::byte> BlendIndicesBuf = vertexPoint.ElementName_ByteValueList_Map["BLENDINDICES"];
		std::vector<std::byte> BlendWeightsBuf = vertexPoint.ElementName_ByteValueList_Map["BLENDWEIGHTS"];
		
		//这里要逐个获取，到底在哪个partName里出现了，获取所有出现的partName
		std::set<int> findPartNameCountSet;

		for (int tmpIndex = 0; tmpIndex < 4; tmpIndex++) {
			std::byte blendIndicesByteValue = BlendIndicesBuf[tmpIndex];
			std::byte blendWeightsByteValue = BlendWeightsBuf[tmpIndex];

			uint8_t blendIndicesNumValue = static_cast<uint8_t>(blendIndicesByteValue);
			uint8_t blendWeightsNumValue = static_cast<uint8_t>(blendIndicesByteValue);
			if (PartNameCount_VertexGroupIndexSet_Map.size() != 0) {
				//否则需要逐个判断
				bool findVertexGroupIndex = false;
				for (const auto& pair : PartNameCount_VertexGroupIndexSet_Map) {
					int partNameCount = pair.first;
					std::set<int> VertexGroupIndexList = pair.second;
					for (int vgindex : VertexGroupIndexList) {
						if (vgindex == blendIndicesNumValue) {
							findPartNameCountSet.insert(partNameCount);
						}
					}

				}
			}
		}

		//如果没找到，那说明是第一个，要做特殊处理。
		if (findPartNameCountSet.size() == 0) {
			std::set<int> NewVertexGroupIndexList;
			for (std::byte byteValue : BlendIndicesBuf) {
				uint8_t num = static_cast<uint8_t>(byteValue);
				NewVertexGroupIndexList.insert(num);
			}
			PartNameCount_VertexGroupIndexSet_Map[0] = NewVertexGroupIndexList;
		}
		else if (findPartNameCountSet.size() == 1) {
			//四个数字中任意一个数字出现在顶点组索引中，就说明剩下的顶点组索引和当前的顶点组索引是相连的
			//这里分为只在一个里面出现，以及在多个里面出现的情况
			//如果只在其中一个出现，则直接扩充VertexGroupIndex即可
			int firstPartNameCount = 0;
			for (int partNameCount: findPartNameCountSet) {
				firstPartNameCount = partNameCount;
				break;
			}

			std::set<int> NewVertexGroupIndexSet = PartNameCount_VertexGroupIndexSet_Map[firstPartNameCount];
			for (std::byte byteValue : BlendIndicesBuf) {
				uint8_t num = static_cast<uint8_t>(byteValue);
				NewVertexGroupIndexSet.insert(num);
			}
			PartNameCount_VertexGroupIndexSet_Map[firstPartNameCount] = NewVertexGroupIndexSet;
		}
		else if (findPartNameCountSet.size() > 1) {
			//如果在多个里面出现，则需要把这多个融合为1个，并生成新的PartNameCount_VertexGroupIndexSet_Map来替换原本的
			LOG.Error("Show in multiple VertexGroup combination!");
		}


	}

	//输出查看每个里面的内容
	for (const auto& pair: PartNameCount_VertexGroupIndexSet_Map) {
		int partNameCount = pair.first;
		std::set<int> VertexGroupIndexList = pair.second;
		LOG.Info("PartNameCount: " + std::to_string(partNameCount));
		for (int vgindex : VertexGroupIndexList) {
			LOG.Info("VertexGroupIndex: " + std::to_string(vgindex));
		}
		LOG.NewLine();
	}


}