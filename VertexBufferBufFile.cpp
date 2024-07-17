#include "VertexBufferBufFile.h"
#include "MMTFileUtils.h"
#include "MMTStringUtils.h"
#include <fstream>
#include "MMTFormatUtils.h"

VertexBufferBufFile::VertexBufferBufFile() {

}


VertexBufferBufFile::VertexBufferBufFile(std::wstring readVBBufFilePath, D3D11GameType d3d11GameType, std::vector<std::string> elementList) {

    int SplitStride = d3d11GameType.getElementListStride(elementList);
    std::vector<std::string> CategoryList = d3d11GameType.getCategoryList(elementList);
    std::unordered_map<std::string, int> CategoryStrideMap = d3d11GameType.getCategoryStrideMap(elementList);

    uint64_t VBFileSize = MMTFile_GetFileSize(readVBBufFilePath);
    uint64_t vbFileVertexNumber = VBFileSize / SplitStride;

    //��ȡ����
    std::vector<std::byte> buffer(VBFileSize);
    std::ifstream VBFile(MMTString_ToByteString(readVBBufFilePath), std::ios::binary);
    VBFile.read(reinterpret_cast<char*>(buffer.data()), VBFileSize);
    VBFile.close();

    //ͨ�����ͣ���ȡ��ȡ���ȣ����ﻹ�������
    std::vector<int> readLengths;
    for (const std::string& category : CategoryList) {
        readLengths.push_back(CategoryStrideMap[category]);
    }

    int offset = 0;
    while (offset < buffer.size()) {
        for (size_t i = 0; i < CategoryList.size(); ++i) {
            const std::string& category = CategoryList[i];
            // ָ��Ҫ��ȡ���ֽڳ���
            int readLength = readLengths[i];
            std::vector<std::byte> categoryDataPatch;
            // Ԥ�����㹻���ڴ�ռ�
            categoryDataPatch.reserve(readLength);
            // ʹ��std::copy������ѭ�������Ч��
            std::copy(buffer.begin() + offset, buffer.begin() + offset + readLength, std::back_inserter(categoryDataPatch));

            // �ҵ�tmp�����
            std::unordered_map<std::wstring, std::vector<std::byte>>& tmpVBCategoryDataMap = this->CategoryVBDataMap;

            // �ҵ�tmp�������Ӧ��category������
            std::vector<std::byte>& categoryData = tmpVBCategoryDataMap[MMTString_ToWideString(category)];


            // ��categoryDataPatch������ֱ�Ӳ���categoryData
            categoryData.insert(categoryData.end(), categoryDataPatch.begin(), categoryDataPatch.end());

            offset += readLength;
        }

    }



};


void VertexBufferBufFile::SelfDivide(int MinNumber, int MaxNumber, int stride) {
    //����MinNumber������+1��ԭ���ǣ������0��ʼ�Ļ����ͻ��ȡ����0�Ĳ���
    //����MaxNumberҪ+1��ԭ���ǣ�GetRangeģ��python���[i:i+1] ���ǲ�����i+1��Ч���������Բ�����+1���Ǹ�
    //����+1���ܱ����ٶ�ȡһ��
    this->FinalVB0Buf = MMTFormat_GetRange_Byte(this->FinalVB0Buf, stride * (MinNumber), stride * (MaxNumber+1));
}


void VertexBufferBufFile::SaveToFile(std::wstring VB0OutputFilePath) {
    std::ofstream outputVBFile(VB0OutputFilePath, std::ofstream::binary);
    outputVBFile.write(reinterpret_cast<const char*>(this->FinalVB0Buf.data()), this->FinalVB0Buf.size());
    outputVBFile.close();
}

