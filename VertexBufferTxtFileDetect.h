#pragma once

#include "D3d11GameType.h"
#include <iostream>
#include <vector>
#include <unordered_map>


class VertexBufferDetect {
public:
    std::wstring Index;
    std::wstring Topology;
    //����ʶ�������ElementList��map
    std::unordered_map<std::string, D3D11Element> elementNameD3D11ElementMap;
    //�ڴ��ļ��������������ݵ�ElementList�������б�
    std::vector<std::string> realElementNameList;
    std::vector<std::string> showElementNameList;
    std::vector<D3D11Element> realElementList;
    std::vector<D3D11Element> showElementList;
    int fileShowStride = 0;
    int fileElementListShowStride = 0;
    int fileRealStride = 0;

    int vbSlotNumber = 0;

    //.txt�ļ���չʾ�Ķ�������
    int fileShowVertexCount = 0; 
    //.txt�ļ�����ʵ����������
    int fileRealVertexCount = 0;
    //��Ӧ.buf�ļ��е� �ļ���С/չʾ���� �õ��Ķ�������
    int fileBufferVertexCount = 0;
    //��Ӧ.buf�ļ��е�ȥ����β0����ʵ�ļ���С/չʾ���� �õ���ʵ��������
    uint64_t fileBufferRealVertexCount = 0;

    VertexBufferDetect();
    VertexBufferDetect(std::wstring);
};