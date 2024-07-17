#pragma once
#include <string>
#include <vector>
#include <map>


class DrawCall {
public:
	std::wstring DrwaIndex;
	std::vector<std::wstring> APICallStringList;
	std::vector<std::wstring> DumpCallStringList;

	DrawCall() {};
};


class FrameAnalysisLog {

public:
	std::wstring WorkFolder = L"";

	std::map<std::wstring, DrawCall> Index_DrawCall_Map;

	FrameAnalysisLog();
	
	FrameAnalysisLog(std::wstring WorkFolder);
};