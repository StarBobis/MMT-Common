#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <map>


class D3D11Element {
public:
    //�ɱ���
    std::string SemanticName = "";
    std::string SemanticIndex = "";
    std::string Format = "";
    int ByteWidth = 0;
    std::string ExtractSlot = "";
    std::string ExtractTechnique = "";
    std::string Category = "";

    //�̶���
    std::string InputSlot = "0";
    std::string InputSlotClass = "per-vertex";
    std::string InstanceDataStepRate = "0";

    //��Ҫ��̬����
    int ElementNumber = 0;
    int AlignedByteOffset = 0;
};


class D3D11GameType {
public:
    std::string GameType;

    //�Ƿ�ʹ��GPU���������̬�任
    bool GPUPreSkinning = false;

    //�Ƿ���Ҫ����blendweights��Ĭ�ϲ���Ҫ
    bool PatchBLENDWEIGHTS = false;

    //UE4����0
    bool UE4PatchNullInBlend = false;

    //ԭ���������ini�ļ����滻ʱд����draw��λ�����ÿһ�������ﶼ��Ҫ�ֶ�ָ��
    std::unordered_map <std::string, std::string> CategoryDrawCategoryMap;
    std::vector<std::string> OrderedFullElementList;

    //��Ҫ��������ó�������
    std::unordered_map<std::string, D3D11Element> ElementNameD3D11ElementMap;
    std::unordered_map <std::string, std::string> CategorySlotMap;
    std::unordered_map <std::string, std::string> CategoryTopologyMap;
    std::unordered_map<std::string, int> CategoryStrideMap;

    D3D11GameType();

    //�����ṩ��ElementList��ȡ�ܵ�Stride
    int getElementListStride(std::vector<std::string>);

    //��ȡCategory Stride Map
    std::unordered_map<std::string, int> getCategoryStrideMap(std::vector<std::string> inputElementList);

    //��ȡCategory List
    std::vector<std::string> getCategoryList(std::vector<std::string> inputElementList);

    //��ȡCategory��ElementList
    std::vector<std::string> getCategoryElementList(std::vector<std::string> inputElementList,std::string category);

    //����OrderedElementList�����������ElementList
    std::vector<std::string> getReorderedElementList(std::vector<std::string> elementList);
};