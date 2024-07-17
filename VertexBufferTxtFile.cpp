#include "VertexBufferTxtFile.h"
#include "MMTFileUtils.h"
#include "MMTStringUtils.h"
#include "MMTLogUtils.h"

#include <sstream>

VertexDataLine::VertexDataLine() {

}


VertexDataLine::VertexDataLine(const std::wstring inputString) {
    // vb0[0]+000 POSITION: 5.04093075, 4.65782213, 1.05010247
    // vb0[115]+000 POSITION: -0.297964305, 0.0108079528, -0.00018915176
    // vb0[115] + 012 NORMAL: 0.51988852, 0.853630543, -0.0321041532
    // vb0[115] + 024 TANGENT : 0, 0.999893308, 0.0146092195, 1
    size_t slotEndPos = inputString.find(L"[");
    Slot = inputString.substr(0, slotEndPos);

    size_t indexStartPos = inputString.find(L"[") + 1;
    size_t indexEndPos = inputString.find(L"]");
    Index = inputString.substr(indexStartPos, indexEndPos - indexStartPos);

    size_t offsetStartPos = inputString.find(L"+");
    Offset = inputString.substr(offsetStartPos, 4);

    size_t elementNameStartPos = offsetStartPos + 5;
    size_t elementNameEndPos = inputString.find(L":");
    ElementName = inputString.substr(elementNameStartPos, elementNameEndPos - elementNameStartPos);

    size_t elementValueStartPos = inputString.find(L":") + 2;
    ElementValue = inputString.substr(elementValueStartPos);

    OriginalStrLine = inputString;
}


//���彫�˶���д����
void VertexBufferTxtFile::outputVB0File(std::wstring outputFileName) {

    //���ȣ�ÿ��VertexDataLine��Ҫ����outputElementList���ֱ�������¼������ǵ�ֵ������д��
    //����ֱ�Ӽ������д����
    std::ofstream file(outputFileName);

    if (file.is_open()) {
        file << "stride: " << MMTString_ToByteString(Stride) << std::endl;
        file << "first vertex: 0" << std::endl;
        file << "vertex count: " << MMTString_ToByteString(VertexCount) << std::endl;
        file << "topology: " << MMTString_ToByteString(Topology) << std::endl;
        //��������ʼ���element�б�
        int elementListIndex = 0;
        int AlignedByteOffset = 0;
        for (std::string elementName : realElementList) {
            file << "element[" << elementListIndex << "]:" << std::endl;
            D3D11Element elementObject = this->d3d11GameType.ElementNameD3D11ElementMap[elementName];

            file << "  SemanticName: " << elementObject.SemanticName << std::endl;
            file << "  SemanticIndex: " << elementObject.SemanticIndex << std::endl;
            file << "  Format: " + elementObject.Format << std::endl;
            file << "  InputSlot: " << elementObject.InputSlot << std::endl;
            file << "  AlignedByteOffset: " << AlignedByteOffset << std::endl;
            file << "  InputSlotClass: " << elementObject.InputSlotClass << std::endl;
            file << "  InstanceDataStepRate: " << elementObject.InstanceDataStepRate << std::endl;

            //��ֵ��֮��ֵҪ���ϴ�Element��ByteWidth
            AlignedByteOffset = AlignedByteOffset + elementObject.ByteWidth;
            //����ƫ��Ҫ+1
            elementListIndex = elementListIndex + 1;
        }

        file << std::endl;
        file << "vertex-data:" << std::endl;
        file << std::endl;

        //��ʼ���VertexData����

        
        for (const auto& pair : IndexVertexDataLineListMap) {
            int indexNumber = pair.first;

            //������Ϊ�˼���Force PointlistIndex������ȷ����������TrianglelistΪ׼
            if (indexNumber >= std::stoi(this->VertexCount)) {
                LOG.Info(L"IndexNumber is greater than vertex count ,force exit!");
                break;
            }
            const std::vector<VertexDataLine>& vertexDataLineList = pair.second;

            // ����vertexDataLineList,�ȸ���outputElementList�е�Ԫ��˳���������
            // �Ȱ�ÿ��VertexDataLine�ó����ŵ�map��
            std::map<std::wstring, VertexDataLine> elementNameVertexDataLineMap;
            for (const VertexDataLine& vertexDataLine : vertexDataLineList) {
                std::wstring elementName;
                if (boost::algorithm::starts_with(vertexDataLine.ElementName,"ATTRIBUTE")) {
                    //TODO
                    //elementName = d3d11ElementAttribute.ExtractSemanticNameD3D11ElementMap[vertexDataLine.ElementName].SemanticName;
                }
                else {
                    elementName = vertexDataLine.ElementName;
                }
                
                elementNameVertexDataLineMap[elementName] = vertexDataLine;
            }

            //����outputElementList
            int AlignedByteOffset = 0;
            for (std::string elementName : realElementList) {
                D3D11Element elementObject = this->d3d11GameType.ElementNameD3D11ElementMap[elementName];
                //�õ���Ӧ��VertexDataLine��ֱ�����
                VertexDataLine vertexDataLine = elementNameVertexDataLineMap[MMTString_ToWideString(elementName)];
                file << "vb0[" << MMTString_ToByteString(vertexDataLine.Index) << "]";
                //alignedOffset����Ϊ��λ��
                std::wstringstream offsetStrStream{};
                offsetStrStream << std::setfill(L'0') << std::setw(3) << AlignedByteOffset;
                file << "+" << MMTString_ToByteString(offsetStrStream.str()) << " " << elementName << ": " << MMTString_ToByteString(vertexDataLine.ElementValue) << std::endl;
                AlignedByteOffset = AlignedByteOffset + elementObject.ByteWidth;
            }
            file << std::endl;

        }

        // �ļ������󳬳�������ʱ���ļ����Զ��ر�
    }
    else {
        std::cout << "Unable to open the file." << std::endl;
    }

}


VertexBufferTxtFile::VertexBufferTxtFile() {};


VertexBufferTxtFile::VertexBufferTxtFile(const std::wstring& filename,D3D11GameType d3d11GameType,bool parseElement,bool parseData) {
    std::filesystem::path filePath(filename);
    this->FileName = filePath.filename().wstring();
    this->Index = this->FileName.substr(0, 6);
    this->Hash = this->FileName.substr(11,8);
    this->d3d11GameType = d3d11GameType;
    
    this->realElementList = parseRealElementList(filename,parseElement);
    
    if (parseData) {
        parseAllLineFromFile(filename);
    }
    LOG.Info(L"VertexBufferFileData parse over.");
}


//�����������ȡ���������õģ��Ҳ�ʹ��d3d11ElementAttribute���������ݣ������������κ�����
VertexBufferTxtFile::VertexBufferTxtFile(const std::wstring& filename) {
    std::filesystem::path filePath(filename);
    this->FileName = filePath.filename().wstring();
    this->Index = this->FileName.substr(0, 6);
    this->Hash = this->FileName.substr(11, 8);
    this->realElementList = parseRealElementList(filename,false);
}


std::vector<D3D11Element> VertexBufferTxtFile::parseShowElementList(const std::wstring& filename) {

    //���ݴ����VBFilePath����ȡ��ǰ��vb��
    std::wstring vbSlotNumber = MMTString_GetFileNameFromFilePath(filename).substr(9, 1);

    //����ÿ��Trianglelist��VB�ļ�
    std::wifstream file(filename);
    if (!file.is_open()) {
        LOG.Error(L"Can't open: " + filename);
    }
    else {
        //LOG.Info(L"Parsing Show ElementList For: " + filename);
    }

    //�����õ��ı���
    std::wstring line;
    std::wstring lineBefore = L"";

    int fileShowStride = 0;

    bool inElementLine = false;
    bool inVertexData = false;

    std::vector<D3D11Element> d3d11ElementList;
    D3D11Element d3d11Element;
    int elementNumber = 0;

    std::vector<VertexDataLine> firstVertexDataList;

    while (std::getline(file, line)) {
        boost::algorithm::to_lower(line);
        boost::algorithm::trim(line);

        if (boost::algorithm::starts_with(line, "element")) {
            inElementLine = true;
            d3d11Element.ElementNumber = elementNumber;
            elementNumber++;
            continue;
        }

        // 2.Ȼ��������е�D3d11Element���б�
        if (inElementLine && !inVertexData) {
            std::size_t colonPos = line.find(':');
            if (colonPos != std::wstring::npos) {
                std::wstring key = line.substr(0, colonPos);
                boost::algorithm::trim(key);

                std::wstring value = line.substr(colonPos + 1);
                boost::algorithm::trim(value);
                //LOG.LogOutput(L"Current key: " + key + L" value: " + value);
                if (boost::algorithm::ends_with(key, "semanticname")) {
                    d3d11Element.SemanticName = MMTString_ToByteString(value);
                }
                else if (boost::algorithm::ends_with(key, "semanticindex")) {
                    d3d11Element.SemanticIndex = MMTString_ToByteString(value);
                }
                else if (boost::algorithm::ends_with(key, "format")) {
                    d3d11Element.Format = MMTString_ToByteString(value);
                }
                else if (boost::algorithm::ends_with(key, "inputslot")) {
                    d3d11Element.InputSlot = MMTString_ToByteString(value);
                }
                else if (boost::algorithm::ends_with(key, "alignedbyteoffset")) {
                    d3d11Element.AlignedByteOffset = std::stoi(value);
                }
                else if (boost::algorithm::ends_with(key, "inputslotclass")) {
                    d3d11Element.InputSlotClass = MMTString_ToByteString(value);
                }
                else if (boost::algorithm::ends_with(key, "instancedatasteprate")) {
                    d3d11Element.InstanceDataStepRate = MMTString_ToByteString(value);
                    d3d11Element.ExtractSlot = "vb" + MMTString_ToByteString(vbSlotNumber);
                    //���������Ӧ�������һ��
                    //LOG.LogOutput(L"Read vbSlotNumber: " + d3d11Element.InputSlot);
                    if (d3d11Element.InputSlot == MMTString_ToByteString(vbSlotNumber)) {
                        //LOG.LogOutput(L"Add into d3d11ElementList: " + d3d11Element.SemanticName);
                        //���ﻹҪ����ExtractVB��ExtractTechnique
                        d3d11ElementList.push_back(d3d11Element);
                    }
                }
            }
        }

        // 3.����VertexDataLine����
        //����vertex-dataҪ�˳��İ�����Ȼ������
        //����ҲҪ����һ��VertexData,��Ȼ�Ļ���һ���������vertexdata����֣��Ǿʹ���ʶ���ˡ�
        if (boost::algorithm::starts_with(line, "vertex-data:")) {
            break;
        }
        lineBefore = line;
    }
    file.close();
    //LOG.NewLine();
    return d3d11ElementList;
}


std::vector<std::string> VertexBufferTxtFile::parseRealElementList(const std::wstring& filename,bool parseElement) {

    std::vector<std::string> tmpElementList;

    std::wstring line;
    std::wstring lineBefore = L"";
    bool startVertexData = false;
    std::vector<VertexDataLine> vertexDataLineList;

    std::wifstream file(filename);

    while (std::getline(file, line)) {
        if (!startVertexData) {
            std::wstring strideStr = L"stride: ";
            std::wstring vertexCountStr = L"vertex count: ";
            std::wstring topologyStr = L"topology: ";
            std::wstring vertexDataFlag = L"vertex-data:";

            if (boost::algorithm::starts_with(line, strideStr)) {
                std::wstring valueStr = line.substr(strideStr.length());
                boost::algorithm::trim(valueStr);
                this->Stride = valueStr;
            }
            else if (boost::algorithm::starts_with(line, vertexCountStr)) {
                std::wstring valueStr = line.substr(vertexCountStr.length());
                boost::algorithm::trim(valueStr);
                this->VertexCount = valueStr;
            }
            else if (boost::algorithm::starts_with(line, topologyStr)) {
                std::wstring valueStr = line.substr(topologyStr.length());
                boost::algorithm::trim(valueStr);
                this->Topology = valueStr;
            }
            else if (boost::algorithm::starts_with(line, vertexDataFlag)) {
                startVertexData = true;
                if (!parseElement) {
                    break;
                }
            }

        }
        else {
            if (line.empty() && !lineBefore.empty()) {

                if (!vertexDataLineList.empty()) {

                    std::unordered_map <std::wstring, int> elementValueNumberMap;
                    for (size_t i = 0; i < vertexDataLineList.size(); i++)
                    {
                        VertexDataLine vertexDataLine = vertexDataLineList[i];
                        int number = elementValueNumberMap[vertexDataLine.ElementValue];
                        if (elementValueNumberMap.count(vertexDataLine.ElementValue) == 0) {
                            elementValueNumberMap[vertexDataLine.ElementValue] = 1;
                        }
                        else {
                            elementValueNumberMap[vertexDataLine.ElementValue] = number + 1;
                        }
                    }

                    std::wstring texcoordOffset = L"";
                    std::wstring texcoord1Offset = L"";
                    for (size_t i = 0; i < vertexDataLineList.size(); i++)
                    {
                        VertexDataLine vertexDataLine = vertexDataLineList[i];
                        int number = elementValueNumberMap[vertexDataLine.ElementValue];
                        if (number == 1) {
                            tmpElementList.push_back(MMTString_ToByteString(vertexDataLine.ElementName));
                            //LOG.LogOutput("�����" + vertexDataLine.ElementName);
                        }
                        else if (number > 1 && vertexDataLine.Offset == L"+000" ) {
                            //�����Ƕ�����һ����������Ĵ���
                            //vb2[0]+000 BLENDWEIGHTS: 1, 0, 0, 0
                            //vb2[0] + 016 BLENDINDICES: 1, 0, 0, 0
                            //��ʱ��BLENDWEIGHTS��BLENDINDICES��ͬ���ᵼ���޷���ȡ��BLENDWEIGHTS
                            if ( vertexDataLine.ElementName == L"BLENDWEIGHTS") {
                                tmpElementList.push_back(MMTString_ToByteString(vertexDataLine.ElementName));

                            }

                            if (vertexDataLine.ElementName == L"BLENDWEIGHT") {
                                tmpElementList.push_back(MMTString_ToByteString(vertexDataLine.ElementName));

                            }
                            // ����Ǳ�����2.0�г��ֵ������������ֻ����Trianglelist��ʱ����ͨ����
                            // ����TEXCOORD1����Ч�ģ�TEXCOORD��Ҫ��ӡ�
                            //vb1[0]+000 TEXCOORD: -0.487339795, 0.0454249829
                            //vb1[0]+000 TEXCOORD1: -0.487339795, 0.0454249829
                            if (vertexDataLine.ElementName == L"TEXCOORD" && Topology == L"trianglelist") {
                                tmpElementList.push_back(MMTString_ToByteString(vertexDataLine.ElementName));
                            }



                        }
                        else if (number > 1 && vertexDataLine.Offset != L"+000") {
                           

                            if (vertexDataLine.ElementName == L"TEXCOORD") {
                                tmpElementList.push_back(MMTString_ToByteString(vertexDataLine.ElementName));
                                texcoordOffset = vertexDataLine.Offset;
                            }
                            else if (vertexDataLine.ElementName == L"TEXCOORD1" && vertexDataLine.Offset != texcoordOffset ) {
                                texcoord1Offset = vertexDataLine.Offset;
                                tmpElementList.push_back(MMTString_ToByteString(vertexDataLine.ElementName));
                            }
                            //��Ӿ�����ר����TEXCOORD2��TEXCOORD3֧��
                            else if (vertexDataLine.ElementName == L"TEXCOORD2" && vertexDataLine.Offset != texcoordOffset && vertexDataLine.Offset != texcoord1Offset) {
                                tmpElementList.push_back(MMTString_ToByteString(vertexDataLine.ElementName));
                            }
                            else if (vertexDataLine.ElementName == L"TEXCOORD3" && vertexDataLine.Offset != texcoordOffset && vertexDataLine.Offset != texcoord1Offset) {
                                tmpElementList.push_back(MMTString_ToByteString(vertexDataLine.ElementName));
                            }
                            else if (!boost::algorithm::starts_with(vertexDataLine.ElementName, L"TEXCOORD")) {
                                //LOG.LogOutput("�����" + vertexDataLine.ElementName);
                                tmpElementList.push_back(MMTString_ToByteString(vertexDataLine.ElementName));
                            }
                        }


                    }

                    break;
                }
                vertexDataLineList.clear();
            }
            else {
                std::string vb0Str = "vb";
                if (boost::algorithm::starts_with(line, vb0Str)) {
                    VertexDataLine dataLine(line);
                    vertexDataLineList.push_back(dataLine);
                }
            }
            lineBefore = line;
        }
        
    }
    file.close();

    // ԭ��4.4���½�Trianglelist�е�VertexCountȥ����
    // ���Զ�ȡ��֮�����û�еĻ���������Ҫ��������ȡÿһ�е�Index����vb0[123]���123������VertexCount
    // ��Ϊ�Ǳ�����������������������������ȷ��VertexCount
    if (this->VertexCount == L"") {
        LOG.Info(L"Can't find vertex count attribute in VB file, count the whole line manually.");
        std::wifstream anotherFile(filename);
        //�����ļ�ĩβ
        anotherFile.seekg(0, std::ios::end);
        //��ȡ�ļ���С
        std::streampos fileSize = anotherFile.tellg();
        //��ת����β-1000��λ��
        anotherFile.seekg(-1000, std::ios::end);

        std::wstring line2;
        while (std::getline(anotherFile, line2)) {
            if (boost::algorithm::starts_with(line2, "vb")) {
                VertexDataLine dataLine(line2);
                this->VertexCount = dataLine.Index;
            }
        }
        anotherFile.close();

        //Index�Ǵ�0��ʼ�ģ�����������Ҫ + 1
        this->VertexCount = std::to_wstring(std::stoi(this->VertexCount) + 1);
        LOG.Info(L"VertexCount: " + this->VertexCount);

    }
    
    return tmpElementList;
}


void VertexBufferTxtFile::parseAllLineFromFile(const std::wstring& filename) {
    this->FileName = filename;
    //���ȴ��ļ������ҽ��ļ���ȡ��һ��Vector�ﱸ��
    std::wifstream file(filename);
    std::vector<std::wstring> lines;
    std::wstring tmpline;
    while (std::getline(file, tmpline)) {
        lines.push_back(tmpline);
    }
    file.close();
    // add a black line so easier to process
    lines.push_back(L"");
    LOG.Info(L"Read file name:" + filename);
    LOG.Info(L"Read all line number: " + std::to_wstring(lines.size()));

    bool startVertexData = false;
    std::wstring lineBefore = L"";
    std::vector<VertexDataLine> vertexDataLineList;

    // process line by line
    for (std::wstring line : lines) {
        if (!startVertexData) {
            if (boost::algorithm::starts_with(line, L"vertex-data:")) {
                startVertexData = true;
            }
        }
        else {
            //�����ж������һ���ж���������һ�ж�ȡ���˿��У��ͻ����
            //��������ļ���βû�п��У��ͻ�©��һ��
            if (line.empty() && !lineBefore.empty()) {
                //����б�Ϊ�գ��ͼ��뱾����б���������
                if (!vertexDataLineList.empty()) {
                    int vertexIndex = std::stoi(vertexDataLineList[0].Index);
                    /*if (vertexIndex > 114) {
                        LOG.LogOutput(vertexDataLineList[0].ElementName);
                    }*/
                    IndexVertexDataLineListMap[vertexIndex] = vertexDataLineList;

                }
                vertexDataLineList.clear();
            }
            else {
                std::wstring vb0Str = L"vb";
                if (boost::algorithm::starts_with(line, vb0Str)) {
                    VertexDataLine dataLine(line);
                    if (boost::algorithm::any_of_equal(this->realElementList,MMTString_ToByteString(dataLine.ElementName))) {

                        vertexDataLineList.push_back(dataLine);
                    }
                }
            }
            lineBefore = line;
        }
    }

}

