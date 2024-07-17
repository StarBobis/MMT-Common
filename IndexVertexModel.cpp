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

	//�Ȼ�ȡBLENDINDICES���ڵ�����ƫ��
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

	//�ٻ�ȡ���
	int BlendIndicesByteWidth = fmtFile.d3d11GameType.ElementNameD3D11ElementMap["BLENDINDICES"].ByteWidth;

	//����ÿһ����������
	for (int i = 0; i < this->vbBufFile.FinalVB0Buf.size(); i=i+ fmtFile.Stride) {
		VertexPoint vertexPoint(this->fmtFile.d3d11GameType,this->fmtFile.ElementNameList, MMTFormat_GetRange_Byte(vbBufFile.FinalVB0Buf, i , i + fmtFile.Stride));
		//��ȡȨ������
		std::vector<std::byte> BlendIndicesBuf = vertexPoint.ElementName_ByteValueList_Map["BLENDINDICES"];
		std::vector<std::byte> BlendWeightsBuf = vertexPoint.ElementName_ByteValueList_Map["BLENDWEIGHTS"];
		
		//����Ҫ�����ȡ���������ĸ�partName������ˣ���ȡ���г��ֵ�partName
		std::set<int> findPartNameCountSet;

		for (int tmpIndex = 0; tmpIndex < 4; tmpIndex++) {
			std::byte blendIndicesByteValue = BlendIndicesBuf[tmpIndex];
			std::byte blendWeightsByteValue = BlendWeightsBuf[tmpIndex];

			uint8_t blendIndicesNumValue = static_cast<uint8_t>(blendIndicesByteValue);
			uint8_t blendWeightsNumValue = static_cast<uint8_t>(blendIndicesByteValue);
			if (PartNameCount_VertexGroupIndexSet_Map.size() != 0) {
				//������Ҫ����ж�
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

		//���û�ҵ�����˵���ǵ�һ����Ҫ�����⴦��
		if (findPartNameCountSet.size() == 0) {
			std::set<int> NewVertexGroupIndexList;
			for (std::byte byteValue : BlendIndicesBuf) {
				uint8_t num = static_cast<uint8_t>(byteValue);
				NewVertexGroupIndexList.insert(num);
			}
			PartNameCount_VertexGroupIndexSet_Map[0] = NewVertexGroupIndexList;
		}
		else if (findPartNameCountSet.size() == 1) {
			//�ĸ�����������һ�����ֳ����ڶ����������У���˵��ʣ�µĶ����������͵�ǰ�Ķ�����������������
			//�����Ϊֻ��һ��������֣��Լ��ڶ��������ֵ����
			//���ֻ������һ�����֣���ֱ������VertexGroupIndex����
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
			//����ڶ��������֣�����Ҫ�������ں�Ϊ1�����������µ�PartNameCount_VertexGroupIndexSet_Map���滻ԭ����
			LOG.Error("Show in multiple VertexGroup combination!");
		}


	}

	//����鿴ÿ�����������
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