#include <filesystem>
#include <fstream>
#include <boost/algorithm/string.hpp>

#include "FrameAnalysisData.h"
#include "VertexBufferTxtFileDetect.h"
#include "IndexBufferTxtFile.h"
#include "MMTFileUtils.h"
#include "MMTLogUtils.h"


void FrameAnalysisData::ReadFrameAnalysisFileList(std::wstring WorkFolder) {
    this->WorkFolder = WorkFolder;
    this->FrameAnalysisFileNameList.clear();
    for (const auto& entry : std::filesystem::directory_iterator(WorkFolder)) {
        if (!(entry.is_regular_file())) {
            continue;
        }
        this->FrameAnalysisFileNameList.push_back(entry.path().filename().wstring());
    }

    //如果找不到.buf文件则弹出警告
    if (this->FindFrameAnalysisFileNameListWithCondition(L"", L".buf").size() == 0) {
        LOG.Error("Can't find any .buf file, please set your analyse_option config in your d3dx.ini to \nanalyse_options = dump_rt dump_tex  dump_cb dump_vb dump_ib buf txt  ");
    }
}


FrameAnalysisData::FrameAnalysisData() {

}


FrameAnalysisData::FrameAnalysisData(std::wstring WorkFolder, std::wstring DrawIB) {
    this->ReadFrameAnalysisFileList(WorkFolder);
    this->TrianglelistIndexList = this->ReadTrianglelistIndexList(DrawIB);
    this->PointlistIndexList = this->ReadPointlistIndexList();
}


FrameAnalysisData::FrameAnalysisData(std::wstring WorkFolder) {
    this->ReadFrameAnalysisFileList(WorkFolder);
}


std::vector<std::wstring> FrameAnalysisData::FindFrameAnalysisFileNameListWithCondition(std::wstring searchStr, std::wstring endStr) {
    std::vector<std::wstring> findFileNameList;
    for (std::wstring fileName :this->FrameAnalysisFileNameList ) {
        if (fileName.find(searchStr) != std::string::npos && fileName.substr(fileName.length() - endStr.length()) == endStr) {
            findFileNameList.push_back(fileName);
        }
    }
    return findFileNameList;
}


std::vector<std::wstring> FrameAnalysisData::ReadTrianglelistIndexList(std::wstring drawIB) {
    //首先根据DrawIB，获取所有的Trianglelist Index
    std::vector<std::wstring> trianglelistIndexList;
    for (std::wstring fileName : this->FrameAnalysisFileNameList) {
        //只需要txt文件
        if (!boost::algorithm::ends_with(fileName, L".txt")) {
            continue;
        }
        //必须包含DrawIB
        if (fileName.find(L"-ib") != std::string::npos && fileName.find(drawIB) != std::string::npos) {
            IndexBufferTxtFile ibTxtFile(this->WorkFolder + fileName, false);
            if (ibTxtFile.Topology == L"trianglelist") {
                if (FindFrameAnalysisFileNameListWithCondition(ibTxtFile.Index + L"-vb0", L".txt").size() != 0) {
                    trianglelistIndexList.push_back(ibTxtFile.Index);
                }
            }
        }
    }
    return trianglelistIndexList;
}


std::vector<std::wstring> FrameAnalysisData::ReadPointlistIndexList() {
    std::vector<std::wstring> pointlistIndexList;
    for (std::wstring fileName: this->FrameAnalysisFileNameList) {
        //只需要txt文件
        if (!boost::algorithm::ends_with(fileName, L".txt")) {
            continue;
        }

        if (fileName.find(L"-vb0") != std::string::npos) {
            VertexBufferDetect vbDetect(this->WorkFolder + fileName);
            if (vbDetect.Topology == L"pointlist") {
                pointlistIndexList.push_back(vbDetect.Index);
            }
        }
    }
    return pointlistIndexList;
}


std::wstring FrameAnalysisData::GetRealIB_IfNoModIB_FromLog(std::wstring DrawIB) {
    std::vector<std::wstring> originalIBFileList = this->FindFrameAnalysisFileNameListWithCondition(DrawIB,L".txt");
    if (originalIBFileList.size() > 0) {
        return DrawIB;
    }
    //这里我们要通过解析log文件来找到这个IB的真实IB值
    std::wstring LogTxtFilePath = this->WorkFolder + L"log.txt";
    if (!std::filesystem::exists(LogTxtFilePath)) {
        LOG.Error(L"Can't find log.txt in your FrameAnalysisFolder: " + this->WorkFolder);
    }
    LOG.Info("Find log.txt in FrameAnalysisFolder");
    std::vector<std::wstring> LogLineList = MMTFile_ReadAllLinesW(LogTxtFilePath);

    //查找包含hash=DrawIB的那一行
    std::wstring Index = L"";
    for (std::wstring logLine : LogLineList) {
        if (logLine.find(L"hash=" + DrawIB) != std::wstring::npos) {
            Index = logLine.substr(0, 6);
            break;
        }
    }
    if (Index == L"") {
        LOG.Error(L"Can't find valid index for: " + DrawIB);
    }
    else {
        LOG.Info(L"Find Index:" + Index);
    }

    std::vector<std::wstring> ibHashFileList = this->FindFrameAnalysisFileNameListWithCondition(Index + L"-ib", L".txt");
    if (ibHashFileList.size() == 0) {
        LOG.Error(L"Can't find ib file for index: " + Index);
    }
    std::wstring ibFileName = ibHashFileList[0];
    LOG.Info(L"Find ibFileName: " + ibFileName);
    std::wstring NewIBHash = ibFileName.substr(10, 8);
    LOG.Info(L"Find NewIB Hash success: " + NewIBHash);
    LOG.NewLine();
    return NewIBHash;

}