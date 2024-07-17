#pragma once
#include <iostream>
#include <vector>

class FrameAnalysisData {
public:
	std::wstring WorkFolder = L"";
	
	std::vector<std::wstring> FrameAnalysisFileNameList;
	std::vector<std::wstring> TrianglelistIndexList;
	std::vector<std::wstring> PointlistIndexList;

	FrameAnalysisData();
	FrameAnalysisData(std::wstring WorkFolder);
	FrameAnalysisData(std::wstring WorkFolder,std::wstring DrawIB);

	void ReadFrameAnalysisFileList(std::wstring WorkFolder);

	std::wstring GetRealIB_IfNoModIB_FromLog(std::wstring DrawIB);
	std::vector<std::wstring> FindFrameAnalysisFileNameListWithCondition(std::wstring searchStr, std::wstring endStr);
	std::vector<std::wstring> ReadTrianglelistIndexList(std::wstring drawIB);
	std::vector<std::wstring> ReadPointlistIndexList();


};